/***************************************************************************//**
 * @file
 * @brief Bluetooth Peer Manager - peripheral
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
#include "sl_component_catalog.h"
#include "ble_peer_manager_common.h"
#include "ble_peer_manager_common_config.h"
#include "ble_peer_manager_common_internal.h"
#include "ble_peer_manager_peripheral.h"
#include "ble_peer_manager_peripheral_internal.h"
#include "ble_peer_manager_peripheral_config.h"
#include "ble_peer_manager_connections.h"

// -----------------------------------------------------------------------------
// Macros and Typedefs
typedef struct ble_peer_manager_advertiser_s {
  uint8_t advertising_handle;
  sl_bt_advertiser_discovery_mode_t adv_discovery_mode;
} ble_peer_manager_advertiser_t;

// -----------------------------------------------------------------------------
// Forward declaration of private functions
static ble_peer_manager_state_id get_state(void);
static void set_state(ble_peer_manager_state_id new_state);

// -----------------------------------------------------------------------------
// Static variables
static ble_peer_manager_advertiser_t advertiser;
static ble_peer_manager_state_id state;

// -----------------------------------------------------------------------------
// Public functions
void ble_peer_manager_peripheral_init()
{
  // Set default values
  advertiser.advertising_handle = SL_BT_INVALID_ADVERTISING_SET_HANDLE;
  advertiser.adv_discovery_mode = BLE_PEER_MANAGER_PERIPHERAL_CONFIG_DEFAULT_ADV_DISCOVERY_MODE;
  ble_peer_manager_clear_all_connections();
  set_state(BLE_PEER_MANAGER_STATE_IDLE);
}

void ble_peer_manager_peripheral_on_bt_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  ble_peer_manager_evt_type_t peer_evt;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_advertiser_timeout_id:
      // Received when the number of advertising events command is done
      if (evt->data.evt_advertiser_timeout.handle == advertiser.advertising_handle) {
        peer_evt.evt_id = BLE_PEER_MANAGER_ON_ADV_STOPPED;
        peer_evt.connection_id = SL_BT_INVALID_CONNECTION_HANDLE;
        set_state(BLE_PEER_MANAGER_STATE_IDLE);
        ble_peer_manager_on_event(&peer_evt);
      }
      break;

    case sl_bt_evt_connection_opened_id:
      if (evt->data.evt_connection_opened.advertiser == advertiser.advertising_handle
          && evt->data.evt_connection_opened.role == sl_bt_connection_role_peripheral) {
        ble_peer_manager_add_connection(evt->data.evt_connection_opened.connection,
                                        evt->data.evt_connection_opened.address,
                                        sl_bt_connection_role_peripheral);
        ble_peer_manager_log_info("Connection opened as peripheral with '%02x:%02x:%02x:%02x:%02x:%02x'" APP_LOG_NL,
                                  evt->data.evt_connection_opened.address.addr[5],
                                  evt->data.evt_connection_opened.address.addr[4],
                                  evt->data.evt_connection_opened.address.addr[3],
                                  evt->data.evt_connection_opened.address.addr[2],
                                  evt->data.evt_connection_opened.address.addr[1],
                                  evt->data.evt_connection_opened.address.addr[0]);
        peer_evt.evt_id = BLE_PEER_MANAGER_ON_CONN_OPENED_PERIPHERAL;
        peer_evt.connection_id = evt->data.evt_connection_opened.connection;
        set_state(BLE_PEER_MANAGER_STATE_IDLE);
        ble_peer_manager_on_event(&peer_evt);
      }
      break;

    case sl_bt_evt_connection_closed_id:
      if (ble_peer_manager_is_conn_handle_in_array(evt->data.evt_connection_closed.connection)
          && !(ble_peer_manager_is_conn_handle_central(evt->data.evt_connection_closed.connection))) {
        ble_peer_manager_log_info("Peripheral device connection closed." APP_LOG_NL);
        sc = ble_peer_manager_delete_connection(evt->data.evt_connection_closed.connection);
        if (sc != SL_STATUS_OK) {
          peer_evt.evt_id = BLE_PEER_MANAGER_ERROR;
          peer_evt.connection_id = SL_BT_INVALID_CONNECTION_HANDLE;
        } else {
          peer_evt.evt_id = BLE_PEER_MANAGER_ON_CONN_CLOSED;
          peer_evt.connection_id = evt->data.evt_connection_closed.connection;
        }
        ble_peer_manager_on_event(&peer_evt);
      }
      break;
  }
}

sl_status_t ble_peer_manager_peripheral_set_advertiser(sl_bt_advertiser_discovery_mode_t adv_discovery_mode)
{
  if (get_state() == BLE_PEER_MANAGER_ADVERTISING) {
    ble_peer_manager_log_error("Already advertising. \
                                Please stop the advertiser before changing it's settings." APP_LOG_NL);
    return SL_STATUS_INVALID_STATE;
  }
  advertiser.adv_discovery_mode = adv_discovery_mode;
  return SL_STATUS_OK;
}

sl_status_t ble_peer_manager_peripheral_create_connection()
{
  if (get_state() == BLE_PEER_MANAGER_ADVERTISING) {
    ble_peer_manager_log_error("Already advertising" APP_LOG_NL);
    return SL_STATUS_INVALID_STATE;
  }
  // Start advertising
  sl_status_t sc = ble_peer_manager_peripheral_start_advertising(advertiser.advertising_handle);
  if (sc == SL_STATUS_OK) {
    set_state(BLE_PEER_MANAGER_ADVERTISING);
  }
  return sc;
}

sl_status_t ble_peer_manager_peripheral_start_advertising(uint8_t advertising_handle)
{
  sl_status_t sc;
  // Stop running advertising
  if (get_state() == BLE_PEER_MANAGER_ADVERTISING) {
    sc = sl_bt_advertiser_stop(advertiser.advertising_handle);
    if (sc != SL_STATUS_OK) {
      return sc;
    }
  }

  if (advertising_handle != SL_BT_INVALID_ADVERTISING_SET_HANDLE) {
    advertiser.advertising_handle = advertising_handle;
  } else {
    // If we already have an existing set, delete it
    if (advertiser.advertising_handle != SL_BT_INVALID_ADVERTISING_SET_HANDLE) {
      sc = sl_bt_advertiser_delete_set(advertiser.advertising_handle);
      if (sc != SL_STATUS_OK) {
        return sc;
      }
    }

    // Create new advertiser set
    sc = sl_bt_advertiser_create_set(&advertiser.advertising_handle);
    if (sc != SL_STATUS_OK) {
      return sc;
    }

    // Generate data for advertising
    sc = sl_bt_legacy_advertiser_generate_data(advertiser.advertising_handle,
                                               advertiser.adv_discovery_mode);
    if (sc != SL_STATUS_OK) {
      return sc;
    }

    // Set advertising interval
    sc = sl_bt_advertiser_set_timing(
      advertiser.advertising_handle,
      BLE_PEER_MANAGER_PERIPHERAL_CONFIG_ADV_TIMING_INTERVAL_MIN,   // min. adv. interval (milliseconds * 1.6)
      BLE_PEER_MANAGER_PERIPHERAL_CONFIG_ADV_TIMING_INTERVAL_MAX,   // max. adv. interval (milliseconds * 1.6)
      BLE_PEER_MANAGER_PERIPHERAL_CONFIG_ADV_TIMING_DURATION,       // adv. duration
      BLE_PEER_MANAGER_PERIPHERAL_CONFIG_ADV_TIMING_MAX_EVENTS);    // max. num. adv. events
    if (sc != SL_STATUS_OK) {
      return sc;
    }
  }
  sc = sl_bt_legacy_advertiser_start(advertiser.advertising_handle,
                                     BLE_PEER_MANAGER_PERIPHERAL_CONFIG_DEFAULT_ADV_CONNECTION_MODE);
  ble_peer_manager_log_info("Started advertising..." APP_LOG_NL);
  return sc;
}

sl_status_t ble_peer_manager_peripheral_stop_advertising(uint8_t advertising_handle)
{
  sl_status_t sc;
  if (advertising_handle != SL_BT_INVALID_ADVERTISING_SET_HANDLE) {
    advertiser.advertising_handle = advertising_handle;
  }

  // Stop running advertising
  sc = sl_bt_advertiser_stop(advertiser.advertising_handle);
  if (sc != SL_STATUS_OK) {
    return sc;
  }
  // Delete advertiser set
  sc = sl_bt_advertiser_delete_set(advertiser.advertising_handle);
  return sc;
}

sl_status_t ble_peer_manager_peripheral_close_connection(uint8_t conn_handle)
{
  sl_status_t sc = SL_STATUS_OK;
  if (ble_peer_manager_is_conn_handle_in_array(conn_handle)
      && !(ble_peer_manager_is_conn_handle_central(conn_handle))) {
    ble_peer_manager_log_info("Closing connection." APP_LOG_NL);
    sc = sl_bt_connection_close(conn_handle);
    set_state(BLE_PEER_MANAGER_STATE_CLOSING);
    if (sc != SL_STATUS_OK) {
      return sc;
    }
  } else {
    ble_peer_manager_log_error("Connection handle not found." APP_LOG_NL);
    return SL_STATUS_NOT_FOUND;
  }
  return sc;
}

// -----------------------------------------------------------------------------
// Private functions
static ble_peer_manager_state_id get_state(void)
{
  return state;
}

static void set_state(ble_peer_manager_state_id new_state)
{
  state = new_state;
}
