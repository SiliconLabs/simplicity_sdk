/***************************************************************************//**
 * @file
 * @brief CS initiator logic.
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

// -----------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#include "sl_bt_api.h"
#include "sl_component_catalog.h"
#include "sl_common.h"
#include "sl_sleeptimer.h"
#include "cs_initiator.h"
#include "cs_initiator_configurator.h"
#include "cs_ras.h"
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
#include "app_timer.h"
#endif // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)

#include "sl_rtl_clib_api.h"

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

// -----------------------------------------------------------------------------
// Macros

// Component logging
#if defined(SL_CATALOG_APP_LOG_PRESENT) && CS_INITIATOR_LOG
#define LOG_PREFIX                  "[initiator] "
#define LOG_NL                      APP_LOG_NL
#define initiator_log_debug(...)    app_log_debug(LOG_PREFIX  __VA_ARGS__)
#define initiator_log_info(...)     app_log_info(LOG_PREFIX  __VA_ARGS__)
#define initiator_log_warning(...)  app_log_warning(LOG_PREFIX  __VA_ARGS__)
#define initiator_log_error(...)    app_log_error(LOG_PREFIX  __VA_ARGS__)
#define initiator_log_critical(...) app_log_critical(LOG_PREFIX  __VA_ARGS__)
#else
#define LOG_NL
#define initiator_log_debug(...)
#define initiator_log_info(...)
#define initiator_log_warning(...)
#define initiator_log_error(...)
#define initiator_log_critical(...)
#endif // defined(SL_CATALOG_APP_LOG_PRESENT) && CS_INITIATOR_LOG

// This configuration is for future use only
#define CONFIG_RAS_REQUEST_PERIODIC_NOTIFICATION_MODE true

#define CS_CHANNEL_MAP_MIN_CHANNELS    15
#define CS_CHANNEL_MAP_MAX_CHANNELS    76

// -----------------------------------------------------------------------------
// Enums, structs, typedefs

typedef enum {
  CS_TRANSPORT_INIT_STATE = 0,
  CS_TRANSPORT_DISCOVER_RAS_SERVICE,
  CS_TRANSPORT_DISCOVER_RAS_CHARACTERISTICS,
  CS_TRANSPORT_SUBSCRIBE_RAS_SUBEVENT_RANGING_DATA,
  CS_TRANSPORT_SUBSCRIBE_RAS_CONTROL_POINT,
  CS_TRANSPORT_STATE_IDLE
} action_t;

typedef struct {
  sl_rtl_cs_subevent_data subevent_data[CS_INITIATOR_MAX_SUBEVENT_PER_PROC];
  uint8_t step_data[CS_INITIATOR_MAX_STEP_DATA_LEN];
  uint16_t step_index;
  uint16_t subevent_index;
  bool ready;
} cs_data_t;

// CS initiator measurement buffer
typedef struct {
  cs_data_t initiator_data;
  cs_data_t reflector_data;
  sl_rtl_cs_procedure cs_procedure;
  rtl_config_t rtl_config;
  cs_initiator_config_t config;
  uint16_t current_cs_subevent_index;  // Associated with ongoing CS measurement
  uint16_t current_procedure_index;    // Associated with ongoing CS measurement
  bool procedure_enable_retry;
  bool procedure_reset_request;
  uint8_t conn_handle;
  uint32_t ras_service_handle;
  struct {
    uint16_t ras_control_point;
    uint16_t ras_procedure_enable_data;
    uint16_t ras_subevent_ranging_data;
  } characteristic_handle;
  struct {
    action_t action;
    struct {
      bool ras_control_point;
      bool ras_procedure_enable_data;
      bool ras_subevent_ranging_data;
      bool ras_all;
    } characteristic_found;
  } discovery_state;
  sl_rtl_cs_libitem rtl_handle;
  cs_result_cb_t result_cb;
  cs_error_cb_t error_cb;
  uint32_t procedure_start_time_ms;
  uint32_t procedure_stop_time_ms;
  uint32_t log_error_counter;
  bool deleted;
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
  app_timer_t error_timer_handle;
  bool error_timer_started;
  bool error_timer_elapsed;
#endif // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
} cs_initiator_t;

// -----------------------------------------------------------------------------
// Static function declarations

static cs_initiator_t *cs_initiator_get_instance(const uint8_t conn_handle);
static void set_procedure_initial_state(const uint8_t         conn_handle,
                                        cs_initiator_config_t *initiator_config,
                                        sl_rtl_cs_procedure   *cs_procedure);
static void initiator_start_cs_procedure(const uint8_t conn_handle);
static void initiator_restart_cs_procedure(const uint8_t conn_handle);
static sl_status_t initiator_stop_cs_procedure(const uint8_t conn_handle);
static bool gatt_procedure_complete_event(const uint8_t conn_handle,
                                          sl_bt_msg_t    *evt);
static bool check_characteristic_uuid(const uint8_t conn_handle,
                                      sl_bt_msg_t *evt);
static cs_procedure_action_t extract_device_cs_data(const uint8_t conn_handle,
                                                    cs_data_t *device_data,
                                                    sl_rtl_cs_subevent_data *rtl_subevent_data,
                                                    uint8_t *rtl_subevent_data_count,
                                                    sl_bt_evt_cs_result_t *evt);
static void show_rtl_api_call_result(const uint8_t conn_handle,
                                     enum sl_rtl_error_code err_code);
static cs_procedure_action_t calculate_distance(const uint8_t conn_handle);
static uint32_t get_num_tones_from_channel_map(const uint8_t *ch_map,
                                               const uint32_t ch_map_len);
static void reset_subevent_data(const uint8_t conn_handle);
static void on_error(const uint8_t conn_handle,
                     cs_error_event_t evt,
                     sl_status_t      sc);
static enum sl_rtl_error_code rtl_library_init(const uint8_t conn_handle,
                                               sl_rtl_cs_libitem *handle,
                                               rtl_config_t      *config,
                                               const uint8_t     cs_mode);
static void init_cs_configuration(const uint8_t conn_handle);
static void start_error_timer(const uint8_t conn_handle);
static void stop_error_timer(const uint8_t conn_handle);

#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
static void error_timer_cb(app_timer_t *handle, void *data);
#endif // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
// -----------------------------------------------------------------------------
// Static variables
static cs_initiator_t cs_initiator_instances[CS_INITIATOR_MAX_CONNECTIONS];

static const uint8_t ras_service_uuid[] = RAS_SERVICE_UUID;
static const uint8_t ras_control_point_characteristic_uuid[] =
  RAS_CONTROL_POINT_CHARACTERISTIC_UUID;

static const uint8_t ras_procedure_enable_data_characteristic_uuid[] =
  RAS_PROCEDURE_ENABLE_DATA_CHARACTERISTIC_UUID;

static const uint8_t ras_se_ranging_data_characteristic_uuid[] =
  RAS_SE_RANGING_DATA_CHARACTERISTIC_UUID;

// -----------------------------------------------------------------------------
// Private function definitions

/******************************************************************************
 * Get pointer to initiator instance based on the connection handle.
 *****************************************************************************/
static cs_initiator_t *cs_initiator_get_instance(const uint8_t conn_handle)
{
  for (uint8_t i = 0u; i < CS_INITIATOR_MAX_CONNECTIONS; i++) {
    if (cs_initiator_instances[i].conn_handle == conn_handle) {
      initiator_log_debug("#%u recognized." LOG_NL, conn_handle);
      return &cs_initiator_instances[i];
    }
  }
  initiator_log_error("#%u no matching instance found!" LOG_NL, conn_handle);
  return NULL;
}

/******************************************************************************
 * CS initiator set initial config.
 *****************************************************************************/
static void set_procedure_initial_state(const uint8_t         conn_handle,
                                        cs_initiator_config_t *initiator_config,
                                        sl_rtl_cs_procedure   *cs_procedure)
{
  initiator_log_debug("#%u: CS procedure - configuration - begin" LOG_NL,
                      conn_handle);
  cs_procedure->cs_config.ch3c_jump = initiator_config->ch3c_jump;
  cs_procedure->cs_config.ch3c_shape = initiator_config->ch3c_shape;
  initiator_log_debug("#%u: ch3c_jump=%u, ch3c_shape=%u" LOG_NL,
                      conn_handle,
                      cs_procedure->cs_config.ch3c_jump,
                      cs_procedure->cs_config.ch3c_shape);

  cs_procedure->cs_config.channel_map_repetition =
    initiator_config->channel_map_repetition;
  cs_procedure->cs_config.cs_sync_phy = (cs_phy_t)initiator_config->phy;
  initiator_log_debug("#%u: channel_map_repetition=%u, cs_sync_phy=%u" LOG_NL,
                      conn_handle,
                      cs_procedure->cs_config.channel_map_repetition,
                      cs_procedure->cs_config.cs_sync_phy);

  cs_procedure->cs_config.main_mode_repetition =
    initiator_config->main_mode_repetition;
  cs_procedure->cs_config.procedure_count = 1;
  initiator_log_debug("#%u: main_mode_repetition=%lu,  procedure_count=%u" LOG_NL,
                      conn_handle,
                      (unsigned long)cs_procedure->cs_config.main_mode_repetition,
                      cs_procedure->cs_config.procedure_count);

  cs_procedure->cs_config.rtt_type = initiator_config->rtt_type;
  cs_procedure->cs_config.subevents_per_event = 1;
  cs_procedure->cs_config.num_antenna_paths = 1;
  initiator_log_debug("#%u: rtt_type=%u,  subevents_per_event=%u,"
                      " num_antenna_paths=%u" LOG_NL,
                      conn_handle,
                      cs_procedure->cs_config.rtt_type,
                      cs_procedure->cs_config.subevents_per_event,
                      cs_procedure->cs_config.num_antenna_paths);

  initiator_log_debug("#%u: CS procedure - configuration - end" LOG_NL,
                      conn_handle);
}

/******************************************************************************
 * CS initiator trigger start channel-sounding procedure
 *****************************************************************************/
static void initiator_start_cs_procedure(const uint8_t conn_handle)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL, conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_CS_PROCEDURE_START_FAILED,
             SL_STATUS_NULL_POINTER);
    return;
  }

  initiator_log_debug("#%u: CS procedure - request to start." LOG_NL,
                      initiator->conn_handle);
  sl_status_t sc = sl_bt_cs_procedure_enable(initiator->conn_handle,
                                             sl_bt_cs_procedure_state_enabled,
                                             initiator->config.config_id);
  if (sc == SL_STATUS_OK) {
    initiator_log_info("#%u: CS procedure - start requested." LOG_NL,
                       initiator->conn_handle);
  } else if (sc == SL_STATUS_BT_CTRL_LL_PROCEDURE_COLLISION) {
    initiator_log_warning("CS procedure start already in progress" LOG_NL);
  } else {
    initiator_log_warning("#%u: CS procedure - start failed! [sc: 0x%lx] "
                          "Retrying ..." LOG_NL,
                          initiator->conn_handle,
                          (unsigned long)sc);
    initiator->procedure_enable_retry = true;
  }
}

/******************************************************************************
 * CS initiator trigger restart channel-sounding procedure
 *****************************************************************************/
static void initiator_restart_cs_procedure(const uint8_t conn_handle)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL, conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_CS_PROCEDURE_RESTART_FAILED,
             SL_STATUS_NULL_POINTER);
    return;
  }
  // If the application controls the procedure start (max_procedure_count != 0)
  // we need to restart it manually, otherwise the stack will take care of it
  if (initiator->config.max_procedure_count != 0) {
    initiator_log_info("#%u: CS procedure - manual restart triggered" LOG_NL,
                       initiator->conn_handle);
    initiator_start_cs_procedure(initiator->conn_handle);
  }
}

/******************************************************************************
 * Stop Channel Sounding procedure
 *****************************************************************************/
static sl_status_t initiator_stop_cs_procedure(const uint8_t conn_handle)
{
  sl_status_t sc;
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL, conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_CS_PROCEDURE_STOP_FAILED,
             SL_STATUS_NULL_POINTER);
    return SL_STATUS_INVALID_HANDLE;
  }

  initiator_log_debug("#%u: CS procedure - requesting stop" LOG_NL,
                      initiator->conn_handle);
  sc = sl_bt_cs_procedure_enable(initiator->conn_handle,
                                 sl_bt_cs_procedure_state_disabled,
                                 initiator->config.config_id);
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  initiator_log_info("#%u: CS procedure - stop requested" LOG_NL, initiator->conn_handle);
  return SL_STATUS_OK;
}

/******************************************************************************
 * State machine dedicated for the discovery process:
 * discovering services on reflector side and checking the characteristics.
 * Once everything is compatible, triggers the configuration of this initiator
 * device.
 *****************************************************************************/
static bool gatt_procedure_complete_event(const uint8_t conn_handle,
                                          sl_bt_msg_t    *evt)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL, conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_CS_PROCEDURE_COMPLETE_FAILED,
             SL_STATUS_NULL_POINTER);
  }
  bool handled = false;
  uint8_t evt_conn_handle = evt->data.evt_gatt_procedure_completed.connection;
  if (evt_conn_handle != initiator->conn_handle) {
    return handled;
  }
  uint16_t procedure_result =  evt->data.evt_gatt_procedure_completed.result;

  sl_status_t sc;

  if (procedure_result != SL_STATUS_OK) {
    initiator_log_error("#%u: GATT procedure failed" LOG_NL,
                        initiator->conn_handle);
    if (initiator->discovery_state.action != CS_TRANSPORT_STATE_IDLE) {
      on_error(initiator->conn_handle,
               CS_ERROR_EVENT_GATT_PROCEDURE_FAILED,
               procedure_result);
    }

    return handled;
  }

  switch (initiator->discovery_state.action) {
    case CS_TRANSPORT_DISCOVER_RAS_SERVICE:
      handled = true;
      initiator_log_info("#%u: RAS - service discovered." LOG_NL,
                         initiator->conn_handle);
      // Service discovery successful, start characteristic discovery
      sc = sl_bt_gatt_discover_characteristics(initiator->conn_handle,
                                               initiator->ras_service_handle);
      if (sc != SL_STATUS_OK) {
        initiator_log_error("#%u: RAS - starting characteristic discovery failed!" LOG_NL,
                            initiator->conn_handle);
        on_error(initiator->conn_handle,
                 CS_ERROR_EVENT_START_CHARACTERISTIC_DISCOVERY_FAILED,
                 sc);
        break;
      }
      // restart error timer after state transition
      start_error_timer(initiator->conn_handle);
      initiator->discovery_state.action = CS_TRANSPORT_DISCOVER_RAS_CHARACTERISTICS;
      initiator_log_info("#%u: RAS - discover characteristics start" LOG_NL,
                         initiator->conn_handle);
      break;

    case CS_TRANSPORT_DISCOVER_RAS_CHARACTERISTICS:
      handled = true;
      if (!initiator->discovery_state.characteristic_found.ras_all) {
        break;
      }
      // Subscribe for notifications on the RAS Subevent Ranging Data
      sc = sl_bt_gatt_set_characteristic_notification(initiator->conn_handle,
                                                      initiator->characteristic_handle.ras_subevent_ranging_data,
                                                      sl_bt_gatt_notification);
      if (sc != SL_STATUS_OK) {
        initiator_log_error("#%u: RAS - subscribe to \'Subevent Ranging Data\' failed!" LOG_NL,
                            initiator->conn_handle);
        on_error(initiator->conn_handle,
                 CS_ERROR_EVENT_SEND_NOTIFICATION_FAILED,
                 sc);
        break;
      }
      // restart error timer after state transition
      start_error_timer(initiator->conn_handle);
      initiator->discovery_state.action = CS_TRANSPORT_SUBSCRIBE_RAS_SUBEVENT_RANGING_DATA;
      break;

    case CS_TRANSPORT_SUBSCRIBE_RAS_SUBEVENT_RANGING_DATA:
      handled = true;
      initiator_log_info("#%u: RAS - subscribed to \'Subevent Ranging Data\' notifications" LOG_NL,
                         initiator->conn_handle);
      // Subscribe for indications on the RAS Control Point
      sc = sl_bt_gatt_set_characteristic_notification(initiator->conn_handle,
                                                      initiator->characteristic_handle.ras_control_point,
                                                      sl_bt_gatt_indication);
      if (sc != SL_STATUS_OK) {
        initiator_log_error("#%u: RAS - subscribe to \'Control Point\' failed!" LOG_NL,
                            initiator->conn_handle);
        on_error(initiator->conn_handle,
                 CS_ERROR_EVENT_SEND_INDICATION_FAILED,
                 sc);
        break;
      }
      // restart error timer after state transition
      start_error_timer(initiator->conn_handle);
      initiator->discovery_state.action = CS_TRANSPORT_SUBSCRIBE_RAS_CONTROL_POINT;
      break;

    case CS_TRANSPORT_SUBSCRIBE_RAS_CONTROL_POINT:
      handled = true;
      initiator_log_info("#%u: RAS - subscribed to \'Control Point\' notifications." LOG_NL,
                         initiator->conn_handle);
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
      if (initiator->error_timer_started) {
        stop_error_timer(initiator->conn_handle);
      }
#endif
      initiator_start_cs_procedure(initiator->conn_handle);

      initiator->discovery_state.action = CS_TRANSPORT_STATE_IDLE;
      break;

    case CS_TRANSPORT_STATE_IDLE:
      break;

    default:
      break;
  }
  return handled;
}

/******************************************************************************
 * Check if the detected characteristic matches the UUIDs that
 * is required for the CS functionality
 *****************************************************************************/
static bool check_characteristic_uuid(const uint8_t conn_handle,
                                      sl_bt_msg_t *evt)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  bool handled = false;
  if (evt->data.evt_gatt_characteristic.uuid.len == UUID_LEN) {
    if (memcmp(ras_control_point_characteristic_uuid,
               evt->data.evt_gatt_characteristic.uuid.data,
               UUID_LEN) == 0) {
      initiator->characteristic_handle.ras_control_point =
        evt->data.evt_gatt_characteristic.characteristic;
      initiator->discovery_state.characteristic_found.ras_control_point = true;
      initiator_log_info("#%u: RAS discovered Control Point characteristic" LOG_NL,
                         initiator->conn_handle);
      start_error_timer(initiator->conn_handle);
      handled = true;
    }

    if (memcmp(ras_procedure_enable_data_characteristic_uuid,
               evt->data.evt_gatt_characteristic.uuid.data,
               UUID_LEN) == 0) {
      initiator->characteristic_handle.ras_procedure_enable_data =
        evt->data.evt_gatt_characteristic.characteristic;
      initiator->discovery_state.characteristic_found.ras_procedure_enable_data = true;
      initiator_log_info("#%u: RAS discovered Procedure Enable Data characteristic" LOG_NL,
                         initiator->conn_handle);
      start_error_timer(initiator->conn_handle);
      handled = true;
    }

    if (memcmp(ras_se_ranging_data_characteristic_uuid,
               evt->data.evt_gatt_characteristic.uuid.data,
               UUID_LEN) == 0) {
      initiator->characteristic_handle.ras_subevent_ranging_data =
        evt->data.evt_gatt_characteristic.characteristic;
      initiator->discovery_state.characteristic_found.ras_subevent_ranging_data = true;
      initiator_log_info("#%u: RAS discovered Subevent Ranging Data characteristic" LOG_NL,
                         initiator->conn_handle);
      start_error_timer(initiator->conn_handle);
      handled = true;
    }

    if (initiator->discovery_state.characteristic_found.ras_control_point
        && initiator->discovery_state.characteristic_found.ras_procedure_enable_data
        && initiator->discovery_state.characteristic_found.ras_subevent_ranging_data) {
      initiator->discovery_state.characteristic_found.ras_all = true;
      initiator_log_info("#%u: RAS all characteristics have been discovered successfully" LOG_NL,
                         initiator->conn_handle);
    }
  }
  return handled;
}

/******************************************************************************
 * Extract cs results (step data, subevent data) and hand over to RTL Lib
 * instance to calculate distance out of it.
 *
 * @return true if procedure restart required, false otherwise
 *****************************************************************************/
cs_procedure_action_t extract_cs_result_data(const uint8_t conn_handle,
                                             bool initiator_part,
                                             sl_bt_evt_cs_result_t *evt)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  cs_procedure_action_t procedure_action = CS_PROCEDURE_ACTION_TRIGGER_STOP;
  if (initiator == NULL) {
    initiator_log_error("instance NULL reference! Can not extract event data for handle %u!" LOG_NL,
                        evt->connection);
    return procedure_action;
  }

  if (initiator_part) {
    initiator_log_info("#%u: extract - initiator data" LOG_NL,
                       initiator->conn_handle);
    procedure_action = extract_device_cs_data(initiator->conn_handle,
                                              &initiator->initiator_data,
                                              initiator->cs_procedure.initiator_subevent_data,
                                              &initiator->cs_procedure.initiator_subevent_data_count,
                                              evt);
  } else {
    initiator_log_info("#%u: extract - reflector data" LOG_NL,
                       initiator->conn_handle);
    procedure_action = extract_device_cs_data(initiator->conn_handle,
                                              &initiator->reflector_data,
                                              initiator->cs_procedure.reflector_subevent_data,
                                              &initiator->cs_procedure.reflector_subevent_data_count,
                                              evt);
  }

  initiator_log_info("----" LOG_NL);

  // Calculate distance if both side are ready and
  // no request to procedure reset
  if (procedure_action != CS_PROCEDURE_ACTION_TRIGGER_RESET
      && initiator->initiator_data.ready
      && initiator->reflector_data.ready) {
    initiator_log_info("#%u: extract - CS procedure #%u sub events extracted."
                       "[initiator events: %u , reflector events: %u]" LOG_NL,
                       initiator->conn_handle,
                       evt->procedure_counter,
                       initiator->cs_procedure.initiator_subevent_data_count,
                       initiator->cs_procedure.reflector_subevent_data_count
                       );
    procedure_action = calculate_distance(initiator->conn_handle);
  } else if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_RESET) {
    initiator_log_warning("#%u: extract - CS procedure #%u reset triggered " LOG_NL,
                          initiator->conn_handle,
                          evt->procedure_counter);
    reset_subevent_data(initiator->conn_handle);
    if (initiator->config.max_procedure_count > 0) {
      procedure_action = CS_PROCEDURE_ACTION_TRIGGER_START;
    }
  }
  if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_RESET) {
    initiator_log_warning("#%u: extract - CS procedure #%u reset triggered " LOG_NL,
                          initiator->conn_handle,
                          evt->procedure_counter);
    reset_subevent_data(initiator->conn_handle);
    if (initiator->config.max_procedure_count > 0) {
      procedure_action = CS_PROCEDURE_ACTION_TRIGGER_START;
    }
  }
  return procedure_action;
}

static cs_procedure_action_t extract_device_cs_data(const uint8_t conn_handle,
                                                    cs_data_t *device_data,
                                                    sl_rtl_cs_subevent_data *rtl_subevent_data,
                                                    uint8_t *rtl_subevent_data_count,
                                                    sl_bt_evt_cs_result_t *evt)
{
  cs_procedure_action_t procedure_action = CS_PROCEDURE_ACTION_TRIGGER_START;
  uint16_t subevent_idx = 0u;
  uint16_t step_idx = 0u;
  uint32_t subevent_time_tick = sl_sleeptimer_get_tick_count();
  uint32_t subevent_time_ms;
  size_t step_data_len = 0u;

  subevent_idx = device_data->subevent_index;

  if (device_data->subevent_data[subevent_idx].num_steps == 0u) {
    // assign step_data array address to the sub events' pointer element
    device_data->subevent_data[subevent_idx].step_data =
      &device_data->step_data[step_idx];

    // assign devices step_data array to the procedure data's sub event pointer element too
    rtl_subevent_data->step_data =
      device_data->subevent_data[subevent_idx].step_data;
  }

  subevent_time_ms = sl_sleeptimer_tick_to_ms(subevent_time_tick);
  initiator_log_info("#%u: extract @ %lu ms (%lu ticks)" LOG_NL,
                     conn_handle,
                     (unsigned long)subevent_time_ms,
                     (unsigned long)subevent_time_tick);

  // set timestamp and freq. compensation data
  device_data->subevent_data[subevent_idx].subevent_timestamp_ms =
    subevent_time_ms;
  device_data->subevent_data[subevent_idx].frequency_compensation =
    evt->frequency_compensation;

  // check step data size
  step_idx = device_data->step_index;
  step_data_len = step_idx + evt->data.len;

  if (step_data_len > CS_INITIATOR_MAX_STEP_DATA_LEN) {
    initiator_log_error("#%u: extract - incoming step data exceeds step data "
                        " buffer size: %u/%u!" LOG_NL,
                        conn_handle,
                        (unsigned int)step_data_len,
                        (unsigned int)CS_INITIATOR_MAX_STEP_DATA_LEN);
    reset_subevent_data(conn_handle);
    return procedure_action;
  } else {
    // copy step data from cs_results event into the device data step_data buffer
    memcpy(&device_data->step_data[step_idx], evt->data.data, evt->data.len);
    initiator_log_debug("#%u: extract - update step_index %u -> %u" LOG_NL,
                        conn_handle,
                        (unsigned int)step_idx,
                        (unsigned int)step_data_len);
    // increment step index
    step_idx += evt->data.len;
    // take back the step index
    device_data->step_index = step_idx;
    // update the number of step data bytes for logger
    device_data->subevent_data[subevent_idx].step_data_count = device_data->step_index;
    initiator_log_debug("#%u: extract - updated num_steps %u -> %u [step_data_count: %u]" LOG_NL,
                        conn_handle,
                        device_data->subevent_data[subevent_idx].num_steps,
                        (device_data->subevent_data[subevent_idx].num_steps + evt->num_steps),
                        device_data->subevent_data[subevent_idx].step_data_count);

    // take over number of steps from cs_results event
    device_data->subevent_data[subevent_idx].num_steps += evt->num_steps;
  }

  // --------------------------------
  // Subevent state check
  if (evt->subevent_done_status == sl_bt_cs_done_status_complete) {
    // subevent arrived - increment data counter
    (*rtl_subevent_data_count)++;

    if (device_data->subevent_index < CS_INITIATOR_MAX_SUBEVENT_PER_PROC) {
      // update subevent index for the device
      device_data->subevent_index++;

      initiator_log_debug("#%u: extract - procedure #%u sub events %u/%u "
                          "extracted [rtl_subevent_data_count=%u]" LOG_NL,
                          conn_handle,
                          evt->procedure_counter,
                          device_data->subevent_index,
                          CS_INITIATOR_MAX_SUBEVENT_PER_PROC,
                          *rtl_subevent_data_count);
    } else {
      reset_subevent_data(conn_handle);
      initiator_log_error("#%u: extract - sub event data buffer is full! "
                          "Data loss inevitable!" LOG_NL,
                          conn_handle);
      return procedure_action;
    }
  } else if (evt->subevent_done_status == sl_bt_cs_done_status_partial_results_continue) {
    initiator_log_debug("#%u: extract - sub event partially arrived." LOG_NL,
                        conn_handle);
    procedure_action = CS_PROCEDURE_ACTION_CONTINUE;
  } else if (evt->subevent_done_status == sl_bt_cs_done_status_aborted) {
    initiator_log_warning("#%u: extract - procedure #%u sub event aborted!"
                          "[reason: 0x%X]" LOG_NL,
                          conn_handle,
                          evt->procedure_counter,
                          evt->abort_reason);
    reset_subevent_data(conn_handle);
    procedure_action = CS_PROCEDURE_ACTION_TRIGGER_RESET;
  }
  // --------------------------------
  // Procedure state check
  if (evt->procedure_done_status == sl_bt_cs_done_status_complete) {
    initiator_log_info("#%u: extract - device ready." LOG_NL,
                       conn_handle);

    // reset indexes in order to prepare to the new procedure
    device_data->subevent_index = 0u;
    device_data->step_index = 0u;
    device_data->ready = true;
    procedure_action = CS_PROCEDURE_ACTION_CONTINUE;
  } else if (evt->procedure_done_status == sl_bt_cs_done_status_partial_results_continue) {
    initiator_log_debug("#%u: extract - procedure #%u partially done." LOG_NL,
                        conn_handle,
                        evt->procedure_counter);
    procedure_action = CS_PROCEDURE_ACTION_CONTINUE;
  } else if (evt->procedure_done_status == sl_bt_cs_done_status_aborted) {
    initiator_log_warning("#%u: extract - procedure #%u aborted! "
                          "[reason: 0x%X]" LOG_NL,
                          conn_handle,
                          evt->procedure_counter,
                          evt->abort_reason);
    reset_subevent_data(conn_handle);
    procedure_action = CS_PROCEDURE_ACTION_TRIGGER_RESET;
  }
  return procedure_action;
}

/******************************************************************************
 * Show error messages based on RTL API call error codes.
 *
 * @param[in] conn_handle connection handle if the selected initiator instance.
 * @param[in] err_code RTL API error code.
 *****************************************************************************/
static void show_rtl_api_call_result(const uint8_t conn_handle,
                                     enum sl_rtl_error_code err_code)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    return;
  }

  switch (err_code) {
    case SL_RTL_ERROR_SUCCESS:
      initiator_log_debug("#%u: RTL - no error." LOG_NL, initiator->conn_handle);
      break;

    case SL_RTL_ERROR_ARGUMENT:
      initiator_log_error("#%u: RTL - invalid argument! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_OUT_OF_MEMORY:
      initiator_log_error("#%u: RTL - memory allocation error! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_ESTIMATION_IN_PROGRESS:
      initiator_log_info("#%u: RTL - estimation not yet finished! [E:%u]" LOG_NL,
                         initiator->conn_handle,
                         err_code);
      break;

    case SL_RTL_ERROR_NUMBER_OF_SNAPHOTS_DO_NOT_MATCH:
      initiator_log_error("#%u: RTL - initialized and calculated "
                          "snapshots do not match! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_ESTIMATOR_NOT_CREATED:
      initiator_log_error("#%u: RTL - estimator not created! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_ESTIMATOR_ALREADY_CREATED:
      initiator_log_error("#%u: RTL - estimator already created! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_NOT_INITIALIZED:
      initiator_log_error("#%u: RTL - library item not initialized! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_INTERNAL:
      initiator_log_error("#%u: RTL - internal error! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_IQ_SAMPLE_QA:
      initiator_log_error("#%u: RTL - IQ sample quality analysis failed! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_FEATURE_NOT_SUPPORTED:
      initiator_log_error("#%u: RTL - feature not supported! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    case SL_RTL_ERROR_INCORRECT_MEASUREMENT:
      initiator_log_error("#%u: RTL - incorrect measurement! Error of the last"
                          " measurement was too large! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;

    default:
      initiator_log_error("#%u: RTL - unknown error! [E:%u]" LOG_NL,
                          initiator->conn_handle,
                          err_code);
      break;
  }
}

/******************************************************************************
 * Calculate distance between initiator and reflector using RTL library.
 *
 * @param[in] initiator Initiator instance.
 * @return A boolean value if restart is needed.
 *****************************************************************************/
static cs_procedure_action_t calculate_distance(const uint8_t conn_handle)
{
  cs_procedure_action_t procedure_action = CS_PROCEDURE_ACTION_TRIGGER_STOP;
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!"
                        "Trigger stopping ongoing CS procedure!" LOG_NL,
                        conn_handle);
    // Stop the procedure!
    return procedure_action;
  }

  cs_result_t result;
  enum sl_rtl_error_code rtl_err = SL_RTL_ERROR_NOT_INITIALIZED;

  bool estimation_valid = false;
  result.connection = initiator->conn_handle;
  result.distance = 0.0f;
  result.likeliness = 0.0f;
  result.rssi_distance = 0.0f;
  result.cs_bit_error_rate = 0u;

  sl_rtl_cs_algorithm_param param;

  initiator_log_info("#%u: RTL - process CS results" LOG_NL,
                     initiator->conn_handle);
  // procedure count is always 1.
  rtl_err = sl_rtl_cs_process(&initiator->rtl_handle,
                              1,
                              &initiator->cs_procedure);
  show_rtl_api_call_result(initiator->conn_handle, rtl_err);
  if (rtl_err != SL_RTL_ERROR_SUCCESS && rtl_err != SL_RTL_ERROR_ESTIMATION_IN_PROGRESS) {
    initiator_log_error("#%u: RTL - failed to process CS data; error='%u'" LOG_NL,
                        initiator->conn_handle,
                        rtl_err);
    reset_subevent_data(initiator->conn_handle);
    procedure_action = CS_PROCEDURE_ACTION_TRIGGER_RESET;
    return procedure_action;
  }

  initiator_log_info("#%u: RTL - get distance" LOG_NL,
                     initiator->conn_handle);
  rtl_err = sl_rtl_cs_get_distance_estimate(&initiator->rtl_handle,
                                            SL_RTL_DISTANCE_ESTIMATE_TYPE_FILTERED,
                                            &result.distance);
  show_rtl_api_call_result(initiator->conn_handle, rtl_err);
  if (rtl_err == SL_RTL_ERROR_SUCCESS) {
    estimation_valid = true;
  } else if (rtl_err == SL_RTL_ERROR_ESTIMATION_IN_PROGRESS) {
    initiator_log_info("#%u: RTL - estimation in progress" LOG_NL,
                       initiator->conn_handle);
  } else {
    initiator_log_error("#%u: RTL - failed to get distance data! %u" LOG_NL,
                        initiator->conn_handle,
                        rtl_err);
  }
  initiator_log_info("#%u: RTL - get distance likeliness" LOG_NL,
                     initiator->conn_handle);
  rtl_err = sl_rtl_cs_get_distance_estimate_confidence(&initiator->rtl_handle,
                                                       SL_RTL_DISTANCE_ESTIMATE_CONFIDENCE_TYPE_LIKELINESS,
                                                       &result.likeliness);
  show_rtl_api_call_result(initiator->conn_handle, rtl_err);
  if (rtl_err == SL_RTL_ERROR_SUCCESS) {
    initiator_log_info("#%u: RTL - estimation valid." LOG_NL,
                       initiator->conn_handle);
    estimation_valid = true;
  } else {
    initiator_log_error("#%u: RTL - failed to calculate distance likeliness! %u" LOG_NL,
                        initiator->conn_handle,
                        rtl_err);
  }

  rtl_err = sl_rtl_cs_get_distance_estimate(&initiator->rtl_handle,
                                            SL_RTL_DISTANCE_ESTIMATE_TYPE_RSSI,
                                            &result.rssi_distance);
  show_rtl_api_call_result(initiator->conn_handle, rtl_err);
  if (rtl_err == SL_RTL_ERROR_SUCCESS) {
    estimation_valid = true;
    param.type = LAST_KNOWN_DISTANCE;
    param.value.last_known_distance = result.distance;
    rtl_err = sl_rtl_cs_set_cs_algorithm_param(&initiator->rtl_handle, &param);
    show_rtl_api_call_result(initiator->conn_handle, rtl_err);
  } else {
    initiator_log_error("#%u: RTL - could not get RSSI distance data!" LOG_NL,
                        initiator->conn_handle);
  }

  if (estimation_valid && initiator->result_cb != NULL) {
    // Call result callback in case of successful process call
    initiator->result_cb(&result, NULL);
  }

  if (initiator->config.max_procedure_count != 0) {
    procedure_action = CS_PROCEDURE_ACTION_TRIGGER_START;
  }

  reset_subevent_data(initiator->conn_handle);
  return procedure_action;
}

/******************************************************************************
 * Get number of tones in channel map
 *
 * @param ch_map is the channel map data
 * @param ch_map_len is the channel map data length
 *****************************************************************************/
static uint32_t get_num_tones_from_channel_map(const uint8_t  *ch_map,
                                               const uint32_t ch_map_len)
{
  uint8_t current_ch_map;
  uint32_t num_hadm_channels = 0;

  if (ch_map == NULL) {
    initiator_log_error("null reference to channel map! Can not get number of tones!" LOG_NL);
    return num_hadm_channels;
  } else {
    for (uint32_t ch_map_index = 0; ch_map_index < ch_map_len; ch_map_index++) {
      current_ch_map = ch_map[ch_map_index];
      for (uint8_t current_bit_index = 0;
           current_bit_index < sizeof(uint8_t) * CHAR_BIT;
           current_bit_index++) {
        if (current_ch_map & (1 << current_bit_index)) {
          num_hadm_channels++;
        }
      }
    }
  }
  return num_hadm_channels;
}

/******************************************************************************
 * Call user error callback on error
 *
 * @param cs_initiator_t instance reference
 * @param cs_error_event_t initiator error event
 * @param sl_status_t status code
 *****************************************************************************/
static void on_error(const uint8_t conn_handle,
                     cs_error_event_t evt,
                     sl_status_t      sc)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    return;
  }

  cs_error_cb_t cb = initiator->error_cb;
  initiator_log_error("#%u: critical error occurred (sc: 0x%lx)" LOG_NL,
                      conn_handle,
                      (unsigned long)sc);
  if (cb != NULL) {
    cb(conn_handle, evt, sc);
  }
}

/******************************************************************************
 * Reset subevent data and synchronization for an initiator instance.
 *
 * @param cs_initiator_t instance reference
 *****************************************************************************/
static void reset_subevent_data(const uint8_t conn_handle)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    return;
  }

  initiator->initiator_data.step_index = 0u;
  initiator->reflector_data.step_index = 0u;

  initiator->initiator_data.subevent_index = 0u;
  initiator->reflector_data.subevent_index = 0u;

  // Assign subevent data pointer to cs_procedure subevent data pointer
  initiator->cs_procedure.initiator_subevent_data =
    initiator->initiator_data.subevent_data;
  initiator->cs_procedure.reflector_subevent_data =
    initiator->reflector_data.subevent_data;

  initiator->cs_procedure.initiator_subevent_data_count = 0u;
  initiator->cs_procedure.reflector_subevent_data_count = 0u;

  initiator->initiator_data.ready = false;
  initiator->reflector_data.ready = false;

  for (uint8_t i = 0; i < CS_INITIATOR_MAX_SUBEVENT_PER_PROC; i++) {
    initiator->initiator_data.subevent_data[i].num_steps = 0;
    // Assign step data pointer to cs_procedure step data pointer
    initiator->initiator_data.subevent_data[i].step_data =
      initiator->initiator_data.step_data;
    // Do it for the reflector side as well
    initiator->reflector_data.subevent_data[i].num_steps = 0;
    initiator->reflector_data.subevent_data[i].step_data =
      initiator->reflector_data.step_data;
  }

  initiator_log_info("#%u: subevent data reset executed." LOG_NL,
                     initiator->conn_handle);
}

/******************************************************************************
 * RTL Lib init function
 *
 * @param[in] conn_handle connection handle
 * @param[in] handle RTL Lib handle reference
 * @param[in] config RTL configuration structure reference
 * @param[in] cs_mode CS measurement mode
 *
 * @return with RTL Lib specified error code
 *****************************************************************************/
static enum sl_rtl_error_code rtl_library_init(const uint8_t conn_handle,
                                               sl_rtl_cs_libitem *handle,
                                               rtl_config_t      *config,
                                               const uint8_t     cs_mode)
{
  enum sl_rtl_error_code rtl_err;

  if (handle != NULL && *handle != NULL) {
    rtl_err = sl_rtl_cs_deinit(handle);
    if (rtl_err != SL_RTL_ERROR_SUCCESS) {
      initiator_log_error("#%u: RTL - failed to deinit lib! "
                          "[err: 0x%02x]" LOG_NL,
                          conn_handle,
                          rtl_err);
      return rtl_err;
    }
    *handle = NULL;
  }

  rtl_err = sl_rtl_cs_init(handle);
  if (rtl_err != SL_RTL_ERROR_SUCCESS) {
    initiator_log_error("#%u: RTL - failed to init lib! "
                        "[err: 0x%02x]" LOG_NL,
                        conn_handle,
                        rtl_err);
    return rtl_err;
  }

  if (config->rtl_logging_enabled) {
    sl_rtl_cs_log_params log_params;
    uint32_t time_tick = sl_sleeptimer_get_tick_count();
    log_params.timestamp.tv_sec = 0;
    log_params.timestamp.tv_nsec = 0;
    log_params.timestamp_ms = sl_sleeptimer_tick_to_ms(time_tick);
    rtl_err = sl_rtl_cs_log_enable(handle, &log_params);
    if (rtl_err != SL_RTL_ERROR_SUCCESS) {
      initiator_log_error("#%u: RTL - failed to enable log! "
                          "[err: 0x%02x]" LOG_NL,
                          conn_handle,
                          rtl_err);
      return rtl_err;
    }
  }

  rtl_err = sl_rtl_cs_set_algo_mode(handle, config->algo_mode);
  if (rtl_err != SL_RTL_ERROR_SUCCESS) {
    initiator_log_error("#%u: RTL - set algo mode failed! "
                        "[err: 0x%02x]" LOG_NL,
                        conn_handle,
                        rtl_err);
    return rtl_err;
  }

  switch (cs_mode) {
    case CS_INITIATOR_MODE_RTT:
      initiator_log_info("#%u: RTL -  set CS mode: RTT" LOG_NL, conn_handle);
      rtl_err = sl_rtl_cs_set_cs_mode(handle, SL_RTL_CS_MODE_RTT);
      if (rtl_err != SL_RTL_ERROR_SUCCESS) {
        initiator_log_error("#%u: RTL - failed to set CS mode! "
                            "[err: 0x%02x]" LOG_NL,
                            conn_handle,
                            rtl_err);
        return rtl_err;
      }
      break;
    case CS_INITIATOR_MODE_PBR:
    {
      config->cs_parameters.max_number_of_frequencies =
        (uint32_t)DEFAULT_NUM_TONES;
      config->cs_parameters.delta_f =
        (uint32_t)DEFAULT_FREQUENCY_DELTA;

      initiator_log_info("#%u: RTL - set CS mode: PBR" LOG_NL, conn_handle);
      rtl_err = sl_rtl_cs_set_cs_mode(handle, SL_RTL_CS_MODE_PBR);
      if (rtl_err != SL_RTL_ERROR_SUCCESS) {
        initiator_log_error("#%u: RTL - failed to set CS mode! "
                            "[err: 0x%02x]" LOG_NL,
                            conn_handle,
                            rtl_err);
        return rtl_err;
      }
      initiator_log_debug("#%u: RTL - CS mode set. Set CS parameters "
                          "[max_number_of_frequencies=%lu, delta_f=%lu]" LOG_NL,
                          conn_handle,
                          (unsigned long)config->cs_parameters.max_number_of_frequencies,
                          (unsigned long)config->cs_parameters.delta_f);
      rtl_err = sl_rtl_cs_set_cs_parameters(handle, &config->cs_parameters);
      if (rtl_err != SL_RTL_ERROR_SUCCESS) {
        initiator_log_error("#%u: RTL - failed to set CS parameters! "
                            "[err: 0x%02x]" LOG_NL,
                            conn_handle,
                            rtl_err);
        return rtl_err;
      }
    }
    break;
    default:
    {
      config->cs_parameters.max_number_of_frequencies =
        (uint32_t)DEFAULT_NUM_TONES;
      config->cs_parameters.delta_f = (uint32_t)DEFAULT_FREQUENCY_DELTA;

      initiator_log_info("#%u: RTL - set CS mode to default [PBR]" LOG_NL,
                         conn_handle);
      rtl_err = sl_rtl_cs_set_cs_mode(handle, SL_RTL_CS_MODE_PBR);
      if (rtl_err != SL_RTL_ERROR_SUCCESS) {
        initiator_log_error("#%u: RTL - failed to set CS mode! "
                            "[err: 0x%02x]" LOG_NL,
                            conn_handle,
                            rtl_err);
        return rtl_err;
      }
      initiator_log_debug("#%u: RTL - CS mode set. Set CS parameters "
                          "[max_number_of_frequencies=%lu, delta_f=%lu]" LOG_NL,
                          conn_handle,
                          (unsigned long)config->cs_parameters.max_number_of_frequencies,
                          (unsigned long)config->cs_parameters.delta_f);
      rtl_err = sl_rtl_cs_set_cs_parameters(handle, &config->cs_parameters);
      if (rtl_err != SL_RTL_ERROR_SUCCESS) {
        initiator_log_error("#%u: RTL - failed to set CS parameters! "
                            "[err: 0x%02x]" LOG_NL,
                            conn_handle,
                            rtl_err);
        return rtl_err;
      }
    }
    break;
  }

  if (rtl_err == SL_RTL_ERROR_SUCCESS) {
    initiator_log_info("#%u: RTL - CS parameters set." LOG_NL, conn_handle);
  }

  initiator_log_info("#%u: RTL - create estimator" LOG_NL, conn_handle);
  rtl_err = sl_rtl_cs_create_estimator(handle);
  if (rtl_err != SL_RTL_ERROR_SUCCESS) {
    initiator_log_error("#%u: RTL - failed to create estimator! [err: 0x%02x]" LOG_NL,
                        conn_handle,
                        rtl_err);
    return rtl_err;
  } else {
    initiator_log_info("#%u: RTL - estimator created." LOG_NL, conn_handle);
  }

  return rtl_err;
}

/******************************************************************************
 * Initialize CS configuration.
 *
 * @param[in] conn_handle connection handle
 *****************************************************************************/
static void init_cs_configuration(const uint8_t conn_handle)
{
  sl_status_t sc = SL_STATUS_OK;

  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_INITIATOR_INSTANCE_NULL,
             SL_STATUS_NULL_POINTER);
    return;
  }
  cs_error_event_t initiator_err = CS_ERROR_EVENT_UNHANDLED;

  initiator_log_info("#%u: CS - set default settings" LOG_NL,
                     initiator->conn_handle);
  sc = sl_bt_cs_set_default_settings(initiator->conn_handle,
                                     sl_bt_cs_role_status_enable,
                                     sl_bt_cs_role_status_disable,
                                     initiator->config.cs_sync_antenna,
                                     initiator->config.max_tx_power_dbm);
  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: set default CS settings failed" LOG_NL,
                        initiator->conn_handle);
    initiator_err = CS_ERROR_EVENT_INITIATOR_FAILED_TO_SET_DEFAULT_CS_SETTINGS;
    on_error(initiator->conn_handle,
             initiator_err,
             sc);
    return;
  }

  initiator_log_debug("#%u: CS - enable security" LOG_NL,
                      initiator->conn_handle);

  sc = sl_bt_cs_security_enable(initiator->conn_handle);
  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: CS - security enable failed! [sc: 0x%lx]" LOG_NL,
                        initiator->conn_handle,
                        (unsigned long)sc);
    initiator_err = CS_ERROR_EVENT_INITIATOR_FAILED_TO_ENABLE_CS_SECURITY;
    on_error(initiator->conn_handle,
             initiator_err,
             sc);
    return;
  } else {
    initiator_log_info("#%u: CS - security enabled." LOG_NL,
                       initiator->conn_handle);
  }

  initiator_log_debug("#%u: CS - create configuration ..." LOG_NL,
                      initiator->conn_handle);
  sc = sl_bt_cs_create_config(initiator->conn_handle,
                              initiator->config.config_id,
                              initiator->config.create_context,
                              initiator->config.cs_mode,
                              initiator->config.submode,
                              initiator->config.min_main_mode_steps,
                              initiator->config.max_main_mode_steps,
                              initiator->config.main_mode_repetition,
                              initiator->config.mode0_step,
                              sl_bt_cs_role_initiator,
                              initiator->config.rtt_type,
                              (cs_phy_t)initiator->config.phy,
                              &initiator->config.channel_map,
                              initiator->config.channel_map_repetition,
                              initiator->config.channel_selection_type,
                              initiator->config.ch3c_shape,
                              initiator->config.ch3c_jump,
                              initiator->config.companion_signal_enable);

  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: CS - configuration create failed! [sc: 0x%lx]" LOG_NL,
                        initiator->conn_handle,
                        (unsigned long)sc);
    initiator_err = CS_ERROR_EVENT_INITIATOR_FAILED_TO_CREATE_CONFIG;
    on_error(initiator->conn_handle, initiator_err, sc);
    return;
  } else {
    initiator_log_info("#%u: CS - configuration created. " LOG_NL,
                       initiator->conn_handle);
  }

  if (sc != SL_STATUS_OK) {
    on_error(initiator->conn_handle, initiator_err, sc);
  }
}

/******************************************************************************
 * Start and restart error timer.
 *****************************************************************************/
static void start_error_timer(const uint8_t conn_handle)
{
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_INITIATOR_INSTANCE_NULL,
             SL_STATUS_NULL_POINTER);
    return;
  }
  sl_status_t sc = SL_STATUS_OK;
  if (!initiator->error_timer_elapsed) {
    if (initiator->error_timer_started) {
      app_timer_stop(&initiator->error_timer_handle);
      initiator->error_timer_started = false;
    }
    sc = app_timer_start(&initiator->error_timer_handle,
                         CS_INITIATOR_ERROR_TIMEOUT_MS,
                         error_timer_cb,
                         (void *)initiator,
                         false);
    if (sc != SL_STATUS_OK) {
      initiator_log_error("#%u: error timer could not start! "
                          "[sc: 0x%lx]" LOG_NL,
                          initiator->conn_handle,
                          (unsigned long)sc);
      on_error(initiator->conn_handle,
               CS_ERROR_EVENT_TIMER_START_ERROR,
               sc);
    } else {
      initiator_log_debug("#%u: Timer started. [%u ms]" LOG_NL,
                          initiator->conn_handle,
                          CS_INITIATOR_ERROR_TIMEOUT_MS);
      initiator->error_timer_started = true;
    }
  }
#else
  (void)conn_handle;
#endif // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
}

/******************************************************************************
 * Stop error timer.
 *****************************************************************************/
static void stop_error_timer(const uint8_t conn_handle)
{
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("#%u: unknown connection id!" LOG_NL,
                        conn_handle);
    on_error(conn_handle,
             CS_ERROR_EVENT_INITIATOR_INSTANCE_NULL,
             SL_STATUS_NULL_POINTER);
    return;
  }
  sl_status_t sc = SL_STATUS_OK;
  sc = app_timer_stop(&initiator->error_timer_handle);
  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: could not stop the error timer! "
                        "[sc: 0x%lx]" LOG_NL,
                        initiator->conn_handle,
                        (unsigned long)sc);
    on_error(initiator->conn_handle,
             CS_ERROR_EVENT_TIMER_STOP_ERROR,
             sc);
    return;
  }
  initiator->error_timer_started = false;
  initiator->error_timer_elapsed = false;
  initiator_log_debug("#%u: Timer stopped." LOG_NL,
                      initiator->conn_handle);
#else
  (void)conn_handle;
#endif  // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
}

// -----------------------------------------------------------------------------
// Public function definitions

/******************************************************************************
 * Check if a connection handle is assigned already to an instance or not.
 *****************************************************************************/
bool cs_initiator_identify(const uint8_t conn_handle)
{
  return (cs_initiator_get_instance(conn_handle) != NULL);
}

/******************************************************************************
 * Init function that sets the first discovery and initiator config
 * and register the selected callback function pointer as a
 * callback for the distance measurement.
 *****************************************************************************/
sl_status_t cs_initiator_create(const uint8_t         conn_handle,
                                cs_initiator_config_t *initiator_config,
                                const rtl_config_t          *rtl_config,
                                cs_result_cb_t        result_cb,
                                cs_error_cb_t         error_cb)
{
  sl_status_t sc = SL_STATUS_OK;
  enum sl_rtl_error_code rtl_err = SL_RTL_ERROR_SUCCESS;
  uint32_t enabled_channels = 0u;
  cs_error_event_t initiator_err = CS_ERROR_EVENT_UNHANDLED;

  if (conn_handle == SL_BT_INVALID_CONNECTION_HANDLE) {
    return SL_STATUS_INVALID_HANDLE;
  }
  if (initiator_config == NULL
      || result_cb == NULL
      || error_cb == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  initiator_log_info("#?: create new initiator instance" LOG_NL);

  // Get free slot
  cs_initiator_t *initiator =
    cs_initiator_get_instance(SL_BT_INVALID_CONNECTION_HANDLE);
  if (initiator == NULL) {
    initiator_log_error("#%u: max initiator count reached! "
                        "Disconnect..." LOG_NL,
                        conn_handle);
    return SL_STATUS_FULL;
  }

  initiator_log_debug("#%u: clean-up initiator and reflector data" LOG_NL,
                      initiator->conn_handle);
  memset(&initiator->initiator_data, 0, sizeof(cs_data_t));
  memset(&initiator->reflector_data, 0, sizeof(cs_data_t));

  // Assign connection handle
  initiator->conn_handle = conn_handle;
  initiator_log_debug("#%u: load initiator configuration" LOG_NL,
                      initiator->conn_handle);
  memcpy(&initiator->config, initiator_config, sizeof(cs_initiator_config_t));
  initiator->deleted = false;

  #ifndef CS_WEB_ASSEMBLY_MODE
  sc = sl_bt_sm_increase_security(initiator->conn_handle);
  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: failed to increase security" LOG_NL,
                        initiator->conn_handle);
    initiator_err = CS_ERROR_EVENT_INITIATOR_FAILED_TO_INCREASE_SECURITY;
    goto cleanup;
  }
  #endif //CS_WEB_ASSEMBLY_MODE

  // take over RTL lib configuration into the instance
  initiator->rtl_config.cs_parameters = rtl_config->cs_parameters;
  initiator->rtl_config.rtl_logging_enabled = rtl_config->rtl_logging_enabled;

  if (initiator->config.use_moving_object_tracking) {
    // Moving target tracking
    initiator->rtl_config.algo_mode = SL_RTL_CS_ALGO_MODE_REAL_TIME_BASIC;
    initiator_log_info("#%u: Tracking mode: moving objects" LOG_NL, initiator->conn_handle);
  } else {
    // Stationary target tracking
    initiator->rtl_config.algo_mode = SL_RTL_CS_ALGO_MODE_STATIC_HIGH_ACCURACY;
    initiator_log_info("#%u: Tracking mode: stationary objects" LOG_NL, initiator->conn_handle);
  }

  set_procedure_initial_state(initiator->conn_handle,
                              &initiator->config,
                              &initiator->cs_procedure);

  initiator_log_debug("#%u: initialize discover state machine" LOG_NL,
                      initiator->conn_handle);

  initiator->result_cb = result_cb;
  initiator->error_cb = error_cb;
  initiator_log_debug("#%u: registered callbacks" LOG_NL,
                      initiator->conn_handle);

  enabled_channels =
    get_num_tones_from_channel_map(initiator->config.channel_map.data,
                                   initiator->config.channel_map_len);

  initiator_log_info("#%u: CS channel map - channel count: %lu" LOG_NL,
                     initiator->conn_handle,
                     (unsigned long)enabled_channels);

  if (enabled_channels < CS_CHANNEL_MAP_MIN_CHANNELS) {
    initiator_log_error("#%u: CS channel map - too few channels channels enabled!" LOG_NL,
                        initiator->conn_handle);
    sc = SL_STATUS_INVALID_PARAMETER;
    initiator_err = CS_ERROR_EVENT_INITIATOR_CHANNEL_MAP_TOO_FEW_CHANNELS;
    goto cleanup;
  }
  if (enabled_channels > CS_CHANNEL_MAP_MAX_CHANNELS) {
    initiator_log_error("#%u: CS channel - too many channels enabled!" LOG_NL,
                        initiator->conn_handle);
    sc = SL_STATUS_INVALID_PARAMETER;
    initiator_err = CS_ERROR_EVENT_INITIATOR_CHANNEL_MAP_TOO_MANY_CHANNELS;
    goto cleanup;
  }

  #ifndef CS_WEB_ASSEMBLY_MODE
  // Request connection parameter update.
  sc = sl_bt_connection_set_parameters(initiator->conn_handle,
                                       initiator->config.min_connection_interval,
                                       initiator->config.max_connection_interval,
                                       initiator->config.latency,
                                       initiator->config.timeout,
                                       initiator->config.min_ce_length,
                                       initiator->config.max_ce_length);
  if (sc != SL_STATUS_OK) {
    initiator_log_error("#%u: CS - failed to set connection parameters! "
                        "[sc: 0x%04lx]" LOG_NL,
                        initiator->conn_handle,
                        (unsigned long)sc);
    initiator_err =
      CS_ERROR_EVENT_INITIATOR_FAILED_TO_SET_CONNECTION_PARAMETERS;
    goto cleanup;
  }
  #endif //CS_WEB_ASSEMBLY_MODE

  initiator_log_debug("#%u: CS - set connection parameters ..." LOG_NL,
                      initiator->conn_handle);
  initiator->discovery_state.characteristic_found.ras_control_point = false;
  initiator->discovery_state.characteristic_found.ras_procedure_enable_data = false;
  initiator->discovery_state.characteristic_found.ras_subevent_ranging_data = false;
  initiator->discovery_state.characteristic_found.ras_all = false;

  start_error_timer(initiator->conn_handle);
  // trying to initialize RTL lib within error-timeout
  initiator_log_debug("#%u: RTL - initialize lib item" LOG_NL,
                      initiator->conn_handle);
  rtl_err = rtl_library_init(initiator->conn_handle,
                             &initiator->rtl_handle,
                             &initiator->rtl_config,
                             initiator->config.cs_mode);
  if (rtl_err != SL_RTL_ERROR_SUCCESS) {
    initiator_log_error("#%u: RTL - failed to init lib item! [err: 0x%02x]" LOG_NL,
                        initiator->conn_handle,
                        rtl_err);
    initiator_err = CS_ERROR_EVENT_INITIATOR_FAILED_TO_INIT_RTL_LIB;
  }
  stop_error_timer(initiator->conn_handle);
  initiator_log_info("#%u: RTL - lib item initialized." LOG_NL,
                     initiator->conn_handle);

  cleanup:
  if (sc != SL_STATUS_OK) {
    on_error(initiator->conn_handle, initiator_err, sc);
  }
  return sc;
}

/******************************************************************************
 * Initialize instance slots.
 *****************************************************************************/
void cs_initiator_init(void)
{
  for (uint8_t i = 0u; i < CS_INITIATOR_MAX_CONNECTIONS; i++) {
    cs_initiator_t *initiator = &cs_initiator_instances[i];
    memset(initiator, 0, sizeof(cs_initiator_instances[0]));
    initiator->conn_handle = SL_BT_INVALID_CONNECTION_HANDLE;
    // Assign subevent data array start address to cs_procedure subevent data
    // pointer element.
    initiator->cs_procedure.initiator_subevent_data =
      &initiator->initiator_data.subevent_data[0];
    initiator->cs_procedure.reflector_subevent_data =
      &initiator->reflector_data.subevent_data[0];
  }
}

/******************************************************************************
 * Delete existing initiator instance after closing its connection to
 * reflector(s).
 *****************************************************************************/
sl_status_t cs_initiator_delete(const uint8_t conn_handle)
{
  enum sl_rtl_error_code rtl_err;

  if (conn_handle == SL_BT_INVALID_CONNECTION_HANDLE) {
    return SL_STATUS_INVALID_HANDLE;
  }

  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("unknown connection id #%u!" LOG_NL,
                        conn_handle);
    return SL_STATUS_NULL_POINTER;
  }
  initiator->deleted = true;

  // We ignore the return value of the stop procedure function
  // as the procedure may have been already stopped by an error
  // or by the connection being closed.
  // If an error did not happen and this is a graceful shutdown,
  // then we'll request the procedure to be stopped.
  (void)initiator_stop_cs_procedure(initiator->conn_handle);
  (void)sl_bt_cs_remove_config(initiator->conn_handle, initiator->config.config_id);

  if (initiator->rtl_handle != NULL) {
    rtl_err = sl_rtl_cs_deinit(&initiator->rtl_handle);
    if (rtl_err != SL_RTL_ERROR_SUCCESS) {
      initiator_log_error("#%u: Failed to deinit RTL lib! [err: 0x%02x]" LOG_NL,
                          initiator->conn_handle,
                          rtl_err);
      return SL_STATUS_FAIL;
    }
  }

  initiator_log_debug("#%u: deleting instance" LOG_NL,
                      initiator->conn_handle);

  initiator_log_debug("#%u: purge initiator cs_results" LOG_NL,
                      initiator->conn_handle);
  memset(&initiator->initiator_data, 0, sizeof(cs_data_t));

  initiator_log_debug("#%u: purge reflector cs_results" LOG_NL,
                      initiator->conn_handle);
  memset(&initiator->reflector_data, 0, sizeof(cs_data_t));
  initiator_log_info("#%u: instance deleted" LOG_NL, initiator->conn_handle);

  initiator->conn_handle = SL_BT_INVALID_CONNECTION_HANDLE;
  return SL_STATUS_OK;
}

/******************************************************************************
 * Deinitialization function of CS initiator.
 * Closes connections and calls up delete initiator for all existing instances.
 *****************************************************************************/
void cs_initiator_deinit(void)
{
  for (uint8_t i = 0u; i < CS_INITIATOR_MAX_CONNECTIONS; i++) {
    cs_initiator_t *initiator = &cs_initiator_instances[i];
    if (initiator->conn_handle != SL_BT_INVALID_CONNECTION_HANDLE) {
      cs_initiator_delete(initiator->conn_handle);
    }
  }
}

/******************************************************************************
 * CS initiator step function.
 *****************************************************************************/
void cs_initiator_step(void)
{
  // Try to initialize CS procedure again if it has failed
  for (uint8_t i = 0u; i < CS_INITIATOR_MAX_CONNECTIONS; i++) {
    if (cs_initiator_instances[i].conn_handle != SL_BT_INVALID_CONNECTION_HANDLE
        && cs_initiator_instances[i].procedure_enable_retry) {
      initiator_log_warning("#%u: CS procedure - retry triggered!" LOG_NL,
                            cs_initiator_instances[i].conn_handle);
      cs_initiator_instances[i].procedure_enable_retry = false;
      initiator_start_cs_procedure(cs_initiator_instances[i].conn_handle);
    }
  }
}

// -----------------------------------------------------------------------------
// Event / callback definitions

/******************************************************************************
 * Bluetooth stack event handler.
 *****************************************************************************/
bool cs_initiator_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  cs_initiator_t *initiator;
  cs_procedure_action_t procedure_action = CS_PROCEDURE_ACTION_CONTINUE;

  uint32_t time_tick = 0u;

  bool handled = false;

  switch (SL_BT_MSG_ID(evt->header)) {
    // --------------------------------
    // Connection parameters set
    case sl_bt_evt_connection_parameters_id:
      initiator = cs_initiator_get_instance(evt->data.evt_connection_parameters.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_connection_parameters_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_connection_parameters.connection);
        break;
      }

      if (evt->data.evt_connection_parameters.security_mode
          != sl_bt_connection_mode1_level1) {
        initiator_log_info("#%u: CS - connection parameters set: "
                           "encryption on. Start discovering RAS service "
                           "& characteristic ..." LOG_NL,
                           initiator->conn_handle);

        sc = sl_bt_gatt_discover_primary_services_by_uuid(initiator->conn_handle,
                                                          UUID_LEN,
                                                          ras_service_uuid);
        if (sc != SL_STATUS_OK) {
          initiator_log_error("#%u: failed to start RAS service discovery" LOG_NL,
                              initiator->conn_handle);
          on_error(initiator->conn_handle,
                   CS_ERROR_EVENT_START_SERVICE_DISCOVERY,
                   sc);

          break;
        }

        init_cs_configuration(initiator->conn_handle);
      }
      break;

    // --------------------------------
    // GATT procedure completed
    case sl_bt_evt_gatt_procedure_completed_id:
      initiator = cs_initiator_get_instance(evt->data.evt_connection_parameters.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_gatt_procedure_completed_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_connection_parameters.connection);
        break;
      }
      handled = gatt_procedure_complete_event(initiator->conn_handle, evt);
      break;

    // --------------------------------
    // New GATT characteristic discovered
    case sl_bt_evt_gatt_characteristic_id:
      initiator = cs_initiator_get_instance(evt->data.evt_gatt_service.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_gatt_characteristic_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_gatt_service.connection);
        break;
      }
      initiator_log_debug("#%u: GATT - discovered new characteristic" LOG_NL,
                          initiator->conn_handle);
      handled = check_characteristic_uuid(initiator->conn_handle, evt);
      break;

    // --------------------------------
    // New GATT service discovered
    case sl_bt_evt_gatt_service_id:
      initiator = cs_initiator_get_instance(evt->data.evt_gatt_service.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event [sl_bt_evt_gatt_service_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_gatt_service.connection);
        break;
      }
      if (evt->data.evt_gatt_service.uuid.len == UUID_LEN) {
        if (memcmp(ras_service_uuid, evt->data.evt_gatt_service.uuid.data, UUID_LEN) == 0) {
          initiator->ras_service_handle = evt->data.evt_gatt_service.service;
          initiator->discovery_state.action = CS_TRANSPORT_DISCOVER_RAS_SERVICE;
          initiator_log_debug("#%u: RAS - found service" LOG_NL,
                              initiator->conn_handle);
          start_error_timer(initiator->conn_handle);
        }
      }
      break;

    // --------------------------------
    // New GATT characteristic value received
    case sl_bt_evt_gatt_characteristic_value_id:
      initiator = cs_initiator_get_instance(evt->data.evt_gatt_characteristic_value.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_gatt_characteristic_value_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_gatt_characteristic_value.connection);
        break;
      }
      if (evt->data.evt_gatt_characteristic_value.characteristic
          == initiator->characteristic_handle.ras_control_point) {
        initiator_log_info("#%u: RAS - received new \'Control Point\' data" LOG_NL,
                           initiator->conn_handle);
        handled = true;

        ras_ranging_data_get_command_t cmd;
        sc = cs_ras_create_control_point_response(evt->data.evt_gatt_characteristic_value.value.data,
                                                  evt->data.evt_gatt_characteristic_value.value.len,
                                                  CONFIG_RAS_REQUEST_PERIODIC_NOTIFICATION_MODE,
                                                  &cmd);
        if (sc != SL_STATUS_OK) {
          initiator_log_error("#%u: RAS - failed to create "
                              "\'Control Point\' response" LOG_NL,
                              initiator->conn_handle);
          on_error(initiator->conn_handle,
                   CS_ERROR_EVENT_SEND_CONTROL_POINT_RESPONSE_FAILED,
                   sc);
          break;
        }
        sc = sl_bt_gatt_write_characteristic_value(initiator->conn_handle,
                                                   initiator->characteristic_handle.ras_control_point,
                                                   sizeof(ras_ranging_data_get_command_t),
                                                   (uint8_t*)&cmd);
        if (sc != SL_STATUS_OK) {
          initiator_log_error("#%u: RAS - failed to write characteristic value" LOG_NL,
                              initiator->conn_handle);
          on_error(initiator->conn_handle,
                   CS_ERROR_EVENT_WRITE_CHARACTERISTIC_FAILED,
                   sc);
          break;
        }

        sc = sl_bt_gatt_send_characteristic_confirmation(initiator->conn_handle);
        if (sc != SL_STATUS_OK) {
          initiator_log_error("#%u: RAS - failed to send "
                              "\'Control \'Point confirmation" LOG_NL,
                              initiator->conn_handle);
          on_error(initiator->conn_handle,
                   CS_ERROR_EVENT_SEND_CHARACTERISTIC_CONFIRMATION_FAILED,
                   sc);
          break;
        }
      }

      if (evt->data.evt_gatt_characteristic_value.characteristic
          != initiator->characteristic_handle.ras_subevent_ranging_data) {
        break;
      }
      initiator_log_info("#%u: RAS - new CS data from the reflector" LOG_NL,
                         initiator->conn_handle);

      if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_notification) {
        handled = true;
        struct sl_bt_evt_cs_result_s *result_ptr =
          (struct sl_bt_evt_cs_result_s*)evt->data.evt_gatt_characteristic_value.value.data;
        initiator_log_debug("#%u: reflector CS packet received. [proc_cnt=%u, "
                            "proc_done=%u se_done=%u len=%u]" LOG_NL,
                            initiator->conn_handle,
                            result_ptr->procedure_counter,
                            result_ptr->procedure_done_status,
                            result_ptr->subevent_done_status,
                            evt->data.evt_gatt_characteristic_value.value.len);

        procedure_action =
          extract_cs_result_data(initiator->conn_handle, false, result_ptr);

        if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_RESET) {
          initiator->procedure_reset_request = true;
        } else if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_START) {
          initiator_restart_cs_procedure(initiator->conn_handle);
        }
      }
      break;

    // --------------------------------
    // CS procedure enable action completed
    case sl_bt_evt_cs_procedure_enable_complete_id:
      initiator = cs_initiator_get_instance(evt->data.evt_cs_procedure_enable_complete.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_cs_procedure_enable_complete_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_cs_procedure_enable_complete.connection);
        break;
      }
      handled = true;

      if (evt->data.evt_cs_procedure_enable_complete.status == SL_STATUS_OK) {
        if (evt->data.evt_cs_procedure_enable_complete.state == sl_bt_cs_procedure_state_disabled) {
          time_tick = sl_sleeptimer_get_tick_count();
          initiator->procedure_stop_time_ms =
            sl_sleeptimer_tick_to_ms(time_tick);
          initiator_log_info("#%u: CS procedure - stopped @ %lu ms "
                             "(%lu tick)" LOG_NL,
                             initiator->conn_handle,
                             (unsigned long)initiator->procedure_stop_time_ms,
                             (unsigned long)time_tick
                             );
          // only in case of procedure reset
          if (initiator->procedure_reset_request) {
            initiator->procedure_reset_request = false;
            initiator_start_cs_procedure(initiator->conn_handle);
          } else {
            initiator_log_error("#%u: CS procedure - stopped because of "
                                "critical error! Instance not functional ..." LOG_NL,
                                initiator->conn_handle);
            on_error(initiator->conn_handle,
                     CS_ERROR_EVENT_CS_SET_PROCEDURE_PARAMETERS_FAILED,
                     evt->data.evt_cs_procedure_enable_complete.status);
          }
        } else {
          time_tick = sl_sleeptimer_get_tick_count();
          initiator->procedure_start_time_ms =
            sl_sleeptimer_tick_to_ms(time_tick);

          initiator->cs_procedure.cs_config.subevent_len
            = evt->data.evt_cs_procedure_enable_complete.subevent_len;
          initiator->cs_procedure.cs_config.subevent_interval =
            evt->data.evt_cs_procedure_enable_complete.subevent_interval;
          initiator_log_info("#%u: -------------------------------------------" LOG_NL,
                             initiator->conn_handle);
          initiator_log_info("#%u: CS procedure - started @ %lu ms (%lu tick) "
                             "subevent_len: %lu, "
                             "subevent_interval: %u]" LOG_NL,
                             initiator->conn_handle,
                             (unsigned long)initiator->procedure_start_time_ms,
                             (unsigned long)time_tick,
                             (unsigned long)initiator->cs_procedure.cs_config.subevent_len,
                             initiator->cs_procedure.cs_config.subevent_interval);
        }
      } else {
        if (evt->data.evt_cs_procedure_enable_complete.state == sl_bt_cs_procedure_state_disabled) {
          initiator_log_error("#%u: CS procedure - stop received error response! [status: 0x%x]" LOG_NL,
                              initiator->conn_handle,
                              evt->data.evt_cs_procedure_enable_complete.status);
          if (!initiator->deleted) {
            initiator_start_cs_procedure(initiator->conn_handle);
          }
        } else {
          initiator_log_error("#%u: CS procedure - start received error response! [status: 0x%x]" LOG_NL,
                              initiator->conn_handle,
                              evt->data.evt_cs_procedure_enable_complete.status);
          initiator_start_cs_procedure(initiator->conn_handle);
        }
        break;
      }
      break;

    // --------------------------------
    // CS configuration completed
    case sl_bt_evt_cs_config_complete_id:
      initiator = cs_initiator_get_instance(evt->data.evt_cs_config_complete.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event "
                            "[sl_bt_evt_cs_config_complete_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_cs_config_complete.connection);
        break;
      } else {
        initiator_log_info("#%u: CS - configuration completed. "
                           "Set CS procedure parameters ..." LOG_NL,
                           initiator->conn_handle);

        stop_error_timer(initiator->conn_handle);

        initiator->cs_procedure.cs_config.ch3c_jump = evt->data.evt_cs_config_complete.ch3c_jump;
        initiator->cs_procedure.cs_config.ch3c_shape = evt->data.evt_cs_config_complete.ch3c_shape;
        initiator->cs_procedure.cs_config.channel_map_repetition = evt->data.evt_cs_config_complete.channel_map_repetition;
        initiator->cs_procedure.cs_config.cs_sync_phy = evt->data.evt_cs_config_complete.cs_sync_phy;
        initiator->cs_procedure.cs_config.rtt_type = evt->data.evt_cs_config_complete.rtt_type;
        initiator->cs_procedure.cs_config.num_calib_steps = evt->data.evt_cs_config_complete.mode_calibration_steps;
        initiator->cs_procedure.cs_config.T_FCS_time = evt->data.evt_cs_config_complete.fcs_time_us;
        initiator->cs_procedure.cs_config.T_IP1_time = evt->data.evt_cs_config_complete.ip1_time_us;
        initiator->cs_procedure.cs_config.T_IP2_time = evt->data.evt_cs_config_complete.ip2_time_us;
        initiator->cs_procedure.cs_config.T_PM_time = evt->data.evt_cs_config_complete.pm_time_us;

        sc = sl_bt_cs_set_procedure_parameters(initiator->conn_handle,
                                               initiator->config.config_id,
                                               initiator->config.max_procedure_duration,
                                               initiator->config.min_procedure_interval,
                                               initiator->config.max_procedure_interval,
                                               initiator->config.max_procedure_count,
                                               initiator->config.min_subevent_len,
                                               initiator->config.max_subevent_len,
                                               initiator->config.antenna_config,
                                               initiator->config.phy,
                                               initiator->config.tx_pwr_delta,
                                               initiator->config.preferred_peer_antenna,
                                               initiator->config.snr_control_initiator,
                                               initiator->config.snr_control_reflector);

        if (sc != SL_STATUS_OK) {
          initiator_log_error("#%u: CS procedure - failed to set parameters! "
                              "[sc: 0x%lx]" LOG_NL,
                              initiator->conn_handle,
                              (unsigned long)sc);
          on_error(initiator->conn_handle,
                   CS_ERROR_EVENT_CS_SET_PROCEDURE_PARAMETERS_FAILED,
                   sc);
          break;
        }
      }
      handled = true;
      break;

    // --------------------------------
    // CS result (initiator) arrived
    case sl_bt_evt_cs_result_id:
      initiator = cs_initiator_get_instance(evt->data.evt_cs_result.connection);
      if (initiator == NULL) {
        initiator_log_error("Unexpected event [sl_bt_evt_cs_result_id]!"
                            "Unknown target connection id: #%u" LOG_NL,
                            evt->data.evt_cs_result.connection);
        break;
      }
      handled = true;

      procedure_action =
        extract_cs_result_data(initiator->conn_handle, true, &evt->data.evt_cs_result);
      if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_RESET) {
        initiator->procedure_reset_request = true;
      } else if (procedure_action == CS_PROCEDURE_ACTION_TRIGGER_START) {
        initiator_restart_cs_procedure(initiator->conn_handle);
      }

      break;

    // --------------------------------
    // Bluetooth stack resource exhausted
    case sl_bt_evt_system_resource_exhausted_id:
    {
      uint8_t num_buffers_discarded =
        evt->data.evt_system_resource_exhausted.num_buffers_discarded;
      uint8_t num_buffer_allocation_failures =
        evt->data.evt_system_resource_exhausted.num_buffer_allocation_failures;
      uint8_t num_heap_allocation_failures =
        evt->data.evt_system_resource_exhausted.num_heap_allocation_failures;
      initiator_log_error("BT stack buffers exhausted, "
                          "data loss may have occurred! [buf_discarded:%u, "
                          "buf_alloc_fail:%u, heap_alloc_fail:%u]" LOG_NL,
                          num_buffers_discarded,
                          num_buffer_allocation_failures,
                          num_heap_allocation_failures);
    }
    break;

    default:
      initiator_log_debug("unhandled BLE event: 0x%lx" LOG_NL,
                          (unsigned long)SL_BT_MSG_ID(evt->header));
      break;
  }

  // Return false if the event was handled above.
  return !handled;
}

/******************************************************************************
 * Error timer callback.
 *****************************************************************************/
#if defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
static void error_timer_cb(app_timer_t *handle, void *data)
{
  cs_initiator_t *initiator = (cs_initiator_t *)data;
  if (handle == &initiator->error_timer_handle) {
    initiator->error_timer_started = false;
    initiator->error_timer_elapsed = true;
    on_error(initiator->conn_handle,
             CS_ERROR_EVENT_TIMER_ELAPSED,
             SL_STATUS_TIMEOUT);
  }
}
#endif // defined(CS_INITIATOR_ERROR_TIMER_USED) && (CS_INITIATOR_ERROR_TIMER_USED == 1)
/******************************************************************************
 * Weak implementation of the on_waiting indication for CS initiator.
 *****************************************************************************/
SL_WEAK void cs_initiator_on_waiting(void)
{
}

sl_rtl_cs_procedure *cs_initiator_get_procedure(const uint8_t conn_handle)
{
  cs_initiator_t *initiator = cs_initiator_get_instance(conn_handle);
  if (initiator == NULL) {
    initiator_log_error("unknown connection id #%u!" LOG_NL,
                        conn_handle);
    return NULL;
  }
  return &initiator->cs_procedure;
}
