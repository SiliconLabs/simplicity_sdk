/***************************************************************************//**
 * @file
 * @brief TZ Non-Secure Execution Start-up
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

#include "em_device.h"
#include "sl_common.h"

#if !defined(__ARM_FEATURE_CMSE) || (__ARM_FEATURE_CMSE != 3U)
  #error "The TZ Non-Secure execution code requires access to the CMSE toolchain extension to set proper SAU settings."
#endif // __ARM_FEATURE_CMSE

/*---------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

#define LINK_TIME_INJECTED_DATA_PATTERN   0x0DF0ADBA /* 0xBAADF00D backwards*/

#if defined (__GNUC__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define __ATTRIBUTE_SECURE_CONFIG_DATA    __attribute__((used, section(".secure_config_data")))
#define __ATTRIBUTE_SECURE_RESET_HANDLER  __attribute__((used, section(".secure_reset_handler")))
#define __ATTRIBUTE_SECURE_FAULT_HANDLER  __attribute__((used, section(".secure_fault_handler")))
#define __NO_PROLOGUE                     __attribute__((naked))

#elif defined(__ICCARM__)

#pragma data_alignment=512
#define __ATTRIBUTE_SECURE_CONFIG_DATA    SL_ATTRIBUTE_SECTION("secure_config_data")
#define __ATTRIBUTE_SECURE_RESET_HANDLER  SL_ATTRIBUTE_SECTION("secure_reset_handler")
#define __ATTRIBUTE_SECURE_FAULT_HANDLER  SL_ATTRIBUTE_SECTION("secure_fault_handler")
#define __NO_PROLOGUE                     __naked

#else
#error "Unsupported compiler."
#endif

/* This data is injected inline with the secure app's binary blob at link-time */
typedef struct secure_config_data {
  uint32_t *Secure_Vector_Table;
  uint32_t *NonSecure_Vector_Table;
  uint32_t MSPU_Region_Size;
} secure_config_data_t;

/*---------------------------------------------------------------------------
 * Internal References
 *---------------------------------------------------------------------------*/

void Secure_Reset_Handler(void);
void Secure_Default_Handler(void);

__ATTRIBUTE_SECURE_CONFIG_DATA secure_config_data_t Secure_Config_Data = {
  .Secure_Vector_Table = (uint32_t *)LINK_TIME_INJECTED_DATA_PATTERN,
  .NonSecure_Vector_Table = (uint32_t *)LINK_TIME_INJECTED_DATA_PATTERN,
  .MSPU_Region_Size = LINK_TIME_INJECTED_DATA_PATTERN
};

#if defined (__GNUC__)
#pragma GCC diagnostic pop
#endif // __GNUC__

/*---------------------------------------------------------------------------
 * Secure Reset Handler called on controller reset
 *---------------------------------------------------------------------------*/
__ATTRIBUTE_SECURE_RESET_HANDLER __NO_PROLOGUE void Secure_Reset_Handler(void)
{
/*
 * This code is meant to be pre-compiled and used as a binary blob in applications
 * that require the tz_non_secure_execution component. This source serves as a
 * reference to audit the code that is contained in the binary blob.
 */

/* The Non-Secure execution code takes care of moving between the security states.
 * It does all the essential configuration to run an application in Non-Secure
 * mode without the possibility of switching back and forth.
 *
 * The code below makes sure that all the peripherals are accessed from non-secure
 * address except SMU while also configuring the core to execute in Non-Secure mode.
 * The MSPU is configured so that all region in the DMEM and all NVM regions are Non-Secure
 *  except for the one NVM region in charge of switching to Non-Secure.
 */

  // Set Secure Vector Table.
  SCB->VTOR = (uint32_t)Secure_Config_Data.Secure_Vector_Table;

  // Set Non-Secure Stack pointer.
  __TZ_set_MSP_NS(*(Secure_Config_Data.NonSecure_Vector_Table));

  // Set Non-Secure vector table.
  SCB_NS->VTOR = (uint32_t)Secure_Config_Data.NonSecure_Vector_Table;

  // Enable SecureFault
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  // Enable BusFault, HardFault and NMI triggering in Non-Secure mode.
  // 0x5FA need to be written to the Vector Key field to write in the AIRCR register.
  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_BFHFNMINS_Msk);

  // Enables Non-Secure access to the Floating-Point Extension.
  SCB->NSACR |= (SCB_NSACR_CP10_Msk | SCB_NSACR_CP11_Msk | SCB_NSACR_CPn_Msk);

  // Set all Interrupt Non-Secure State (ITNS) bits. This results
  // in all IRQs being targeted at the NS world.
  __ASM volatile (

    // for (uint8_t i = 0; i < 16; i++) {
    //  NVIC->ITNS = 0xFFFFFFFF;
    // }

    "MOVS         R3, #0               \n"
    "MOV          R2, #0xFFFFFFFF      \n"
    "MOV          R1, %[itns]          \n"
    "nvic_itns_loop:                   \n"
    "STR          R2, [R1, R3, LSL #2] \n"
    "ADDS         R3, #1               \n"
    "CMP          R3, #16              \n"
    "BNE          nvic_itns_loop       \n"
    : // No outputs
    :[itns] "r" (&NVIC->ITNS)
    : "r1", "r2", "r3", "r5"
    );

  // Enable necessary clocks.
  CMU->CLKEN1_SET = CMU_CLKEN1_SMU | CMU_CLKEN1_L2ICACHE0 | CMU_CLKEN1_ICACHE0;

  // Enable Faults on BMPU and PPU access.
  SMU->BMPURSPERRD0_SET = _SMU_BMPURSPERRD0_MASK;
  SMU->PPURSPERRD_SET = _SMU_PPURSPERRD_MASK;

  // Set all Bus Master to Non-Secure.
  SMU->BMPUSATD0_CLR = _SMU_BMPUSATD0_MASK;

  // Set NVM MSPU region's size based on actual region's size in the linker.
  switch (Secure_Config_Data.MSPU_Region_Size) {
    case 0x4000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_16KB;
      break;
    case 0x8000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_32KB;
      break;
    case 0x10000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_64KB;
      break;
    case 0x20000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_128KB;
      break;
    case 0x40000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_256KB;
      break;
    case 0x80000:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_512KB;
      break;
    default:
      SMU->MSPUNVMREGIONSIZE_SET = SMU_MSPUNVMREGIONSIZE_MSPURSIZE_MSPU_16KB;
  }

  // Set all RAM regions to Non-Secure.
  SMU->MSPUDMEMNSREGIONFLAG0_SET = _SMU_MSPUDMEMNSREGIONFLAG0_MASK;

  // Fetch the Non-Secure Reset Handler's address and save it in a register.
  __ASM volatile (
    "MOV            R3, %[address_ns]   \n"
    "LDR            R4, [R3, #4]        \n"
    : // No outputs
    :[address_ns] "r" (Secure_Config_Data.NonSecure_Vector_Table)
    : "r3", "r4", "r5"
    );

  // Flush both Cache before switching to Non-Secure.
  __ASM volatile (

    // L1ICACHE0->CMD = ICACHE_CMD_INVALIDATE;
    "MOVS           R2, %[L1icache_flush] \n"
    "MOV            R3, %[L1icache_cmd]   \n"
    "STR            R2, [R3]              \n"

    // L2ICACHE0->FLUSHCMD = L2CACHE_FLUSHCMD_FLUSHALL;
    "MOVS           R2, %[L2icache_flush]     \n"
    "MOV            R3, %[L2icache_flushcmd]  \n"
    "STR            R2, [R3]									\n"

    // while (L2ICACHE0->STATUS & L2CACHE_STATUS_FLUSHRUNNING) ;
    "l2cache_flushrunning_loop:               \n"
    "LDR            R2, [R3]                  \n"
    "AND            R2, R2, %[L2icache_flush] \n"
    "CMP            R2, #0                    \n"
    "BNE            l2cache_flushrunning_loop \n"

    : // No outputs
    :[L1icache_cmd] "r" (&L1ICACHE0->CMD_SET),
    [L1icache_flush] "r" (ICACHE_CMD_INVALIDATE),
    [L2icache_flushcmd] "r" (&L2ICACHE0->FLUSHCMD_SET),
    [L2icache_flush] "r" (L2CACHE_FLUSHCMD_FLUSHALL),
    [L2icache_status] "r" (&L2ICACHE0->STATUS),
    [L2icache_flushrunning] "r" (L2CACHE_STATUS_FLUSHRUNNING)
    : "r2", "r3", "r4", "r5"
    );

  // Disable the clock for the Caches' modules.
  __ASM volatile (

    // CMU->CLKEN1_CLR = CMU_CLKEN1_L2ICACHE0 | CMU_CLKEN1_ICACHE0;
    "MOV      R3, %[clken1_clr]         \n"
    "MOV      R2, %[mask]               \n"
    "STR      R2, [R3]                  \n"
    : // No outputs.
    :[clken1_clr] "r" (&CMU->CLKEN1_CLR),
    [mask] "r" (CMU_CLKEN1_L2ICACHE0 | CMU_CLKEN1_ICACHE0)
    : "r2", "r3", "r4"
    );

  // Configure all peripherals to Non-Secure except for SMU.
  __ASM volatile (

    // SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK;
    "MOV      R3, %[smu_ppusatd0_clear]   \n"
    "MOV      R2, %[ppusatd0_mask]        \n"
    "STR      R2, [R3]                    \n"

    // SMU->PPUSATD1_CLR = _SMU_PPUSATD1_MASK & (~SMU_PPUSATD1_SMU);
    "MOV      R3, %[smu_ppusatd1_clear]   \n"
    "MOV      R2, %[ppusatd1_mask]        \n"
    "STR      R2, [R3]                    \n"
    : // No outputs.
    :[smu_ppusatd0_clear] "r" (&SMU->PPUSATD0_CLR),
    [ppusatd0_mask] "r" (_SMU_PPUSATD0_MASK),
    [smu_ppusatd1_clear] "r" (&SMU->PPUSATD1_CLR),
    [ppusatd1_mask] "r" (_SMU_PPUSATD1_MASK & (~SMU_PPUSATD1_SMU))
    : "r2", "r3", "r4", "r5"
    );

  // Enables the ESAU.
  __ASM volatile (

    // SMU->ESAURTYPES_SET = SMU_ESAURTYPES_ESAUEN;
    "MOV      R3, %[smu_esaurtypes_set]   \n"
    "MOVS     R2, %[esauen]               \n"
    "STR      R2, [R3]                    \n"
    : // No outputs.
    :[smu_esaurtypes_set] "r" (&SMU->ESAURTYPES_SET),
    [esauen] "r" (SMU_ESAURTYPES_ESAUEN)
    : "r2", "r3", "r4", "r5"
    );

  // Set all MSPU region as Non-Secure except for the one containing the
  // secure code necessary for configuring and switching the core to Non-Secure.
  __ASM volatile (

    // uint32_t *reg = &SMU->MSPUNVMNSREGIONFLAG0_SET;
    // uint32_t offset = (((uintptr_t)&__SECURE_VECTOR_TABLE & 0xFFFFFF) / (uintptr_t)&__secure_flash_size__);
    // do {
    //   if (offset >= 32) {
    //     *reg = 0xFFFFFFFF;
    //     offset -= 32;
    //   } else {
    //     *reg = ~(1 << offset);
    //     offset = 0xFFFFFFFF;
    //   }
    //   reg++;
    // } while (reg <= &SMU->MSPUNVMNSREGIONFLAG3_SET);

    "MOV            R1, %[nvm_region_flag0]           \n"
    "BIC            R3, %[secure_vector], #0xFF000000 \n"
    "UDIV           R3, R3, %[secure_flash_size]      \n"

    "nvm_region_reg_loop:                             \n"
    "CMP            R3, #32                           \n"
    "BLS            nvm_region_reg_with_secure_region \n"
    "SUB            R3, #32                           \n"
    "MOV            R2, #0xFFFFFFFF                   \n"
    "STR            R2, [R1]                          \n"
    "B              nvm_region_reg_loop_exit          \n"

    "nvm_region_reg_with_secure_region:               \n"
    "MOV            R2, #1                            \n"
    "LSLS           R2, R3                            \n"
    "MVN            R2, R2                            \n"
    "STR            R2, [R1]                          \n"
    "MOV            R3, #0xFFFFFFFF                   \n"
    "B              nvm_region_reg_loop_exit          \n"

    "nvm_region_reg_loop_exit:                        \n"
    "ADD            R1, #4                            \n"
    "CMP            R1, %[nvm_region_flag3]           \n"
    "BLE            nvm_region_reg_loop               \n"
    : // No outputs.
    :[secure_vector] "r" (Secure_Config_Data.Secure_Vector_Table),
    [secure_flash_size] "r" (Secure_Config_Data.MSPU_Region_Size),
    [nvm_region_flag0] "r" (&SMU->MSPUNVMNSREGIONFLAG0_SET),
    [nvm_region_flag3] "r" (&SMU->MSPUNVMNSREGIONFLAG3_SET)
    : "r1", "r2", "r3", "r4"
    );

  // Sets the core to Non-Secure using the SAU and calls the non-secure reset handler.
  // Should not return.
  __ASM volatile (
    "LSRS           R4, R4, #1          \n"
    "LSLS           R4, R4, #1          \n"

    // SAU->CTRL = SAU_CTRL_ALLNS_Msk;
    "MOVS           R2, #2              \n"
    "MOV            R3, %[SauCtrl]      \n"
    "STR            R2, [R3]            \n"

    // Invalidate all registers.
    "MOVS           R0, #0              \n"
    "MOVS           R1, #0              \n"
    "MOVS           R2, #0              \n"
    "MOVS           R3, #0              \n"
    "MOVS           R5, #0              \n"
    "MOVS           R6, #0              \n"
    "MOVS           R7, #0              \n"
    "MOVS           R8, #0              \n"
    "MOVS           R9, #0              \n"
    "MOVS           R10, #0             \n"
    "MOVS           R11, #0             \n"
    "MOVS           R12, #0             \n"
    "DSB                                \n"

    // Jump to Non-Secure Reset Handler (Application).
    "BXNS           R4                  \n"
    : // No outputs
    :[SauCtrl] "r" (&SAU->CTRL)
    : "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
    );
}

/*----------------------------------------------------------------------------
 * Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
__ATTRIBUTE_SECURE_FAULT_HANDLER __NO_PROLOGUE void Secure_Default_Handler(void)
{
  // Falling here means that an unauthorized bus access was tried triggering
  // a SecureFault. Care should be taken to not access Secure ressources.
  // Secure ressources are the SMU, the Secure aliases and the flash
  // MSPU region in charge of switching the core to Non-Secure.
  while (1) {
  }
}
