/***************************************************************************//**
 * @file
 * @brief Core interrupt handling API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_core.h"
#include "sl_assert.h"

/* *INDENT-OFF* */
// *****************************************************************************
///  @addtogroup core CORE - Interrupt Handling
///
///
///  @warning
///    If you are looking for atomic and critical sections, they have been moved
///    to platform/common in the Core API.
///
///  @section core_intro Introduction
///
///  This module provides support for NVIC sections. NVIC sections are like
///  critical sections, except interrupts are masked on an individual IRQ basis.
///  This module also provides an API to relocate the vector table in RAM, and
///  register IRQ handlers in the RAM based interrupt vector table.
///
///  @li <b>NVIC mask section</b>: Mask interrupts (external interrupts) on an
///     individual IRQ basis.
///
///  @section core_examples Examples
///
///  Implement an NVIC critical section:
///  @code{.c}
///  {
///    CORE_DECLARE_NVIC_ZEROMASK(mask); // A zero initialized NVIC disable mask
///
///    // Set mask bits for IRQs to block in the NVIC critical section.
///    // In many cases, you can create the disable mask once upon application
///    // startup and use the mask globally throughout the application lifetime.
///    CORE_NvicMaskSetIRQ(LEUART0_IRQn, &mask);
///    CORE_NvicMaskSetIRQ(VCMP_IRQn,    &mask);
///
///    // Enter NVIC critical section with the disable mask
///    CORE_NVIC_SECTION(&mask,
///      ...
///      ... your code goes here ...
///      ...
///    )
///  }
///  @endcode
///
///  @section core_vector_tables Interrupt vector tables
///
///  When using RAM based interrupt vector tables it is the user's responsibility
///  to allocate the table space correctly. The tables must be aligned as specified
///  in the CPU reference manual.
///
///  Use @ref CORE_InitNvicVectorTable() to initialize a RAM based vector table
///  by copying table entries from a source vector table to a target table.
///  VTOR is set to the address of the target vector table.
///
///  Use @ref CORE_GetNvicRamTableHandler() @ref CORE_SetNvicRamTableHandler()
///  to get or set the interrupt handler for a specific IRQn. They both use
///  the interrupt vector table defined by the current VTOR register value.
///
/// @{
// *****************************************************************************
/* *INDENT-ON* */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enter a NVIC mask section.
 *
 *   When a NVIC mask section is entered, specified NVIC interrupts
 *   are disabled.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[out] nvicState
 *   Return NVIC interrupts enable mask prior to section entry.
 *
 * @param[in] disable
 *   A mask specifying which NVIC interrupts to disable within the section.
 ******************************************************************************/
void CORE_EnterNvicMask(CORE_nvicMask_t *nvicState,
                        const CORE_nvicMask_t *disable)
{
  CORE_CRITICAL_SECTION(
    *nvicState = *(CORE_nvicMask_t*)((uint32_t)&NVIC->ICER[0]);
    *(CORE_nvicMask_t*)((uint32_t)&NVIC->ICER[0]) = *disable;
    )
}

/***************************************************************************//**
 * @brief
 *   Disable NVIC interrupts.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[in] disable
 *   A mask specifying which NVIC interrupts to disable.
 ******************************************************************************/
void CORE_NvicDisableMask(const CORE_nvicMask_t *disable)
{
  CORE_CRITICAL_SECTION(
    *(CORE_nvicMask_t*)((uint32_t)&NVIC->ICER[0]) = *disable;
    )
}

/***************************************************************************//**
 * @brief
 *   Set current NVIC interrupt enable mask.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[out] enable
 *   A mask specifying which NVIC interrupts are currently enabled.
 ******************************************************************************/
void CORE_NvicEnableMask(const CORE_nvicMask_t *enable)
{
  CORE_CRITICAL_SECTION(
    *(CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]) = *enable;
    )
}

/***************************************************************************//**
 * @brief
 *   Brief NVIC interrupt enable/disable sequence to allow handling of
 *   pending interrupts.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[in] enable
 *   A mask specifying which NVIC interrupts to briefly enable.
 *
 * @note
 *   Usually used within an NVIC mask section.
 ******************************************************************************/
void CORE_YieldNvicMask(const CORE_nvicMask_t *enable)
{
  CORE_nvicMask_t nvicMask;

  // Get current NVIC enable mask.
  CORE_CRITICAL_SECTION(
    nvicMask = *(CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]);
    )

  // Make a mask with bits set for those interrupts that are currently
  // disabled but are set in the enable mask.
#if (CORE_NVIC_REG_WORDS == 1)
  nvicMask.a[0] &= enable->a[0];
  nvicMask.a[0] = ~nvicMask.a[0] & enable->a[0];

  if (nvicMask.a[0] != 0) {
#elif (CORE_NVIC_REG_WORDS == 2)
  nvicMask.a[0] &= enable->a[0];
  nvicMask.a[1] &= enable->a[1];
  nvicMask.a[0] = ~nvicMask.a[0] & enable->a[0];
  nvicMask.a[1] = ~nvicMask.a[1] & enable->a[1];

  if ((nvicMask.a[0] != 0U) || (nvicMask.a[1] != 0U)) {
#elif (CORE_NVIC_REG_WORDS == 3)
  nvicMask.a[0] &= enable->a[0];
  nvicMask.a[1] &= enable->a[1];
  nvicMask.a[2] &= enable->a[2];
  nvicMask.a[0] = ~nvicMask.a[0] & enable->a[0];
  nvicMask.a[1] = ~nvicMask.a[1] & enable->a[1];
  nvicMask.a[2] = ~nvicMask.a[2] & enable->a[2];

  if ((nvicMask.a[0] != 0U) || (nvicMask.a[1] != 0U) || (nvicMask.a[2] != 0U)) {
#endif

    // Enable previously disabled interrupts.
    *(CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]) = nvicMask;

    // Disable those interrupts again.
    *(CORE_nvicMask_t*)((uint32_t)&NVIC->ICER[0]) = nvicMask;
  }
}

/***************************************************************************//**
 * @brief
 *   Utility function to set an IRQn bit in a NVIC enable/disable mask.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *   Use sl_interrupt_manager_enable from the interrupt_manager service to replace
 *   the calls to this API. Note that each interrupts will need to be enabled
 *   individually instead of using a mask.
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt.
 *
 * @param[in,out] mask
 *   The mask to set the interrupt bit in.
 ******************************************************************************/
void CORE_NvicMaskSetIRQ(IRQn_Type irqN, CORE_nvicMask_t *mask)
{
  EFM_ASSERT(((int)irqN >= 0) && ((int)irqN < EXT_IRQ_COUNT));
  mask->a[(unsigned)irqN >> 5] |= 1UL << ((unsigned)irqN & 0x1FUL);
}

/***************************************************************************//**
 * @brief
 *   Utility function to clear an IRQn bit in a NVIC enable/disable mask.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *   Use sl_interrupt_manager_enable from the interrupt_manager service to replace
 *   the calls to this API. Note that each interrupts will need to be disabled
 *   individually instead of using a mask.
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt.
 *
 * @param[in,out] mask
 *   The mask to clear the interrupt bit in.
 ******************************************************************************/
void CORE_NvicMaskClearIRQ(IRQn_Type irqN, CORE_nvicMask_t *mask)
{
  EFM_ASSERT(((int)irqN >= 0) && ((int)irqN < EXT_IRQ_COUNT));
  mask->a[(unsigned)irqN >> 5] &= ~(1UL << ((unsigned)irqN & 0x1FUL));
}

/***************************************************************************//**
 * @brief
 *   Get the current NVIC enable mask state.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[out] mask
 *   The current NVIC enable mask.
 ******************************************************************************/
void CORE_GetNvicEnabledMask(CORE_nvicMask_t *mask)
{
  CORE_CRITICAL_SECTION(
    *mask = *(CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]);
    )
}

/***************************************************************************//**
 * @brief
 *   Get NVIC disable state for a given mask.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[in] mask
 *   An NVIC mask to check.
 *
 * @return
 *   True if all NVIC interrupt mask bits are clear.
 ******************************************************************************/
bool CORE_GetNvicMaskDisableState(const CORE_nvicMask_t *mask)
{
  CORE_nvicMask_t nvicMask;

  CORE_CRITICAL_SECTION(
    nvicMask = *(CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]);
    )

#if (CORE_NVIC_REG_WORDS == 1)
  return (mask->a[0] & nvicMask.a[0]) == 0U;

#elif (CORE_NVIC_REG_WORDS == 2)
  return ((mask->a[0] & nvicMask.a[0]) == 0U)
         && ((mask->a[1] & nvicMask.a[1]) == 0U);

#elif (CORE_NVIC_REG_WORDS == 3)
  return ((mask->a[0] & nvicMask.a[0]) == 0U)
         && ((mask->a[1] & nvicMask.a[1]) == 0U)
         && ((mask->a[2] & nvicMask.a[2]) == 0U);
#endif
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/***************************************************************************//**
 * @brief Internal function to query the state an IRQ. Present for compatibility
 *        during the deprecation process.
 *
 * @param[in] irqN IRQ number.
 *
 * @return True of False depending on if the IRQ is active in the NVIC or not.
 ******************************************************************************/
__STATIC_INLINE bool internal_NvicIRQDisabled(IRQn_Type irqN)
{
  CORE_nvicMask_t *mask;

  EFM_ASSERT(((int)irqN >= 0) && ((int)irqN < EXT_IRQ_COUNT));
  mask = (CORE_nvicMask_t*)((uint32_t)&NVIC->ISER[0]);
  return (mask->a[(unsigned)irqN >> 5U] & (1UL << ((unsigned)irqN & 0x1FUL)))
         == 0UL;
}
/** @endcond */

/***************************************************************************//**
 * @brief
 *   Check if an NVIC interrupt is disabled.
 *
 * @deprecated Will be removed from the Simplicity SDK, please now use the
 *  function sl_interrupt_manager_is_irq_disabled in the interrupt manager
 *  service component.
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt to check.
 *
 * @return
 *   True if the interrupt is disabled.
 ******************************************************************************/
bool CORE_NvicIRQDisabled(IRQn_Type irqN)
{
  return internal_NvicIRQDisabled(irqN);
}

/***************************************************************************//**
 * @brief
 *   Utility function to get the handler for a specific interrupt.
 *
 * @deprecated Will be removed from the Simplicity SDK
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt.
 *
 * @return
 *   The handler address.
 *
 * @note
 *   Uses the interrupt vector table defined by the current VTOR register value.
 ******************************************************************************/
void *CORE_GetNvicRamTableHandler(IRQn_Type irqN)
{
  EFM_ASSERT(((int)irqN >= -16) && ((int)irqN < EXT_IRQ_COUNT));
  return (void*)((uint32_t*)(((uint32_t*)SCB->VTOR)[(int)irqN + 16]));
}

/***************************************************************************//**
 * @brief
 *   Utility function to set the handler for a specific interrupt.
 *
 * @deprecated Will be removed from the Simplicity SDK, please now use the
 *  function sl_interrupt_manager_set_irq_handler in the interrupt manager
 *  service component.
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt.
 *
 * @param[in] handler
 *   The handler address.
 *
 * @note
 *   Uses the interrupt vector table defined by the current VTOR register value.
 ******************************************************************************/
void CORE_SetNvicRamTableHandler(IRQn_Type irqN, void *handler)
{
  EFM_ASSERT(((int)irqN >= -16) && ((int)irqN < EXT_IRQ_COUNT));
  ((uint32_t*)SCB->VTOR)[(int)irqN + 16] = (uint32_t)((uint32_t*)handler);
}

/***************************************************************************//**
 * @brief
 *   Initialize an interrupt vector table by copying table entries from a
 *   source to a target table.
 *
 * @note This function will set a new VTOR register value.
 *
 * @param[in] sourceTable
 *   The address of the source vector table.
 *
 * @param[in] sourceSize
 *   A number of entries in the source vector table.
 *
 * @param[in] targetTable
 *   The address of the target (new) vector table.
 *
 * @param[in] targetSize
 *   A number of entries in the target vector table.
 *
 * @param[in] defaultHandler
 *   An address of the interrupt handler used for target entries for which where there
 *   is no corresponding source entry (i.e., the target table is larger than the source
 *   table).
 *
 * @param[in] overwriteActive
 *   When true, a target table entry is always overwritten with the
 *   corresponding source entry. If false, a target table entry is only
 *   overwritten if it is zero. This makes it possible for an application
 *   to partly initialize a target table before passing it to this function.
 *
 ******************************************************************************/
void CORE_InitNvicVectorTable(uint32_t *sourceTable,
                              uint32_t sourceSize,
                              uint32_t *targetTable,
                              uint32_t targetSize,
                              void *defaultHandler,
                              bool overwriteActive)
{
  uint32_t i;

  // ASSERT on non SRAM-based target table.
  EFM_ASSERT(((uint32_t)targetTable >= SRAM_BASE)
             && ((uint32_t)targetTable < (SRAM_BASE + SRAM_SIZE)));

  // ASSERT if misaligned with respect to the VTOR register implementation.
#if defined(SCB_VTOR_TBLBASE_Msk)
  EFM_ASSERT(((uint32_t)targetTable & ~(SCB_VTOR_TBLOFF_Msk
                                        | SCB_VTOR_TBLBASE_Msk)) == 0U);
#else
  EFM_ASSERT(((uint32_t)targetTable & ~SCB_VTOR_TBLOFF_Msk) == 0U);
#endif

  // ASSERT if misaligned with respect to the vector table size.
  // The vector table address must be aligned at its size rounded up to nearest 2^n.
  EFM_ASSERT(((uint32_t)targetTable
              & ((1UL << (32UL - __CLZ((targetSize * 4UL) - 1UL))) - 1UL))
             == 0UL);

  for (i = 0; i < targetSize; i++) {
    if (overwriteActive) {                      // Overwrite target entries.
      if (i < sourceSize) {                     //   targetSize <= sourceSize
        targetTable[i] = sourceTable[i];
      } else {                                  //   targetSize > sourceSize
        targetTable[i] = (uint32_t)((uint32_t*)defaultHandler);
      }
    } else {                            // Overwrite target entries which are 0.
      if (i < sourceSize) {                     // targetSize <= sourceSize
        if (targetTable[i] == 0U) {
          targetTable[i] = sourceTable[i];
        }
      } else {                                  // targetSize > sourceSize
        if (targetTable[i] == 0U) {
          targetTable[i] = (uint32_t)((uint32_t*)defaultHandler);
        }
      }
    }
  }
  SCB->VTOR = (uint32_t)targetTable;
}

/***************************************************************************//**
 * @brief
 *   Check if a specific interrupt is disabled or blocked.
 *
 * @deprecated Will be removed from the Simplicity SDK, please now use the
 *  function sl_interrupt_manager_is_irq_blocked in the interrupt manager
 *  service component.
 *
 * @param[in] irqN
 *   The IRQn_Type enumerator for the interrupt to check.
 *
 * @return
 *   True if the interrupt is disabled or blocked.
 ******************************************************************************/
SL_WEAK bool CORE_IrqIsBlocked(IRQn_Type irqN)
{
  uint32_t irqPri, activeIrq;

#if (__CORTEX_M >= 3)
  uint32_t basepri;

  EFM_ASSERT((irqN >= MemoryManagement_IRQn)
             && (irqN < (IRQn_Type)EXT_IRQ_COUNT));
#else
  EFM_ASSERT((irqN >= SVCall_IRQn) && ((IRQn_Type)irqN < EXT_IRQ_COUNT));
#endif

  if ((__get_PRIMASK() & 1U) != 0U) {
    return true;                            // All IRQs are disabled.
  }

  if (internal_NvicIRQDisabled(irqN)) {
    return true;                            // The IRQ in question is disabled.
  }

  irqPri  = NVIC_GetPriority(irqN);
#if (__CORTEX_M >= 3)
  basepri = __get_BASEPRI();
  if ((basepri != 0U)
      && (irqPri >= (basepri >> (8U - __NVIC_PRIO_BITS)))) {
    return true;                            // The IRQ in question has too low
  }                                         // priority vs. BASEPRI.
#endif

  // Check if already in an interrupt handler. If so, an interrupt with a
  // higher priority (lower priority value) can preempt.
  activeIrq = (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos;
  if (activeIrq != 0U) {
    if (irqPri >= NVIC_GetPriority((IRQn_Type)(activeIrq - 16U))) {
      return true;                          // The IRQ in question has too low
    }                                       // priority vs. current active IRQ
  }

  return false;
}

/** @} (end addtogroup core) */
