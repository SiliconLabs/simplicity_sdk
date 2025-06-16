/***************************************************************************//**
 * @file
 * @brief PA power tables conversion functions
 * @details This file contains the logics with which we convert the given power
 * to raw power level using power setting tables
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

#ifndef SL_RAIL_PA_TABLES_H
#define SL_RAIL_PA_TABLES_H

#include "sl_rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum sl_rail_pa_power_setting_submode_t
 * @brief An enumeration of the power sub modes for Power Setting tables
 *
 */
SLI_RAIL_ENUM(sl_rail_pa_power_setting_submode_t) {
  SL_RAIL_PA_POWER_SETTING_SUBMODE_HP       = 0U,
  SL_RAIL_PA_POWER_SETTING_SUBMODE_MP       = 1U,
  SL_RAIL_PA_POWER_SETTING_SUBMODE_LP       = 2U,
  SL_RAIL_PA_POWER_SETTING_SUBMODE_LLP      = 3U,
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define SL_RAIL_PA_POWER_SETTING_SUBMODE_HP ((sl_rail_pa_power_setting_submode_t) SL_RAIL_PA_POWER_SETTING_SUBMODE_HP)
#if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1) || (SL_RAIL_SUPPORTS_SUBGHZ_BAND)
#define SL_RAIL_PA_POWER_SETTING_SUBMODE_MP ((sl_rail_pa_power_setting_submode_t) SL_RAIL_PA_POWER_SETTING_SUBMODE_MP)
#endif//_SILICON_LABS_32B_SERIES_2_CONFIG == 1
#if ((_SILICON_LABS_32B_SERIES_2_CONFIG != 3) && (_SILICON_LABS_32B_SERIES_2_CONFIG != 8))
#define SL_RAIL_PA_POWER_SETTING_SUBMODE_LP ((sl_rail_pa_power_setting_submode_t) SL_RAIL_PA_POWER_SETTING_SUBMODE_LP)
#endif//((_SILICON_LABS_32B_SERIES_2_CONFIG != 3) && (_SILICON_LABS_32B_SERIES_2_CONFIG != 8))
#define SL_RAIL_PA_POWER_SETTING_SUBMODE_LLP ((sl_rail_pa_power_setting_submode_t) SL_RAIL_PA_POWER_SETTING_SUBMODE_LLP)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/* PA table selection macros used by rail_util_pa_tables plugin */
#define SL_RAIL_PA_TABLE_SELECTION_DISABLE     0U
#define SL_RAIL_PA_TABLE_SELECTION_HP          1U
#define SL_RAIL_PA_TABLE_SELECTION_MP          2U
#define SL_RAIL_PA_TABLE_SELECTION_LP          3U
#define SL_RAIL_PA_TABLE_SELECTION_LLP         4U
#define SL_RAIL_PA_TABLE_SELECTION_AUTO_HP     5U
#define SL_RAIL_PA_TABLE_SELECTION_AUTO_MP     6U

#define PA_SUBMODE_TO_2P4_GHZ_MODE(submode)              \
  (submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_HP)       \
  ? RAIL_TX_POWER_MODE_2P4GIG_HP                         \
  : ((submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_MP)    \
     ? RAIL_TX_POWER_MODE_2P4GIG_MP                      \
     : ((submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_LP) \
        ? RAIL_TX_POWER_MODE_2P4GIG_LP                   \
        : RAIL_TX_POWER_MODE_2P4GIG_LLP))

#define PA_SUBMODE_TO_SUB_GHZ_MODE(submode)              \
  (submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_HP)       \
  ? RAIL_TX_POWER_MODE_SUBGIG_HP                         \
  : ((submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_MP)    \
     ? RAIL_TX_POWER_MODE_SUBGIG_MP                      \
     : ((submode == SL_RAIL_PA_POWER_SETTING_SUBMODE_LP) \
        ? RAIL_TX_POWER_MODE_SUBGIG_LP                   \
        : RAIL_TX_POWER_MODE_SUBGIG_LLP))

/**
 * @addtogroup PA_Table_Conversions PA Table Conversions
 * @ingroup PA
 * @{
 */

/**
 * Converts the desired decibel value (in units of deci-dBm)
 * to a \ref sl_rail_tx_power_setting_entry_t.
 *
 * @param[in] rail_handle A RAIL instance handle.
 * @param[in] mode PA mode for which the conversion is to be done.
 * @param[in] power_ddbm Desired dBm value in units of deci-dBm.
 * @param[out] pPowerSettingInfo A non-NULL pointer to the
 *   \ref sl_rail_tx_power_setting_entry_t structure to be filled in with the
 *   converted value.
 * @return Status code indicating success of function call.
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be characterized and implemented in an
 * overriding version of \ref sl_rail_util_pa_convert_dbm_to_power_setting_entry().
 * In the weak version provided with the RAIL
 * library, railHandle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle, or one
 * of the radio-generic ones, such as \ref SL_RAIL_EFR32_HANDLE.
 *
 * Although the implementation of this function may change, the signature
 * must be as declared here.
 */
sl_rail_status_t sl_rail_util_pa_convert_dbm_to_power_setting_entry(sl_rail_handle_t railHandle,
                                                                    sl_rail_tx_power_t power,
                                                                    sl_rail_tx_power_setting_entry_t *pPowerSettingInfo);

/** @} */ // PA_Curve_Conversions

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_PA_TABLES_H
