/***************************************************************************//**
 * @file
 * @brief ICACHE Disable
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

#include "sli_icache_disable.h"
#include "em_device.h"

/* Only supports ARM Cortex-M33 with MPU for now. */
#if (!defined(__MPU_PRESENT) || (__MPU_PRESENT != 1U))
  #error "The device does not have an MPU, and therefore the \
  code_classification_validator is not supported."
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 **************************  GLOBAL VARIABLES   ********************************
 ******************************************************************************/
extern const uint32_t linker_code_begin;
extern const uint32_t linker_code_end;
extern const uint32_t __vma_ramfuncs_start__;
extern const uint32_t __vma_ramfuncs_end__;

/*******************************************************************************
 *****************************   DEFINES   *************************************
 ******************************************************************************/
#define FLASH_CODE_BASE               (uint32_t)&linker_code_begin
#define FLASH_CODE_LIMIT              (uint32_t)&linker_code_end

#define RAM_CODE_BASE                 (uint32_t)&__vma_ramfuncs_start__
#define RAM_CODE_LIMIT                (uint32_t)&__vma_ramfuncs_end__

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 *  Clear all MPU entries.
 ******************************************************************************/
static void mpu_entry_table_clear(void);

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 *  Clear all MPU entries.
 ******************************************************************************/
static void mpu_entry_table_clear(void)
{
  // ARM Cortex-M33
  const uint8_t num_mpu_regions = (MPU->TYPE & MPU_TYPE_DREGION_Msk)
                                  >> MPU_TYPE_DREGION_Pos;

  for ( uint8_t i = 0; i < num_mpu_regions; i++) {
    MPU->RNR = i;
    MPU->RBAR = 0U;
    MPU->RLAR = 0U;
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 *  Disable the ICACHE by creating MPU entries for FLASH and RAM code with
 *  non-cacheable attributes. This will overwrite any previous MPU
 *  configuration.
 ******************************************************************************/
void sli_icache_disable(void)
{
  ARM_MPU_Disable();
  {
    mpu_entry_table_clear();

    // Set MAIR0 memory attributes for non-transient, non-cacheable data.
    ARM_MPU_SetMemAttr(0,                                             // MAIR0
                       ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 0, 0, 0), // Outer
                                    ARM_MPU_ATTR_MEMORY_(1, 0, 0, 0)) // Inner
                       );

    // RAM Code Execute with MAIR0 memory attributes.
    MPU->RNR = 0U;
    MPU->RBAR = ARM_MPU_RBAR(RAM_CODE_BASE,
                             ARM_MPU_SH_NON,
                             0,  // Read-Write
                             1,  // Non-privileged
                             0); // Executable
    MPU->RLAR = ARM_MPU_RLAR(RAM_CODE_LIMIT,
                             0); // Attribute Index 0

    // FLASH Code Execute with MAIR0 memory attributes
    MPU->RNR = 1U;
    MPU->RBAR = ARM_MPU_RBAR(FLASH_CODE_BASE,
                             ARM_MPU_SH_NON,
                             0,  // Read-Write
                             1,  // Non-privileged
                             0); // Executable
    MPU->RLAR = ARM_MPU_RLAR(FLASH_CODE_LIMIT,
                             0); // Attribute Index 0
  }
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

#ifdef __cplusplus
}
#endif
