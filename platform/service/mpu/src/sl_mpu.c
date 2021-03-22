/***************************************************************************//**
 * @file
 * @brief MPU API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_mpu.h"

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "em_device.h"
#include "sl_core.h"

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

#define MPU_MEMORY_ATTRIBUTE_IX_0   0U
#define MPU_MEMORY_ATTRIBUTE_IX_1   1U

#define MPU_RBAR_BASE_ADDR_NONE     0U
#define MPU_RBAR_AP_READ_WRITE      0U
#define MPU_RBAR_AP_READ_ONLY       1U
#define MPU_RBAR_AP_PRIVILEGED      0U
#define MPU_RBAR_AP_NON_PRIVILEGED  1U
#define MPU_RBAR_XN_EXECUTION       0U
#define MPU_RBAR_XN_NON_EXECUTION   1U

// Memory region attributes: non-shareable, read-write, non-privileged, non-executable.
#define MPU_RBAR_VALUE    ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,    \
                                       ARM_MPU_SH_NON,             \
                                       MPU_RBAR_AP_READ_WRITE,     \
                                       MPU_RBAR_AP_NON_PRIVILEGED, \
                                       MPU_RBAR_XN_NON_EXECUTION)

// The MPU Region Limit Address Register defines the ending address of an MPU region.
// Bit [4:0] of the address value is assigned with 0x1F to provide the limit
// address to be checked against.
#define MPU_RLAR_LIMIT_ADDRESS_ALIGNMENT   32U

// ARM memory map SRAM location and size.
#if defined(_SILICON_LABS_32B_SERIES_2) \
  || (defined(_SILICON_LABS_32B_SERIES_3_CONFIG_1) && defined(SL_RAM_LINKER))
#define MPU_ARM_SRAM_MEM_BASE          SRAM_BASE

// This RAM size is not the real device size. It corresponds to the SRAM max size in the standard
// ARM Cortex-M33 memory map. The max size is 0.5GB.
#define MPU_ARM_SRAM_MEM_SIZE          SRAM_SIZE

#elif defined(_SILICON_LABS_32B_SERIES_3_CONFIG_1)
// These local constants ensure the MPU regions for the secure/non-secure RAM aliases
// and the secure/non-secure RAM non-aliases are properly managed for all
// secure/non-secure project configurations.
#define MPU_RAMFUNC_SRAM_ALIAS_START      SRAM_ALIAS_BASE
#define MPU_RAMFUNC_SRAM_ALIAS_END        (SRAM_ALIAS_BASE + (SRAM_SIZE - 1U))

#define MPU_DATA_SRAM_START               SRAM_BASE
#define MPU_DATA_SRAM_END                 (SRAM_BASE + (SRAM_SIZE - 1U))

#if defined(SL_TRUSTZONE_SECURE)
#define MPU_NOT_USED_SRAM_ALIAS_START     DMEM_INSTR_NS_MEM_BASE
#define MPU_NOT_USED_SRAM_ALIAS_END       DMEM_INSTR_NS_MEM_END

#define MPU_NOT_USED_SRAM_START           DMEM_NS_MEM_BASE
#define MPU_NOT_USED_SRAM_END             DMEM_NS_MEM_END
#else
#define MPU_NOT_USED_SRAM_ALIAS_START     DMEM_INSTR_S_MEM_BASE
#define MPU_NOT_USED_SRAM_ALIAS_END       DMEM_INSTR_S_MEM_END

#define MPU_NOT_USED_SRAM_START           DMEM_S_MEM_BASE
#define MPU_NOT_USED_SRAM_END             DMEM_S_MEM_END
#endif
#endif

#if defined(__ICCARM__)
// iccarm
#pragma section = ".textrw"
#define RAMFUNC_SECTION_BEGIN       ((uint32_t)(uint32_t *)__section_begin(".textrw"))
#define RAMFUNC_SECTION_END         ((uint32_t)(uint32_t *)__section_end(".textrw"))
#define RAMFUNC_SECTION_SIZE        __section_size(".textrw")

#elif defined(__GNUC__)
// armgcc
extern uint32_t __vma_ramfuncs_start__;
extern uint32_t __vma_ramfuncs_end__;
#define RAMFUNC_SECTION_BEGIN       (uint32_t) &__vma_ramfuncs_start__
#define RAMFUNC_SECTION_END         (uint32_t) &__vma_ramfuncs_end__
#define RAMFUNC_SECTION_SIZE        (RAMFUNC_SECTION_END - RAMFUNC_SECTION_BEGIN)

#elif defined(__CC_ARM)
// armcc
// The section name in the armcc scatter file must be "ram_code".
extern uint32_t ram_code$$Base;
extern uint32_t ram_code$$Limit;
#define RAMFUNC_SECTION_BEGIN       (uint32_t) &ram_code$$Base
#define RAMFUNC_SECTION_END         (uint32_t) &ram_code$$Limit
#define RAMFUNC_SECTION_SIZE        (RAMFUNC_SECTION_END - RAMFUNC_SECTION_BEGIN)

#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static uint32_t region_nbr = 0;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
 * Configures internal SRAM as non-executable and enable MPU.
 *
 * @note  (1) On series 2 devices, the MPU regions configuration for the RAM layout
 *        is for GCC and IAR:
 *
 *                                         MPU Region  Region Attributes              Adresses Range
 *                                         ------------------------------------------------------------------------
 * 0x2000_0000 0x2007_FFFF (DATA, RAMFUNC) 0           non shareable, executable      RAMFunc start to RAMFunc end
 *                                         1           non shareable, non executable  RAMFunc end to range end
 * or
 * 0x2000_0000 0x2007_FFFF (DATA)          0           shareable,     non executable  Entire range
 *
 * @note  (2) On series 3 SIXG301 devices, there are the following RAM-related
 *        address ranges.
 *        - 0x0080_0000 0x0087_FFFF (Alias to DMEM (execute only)) - Secure
 *        - 0x1080_0000 0x1087_FFFF (Alias to DMEM_NS) - Non-secure
 *        - 0x2000_0000 0x2007_FFFF (DMEM) - Secure
 *        - 0x3000_00000x3007_FFFF (DMEM_NS) - Non-secure
 *
 *        The MPU regions are configured differently depending on the RAM layout
 *        described in GCC and IAR linker scripts. A common characteristic
 *        to all the MPU regions for GCC and IAR is:
 *        - All MPU regions for RAM are non-cacheable.
 *
 *        The table below presents the MPU regions for series 3 SIXG301 devices.
 *        The table assumes there are RAMFunc in RAM. If there are no RAMFunc
 *        functions, then the associated region will be a single region covering
 *        the full addresses range.
 *
 * GCC
 * ---
 *                                          MPU Region* Region Attributes              Adresses Range
 *                                          ------------------------------------------------------------------------
 * 0x0080_0000 0x0087_FFFF (RAMFUNC)*       0           shareable,     executable      RAMFunc start to RAMFunc end
 *                                          1           non shareable, non executable  RAMFunc end to range end
 * or
 * 0x0080_0000 0x0087_FFFF (NOT USED)*      x           non shareable, non executable  Entire range (if no RAMFunc)
 *
 * 0x1080_0000 0x1087_FFFF (NOT USED)*      2           non shareable, non executable  Entire range
 * 0x2000_0000 0x2007_FFFF (DATA)*          3           shareable,     non executable  Entire range
 * 0x3000_0000 0x3007_FFFF (NOT USED)*      4           non shareable, non executable  Entire range
 *
 * IAR
 * ---
 *                                          MPU Region* Region Attributes              Adresses Range
 *                                          ------------------------------------------------------------------------
 * 0x0080_0000 0x0087_FFFF (NOT USED)*      0           non shareable, non executable  Entire range
 * 0x1080_0000 0x1087_FFFF (NOT USED)*      1           non shareable, non executable  Entire range
 * 0x2000_0000 0x2007_FFFF (DATA, RAMFUNC)* 2           shareable,     non executable  Range start to RAMFunc start
 *                                          3           shareable,         executable  RAMFunc start to RAMFunc end
 *                                          4           shareable,     non executable  RAMFunc end to range end
 * or
 * 0x2000_0000 0x2007_FFFF (NOT USED)*      x           non shareable, non executable  Entire range (if no RAMFunc)
 *
 * 0x3000_0000 0x3007_FFFF (NOT USED)*      5           non shareable, non executable  Entire range

 *        *If the ARM Cortex-M33 works in non-secure, all non-secure alias/
 *        non-alias will be used by default for RAMfunc and data in RAM.
 *        Same logic if the Cortex-M33 is in secure, all secure alias/non-alias
 *        will be used by default. The MPU regions creation will adapt to
 *        the default secure or non-secure addresses.And thus the order
 *        in which the MPU regions are created can vary.
 *****************************************************************************/
void sl_mpu_disable_execute_from_ram(void)
{
  uint32_t mpu_region_begin = 0u;
  uint32_t mpu_region_end = 0u;
  uint32_t rbar;

  ARM_MPU_Disable();

  // See Note #1.
#if defined(_SILICON_LABS_32B_SERIES_2) \
  || (defined(_SILICON_LABS_32B_SERIES_3_CONFIG_1) && defined(SL_RAM_LINKER))
  // Memory attributes:
  // Outer memory with ARM_MPU_ATTR_MEMORY_(): non-transient data, Write-Through, cache allocation on read miss, no cache allocation on write miss.
  // ARM_MPU_ATTR(): outer attributes filled, no inner memory
  ARM_MPU_SetMemAttr(MPU_MEMORY_ATTRIBUTE_IX_0,
                     ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 0, 1, 0), 0));

  // Region end address LSB are always considered 0x1F.
  mpu_region_begin = MPU_ARM_SRAM_MEM_BASE;
  mpu_region_end = (RAMFUNC_SECTION_SIZE > 0) ? (RAMFUNC_SECTION_BEGIN & MPU_RBAR_BASE_Msk) - MPU_RLAR_LIMIT_ADDRESS_ALIGNMENT
                   : (MPU_ARM_SRAM_MEM_BASE + MPU_ARM_SRAM_MEM_SIZE);

  // Define initial MPU region: either 1 unique region = entire RAM. Or 1 region = from RAM START to RAMFUNC START.
  if (mpu_region_begin <= mpu_region_end) {
    // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
    rbar = MPU_RBAR_VALUE | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;
  }

  // Only if functions placed in RAM, define another region from RAMFUNC END to RAM END.
  if (RAMFUNC_SECTION_SIZE > 0u) {
    // Region end address LSB are always considered 0x1F.
    mpu_region_begin = (RAMFUNC_SECTION_END + 31u) & MPU_RLAR_LIMIT_Msk;
    mpu_region_end = MPU_ARM_SRAM_MEM_BASE + MPU_ARM_SRAM_MEM_SIZE - MPU_RLAR_LIMIT_ADDRESS_ALIGNMENT;

    // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
    rbar = MPU_RBAR_VALUE | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, 0u));
    region_nbr++;
  }

  // See Note #2.
#elif (defined(_SILICON_LABS_32B_SERIES_3_CONFIG_1) && !defined(SL_RAM_LINKER))
  // 1. Set the memory attributes for the entire RAM (alias and non-alias ranges).
  // Outer & inner memories: non-cacheable.
  ARM_MPU_SetMemAttr(MPU_MEMORY_ATTRIBUTE_IX_0,
                     ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));

  // 2. Create different MPU regions accounting for all RAM addresses ranges (secure/non-secure alias/non-alias)
  // Secure or non secure alias (not used): 1 region (non shareable, non cacheable, non executable)
  mpu_region_begin = MPU_NOT_USED_SRAM_ALIAS_START;
  rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                      ARM_MPU_SH_NON,
                      MPU_RBAR_AP_READ_ONLY,
                      MPU_RBAR_AP_NON_PRIVILEGED,
                      MPU_RBAR_XN_NON_EXECUTION)
         | (mpu_region_begin & MPU_RBAR_BASE_Msk);
  mpu_region_end = (MPU_NOT_USED_SRAM_ALIAS_END & MPU_RLAR_LIMIT_Msk);
  ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
  region_nbr++;

  // Non secure or secure alias (used for code if functions placed in RAM):
#if defined(__GNUC__)
  if (RAMFUNC_SECTION_SIZE > 0u) {
    // Functions placed in RAM: 1 region (shareable, non cacheable, executable)
    mpu_region_begin = RAMFUNC_SECTION_BEGIN;
    rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                        ARM_MPU_SH_OUTER,
                        MPU_RBAR_AP_READ_ONLY,
                        MPU_RBAR_AP_NON_PRIVILEGED,
                        MPU_RBAR_XN_EXECUTION)
           | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    mpu_region_end = (RAMFUNC_SECTION_END & MPU_RLAR_LIMIT_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;

    // Rest of the non secure or secure alias: non shareable, non cacheable, non executable
    mpu_region_begin = (RAMFUNC_SECTION_END + 31u) & MPU_RLAR_LIMIT_Msk; // +31u allows to round up to the next 32 bytes alignment.
    rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                        ARM_MPU_SH_NON,
                        MPU_RBAR_AP_READ_ONLY,
                        MPU_RBAR_AP_NON_PRIVILEGED,
                        MPU_RBAR_XN_NON_EXECUTION)
           | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    mpu_region_end = (MPU_RAMFUNC_SRAM_ALIAS_END & MPU_RLAR_LIMIT_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;
  } else {
#endif
  // No functions placed in RAM: 1 region (non shareable, non cacheable, non executable)
  mpu_region_begin = MPU_RAMFUNC_SRAM_ALIAS_START;
  rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                      ARM_MPU_SH_NON,
                      MPU_RBAR_AP_READ_ONLY,
                      MPU_RBAR_AP_NON_PRIVILEGED,
                      MPU_RBAR_XN_NON_EXECUTION)
         | (mpu_region_begin & MPU_RBAR_BASE_Msk);
  mpu_region_end = (MPU_RAMFUNC_SRAM_ALIAS_END & MPU_RLAR_LIMIT_Msk);
  ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
  region_nbr++;
#if defined(__GNUC__)
}
#endif

  // Secure or non secure non alias (used for data): 1 region (shareable, non cacheable, non executable)
#if defined(__ICCARM__)
  if (RAMFUNC_SECTION_SIZE > 0u) {
    // Beginning of non secure alias: shareable, non cacheable, non executable
    mpu_region_begin = MPU_DATA_SRAM_START;
    rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                        ARM_MPU_SH_OUTER,
                        MPU_RBAR_AP_READ_WRITE,
                        MPU_RBAR_AP_NON_PRIVILEGED,
                        MPU_RBAR_XN_NON_EXECUTION)
           | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    mpu_region_end = ((RAMFUNC_SECTION_BEGIN - 1u) & MPU_RLAR_LIMIT_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;

    // Functions placed in RAM: 1 region (shareable, non cacheable, executable)
    mpu_region_begin = RAMFUNC_SECTION_BEGIN;
    rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                        ARM_MPU_SH_OUTER,
                        MPU_RBAR_AP_READ_ONLY,
                        MPU_RBAR_AP_NON_PRIVILEGED,
                        MPU_RBAR_XN_EXECUTION)
           | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    mpu_region_end = (RAMFUNC_SECTION_END & MPU_RLAR_LIMIT_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;

    // Rest of non secure alias: shareable, non cacheable, non executable
    mpu_region_begin = (RAMFUNC_SECTION_END + 31u) & MPU_RLAR_LIMIT_Msk; // +31u allows to round up to the next 32 bytes alignment.
    rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                        ARM_MPU_SH_OUTER,
                        MPU_RBAR_AP_READ_WRITE,
                        MPU_RBAR_AP_NON_PRIVILEGED,
                        MPU_RBAR_XN_NON_EXECUTION)
           | (mpu_region_begin & MPU_RBAR_BASE_Msk);
    mpu_region_end = (MPU_DATA_SRAM_END & MPU_RLAR_LIMIT_Msk);
    ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
    region_nbr++;
  } else {
#endif
  mpu_region_begin = MPU_DATA_SRAM_START;
  rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                      ARM_MPU_SH_OUTER,
                      MPU_RBAR_AP_READ_WRITE,
                      MPU_RBAR_AP_NON_PRIVILEGED,
                      MPU_RBAR_XN_NON_EXECUTION)
         | (mpu_region_begin & MPU_RBAR_BASE_Msk);
  mpu_region_end = (MPU_DATA_SRAM_END & MPU_RLAR_LIMIT_Msk);
  ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
  region_nbr++;
#if defined(__ICCARM__)
}
#endif

  // Non secure or secure non alias (not used): 1 region (non shareable, non cacheable, non executable)
  mpu_region_begin = MPU_NOT_USED_SRAM_START;
  rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                      ARM_MPU_SH_NON,
                      MPU_RBAR_AP_READ_ONLY,
                      MPU_RBAR_AP_NON_PRIVILEGED,
                      MPU_RBAR_XN_NON_EXECUTION)
         | (mpu_region_begin & MPU_RBAR_BASE_Msk);
  mpu_region_end = (MPU_NOT_USED_SRAM_END & MPU_RLAR_LIMIT_Msk);
  ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_0));
  region_nbr++;
#endif

  // Enable MPU with default background region.
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  __DSB();
  __ISB();
}

/**************************************************************************//**
 * Enables simplified MPU driver. Configures memory address as non-executable.
 *****************************************************************************/
sl_status_t sl_mpu_disable_execute(uint32_t address_begin,
                                   uint32_t address_end,
                                   uint32_t size)
{
  uint32_t mpu_region_begin = 0u;
  uint32_t mpu_region_end = 0u;
  sl_status_t status = SL_STATUS_OK;

  // Ensure there is still an available MPU region to configure.
  if (region_nbr > MPU_RNR_REGION_Msk) {
    status = SL_STATUS_NO_MORE_RESOURCE;
  }

  ARM_MPU_Disable();

  uint32_t rbar;
  uint8_t is_overlapping = 0u;
  uint32_t prev_base_address = 0u;
  uint32_t prev_limit_address = 0u;

  // Size of memory region must be 32 bytes or more.
  if (size >= 32u) {
    // Round inside the memory region, if address is not align on 32 bytes.
    mpu_region_begin = ((address_begin % 32u) == 0u) ? address_begin
                       : (address_begin + (32u - (address_begin % 32u)));

    // Round inside the memory region, if address is not align on 32 bytes.
    mpu_region_end = ((address_end % 32u) == 0u) ? address_end
                     : (address_end  - (address_end % 32u));

    // The scanning to check the overlapping region
    for (uint8_t index_region = 0; index_region < region_nbr; index_region++) {
      // Set to the previous region number
      MPU->RNR = index_region;

      // Read the base address that was configured by the region number register before
      prev_base_address = (MPU->RBAR & MPU_RBAR_BASE_Msk);
      // Read the limit address that was configured by the region number register before
      prev_limit_address = (MPU->RLAR & MPU_RLAR_LIMIT_Msk);

      // Check the overlapping region
      if ((mpu_region_begin == prev_base_address) && (mpu_region_end == prev_limit_address)) {
        // The new region is the same as the previous region
        is_overlapping = 1;
        status = SL_STATUS_OK;
      } else if (!((mpu_region_begin > prev_limit_address) || (mpu_region_end < prev_base_address))) {
        // The new region is invalid
        is_overlapping = 1;
        status = SL_STATUS_INVALID_RANGE;
      }

      if (is_overlapping == 1) {
        break;
      }

      MPU->RNR &= ~MPU_RNR_REGION_Msk;
    }

    // Set specified memory region if no overlap has been detected.
    if (!is_overlapping) {
      // Device memory type non Gathering, non Re-ordering, Early Write Acknowledgment
      ARM_MPU_SetMemAttr(MPU_MEMORY_ATTRIBUTE_IX_1, ARM_MPU_ATTR_DEVICE_nGnRE);

      // A bug exists in some versions of ARM_MPU_RBAR(). Set base addr manually.
      // Memory region attributes: non-shareable, read-write, non-privileged, non-executable
      rbar = ARM_MPU_RBAR(MPU_RBAR_BASE_ADDR_NONE,
                          ARM_MPU_SH_NON,
                          MPU_RBAR_AP_READ_WRITE,
                          MPU_RBAR_AP_NON_PRIVILEGED,
                          MPU_RBAR_XN_NON_EXECUTION)
             | (mpu_region_begin & MPU_RBAR_BASE_Msk);

      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_ATOMIC();
      ARM_MPU_SetRegion(region_nbr, rbar, ARM_MPU_RLAR(mpu_region_end, MPU_MEMORY_ATTRIBUTE_IX_1));
      CORE_EXIT_ATOMIC();
      region_nbr++;
    }
  }

  // Enable MPU with default background region
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

  __DSB();
  __ISB();

  return status;
}

#if __CORTEX_M != (0u)
/**************************************************************************//**
 * MemManage default exception handler. Reset target.
 *****************************************************************************/
__WEAK void mpu_fault_handler(void)
{
  // Force fail assert to trigger reset
  __NVIC_SystemReset();
}

/**************************************************************************//**
 * MemManage exception handler.
 *****************************************************************************/
void MemManage_Handler(void)
{
  mpu_fault_handler();
}
#endif
