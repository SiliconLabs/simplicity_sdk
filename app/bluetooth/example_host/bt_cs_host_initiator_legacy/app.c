/***************************************************************************//**
 * @file
 * @brief CS legacy initiator Example Project.
 *
 * Reference implementation of a CS initiator host, which is
 * typically run on a host computer without radio. It can connect to a CS enabled NCP via
 * VCOM to access the Bluetooth stack of the NCP and to control it using BGAPI.
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
#include <time.h>
#include "app.h"
#include "ncp_host.h"
#include "app_log.h"
#include "app_log_cli.h"
#include "app_assert.h"
#include "sl_bt_api.h"
#include "cs_initiator.h"
#include "cs_initiator_configurator.h"
#include "cs_antenna.h"
#include "host_comm.h"
#include "ble_peer_manager_common.h"
#include "ble_peer_manager_connections.h"
#include "ble_peer_manager_central.h"
#include "ble_peer_manager_filter.h"
#include "rtl_log.h"
#include "cs_log.h"

// Optstring argument for getopt
#define OPTSTRING      NCP_HOST_OPTSTRING APP_LOG_OPTSTRING HOST_COMM_CPC_OPTSTRING "hm:F:rwo:"

// Usage info
#define USAGE          APP_LOG_NL "%s " NCP_HOST_USAGE APP_LOG_USAGE HOST_COMM_CPC_USAGE \
  "\n[-m <mode>] [-F <Reflector_BLE_address>] [-r] [-w] [-o] [-h]" APP_LOG_NL

// CS Host Initiator options
#define CS_HOST_INITIATOR_OPTIONS                                             \
  "    -m  CS mode.\n"                                                        \
  "        <mode>           Integer representing CS mode, default: 2, PBR.\n" \
  "        1 : RTT\n"                                                         \
  "        2 : PBR\n"                                                         \
  "    -F  Reflector filter address.\n"                                       \
  "        <reflector_address>\n"                                             \
  "    -r  Enable RSSI distance measurement\n"                                \
  "    -w  Use wired antenna offset\n"                                        \
  "    -o  Object tracking mode, default: 1\n"                                \
  "        1 : moving object tracking (up to 5 km/h) (REAL_TIME_BASIC)\n"     \
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

#define NL                APP_LOG_NL

#define MAX_REFLECTOR_INSTANCES  2u

static uint64_t measurement_counter;
static time_t start_time;

static cs_initiator_config_t initiator_config;
static rtl_config_t rtl_config;
static uint8_t connection_id = SL_BT_INVALID_CONNECTION_HANDLE;

typedef struct {
  sl_bt_cs_mode_t cs_mode;
  bool rssi_measurement_enabled;
  bool use_antenna_wired_offset;
  bool use_moving_object_tracking;
  bd_addr accepted_bt_address_list[MAX_REFLECTOR_INSTANCES];
  uint32_t accepted_bt_address_count;
  uint32_t max_reflector_instances;
} cs_host_config_t;

cs_host_config_t cs_host_config = {
  .cs_mode = sl_bt_cs_mode_pbr,
  .rssi_measurement_enabled = false,
  .use_antenna_wired_offset = false,
  .use_moving_object_tracking = true,
  .accepted_bt_address_count = 0u,
  .max_reflector_instances = 0u
};

static void cs_on_result(const cs_result_t *result, const void *user_data);
static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  sl_status_t sc;
  int opt;
  int cs_mode = sl_bt_cs_mode_pbr;

  app_log_info("Silicon Labs CS Legacy Initiator" APP_LOG_NL);
  app_log_info("--------------------------------" APP_LOG_NL);

  // Default parameters
  sc = cs_initiator_set_default_config(&initiator_config, &rtl_config);
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initialize the list of accepted BT addresses
  for (uint32_t i = 0u; i < MAX_REFLECTOR_INSTANCES; i++) {
    memset(cs_host_config.accepted_bt_address_list[i].addr, 0xFF, BT_ADDR_LEN);
  }

  // Process command line options.
  while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
    switch (opt) {
      // Print help.
      case 'h':
        app_log(USAGE, argv[0]);
        app_log(OPTIONS);
        exit(EXIT_SUCCESS);

      case 'm':
        // Mode.
        // 1 - RTT
        // 2 - Phase based
        cs_mode = atoi(optarg);
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
      default:
        sc = ncp_host_set_option((char)opt, optarg);
        if (sc == SL_STATUS_NOT_FOUND) {
          sc = app_log_set_option((char)opt, optarg);
        }
        if (sc != SL_STATUS_OK) {
          app_log(USAGE, argv[0]);
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  if (cs_host_config.accepted_bt_address_count > 0) {
    app_log_info("BLE address filtering enabled with %u specified BT addresses" APP_LOG_NL,
                 cs_host_config.accepted_bt_address_count);
  } else {
    app_log_info("BLE address filtering disabled" APP_LOG_NL);
  }

  if (cs_host_config.use_moving_object_tracking) {
    app_log_info("Tracking mode: moving objects" APP_LOG_NL);
    initiator_config.use_moving_object_tracking = true;
  } else {
    app_log_info("Tracking mode: stationary objects" APP_LOG_NL);
    initiator_config.use_moving_object_tracking = false;
  }

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

  initiator_config.cs_mode = cs_host_config.cs_mode;

  // Initialize NCP connection.
  sc = ncp_host_init();
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  app_assert_status(sc);
  app_log_info("NCP host initialised." APP_LOG_NL);
  app_log_info("Press Crtl+C to quit" APP_LOG_NL APP_LOG_NL);

  cs_log_create(argc, argv);
  rtl_log_init();
  ble_peer_manager_central_init();
  ble_peer_manager_filter_init();
  cs_initiator_init();
  start_time = time(NULL);
  measurement_counter = 0;
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
  cs_initiator_step();
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Deinit.
 *****************************************************************************/
void app_deinit(void)
{
  time_t end_time;
  double run_time;
  double measurements_frequency;

  end_time = time(NULL);
  cs_initiator_deinit();
  rtl_log_deinit();
  if (connection_id != SL_BT_INVALID_CONNECTION_HANDLE) {
    ble_peer_manager_central_close_connection(connection_id);
  }
  ncp_host_deinit();

  if (end_time > 0 && start_time > 0 && measurement_counter > 0) {
    run_time = difftime(end_time, start_time);
    if (run_time > 0) {
      measurements_frequency = measurement_counter / run_time;
      app_log("Measurement frequency = %f" APP_LOG_NL, measurements_frequency);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application deinit code here!                       //
  // This is called once during termination.                                 //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      sc = cs_antenna_configure(cs_host_config.use_antenna_wired_offset);
      app_assert_status(sc);
      break;

    // -------------------------------
    // Default event handler.
    default:
      // All other event is handled by the initiator event handler.
      cs_initiator_on_event(evt);
      break;
  }
}

/******************************************************************************
 * BLE peer manager event handler
 *
 * @param[in] evt Event coming from the peer manager.
 *****************************************************************************/
void ble_peer_manager_on_event(ble_peer_manager_evt_type_t *event)
{
  sl_status_t sc;
  const char* device_name = INITIATOR_DEVICE_NAME;

  switch (event->evt_id) {
    case BLE_PEER_MANAGER_ON_BOOT:
      app_log_info("peer manager: started." APP_LOG_NL);
      sc = ble_peer_manager_set_filter_device_name(device_name,
                                                   strlen(device_name),
                                                   false);
      app_assert_status(sc);

      // Start scanning for reflector connections
      sc = ble_peer_manager_central_create_connection();
      app_assert_status(sc);
      app_log_info("peer manager: scanning for device name \'%s\' ..." NL, device_name);

      break;

    case BLE_PEER_MANAGER_ON_CONN_OPENED_CENTRAL:
      app_log_info("peer manager: connection opened: #%u." NL, event->connection_id);
      sc = cs_initiator_create(event->connection_id,
                               &initiator_config,
                               &rtl_config,
                               cs_on_result,
                               cs_on_error);
      measurement_counter = 0u;
      connection_id = event->connection_id;
      break;

    case BLE_PEER_MANAGER_ON_CONN_CLOSED:
      app_log_info("peer manager: connection closed." NL);
      sc = cs_initiator_delete(event->connection_id);
      connection_id = SL_BT_INVALID_CONNECTION_HANDLE;
      break;

    case BLE_PEER_MANAGER_ERROR:
      app_log_info("peer manager: error on connection #%u!" NL,
                   event->connection_id);
      break;

    default:
      app_log_info("peer manager: unhandled event on connection #%u! [evt: 0x%x]" NL,
                   event->connection_id,
                   event->evt_id);
      break;
  }
}

static void cs_on_result(const cs_result_t *result, const void *user_data)
{
  (void) user_data;

  app_log_info("+-[I#%u - Measurement %04llu]----------------------------+" NL,
               result->connection,
               (unsigned long long)measurement_counter);

  app_log_info("Measurement result: %u mm" NL,
               (uint32_t)(result->distance * 1000.f));

  app_log_info("Measurement likeliness: %02f" NL,
               result->likeliness);

  if (initiator_config.rssi_measurement_enabled) {
    app_log_info("RSSI distance: %u mm" NL,
                 (uint32_t)(result->rssi_distance * 1000.f));
  }

  app_log_info("CS bit error rate: %u" NL, result->cs_bit_error_rate);
  app_log_info("+-------------------------------------------------------+" NL NL);
  measurement_counter++;
}

static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc)
{
  sl_status_t peer_sc = SL_STATUS_OK;
  app_log_error("!!! Error happened @ initiator #%u!"
                "[E: 0x%x sc: 0x%x]" NL,
                conn_handle,
                err_evt,
                sc);
  peer_sc = ble_peer_manager_central_close_connection(conn_handle);
  if (sc != SL_STATUS_OK) {
    app_log_error("peer manager: could not close connection #%u! [sc: 0x%x]" NL,
                  conn_handle,
                  peer_sc);
  }
}
