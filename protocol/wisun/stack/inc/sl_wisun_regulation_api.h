/***************************************************************************//**
 * @file sl_wisun_regulation_api.h
 * @brief Wi-SUN Regional Regulation API
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_REGULATION_API_H
#define SL_WISUN_REGULATION_API_H

/// API version used to check compatibility (do not edit when using this header)
#define SL_WISUN_REGULATION_PARAMS_API_VERSION  1

/**************************************************************************//**
 * @addtogroup SL_WISUN_REGULATION_API Wi-SUN Regional Regulation API
 * @ingroup SL_WISUN_API
 * @brief Wi-SUN Regional Regulation API
 * @details
 * This API provides the parameters for regional regulation compliance.
 * It allows to set the duty cycle budget, thresholds for warnings and alerts,
 * and other parameters related to the regional regulation.
 * The Wi-SUN stack provides predefined parameter sets for the following regulations:
 * - ARIB (Japan)
 * - WPC (India)
 * - ETSI (Europe)
 *
 * The regional regulation is configured with sl_wisun_set_regulation_parameters().
 * Predefined parameter sets can be used for common regulatory requirements, they
 * can be selected with sl_wisun_set_regulation().
 *
 * Special attention must be taken with frame length when ARIB regulation is used
 * in Japan. The maximum length of a radio frame is indirectly constrained by the
 * channel bandwidth. This is because the regulation imposes a maximum transmission
 * duration per frame, which varies depending on the bandwidth used:
 * - with a 200 kHz bandwidth, frame duration must be less than 400 ms
 * - with a 400 kHz bandwidth, frame duration must be less than 200 ms
 * - with 600 and 800 kHz bandwidths, frame duration must be less than 100 ms
 *
 * The Wi-SUN stack doesn't currently enforce these constraints, and defers the
 * responsibility to the application.
 * Depending on the selected PHY, the application must ensure that the transmission
 * duration complies with the ARIB regulation by:
 * - reducing the frame length
 * - adjusting the 6LoWPAN MTU leading to fragmentation for larger packets
 * @{
 *****************************************************************************/

///  Regional regulation parameter set
SL_PACK_START(1)
typedef struct {
  /**
   * Version of this API.
   *
   * This field allows to store the parameters in an NVM and check on reload
   * that they are compatible with the stack if there was an update.
   */
  uint32_t version;
  /// Maximum Extended Directed Frame Exchange (EDFE) duration in milliseconds, 0 to forbid EDFE
  uint32_t max_edfe_duration_ms;
  /// Duty cycle budget in seconds per hour, -1 to disable duty cycle budget
  int16_t duty_cycle_budget_s;
  /// Duty cycle budget per channel in seconds per hour, -1 to disable duty cycle budget per channel
  int16_t duty_cycle_budget_per_channel_s;
  /// Duty cycle warning threshold in percent, -1 to disable duty cycle warning threshold
  int8_t duty_cycle_warning_threshold_percent;
  /// Duty cycle alert threshold in percent, -1 to disable duty cycle alert threshold
  int8_t duty_cycle_alert_threshold_percent;
  /// Skip ACKs from sum of transmissions
  uint8_t duty_cycle_skip_ack;
  /// Skip responses from sum of transmissions (ARIB conditions)
  uint8_t duty_cycle_arib_skip_ack;
  /// Enable Adaptive Power Control
  uint8_t enable_apc;
  /// Ensure a min IFS before ACKs (ARIB conditions)
  uint8_t enable_arib_channel_access;
  /// Fixed CCA threshold in dBm, INT8_MAX to use adaptive CCA threshold
  int8_t fixed_cca_threshold_dbm;
  /// Reserved, set to zero
  uint8_t reserved;
} SL_ATTRIBUTE_PACKED sl_wisun_regulation_params_t;
SL_PACK_END()

/***************************************************************************//**
 * @addtogroup SL_WISUN_REGULATION_PARAMETER_SETS Predefined regional regulation parameter sets
 *
 * Predefined regional regulation parameter sets for sl_wisun_set_regulation_parameters().
 *
 * These parameter sets can be used either as-is or used as an initialization
 * value for an application-specific parameter set.
 *
 * @{
 ******************************************************************************/

/// Default parameter set for no regional regulation.
/// This parameter set is used when no regional regulation is applied.
static const sl_wisun_regulation_params_t SL_WISUN_REGULATION_PARAMS_NONE = {
  .version = SL_WISUN_REGULATION_PARAMS_API_VERSION,
  .max_edfe_duration_ms = UINT32_MAX,  // No EDFE duration limit
  .duty_cycle_budget_s = -1,
  .duty_cycle_budget_per_channel_s = -1,
  .duty_cycle_warning_threshold_percent = -1,
  .duty_cycle_alert_threshold_percent = -1,
  .duty_cycle_skip_ack = false,
  .duty_cycle_arib_skip_ack = false,
  .enable_apc = false,
  .enable_arib_channel_access = false,
  .fixed_cca_threshold_dbm = INT8_MAX,  // Use adaptive CCA threshold
};

/// Parameter set for ARIB regional regulation.
/// This parameter set is used for Japan and other ARIB compliant regions.
static const sl_wisun_regulation_params_t SL_WISUN_REGULATION_PARAMS_ARIB = {
  .version = SL_WISUN_REGULATION_PARAMS_API_VERSION,
  .max_edfe_duration_ms = 0,  // EDFE not supported
  .duty_cycle_budget_s = 720,  // 720s per arbitrary one hour per radio equipment
  .duty_cycle_budget_per_channel_s = 360,  // 360s per arbitrary one hour per channel
  .duty_cycle_warning_threshold_percent = 50,
  .duty_cycle_alert_threshold_percent = 90,
  .duty_cycle_skip_ack = false,
  .duty_cycle_arib_skip_ack = true,
  .enable_apc = false,
  .enable_arib_channel_access = true,  // ARIB-STD-T108, Part 2 Section 3.4.1
  .fixed_cca_threshold_dbm = -80,  // ARIB-STD-T108, Part 2 Section 3.4.2
};

/// Parameter set for WPC regional regulation.
/// This parameter set is used for India and other WPC compliant regions.
static const sl_wisun_regulation_params_t SL_WISUN_REGULATION_PARAMS_WPC = {
  .version = SL_WISUN_REGULATION_PARAMS_API_VERSION,
  .max_edfe_duration_ms = UINT32_MAX,  // No EDFE duration limit
  .duty_cycle_budget_s = -1,
  .duty_cycle_budget_per_channel_s = -1,
  .duty_cycle_warning_threshold_percent = -1,
  .duty_cycle_alert_threshold_percent = -1,
  .duty_cycle_skip_ack = false,
  .duty_cycle_arib_skip_ack = false,
  .enable_apc = true,
  .enable_arib_channel_access = false,
  .fixed_cca_threshold_dbm = INT8_MAX,  // Use adaptive CCA threshold
};

/// Parameter set for ETSI regional regulation.
/// This parameter set is used for Europe and other ETSI compliant regions.
static const sl_wisun_regulation_params_t SL_WISUN_REGULATION_PARAMS_ETSI = {
  .version = SL_WISUN_REGULATION_PARAMS_API_VERSION,
  .max_edfe_duration_ms = UINT32_MAX,  // No EDFE duration limit
  .duty_cycle_budget_s = 90,  // 10% (LBR) and 2.5% (RN) measured over a period of 60 minutes
  .duty_cycle_budget_per_channel_s = -1,
  .duty_cycle_warning_threshold_percent = 50,
  .duty_cycle_alert_threshold_percent = 90,
  .duty_cycle_skip_ack = false,
  .duty_cycle_arib_skip_ack = false,
  .enable_apc = true,
  .enable_arib_channel_access = false,
  .fixed_cca_threshold_dbm = INT8_MAX,  // Use adaptive CCA threshold
};

/** @} (end SL_WISUN_REGULATION_PARAMETER_SETS) */

/** @} (end SL_WISUN_REGULATION_API) */

#endif  /* SL_WISUN_REGULATION_API_H */
