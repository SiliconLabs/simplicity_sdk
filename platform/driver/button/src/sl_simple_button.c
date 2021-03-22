/***************************************************************************//**
 * @file
 * @brief Simple Button Driver
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

#include "sl_simple_button.h"
#include "sl_simple_button_config.h"

#if defined(SL_CATALOG_GPIO_PRESENT)
#include "sl_gpio.h"
#else
#include "gpiointerrupt.h"
#endif

#include "sl_clock_manager.h"

#if (SL_SIMPLE_BUTTON_DEBOUNCE_BITS < 1U)
#undef SL_SIMPLE_BUTTON_DEBOUNCE_BITS
#define SL_SIMPLE_BUTTON_DEBOUNCE_BITS  1U
#endif
#if (SL_SIMPLE_BUTTON_DEBOUNCE_BITS > 15U)
#undef SL_SIMPLE_BUTTON_DEBOUNCE_BITS
#define SL_SIMPLE_BUTTON_DEBOUNCE_BITS  15U
#endif

static const uint16_t check_press = (uint16_t)(0xffff << SL_SIMPLE_BUTTON_DEBOUNCE_BITS);
static const uint16_t check_release = (uint16_t)(~(0x1 << SL_SIMPLE_BUTTON_DEBOUNCE_BITS));
static const uint16_t debounce_window = (uint16_t)(0xffff << (SL_SIMPLE_BUTTON_DEBOUNCE_BITS + 1));

/***************************************************************************//**
 * An internal callback called in interrupt context whenever a button changes
 * its state. (mode - SL_SIMPLE_BUTTON_MODE_INTERRUPT)
 *
 * @note The button state is updated by this function. The application callback
 * should not update it again.
 *
 * @param[in] interrupt_no      Interrupt number (pin number)
 * @param[in] ctx               Pointer to button handle
 ******************************************************************************/
static void sli_simple_button_on_change(uint8_t interrupt_no, void *ctx)
{
  (void)interrupt_no;
  sl_button_t *button = (sl_button_t *)ctx;
  sl_simple_button_context_t *simple_button = button->context;

  if (simple_button->state != SL_SIMPLE_BUTTON_DISABLED) {
#if defined(SL_CATALOG_GPIO_PRESENT)
    sl_gpio_t gpio;
    bool pin_value;
    gpio.port = simple_button->port;
    gpio.pin = simple_button->pin;
    sl_gpio_get_pin_input(&gpio, &pin_value);
    simple_button->state = ((bool)pin_value == SL_SIMPLE_BUTTON_POLARITY);
#else
    simple_button->state = ((bool)GPIO_PinInGet(simple_button->port, simple_button->pin) == SL_SIMPLE_BUTTON_POLARITY);
#endif

    sl_button_on_change(button);
  }
}

sl_status_t sl_simple_button_init(const sl_button_t *handle)
{
  int32_t interrupt;
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

#if defined(SL_CATALOG_GPIO_PRESENT)
  sl_gpio_t gpio;
  bool pin_value;
  gpio.port = simple_button->port;
  gpio.pin = simple_button->pin;
  sl_gpio_set_pin_mode(&gpio, SL_SIMPLE_BUTTON_GPIO_MODE, SL_SIMPLE_BUTTON_GPIO_DOUT);
  sl_gpio_get_pin_input(&gpio, &pin_value);
  simple_button->state = ((bool)pin_value == SL_SIMPLE_BUTTON_POLARITY);
#else
  GPIO_PinModeSet(simple_button->port,
                  simple_button->pin,
                  SL_SIMPLE_BUTTON_GPIO_MODE,
                  SL_SIMPLE_BUTTON_GPIO_DOUT);

  simple_button->state = ((bool)GPIO_PinInGet(simple_button->port, simple_button->pin) == SL_SIMPLE_BUTTON_POLARITY);
#endif

  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_INTERRUPT) {
#if defined(SL_CATALOG_GPIO_PRESENT)
    interrupt = SL_GPIO_INTERRUPT_UNAVAILABLE;
    sl_status_t status = sl_gpio_configure_external_interrupt(&gpio,
                                                              &interrupt,
                                                              SL_GPIO_INTERRUPT_RISING_FALLING_EDGE,
                                                              (sl_gpio_irq_callback_t)sli_simple_button_on_change,
                                                              button);
    EFM_ASSERT(status == SL_STATUS_OK);
#else
    GPIOINT_Init();

    // Try to register an EM4WU interrupt for the given pin
    interrupt = GPIOINT_EM4WUCallbackRegisterExt(simple_button->port,
                                                 simple_button->pin,
                                                 (GPIOINT_IrqCallbackPtrExt_t)sli_simple_button_on_change,
                                                 button);
    if (interrupt == INTERRUPT_UNAVAILABLE) {
      // if the pin not EM4WU-compatible, instead register a regualr interrupt
      interrupt = GPIOINT_CallbackRegisterExt(simple_button->pin,
                                              (GPIOINT_IrqCallbackPtrExt_t)sli_simple_button_on_change,
                                              button);
      EFM_ASSERT(interrupt != INTERRUPT_UNAVAILABLE);
      GPIO_ExtIntConfig(simple_button->port,
                        simple_button->pin,
                        interrupt,
                        true,
                        true,
                        true);
    } else {
      // If the pin is EM4WU-compatible, setup the pin as an EM4WU pin
      GPIO_EM4WUExtIntConfig(simple_button->port,
                             simple_button->pin,
                             interrupt,
                             SL_SIMPLE_BUTTON_POLARITY,
                             true);

      // Since EM4WU interrupts are level-sensitive and not edge-sensitive, also register a regular edge-sensitive interrupt to capture the other edge
      interrupt = GPIOINT_CallbackRegisterExt(simple_button->pin,
                                              (GPIOINT_IrqCallbackPtrExt_t)sli_simple_button_on_change,
                                              button);
      EFM_ASSERT(interrupt != INTERRUPT_UNAVAILABLE);
      GPIO_ExtIntConfig(simple_button->port,
                        simple_button->pin,
                        interrupt,
                        (SL_SIMPLE_BUTTON_POLARITY == 0U), // Register a Rising Edge interrupt for an Active Low button
                        (SL_SIMPLE_BUTTON_POLARITY == 1U), // Register a Falling Edge interrupt for an Active High button
                        true);
    }
#endif
  }

  return SL_STATUS_OK;
}

sl_button_state_t sl_simple_button_get_state(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  return simple_button->state;
}

void sl_simple_button_poll_step(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;
  bool button_press;

  if (simple_button->state == SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }

#if defined(SL_CATALOG_GPIO_PRESENT)
  sl_gpio_t gpio;
  bool pin_value;
  gpio.port = simple_button->port;
  gpio.pin = simple_button->pin;
  sl_gpio_get_pin_input(&gpio, &pin_value);
  button_press = (bool)pin_value;
#else
  button_press = (bool)GPIO_PinInGet(simple_button->port, simple_button->pin);
#endif

  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE) {
    uint16_t history = simple_button->history;
    history = (history << 1) | (button_press ^ SL_SIMPLE_BUTTON_POLARITY) | (debounce_window);

    if (history == check_press) {
      simple_button->state = SL_SIMPLE_BUTTON_PRESSED;
    }
    if (history == check_release) {
      simple_button->state = SL_SIMPLE_BUTTON_RELEASED;
    }

    simple_button->history = history;
  } else if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_POLL) {
    simple_button->state = (button_press == SL_SIMPLE_BUTTON_POLARITY);
  }
}

void sl_simple_button_enable(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  // Return if the button is not disabled
  if (simple_button->state != SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }

  // Clear history
  simple_button->history = 0;
  // Reinit button
  sl_simple_button_init(handle);
}

void sl_simple_button_disable(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  // Return if the button is disabled
  if (simple_button->state == SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }
  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_INTERRUPT) {
#if defined(SL_CATALOG_GPIO_PRESENT)
    sl_gpio_deconfigure_external_interrupt(simple_button->pin);
#else
    GPIOINT_CallbackUnRegister(simple_button->pin);
    // Disable interrupts
    GPIO_ExtIntConfig(simple_button->port,
                      simple_button->pin,
                      simple_button->pin,
                      false,
                      false,
                      false);
#endif
  }
  // Disable the button
  simple_button->state = SL_SIMPLE_BUTTON_DISABLED;
}
