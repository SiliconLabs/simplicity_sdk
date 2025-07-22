/***************************************************************************//**
 * @file
 * @brief Clock Manager Init HAL APIs.
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

#ifndef SL_CLOCK_MANAGER_INIT_HAL_H
#define SL_CLOCK_MANAGER_INIT_HAL_H

#include "sl_status.h"
#include "sl_clock_manager_tree_config.h"
#include "sl_clock_manager_oscillator_config.h"
#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ********************************  MACROS   ************************************
 ******************************************************************************/

#if (SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFRCODPLL)
#define SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_CONCATENATION      _HFRCODPLL
#elif (SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_HFXO)
#define SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_CONCATENATION      _HFXO
#elif (SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_FSRCO)
#define SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE_CONCATENATION      _FSRCO
#else
#error "SL_CLOCK_MANAGER_DEFAULT_HF_CLOCK_SOURCE configuration value is invalid."
#endif

#if (SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFRCO)
#define SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_CONCATENATION       _LFRCO
#elif (SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_LFXO)
#define SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_CONCATENATION       _LFXO
#elif (SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE == SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_ULFRCO)
#define SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE_CONCATENATION      _ULFRCO
#else
#error "SL_CLOCK_MANAGER_DEFAULT_LF_CLOCK_SOURCE configuration value is invalid."
#endif

#define CLOCK_MANAGER_CLOCK_SELECT_SET(clock_branch, clock_source)                               \
  do {                                                                                           \
    CMU->clock_branch##CTRL = (CMU->clock_branch##CTRL & ~_CMU_##clock_branch##CTRL_CLKSEL_MASK) \
                              | clock_source;                                                    \
  } while (0)

#define CLOCK_MANAGER_GET_DEFAULT_CLOCK_SOURCE_NX(clock_branch, clock_source) CMU_##clock_branch##CTRL_CLKSEL##clock_source
#define CLOCK_MANAGER_GET_DEFAULT_CLOCK_SOURCE(clock_branch, clock_source)    CLOCK_MANAGER_GET_DEFAULT_CLOCK_SOURCE_NX(clock_branch, clock_source)

#if !defined(SLI_CLOCK_MANAGER_RUNTIME_CONFIGURATION)
#define SLI_CLOCK_MANAGER_HFXO_MODE SL_CLOCK_MANAGER_HFXO_MODE
#define SLI_CLOCK_MANAGER_HFXO_FREQ SL_CLOCK_MANAGER_HFXO_FREQ

#define SLI_CLOCK_MANAGER_HFRCO_BAND      SL_CLOCK_MANAGER_HFRCO_BAND
#define SLI_CLOCK_MANAGER_HFRCO_DPLL_EN   SL_CLOCK_MANAGER_HFRCO_DPLL_EN
#define SLI_CLOCK_MANAGER_DPLL_FREQ       SL_CLOCK_MANAGER_DPLL_FREQ
#define SLI_CLOCK_MANAGER_DPLL_N          SL_CLOCK_MANAGER_DPLL_N
#define SLI_CLOCK_MANAGER_DPLL_M          SL_CLOCK_MANAGER_DPLL_M

#if defined(SL_CLOCK_MANAGER_SOCPLL_EN) && (SL_CLOCK_MANAGER_SOCPLL_EN == 1)
#if defined(SL_CLOCK_MANAGER_SOCPLL_ADVANCED_SETTINGS) && (SL_CLOCK_MANAGER_SOCPLL_ADVANCED_SETTINGS == 0)
#if defined(SL_CATALOG_RAIL_LIB_PRESENT) || (defined(SL_CLOCK_MANAGER_HFXO_EN) && (SL_CLOCK_MANAGER_HFXO_EN == 1))
#define SL_CLOCK_MANAGER_SOCPLL_REFCLK        SOCPLL_CTRL_REFCLKSEL_REF_HFXO
#else
#define SL_CLOCK_MANAGER_SOCPLL_REFCLK        SOCPLL_CTRL_REFCLKSEL_REF_HFRCO
#endif
#endif // SL_CLOCK_MANAGER_SOCPLL_ADVANCED_SETTINGS
#endif // SL_CLOCK_MANAGER_SOCPLL_EN
#endif // !defined(SLI_CLOCK_MANAGER_RUNTIME_CONFIGURATION)

#if defined(SL_CLOCK_MANAGER_SOCPLL_EN) && (SL_CLOCK_MANAGER_SOCPLL_EN == 1)
#if (SL_CLOCK_MANAGER_SOCPLL_REFCLK == SOCPLL_CTRL_REFCLKSEL_REF_HFXO)
#define SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ   SL_CLOCK_MANAGER_HFXO_FREQ
#elif (SL_CLOCK_MANAGER_SOCPLL_REFCLK == SOCPLL_CTRL_REFCLKSEL_REF_HFRCO)
#define SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ   SL_CLOCK_MANAGER_HFRCO_BAND
#elif (SL_CLOCK_MANAGER_SOCPLL_REFCLK == SOCPLL_CTRL_REFCLKSEL_REF_EXTCLK)
#error External Clock is not supported as SOCPLL reference clock
#endif

#if (SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ < 34000000) || (SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ > 44000000)
#error SOCPLL reference clock frequency must be between 34MHz and 44MHz
#endif

#ifndef SL_CLOCK_MANAGER_SOCPLL_FRACTIONAL_EN
#define SL_CLOCK_MANAGER_SOCPLL_FRACTIONAL_EN 1
#endif

// SOCPLL Formula: SOCPLL_FREQ = REFCLK_FREQ * (DIVN+2 + DIVF/1024) / 6
// SL_CLOCK_MANAGER_SOCPLL_DIVN is rounded down and SL_CLOCK_MANAGER_SOCPLL_DIVF is rounded to the closest integer.
#ifndef SL_CLOCK_MANAGER_SOCPLL_DIVN
#define SL_CLOCK_MANAGER_SOCPLL_DIVN          (6ULL * SL_CLOCK_MANAGER_SOCPLL_FREQ / SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ - 2ULL)
#endif
#ifndef SL_CLOCK_MANAGER_SOCPLL_DIVF
#define SL_CLOCK_MANAGER_SOCPLL_DIVF          ((6ULL * 1024ULL * SL_CLOCK_MANAGER_SOCPLL_FREQ + SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ / 2ULL) / SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ - 1024ULL * (SL_CLOCK_MANAGER_SOCPLL_DIVN + 2ULL))
#endif
#endif // SL_CLOCK_MANAGER_SOCPLL_EN

#if !defined(SLI_CLOCK_MANAGER_RUNTIME_CONFIGURATION)
#if defined(SL_CLOCK_MANAGER_SOCPLL_EN) && (SL_CLOCK_MANAGER_SOCPLL_EN == 1)
#define SLI_CLOCK_MANAGER_SOCPLL_EN            SL_CLOCK_MANAGER_SOCPLL_EN
#define SLI_CLOCK_MANAGER_SOCPLL_FREQ          SL_CLOCK_MANAGER_SOCPLL_FREQ
#define SLI_CLOCK_MANAGER_SOCPLL_REFCLK        SL_CLOCK_MANAGER_SOCPLL_REFCLK
#define SLI_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ   SL_CLOCK_MANAGER_SOCPLL_REFCLK_FREQ
#define SLI_CLOCK_MANAGER_SOCPLL_FRACTIONAL_EN SL_CLOCK_MANAGER_SOCPLL_FRACTIONAL_EN
#define SLI_CLOCK_MANAGER_SOCPLL_DIVN          SL_CLOCK_MANAGER_SOCPLL_DIVN
#define SLI_CLOCK_MANAGER_SOCPLL_DIVF          SL_CLOCK_MANAGER_SOCPLL_DIVF
#endif  // SL_CLOCK_MANAGER_SOCPLL_EN
#endif // !defined(SLI_CLOCK_MANAGER_RUNTIME_CONFIGURATION)

/*******************************************************************************
 ******************************  PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initializes Oscillators and Clock branches.
 ******************************************************************************/
sl_status_t sli_clock_manager_hal_init(void);

#if defined(CLOCK_MANAGER_INIT_HAL_INTERNAL_PRESENT)
/***************************************************************************//**
 * Initializes internal oscillators.
 ******************************************************************************/
void sli_clock_manager_hal_init_oscillators_internal(void);

/***************************************************************************//**
 * Initializes internal Clock branches.
 ******************************************************************************/
void sli_clock_manager_hal_init_clock_branches_internal(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // SL_CLOCK_MANAGER_INIT_HAL_H
