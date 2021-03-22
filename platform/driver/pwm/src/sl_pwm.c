/***************************************************************************//**
 * @file
 * @brief PWM Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_pwm.h"

#if defined(SL_CATALOG_GPIO_PRESENT)
#include "sl_gpio.h"
#else
#include "em_gpio.h"
#endif

#if defined(_SILICON_LABS_32B_SERIES_3)
#include "sl_hal_bus.h"
#include "sl_hal_timer.h"
#else
#include "em_bus.h"
#include "em_timer.h"
#endif

#include "sl_clock_manager.h"
#include "sl_device_peripheral.h"

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// stores the address of corresponding sl_peripheral_t variable
static sl_peripheral_t peripheral;

static sl_bus_clock_t get_timer_clock(TIMER_TypeDef *timer)
{
  sl_bus_clock_t timer_clock = 0;
  switch ((uint32_t)timer) {
#if defined(TIMER0_BASE)
    case TIMER0_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER0;
      peripheral = SL_PERIPHERAL_TIMER0;
      break;
#endif
#if defined(TIMER1_BASE)
    case TIMER1_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER1;
      peripheral = SL_PERIPHERAL_TIMER1;
      break;
#endif
#if defined(TIMER2_BASE)
    case TIMER2_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER2;
      peripheral = SL_PERIPHERAL_TIMER2;
      break;
#endif
#if defined(TIMER3_BASE)
    case TIMER3_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER3;
      peripheral = SL_PERIPHERAL_TIMER3;
      break;
#endif
#if defined(TIMER4_BASE)
    case TIMER4_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER4;
      peripheral = SL_PERIPHERAL_TIMER4;
      break;
#endif
#if defined(TIMER5_BASE)
    case TIMER5_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER5;
      peripheral = SL_PERIPHERAL_TIMER5;
      break;
#endif
#if defined(TIMER6_BASE)
    case TIMER6_BASE:
      timer_clock = SL_BUS_CLOCK_TIMER6;
      peripheral = SL_PERIPHERAL_TIMER6;
      break;
#endif
    default:
      EFM_ASSERT(0);
      break;
  }
  return timer_clock;
}

sl_status_t sl_pwm_init(sl_pwm_instance_t *pwm, sl_pwm_config_t *config)
{
  sl_bus_clock_t timer_clock = get_timer_clock(pwm->timer);
  sl_clock_manager_enable_bus_clock(timer_clock);

  // Set PWM pin as output
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

#if defined(SL_CATALOG_GPIO_PRESENT)
  sl_gpio_t gpio;
  gpio.port = (sl_gpio_port_t)pwm->port;
  gpio.pin = pwm->pin;
  sl_gpio_set_pin_mode(&gpio,
                       SL_GPIO_MODE_PUSH_PULL,
                       config->polarity);
#else
  GPIO_PinModeSet((GPIO_Port_TypeDef)pwm->port,
                  pwm->pin,
                  gpioModePushPull,
                  config->polarity);
#endif

  // Set CC channel parameters
#if defined(_SILICON_LABS_32B_SERIES_2)
  TIMER_InitCC_TypeDef channel_init = TIMER_INITCC_DEFAULT;
  channel_init.mode = timerCCModePWM;
  channel_init.cmoa = timerOutputActionToggle;
  channel_init.edge = timerEdgeBoth;
  channel_init.outInvert = (config->polarity == PWM_ACTIVE_LOW);
  TIMER_InitCC(pwm->timer, pwm->channel, &channel_init);
#else
  sl_hal_timer_channel_config_t channel_init = SL_HAL_TIMER_CHANNEL_CONFIG_DEFAULT;
  channel_init.channel_mode = SL_HAL_TIMER_CHANNEL_MODE_PWM;
  channel_init.compare_match_output_action = SL_HAL_TIMER_CHANNEL_OUTPUT_ACTION_TOGGLE;
  channel_init.input_capture_edge = SL_HAL_TIMER_CHANNEL_EDGE_BOTH;
  channel_init.output_invert = (config->polarity == PWM_ACTIVE_LOW);
  sl_hal_timer_channel_init(pwm->timer, pwm->channel, &channel_init);
#endif
  // Configure CC channel pinout
#if defined(_TIMER_ROUTE_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedWrite(&pwm->timer->ROUTE,
                     _TIMER_ROUTE_LOCATION_MASK,
                     pwm->location << _TIMER_ROUTE_LOCATION_SHIFT);
#else
  sl_hal_bus_reg_write_mask(&pwm->timer->ROUTE,
                            _TIMER_ROUTE_LOCATION_MASK,
                            pwm->location << _TIMER_ROUTE_LOCATION_SHIFT);
#endif
#elif defined(_TIMER_ROUTELOC0_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedWrite(&pwm->timer->ROUTELOC0,
                     _TIMER_ROUTELOC0_CC0LOC_MASK << (pwm->channel * 8U),
                     pwm->location << (pwm->channel * 8U));
#else
  sl_hal_bus_reg_write_mask(&pwm->timer->ROUTELOC0,
                            _TIMER_ROUTELOC0_CC0LOC_MASK << (pwm->channel * 8U),
                            pwm->location << (pwm->channel * 8U));
#endif
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  volatile uint32_t * route_register = &GPIO->TIMERROUTE[TIMER_NUM(pwm->timer)].CC0ROUTE;
  route_register += pwm->channel;
  *route_register = (pwm->port << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (pwm->pin << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
#else
#error "Unknown route setting"
#endif

  // Configure TIMER frequency
  uint32_t clock_freq;
  sl_clock_branch_t clock_branch;

  clock_branch = sl_device_peripheral_get_clock_branch(peripheral);
  sl_clock_manager_get_clock_branch_frequency(clock_branch, &clock_freq);
  uint32_t top = (clock_freq / (config->frequency)) - 1U;
#if defined(_SILICON_LABS_32B_SERIES_2)
  TIMER_TopSet(pwm->timer, top);

  // Set initial duty cycle to 0%
  TIMER_CompareSet(pwm->timer, pwm->channel, 0U);

  // Initialize TIMER
  TIMER_Init_TypeDef timer_init = TIMER_INIT_DEFAULT;
  TIMER_Init(pwm->timer, &timer_init);
#else
  sl_hal_timer_set_top(pwm->timer, top);

  // Set initial duty cycle to 0%
  sl_hal_timer_channel_set_compare(pwm->timer, pwm->channel, 0U);

  // Initialize TIMER
  sl_hal_timer_config_t timer_init = SL_HAL_TIMER_CONFIG_DEFAULT;
  sl_hal_timer_init(pwm->timer, &timer_init);
#endif

  return SL_STATUS_OK;
}

sl_status_t sl_pwm_deinit(sl_pwm_instance_t *pwm)
{
  // Reset TIMER routes
  sl_pwm_stop(pwm);

#if defined(_TIMER_ROUTE_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedClear(&pwm->timer->ROUTE, _TIMER_ROUTE_LOCATION_MASK);
#else
  sl_hal_bus_reg_clear_mask(&pwm->timer->ROUTE, _TIMER_ROUTE_LOCATION_MASK);
#endif
#elif defined(_TIMER_ROUTELOC0_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedClear(&pwm->timer->ROUTELOC0, _TIMER_ROUTELOC0_CC0LOC_MASK << (pwm->channel * 8));
#else
  sl_hal_bus_reg_clear_mask(&pwm->timer->ROUTELOC0, _TIMER_ROUTELOC0_CC0LOC_MASK << (pwm->channel * 8));
#endif
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  volatile uint32_t * route_register = &GPIO->TIMERROUTE[TIMER_NUM(pwm->timer)].CC0ROUTE;
  route_register += pwm->channel;
  *route_register = 0;
#else
#error "Unknown route setting"
#endif

  // Reset TIMER
#if defined(_SILICON_LABS_32B_SERIES_2)
  TIMER_Reset(pwm->timer);
#else
  sl_hal_timer_reset(pwm->timer);
#endif

  // Reset GPIO
#if defined(SL_CATALOG_GPIO_PRESENT)
  sl_gpio_t reset_gpio;
  reset_gpio.port = (sl_gpio_port_t)pwm->port;
  reset_gpio.pin = pwm->pin;
  sl_gpio_set_pin_mode(&reset_gpio,
                       SL_GPIO_MODE_DISABLED,
                       0);
#else
  GPIO_PinModeSet((GPIO_Port_TypeDef)pwm->port,
                  pwm->pin,
                  gpioModeDisabled,
                  0);
#endif

  sl_bus_clock_t timer_clock = get_timer_clock(pwm->timer);
  sl_clock_manager_disable_bus_clock(timer_clock);

  return SL_STATUS_OK;
}

void sl_pwm_start(sl_pwm_instance_t *pwm)
{
  // Enable PWM output
#if defined(_TIMER_ROUTE_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedSet(&pwm->timer->ROUTE,
                   1 << (pwm->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#else
  sl_hal_bus_reg_set_mask(&pwm->timer->ROUTE,
                          1 << (pwm->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#endif
#elif defined(_TIMER_ROUTELOC0_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedSet(&pwm->timer->ROUTEPEN,
                   1 << (pwm->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#else
  sl_hal_bus_reg_set_mask(&pwm->timer->ROUTEPEN,
                          1 << (pwm->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#endif
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  GPIO->TIMERROUTE_SET[TIMER_NUM(pwm->timer)].ROUTEEN = 1 << (pwm->channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
#error "Unknown route setting"
#endif
}

void sl_pwm_stop(sl_pwm_instance_t *pwm)
{
  // Disable PWM output
#if defined(_TIMER_ROUTE_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedClear(&pwm->timer->ROUTE,
                     1 << (pwm->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#else
  sl_hal_bus_reg_clear_mask(&pwm->timer->ROUTE,
                            1 << (pwm->channel + _TIMER_ROUTE_CC0PEN_SHIFT));
#endif
#elif defined(_TIMER_ROUTELOC0_MASK)
#if defined(_SILICON_LABS_32B_SERIES_2)
  BUS_RegMaskedClear(&pwm->timer->ROUTEPEN,
                     1 << (pwm->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#else
  sl_hal_bus_reg_clear_mask(&pwm->timer->ROUTEPEN,
                            1 << (pwm->channel + _TIMER_ROUTEPEN_CC0PEN_SHIFT));
#endif
#elif defined(_GPIO_TIMER_ROUTEEN_MASK)
  GPIO->TIMERROUTE_CLR[TIMER_NUM(pwm->timer)].ROUTEEN = 1 << (pwm->channel + _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT);
#else
#error "Unknown route setting"
#endif

  // Keep timer running in case other channels are in use
}

void sl_pwm_set_duty_cycle(sl_pwm_instance_t *pwm, uint8_t percent)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  uint32_t top = TIMER_TopGet(pwm->timer);

  // Set compare value
  TIMER_CompareBufSet(pwm->timer, pwm->channel, (top * percent) / 100);
#else
  uint32_t top = sl_hal_timer_get_top(pwm->timer);

  // Set compare value
  sl_hal_timer_channel_set_compare_buffer(pwm->timer, pwm->channel, (top * percent) / 100);
#endif
}

uint8_t sl_pwm_get_duty_cycle(sl_pwm_instance_t *pwm)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  uint32_t top = TIMER_TopGet(pwm->timer);
  uint32_t compare = TIMER_CaptureGet(pwm->timer, pwm->channel);
#else
  uint32_t top = sl_hal_timer_get_top(pwm->timer);
  uint32_t compare = sl_hal_timer_channel_get_capture(pwm->timer, pwm->channel);
#endif

  uint8_t percent = (uint8_t)((compare * 100) / top);

  return percent;
}
