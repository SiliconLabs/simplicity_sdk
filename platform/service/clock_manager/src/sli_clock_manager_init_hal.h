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
#define CLOCK_MANAGER_GET_DEFAULT_CLOCK_SOURCE(clock_branch, clock_source)   CLOCK_MANAGER_GET_DEFAULT_CLOCK_SOURCE_NX(clock_branch, clock_source)

/*******************************************************************************
 ******************************  PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initializes Oscillators and Clock branches.
 ******************************************************************************/
sl_status_t sli_clock_manager_hal_init(void);

#ifdef __cplusplus
}
#endif

#endif // SL_CLOCK_MANAGER_INIT_HAL_H
