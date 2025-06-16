/***************************************************************************//**
 * @file
 * @brief PA power conversion functions provided to the customer as source for
 *   highest level of customization.
 * @details This file contains the curves and logic that convert PA power
 *   levels to dBm powers.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
// For details on how to use this plugin, see
//   https://www.silabs.com/documents/public/application-notes/an1127-power-amplifier-power-conversion-functions.pdf

#include "em_device.h"
#if defined(_SILICON_LABS_32B_SERIES_2) || defined(SIMULATION_DEVICE)
#include "em_cmu.h"
#else
#include "sl_clock_manager.h"
#endif

#include "sl_rail_util_pa_conversions.h"
#include "sl_rail.h"
#include "rail.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static sl_rail_tx_power_table_config_t sli_rail_util_pa_power_table;
const bool sli_rail_3_pa = true;

#define PA_SUBMODE_MASK 0xC000UL

#define PA_SUBMODE_SHIFT 14U

sl_rail_status_t sl_rail_init_tx_power_table(const sl_rail_tx_power_table_config_t *p_tx_power_table_config)
{
  sl_rail_status_t status = sl_rail_verify_tx_power_curves(p_tx_power_table_config);
  if (status == RAIL_STATUS_NO_ERROR) {
    sli_rail_util_pa_power_table = *p_tx_power_table_config;
  }
  return status;
}

RAIL_TxPowerLevel_t RAIL_ConvertDbmToRaw(RAIL_Handle_t railHandle,
                                         RAIL_TxPowerMode_t mode,
                                         RAIL_TxPower_t power)
{
  // TODO stub, this will go as we change calls in the lib to use new cb
  (void)railHandle;
  (void)mode;
  (void)power;
  return SL_RAIL_TX_POWER_LEVEL_INVALID;
}

RAIL_TxPower_t RAIL_ConvertRawToDbm(RAIL_Handle_t railHandle,
                                    RAIL_TxPowerMode_t mode,
                                    RAIL_TxPowerLevel_t powerLevel)
{
  // TODO stub, this will go as we change calls in the lib to use new cb
  (void)railHandle;
  (void)mode;
  (void) powerLevel;
  return SL_RAIL_TX_POWER_MIN;
}

const sl_rail_pa_power_setting_t *sl_rail_util_pa_get_power_setting_table(sl_rail_handle_t rail_handle,
                                                                          sl_rail_tx_pa_mode_t pa_mode,
                                                                          sl_rail_tx_power_t *p_min_ddbm,
                                                                          sl_rail_tx_power_t *p_max_ddbm,
                                                                          sl_rail_tx_power_level_t *p_step_ddbm)
{
  (void)rail_handle;
  sl_rail_pa_descriptor_t *p_pa_descriptor = &(sli_rail_util_pa_power_table.p_pa_table_descriptor[pa_mode]);
  *p_min_ddbm = p_pa_descriptor->min_power_ddbm;
  *p_max_ddbm = p_pa_descriptor->max_power_ddbm;
  *p_step_ddbm = p_pa_descriptor->step_power_ddbm;
  return (sl_rail_pa_power_setting_t*)(p_pa_descriptor->p_power_setting_table); //This includes sl_rail_pa_power_setting_t and curr_pa_power_ddbm
}

sl_rail_status_t sl_railcb_convert_ddbm_to_power_setting_entry(sl_rail_handle_t rail_handle,
                                                               sl_rail_tx_power_t power_ddbm,
                                                               sl_rail_tx_pa_mode_t pa_mode,
                                                               sl_rail_tx_power_t channel_restr_max_power_ddbm,
                                                               sl_rail_tx_power_setting_entry_t *p_power_setting_info)
{
  (void) rail_handle;
  // Initialize the powersetting config from the table in a init fn()
  // Choose the table based on pa_mode
  sl_rail_tx_power_table_config_t const *p_power_table_config = &sli_rail_util_pa_power_table;
  if (pa_mode > p_power_table_config->num_of_tables) {
    return SL_RAIL_STATUS_INVALID_CALL;
  }
  sl_rail_pa_descriptor_t *p_pa_descriptor = &(p_power_table_config->p_pa_table_descriptor[pa_mode]);

  sl_rail_tx_power_t min_power_ddbm = p_pa_descriptor->min_power_ddbm;
  sl_rail_tx_power_t max_power_ddbm = p_pa_descriptor->max_power_ddbm;
  sl_rail_tx_power_t table_step_ddbm = p_pa_descriptor->step_power_ddbm;

  // Cap the power to within the range of the mapping table
  if (power_ddbm < min_power_ddbm) {
    power_ddbm = min_power_ddbm;
  } else if (power_ddbm > max_power_ddbm) {
    power_ddbm = max_power_ddbm;
  } else {
    // Power level is within bounds (MISRA required else)
  }

  // Find the mapping table for the given pa_mode
  int32_t *p_curr_power_setting_table = p_pa_descriptor->p_power_setting_table;

  // Calculate indices
  uint16_t max_index = p_pa_descriptor->num_of_values;
  uint16_t power_index = (uint16_t)((power_ddbm - min_power_ddbm) / table_step_ddbm);

  // Adjust the index if our estimated max power is less than max power provided
  // in the table because an extra entry is added in table in this case.
  if (max_power_ddbm > (min_power_ddbm + max_index * table_step_ddbm)) {
    max_index++;
  }

  // Ensure power_index is within bounds
  if (power_index > max_index || power_ddbm == max_power_ddbm) {
    power_index = max_index;
  }
  bool find_power_under_chan_restr = true;
  sl_rail_pa_power_setting_t power_setting = { 0 };
  while (find_power_under_chan_restr) {
    //TODO: Powersetting has to be extracted once offsets are also part of this 32 bits
    power_setting = p_pa_descriptor->p_power_setting_table[power_index];

    while ((power_index > 0U)
           && (power_setting
               == (sl_rail_pa_power_setting_t)p_pa_descriptor->p_power_setting_table[power_index - 1U])) {
      // TODO: This might not be true comparison if we add offset in the table,
      // we will have to extract the powersetting entry
      power_index--;
    }

    // get estimated power
    // (this is true until we have offsets in place)
    if (power_ddbm != max_power_ddbm) {
      // Calculate actual power
      power_ddbm = min_power_ddbm + ((sl_rail_tx_power_t)power_index * table_step_ddbm);
    }
    //TODO: Should there be a else? to get the max power
    #if 0
    //Once offset are in place remove the above if block and activate this block
    // And define the mask and shifts for offset as required by the table.
    power_ddbm  = ((p_pa_descriptor->p_power_setting_table[power_index])
                   & (PA_ACTUAL_DBM_OFFSET_MASK))
                  >> PA_ACTUAL_DBM_OFFSET_SHIFT;
    #endif
    // Restrictions should be based on rounding mode
    // TODO: this should also be depend on rounding mode
    // uint8_t rounding_mode = p_pa_descriptor->pa_rounding_mode;
    if (power_ddbm <= channel_restr_max_power_ddbm) {
      find_power_under_chan_restr = false;
    } else {
      if (power_index == 0) {
        break;
      }
      power_index--;
    }
  }

  // Find the powersetting entry from the table for power_ddbm and pa_mode
  // Check if the actual power returned from the table is within the
  p_power_setting_info->pa_power_setting = power_setting;
  //TODO:
#if 0
  p_power_setting_info->pa_power_setting = power_setting
                                           & (~PA_ACTUAL_DBM_OFFSET_MASK);
#endif
  p_power_setting_info->curr_pa_power_ddbm = power_ddbm;
#if 0 //This should be done in library
  //TODO : Move RAIL_ConfigTxPower to the plugin which basically sets the state
  // have another function tha actually calls PA_Configs and sets up the registers
  sl_rail_tx_power_config_t *p_tx_power_config = tx_power_config;
  p_tx_power_config->mode = ((power_setting & PA_SUBMODE_MASK) >> PA_SUBMODE_SHIFT);
  sl_rail_status_t status = RAIL_ConfigTxPower(rail_handle, (RAIL_TxPowerConfig_t *)p_tx_power_config);
#endif
  return SL_RAIL_STATUS_NO_ERROR;
}

#include "sl_rail_util_pa_config.h"

#if     SL_RAIL_UTIL_PA_NVM_ENABLED
#include "sl_rail_util_pa_nvm_configs.h"
#endif//SL_RAIL_UTIL_PA_NVM_ENABLED

static sl_rail_tx_power_config_t tx_power_config = {
  .mode = RAIL_TX_POWER_MODE_NONE,
  .voltage_mv = SL_RAIL_UTIL_PA_VOLTAGE_MV,
  .ramp_time_us = SL_RAIL_UTIL_PA_RAMP_TIME_US,
};

#if RAIL_SUPPORTS_2P4GHZ_BAND
static RAIL_TxPowerConfig_t txPowerConfig2p4Ghz = {
  .mode = SL_RAIL_UTIL_PA_SELECTION_2P4GHZ,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
  .rampTime = SL_RAIL_UTIL_PA_RAMP_TIME_US,
};
#endif
#if RAIL_SUPPORTS_SUBGHZ_BAND
static RAIL_TxPowerConfig_t txPowerConfigSubGhz = {
  .mode = SL_RAIL_UTIL_PA_SELECTION_SUBGHZ,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
  .rampTime = SL_RAIL_UTIL_PA_RAMP_TIME_US,
};
#endif
#if RAIL_SUPPORTS_OFDM_PA
#ifndef SL_RAIL_UTIL_PA_SELECTION_OFDM
#define SL_RAIL_UTIL_PA_SELECTION_OFDM RAIL_TX_POWER_MODE_OFDM_PA_POWERSETTING_TABLE
#endif
static RAIL_TxPowerConfig_t txPowerConfigOFDM = {
  .mode = SL_RAIL_UTIL_PA_SELECTION_OFDM,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
};
#endif // RAIL_SUPPORTS_OFDM_PA

void sl_rail_util_pa_init(void)
{
  const sl_rail_tx_power_table_config_t *tx_power_tables;
  tx_power_tables = &sl_rail_tx_power_table;

  (void)sl_rail_init_tx_power_table(tx_power_tables);
  // Configure default config power
  RAIL_ConfigTxPower(SL_RAIL_EFR32_HANDLE, (RAIL_TxPowerConfig_t *)&tx_power_config);
#if SL_RAIL_UTIL_PA_CALIBRATION_ENABLE
  RAIL_EnablePaCal(true);
#else
  RAIL_EnablePaCal(false);
#endif
}

sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_2p4ghz(void)
{
#if SL_RAIL_SUPPORTS_2P4_GHZ_BAND
  return (sl_rail_tx_power_config_t *)&txPowerConfig2p4Ghz;
#else
  return NULL;
#endif
}

sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_subghz(void)
{
#if SL_RAIL_SUPPORTS_SUB_GHZ_BAND
  return (sl_rail_tx_power_config_t *)&txPowerConfigSubGhz;
#else
  return NULL;
#endif
}

#if RAIL_SUPPORTS_OFDM_PA
#ifndef SL_RAIL_UTIL_PA_SELECTION_OFDM
#define SL_RAIL_UTIL_PA_SELECTION_OFDM RAIL_TX_POWER_MODE_OFDM_PA_POWERSETTING_TABLE
#endif
static RAIL_TxPowerConfig_t txPowerConfigOFDM = {
  .mode = SL_RAIL_UTIL_PA_SELECTION_OFDM,
  .voltage = SL_RAIL_UTIL_PA_VOLTAGE_MV,
};
#endif // RAIL_SUPPORTS_OFDM_PA

sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_ofdm(void)
{
#if SL_RAIL_SUPPORTS_OFDM_PA
  return (sl_rail_tx_power_config_t *)&txPowerConfigOFDM;
#else
  return NULL;
#endif // RAIL_SUPPORTS_OFDM_PA
}

void sl_rail_util_pa_on_channel_config_change(sl_rail_handle_t rail_handle,
                                              const sl_rail_channel_config_entry_t *p_entry)
{
  if (true) {     // No PA auto-mode in RAIL 3
    sl_rail_tx_power_config_t currentTxPowerConfig;
    sl_rail_tx_power_config_t *newTxPowerConfigPtr;
    sl_rail_status_t status;

    // Get current TX Power Config.
    status = sl_rail_get_tx_power_config(rail_handle, &currentTxPowerConfig);
    if (status != SL_RAIL_STATUS_NO_ERROR) {
      while (true) {
      }     // Error: Can't get TX Power Config
    }

#if SL_RAIL_SUPPORTS_DUAL_BAND
    // Determine new TX Power Config.
    if (p_entry->base_frequency_hz < 1000000000UL) {
      newTxPowerConfigPtr = (sl_rail_tx_power_config_t *)&txPowerConfigSubGhz;
    } else {
      newTxPowerConfigPtr = (sl_rail_tx_power_config_t *)&txPowerConfig2p4Ghz;
    }
#else
    (void) p_entry;
#if SL_RAIL_SUPPORTS_2P4_GHZ_BAND
    newTxPowerConfigPtr = (sl_rail_tx_power_config_t *)&txPowerConfig2p4Ghz;
#else
    newTxPowerConfigPtr = (sl_rail_tx_power_config_t *)&txPowerConfigSubGhz;
#endif
#endif

#if SL_RAIL_IEEE802154_SUPPORTS_DUAL_PA_CONFIG
    if (currentTxPowerConfig.mode == SL_RAIL_TX_POWER_MODE_NONE) {
#if SL_RAIL_SUPPORTS_OFDM_PA
      if (sl_rail_supports_tx_power_mode(rail_handle,
                                         (sl_rail_tx_power_mode_t*)&txPowerConfigOFDM.mode,
                                         NULL, NULL)) {
        // Apply OFDM Power Config.
        status = sl_rail_config_tx_power(rail_handle, (sl_rail_tx_power_config_t *)&txPowerConfigOFDM);
        if (status != SL_RAIL_STATUS_NO_ERROR) {
          while (true) {
          }     // Error: Can't set TX Power Config
        }
        // Set default TX power after sl_rail_config_tx_power.
        status = sl_rail_set_tx_power_dbm(rail_handle, SL_RAIL_UTIL_PA_POWER_DECI_DBM);
        if (status != SL_RAIL_STATUS_NO_ERROR) {
          while (true) {
          }     // Error: Can't set TX Power
        }
      }
#endif // SL_RAIL_SUPPORTS_OFDM_PA
      // Apply FSK Power Config.
      status = sl_rail_config_tx_power(rail_handle, newTxPowerConfigPtr);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        while (true) {
        }     // Error: Can't set TX Power Config
      }
      // Set default TX power after sl_rail_config_tx_power.
      status = sl_rail_set_tx_power_dbm(rail_handle, SL_RAIL_UTIL_PA_POWER_DECI_DBM);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        while (true) {
        }     // Error: Can't set TX Power
      }
    }
#else
    // Call sl_rail_config_tx_power() only if TX Power Config mode has changed.
    if (currentTxPowerConfig.mode != newTxPowerConfigPtr->mode) {
      // Save current TX power before sl_rail_config_tx_power (because not preserved).
      sl_rail_tx_power_t txPowerDeciDbm;
      if (currentTxPowerConfig.mode == SL_RAIL_TX_POWER_MODE_NONE) {
        txPowerDeciDbm = SL_RAIL_UTIL_PA_POWER_DECI_DBM;
      } else {
        txPowerDeciDbm = sl_rail_get_tx_power_dbm(rail_handle);
      }

      // Apply new TX Power Config.
      status = sl_rail_config_tx_power(rail_handle, newTxPowerConfigPtr);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        while (true) {
        }     // Error: Can't set TX Power Config
      }
      // Restore TX power after sl_rail_config_tx_power.
      status = sl_rail_set_tx_power_dbm(rail_handle, txPowerDeciDbm);
      if (status != SL_RAIL_STATUS_NO_ERROR) {
        while (true) {
        }     // Error: Can't set TX Power
      }
      // If requested a HIGHEST setting, update it with the real one selected
      // to short-circuit the next time through here since HIGHEST never
      // matches the real PA returned by sl_rail_get_tx_power_config(), causing
      // reconfiguration of the same PA on every callback.
      if (false
         #ifdef  SL_RAIL_TX_POWER_MODE_2P4_GHZ_HIGHEST
          || (newTxPowerConfigPtr->mode == SL_RAIL_TX_POWER_MODE_2P4_GHZ_HIGHEST)
         #endif
         #ifdef  SL_RAIL_TX_POWER_MODE_SUB_GHZ_HIGHEST
          || (newTxPowerConfigPtr->mode == SL_RAIL_TX_POWER_MODE_SUB_GHZ_HIGHEST)
         #endif
          ) {
        (void) sl_rail_get_tx_power_config(rail_handle, &currentTxPowerConfig);
        newTxPowerConfigPtr->mode = currentTxPowerConfig.mode;
      }
    }
#endif
  }
}
