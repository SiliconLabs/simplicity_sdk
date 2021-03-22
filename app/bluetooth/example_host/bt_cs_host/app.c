/***************************************************************************//**
 * @file
 * @brief CS NCP host
 *
 * Reference implementation of a CS host with initiator and reflector support.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "app.h"
#include "ncp_host.h"
#include "app_log.h"
#include "app_log_cli.h"
#include "app_assert.h"
#include "sl_bt_api.h"
#include "host_comm.h"
#include "gatt_db.h"
#include "rtl_log.h"
#include "ble_peer_manager_common.h"
#include "ble_peer_manager_connections.h"
#include "ble_peer_manager_central.h"
#include "ble_peer_manager_peripheral.h"
#include "ble_peer_manager_filter.h"
#include "cs_reflector.h"
#include "cs_reflector_config.h"
#include "cs_acp.h"
#include "cs_initiator_configurator.h"
#include "cs_antenna.h"
#include "cs_log.h"

#define MAX_INITIATOR_INSTANCES  2u
#define MAX_REFLECTOR_INSTANCES  2u

#define CREATE_REFLECTOR_MSG_LEN (sizeof(cs_acp_cmd_id_t) + sizeof(cs_acp_create_reflector_cmd_data_t))
#define REFLECTOR_ACTION_MSG_LEN (sizeof(cs_acp_cmd_id_t) + sizeof(cs_acp_reflector_action_cmd_data_t))

#define CREATE_INITATOR_MSG_LEN (sizeof(cs_acp_cmd_id_t) + sizeof(cs_acp_create_initiator_cmd_data_t))
#define INITATOR_ACTION_MSG_LEN (sizeof(cs_acp_cmd_id_t) + sizeof(cs_acp_initiator_action_cmd_data_t))

static uuid_128 ras_service_uuid = {
  .data = { 0xf3, 0x20, 0x18, 0xc7, 0x32, 0x2d, 0xc7, 0xab, 0xcf, 0x46, 0xf7, 0xff, 0x70, 0x9e, 0xb9, 0xbb }
};

// Optstring argument for getopt
#define OPTSTRING NCP_HOST_OPTSTRING APP_LOG_OPTSTRING HOST_COMM_CPC_OPTSTRING "hm:R:I:F:rwo:"

// Usage info
#define USAGE APP_LOG_NL "%s " NCP_HOST_USAGE APP_LOG_USAGE HOST_COMM_CPC_USAGE                                  \
  "\n[-m <cs_mode>] [-R <max_reflector_instances>] [-I <max_initiator_instances>] [-F <reflector_ble_address>] " \
  "[-r] [-w] [-o] [-h]" APP_LOG_NL

// Detailed argument list
#define CS_HOST_INITIATOR_OPTIONS                                           \
  "    -m  Number corresponding to the selected CS mode, default: 2, PBR\n" \
  "        <cs_mode>\n"                                                     \
  "        1 : RTT\n"                                                       \
  "        2 : PBR\n"                                                       \
  "    -R  Maximum number of reflector instances, default: 1\n"             \
  "        <max_reflector_instances>\n"                                     \
  "    -I  Maximum number of initiator instances, default: 1\n"             \
  "        <max_initiator_instances>\n"                                     \
  "    -F  Enable reflector BLE address filtering\n"                        \
  "        <reflector_ble_address>\n"                                       \
  "    -r  Enable RSSI distance measurement\n"                              \
  "    -w  Use wired antenna offset\n"                                      \
  "    -o  Object tracking mode, default: 1\n"                              \
  "        1 : moving object tracking (up to 5 km/h) (REAL_TIME_BASIC)\n"   \
  "        2 : stationary object tracking (STATIC_HIGH_ACCURACY)\n"

// Options info
#define OPTIONS             \
  "\nOPTIONS\n"             \
  NCP_HOST_OPTIONS          \
  APP_LOG_OPTIONS           \
  CS_HOST_INITIATOR_OPTIONS \
  HOST_COMM_CPC_OPTIONS     \
  "    -h  Print this help message.\n"

#define BT_ADDR_LEN 6u

typedef struct {
  sl_bt_cs_mode_t cs_mode;
  bool rssi_measurement_enabled;
  bool use_antenna_wired_offset;
  bool use_moving_object_tracking;
  bd_addr accepted_bt_address_list[MAX_REFLECTOR_INSTANCES];
  uint32_t accepted_bt_address_count;
  uint32_t max_reflector_instances;
  uint32_t max_initiator_instances;
} cs_host_config_t;

typedef struct {
  uint32_t num_reflector_connections;
  uint32_t num_initiator_connections;
  uint8_t reflector_conn_handles[MAX_REFLECTOR_INSTANCES];
  uint8_t initiator_conn_handles[MAX_INITIATOR_INSTANCES];
} cs_host_state_t;

cs_host_config_t cs_host_config = {
  .cs_mode = sl_bt_cs_mode_pbr,
  .rssi_measurement_enabled = false,
  .use_antenna_wired_offset = false,
  .use_moving_object_tracking = true,
  .accepted_bt_address_count = 0u,
  .max_reflector_instances = 0u,
  .max_initiator_instances = 0u
};

cs_host_state_t cs_host_state = {
  .num_reflector_connections = 0u,
  .num_initiator_connections = 0u,
  .reflector_conn_handles = { SL_BT_INVALID_CONNECTION_HANDLE },
  .initiator_conn_handles = { SL_BT_INVALID_CONNECTION_HANDLE }
};

void cs_on_result(cs_result_t *result, void *user_data);
void handle_connection_opened_with_initiator(uint8_t conn_handle);
void handle_connection_opened_with_reflector(uint8_t conn_handle);
void handle_connection_closed(uint8_t conn_handle);
void stop_procedure(void);

/**************************************************************************//**
 * Application initialization
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  sl_status_t sc;
  int cli_opt;
  int cs_mode = sl_bt_cs_mode_pbr;
  bool max_initiator_instances_set = false;
  bool max_reflector_instances_set = false;

  app_log_info("Silicon Labs CS host" APP_LOG_NL);
  app_log_info("---------------------------" APP_LOG_NL);

  // Initialize the list of accepted BT addresses
  for (uint32_t i = 0u; i < MAX_REFLECTOR_INSTANCES; i++) {
    memset(cs_host_config.accepted_bt_address_list[i].addr, 0xFF, BT_ADDR_LEN);
  }

  // Process command line options
  while ((cli_opt = getopt(argc, argv, OPTSTRING)) != -1) {
    switch (cli_opt) {
      // Print help
      case 'h':
        app_log(USAGE, argv[0]);
        app_log(OPTIONS);
        exit(EXIT_SUCCESS);

      case 'm':
        // Mode - (1 - RTT) (2 - PBR)
        cs_mode = atoi(optarg);
        break;

      case 'R':
        cs_host_config.max_reflector_instances = atoi(optarg);
        max_reflector_instances_set = true;
        if (cs_host_config.max_reflector_instances > MAX_REFLECTOR_INSTANCES) {
          app_log_error("Invalid number of 'maximum reflectors' argument (%d) provided, "
                        "must be in the range of 0 to %u" APP_LOG_NL,
                        cs_host_config.max_reflector_instances,
                        MAX_REFLECTOR_INSTANCES);
          exit(EXIT_FAILURE);
        }
        break;

      case 'I':
        cs_host_config.max_initiator_instances = atoi(optarg);
        max_initiator_instances_set = true;
        if (cs_host_config.max_initiator_instances > MAX_INITIATOR_INSTANCES) {
          app_log_error("Invalid number of 'maximum initiators' argument (%d) provided, "
                        "must be in the range of 0 to %u" APP_LOG_NL,
                        cs_host_config.max_initiator_instances,
                        MAX_INITIATOR_INSTANCES);
          exit(EXIT_FAILURE);
        }
        break;

      case 'F':
      {
        // Set up reflector BLE address filtering
        unsigned int accepted_ble_address[BT_ADDR_LEN];
        // Try parsing without a ':' separator
        int accepted_ble_address_len = sscanf(optarg, "%2X%2X%2X%2X%2X%2X",
                                              &accepted_ble_address[5],
                                              &accepted_ble_address[4],
                                              &accepted_ble_address[3],
                                              &accepted_ble_address[2],
                                              &accepted_ble_address[1],
                                              &accepted_ble_address[0]);

        // If parsing without a ':' failed - try parsing with a ':' separator
        if (accepted_ble_address_len != BT_ADDR_LEN) {
          accepted_ble_address_len = sscanf(optarg, "%2X:%2X:%2X:%2X:%2X:%2X",
                                            &accepted_ble_address[5],
                                            &accepted_ble_address[4],
                                            &accepted_ble_address[3],
                                            &accepted_ble_address[2],
                                            &accepted_ble_address[1],
                                            &accepted_ble_address[0]);
          // If none of the parsing methods worked - exit with failure
          if (accepted_ble_address_len != BT_ADDR_LEN) {
            app_log_error("Invalid BLE address filter provided" APP_LOG_NL);
            exit(EXIT_FAILURE);
          }
        }
        // Check if we have room for more accepted addresses
        if (cs_host_config.accepted_bt_address_count >= MAX_REFLECTOR_INSTANCES) {
          app_log_error("Maximum number of accepted BLE addresses (%u) reached, "
                        "ignoring additional addresses" APP_LOG_NL, MAX_REFLECTOR_INSTANCES);
          break;
        }
        // Add the accepted BLE address to the list
        bd_addr *bt_addr_to_add = &cs_host_config.accepted_bt_address_list[cs_host_config.accepted_bt_address_count];
        for (uint8_t i = 0; i < BT_ADDR_LEN; i++) {
          bt_addr_to_add->addr[i] = (uint8_t)accepted_ble_address[i];
        }
        app_log_info("BLE address accept filter added for: '%02x:%02x:%02x:%02x:%02x:%02x'" APP_LOG_NL,
                     bt_addr_to_add->addr[5],
                     bt_addr_to_add->addr[4],
                     bt_addr_to_add->addr[3],
                     bt_addr_to_add->addr[2],
                     bt_addr_to_add->addr[1],
                     bt_addr_to_add->addr[0]);
        cs_host_config.accepted_bt_address_count++;
      }
      break;

      case 'r':
        cs_host_config.rssi_measurement_enabled = true;
        break;
      case 'w':
        cs_host_config.use_antenna_wired_offset = true;
        break;
      case 'o':
      {
        int object_tracking_mode = atoi(optarg);
        if (object_tracking_mode == 1) {
          cs_host_config.use_moving_object_tracking = true;
        } else if (object_tracking_mode == 2) {
          cs_host_config.use_moving_object_tracking = false;
        } else {
          app_log_error("Invalid object tracking mode (%d) provided!" APP_LOG_NL, object_tracking_mode);
          exit(EXIT_FAILURE);
        }
      }
      break;
      default:
        sc = ncp_host_set_option((char)cli_opt, optarg);
        if (sc == SL_STATUS_NOT_FOUND) {
          sc = app_log_set_option((char)cli_opt, optarg);
        }
        if (sc != SL_STATUS_OK) {
          app_log(USAGE, argv[0]);
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // Sanity check of argument combinations
  if (cs_host_config.max_initiator_instances == 0 && cs_host_config.max_reflector_instances == 0) {
    if (!max_initiator_instances_set && !max_reflector_instances_set) {
      cs_host_config.max_initiator_instances = 1;
      cs_host_config.max_reflector_instances = 1;
      app_log_info("Not specified <max_initiator_instances> and "
                   "<max_reflector_instances>. Using 1-1 of each" APP_LOG_NL);
    } else {
      app_log_error("<max_initiator_instances> and <max_reflector_instances>"
                    " cannot be both zero" APP_LOG_NL);
      exit(EXIT_FAILURE);
    }
  }

  if (cs_host_config.max_initiator_instances > 0) {
    if (cs_mode == (int)sl_bt_cs_mode_rtt) {
      cs_host_config.cs_mode = sl_bt_cs_mode_rtt;
      app_log_info("CS mode: 1 - RTT" APP_LOG_NL);
    } else if (cs_mode == (int)sl_bt_cs_mode_pbr) {
      cs_host_config.cs_mode = sl_bt_cs_mode_pbr;
      app_log_info("CS mode: 2 - PBR" APP_LOG_NL);
    } else {
      app_log_error("Invalid CS mode argument (%d) provided" APP_LOG_NL, cs_mode);
      exit(EXIT_FAILURE);
    }
  }

  if (cs_host_config.accepted_bt_address_count > 0) {
    app_log_info("BLE address filtering enabled with %u specified BT addresses" APP_LOG_NL,
                 cs_host_config.accepted_bt_address_count);
  } else {
    app_log_info("BLE address filtering disabled" APP_LOG_NL);
  }

  app_log_info("Maximum number of reflector instances: %u" APP_LOG_NL, cs_host_config.max_reflector_instances);
  app_log_info("Maximum number of initiator instances: %u" APP_LOG_NL, cs_host_config.max_initiator_instances);

  if (cs_host_config.use_moving_object_tracking) {
    app_log_info("Tracking mode: moving objects" APP_LOG_NL);
  } else {
    app_log_info("Tracking mode: stationary objects" APP_LOG_NL);
  }

  // Initialize the list of reflector connection handles
  for (uint32_t i = 0u; i < cs_host_config.max_initiator_instances; i++) {
    cs_host_state.reflector_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;
  }
  // Initialize the list of initiator connection handles
  for (uint32_t i = 0u; i < cs_host_config.max_reflector_instances; i++) {
    cs_host_state.initiator_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;
  }

  // Initialize the NCP connection
  sc = ncp_host_init();
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  app_assert_status(sc);
  app_log_info("NCP host initialized" APP_LOG_NL);
  app_log_info("Press Crtl+C to quit" APP_LOG_NL APP_LOG_NL);

  cs_log_create(argc, argv);
  rtl_log_init();

  ble_peer_manager_central_init();
  ble_peer_manager_peripheral_init();
  ble_peer_manager_filter_init();
}

void sl_bt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    // --------------------------------
    case sl_bt_evt_system_boot_id:
    {
      sl_status_t sc = cs_antenna_configure(cs_host_config.use_antenna_wired_offset);
      app_assert_status(sc);
      break;
    }

    // --------------------------------
    case sl_bt_evt_user_cs_service_message_to_host_id:
    {
      cs_acp_event_t *cs_evt = (cs_acp_event_t *)evt->data.evt_user_cs_service_message_to_host.message.data;
      switch (cs_evt->acp_evt_id) {
        case CS_ACP_EVT_LOG_DATA_ID:
          switch (cs_evt->data.log.src) {
            case LOG_SRC_RTL:
              rtl_log_on_event(&cs_evt->data.log);
              break;
            case LOG_SRC_INITIATOR:
              app_log("[INITIATOR] %s" APP_LOG_NL, cs_evt->data.log.log_data.data);
              break;
            case LOG_SRC_REFLECTOR:
              app_log("[REFLECTOR] %s" APP_LOG_NL, cs_evt->data.log.log_data.data);
              break;
            default:
              app_log_warning("Unknown log source: %u" APP_LOG_NL, cs_evt->data.log.src);
              break;
          }
          break;
        case CS_ACP_EVT_RESULT_ID:
        {
          cs_result_t cs_result = {
            .connection = cs_evt->connection_id,
            .cs_bit_error_rate = cs_evt->data.result.bit_error_rate,
            .distance = cs_evt->data.result.estimated_distance_mm,
            .likeliness = cs_evt->data.result.likeliness,
            .rssi_distance = cs_evt->data.result.rssi_distance_mm
          };
          cs_on_result(&cs_result, NULL);
          break;
        }
        case CS_ACP_EVT_STATUS_ID:
          app_log_error("[NCP] Error code %u" APP_LOG_NL, cs_evt->data.stat.error);
          app_log_status_f(cs_evt->data.stat.sc, "Status" APP_LOG_NL);
          break;
        default:
          app_log("Unknown ACP event" APP_LOG_NL);
          break;
      }
      break;
    }
  }
}

/**************************************************************************//**
 * Application process action
 *****************************************************************************/
void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application deinit
 *****************************************************************************/
void app_deinit(void)
{
  rtl_log_deinit();
  stop_procedure();
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application deinit code here!                       //
  // This is called once during termination.                                 //
  /////////////////////////////////////////////////////////////////////////////
}

void handle_connection_opened_with_reflector(uint8_t conn_handle)
{
  sl_status_t sc;

  // Check if we can accept one more reflector connection
  if (cs_host_state.num_reflector_connections >= cs_host_config.max_initiator_instances) {
    app_log_error("Maximum number of initiator instances (%u) reached, "
                  "dropping connection..." APP_LOG_NL,
                  cs_host_config.max_initiator_instances);
    sc = sl_bt_connection_close(conn_handle);
    app_assert_status(sc);
    return;
  }

  // Store the new reflector connection handle
  bool could_store = false;
  for (uint32_t i = 0u; i < cs_host_config.max_initiator_instances; i++) {
    if (cs_host_state.reflector_conn_handles[i] == SL_BT_INVALID_CONNECTION_HANDLE) {
      cs_host_state.reflector_conn_handles[i] = conn_handle;
      cs_host_state.num_reflector_connections++;
      could_store = true;
      break;
    }
  }
  if (!could_store) {
    app_log_error("Failed to store reflector connection handle" APP_LOG_NL);
    return;
  }

  cs_acp_create_initiator_cmd_data_t initiator_cmd_data;

  initiator_cmd_data.connection_id = conn_handle;
  cs_initiator_set_default_config(&initiator_cmd_data.initiator_config, &initiator_cmd_data.rtl_config);
  initiator_cmd_data.initiator_config.use_moving_object_tracking = cs_host_config.use_moving_object_tracking;

  cs_acp_cmd_t acp_cmd;
  // Construct an ACP command
  acp_cmd.cmd_id = CS_ACP_CMD_CREATE_INITIATOR;
  acp_cmd.data.initiator_cmd_data = initiator_cmd_data;

  // Start the Initiator
  sc = sl_bt_user_cs_service_message_to_target(CREATE_INITATOR_MSG_LEN,
                                               (uint8_t *)&acp_cmd,
                                               0,
                                               NULL,
                                               NULL);

  if (sc != SL_STATUS_OK) {
    app_log_status_f(sc, "Failed to create initiator instance" APP_LOG_NL);
    return;
  }

  app_log_info("New initiator instance created; connection_handle='%u'" APP_LOG_NL, conn_handle);

  app_log_info("Initiator instances in use: %u/%u" APP_LOG_NL,
               cs_host_state.num_reflector_connections,
               cs_host_config.max_initiator_instances);

  // Scan for new reflector connections if we have room for more
  if (cs_host_state.num_reflector_connections < cs_host_config.max_initiator_instances) {
    sl_status_t sc = ble_peer_manager_central_create_connection();
    app_assert_status(sc);
    app_log_info("Scanning restarted for new reflector connections..." APP_LOG_NL);
  }
}

void handle_connection_opened_with_initiator(uint8_t conn_handle)
{
  // Check if we can accept one more initiator connection
  if (cs_host_state.num_initiator_connections >= cs_host_config.max_reflector_instances) {
    app_log_error("Maximum number of reflector instances (%u) reached, "
                  "dropping connection..." APP_LOG_NL,
                  cs_host_config.max_reflector_instances);
    sl_status_t sc = sl_bt_connection_close(conn_handle);
    app_assert_status(sc);
    return;
  }

  // Store the new initiator connection handle
  for (uint32_t i = 0u; i < cs_host_config.max_reflector_instances; i++) {
    if (cs_host_state.initiator_conn_handles[i] == SL_BT_INVALID_CONNECTION_HANDLE) {
      cs_host_state.initiator_conn_handles[i] = conn_handle;
      cs_host_state.num_initiator_connections++;
      break;
    }
  }
  // Call the ACP API to create a new reflector instance for the connection handle
  sl_status_t sc;
  cs_acp_cmd_t msg;

  msg.cmd_id = CS_ACP_CMD_CREATE_REFLECTOR;
  msg.data.reflector_cmd_data.connection_id = conn_handle;
  msg.data.reflector_cmd_data.reflector_config.max_tx_power_dbm = CS_REFLECTOR_MAX_TX_POWER_DBM;
  sc = sl_bt_user_cs_service_message_to_target(CREATE_REFLECTOR_MSG_LEN,
                                               (uint8_t *)&msg,
                                               0,
                                               NULL,
                                               NULL);
  app_assert_status(sc);
  app_log_info("New reflector instance created; connection_handle='%u'" APP_LOG_NL, conn_handle);

  app_log_info("Reflector instances in use: %u/%u" APP_LOG_NL,
               cs_host_state.num_initiator_connections,
               cs_host_config.max_reflector_instances);

  // Advertise for new initiator connections if we have room for more
  if (cs_host_state.num_initiator_connections < cs_host_config.max_reflector_instances) {
    sl_status_t sc = ble_peer_manager_peripheral_start_advertising(SL_BT_INVALID_ADVERTISING_SET_HANDLE);
    app_assert_status(sc);
    app_log_info("Advertising restarted for new initiator connections..." APP_LOG_NL);
  }
}

void handle_connection_closed(uint8_t conn_handle)
{
  // Check if the connection handle is a reflector connection - if yes remove the corresponding initiator instance
  for (uint32_t i = 0u; i < cs_host_config.max_initiator_instances; i++) {
    if (cs_host_state.reflector_conn_handles[i] == conn_handle) {
      cs_host_state.reflector_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;
      app_log_info("Reflector connection closed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      // Call the ACP API to remove the initiator instance for the connection handle
      cs_acp_cmd_t acp_cmd;
      sl_status_t sc;

      acp_cmd.cmd_id = CS_ACP_CMD_INITIATOR_ACTION;
      acp_cmd.data.initiator_action_data.connection_id = conn_handle;
      acp_cmd.data.initiator_action_data.initiator_action = CS_ACP_ACTION_DELETE_INITIATOR;
      sc = sl_bt_user_cs_service_message_to_target(INITATOR_ACTION_MSG_LEN,
                                                   (uint8_t *)&acp_cmd,
                                                   0,
                                                   NULL,
                                                   NULL);
      app_assert_status(sc);
      app_log_info("Initiator instance removed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      // Restart scanning for new reflector connections
      (void)ble_peer_manager_central_create_connection();
      app_log_info("Scanning restarted for new reflector connections..." APP_LOG_NL);

      cs_host_state.num_reflector_connections--;
      app_log_info("Initiator instances in use: %u/%u" APP_LOG_NL,
                   cs_host_state.num_reflector_connections,
                   cs_host_config.max_initiator_instances);
      break;
    }
  }

  // Check if the connection handle is an initiator connection - if yes remove the corresponding reflector instance
  for (uint32_t i = 0u; i < cs_host_config.max_reflector_instances; i++) {
    if (cs_host_state.initiator_conn_handles[i] == conn_handle) {
      cs_host_state.initiator_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;
      app_log_info("Initiator connection closed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      // Call the ACP API to remove the reflector instance for the connection handle
      sl_status_t sc;
      cs_acp_cmd_t msg;

      msg.cmd_id = CS_ACP_CMD_REFLECTOR_ACTION;
      msg.data.reflector_action_data.connection_id = conn_handle;
      msg.data.reflector_action_data.reflector_action = CS_ACP_ACTION_DELETE_REFLECTOR;
      sc = sl_bt_user_cs_service_message_to_target(REFLECTOR_ACTION_MSG_LEN,
                                                   (uint8_t *)&msg,
                                                   0,
                                                   NULL,
                                                   NULL);
      app_assert_status(sc);
      app_log_info("Reflector instance removed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      // Restart advertising for new initiator connections if we were at the limit
      if (cs_host_state.num_initiator_connections == cs_host_config.max_reflector_instances) {
        sl_status_t sc = ble_peer_manager_peripheral_start_advertising(SL_BT_INVALID_ADVERTISING_SET_HANDLE);
        app_assert_status(sc);
        app_log_info("Advertising restarted for new initiator connections..." APP_LOG_NL);
      }

      cs_host_state.num_initiator_connections--;
      app_log_info("Reflector instances in use: %u/%u" APP_LOG_NL,
                   cs_host_state.num_initiator_connections,
                   cs_host_config.max_reflector_instances);
      break;
    }
  }
}

void ble_peer_manager_on_event(ble_peer_manager_evt_type_t *event)
{
  sl_status_t sc;
  const char* device_name = INITIATOR_DEVICE_NAME;

  switch (event->evt_id) {
    case BLE_PEER_MANAGER_ON_BOOT:
    {
      // Print the Bluetooth address
      bd_addr address;
      uint8_t address_type;
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);
      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   address_type ? "static random" : "public device",
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);

      // Filter for advertised name (CS_RFLCT)
      sc = ble_peer_manager_set_filter_device_name(device_name, strlen(device_name), false);
      app_assert_status(sc);

      // Filter for advertised service UUID (RAS)
      sc = ble_peer_manager_set_filter_service_uuid128(&ras_service_uuid);
      app_assert_status(sc);

      // Filter for BLE address if enabled
      if (cs_host_config.accepted_bt_address_count > 0) {
        // Enable Peer Manager BT address filtering
        ble_peer_manager_set_filter_bt_address(true);
        // Add all user specified BT addresses to the allowed list
        for (uint32_t i = 0u; i < cs_host_config.accepted_bt_address_count; i++) {
          sc = ble_peer_manager_add_allowed_bt_address(&cs_host_config.accepted_bt_address_list[i]);
          app_assert_status(sc);
        }
      }

      // Start scanning for reflector connections
      if (cs_host_config.max_initiator_instances > 0 ) {
        sc = ble_peer_manager_central_create_connection();
        app_assert_status(sc);
        app_log_info("Scanning started for reflector connections..." APP_LOG_NL);
      }

      // Start advertising for initiator connections
      if (cs_host_config.max_reflector_instances > 0 ) {
        sc = ble_peer_manager_peripheral_start_advertising(SL_BT_INVALID_ADVERTISING_SET_HANDLE);
        app_assert_status(sc);
        app_log_info("Advertising started for initiator connections..." APP_LOG_NL);
      }
    }
    break;

    case BLE_PEER_MANAGER_ON_CONN_OPENED_CENTRAL:
      app_log_info("[%u] Connection opened as central with a CS reflector" APP_LOG_NL, event->connection_id);
      handle_connection_opened_with_reflector(event->connection_id);
      break;

    case BLE_PEER_MANAGER_ON_CONN_OPENED_PERIPHERAL:
      app_log_info("[%u] Connection opened as peripheral with a CS initiator" APP_LOG_NL, event->connection_id);
      handle_connection_opened_with_initiator(event->connection_id);
      break;

    case BLE_PEER_MANAGER_ON_CONN_CLOSED:
      app_log_info("[%u] Connection closed" APP_LOG_NL, event->connection_id);
      handle_connection_closed(event->connection_id);
      break;

    case BLE_PEER_MANAGER_ON_ADV_STOPPED:
      app_log_info("[%u] Advertisement stopped" APP_LOG_NL, event->connection_id);
      break;

    case BLE_PEER_MANAGER_ERROR:
      app_log_info("[%u] Error" APP_LOG_NL, event->connection_id);
      break;

    default:
      app_log_info("[%u] Unhandled Peer Manager event (%u)" APP_LOG_NL, event->connection_id, event->evt_id);
      break;
  }
}

void cs_on_result(cs_result_t *result, void *user_data)
{
  (void)user_data;

  app_log_info("---" APP_LOG_NL);
  app_log_info("Measurement result: %u mm" APP_LOG_NL, (uint32_t)(result->distance * 1000.f));
  app_log_info("Measurement likeliness: %f" APP_LOG_NL, result->likeliness);

  if (cs_host_config.rssi_measurement_enabled) {
    app_log_info("RSSI distance: %u mm" APP_LOG_NL, (uint32_t)(result->rssi_distance * 1000.f));
  }
  app_log_info("CS bit error rate: %u" APP_LOG_NL, result->cs_bit_error_rate);
}

void stop_procedure(void)
{
  cs_acp_cmd_t acp_cmd;
  sl_status_t sc;
  uint8_t conn_handle;

  // Close all reflector connections, and delete initiator instances on the NCP
  for (uint32_t i = 0u; i < cs_host_config.max_initiator_instances; i++) {
    conn_handle = cs_host_state.reflector_conn_handles[i];
    if (conn_handle != SL_BT_INVALID_CONNECTION_HANDLE) {
      cs_host_state.reflector_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;

      // Call the ACP API to remove the initiator instance for the connection handle

      acp_cmd.cmd_id = CS_ACP_CMD_INITIATOR_ACTION;
      acp_cmd.data.initiator_action_data.connection_id = conn_handle;
      acp_cmd.data.initiator_action_data.initiator_action = CS_ACP_ACTION_DELETE_INITIATOR;
      sc = sl_bt_user_cs_service_message_to_target(INITATOR_ACTION_MSG_LEN,
                                                   (uint8_t *)&acp_cmd,
                                                   0,
                                                   NULL,
                                                   NULL);
      app_assert_status(sc);
      app_log_info("Initiator instance removed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      sc = ble_peer_manager_central_close_connection(conn_handle);
      app_assert_status(sc);
      app_log_info("Reflector connection closed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      cs_host_state.num_reflector_connections--;
      app_log_info("Initiator instances in use: %u/%u" APP_LOG_NL,
                   cs_host_state.num_reflector_connections,
                   cs_host_config.max_initiator_instances);
    }
  }

  // Close all initiator connections, and delete reflector instances on the NCP
  for (uint32_t i = 0u; i < cs_host_config.max_reflector_instances; i++) {
    conn_handle = cs_host_state.initiator_conn_handles[i];
    if (conn_handle != SL_BT_INVALID_CONNECTION_HANDLE) {
      cs_host_state.initiator_conn_handles[i] = SL_BT_INVALID_CONNECTION_HANDLE;

      // Call the ACP API to remove the initiator instance for the connection handle
      acp_cmd.cmd_id = CS_ACP_CMD_REFLECTOR_ACTION;
      acp_cmd.data.reflector_action_data.connection_id = conn_handle;
      acp_cmd.data.reflector_action_data.reflector_action = CS_ACP_ACTION_DELETE_REFLECTOR;
      sc = sl_bt_user_cs_service_message_to_target(REFLECTOR_ACTION_MSG_LEN,
                                                   (uint8_t *)&acp_cmd,
                                                   0,
                                                   NULL,
                                                   NULL);
      app_assert_status(sc);
      app_log_info("Reflector instance removed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      sc = ble_peer_manager_peripheral_close_connection(conn_handle);
      app_assert_status(sc);
      app_log_info("Initiator connection closed; connection_handle='%u'" APP_LOG_NL, conn_handle);

      cs_host_state.num_initiator_connections--;
      app_log_info("Reflector instances in use: %u/%u" APP_LOG_NL,
                   cs_host_state.num_initiator_connections,
                   cs_host_config.max_reflector_instances);
    }
  }
}
