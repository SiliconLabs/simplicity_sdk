/***************************************************************************//**
 * @file
 * @brief CS Initiator example application logic
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
#include "sl_bluetooth.h"
#include "sl_component_catalog.h"
#include "em_common.h"
#include "app_assert.h"

// app content
#include "app.h"
#include "initiator_app_config.h"
#include "rtl_log.h"

// initiator content
#include "cs_initiator.h"
#include "cs_initiator_configurator.h"
#include "cs_initiator_config.h"
#include "cs_initiator_display.h"
#include "cs_antenna.h"

// other required content
#include "ble_peer_manager_common.h"
#include "ble_peer_manager_connections.h"
#include "ble_peer_manager_central.h"
#include "ble_peer_manager_filter.h"
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"
#ifdef SL_CATALOG_CS_INITIATOR_CLI_PRESENT
#include "cs_initiator_cli.h"
#endif // SL_CATALOG_CS_INITIATOR_CLI_PRESENT

#define PB0               SL_SIMPLE_BUTTON_INSTANCE(0)
#define NL                APP_LOG_NL

static void cs_on_result(const cs_result_t *result, const void *user_data);
static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc);

static bool button_pressed = false;
static bool measurement_arrived = false;
static uint32_t measurement_cnt = 0u;
static cs_initiator_config_t initiator_config;
static rtl_config_t rtl_config;

static cs_result_t measurement;

/******************************************************************************
 * Application Init
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  uint8_t mode = CS_INITIATOR_MODE_PBR; // PBR, the default mode

  app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
  app_log_filter_threshold_enable(true);
  app_log_info(NL);
  app_log_info("+-[CS initiator by Silicon Labs]------------------------+" NL);
  app_log_info("| Press PB0 while reset to select RTT measurement mode! |" NL);
  app_log_info("+-------------------------------------------------------+" NL);

  app_log_info("Wire%s antenna offset will be used." NL,
               CS_INITIATOR_ANTENNA_OFFSET ? "d" : "less");

  app_log_info("Minimum CS procedure interval: %u" NL, CS_INITIATOR_MIN_INTERVAL);
  app_log_info("Maximum CS procedure interval: %u" NL, CS_INITIATOR_MAX_INTERVAL);

  // initialize measurement variable
  memset(&measurement, 0u, sizeof(measurement));
  measurement.connection = SL_BT_INVALID_CONNECTION_HANDLE;

  rtl_log_init();
  ble_peer_manager_central_init();
  ble_peer_manager_filter_init();
  cs_initiator_init();
  sl_status_t sc = cs_initiator_display_init();
  app_assert_status_f(sc, "cs_initiator_display_init failed");

  // Default parameters
  cs_initiator_set_default_config(&initiator_config, &rtl_config);

  // Log channel map
  app_log_info("+-[ CS channel map ]------------------------------------+" NL);
  app_log_info("");
  for (uint32_t i = 0; i < initiator_config.channel_map_len; i++) {
    app_log_append_info("0x%02x ", initiator_config.channel_map.data[i]);
  }
  app_log_append_info(NL);
  app_log_info("+-------------------------------------------------------+" NL);

  // Configured parameters
  initiator_config.rssi_measurement_enabled = CS_RSSI_MEASUREMENT_ENABLED;
  app_log_info("RSSI will");
  if (!initiator_config.rssi_measurement_enabled) {
    app_log_append_info(" not");
  }
  app_log_append_info(" be measured." NL);

  if (SL_SIMPLE_BUTTON_COUNT >= 1) {
    button_pressed = sl_button_get_state(PB0);
  }

  if (MEASUREMENT_MODE == CS_INITIATOR_MODE_DYNAMIC) {
    if (button_pressed == SL_SIMPLE_BUTTON_PRESSED) {
      mode = CS_INITIATOR_MODE_RTT;
    }
  } else {
    mode = MEASUREMENT_MODE;
  }

  switch (mode) {
    case CS_INITIATOR_MODE_RTT:
      initiator_config.cs_mode = sl_bt_cs_mode_rtt;
      app_log_info("RTT");
      cs_initiator_display_write_text("RTT mode", ROW_MODE);
      break;
    case CS_INITIATOR_MODE_PBR:
      initiator_config.cs_mode = sl_bt_cs_mode_pbr;
      app_log_info("PBR");
      cs_initiator_display_write_text("PBR mode", ROW_MODE);
      break;
    default:
      break;
  }

  app_log_append_info(" measurement mode selected." NL);

  cs_initiator_display_set_alignment(CS_INITIATOR_DISPLAY_ALIGNMENT_CENTER);
  cs_initiator_display_write_text("CS distance", ROW_DISTANCE_TEXT);
  cs_initiator_display_print_value(0.0f, ROW_DISTANCE_VALUE, "m");
  cs_initiator_display_write_text("Likeliness", ROW_LIKELINESS_TEXT);
  cs_initiator_display_print_value(0.0f, ROW_LIKELINESS_VALUE, NULL);
  if (initiator_config.rssi_measurement_enabled) {
    cs_initiator_display_write_text("RSSI distance", ROW_RSSI_DISTANCE_TEXT);
    cs_initiator_display_print_value(0.0f, ROW_RSSI_DISTANCE_VALUE, "m");
  }
  cs_initiator_display_write_text("Bit error rate", ROW_BIT_ERROR_RATE_TEXT);
  cs_initiator_display_print_value(0, ROW_BIT_ERROR_RATE_VALUE, NULL);
  cs_initiator_display_update();

  app_log_info("+-------------------------------------------------------+" NL);
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * Application Process Action
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  if (measurement_arrived) {
    // write results to the display & to the iostream
    measurement_arrived = false;

    app_log_info("+-[I#%u - Measurement %04lu]----------------------------+" NL,
                 measurement.connection,
                 measurement_cnt);

    cs_initiator_display_set_alignment(CS_INITIATOR_DISPLAY_ALIGNMENT_CENTER);
    app_log_info("Measurement result: %lu mm" NL,
                 (uint32_t)(measurement.distance * 1000.f));
    cs_initiator_display_print_value(measurement.distance, ROW_DISTANCE_VALUE, "m");

    uint32_t likeliness_whole = (uint32_t)measurement.likeliness;
    uint32_t likeliness_frac =
      (uint32_t)((measurement.likeliness - (float)likeliness_whole) * 100.0f);
    app_log_info("Measurement likeliness: %lu.%02lu" NL,
                 likeliness_whole,
                 likeliness_frac);
    cs_initiator_display_print_value(measurement.likeliness, ROW_LIKELINESS_VALUE, NULL);

    if (initiator_config.rssi_measurement_enabled) {
      app_log_info("RSSI distance: %lu mm" NL,
                   (uint32_t)(measurement.rssi_distance * 1000.f));
      cs_initiator_display_print_value(measurement.rssi_distance,
                                       ROW_RSSI_DISTANCE_VALUE,
                                       "m");
    }

    app_log_info("CS bit error rate: %u" NL, measurement.cs_bit_error_rate);
    cs_initiator_display_print_value(measurement.cs_bit_error_rate, ROW_BIT_ERROR_RATE_VALUE, NULL);
    cs_initiator_display_update();
    app_log_info("+-------------------------------------------------------+" NL NL);
  }
  rtl_log_step();

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * BLE peer manager event handler
 *
 * @param[in] evt Event coming from the peer manager.
 *****************************************************************************/
void ble_peer_manager_on_event_initiator(ble_peer_manager_evt_type_t *event)
{
  sl_status_t sc;
  const char* device_name = INITIATOR_DEVICE_NAME;

  switch (event->evt_id) {
    case BLE_PEER_MANAGER_ON_BOOT:
    {
      app_log_info("peer manager: started" NL);
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

      // Set antenna offset
      sc = cs_antenna_configure(CS_INITIATOR_ANTENNA_OFFSET);
      app_assert_status(sc);

      sc = ble_peer_manager_set_filter_device_name(device_name,
                                                   strlen(device_name),
                                                   false);
      app_assert_status(sc);

#ifndef SL_CATALOG_CS_INITIATOR_CLI_PRESENT
      sc = ble_peer_manager_central_create_connection();
      app_assert_status(sc);
#else
      app_log_info("CS CLI is active." APP_LOG_NL);
#endif // SL_CATALOG_CS_INITIATOR_CLI_PRESENT
      // Start scanning for reflector connections
      app_log_info("peer manager: scanning for device name \'%s\' ..." NL, device_name);

      break;

      case BLE_PEER_MANAGER_ON_CONN_OPENED_CENTRAL:
        app_log_info("peer manager: connection opened: #%u." NL, event->connection_id);
#ifdef SL_CATALOG_CS_INITIATOR_CLI_PRESENT
        initiator_config.cs_mode = cs_initiator_cli_get_mode();
#endif // SL_CATALOG_CS_INITIATOR_CLI_PRESENT
        sc = cs_initiator_create(event->connection_id,
                                 &initiator_config,
                                 &rtl_config,
                                 cs_on_result,
                                 cs_on_error);
        measurement_cnt = 0u;
    }
    break;

    case BLE_PEER_MANAGER_ON_CONN_CLOSED:
      app_log_info("peer manager: connection closed." NL);
      sc = cs_initiator_delete(event->connection_id);
      // Start scanning for reflector connections
      sc = ble_peer_manager_central_create_connection();
      app_assert_status(sc);
      app_log_info("peer manager: scanning for device name \'%s\' ..." NL, device_name);
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
  if (result != NULL) {
    memcpy(&measurement, result, sizeof(measurement));
    measurement_arrived = true;
    measurement_cnt++;
  }
}

static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc)
{
  sl_status_t peer_sc = SL_STATUS_OK;
  app_log_error("!!! Error happened @ initiator #%u!"
                "[E: 0x%x sc: 0x%lx]" NL,
                conn_handle,
                err_evt,
                sc);
  peer_sc = ble_peer_manager_central_close_connection(conn_handle);
  if (sc != SL_STATUS_OK) {
    app_log_error("peer manager: could not close connection #%u! [sc: 0x%lx]" NL,
                  conn_handle,
                  peer_sc);
  }
}
