/***************************************************************************//**
 * @file
 * @brief CS initiator configurator API
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

#ifndef CS_INITIATOR_CONFIGURATOR_H
#define CS_INITIATOR_CONFIGURATOR_H

// -----------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include "sl_bt_api.h"
#include "sl_rtl_clib_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Macros

// GATT

// Length of UUID in bytes
#define UUID_LEN                                      16

// GATT / Initiator

// Initiator GATT service
#define INITIATOR_SERVICE_UUID                        { 0xf2, 0x20, 0x18, 0xc7, 0x32, 0x2d, 0xc7, 0xab, \
                                                        0xcf, 0x46, 0xf7, 0xff, 0x70, 0x9e, 0xb9, 0xbb };

// Initiator GATT characteristic
#define INITIATOR_CHARACTERISTIC_UUID                 { 0x91, 0xbe, 0x18, 0xa4, 0x22, 0x3c, 0x49, 0x9b, \
                                                        0x03, 0x43, 0x91, 0x13, 0xec, 0x95, 0x9f, 0x92 };

#define SUBMODE_NOT_USED                              0xff

#define DEFAULT_NUM_TONES                             79            // Number of tones
#define DEFAULT_FREQUENCY_DELTA                       1000000.0f    // Channel frequency delta

#define INITIATOR_DEVICE_NAME "CS RFLCT"

// -----------------------------------------------------------------------------
// Enums, structs, typedefs

// TX power type
typedef int8_t cs_tx_power_t;

// PHY type
typedef sl_bt_gap_phy_coding_t cs_phy_t;

// UI modes enumerator
typedef enum {
  CS_INITIATOR_MODE_DYNAMIC = 0,
  CS_INITIATOR_MODE_RTT = sl_bt_cs_mode_rtt,
  CS_INITIATOR_MODE_PBR = sl_bt_cs_mode_pbr
} cs_initiator_mode_t;

// Discovery state enumerator
typedef enum {
  CS_DISCOVERY_STATE_IDLE,
  CS_DISCOVERY_STATE_CONN,
  CS_DISCOVERY_STATE_SCAN,
  CS_DISCOVERY_STATE_FINISHED
} cs_discovery_state_t;

// RTL library calculation result type
typedef struct {
  uint8_t connection;                   ///< Connection handle
  float distance;                       ///< Calculated distance
  float likeliness;                     ///< Calculated distance likeliness
  float rssi_distance;                  ///< Distance calculated with RSSI values
  uint8_t cs_bit_error_rate;            ///< CS bit error rate
} cs_result_t;

typedef enum {
  CS_ERROR_EVENT_UNHANDLED,
  CS_ERROR_EVENT_TIMER_HANDLE_NULL_REFERENCE,
  CS_ERROR_EVENT_TIMER_START_ERROR,
  CS_ERROR_EVENT_TIMER_ELAPSED,
  CS_ERROR_EVENT_TIMER_STOP_ERROR,
  CS_ERROR_EVENT_START_SERVICE_DISCOVERY,
  CS_ERROR_EVENT_GATT_PROCEDURE_FAILED,
  CS_ERROR_EVENT_START_CHARACTERISTIC_DISCOVERY_FAILED,
  CS_ERROR_EVENT_SEND_NOTIFICATION_FAILED,
  CS_ERROR_EVENT_SEND_INDICATION_FAILED,
  CS_ERROR_EVENT_SEND_CONTROL_POINT_RESPONSE_FAILED,
  CS_ERROR_EVENT_WRITE_CHARACTERISTIC_FAILED,
  CS_ERROR_EVENT_SEND_CHARACTERISTIC_CONFIRMATION_FAILED,
  CS_ERROR_EVENT_CS_SET_PROCEDURE_PARAMETERS_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_SET_STARTTIME_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_ENABLE_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_START_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_RESTART_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_STOP_FAILED,
  CS_ERROR_EVENT_CS_PROCEDURE_COMPLETE_FAILED,
  CS_ERROR_EVENT_INITIATOR_INSTANCE_NULL,
  CS_ERROR_EVENT_INITIATOR_INSTANCE_SLOTS_FULL,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_SET_DEFAULT_CS_SETTINGS,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_CREATE_CONFIG,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_ENABLE_CS_SECURITY,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_INCREASE_SECURITY,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_INIT_RTL_LIB,
  CS_ERROR_EVENT_INITIATOR_CHANNEL_MAP_TOO_FEW_CHANNELS,
  CS_ERROR_EVENT_INITIATOR_CHANNEL_MAP_TOO_MANY_CHANNELS,
  CS_ERROR_EVENT_INITIATOR_FAILED_TO_SET_CONNECTION_PARAMETERS,
  CS_ERROR_EVENT_FILE_LOGGER_INIT_FAILED
} cs_error_event_t;

typedef PACKSTRUCT (struct {
  uint8_t phy;
  uint8_t cs_mode;
  uint8_t submode;
  uint8_t max_main_mode_steps;
  uint8_t min_main_mode_steps;
  uint8_t main_mode_repetition;
  uint8_t mode0_step;
  uint8_t channel_map_repetition;
  uint8_t ch3c_jump;
  uint8_t ch3c_shape;
  uint8_t antenna_config;
  uint8_t rtt_type;
  uint8_t channel_selection_type;
  uint8_t cs_sync_antenna;
  uint8_t companion_signal_enable;
  uint8_t config_id;
  uint8_t preferred_peer_antenna;
  uint8_t create_context;
  bool rssi_measurement_enabled;
  int8_t tx_pwr_delta;
  int8_t max_tx_power_dbm;
  cs_tx_power_t tx_power_max_dbm;
  cs_tx_power_t tx_power_min_dbm;
  cs_tx_power_t tx_power_requested_max_dbm;
  cs_tx_power_t tx_power_requested_min_dbm;
  float rssi_tx_power;
  uint32_t min_subevent_len;
  uint32_t max_subevent_len;
  uint16_t min_connection_interval;
  uint16_t max_connection_interval;
  uint16_t min_procedure_interval;
  uint16_t max_procedure_interval;
  uint16_t max_procedure_duration;
  uint16_t max_procedure_count;
  uint16_t latency;
  uint16_t timeout;
  uint16_t min_ce_length;
  uint16_t max_ce_length;
  sl_bt_cs_channel_map_t channel_map;
  uint32_t channel_map_len;
  uint8_t snr_control_initiator;
  uint8_t snr_control_reflector;
  bool use_moving_object_tracking;
}) cs_initiator_config_t;

typedef PACKSTRUCT (struct {
  uint8_t algo_mode;
  struct sl_rtl_cs_parameters cs_parameters;
  bool rtl_logging_enabled;
}) rtl_config_t;

// -----------------------------------------------------------------------------
// Function declarations

/**************************************************************************//**
 * Set default config values for initiator.
 * @param[out] config Pointer to an initiator config structure
 *
 * @return SL_STATUS_OK if default config set successfully
 *****************************************************************************/
sl_status_t cs_initiator_set_default_config(cs_initiator_config_t *config, rtl_config_t *rtl_config);

#ifdef __cplusplus
};
#endif

#endif // CS_INITIATOR__CONFIGURATOR_H
