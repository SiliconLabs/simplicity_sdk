/***************************************************************************//**
 * @file
 * @brief API defining access to SYSCFG registers
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_HAL_SYSCFG_H
#define SL_HAL_SYSCFG_H

#include <stdint.h>

#if defined(SL_TRUSTZONE_NONSECURE)
#include "sli_tz_service_syscfg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup syscfg SYSTEM CONFIGURATION
 * @brief System Configuration (SYSCFG) peripheral API
 * @details The SYSCFG module provides access to various system configuration
 *          registers including chip revision information, system tick clock
 *          configuration, and memory controller settings.
 *
 * @{
 ******************************************************************************/

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/*******************************************************************************
 ******************************** TZ SERVICES **********************************
 ******************************************************************************/

/*******************************************************************************
 * @brief
 *   Reads CHIPREV register.
 *
 * @return The current value of the CHIPREV register, containing chip revision
 *         information.
 ******************************************************************************/
uint32_t sl_hal_syscfg_read_chip_rev(void);

/*******************************************************************************
 * @brief
 *   Enables the external clock for the SysTick timer.
 ******************************************************************************/
void sl_hal_syscfg_set_systicextclken_cfgsystic(void);

/*******************************************************************************
 * @brief
 *   Disables the external clock source for the SysTick timer.
 ******************************************************************************/
void sl_hal_syscfg_clear_systicextclken_cfgsystic(void);

#if defined(_SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK)
/*******************************************************************************
 * @brief
 *   Enables the RAM wait state feature for DMEM0.
 *
 * @details
 *   This function sets the RAMWSEN bit in the DMEM0RAMCTRL register to `1`,
 *   enabling the RAM wait state feature for DMEM0. This feature is used to
 *   configure the memory access timing for DMEM0.
 ******************************************************************************/
__INLINE void sl_hal_syscfg_set_dmem0ramctrl_ramwsen_bit(void)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  sli_tz_syscfg_set_dmem0ramctrl_ramwsen_bit();
#else

  SYSCFG->DMEM0RAMCTRL = SYSCFG->DMEM0RAMCTRL | _SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK;
#endif
}

/*******************************************************************************
 * @brief
 *   Disables the RAM wait state feature for DMEM0.
 *
 * @details
 *   This function clears the RAMWSEN bit in the DMEM0RAMCTRL register to `0`,
 *   disabling the RAM wait state feature for DMEM0. This ensures that memory
 *   access timing for DMEM0 operates without additional wait states.
 ******************************************************************************/
__INLINE void sl_hal_syscfg_clear_dmem0ramctrl_ramwsen_bit(void)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  sli_tz_syscfg_clear_dmem0ramctrl_ramwsen_bit();
#else
  SYSCFG->DMEM0RAMCTRL = SYSCFG->DMEM0RAMCTRL & ~_SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK;
#endif
}

/*******************************************************************************
 * @brief
 *    Reads the RAMWSEN bit from the DMEM0RAMCTRL register.
 *
 * @details
 *   This function retrieves the value of the RAMWSEN bit in the DMEM0RAMCTRL
 *   register. The RAMWSEN bit indicates whether the RAM wait state feature
 *   for DMEM0 is enabled (`1`) or disabled (`0`).
 *
 * @return
 *    The value of the RAMWSEN bit (1 if enabled, 0 if disabled).
 ******************************************************************************/
__INLINE uint32_t sl_hal_syscfg_get_dmem0ramctrl_ramwsen_bit(void)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  return sli_tz_syscfg_get_dmem0ramctrl_ramwsen_bit();
#else
  return (SYSCFG->DMEM0RAMCTRL & _SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK) >> _SYSCFG_DMEM0RAMCTRL_RAMWSEN_SHIFT;
#endif
}

#endif /* _SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK */

#if defined(_SYSCFG_DMEM0RETNCTRL_MASK)
/*******************************************************************************
 * @brief
 *    Reads the DMEM0RETNCTRL register.
 *
 * @details
 *    This function retrieves the current value of the DMEM0RETNCTRL
 *    register, which is used to control the retention settings for
 *    DMEM0 memory blocks.
 *
 * @return The current value of the DMEM0RETNCTRL register.
 ******************************************************************************/
__INLINE uint32_t sl_hal_syscfg_read_dmem0retnctrl(void)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  return sli_tz_syscfg_read_dmem0retnctrl_register();
#else
  return SYSCFG->DMEM0RETNCTRL;
#endif
}

/*******************************************************************************
 * @brief
 *   Applies a mask to the DMEM0RETNCTRL register.
 *
 * @details
 *   This function performs a bitwise OR operation between the current
 *   value of the DMEM0RETNCTRL register and the provided mask,
 *   setting specific bits in the register.
 *
 * @param[in] mask
 *   The bit mask to apply to the DMEM0RETNCTRL register.
 ******************************************************************************/
__INLINE void sl_hal_syscfg_mask_dmem0retnctrl(uint32_t mask)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  sli_tz_syscfg_mask_dmem0retnctrl_register(mask);
#else
  SYSCFG->DMEM0RETNCTRL = SYSCFG->DMEM0RETNCTRL | mask;
#endif
}

/*******************************************************************************
 * @brief
 *   Sets the DMEM0RETNCTRL register to zero.
 *
 * @details
 *    This function clears all bits in the DMEM0RETNCTRL register by
 *    setting its value to 0. This effectively disables all retention
 *    settings for DMEM0 memory blocks.
 ******************************************************************************/
__INLINE void sl_hal_syscfg_zero_dmem0retnctrl(void)
{
#if defined(SL_TRUSTZONE_NONSECURE)
  sli_tz_syscfg_zero_dmem0retnctrl_register();
#else
  SYSCFG->DMEM0RETNCTRL = 0x0UL;
#endif
}
#endif /* _SYSCFG_DMEM0RETNCTRL_MASK */

/** @} (end addtogroup syscfg) */

#ifdef __cplusplus
}
#endif
#endif // SL_HAL_SYSCFG_H
