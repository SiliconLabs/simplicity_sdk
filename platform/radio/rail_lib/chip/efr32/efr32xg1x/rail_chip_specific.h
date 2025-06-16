/***************************************************************************//**
 * @file
 * @brief This file contains the type definitions for EFR32xG1x chip-specific
 *   aspects of RAIL.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifdef  SLI_LIBRARY_BUILD

// This file should not be included when doing SLI_LIBRARY_BUILDs

#else//!SLI_LIBRARY_BUILD

#ifndef __RAIL_CHIP_SPECIFIC_H_
#if !defined(__RAIL_TYPES_H__) && !defined(DOXYGEN_SHOULD_SKIP_THIS)
#warning rail_chip_specific.h should only be included by rail_types.h
#include "rail_types.h" // Force rail_chip_specific.h only within rail_types.h
#else // __RAIL_TYPES_H__
/// Include guard
#define __RAIL_CHIP_SPECIFIC_H_

#include "rail_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * General Structures
 *****************************************************************************/
/**
 * @addtogroup General_EFR32XG1X EFR32xG1x
 * @ingroup General
 * @{
 * @brief Types specific to the EFR32xG1x for general configuration.
 */

#ifndef DOXYGEN_UNDOCUMENTED

/**
 * @def RAIL_EFR32XG1_STATE_BUFFER_BYTES
 * @brief The EFR32xG1 series size needed for
 *   \ref RAIL_StateBufferEntry_t::bufferBytes.
 */
#define RAIL_EFR32XG1_STATE_BUFFER_BYTES 440

/**
 * @def RAIL_EFR32XG12_STATE_BUFFER_BYTES
 * @brief The EFR32xG12 series size needed for
 *   \ref RAIL_StateBufferEntry_t::bufferBytes.
 */
#define RAIL_EFR32XG12_STATE_BUFFER_BYTES 456

/**
 * @def RAIL_EFR32XG13_STATE_BUFFER_BYTES
 * @brief The EFR32xG13 series size needed for
 *   \ref RAIL_StateBufferEntry_t::bufferBytes.
 */
#define RAIL_EFR32XG13_STATE_BUFFER_BYTES 464

/**
 * @def RAIL_EFR32XG14_STATE_BUFFER_BYTES
 * @brief The EFR32xG14 series size needed for
 *   \ref RAIL_StateBufferEntry_t::bufferBytes.
 */
#define RAIL_EFR32XG14_STATE_BUFFER_BYTES 464

/**
 * @def RAIL_STATE_BUFFER_BYTES
 * @brief The size needed for \ref RAIL_StateBufferEntry_t::bufferBytes
 *   on this platform for this radio. This compile-time size may be slightly
 *   larger than what \ref RAIL_GetStateBufferSize() determines at run-time.
 */
#if (_SILICON_LABS_32B_SERIES_1_CONFIG == 1)
#define RAIL_STATE_BUFFER_BYTES RAIL_EFR32XG1_STATE_BUFFER_BYTES
#elif (_SILICON_LABS_32B_SERIES_1_CONFIG == 2)
#define RAIL_STATE_BUFFER_BYTES RAIL_EFR32XG12_STATE_BUFFER_BYTES
#elif (_SILICON_LABS_32B_SERIES_1_CONFIG == 3)
#define RAIL_STATE_BUFFER_BYTES RAIL_EFR32XG13_STATE_BUFFER_BYTES
#elif (_SILICON_LABS_32B_SERIES_1_CONFIG == 4)
#define RAIL_STATE_BUFFER_BYTES RAIL_EFR32XG14_STATE_BUFFER_BYTES
#else
#define RAIL_STATE_BUFFER_BYTES 0 // Sate Doxygen
#error "Unsupported platform!"
#endif

#endif//DOXYGEN_UNDOCUMENTED

#ifndef DOXYGEN_UNDOCUMENTED

/**
 * @def RAIL_SEQ_IMAGE_1
 * @brief A macro for the first sequencer image.
 */
#define RAIL_SEQ_IMAGE_1                  1

/**
 * @def RAIL_SEQ_IMAGE_2
 * @brief A macro for the second sequencer image.
 */
#define RAIL_SEQ_IMAGE_2                  2

#if (_SILICON_LABS_32B_SERIES_1_CONFIG == 3)
/**
 * @def RAIL_SEQ_IMAGE_ZWAVE
 * @brief A chip-specific macro for the sequencer image used on EFR32xG13 OPNs
 *   with ZWave.
 */
#define RAIL_SEQ_IMAGE_ZWAVE          RAIL_SEQ_IMAGE_1

/**
 * @def RAIL_SEQ_IMAGE_HIGH_BW_PHY
 * @brief A chip-specific macro for the sequencer image used on EFR32xG13 OPNs
 *   with High BW PHYs supported.
 */
#define RAIL_SEQ_IMAGE_HIGH_BW_PHY         RAIL_SEQ_IMAGE_2

/**
 * @def RAIL_SEQ_IMAGE_COUNT
 * @brief A macro for the total number of sequencer images supported on the
 *   platform.
 */
#define RAIL_SEQ_IMAGE_COUNT              2

#else //(_SILICON_LABS_32B_SERIES_1_CONFIG != 3)

/**
 * @def RAIL_SEQ_IMAGE_DEFAULT
 * @brief A chip-specific macro for the default sequencer image on platforms
 *   that support only one sequencer image.
 */
#define RAIL_SEQ_IMAGE_DEFAULT            RAIL_SEQ_IMAGE_1

/**
 * @def RAIL_SEQ_IMAGE_COUNT
 * @brief A macro for the total number of sequencer images supported on the
 *   platform.
 */
#define RAIL_SEQ_IMAGE_COUNT              1
#endif //(_SILICON_LABS_32B_SERIES_1_CONFIG == 3)

/**
 * @typedef RAIL_TimerTick_t
 * @brief Internal RAIL hardware timer tick that drives the RAIL timebase.
 *   This wraps at the same time as the RAIL timebase, but at a value before
 *   the full 32 bit range.
 *
 * @note \ref RAIL_TimerTicksToUs() can be used to convert the delta between
 *   two \ref RAIL_TimerTick_t values to microseconds.
 */
typedef uint32_t RAIL_TimerTick_t;

/**
 * @def RAIL_GetTimerTick(channel)
 * @brief The RAIL hardware timer ticks value.
 *
 * @note channel is added for compatibility reasons and is ignored here.
 */
#define RAIL_GetTimerTick(channel) (*RAIL_TimerTick)

#ifndef SLI_LIBRAIL_ALIAS

/**
 * A global pointer to the memory address of the internal RAIL hardware timer
 * that drives the RAIL timebase.
 *
 * @note The corresponding timer tick value is not adjusted for overflow or the
 *   clock period, and will simply be a register read. On EFR32xG1x family of
 *   chips, ticks wrap in about 72 minutes and for all other series 1 they
 *   wrap in about 17 minutes, since it does not use the full 32-bit range.
 *   For more details, check the documentation for \ref RAIL_TimerTick_t.
 */
extern volatile uint32_t * const RAIL_TimerTick;

/**
 * A global pointer to the memory address of the internal RAIL hardware timer
 * that captures the latest RX packet reception time.
 * See \ref RAIL_TimerTick_t for its granularity and range.
 *
 * @note This would not include the RX chain delay, so may not exactly
 *   correspond to the \ref RAIL_Time_t packet timestamp available within
 *   \ref RAIL_RxPacketDetails_t::timeReceived which reflects the actual
 *   on-air time that the packet finished.
 */
extern volatile uint32_t * const RAIL_RxPacketTimestamp;

/**
 * Get elapsed time, in microseconds, between two \ref RAIL_TimerTick_t ticks.
 *
 * @param[in] startTick Tick recorded at the start of the operation.
 * @param[in] endTick Tick recorded at the end of the operation.
 * @return The elapsed time, in microseconds, between two timer ticks.
 */
RAIL_Time_t RAIL_TimerTicksToUs(RAIL_TimerTick_t startTick,
                                RAIL_TimerTick_t endTick);

/**
 * Get \ref RAIL_TimerTick_t tick corresponding to the \ref RAIL_Time_t time.
 *
 * @param[in] microseconds Time in microseconds.
 * @return The \ref RAIL_TimerTick_t tick corresponding to the
 *   \ref RAIL_Time_t time.
 */
RAIL_TimerTick_t RAIL_UsToTimerTicks(RAIL_Time_t microseconds);

#endif//SLI_LIBRAIL_ALIAS

#endif//DOXYGEN_UNDOCUMENTED

/** @} */ // end of group General_EFR32XG1X

/******************************************************************************
 * Multiprotocol
 *****************************************************************************/
/**
 * @addtogroup Multiprotocol_EFR32XG1X EFR32xG1x
 * @ingroup Multiprotocol
 * @{
 * @brief EFR32xG1x-specific multiprotocol support defines.
 */

/**
 * @def TRANSITION_TIME_US
 * @brief Time it takes to take care of protocol switching.
 */
#define TRANSITION_TIME_US 430

/** @} */ // end of group Multiprotocol_EFR32XG1X

/******************************************************************************
 * Calibration
 *****************************************************************************/
/**
 * @addtogroup Calibration_EFR32XG1X EFR32xG1x
 * @ingroup Calibration
 * @{
 * @brief EFR32xG1x-specific Calibrations.
 */

/**
 * @def RAIL_RF_PATHS
 * @brief Indicates the number of RF Paths supported.
 */
#define RAIL_RF_PATHS 1

#if     (RAIL_RF_PATHS > RAIL_MAX_RF_PATHS)
#error "Update rail_types.h RAIL_MAX_RF_PATHS"
#endif

/**
 * @struct RAIL_ChannelConfigEntryAttr
 * @brief A channel configuration entry attribute structure. Items listed
 *  are designed to be altered and updated during run-time.
 */
struct RAIL_ChannelConfigEntryAttr {
  /** IR calibration attributes specific to each channel configuration entry. */
  RAIL_RxIrCalValues_t calValues;
};

/** @} */ // end of group Calibration_EFR32XG1X

/******************************************************************************
 * Transmit
 *****************************************************************************/
/**
 * @addtogroup PA_EFR32XG1X EFR32xG1x
 * @ingroup PA
 * @{
 * @brief Types specific to the EFR32xG1x for dealing with the on-chip PAs.
 */

/**
 * The maximum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_2P4_LP mode.
 */
#define RAIL_TX_POWER_LEVEL_2P4_LP_MAX     (7U)
/**
 * The maximum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_2P4_HP mode.
 */
#define RAIL_TX_POWER_LEVEL_2P4_HP_MAX     (252U)
/**
 * The maximum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_SUBGIG mode.
 */
#define RAIL_TX_POWER_LEVEL_SUBGIG_HP_MAX (248U)
/**
 * The minimum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_2P4_LP mode.
 */
#define RAIL_TX_POWER_LEVEL_2P4_LP_MIN     (1U)
/**
 * The minimum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_2P4_HP mode.
 */
#define RAIL_TX_POWER_LEVEL_2P4_HP_MIN     (0U)
/**
 * The minimum valid value for the \ref RAIL_TxPowerLevel_t when in \ref
 * RAIL_TX_POWER_MODE_SUBGIG mode.
 */
#define RAIL_TX_POWER_LEVEL_SUBGIG_HP_MIN (0U)

/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_LP_MAX      RAIL_TX_POWER_LEVEL_2P4_LP_MAX
/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_HP_MAX      RAIL_TX_POWER_LEVEL_2P4_HP_MAX
/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_SUBGIG_MAX  RAIL_TX_POWER_LEVEL_SUBGIG_HP_MAX
/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_LP_MIN      RAIL_TX_POWER_LEVEL_2P4_LP_MIN
/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_HP_MIN      RAIL_TX_POWER_LEVEL_2P4_HP_MIN
/** Backwards compatibility define */
#define RAIL_TX_POWER_LEVEL_SUBGIG_MIN  RAIL_TX_POWER_LEVEL_SUBGIG_HP_MIN

/**
 * The number of PA's on this chip (including Virtual PAs).
 */
#define RAIL_NUM_PA (3U)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
// Only those supported per-platform are defined, for use with #ifdef in
// apps or librail code.
#define RAIL_TX_POWER_MODE_2P4GIG_HP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4GIG_HP)
#define RAIL_TX_POWER_MODE_2P4_HP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4_HP)
#define RAIL_TX_POWER_MODE_2P4GIG_LP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4GIG_LP)
#define RAIL_TX_POWER_MODE_2P4_LP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4_LP)
#define RAIL_TX_POWER_MODE_SUBGIG_HP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_SUBGIG_HP)
#define RAIL_TX_POWER_MODE_SUBGIG ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_SUBGIG)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group PA_EFR32XG1X

/******************************************************************************
 * RX Channel Hopping
 *****************************************************************************/
/**
 * @addtogroup Rx_Channel_Hopping_EFR32XG1X EFR32xG1x
 * @ingroup Rx_Channel_Hopping
 * @{
 * @brief EFR32xG1x-specific RX channel hopping.
 */

/// The static amount of memory needed per channel for channel hopping, measured
/// in 32 bit words, regardless of the size of radio configuration structures.
#define RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL (55U)

#if     (RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL \
         > RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL_WORST_CASE)
#error "Update rail_types.h RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL_WORST_CASE"
#endif

/** @} */  // end of group Rx_Channel_Hopping_EFR32XG1X

/******************************************************************************
 * Sleep Structures
 *****************************************************************************/
/**
 * @addtogroup Sleep_EFR32XG1X EFR32xG1x
 * @ingroup Sleep
 * @{
 * @brief EFR32xG1x-specific Sleeping.
 */

/// Default PRS channel to use when configuring sleep
#define RAILINT_TIMER_SYNC_PRS_CHANNEL_DEFAULT  (7U)

#if _SILICON_LABS_32B_SERIES_1_CONFIG >= 3
/// Default RTCC channel to use when configuring sleep
#define RAILINT_TIMER_SYNC_RTCC_CHANNEL_DEFAULT (1U)
#else
/// Default RTCC channel to use when configuring sleep
#define RAILINT_TIMER_SYNC_RTCC_CHANNEL_DEFAULT (0U)
#endif

/** @} */ // end of group Sleep_EFR32XG1X

/******************************************************************************
 * State Transitions
 *****************************************************************************/
/**
 * @addtogroup State_Transitions_EFR32XG1X EFR32xG1x
 * @ingroup State_Transitions
 * @{
 * @brief EFR32xG1x-specific State Transitions.
 */

/**
 * @def RAIL_MINIMUM_TRANSITION_US
 * @brief The minimum value for a consistent RAIL transition.
 * @note Transitions may need to be slower than this when using longer
 *   \ref RAIL_TxPowerConfig_t::rampTime values.
 */
#define RAIL_MINIMUM_TRANSITION_US (100U)

/**
 * @def RAIL_MAXIMUM_TRANSITION_US
 * @brief The maximum value for a consistent RAIL transition.
 */
#if     (_SILICON_LABS_32B_SERIES_1_CONFIG == 1)
#define RAIL_MAXIMUM_TRANSITION_US (13000U)
#else//!(_SILICON_LABS_32B_SERIES_1_CONFIG == 1)
#define RAIL_MAXIMUM_TRANSITION_US (1000000U)
#endif//(_SILICON_LABS_32B_SERIES_1_CONFIG == 1)

#ifndef DOXYGEN_UNDOCUMENTED
/**
 * Internal Radio State type mapping for EFR32 chips.
 */
typedef RAIL_RadioStateEfr32_t RAIL_RacRadioState_t;
#endif//DOXYGEN_UNDOCUMENTED

/** @} */ // end of group State_Transitions_EFR32XG1X

#ifdef __cplusplus
}
#endif

#endif // __RAIL_TYPES_H__

#endif // __RAIL_CHIP_SPECIFIC_H_

#endif // SLI_LIBRARY_BUILD
