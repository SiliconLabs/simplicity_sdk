/***************************************************************************//**
 * @file
 * @brief PA power conversion curve types used by Silicon Labs PA power
 *   conversion functions.
 * @details This file contains the curve types needed convert PA power levels
 *   to dBm powers.
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

#ifndef SL_RAIL_PA_CURVE_TYPES_EFR32_H
#define SL_RAIL_PA_CURVE_TYPES_EFR32_H

#include "sl_rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup PA_Curve_Conversions PA Curve Conversions
 * @ingroup PA
 * @{
 */

/**
 * @struct sl_rail_tx_power_curve_segment_t
 *
 * @brief Structure containing data defining each segment of the
 *   deci-dBm to raw power_level mapping curve fits.
 *
 * Note, these used in an equation of the form:
 *
 *    power_level * 1000 = slope * power + intercept
 *
 * power_level is the 0-252/0-248/1-7 values used in the sl_rail_get/set_tx_power()
 * functions, and power is the actual output power of the PA, specified
 * in deci-dBm.
 *
 * @note If the curves are generated with
 *   max_power_ddbm and increment_ddbm other than \ref SL_RAIL_TX_POWER_CURVE_DEFAULT_MAX and
 *   \ref SL_RAIL_TX_POWER_CURVE_DEFAULT_INCREMENT respectively, then the first
 *   \ref sl_rail_tx_power_curve_segment_t has its max_power_level equal to
 *   \ref SL_RAIL_TX_POWER_LEVEL_INVALID and its slope and intercept stores the
 *   max_power_ddbm and increment_ddbm in deci-dBm respectively.
 */
typedef struct sl_rail_tx_power_curve_segment {
  /** The highest power level that this segment will be used to convert */
  uint16_t max_power_level;
  /** slope of the line */
  int16_t slope;
  /** y-intercept of the line */
  int32_t intercept;
} sl_rail_tx_power_curve_segment_t;

/**
 * @enum sl_rail_pa_conversion_algorithm_t
 * @brief PA conversion algorithms types for converting between dBm and power levels
 */
SLI_RAIL_ENUM(sl_rail_pa_conversion_algorithm_t) {
  /** Piecewise linear fit. */
  SL_RAIL_PA_ALGORITHM_PIECEWISE_LINEAR = 0u,
  /** Mapping table between quantities. */
  SL_RAIL_PA_ALGORITHM_MAPPING_TABLE = 1u,
  /** Mapping table between pa power settings and dBm values. */
  SL_RAIL_PA_ALGORITHM_DBM_POWERSETTING_MAPPING_TABLE = 2u,
  /** Algorithm count.  Must be last. */
  SL_RAIL_PA_ALGORITHM_COUNT = 3u
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using SLI_RAIL_ENUM
#define SL_RAIL_PA_ALGORITHM_PIECEWISE_LINEAR               ((sl_rail_pa_conversion_algorithm_t) SL_RAIL_PA_ALGORITHM_PIECEWISE_LINEAR)
#define SL_RAIL_PA_ALGORITHM_MAPPING_TABLE                  ((sl_rail_pa_conversion_algorithm_t) SL_RAIL_PA_ALGORITHM_MAPPING_TABLE)
#define SL_RAIL_PA_ALGORITHM_DBM_POWERSETTING_MAPPING_TABLE ((sl_rail_pa_conversion_algorithm_t) SL_RAIL_PA_ALGORITHM_DBM_POWERSETTING_MAPPING_TABLE)
#define SL_RAIL_PA_ALGORITHM_COUNT                          ((sl_rail_pa_conversion_algorithm_t) SL_RAIL_PA_ALGORITHM_COUNT)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct sl_rail_tx_power_curve_t
 *
 * @brief Structure containing the min and max values for a given
 *   PA and voltage supply combination (in deci-dBm).
 */
typedef struct sl_rail_tx_power_curve {
  /** max deci-dBm value */
  sl_rail_tx_power_t max_power_ddbm;
  /** min deci-dBm value */
  sl_rail_tx_power_t min_power_ddbm;
  /**
   * Array of \ref sl_rail_pa_descriptor_t::segments \ref sl_rail_tx_power_curve_segment_t
   * structures for the deci-dBm to raw power level conversion fits.
   */
//Array does not have a size since it can be various sizes.
//No further fields allowed after this one.
  sl_rail_tx_power_curve_segment_t power_params[];
} sl_rail_tx_power_curve_t;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#if defined(SL_RAIL_UTIL_PA_POWERSETTING_TABLE_VERSION)
#if RAIL_SUPPORTS_COMMON_PA_INTERFACE
#if SL_RAIL_UTIL_PA_POWERSETTING_TABLE_VERSION == 1
/// The entry in the powersetting table have the below bitfields
/// |15-14 =sub-mode|13-8:unused|7-0:scalor(stripe+slice)|
/// Mask for submode
#define SLI_RAIL_UTIL_PA_TABLE_SUBMODE_MASK 0xC000UL
/// Shift for submode
#define SLI_RAIL_UTIL_PA_TABLE_SUBMODE_SHIFT 14U
/// Mask for scalor
#define SLI_RAIL_UTIL_PATABLE_SCALOR_MASK 0xFFU
/// Shift for scalor
#define SLI_RAIL_UTIL_PA_TABLE_SCALOR_SHIFT 0U
#endif //SL_RAIL_UTIL_PA_POWERSETTING_TABLE_VERSION == 1
#endif //RAIL_SUPPORTS_COMMON_PA_INTERFACE
#endif //defined(SL_RAIL_UTIL_PA_POWERSETTING_TABLE_VERSION)
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @struct sl_rail_power_conversion_t
 *
 * @brief Union containing a pointer to algorithm-specific conversion data.
 */
typedef union sl_rail_power_conversion {
  /**
   * Pointer to a powerCurve containing line segment data for the curves
   * corresponding to a specific PA.
   *
   * @note By the default conversion implementation, segments must be specified
   *   in decreasing power order. That is, the 0th entry of this array should be
   *   used to convert the highest power (levels). Segment at position n is valid
   *   from max_power_level+1 from the segment at n+1 (or 0 if n is array length -
   *   1) to max_power_level of segment n, inclusive.
   */
  const sl_rail_tx_power_curve_t *p_power_curve;
  /**
   * Lookup table for PA's which use the mapping table algorithm for converting
   * between deci-dBm and power levels.
   */
#if SL_RAIL_SUPPORTS_DBM_POWERSETTING_MAPPING_TABLE
#if SL_RAIL_SUPPORTS_COMMON_PA_INTERFACE
  const int16_t *p_mapping_table;
#else
  const int32_t *p_mapping_table;
#endif
#else
  const int16_t *p_mapping_table;
#endif
} sl_rail_power_conversion_t;

/**
 * @struct sl_rail_pa_descriptor_t
 *
 * @brief Struct containing specifics of PA configuration.
 *   PA descriptor as used in the PA conversion functions.
 */
typedef struct sl_rail_pa_descriptor {
  /** Algorithm used to map dBm to power levels for this PA. */
  sl_rail_pa_conversion_algorithm_t algorithm;
  /**
   * The number of piecewise segments provided to the PA in a piecewise linear
   * curve fit. The default is 8. Should be set to 0 when not using the
   * piecewise linear algorithm.
   */
  uint8_t segments;
  /** Min power level for this PA. */
  sl_rail_tx_power_level_t min;
  /** Max power level for this PA. */
  sl_rail_tx_power_level_t max;
#if SL_RAIL_SUPPORTS_DBM_POWERSETTING_MAPPING_TABLE
  /** step size in deci-dBm between entries in table. */
  sl_rail_tx_power_level_t step;
  /** structure padding. */
  uint8_t padding;
  /** structure padding. */
  uint16_t padding2;
  /** Min power in deci-dBm for this PA. */
  sl_rail_tx_power_t min_power_ddbm;
  /** Max power in deci-dBm for this PA. */
  sl_rail_tx_power_t max_power_ddbm;
#endif
  /** Union containing a pointer to algorithm-specific conversion data. */
  sl_rail_power_conversion_t conversion;
} sl_rail_pa_descriptor_t;

/**
 * @struct sl_rail_tx_power_curves_config_t
 *
 * @brief More generic structure containing information about
 *   piecewise linear curves and mapping tables, instead of specific PA's.
 */
typedef struct sl_rail_tx_power_curves_config {
  /** The curves for each PA. */
  sl_rail_pa_descriptor_t curves[SL_RAIL_NUM_PA];
  /** Signature used for validation of the curves configuruation. */
  uint32_t signature;
  /** PA VDD voltage, in millivolts. */
  uint16_t pa_voltage_mv;
} sl_rail_tx_power_curves_config_t;

/** @} */ // PA_Curve_Conversions

#ifdef __cplusplus
}
#endif

#endif // SL_RAIL_PA_CURVE_TYPES_EFR32_H
