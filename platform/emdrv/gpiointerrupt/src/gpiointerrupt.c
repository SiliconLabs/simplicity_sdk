/***************************************************************************//**
 * @file
 * @brief GPIOINT API implementation
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

#include "gpiointerrupt.h"
#include "sl_assert.h"
#include "sl_common.h"
#include "sl_interrupt_manager.h"

/***************************************************************************//**
 * @addtogroup gpioint
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define _GPIOINT_IF_EVEN_MASK ((_GPIO_IF_MASK) & 0x55555555UL)
#define _GPIOINT_IF_ODD_MASK  ((_GPIO_IF_MASK) & 0xAAAAAAAAUL)

/*******************************************************************************
 ********************************   MACROS   ***********************************
 ******************************************************************************/

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

typedef struct {
  /* Pin interrupt number in range of 0 to 31 */
  uint32_t intNo;
  /* Pointer to the callback function */
  void *callback;
  /* Pointer to the callback context */
  void *context;
  /* True if callback takes a context */
  bool context_flag;
} GPIOINT_CallbackDesc_t;

/*******************************************************************************
 ********************************   GLOBALS   **********************************
 ******************************************************************************/

/* Array of user callbacks. One for each pin interrupt number. */
static GPIOINT_CallbackDesc_t gpioCallbacks[32] = { 0 };

/*******************************************************************************
 ******************************   PROTOTYPES   *********************************
 ******************************************************************************/
static void GPIOINT_IRQDispatcher(uint32_t iflags);

/** @endcond */

/*******************************************************************************
 ***************************   GLOBAL FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initialization of GPIOINT module.
 *
 ******************************************************************************/
void GPIOINT_Init(void)
{
  if (sl_interrupt_manager_is_irq_disabled(GPIO_ODD_IRQn)) {
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
  }
  if (sl_interrupt_manager_is_irq_disabled(GPIO_EVEN_IRQn)) {
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  }
}

/***************************************************************************//**
 * @brief
 *   Registers user callback for given pin interrupt number.
 *
 * @details
 *   Use this function to register a callback which shall be called upon
 *   interrupt generated for a given pin interrupt number.
 *   Interrupt itself must be configured externally. Function overwrites previously
 *   registered callback.
 *
 * @param[in] intNo
 *   Pin interrupt number for the callback.
 * @param[in] callbackPtr
 *   A pointer to callback function.
 ******************************************************************************/
void GPIOINT_CallbackRegister(uint8_t intNo, GPIOINT_IrqCallbackPtr_t callbackPtr)
{
  CORE_ATOMIC_SECTION(
    /* Dispatcher is used */
    gpioCallbacks[intNo].callback = (void *)callbackPtr;
    gpioCallbacks[intNo].context_flag = false;
    )
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Registers user em4 wakeup callback for given port and pin interrupt number.
 *
 * @details
 *   Use this function to register an EM4 wakeup callback with context which shall
 *   be called upon interrupt generated for a given pin number.
 *   The function will return an interrupt number if one is available and pin is
 *   EM4WU compatible.
 *   Interrupt itself must be configured externally.
 *
 * @param[in] port
 *   GPIO Port for the callback.
 * @param[in] pin
 *   Pin number for the callback.
 * @param[in] callbackPtr
 *   A pointer to callback function.
 * @param[in] callbackCtx
 *   A pointer to the callback context.
 *
 * @return
 *   Interrupt number, or INTERRUPT_UNAVAILABLE if all are in use or pin doesn't
 *   support em4 wakeup.
 ******************************************************************************/
unsigned int GPIOINT_EM4WUCallbackRegisterExt(GPIO_Port_TypeDef port,
                                              uint8_t pin,
                                              GPIOINT_IrqCallbackPtrExt_t callbackPtr,
                                              void *callbackCtx)
{
  CORE_DECLARE_IRQ_STATE;
  unsigned int intNo = INTERRUPT_UNAVAILABLE;

  CORE_ENTER_ATOMIC();

  if (false) {
    /* Check all the EM4WU Pins and check if given pin matches any of them. */
#if defined(GPIO_EM4WU0_PORT)
  } else if (GPIO_EM4WU0_PORT == port && GPIO_EM4WU0_PIN == pin) {
    intNo = 0;
#endif
#if defined(GPIO_EM4WU3_PORT)
  } else if (GPIO_EM4WU3_PORT == port && GPIO_EM4WU3_PIN == pin) {
    intNo = 3;
#endif
#if defined(GPIO_EM4WU4_PORT)
  } else if (GPIO_EM4WU4_PORT == port && GPIO_EM4WU4_PIN == pin) {
    intNo = 4;
#endif
#if defined(GPIO_EM4WU6_PORT)
  } else if (GPIO_EM4WU6_PORT == port && GPIO_EM4WU6_PIN == pin) {
    intNo = 6;
#endif
#if defined(GPIO_EM4WU7_PORT)
  } else if (GPIO_EM4WU7_PORT == port && GPIO_EM4WU7_PIN == pin) {
    intNo = 7;
#endif
#if defined(GPIO_EM4WU8_PORT)
  } else if (GPIO_EM4WU8_PORT == port && GPIO_EM4WU8_PIN == pin) {
    intNo = 8;
#endif
#if defined(GPIO_EM4WU9_PORT)
  } else if (GPIO_EM4WU9_PORT == port && GPIO_EM4WU9_PIN == pin) {
    intNo = 9;
#endif
#if defined(GPIO_EM4WU10_PORT)
  } else if (GPIO_EM4WU10_PORT == port && GPIO_EM4WU10_PIN == pin) {
    intNo = 10;
#endif
  }

  if (intNo != INTERRUPT_UNAVAILABLE) {
#if defined(_GPIO_IEN_EM4WUIEN_SHIFT)
    gpioCallbacks[_GPIO_IEN_EM4WUIEN_SHIFT + intNo].callback = (void *)callbackPtr;
    gpioCallbacks[_GPIO_IEN_EM4WUIEN_SHIFT + intNo].context = callbackCtx;
    gpioCallbacks[_GPIO_IEN_EM4WUIEN_SHIFT + intNo].context_flag = true;
#else
    gpioCallbacks[_GPIO_IEN_EM4WUIEN0_SHIFT + intNo].callback = (void *)callbackPtr;
    gpioCallbacks[_GPIO_IEN_EM4WUIEN0_SHIFT + intNo].context = callbackCtx;
    gpioCallbacks[_GPIO_IEN_EM4WUIEN0_SHIFT + intNo].context_flag = true;
#endif
  }

  CORE_EXIT_ATOMIC();

  return intNo;
}
#endif

/***************************************************************************//**
 * @brief
 *   Registers user callback for given pin interrupt number.
 *
 * @details
 *   Use this function to register a callback with context which shall be called upon
 *   interrupt generated for a given pin number.
 *   The function will return an interrupt number if one is available.
 *   Interrupt itself must be configured externally.
 *
 * @param[in] pin
 *   Pin number for the callback.
 * @param[in] callbackPtr
 *   A pointer to callback function.
 * @param[in] callbackCtx
 *   A pointer to the callback context.
 *
 * @return
 *   Interrupt number, or INTERRUPT_UNAVAILABLE if all are in use
 ******************************************************************************/
unsigned int GPIOINT_CallbackRegisterExt(uint8_t pin, GPIOINT_IrqCallbackPtrExt_t callbackPtr, void *callbackCtx)
{
  CORE_DECLARE_IRQ_STATE;
  unsigned int intNo = INTERRUPT_UNAVAILABLE;

  CORE_ENTER_ATOMIC();

#if defined(_GPIO_EXTIPINSELL_MASK)
  uint32_t intToCheck;
  uint32_t intGroupStart = (pin & 0xFFC);
  uint32_t intsEnabled = GPIO_EnabledIntGet();

  // loop through the interrupt group, starting
  // from the pin number, and take
  // the first available
  for (uint8_t i = 0; i < 4; i++) {
    intToCheck = intGroupStart + ((pin + i) & 0x3); // modulo 4
    if (((intsEnabled >> intToCheck) & 0x1) == 0) {
      intNo = (unsigned int)intToCheck;
      break;
    }
  }
#else
  if (gpioCallbacks[pin].callback == 0) {
    intNo = (unsigned int)pin;
  }
#endif

  if (intNo != INTERRUPT_UNAVAILABLE) {
    gpioCallbacks[intNo].callback = (void *)callbackPtr;
    gpioCallbacks[intNo].context = callbackCtx;
    gpioCallbacks[intNo].context_flag = true;
  }

  CORE_EXIT_ATOMIC();

  return intNo;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Function calls users callback for registered pin interrupts.
 *
 * @details
 *   This function is called when GPIO interrupts are handled by the IRQHandlers.
 *   Function gets even or odd interrupt flags and calls user callback
 *   registered for that pin. Function iterates on flags starting from MSB.
 *
 * @param iflags
 *  Interrupt flags which shall be handled by the dispatcher.
 *
 ******************************************************************************/
static void GPIOINT_IRQDispatcher(uint32_t iflags)
{
  uint32_t irqIdx;
  GPIOINT_CallbackDesc_t *callback;

  /* check for all flags set in IF register */
  while (iflags != 0U) {
    irqIdx = SL_CTZ(iflags);

    /* clear flag*/
    iflags &= ~(1UL << irqIdx);

    callback = &gpioCallbacks[irqIdx];
    if (callback->callback) {
      /* call user callback */
      if (callback->context_flag) {
        GPIOINT_IrqCallbackPtrExt_t func = (GPIOINT_IrqCallbackPtrExt_t)(callback->callback);
        func((uint8_t)irqIdx, callback->context);
      } else {
        GPIOINT_IrqCallbackPtr_t func = (GPIOINT_IrqCallbackPtr_t)(callback->callback);
        func((uint8_t)irqIdx);
      }
    }
  }
}

/***************************************************************************//**
 * @brief
 *   GPIO EVEN interrupt handler. Interrupt handler clears all IF even flags and
 *   call the dispatcher passing the flags which triggered the interrupt.
 *
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint32_t iflags;

  /* Get all even interrupts. */
  iflags = GPIO_IntGetEnabled() & _GPIOINT_IF_EVEN_MASK;

  /* Clean only even interrupts. */
  GPIO_IntClear(iflags);

  GPIOINT_IRQDispatcher(iflags);
}

/***************************************************************************//**
 * @brief
 *   GPIO ODD interrupt handler. Interrupt handler clears all IF odd flags and
 *   call the dispatcher passing the flags which triggered the interrupt.
 *
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  uint32_t iflags;

  /* Get all odd interrupts. */
  iflags = GPIO_IntGetEnabled() & _GPIOINT_IF_ODD_MASK;

  /* Clean only odd interrupts. */
  GPIO_IntClear(iflags);

  GPIOINT_IRQDispatcher(iflags);
}

/** @endcond */

/** @} (end addtogroup gpioint) */

/* *INDENT-OFF* */
// ******* THE REST OF THE FILE IS DOCUMENTATION ONLY !************************
/// @addtogroup gpioint GPIOINT - GPIO Interrupt
/// @brief GPIOINT General Purpose Input/Output Interrupt dispatcher
/// @{
///
///   @details
///   The source files for the GPIO interrupt dispatcher library resides in the
///   emdrv/gpiointerrupt folder, and are named gpiointerrupt.c and gpiointerrupt.h.
///
///
///   @n @section gpioint_intro Introduction
///   EFM32/EZR32/EFR32 has two GPIO interrupts lines, Odd and Even. If more
///   than two interrupts are used then interrupt routine must dispatch from a callback
///   register. This module provides small dispatcher for both GPIO interrupts enabling
///   handling of up to 32 GPIO pin interrupts.
///
///   It is up to the user to configure and enable interrupt on given pin. This can be done
///   using the GPIO library (emlib). This module handles the dispatch register and clearing of
///   interrupt flags.
///
///   In order to use this dispatcher, it has to be initialized first by
///   calling GPIOINT_Init(). Then each pin interrupt number must be configured by first
///   registering the callback function for given interrupt number and then configure and
///   enabling the interrupt number in the GPIO module.
///
///   The extended function GPIOINT_CallbackRegisterExt() may also be used to register a callback
///   with context for a given pin number. The first available interrupt number will be returned.
///
///   @n @section gpioint_api The API
///   This section contain brief descriptions of the functions in the API. You will
///   find detailed information on parameters by clicking on the hyperlinked function names.
///
///   Your application code must include one header file: @em gpiointerrupt.h.
///
///   @ref GPIOINT_Init() @n
///    This functions initializes the dispatcher register. Typically
///    GPIOINT_Init() is called once in your startup code.
///
///   @ref GPIOINT_CallbackRegister() @n
///    Register a callback function on a pin interrupt number.
///
///   @ref GPIOINT_CallbackUnRegister() @n
///    Un-register a callback function on a pin interrupt number.
///
///   @ref GPIOINT_CallbackRegisterExt() @n
///    Register a callback function with context on a pin number.
///
///   @n @section gpioint_example Example
///   @code{.c}
///
///#include "gpiointerrupt.h"
///
///#include "em_chip.h"
///#include "em_cmu.h"
///#include "em_gpio.h"
///
///// An array to track if given pin callback was called
///volatile uint8_t pinInt[32];
///
///// Gpio callbacks called when pin interrupt was triggered.
///void gpioCallback1(uint8_t intNo)
///{
///  pinInt[intNo]++;
///}
///
///void gpioCallback3(uint8_t intNo)
///{
///  pinInt[intNo]++;
///}
///
///void gpioCallback8(uint8_t intNo)
///{
///  pinInt[intNo]++;
///}
///
///int main(void)
///{
///  CHIP_Init();
///
///  // Enable clock for GPIO module, initialize GPIOINT
///  CMU_ClockEnable(cmuClock_GPIO, true);
///  GPIOINT_Init();
///
///  // Register callback functions and enable interrupts
///  GPIOINT_CallbackRegister(1, gpioCallback1);
///  GPIOINT_CallbackRegister(3, gpioCallback3);
///  unsigned int intPin8 = GPIOINT_CallbackRegisterExt(8, gpioCallback8, (void *)callback8context);
///  GPIO_IntEnable(1<<1 | 1<<3 | 1<<intPin8);
///
///  while(true);
///}
///
///   @endcode
///
/// @} end group gpioint *******************************************************
