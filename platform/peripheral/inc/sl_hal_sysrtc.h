/***************************************************************************//**
 * @file
 * @brief System Real Time Counter (SYSRTC) peripheral API
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

#ifndef SL_HAL_SYSRTC_H
#define SL_HAL_SYSRTC_H

#include "em_device.h"

#if defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0)
#include <stdbool.h>
#include "sl_code_classification.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "sl_hal_sysrtc_compat.h"
#include "sl_enum.h"
#include "sl_assert.h"

/***************************************************************************//**
 * @addtogroup sysrtc SYSRTC - System RTC
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/// Minimum compare channels for SYSRTC group.
#define SYSRTC_GROUP_MIN_CHANNEL_COMPARE  1u
/// Maximum compare channels for SYSRTC group.
#define SYSRTC_GROUP_MAX_CHANNEL_COMPARE  3u

/// Minimum capture channels for SYSRTC group.
#define SYSRTC_GROUP_MIN_CHANNEL_CAPTURE  0u
/// Maximum capture channels for SYSRTC group.
#define SYSRTC_GROUP_MAX_CHANNEL_CAPTURE  1u

/// SYSRTC group number.
#if !defined(SYSRTC_GROUP_NUMBER)
#define SYSRTC_GROUP_NUMBER   1u
#endif

/// Validation of valid SYSRTC group for assert statements.
#define SYSRTC_GROUP_VALID(group)    ((unsigned)(group) < SYSRTC_GROUP_NUMBER)

/*******************************************************************************
 *********************************   ENUM   ************************************
 ******************************************************************************/

/// Capture input edge select.
SL_ENUM(sl_hal_sysrtc_capture_edge_t) {
  SL_HAL_SYSRTC_CAPTURE_EDGE_RISING = 0,  ///< Rising edges detected.
  SL_HAL_SYSRTC_CAPTURE_EDGE_FALLING,     ///< Falling edges detected.
  SL_HAL_SYSRTC_CAPTURE_EDGE_BOTH         ///< Both edges detected.
};

/// Compare match output action mode.
SL_ENUM(sl_hal_sysrtc_compare_match_out_action_t) {
  SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_CLEAR = 0, ///< Clear output.
  SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_SET,       ///< Set output.
  SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_PULSE,     ///< Generate a pulse.
  SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_TOGGLE,    ///< Toggle output.
  SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_CMPIF      ///< Export CMPIF.
};

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/// SYSRTC configuration structure.
typedef struct {
  bool enable_debug_run;      ///< Counter shall keep running during debug halt.
} sl_hal_sysrtc_init_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Typedef for configuration structure used for backward compatibility purposes.
typedef sl_hal_sysrtc_init_t sl_hal_sysrtc_config_t;
/** @endcond */

/// Suggested default values for SYSRTC configuration structure.
#define SL_HAL_SYSRTC_INIT_DEFAULT                   \
  {                                                  \
    false, /* Disable updating during debug halt. */ \
  }

/// Compare channel configuration structure.
typedef struct {
  sl_hal_sysrtc_compare_match_out_action_t  compare_match_out_action; ///< Compare mode channel match output action.
} sl_hal_sysrtc_group_channel_compare_config_t;

/// Capture channel configuration structure.
typedef struct {
  sl_hal_sysrtc_capture_edge_t  capture_input_edge; ///< Capture mode channel input edge.
} sl_hal_sysrtc_group_channel_capture_config_t;

/// Group configuration structure.
typedef struct {
  bool compare_channel0_enable;                                                  ///< Enable/Disable compare channel 0.
  bool compare_channel1_enable;                                                  ///< Enable/Disable compare channel 1.
  bool compare_channel2_enable;                                                  ///< Enable/Disable compare channel 2.
  bool capture_channel0_enable;                                                  ///< Enable/Disable capture channel 0.
  sl_hal_sysrtc_group_channel_compare_config_t const *p_compare_channel0_config; ///< Pointer to compare channel 0 config.
  sl_hal_sysrtc_group_channel_compare_config_t const *p_compare_channel1_config; ///< Pointer to compare channel 1 config.
  sl_hal_sysrtc_group_channel_compare_config_t const *p_compare_channel2_config; ///< Pointer to compare channel 2 config.
  sl_hal_sysrtc_group_channel_capture_config_t const *p_capture_channel0_config; ///< Pointer to capture channel 0 config.
} sl_hal_sysrtc_group_init_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Typedef for configuration structure used for backward compatibility purposes.
typedef sl_hal_sysrtc_group_init_t sl_hal_sysrtc_group_config_t;
/** @endcond */

/// Suggested default values for compare channel configuration structure.
#define SL_HAL_SYSRTC_GROUP_CHANNEL_COMPARE_INIT_DEFAULT \
  {                                                      \
    SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_PULSE         \
  }

/// Compare channel configuration for starting HFXO using PRS.
#define SYSRTC_GROUP_CHANNEL_COMPARE_CONFIG_EARLY_WAKEUP \
  {                                                      \
    SL_HAL_SYSRTC_COMPARE_MATCH_OUT_ACTION_CMPIF         \
  }

/// Suggested default values for capture channel configuration structure.
#define SL_HAL_SYSRTC_GROUP_CHANNEL_CAPTURE_INIT_DEFAULT \
  {                                                      \
    SL_HAL_SYSRTC_CAPTURE_EDGE_RISING                    \
  }

/// Suggested default values for SYSRTC group configuration structure.
#define SL_HAL_SYSRTC_GROUP_INIT_DEFAULT                                        \
  {                                                                             \
    true,  /* Enable compare channel 0. */                                      \
    false, /* Disable compare channel 1. */                                     \
    false, /* Disable compare channel 2. */                                     \
    false, /* Disable capture channel 0. */                                     \
    NULL,  /* NULL Pointer to configuration structure for compare channel 0. */ \
    NULL,  /* NULL Pointer to configuration structure for compare channel 1. */ \
    NULL,  /* NULL Pointer to configuration structure for compare channel 2. */ \
    NULL   /* NULL Pointer to configuration structure for capture channel 0. */ \
  }

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Alias for deprecated macro names used for backward compatibility purposes.
#define SYSRTC_CONFIG_DEFAULT                         SL_HAL_SYSRTC_INIT_DEFAULT
#define SYSRTC_GROUP_CHANNEL_COMPARE_CONFIG_DEFAULT   SL_HAL_SYSRTC_GROUP_CHANNEL_COMPARE_INIT_DEFAULT
#define SYSRTC_GROUP_CHANNEL_CAPTURE_CONFIG_DEFAULT   SL_HAL_SYSRTC_GROUP_CHANNEL_CAPTURE_INIT_DEFAULT
#define SYSRTC_GROUP_CONFIG_DEFAULT                   SL_HAL_SYSRTC_GROUP_INIT_DEFAULT
/** @endcond */

#if defined(_SYSRTC_GRP0_PRETRIG_MASK)
/// Pre-trigger configuration structure.
typedef struct {
  bool enable;   ///< Enable/Disable the pre-trigger.
  uint8_t ticks; ///< Number of LF clock ticks the pre-trigger occurs before the counter reach the compare channel.
} sl_hal_sysrtc_pretrigger_config_t;

/// Group pre-trigger configuration structure.
typedef struct {
  sl_hal_sysrtc_pretrigger_config_t emu_wakeup; ///< Pre-trigger to wake up EMU.
  sl_hal_sysrtc_pretrigger_config_t hfxo_start; ///< Pre-trigger to start HFXO.
  uint8_t compare_channel;                      ///< Compare channel used for the pre-triggers.
} sl_hal_sysrtc_group_pretrigger_config_t;

/// Suggested default values for pre-trigger configuration structure.
#define SL_HAL_SYSRTC_PRETRIGGER_INIT_DEFAULT                          \
  {                                                                    \
    false, /* Disable the pre-trigger. */                              \
    0,     /* The pre-trigger occurs 0 LF ticks before the compare. */ \
  }

/// Suggested default values for group pre-trigger configuration structure.
#define SL_HAL_SYSRTC_GROUP_PRETRIGGER_INIT_DEFAULT                                                \
  {                                                                                                \
    SL_HAL_SYSRTC_PRETRIGGER_INIT_DEFAULT, /* Default pre-trigger configuration to wake up EMU. */ \
    SL_HAL_SYSRTC_PRETRIGGER_INIT_DEFAULT, /* Default pre-trigger configuration to start HFXO. */  \
    0                                      /* Compare channel used by the pre-triggers. */         \
  }

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
// Alias for deprecated macro name used for backward compatibility purposes.
#define SYSRTC_PRETRIGGER_CONFIG_DEFAULT SL_HAL_SYSRTC_PRETRIGGER_INIT_DEFAULT
#define SYSRTC_GROUP_PRETRIGGER_CONFIG_DEFAULT SL_HAL_SYSRTC_GROUP_PRETRIGGER_INIT_DEFAULT
/** @endcond */
#endif // defined(_SYSRTC_GRP0_PRETRIG_MASK)

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initializes SYSRTC module.
 *
 * @Note
 *   that the compare values must be set separately with
 * (sl_hal_sysrtc_set_group_compare_channel_value()), which should probably be
 * done prior to the use of this function if configuring the SYSRTC to start
 * when initialization is completed.
 *
 * @param[in] init
 *   A pointer to the SYSRTC initialization structure
 *   variable.
 ******************************************************************************/
void sl_hal_sysrtc_init(const sl_hal_sysrtc_init_t *init);

/***************************************************************************//**
 * @brief
 *   Enables the SYSRTC module and starts the counter.
 ******************************************************************************/
void sl_hal_sysrtc_enable(void);

/***************************************************************************//**
 * @brief
 *   Stops the counter and disables the SYSRTC.
 ******************************************************************************/
void sl_hal_sysrtc_disable(void);

/***************************************************************************//**
 * @brief
 *   Waits for the SYSRTC to complete all synchronization of register changes
 *   and commands.
 *
 * @brief
 *   Wait for synchronization of register changes to low frequency domain.
 *
 * @note
 *   This function will stall execution until the synchronization is complete.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
__INLINE void sl_hal_sysrtc_wait_sync(void)
{
  while ((SYSRTC0->EN & SYSRTC_EN_EN) && (SYSRTC0->SYNCBUSY != 0U)) {
    // Wait for all synchronizations to finish.
  }
}

/***************************************************************************//**
 * @brief
 *   Waits for the SYSRTC to complete resetting or disabling procedure.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_wait_ready(void)
{
  while ((SYSRTC0->SWRST & _SYSRTC_SWRST_RESETTING_MASK) || (SYSRTC0->EN & _SYSRTC_EN_DISABLING_MASK) || (SYSRTC0->SYNCBUSY != 0U)) {
    // Wait for all synchronizations to finish.
  }
}

/***************************************************************************//**
 * @brief
 *   Starts SYSRTC counter.
 *
 *   This function will send a start command to the SYSRTC peripheral. The SYSRTC
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The sl_hal_sysrtc_wait_sync() function can be used to wait for the start
 *   command to be executed.
 *
 * @note
 *   This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_start(void)
{
  // Make sure module is enabled.
  EFM_ASSERT(SYSRTC0->EN & SYSRTC_EN_EN);

  // Wait to synchronize before sending the command.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->CMD = SYSRTC_CMD_START;
}

/***************************************************************************//**
 * @brief
 *   Stops the SYSRTC counter.
 *
 *   This function will send a stop command to the SYSRTC peripheral. The SYSRTC
 *   peripheral will use some LF clock ticks before the command is executed.
 *   The sl_hal_sysrtc_wait_sync() function can be used to wait for the stop
 *   command to be executed.
 *
 * @note
 *   This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_stop(void)
{
  // Make sure module is enabled.
  EFM_ASSERT(SYSRTC0->EN & SYSRTC_EN_EN);

  // Wait to synchronize before sending the command.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->CMD = SYSRTC_CMD_STOP;
}

/***************************************************************************//**
 * @brief
 *   Restores SYSRTC to its reset state.
 ******************************************************************************/
void sl_hal_sysrtc_reset(void);

/***************************************************************************//**
 * @brief
 *   Gets SYSRTC STATUS register value.
 *
 * @return
 *   Current STATUS register value.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_status(void)
{
  return SYSRTC0->STATUS;
}

/***************************************************************************//**
 * @brief
 *   Locks SYSRTC registers.
 *
 * @note
 *   When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 *   SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written
 *   to.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_lock(void)
{
  SYSRTC0->LOCK = ~SYSRTC_LOCK_LOCKKEY_UNLOCK;
}

/***************************************************************************//**
 * @brief
 *   Unlocks SYSRTC registers.
 *
 * @note
 *   When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 *   SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written
 *   to.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_unlock(void)
{
  SYSRTC0->LOCK = SYSRTC_LOCK_LOCKKEY_UNLOCK;
}

#if defined(_SYSRTC_IF_MASK)
/***************************************************************************//**
 * @brief
 *   Enables one or more SYSRTC interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   sl_hal_sysrtc_clear_interrupts() prior to enabling the interrupt.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to enable.
 *   Use a set of interrupt flags OR-ed together to set
 *   multiple interrupt sources for SYSRTC.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_enable_interrupts(uint32_t flags)
{
  SYSRTC0->IEN_SET = flags;
}

/***************************************************************************//**
 * @brief
 *   Disables one or more SYSRTC interrupts for.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to disable.
 *   Use a set of interrupt flags OR-ed together to disable
 *   multiple interrupt sources for SYSRTC.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_disable_interrupts(uint32_t flags)
{
  SYSRTC0->IEN_CLR = flags;
}

/***************************************************************************//**
 * @brief
 *   Clears one or more pending SYSRTC interrupts.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to clear.
 *   Use a set of interrupt flags OR-ed together to clear
 *   multiple interrupt sources for SYSRTC.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_clear_interrupts(uint32_t flags)
{
  SYSRTC0->IF_CLR = flags;
}

/***************************************************************************//**
 * @brief
 *   Gets pending SYSRTC interrupt flags.
 *
 * @note
 *   Event bits are not cleared by using this function.
 *
 * @return
 *   Pending SYSRTC interrupt sources.
 *   Returns a set of interrupt flags OR-ed together for multiple
 *   interrupt sources in SYSRTC.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_interrupts(void)
{
  return SYSRTC0->IF;
}

/***************************************************************************//**
 * @brief
 *   Gets enabled and pending SYSRTC interrupt flags.
 *
 * Useful
 *   for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by using this function.
 *
 * @return
 *   Pending and enabled SYSRTC interrupt sources.
 *   The return value is the bitwise AND of
 *   - the enabled interrupt sources in SYSRTC_IEN and
 *   - the pending interrupt flags SYSRTC_IF.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_enabled_interrupts(void)
{
  uint32_t ien = SYSRTC0->IEN;
  return SYSRTC0->IF & ien;
}

/***************************************************************************//**
 * @brief
 *   Sets one or more pending SYSRTC interrupts from Software.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to set to pending.
 *   Use a set of interrupt flags OR-ed together to set
 *   multiple interrupt sources for SYSRTC.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_set_interrupts(uint32_t flags)
{
  SYSRTC0->IF_SET = flags;
}
#endif // defined(_SYSRTC_IF_MASK)

/***************************************************************************//**
 * @brief
 *   Gets SYSRTC counter value.
 *
 * @return
 *   Current SYSRTC counter value.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
__INLINE uint32_t sl_hal_sysrtc_get_counter(void)
{
  // Wait for Counter to synchronize before getting the value.
  sl_hal_sysrtc_wait_sync();

  return SYSRTC0->CNT;
}

/***************************************************************************//**
 * @brief
 *   Sets the SYSRTC counter value.
 *
 * @param[in] value
 *   The new SYSRTC counter value.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_set_counter(uint32_t value)
{
  // Wait for Counter to synchronize before setting the value.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->CNT = value;
}

#if defined(_SYSRTC_MSCNT_MASK)
/***************************************************************************//**
 * @brief
 *   Starts the SYSRTC MS counter.
 *
 * @note
 *   This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_start_ms(void)
{
  // Make sure module is enabled.
  EFM_ASSERT(SYSRTC0->EN & SYSRTC_EN_EN);

  // Wait to synchronize before sending the command.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->CMD = SYSRTC_CMD_MSSTART;
}

/***************************************************************************//**
 * @brief
 *   Stops the SYSRTC MS counter.
 *
 * @note
 *   This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_stop_ms(void)
{
  // Make sure module is enabled.
  EFM_ASSERT(SYSRTC0->EN & SYSRTC_EN_EN);

  // Wait to synchronize before sending the command.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->CMD = SYSRTC_CMD_MSSTOP;
}

/***************************************************************************//**
 * @brief
 *   Gets SYSRTC MS counter value.
 *
 * @return
 *   Current SYSRTC MS counter value.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_ms_counter(void)
{
  // Wait to synchronize before before getting the value.
  sl_hal_sysrtc_wait_sync();

  return SYSRTC0->MSCNT;
}

/***************************************************************************//**
 * @brief
 *   Sets the SYSRTC MS compare value.
 *
 * @param[in] value
 *   The new SYSRTC MS compare value.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_set_ms_compare(uint32_t value)
{
  // Wait to synchronize before before setting the value.
  sl_hal_sysrtc_wait_sync();

  SYSRTC0->MSCMPVAL = value;
}

/***************************************************************************//**
 * @brief
 *   Gets the SYSRTC MS compare value.
 *
 * @return
 *   Current SYSRTC MS compare value.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_ms_compare(void)
{
  // Wait to synchronize before before getting the value.
  sl_hal_sysrtc_wait_sync();

  return SYSRTC0->MSCMPVAL;
}

/***************************************************************************//**
 * @brief
 *   Sets the SYSRTC MS counter compare value buffer.
 *
 * @details
 *  The compare value buffer holds the value which will be written to
 *  MSCMPVAL on an update event if the buffer has been updated since
 *  the last event.
 *
 * @param[in] value
 *   The new SYSRTC MS compare value buffer.
 ******************************************************************************/
__INLINE void sl_hal_sysrtc_set_ms_compare_buffer(uint32_t value)
{
  SYSRTC0->MSCMPBUF = value;
}

/***************************************************************************//**
 * @brief
 *   Gets the SYSRTC MS compare value buffer.
 *
 * @return
 *   Current SYSRTC MS compare value buffer.
 ******************************************************************************/
__INLINE uint32_t sl_hal_sysrtc_get_ms_compare_buffer(void)
{
  return SYSRTC0->MSCMPBUF;
}
#endif // defined(_SYSRTC_MSCNT_MASK)

/***************************************************************************//**
 * @brief
 *   Waits for the SYSRTC group to complete all synchronization of register
 *   changes and commands.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
void sl_hal_sysrtc_wait_sync_group(uint8_t group_number);

/***************************************************************************//**
 * @brief
 *   Initializes the selected SYSRTC group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] group_init
 *   Pointer to group initialization structure variable.
 ******************************************************************************/
void sl_hal_sysrtc_init_group(uint8_t group_number,
                              const sl_hal_sysrtc_group_init_t *group_init);

/***************************************************************************//**
 * @brief
 *   Enables one or more SYSRTC interrupts for the given group.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. To ignore a pending interrupt, consider using
 *   sl_hal_sysrtc_clear_group_interrupts() prior to enabling the interrupt.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to enable.
 *   Use a set of interrupt flags OR-ed together to set
 *   multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
void sl_hal_sysrtc_enable_group_interrupts(uint8_t group_number,
                                           uint32_t flags);

/***************************************************************************//**
 * @brief
 *   Disables one or more SYSRTC interrupts for the given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to disable.
 *   Use a set of interrupt flags OR-ed together to disable
 *   multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
void sl_hal_sysrtc_disable_group_interrupts(uint8_t group_number,
                                            uint32_t flags);

/***************************************************************************//**
 * @brief
 *   Clears one or more pending SYSRTC interrupts for the given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to clear.
 *   Use a set of interrupt flags OR-ed together to clear
 *   multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
void sl_hal_sysrtc_clear_group_interrupts(uint8_t group_number,
                                          uint32_t flags);

/***************************************************************************//**
 * @brief
 *   Gets pending SYSRTC interrupt flags for the given group.
 *
 * @note
 *   Event bits are not cleared by using this function.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @return
 *   Pending SYSRTC interrupt sources.
 *   Returns a set of interrupt flags OR-ed together for multiple
 *   interrupt sources in the SYSRTC group.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
uint32_t sl_hal_sysrtc_get_group_interrupts(uint8_t group_number);

/***************************************************************************//**
 * @brief
 *   Gets enabled and pending SYSRTC interrupt flags for the given group.
 *   Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note
 *   Interrupt flags are not cleared by using this function.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @return
 *   Pending and enabled SYSRTC interrupt sources.
 *   The return value is the bitwise AND of
 *   - the enabled interrupt sources in SYSRTC_GRPx_IEN and
 *   - the pending interrupt flags SYSRTC_GRPx_IF.
 ******************************************************************************/
uint32_t sl_hal_sysrtc_get_group_enabled_interrupts(uint8_t group_number);

/***************************************************************************//**
 * @brief
 *   Sets one or more pending SYSRTC interrupts for the given group from Software.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] flags
 *   SYSRTC interrupt sources to set to pending. Use a set of interrupt flags
 *   OR-ed together to set multiple interrupt sources for the SYSRTC group.
 ******************************************************************************/
void sl_hal_sysrtc_set_group_interrupts(uint8_t group_number,
                                        uint32_t flags);

/***************************************************************************//**
 * @brief
 *   Gets SYSRTC compare register value for selected channel of given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] channel
 *   Channel selector.
 *
 * @return
 *   Compare register value.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
uint32_t sl_hal_sysrtc_get_group_compare_channel_value(uint8_t group_number,
                                                       uint8_t channel);

/***************************************************************************//**
 * @brief
 *   Sets SYSRTC compare register value for selected channel of given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] channel
 *   Channel selector.
 *
 * @param[in] value
 *   Compare register value.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
void sl_hal_sysrtc_set_group_compare_channel_value(uint8_t group_number,
                                                   uint8_t channel,
                                                   uint32_t value);

/***************************************************************************//**
 * @brief
 *   Gets SYSRTC input capture register value for capture channel of given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @return
 *   Capture register value.
 ******************************************************************************/
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_HAL_SYSRTC, SL_CODE_CLASS_TIME_CRITICAL)
uint32_t sl_hal_sysrtc_get_group_capture_channel_value(uint8_t group_number);

#if defined(_SYSRTC_GRP0_PRETRIG_MASK)
/***************************************************************************//**
 * @brief
 *   Sets the pre-triggers for a given group.
 *
 * @param[in] group_number
 *   SYSRTC group number to use.
 *
 * @param[in] p_group_pretrigger_config
 *   Pointer to group pre-trigger configuration structure variable.
 ******************************************************************************/
void sl_hal_sysrtc_set_group_pretrigger(uint8_t group_number,
                                        sl_hal_sysrtc_group_pretrigger_config_t const *p_group_pretrigger_config);

/***************************************************************************//**
 * @brief
 *   Gets the status of the pre-triggers for a given group.
 *
 * @param[in]  group_number
 *   SYSRTC group number to use.
 *
 * @return
 *   Pre-trigger status flags for a given group.
 ******************************************************************************/
uint32_t sl_hal_sysrtc_get_group_pretrigger_status(uint8_t group_number);

/***************************************************************************//**
 * @brief
 *   Clears the status of the pre-triggers for a given group.
 *
 * @details
 *   The ACTIVE status must be cleared from Software.
 *
 * @param[in]  group_number
 *   SYSRTC group number to use.
 *
 * @param[out] flags
 *   Pre-trigger status flags for a given group.
 ******************************************************************************/
void sl_hal_sysrtc_clear_group_pretrigger_status(uint8_t group_number,
                                                 uint32_t flags);
#endif // defined(_SYSRTC_GRP0_PRETRIG_MASK)

/** @} (end addtogroup sysrtc) */

#ifdef __cplusplus
}
#endif

/****************************************************************************//**
 * @addtogroup sysrtc SYSRTC - System RTC
 * @{
 *
 *@n @section sysrtc_example Example
 *  This example demonstrates how to configure and use the System Real-Time Counter (SYSRTC)
 *  to generate periodic interrupts.
 *
 * @code
 * #include "sl_hal_sysrtc.h"
 * #include "sl_clock_manager.h"
 * #include "sl_interrupt_manager.h"
 *
 * // SYSRTC group 0 compare 0 interrupt handler
 * void SYSRTC_APP_IRQHandler(void)
 * {
 *   uint32_t flags = sl_hal_sysrtc_get_group_interrupts(0);
 *   if (flags & SYSRTC_GRP0_IF_CMP0) {
 *     // Handle compare match 0 interrupt
 *     sl_hal_sysrtc_clear_group_interrupts(0, SYSRTC_GRP0_IF_CMP0);
 *     // Add your application code here (e.g., toggle an LED)
 *   }
 * }
 *
 * void sysrtc_example(void)
 * {
 *   // Enable SYSRTC clock
 *   sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_SYSRTC0);
 *
 *   // Initialize SYSRTC with default config
 *   sl_hal_sysrtc_init_t sysrtc_init = SL_HAL_SYSRTC_INIT_DEFAULT;
 *   sl_hal_sysrtc_init(&sysrtc_init);
 *
 *   // Initialize group 0 and enable compare channel 0 with config
 *   sl_hal_sysrtc_group_init_t group_init = SL_HAL_SYSRTC_GROUP_INIT_DEFAULT;
 *   sl_hal_sysrtc_group_channel_compare_config_t compare0_config = SL_HAL_SYSRTC_GROUP_CHANNEL_COMPARE_INIT_DEFAULT;
 *   group_init.compare_channel0_enable = true;
 *   group_init.p_compare_channel0_config = &compare0_config;
 *   sl_hal_sysrtc_init_group(0, &group_init);
 *
 *   // Set compare value for group 0, channel 0
 *   sl_hal_sysrtc_set_group_compare_channel_value(0, 0, 1000);
 *
 *   // Enable group 0 compare 0 interrupt
 *   sl_hal_sysrtc_enable_group_interrupts(0, SYSRTC_GRP0_IF_CMP0);
 *
 *   // Enable SYSRTC interrupt in NVIC
 *   sl_interrupt_manager_clear_irq_pending(SYSRTC_APP_IRQn);
 *   sl_interrupt_manager_enable_irq(SYSRTC_APP_IRQn);
 *
 *   // Start SYSRTC
 *   sl_hal_sysrtc_enable();
 *   sl_hal_sysrtc_wait_sync();
 *
 *   // Application main loop
 *   while (1) {
 *     // Read current counter value if needed
 *     uint32_t count = sl_hal_sysrtc_get_counter();
 *     // Sleep or perform other tasks
 *   }
 * }
 * @endcode
 * @} (end addtogroup sysrtc)
 *******************************************************************************/

#endif /* defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0) */
#endif /* SL_HAL_SYSRTC_H */
