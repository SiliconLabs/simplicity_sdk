/***************************************************************************//**
 * @file
 * @brief This file contains the type definitions for efr32xg2x chip specific
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

#ifndef __RAIL_CHIP_SPECIFIC_H_
#if !defined(__RAIL_TYPES_H__) && !defined(DOXYGEN_SHOULD_SKIP_THIS)
#warning rail_chip_specific.h should only be included by rail_types.h
#include "rail_types.h" // Force rail_chip_specific.h only within rail_types.h
#else // __RAIL_TYPES_H__
/// Include guard
#define __RAIL_CHIP_SPECIFIC_H_

#include "rail_features.h"
#ifdef RAIL_INTERNAL_BUILD
#include "rail_chip_specific_internal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup General_EFR32XG3 EFR32xG3
 * @{
 * @brief EFR32xG3-specific initialization data types
 * @ingroup General
 */

/**
 * A placeholder for a chip-specific RAIL handle. Using NULL as a RAIL handle is
 * not recommended. As a result, another value that can't be de-referenced is used.
 *
 * This generic handle can and should be used for RAIL APIs that are called
 * prior to RAIL initialization.
 */
#define RAIL_EFR32_HANDLE ((RAIL_Handle_t)0xFFFFFFFFUL)

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @def RAIL_SIXG301_STATE_BUFFER_BYTES
 * @brief The SIXG301 series size needed for
 *   \ref RAIL_StateBufferEntry_t::bufferBytes.
 */
#define RAIL_SIXG301_STATE_BUFFER_BYTES 576

#ifndef RAIL_STATE_BUFFER_BYTES
/**
 * @def RAIL_STATE_BUFFER_BYTES
 * @brief The size needed for \ref RAIL_StateBufferEntry_t::bufferBytes
 *   on this platform for this radio. This compile-time size may be slightly
 *   larger than what \ref RAIL_GetStateBufferSize() determines at run-time.
 */
#if (_SILICON_LABS_32B_SERIES_3_CONFIG == 1)
#define RAIL_STATE_BUFFER_BYTES RAIL_SIXG301_STATE_BUFFER_BYTES
#else
#define RAIL_STATE_BUFFER_BYTES 0 // Sate Doxygen
#error "Unsupported platform!"
#endif //_SILICON_LABS_32B_SERIES_3_CONFIG
#endif //#ifndef RAIL_STATE_BUFFER_BYTES

/**
 * Redefined to use \ref RAIL_RadioStateSix3x_t instead of \ref RAIL_RadioStateEfr32_t
 */
#ifdef RAIL_RAC_STATE_NONE
#undef RAIL_RAC_STATE_NONE
#define RAIL_RAC_STATE_NONE RAIL_RAC_STATE_SIX3X_NONE
#endif

/**
 * @typedef RAIL_TimerTick_t
 * @brief Internal RAIL hardware timer tick that drives the RAIL timebase.
 *
 * @note \ref RAIL_TimerTicksToUs() can be used to convert the delta between
 *   two \ref RAIL_TimerTick_t values to microseconds.
 */
typedef uint64_t RAIL_TimerTick_t;

/**
 * @typedef RAIL_GetTimerTick_t
 * @brief A pointer to a function to RAIL internal timer tick.
 *
 * @param[in] timerChannel \ref RAIL_TimerTickType_t timer tick
 *   channel to read.
 * @return RAIL timer tick, type \ref RAIL_TimerTick_t, corresponding to the
 *   timer channel.
 */
typedef RAIL_TimerTick_t (*RAIL_GetTimerTick_t) (RAIL_TimerTickType_t timerChannel);

/**
 * Function pointer of type \ref RAIL_GetTimerTick_t to get RAIL timer
 * tick.
 *
 * @note This function pointer is only supported for series-3 chips and will be
 *   NULL otherwise.
 */
extern RAIL_GetTimerTick_t RAIL_GetTimerTick;

/**
 * A global pointer to the memory address of the internal RAIL hardware timer
 * that drives the RAIL timebase.
 *
 * @note The corresponding timer tick value is not adjusted for overflow or the
 *   clock period, and will simply be a register read. The ticks wrap after about
 *   9 minutes on series 3 chips.
 *   For more details, check the documentation for \ref RAIL_TimerTick_t.
 */
extern const volatile uint32_t *RAIL_TimerTick;

/**
 * A global pointer to the memory address of the internal RAIL hardware timer
 * that captures the latest RX packet reception time.  This would not include
 * the RX chain delay, so may not be equal to the packet timestamp, passed to
 * the application, representing the actual on-air time the packet finished.
 *
 * @note The corresponding timer tick value is not adjusted for overflow or the
 *   clock period, and will simply be a register read. The ticks wrap after about
 *   9 minutes on series 3 chips.
 *   For more details, check the documentation for \ref RAIL_TimerTick_t.
 */
extern const volatile uint32_t *RAIL_RxPacketTimestamp;

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
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group General_EFR32XG2

// -----------------------------------------------------------------------------
// Multiprotocol
// -----------------------------------------------------------------------------
/**
 * @addtogroup Multiprotocol_EFR32 EFR32
 * @{
 * @brief EFR32-specific multiprotocol support defines
 * @ingroup Multiprotocol
 */

/**
 * @def TRANSITION_TIME_US
 * @brief Time it takes to take care of protocol switching.
 */
#define TRANSITION_TIME_US 510

/** @} */ // end of group Multiprotocol_EFR32

// -----------------------------------------------------------------------------
// Calibration
// -----------------------------------------------------------------------------
/**
 * @addtogroup Calibration_EFR32XG3X EFR32XG3X
 * @{
 * @brief EFR32XG3X-specific Calibrations
 * @ingroup Calibration
 */

/**
 * @def RAIL_RF_PATHS_2P4GIG
 * @brief Indicates the number of 2.4 GHz RF Paths suppported
 */
#if _SILICON_LABS_32B_SERIES_3_CONFIG == 1
#define RAIL_RF_PATHS_2P4GIG 1
#else
#define RAIL_RF_PATHS_2P4GIG 0
#endif

/**
 * @def RAIL_RF_PATHS_SUBGIG
 * @brief Indicates the number of sub-GHz RF Paths supported
 */
#define RAIL_RF_PATHS_SUBGIG 0

/**
 * @def RAIL_RF_PATHS
 * @brief Indicates the number of RF Paths supported
 */
#define RAIL_RF_PATHS (RAIL_RF_PATHS_SUBGIG + RAIL_RF_PATHS_2P4GIG)

/**
 * @def RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS
 * @brief Indicates this version of RAIL supports IR calibration on multiple RF paths
 * Needed for backwards compatibility.
 */
#if RAIL_RF_PATHS > 1
#define RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS 1
#else
#ifdef  DOXYGEN_SHOULD_SKIP_THIS // Leave undefined except for doxygen
#define RADIO_CONFIG_ENABLE_IRCAL_MULTIPLE_RF_PATHS 0
#endif//DOXYGEN_SHOULD_SKIP_THIS
#endif // RAIL_RF_PATHS

/**
 * @struct RAIL_ChannelConfigEntryAttr
 * @brief A channel configuration entry attribute structure. Items listed
 *  are designed to be altered and updated during run-time.
 */
struct RAIL_ChannelConfigEntryAttr {
  /** IR calibration attributes specific to each channel configuration entry. */
#if     RAIL_SUPPORTS_OFDM_PA
  RAIL_IrCalValues_t calValues;
 #else//!RAIL_SUPPORTS_OFDM_PA
  RAIL_RxIrCalValues_t calValues;
 #endif//RAIL_SUPPORTS_OFDM_PA
};

/** @} */ // end of group Calibration_EFR32

// -----------------------------------------------------------------------------
// Transmit
// -----------------------------------------------------------------------------
/**
 * @addtogroup PA_EFR32XG3X EFR32XG3X
 * @{
 * @ingroup PA
 * @brief Types specific to the EFR32 for dealing with the on-chip PAs.
 */

#if _SILICON_LABS_32B_SERIES_3_CONFIG == 1
/**
 * The maximum valid value for the \ref RAIL_TxPowerLevel_t for both \ref
 * RAIL_TX_POWER_MODE_2P4GIG_HP and \ref RAIL_TX_POWER_MODE_2P4GIG_LP modes.
 */
#define RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MAX     (95U)
/**
 * The minimum valid value for the \ref RAIL_TxPowerLevel_t for both \ref
 * RAIL_TX_POWER_MODE_2P4GIG_HP and \ref RAIL_TX_POWER_MODE_2P4GIG_LP modes.
 */
#define RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MIN     (0U)
/**
 * Legacy defines for High Power (HP) and Low Power (LP) modes.
 * These defines are used for setting the minimum and maximum transmit power levels.
 */
#define RAIL_TX_POWER_LEVEL_2P4_LP_MIN        (RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MIN)
#define RAIL_TX_POWER_LEVEL_2P4_LP_MAX        (RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MAX)
#define RAIL_TX_POWER_LEVEL_2P4_HP_MIN        (RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MIN)
#define RAIL_TX_POWER_LEVEL_2P4_HP_MAX        (RAIL_TX_POWER_LEVEL_2P4GIG_HP_LP_MAX)
#else
#error "RAIL_TX_POWER_LEVEL not defined for this device"
#endif //_SILICON_LABS_32B_SERIES_3_CONFIG

/**
 * The number of PA's on this chip.
 */
#if (_SILICON_LABS_32B_SERIES_3_CONFIG == 1)
#define RAIL_NUM_PA (2U)
#else
#error "RAIL_NUM_PA undefined for platform"
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
// Only those supported per-platform are defined, for use with #ifdef in
// apps or librail code.
#if     RAIL_SUPPORTS_2P4GHZ_BAND
#define RAIL_TX_POWER_MODE_2P4GIG_HP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4GIG_HP)
#define RAIL_TX_POWER_MODE_2P4_HP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4_HP)
#define RAIL_TX_POWER_MODE_2P4GIG_LP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4GIG_LP)
#define RAIL_TX_POWER_MODE_2P4_LP ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4_LP)
#define RAIL_TX_POWER_MODE_2P4GIG_HIGHEST ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4GIG_HIGHEST)
#define RAIL_TX_POWER_MODE_2P4_HIGHEST ((RAIL_TxPowerMode_t) RAIL_TX_POWER_MODE_2P4_HIGHEST)
#endif//RAIL_SUPPORTS_2P4GHZ_BAND

/** Convenience macro for any mapping table mode. */
#define RAIL_POWER_MODE_IS_ANY_DBM_POWERSETTING_MAPPING_TABLE(x) \
  (((x) == RAIL_TX_POWER_MODE_2P4GIG_HP)                         \
   || ((x) == RAIL_TX_POWER_MODE_2P4GIG_LP))
#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group PA_EFR32

/******************************************************************************
 * User Sequencer Structures
 *****************************************************************************/
/**
 * @addtogroup User Sequencer
 * @{
 */

/**
 * TODO: Document and cleanup.
 */
typedef struct RAIL_UserCommonGlobal {
  void *pLocSeqVirtualReg;
  void *pLocSeqTiming;
  void *pLocUserSeqConfig;
  void *pLocRtccsyncConfig;
  void *pLocStateVarConfig;
  void *pLocGenericPhyConfig;
  void *pLocpSeqTimestamp;
} RAIL_UserCommonGlobal_t;

/**
 * TODO: Document and cleanup.
 */
typedef struct UserSeqShMem {
  /**
   * pointer to the start of M33 and sequencer shared memory.
   * TBD: This part of memory should be moved to user memory intead of in generic_seq_common.h
   */
  void *pStart;
  /**
   * size of shared memory in bytes.
   * TBD: This part of memory should be moved to user memory intead of in generic_seq_common.h
   */
  uint32_t szBytes;
} UserSeqShMem_t;

/**
 * @struct RAIL_SeqUserAppInfo_t
 * @brief RAIL sequencer user application structure
 *
 * This structure describe the user application that is loaded on the sequencer.
 */
// TBD: Is this the right place for this strucutre?  This should probably stays internal?????
typedef void (*RAIL_SEQ_UserStartMain_t)(void);
typedef struct {
  uint32_t version;         // Version of the structure? do we need this? how would this work?
  uint8_t *pProgramStartMem;// pointer to the start of user executable in memory
  uint8_t *pProgramStartLoc;// pointer to the start of user executable storage
  uint32_t programSzB;      // size of user executable in bytes
  RAIL_SEQ_UserStartMain_t programInitStart;// main user function entry
  uint8_t *pDataStartMem;   // pointer to the start of user initialized data in memory
  uint8_t *pDataStartLoc;   // pointer to the start of user initialized data storage
  uint32_t dataSzB;         // size of user initialized data in bytes
  uint8_t *pScratchStartMem;// pointer to the start of user un-initialized data in memory
  uint32_t scratchSzB;      // size of user un-initialized data in bytes
} RAIL_SeqUserAppInfo_t;

/** @} */ // end of group User Sequencer

/******************************************************************************
 * State Transitions
 *****************************************************************************/
/**
 * @addtogroup State_Transitions
 * @{
 */

/**
 * @enum RAIL_RadioStateSix3x_t
 * @brief Detailed Series-3 Radio state machine statuses.
 */
RAIL_ENUM(RAIL_RadioStateSix3x_t) {
  RAIL_RAC_STATE_SIX3X_OFF,         /**< Radio is off. */
  RAIL_RAC_STATE_SIX3X_RXWARM,      /**< Radio is enabling the receiver. */
  RAIL_RAC_STATE_SIX3X_RXSEARCH,    /**< Radio is listening for incoming frames. */
  RAIL_RAC_STATE_SIX3X_RXFRAME,     /**< Radio is receiving a frame. */
  RAIL_RAC_STATE_SIX3X_RXWRAPUP,    /**< Radio is going to RX Wrapup mode after
                                         receiving a frame. */
  RAIL_RAC_STATE_SIX3X_TXWARM,      /**< Radio is enabling transmitter. */
  RAIL_RAC_STATE_SIX3X_TX,          /**< Radio is transmitting data. */
  RAIL_RAC_STATE_SIX3X_TXWRAPUP,    /**< Radio is going to TX Wrapup mode after
                                         transmitting a frame. */
  RAIL_RAC_STATE_SIX3X_SHUTDOWN,    /**< Radio is powering down receiver and going to
                                         OFF state. */
  RAIL_RAC_STATE_SIX3X_POR,         /**< Radio power-on-reset state (EFR32xG22 and later) */
  RAIL_RAC_STATE_SIX3X_NONE         /**< Invalid Radio state, must be the last entry. */
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Self-referencing defines minimize compiler complaints when using RAIL_ENUM
#define RAIL_RAC_STATE_SIX3X_OFF         ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_OFF)
#define RAIL_RAC_STATE_SIX3X_RXWARM      ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_RXWARM)
#define RAIL_RAC_STATE_SIX3X_RXSEARCH    ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_RXSEARCH)
#define RAIL_RAC_STATE_SIX3X_RXFRAME     ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_RXFRAME)
#define RAIL_RAC_STATE_SIX3X_RXWRAPUP    ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_RXWRAPUP)
#define RAIL_RAC_STATE_SIX3X_TXWARM      ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_TXWARM)
#define RAIL_RAC_STATE_SIX3X_TX          ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_TX)
#define RAIL_RAC_STATE_SIX3X_TXWRAPUP    ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_TXWRAPUP)
#define RAIL_RAC_STATE_SIX3X_SHUTDOWN    ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_SHUTDOWN)
#define RAIL_RAC_STATE_SIX3X_POR         ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_POR)
#define RAIL_RAC_STATE_SIX3X_NONE        ((RAIL_RadioStateEfr32_t) RAIL_RAC_STATE_SIX3X_NONE)
#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * Redefined here for use in common source code \ref RAIL_RadioStateSix3x_t
 */
typedef RAIL_RadioStateSix3x_t RAIL_RacRadioState_t;

/** @} */ // end of group State_Transitions

/******************************************************************************
 * RX Channel Hopping
 *****************************************************************************/
/**
 * @addtogroup Rx_Channel_Hopping RX Channel Hopping
 * @{
 */

/// The static amount of memory needed per channel for channel hopping, measured
/// in 32 bit words, regardless of the size of radio configuration structures.
#define RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL (54U)

/** @} */  // end of group Rx_Channel_Hopping

/**
 * @addtogroup Sleep
 * @{
 */

/// Default PRS channel to use when configuring sleep
#define RAIL_TIMER_SYNC_PRS_CHANNEL_DEFAULT  (7U)

/// Default RTCC channel to use when configuring sleep
#define RAIL_TIMER_SYNC_RTCC_CHANNEL_DEFAULT (0U)

/** @} */ // end of group Sleep

/**
 * @addtogroup Data_Management_EFR32XG3X EFR32XG3X
 * @{
 * @ingroup Data_Management
 */

/// Fixed-width type indicating the needed alignment for RX and TX FIFOs. Note
/// that docs.silabs.com will incorrectly indicate that this is always a
/// uint8_t, but it does vary across RAIL platforms.
#if _SILICON_LABS_32B_SERIES_3_CONFIG == 1
#define RAIL_FIFO_ALIGNMENT_TYPE uint32_t
#endif

/// Alignment that is needed for the RX and TX FIFOs.
#define RAIL_FIFO_ALIGNMENT (sizeof(RAIL_FIFO_ALIGNMENT_TYPE))

/** @} */ // end of group Data_Management_EFR32

/**
 * @addtogroup State_Transitions_EFR32XG2X
 * @{
 * @ingroup State_Transitions
 */

/**
 * @def RAIL_MINIMUM_TRANSITION_US
 * @brief The minimum value for a consistent RAIL transition
 * @note Transitions may need to be slower than this when using longer
 *   \ref RAIL_TxPowerConfig_t::rampTime values
 */
#define RAIL_MINIMUM_TRANSITION_US (100U)

/**
 * @def RAIL_MAXIMUM_TRANSITION_US
 * @brief The maximum value for a consistent RAIL transition
 */
#define RAIL_MAXIMUM_TRANSITION_US (1000000U)

/** @} */ // end of group State_Transitions_EFR32

/**
 * Load sequencer user application to memory.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pSeqUserApp pointer to the structure describing the sequencer
 *   user application metadata.
 * @return Status code indicating success of the function call.
 *
 * Attempts to load sequencer user application to the sequencer memory and
 * execute its initialization function.
 */
RAIL_Status_t RAIL_LoadUserSeqApp(RAIL_Handle_t railHandle,
                                  RAIL_SeqUserAppInfo_t *pSeqUserApp);

/**
 * Send a shutdown message to the sequencer user application.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call.
 *
 */
RAIL_Status_t RAIL_ShutdownUserSeqApp(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports User Sequencer.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if User Sequencer is supported; false otherwise.
 */
bool RAIL_SupportsUserSequencer(RAIL_Handle_t railHandle);

/**
 * Prints the common global variables.
 *
 * This function sets the pointers of the common global variables to their corresponding values.
 *
 * @param[in] railHandle A handle of the RAIL instance.
 * @param[in] pCommonGlobal A pointer to the common global variables.
 * @return RAIL_Status_t Returns RAIL_STATUS_NO_ERROR on success or an error code on failure.
 */
RAIL_Status_t RAIL_USER_printCommonGlobal(RAIL_Handle_t railHandle,
                                          RAIL_UserCommonGlobal_t *pCommonGlobal);

/**
 * TODO: Document and cleanup.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pMsg
 * @return RAIL_Status_t Returns RAIL_STATUS_NO_ERROR on success or an error code on failure.
 */
RAIL_Status_t RAIL_USER_GetMboxMsg(RAIL_Handle_t railHandle,
                                   uint32_t *pMsg);

/**
 * TODO: Document and cleanup.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] msg
 * @return RAIL_Status_t Returns RAIL_STATUS_NO_ERROR on success or an error code on failure.
 */
RAIL_Status_t RAIL_USER_SendMbox(RAIL_Handle_t railHandle,
                                 uint32_t msg);

#ifdef __cplusplus
}
#endif

#endif //__RAIL_TYPES_H__

#endif //__RAIL_CHIP_SPECIFIC_H_
