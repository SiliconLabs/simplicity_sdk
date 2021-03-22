/***************************************************************************//**
 * @file
 * @brief CS reflector
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

#include "sl_bt_api.h"
#include "app_assert.h"
#include "app_log.h"
#include "cs_ras.h"
#include "cs_reflector.h"
#include "cs_reflector_config.h"
#include "cs_reflector_connmanager.h"
#include "cs_reflector_event_buf.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

// -----------------------------------------------------------------------------
// Macros

// Component logging
#if defined(SL_CATALOG_APP_LOG_PRESENT) && CS_REFLECTOR_LOG
#define LOG_PREFIX                  "[reflector] "
#define LOG_NL                      APP_LOG_NL
#define reflector_log_debug(...)    app_log_debug(LOG_PREFIX  __VA_ARGS__)
#define reflector_log_info(...)     app_log_info(LOG_PREFIX  __VA_ARGS__)
#define reflector_log_warning(...)  app_log_warning(LOG_PREFIX  __VA_ARGS__)
#define reflector_log_error(...)    app_log_error(LOG_PREFIX  __VA_ARGS__)
#define reflector_log_critical(...) app_log_critical(LOG_PREFIX  __VA_ARGS__)
#else
#define LOG_NL
#define reflector_log_debug(...)
#define reflector_log_info(...)
#define reflector_log_warning(...)
#define reflector_log_error(...)
#define reflector_log_critical(...)
#endif

static bool cs_reflector_send_next_available(const uint8_t conn_handle);
static void cs_indicate_ranging_data_ready(const uint8_t conn_handle);
static void cs_reflector_find_and_send_cs_result(const uint8_t conn_handle,
                                                 const uint16_t procedure_index,
                                                 const uint16_t subevent_index);

static uint16_t gattdb_ras_control_point_chr_handle;
static uint16_t gattdb_ras_subevent_ranging_data_chr_handle;
static void cs_reflector_gattdb_init(void);

bool cs_reflector_identify(uint8_t conn_handle)
{
  connection_ctx_t *conn_ctx = cs_rcm_get_connection_ctx(conn_handle);
  if (conn_ctx == NULL) {
    return false;
  }
  return true;
}

sl_status_t cs_reflector_create(uint8_t               conn_handle,
                                cs_reflector_config_t *reflector_config)
{
  reflector_log_info("R#%u - Creating new reflector instance; conn_handle='%u'" LOG_NL,
                     conn_handle,
                     conn_handle);
  sl_status_t sc = sl_bt_cs_set_default_settings(conn_handle,
                                                 sl_bt_cs_role_status_disable,
                                                 sl_bt_cs_role_status_enable,
                                                 1,
                                                 reflector_config->max_tx_power_dbm);
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  // Add the new instance
  if (!cs_rcm_add_new_initiator_connection(conn_handle)) {
    // New instance cannot be added - we're at capacity
    reflector_log_warning("R#%u - Cannot add new reflector connection" LOG_NL,
                          conn_handle);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  cs_reflector_gattdb_init();

  reflector_log_info("Reflector instances: %lu/%u" LOG_NL,
                     cs_rcm_get_number_of_connections(),
                     CS_REFLECTOR_MAX_CONNECTIONS);
  return sc;
}

uint32_t cs_reflector_get_active_instance_count(void)
{
  return cs_rcm_get_number_of_connections();
}

sl_status_t cs_reflector_delete(uint8_t conn_handle)
{
  if (!cs_reflector_identify(conn_handle)) {
    return SL_STATUS_NOT_FOUND;
  }
  reflector_log_info("R#%u - Deleting instance" LOG_NL, conn_handle);
  cs_rcm_remove_initiator_connection(conn_handle);
  cs_reflector_event_buf_purge_data(conn_handle);
  reflector_log_info("Reflector instances: %lu/%u" LOG_NL,
                     cs_rcm_get_number_of_connections(),
                     CS_REFLECTOR_MAX_CONNECTIONS);
  return SL_STATUS_OK;
}

bool cs_reflector_on_bt_event(sl_bt_msg_t *evt)
{
  bool handled = false;
  sl_status_t sc;
  (void)sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates that the BT stack has booted successfully
    case sl_bt_evt_system_boot_id:
    {
      cs_reflector_event_buf_clear();
    }
    break;

    // -------------------------------
    // This event indicates that a CS procedure was started by the initiator
    case sl_bt_evt_cs_procedure_enable_complete_id:
    {
      uint8_t conn_handle = evt->data.evt_cs_procedure_enable_complete.connection;
      if (!cs_reflector_identify(conn_handle)) {
        break;
      }
      // Set message handled
      handled = true;
      reflector_log_info("-------------------------------------------------------" LOG_NL);
      reflector_log_info("R#%u - CS procedure started" LOG_NL, conn_handle);
    }
    break;

    // -------------------------------
    // This event indicates that a new Channel Sounding result is available
    case sl_bt_evt_cs_result_id:
    {
      uint8_t conn_handle = evt->data.evt_cs_result.connection;
      connection_ctx_t *conn_ctx = cs_rcm_get_connection_ctx(conn_handle);
      if (conn_ctx == NULL) {
        break;
      }
      // Set message handled
      handled = true;

      uint16_t current_procedure_index = conn_ctx->current_procedure_index;
      uint16_t current_cs_subevent_index = conn_ctx->current_cs_subevent_index;
      uint16_t received_procedure_index = 0u;

      received_procedure_index = evt->data.evt_cs_result.procedure_counter;
      reflector_log_debug("R#%u - CS procedure result; idx_original='%u'" LOG_NL,
                          conn_handle, evt->data.evt_cs_result.procedure_counter);

      // Count the real procedure index (stack returns 0 for consecutive subevents)
      // If the current index is not zero and we receive a zero that means
      // that it's a consecutive event
      if (current_procedure_index != 0 && received_procedure_index == 0) {
        received_procedure_index = current_procedure_index;
      } else {
        current_procedure_index = received_procedure_index;
      }

      reflector_log_info("R#%u - CS procedure result available; procedure_index='%u' se_idx='%u' se_done='%02x'" LOG_NL,
                         conn_handle,
                         current_procedure_index,
                         current_cs_subevent_index,
                         evt->data.evt_cs_result.subevent_done_status);

      // Get a free space in the event buffer we can write
      cs_event_data_t *cs_event_buf_entry = cs_reflector_event_buf_get_element_for_write();
      if (cs_event_buf_entry == NULL) {
        reflector_log_error("CS event buffer overflown! No more space for new events!" LOG_NL);
        break;
      }

      // Get the size of the CS result data
      size_t result_len = sizeof(evt->data.evt_cs_result) + evt->data.evt_cs_result.data.len;
      // Check if we can copy the whole result data
      app_assert((result_len <= CS_EVENT_BUF_DATA_MAX_LEN),
                 "Event data exceeds the event buffer size, possible data loss, aborting; "
                 "result_len='%u' cs_event_buf_data_len='%u'",
                 result_len, CS_EVENT_BUF_DATA_MAX_LEN);

      // Copy the result to the event buffer element
      memcpy(cs_event_buf_entry->data, (uint8_t *)&evt->data.evt_cs_result, result_len);
      // Fill the rest of the results
      cs_event_buf_entry->data_len = result_len;
      cs_event_buf_entry->procedure_index = current_procedure_index;
      cs_event_buf_entry->subevent_index = current_cs_subevent_index;
      cs_event_buf_entry->empty = false;
      cs_event_buf_entry->indication_sent = false;
      cs_event_buf_entry->conn_handle = evt->data.evt_cs_result.connection;

      reflector_log_debug("CS event buf: %lu/%u" LOG_NL,
                          CS_REFLECTOR_CS_EVENT_BUF_SIZE - cs_reflector_event_buf_get_free_space(),
                          CS_REFLECTOR_CS_EVENT_BUF_SIZE);

      // Count the subevent in the current procedure
      if (current_procedure_index == received_procedure_index) {
        current_cs_subevent_index++;
      } else {
        current_cs_subevent_index = 0u;
      }

      conn_ctx->current_procedure_index = current_procedure_index;
      conn_ctx->current_cs_subevent_index = current_cs_subevent_index;

      cs_indicate_ranging_data_ready(conn_ctx->conn_handle);

      // If the periodic RAS Periodic Notification mode is enabled send the results immediately
      if (conn_ctx->ras_periodic_notification_mode_enabled) {
        cs_reflector_send_next_available(conn_ctx->conn_handle);
      }

      uint8_t cs_procedure_status = evt->data.evt_cs_result.procedure_done_status;
      if (cs_procedure_status == 0x00) {
        reflector_log_info("R#%u - CS procedure finished" LOG_NL, conn_handle);
      } else if (cs_procedure_status == 0x01) {
        reflector_log_info("R#%u - CS procedure ongoing, waiting for further results..." LOG_NL,
                           conn_handle);
      } else {
        reflector_log_error("R#%u - CS procedure failed; error='0x%02x" LOG_NL,
                            conn_handle,
                            cs_procedure_status);
      }
    }
    break;

    // -------------------------------
    // This event indicates that a value was changed in the local GATT by a remote device
    case sl_bt_evt_gatt_server_attribute_value_id:
    {
      uint8_t conn_handle = evt->data.evt_gatt_characteristic_value.connection;
      //reflector_log_info("R#%u - Received new GATT data" LOG_NL, conn_handle);

      connection_ctx_t *conn_ctx = cs_rcm_get_connection_ctx(conn_handle);
      if (conn_ctx == NULL) {
        break;
      }
      // Set message handled
      handled = true;
      // If the RAS Control Point was written
      if (evt->data.evt_gatt_characteristic_value.characteristic == gattdb_ras_control_point_chr_handle) {
        reflector_log_info("R#%u - Received new RAS Control Point message" LOG_NL,
                           conn_handle);
        // Parse the incoming RAS message
        // The result - if valid - will tell the opcode and whether the
        // periodic notification mode was enabled / disabled
        ras_control_point_parse_result ras_result = cs_ras_parse_control_point_message(
          evt->data.evt_gatt_characteristic_value.value.data,
          evt->data.evt_gatt_characteristic_value.value.len);

        ras_opcode_t ras_opcode = ras_result.opcode;
        conn_ctx->ras_periodic_notification_mode_enabled = ras_result.periodic_notification_mode_set_enabled;

        // Initiator requesting a certain measurement
        // The initiator can optionally turn on the 'Periodic Notification Mode' while issuing this command
        if (ras_opcode == CS_RAS_OPCODE_RANGING_DATA_GET_COMMAND) {
          ras_ranging_data_get_command_t rx_cmd =
            *((ras_ranging_data_get_command_t*)(evt->data.evt_gatt_characteristic_value.value.data));
          cs_reflector_find_and_send_cs_result(conn_handle, rx_cmd.procedure_index, rx_cmd.subevent_index);

          // If the RAS Periodic Notification was enabled
          if (conn_ctx->ras_periodic_notification_mode_enabled) {
            reflector_log_info("R#%u - RAS Periodic Notification mode enabled" LOG_NL,
                               conn_handle);
            // Send out all the results we have
            while (cs_reflector_send_next_available(conn_handle)) ;
          } else {
            reflector_log_info("R#%u - RAS Periodic Notification mode disabled" LOG_NL,
                               conn_handle);
          }
        }
      }
    }
    break;

    // -------------------------------
    // This event is received when a GATT characteristic status changes
    case sl_bt_evt_gatt_server_characteristic_status_id:
    {
      uint8_t conn_handle = evt->data.evt_gatt_characteristic_value.connection;
      if (!cs_reflector_identify(conn_handle)) {
        break;
      }
      // Check for the RAS control point characteristic status changes
      if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_ras_control_point_chr_handle) {
        connection_ctx_t *conn_ctx = cs_rcm_get_connection_ctx(conn_handle);
        if (conn_ctx == NULL) {
          reflector_log_warning("R#%u - Couldn't find registered connection for handle '%u'" LOG_NL,
                                conn_handle,
                                conn_handle);
          break;
        }
        // Set message handled
        handled = true;
        if ((evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x02)
            && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01)) {
          reflector_log_info("R#%u - RAS control point indication enabled" LOG_NL, conn_handle);
          conn_ctx->ras_control_point_indication_enabled = true;
        } else if ((evt->data.evt_gatt_server_characteristic_status.client_config_flags == 0x00)
                   && (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x01)) {
          reflector_log_info("R#%u - RAS control point indication disabled" LOG_NL, conn_handle);
          conn_ctx->ras_control_point_indication_enabled = false;
        } else if (evt->data.evt_gatt_server_characteristic_status.status_flags == 0x02) {
          reflector_log_info("R#%u - RAS control point confirmation received" LOG_NL, conn_handle);
          conn_ctx->indication_in_progress = false;
          cs_indicate_ranging_data_ready(conn_handle);
        }
      }
    }
    break;

    // -------------------------------
    // This event indicates that the Channel Sounding Security Enable procedure has completed
    case sl_bt_evt_cs_security_enable_complete_id:
    {
      uint8_t conn_handle = evt->data.evt_cs_security_enable_complete.connection;
      if (!cs_reflector_identify(conn_handle)) {
        break;
      }
      // Set message handled
      handled = true;
      reflector_log_info("R#%u - CS Security Enable completed" LOG_NL, conn_handle);
    }
    break;

    // -------------------------------
    // This event indicates that the Channel Sounding configuration was completed
    case sl_bt_evt_cs_config_complete_id:
    {
      uint8_t conn_handle = evt->data.evt_cs_config_complete.connection;
      if (!cs_reflector_identify(conn_handle)) {
        break;
      }
      // Set message handled
      handled = true;
      reflector_log_info("R#%u - CS configuration completed" LOG_NL, conn_handle);
    }
    break;
  }
  // Return false if the message was handled above
  return !handled;
}

// Finds the next available CS result and sends it to the initiator
static bool cs_reflector_send_next_available(const uint8_t conn_handle)
{
  cs_event_data_t *cs_event_buf_entry = cs_reflector_event_buf_get_next_element(conn_handle);
  if (cs_event_buf_entry == NULL) {
    reflector_log_warning("R#%u - No results left to transfer for this connection" LOG_NL,
                          conn_handle);
    return false;
  }

  reflector_log_info("R#%u - Transferring next CS result to the initiator; index='%u' subevent_index='%u' len='%u'" LOG_NL,
                     conn_handle,
                     cs_event_buf_entry->procedure_index,
                     cs_event_buf_entry->subevent_index,
                     cs_event_buf_entry->data_len);

  struct sl_bt_evt_cs_result_s* result_ptr = (struct sl_bt_evt_cs_result_s*)cs_event_buf_entry->data;
  reflector_log_info("R#%u - CS packet details; proc_cnt='%u' proc_done='%u' se_done='%u' len='%u'" LOG_NL,
                     conn_handle,
                     result_ptr->procedure_counter,
                     result_ptr->procedure_done_status,
                     result_ptr->subevent_done_status,
                     cs_event_buf_entry->data_len);

  sl_status_t sc = sl_bt_gatt_server_send_notification(conn_handle,
                                                       gattdb_ras_subevent_ranging_data_chr_handle,
                                                       cs_event_buf_entry->data_len,
                                                       cs_event_buf_entry->data);
  // Give warning that we couldn't transfer the data
  // Data will be discarded anyways
  if (sc != SL_STATUS_OK) {
    reflector_log_warning("R#%u - Cannot transfer CS result, discarding; idx='%u' sc='0x%02lx'" LOG_NL,
                          conn_handle,
                          cs_event_buf_entry->procedure_index,
                          sc);
  }
  // Mark the event buffer element as consumed
  cs_event_buf_entry->empty = true;
  return true;
}

// Finds and sends the requested CS result (if available) to the initiator
static void cs_reflector_find_and_send_cs_result(const uint8_t conn_handle,
                                                 const uint16_t procedure_index,
                                                 const uint16_t subevent_index)
{
  cs_event_data_t *cs_event_buf_entry = cs_reflector_event_buf_find(conn_handle,
                                                                    procedure_index,
                                                                    subevent_index);
  if (cs_event_buf_entry == NULL) {
    reflector_log_warning("R#%u - Requested CS result cannot be found; index='%u' subevent_index='%u'" LOG_NL,
                          conn_handle, procedure_index, subevent_index);
    return;
  }

  reflector_log_info("R#%u - Transferring CS result to the initiator; index='%u' subevent_index='%u'" LOG_NL,
                     conn_handle,
                     procedure_index,
                     subevent_index);
  sl_status_t sc = sl_bt_gatt_server_send_notification(conn_handle,
                                                       gattdb_ras_subevent_ranging_data_chr_handle,
                                                       cs_event_buf_entry->data_len,
                                                       cs_event_buf_entry->data);
  app_assert_status(sc);
  // Mark the event buffer element as consumed
  cs_event_buf_entry->empty = true;
}

// Looks for the first result we haven't sent an indication about and sends it
void cs_indicate_ranging_data_ready(const uint8_t conn_handle)
{
  connection_ctx_t *conn_ctx = cs_rcm_get_connection_ctx(conn_handle);
  if (conn_ctx == NULL) {
    reflector_log_warning("R#%u - Couldn't find registered connection for handle '%u'" LOG_NL,
                          conn_handle,
                          conn_handle);
    return;
  }
  // If the RAS Control Point notifications/indications are enabled and not busy,
  // and the RAS Periodic Notification was not requested
  if (!conn_ctx->ras_control_point_indication_enabled
      || conn_ctx->indication_in_progress
      || conn_ctx->ras_periodic_notification_mode_enabled) {
    return;
  }

  // Get the next result we haven't indicated about
  cs_event_data_t *current_event = cs_reflector_event_buf_get_next_unindicated(conn_handle);
  // Return if there's no result we haven't indicated about
  if (current_event == NULL) {
    return;
  }

  // Prepare the indication data
  ras_ranging_data_ready_indication_t cmd;
  cmd.opcode = CS_RAS_OPCODE_RANGING_DATA_READY_INDICATION;
  cmd.procedure_index = current_event->procedure_index;
  cmd.number_of_subevents = 1u;
  cmd.subevent_index = current_event->subevent_index;
  cmd.subevent_index_data_size = current_event->data_len;
  reflector_log_info("R#%u - Sending RAS Ranging Data Ready indication; "
                     "index='%u' num_se='%u' se_idx='%u' size='%u'" LOG_NL,
                     conn_handle,
                     cmd.procedure_index,
                     cmd.number_of_subevents,
                     cmd.subevent_index,
                     cmd.subevent_index_data_size);

  // Send the notification
  sl_status_t sc = sl_bt_gatt_server_send_indication(conn_handle,
                                                     gattdb_ras_control_point_chr_handle,
                                                     sizeof(ras_ranging_data_ready_indication_t),
                                                     (uint8_t*)&cmd);
  if (sc != SL_STATUS_OK) {
    reflector_log_warning("R#%u - Failed to send RAS Ranging Data Ready indication" LOG_NL,
                          conn_handle);
  } else {
    current_event->indication_sent = true;
    conn_ctx->indication_in_progress = true;
  }
}

static void cs_reflector_gattdb_init(void)
{
  // Look up the 'RAS control point' and the 'RAS subevent ranging data' characteristic handles from the GATT database
  sl_status_t sc;
  static const uint8_t ras_control_point_uuid[] = RAS_CONTROL_POINT_CHARACTERISTIC_UUID;
  sc =  sl_bt_gatt_server_find_attribute(0u,
                                         sizeof(ras_control_point_uuid),
                                         ras_control_point_uuid,
                                         &gattdb_ras_control_point_chr_handle);
  app_assert_status(sc);

  static const uint8_t ras_subevent_ranging_data_uuid[] = RAS_SE_RANGING_DATA_CHARACTERISTIC_UUID;
  sc =  sl_bt_gatt_server_find_attribute(0u,
                                         sizeof(ras_subevent_ranging_data_uuid),
                                         ras_subevent_ranging_data_uuid,
                                         &gattdb_ras_subevent_ranging_data_chr_handle);
  app_assert_status(sc);
}
