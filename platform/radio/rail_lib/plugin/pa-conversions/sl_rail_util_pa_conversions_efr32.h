/***************************************************************************//**
 * @file
 * @brief PA power conversion functions provided to the customer as source for
 *   highest level of customization.
 * @details This file contains the curves and logic that convert PA power
 *   levels to dBm powers.
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

#ifndef SL_RAIL_PA_CONVERSIONS_EFR32_H
#define SL_RAIL_PA_CONVERSIONS_EFR32_H

#include "sl_rail_types.h"

// This macro is defined when Silicon Labs builds curves into the library as WEAK
// to ensure it can be overriden by customer versions of these functions. It
// should *not* be defined in a customer build.
#ifndef  RAIL_PA_CONVERSIONS_WEAK
#ifdef   SL_RAIL_UTIL_PA_CONFIG_HEADER
#include SL_RAIL_UTIL_PA_CONFIG_HEADER
#else
#include "sl_rail_util_pa_conversions_efr32_config.h"
#endif
#endif

#ifdef   SL_RAIL_UTIL_PA_CURVE_TYPES
#include SL_RAIL_UTIL_PA_CURVE_TYPES
#else
#include "sl_rail_util_pa_curve_types_efr32.h"
#endif

#ifdef   SL_RAIL_UTIL_PA_CURVE_HEADER
#include SL_RAIL_UTIL_PA_CURVE_HEADER
#else
#include "pa_curves_efr32.h" // TODO replace with sl_rail_util_pa name
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup PA_Curve_Conversions PA Curve Conversions
 * @ingroup PA
 * @{
 */

/// The curves to be used when battery voltage powers transmission
extern const sl_rail_tx_power_curves_config_t sl_rail_tx_power_curves_vbat;

/// The curves to be used when the DC-DC converter powers transmission
extern const sl_rail_tx_power_curves_config_t sl_rail_tx_power_curves_dcdc;

/**
 * Initialize Transmit power curves.
 *
 * @param[in] radio_handle A radio-generic RAIL handle.
 * @param[in] p_tx_power_curves_config A pointer to the TX power curves to use.
 * @return Status code indicating success of the function call.
 */
sl_rail_status_t sl_rail_init_tx_power_curves(sl_rail_handle_t radio_handle,
                                              const sl_rail_tx_power_curves_config_t *p_tx_power_curves_config);

/**
 * Gets the maximum power in deci-dBm that should be used for calculating
 * the segments and to find right curve segment to convert Dbm to raw power
 * level for a specific PA.
 *
 * @param[in] rail_handle A radio-generic or real RAIL instance handle.
 * @param[in] pa_mode PA mode whose curves are needed.
 * @param[out] p_max_power_ddbm A non-NULL pointer to memory allocated to hold
 *   the max power in deci-dBm used in calculation of curve segments.
 * @param[out] p_increment_ddbm A non-NULL pointer to memory allocated to hold
 *   the increment in deci-dBm used in calculation of curve segments.
 * @return Status code indicating success of the function call.
 *
 * For the PAs with \ref sl_rail_pa_conversion_algorithm_t
 * \ref SL_RAIL_PA_ALGORITHM_PIECEWISE_LINEAR, if the curves are generated with
 * max_power_ddbm and increment_ddbm other than \ref SL_RAIL_TX_POWER_CURVE_DEFAULT_MAX and
 * \ref SL_RAIL_TX_POWER_CURVE_DEFAULT_INCREMENT respectively, then the first
 * \ref sl_rail_tx_power_curve_segment_t has its max_power_level equal to
 * \ref SL_RAIL_TX_POWER_LEVEL_INVALID and its slope and intercept stores the
 * p_max_power_ddbm and p_increment_ddbm in deci-dBm respectively.
 */
sl_rail_status_t sl_rail_get_tx_power_curve_limits(sl_rail_handle_t rail_handle,
                                                   sl_rail_tx_power_mode_t pa_mode,
                                                   sl_rail_tx_power_t *p_max_power_ddbm,
                                                   sl_rail_tx_power_t *p_increment_ddbm);

/**
 * Initialize PA TX Curves.
 */
void sl_rail_util_pa_init(void);

/**
 * Get a pointer to the TX Power Config 2.4 GHz structure.
 *
 * @return A pointer to the TX Power Config stucture.
 */
sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_2p4ghz(void);

/**
 * Get a pointer to the TX Power Config Sub-GHz structure.
 *
 * @return A pointer to the TX Power Config stucture.
 */
sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_subghz(void);

/**
 * Get a pointer to the TX Power Config OFDM structure.
 *
 * @return A pointer to the TX Power Config stucture.
 */
sl_rail_tx_power_config_t *sl_rail_util_pa_get_tx_power_config_ofdm(void);

/**
 * Provide a channel config change callback capable of configuring the PA
 * correctly.
 *
 * @param[in] rail_handle The real RAIL handle being passed into this callback.
 * @param[in] p_entry A pointer to the channel config entry being switched
 *   to by hardware.
 */
void sl_rail_util_pa_on_channel_config_change(sl_rail_handle_t rail_handle,
                                              const sl_rail_channel_config_entry_t *p_entry);

/**
 * Convert raw values written to registers to decibel value (in units of
 * deci-dBm).
 *
 * @param[in] rail_handle A real RAIL instance handle.
 * @param[in] mode PA mode for which to convert.
 * @param[in] power_level A raw amplifier register value to be converted to
 *   deci-dBm.
 * @return raw amplifier values converted to units of deci-dBm.
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be re-characterized and implemented in an
 * overriding version of \ref sl_rail_util_pa_convert_raw_to_dbm(). For minimum code size and
 * best speed, use only raw values with the \ref sl_rail_set_tx_power() API and override this
 * function with a smaller function. In the weak version provided with the RAIL
 * library, rail_handle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle, or one
 * of the radio-generic ones, such as \ref SL_RAIL_EFR32_HANDLE.
 *
 * Although the implementation of this function may change, the signature
 * must be as declared here.
 */
sl_rail_tx_power_t sl_rail_util_pa_convert_raw_to_dbm(sl_rail_handle_t rail_handle,
                                                      sl_rail_tx_power_mode_t mode,
                                                      sl_rail_tx_power_level_t power_level);

/**
 * Convert the desired decibel value (in units of deci-dBm)
 * to raw integer values used by the TX amplifier registers.
 *
 * @param[in] rail_handle A real RAIL instance handle.
 * @param[in] mode PA mode for which to do the conversion.
 * @param[in] power_ddbm Desired dBm value in units of deci-dBm.
 * @return deci-dBm value converted to a raw
 *   integer value that can be used directly with \ref sl_rail_set_tx_power().
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be characterized and implemented in an
 * overriding version of \ref sl_rail_util_pa_convert_dbm_to_raw(). For minimum code size and
 * best speed use only raw values with the \ref sl_rail_set_tx_power() API and override this
 * function with a smaller function. In the weak version provided with the RAIL
 * library, rail_handle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle, or one
 * of the radio-generic ones, such as \ref SL_RAIL_EFR32_HANDLE.
 *
 * Although the implementation of this function may change, the signature
 * must be as declared here.
 */
sl_rail_tx_power_level_t sl_rail_util_pa_convert_dbm_to_raw(sl_rail_handle_t rail_handle,
                                                            sl_rail_tx_power_mode_t mode,
                                                            sl_rail_tx_power_t power_ddbm);

/**
 * Converts the desired decibel value (in units of deci-dBm)
 * to a \ref sl_rail_tx_power_setting_entry_t.
 *
 * @param[in] rail_handle A radio-generic or real RAIL instance handle.
 * @param[in] mode PA mode for which conversion is to be done.
 * @param[in] power_ddbm Desired dBm value in units of deci-dBm.
 * @param[out] p_power_setting A non-NULL pointer to
 *   \ref sl_rail_tx_power_setting_entry_t structure to be filled with the
 *   converted value.
 * @return Status code indicating success of the function call.
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be characterized and implemented in an
 * overriding version of \ref sl_rail_util_pa_convert_dbm_to_power_setting_entry().
 * In the weak version provided with the RAIL
 * library, rail_handle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle, or one
 * of the radio-generic ones, such as \ref SL_RAIL_EFR32_HANDLE.
 *
 * Although the implementation of this function may change, the signature
 * must be as declared here.
 */
sl_rail_status_t sl_rail_util_pa_convert_dbm_to_power_setting_entry(sl_rail_handle_t rail_handle,
                                                                    sl_rail_tx_power_mode_t mode,
                                                                    sl_rail_tx_power_t power_ddbm,
                                                                    sl_rail_tx_power_setting_entry_t *p_power_setting);

/**
 * Get the TX PA power setting table and related values.
 *
 * @param[in] rail_handle A real RAIL instance handle.
 * @param[in] mode PA mode for which to get the powersetting table.
 * @param[out] p_min_ddbm A pointer to a \ref sl_rail_tx_power_t.
 * @param[out] p_max_ddbm A pointer to a \ref sl_rail_tx_power_t.
 * @param[out] p_step_ddbm A pointer to a \ref sl_rail_tx_power_level_t, but in deci-dBm units.
 * @return Power setting table start address. When NULL is returned all out params
 *   above won't be set.
 *
 * The number of entries in the table can be calculated based on output
 * *p_min_ddbm, *p_max_ddbm, and *p_step_ddbm parameters.
 * For example, for *p_min_ddbm = 115 (11.5 dBm),
 * *p_max_ddbm = 300 (30 dBm), and *p_step_ddbm = 1, the number of
 * entries in table would be 186.
 */
const sl_rail_pa_power_setting_t *sl_rail_util_pa_get_power_setting_table(sl_rail_handle_t rail_handle,
                                                                          sl_rail_tx_power_mode_t mode,
                                                                          sl_rail_tx_power_t *p_min_ddbm,
                                                                          sl_rail_tx_power_t *p_max_ddbm,
                                                                          sl_rail_tx_power_level_t *p_step_ddbm);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Temporary RAIL 3.x API mappings to RAIL 2.x APIs
#include "pa_conversions_efr32.h"
#define sl_rail_tx_power_curves_vbat \
  RAIL_TxPowerCurvesVbat
#define sl_rail_tx_power_curves_dcdc \
  RAIL_TxPowerCurvesDcdc
#define sl_rail_init_tx_power_curves(a, b)                         \
  ((sl_rail_status_t)RAIL_InitTxPowerCurvesAlt((RAIL_Handle_t)(a), \
                                               (const RAIL_TxPowerCurvesConfigAlt_t *)(b)))
#define sl_rail_get_tx_power_curve_limits(a, b, c, d)                    \
  ((sl_rail_status_t)RAIL_GetTxPowerCurveLimits((RAIL_Handle_t)(a),      \
                                                (RAIL_TxPowerMode_t)(b), \
                                                (RAIL_TxPower_t *)(c),   \
                                                (RAIL_TxPower_t *)(d)))
#define sl_rail_util_pa_convert_raw_to_dbm(a, b, c)                  \
  ((sl_rail_tx_power_t)RAIL_ConvertRawToDbm((RAIL_Handle_t)(a),      \
                                            (RAIL_TxPowerMode_t)(b), \
                                            (RAIL_TxPowerLevel_t)(c)))
#define sl_rail_util_pa_convert_dbm_to_raw(a, b, c)                        \
  ((sl_rail_tx_power_level_t)RAIL_ConvertDbmToRaw((RAIL_Handle_t)(a),      \
                                                  (RAIL_TxPowerMode_t)(b), \
                                                  (RAIL_TxPower_t)(c)))
#define sl_rail_util_pa_convert_dbm_to_power_setting_entry(a, b, c, d)           \
  ((sl_rail_status_t)RAIL_ConvertDbmToPowerSettingEntry((RAIL_Handle_t)(a),      \
                                                        (RAIL_TxPowerMode_t)(b), \
                                                        (RAIL_TxPower_t)(c),     \
                                                        (RAIL_TxPowerSettingEntry_t *)(d)))
#define sl_rail_util_pa_get_power_setting_table(a, b, c, d, e)                            \
  ((const sl_rail_pa_power_setting_t *)RAIL_GetPowerSettingTable((RAIL_Handle_t)(a),      \
                                                                 (RAIL_TxPowerMode_t)(b), \
                                                                 (RAIL_TxPower_t *)(c),   \
                                                                 (RAIL_TxPower_t *)(d),   \
                                                                 (RAIL_TxPowerLevel_t *)(e)))

#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // PA_Curve_Conversions

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_PA_CONVERSIONS_EFR32_H
