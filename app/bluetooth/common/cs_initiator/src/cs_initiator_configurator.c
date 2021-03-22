/***************************************************************************//**
 * @file
 * @brief CS initiator configurator logic
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

#include <stdint.h>
#include <stdbool.h>
#include "sl_rtl_clib_api.h"
#include "sl_bt_api.h"
#include "cs_initiator.h"
#include "cs_initiator_configurator.h"
#include "cs_initiator_config.h"

// -----------------------------------------------------------------------------
// Public function definitions

/******************************************************************************
 * Set default config values for initiator.
 *****************************************************************************/
sl_status_t cs_initiator_set_default_config(cs_initiator_config_t *config,
                                            rtl_config_t *rtl_config)
{
  // Main mode. mode_2: PBR, mode_1: RTT
  config->cs_mode =                     sl_bt_cs_mode_pbr;
  // Submode. Not used.
  config->submode =                     SUBMODE_NOT_USED;
  // Minimum suggested duration for each CS subevent in microseconds. Range: 0x01 to 0xFFFFFF
  config->min_subevent_len =            1;
  // Maximum suggested duration for each CS subevent in microseconds. Range: 0x01 to 0xFFFFFF
  config->max_subevent_len =            0xffffu;
  // Minimum procedure interval
  config->min_procedure_interval =      CS_INITIATOR_MIN_INTERVAL;
  // Maximum procedure interval
  config->max_procedure_interval =      CS_INITIATOR_MAX_INTERVAL;
  // In 1.25ms units, 20ms
  config->min_connection_interval =     16;
  // In 1.25ms units, 20ms
  config->max_connection_interval =     16;
  // Procedure execution number
  // Value: 0. Free running
  // Value: 1. Start new procedure after one finished.
  config->max_procedure_count =         1;
  // Connection PHY
  config->phy =                         sl_bt_gap_phy_coding_1m_uncoded;
  // Only one channel is supported
  config->config_id =                   1;
  // Range: 0x02 to 0xA0
  config->min_main_mode_steps =         3;
  // Range: 0x02 to 0xA0
  config->max_main_mode_steps =         5;
  // The number of main mode steps taken from the end of the last CS subevent
  // to be repeated at the beginning of the current CS subevent directly after
  // the last Mode 0 step of that event. Range: 0x00 to 0x03
  config->main_mode_repetition =        0;
  // Number of Mode 0 steps to be included at the beginning of the test CS subevent.
  config->mode0_step =                  3;
  // The number of times the channel_map field will be cycled through for non-Mode 0 steps within a CS procedure.
  // Range: 0x01 to 0x03
  config->channel_map_repetition =      1;
  // Number of channels skipped in each rising and falling sequence. Range: 2 to 8
  config->ch3c_jump =                   2;
  // Maximum duration for each measurement procedure. Value in units of 0.625 ms
  // Range: 0x0001 to 0xFFFF
  config->max_procedure_duration =      0xffff;
  // Transmit power delta, in signed dB.
  config->tx_pwr_delta =                0x00;
  // Antenna configuration index. Range: 0x00 to 0x07
  // Regardless of SL_RAIL_UTIL_CS_ANTENNA_COUNT, the only accepted value is 0 for now.
  config->antenna_config =              0;
  // The preferred peer-ordered antenna elements to be used by the remote device
  // for the antenna configuration denoted by the tone antenna config selection.
  config->preferred_peer_antenna =      1;
  // CS power
  config->max_tx_power_dbm =            20;
  // Value: 0x00. Write CS configuration in the local controller only
  // Value: 0x01. Write CS configuration in both the local and remote controller using a configuration procedure
  config->create_context =              1;
  // RSSI measurement enabled
  config->rssi_measurement_enabled =    true;
  // Reference RSSI value of the Tx device at 1.0 m distance in dBm
  config->rssi_tx_power =               -40.0F;
  // Peripheral latency, which defines how many connection intervals the peripheral
  // can skip if it has no data to send
  config->latency =                     0;
  // Supervision timeout
  config->timeout =                     200;
  // Minimum length of the connection event
  config->min_ce_length =               0;
  // Maximum length of the connection event
  config->max_ce_length =               0xffff;
  // Antenna identifier to be used for CS
  config->cs_sync_antenna =             1;
  // Connection maximum TX power in dBm
  config->tx_power_requested_max_dbm =  20;
  // Connection minimum TX power in dBm
  config->tx_power_requested_min_dbm =  -10;
  // RTT type
  config->rtt_type =                    sl_bt_cs_rtt_type_fractional_96_bit_sounding;
  // Channel selection type.
  config->channel_selection_type =      sl_bt_cs_channel_selection_algorithm_3b;
  // Ch3c shape.
  config->ch3c_shape =                  sl_bt_cs_ch3c_shape_hat;
  // Companion signal enable/disable.
  config->companion_signal_enable =     sl_bt_cs_companion_signal_status_disable;
  // Channel map length
  #define CHANNEL_MAP_LEN sizeof(config->channel_map.data) / sizeof(config->channel_map.data[0])
  config->channel_map_len = CHANNEL_MAP_LEN;

  // NOTE: the RTL library doesn't support anything else besides
  // 72 channels (excluding advertising channels) enabled as of yet - that's why the config is overridden here.
  // This part should be deleted once RTL supports arbitrary channel mapping.
  // Exclude advertising channels 0, 1, 23, 24, 25, 77, 78, 79
  const uint8_t fixed_channel_config[CHANNEL_MAP_LEN] =
  { 0xFC, 0xFF, 0x7F, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F };
  memcpy(config->channel_map.data, fixed_channel_config, sizeof(config->channel_map.data));

  config->snr_control_initiator = sl_bt_cs_snr_control_adjustment_not_applied;
  config->snr_control_reflector = sl_bt_cs_snr_control_adjustment_not_applied;

  // Moving object tracking is enabled by default
  config->use_moving_object_tracking = true;

  // RTL configuration
  rtl_config->algo_mode = config->use_moving_object_tracking
                          ? SL_RTL_CS_ALGO_MODE_REAL_TIME_BASIC
                          : SL_RTL_CS_ALGO_MODE_STATIC_HIGH_ACCURACY;
  rtl_config->cs_parameters.max_number_of_frequencies = DEFAULT_NUM_TONES;
  rtl_config->cs_parameters.delta_f = DEFAULT_FREQUENCY_DELTA;
  rtl_config->rtl_logging_enabled = true;

  return SL_STATUS_OK;
}
