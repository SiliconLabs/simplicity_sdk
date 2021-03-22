/***************************************************************************//**
 * @file
 * @brief IO Stream EUSART Component.
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sl_status.h"
#include "sl_slist.h"
#include "sl_iostream.h"
#include "sl_iostream_uart.h"
#include "sli_iostream_uart.h"
#include "sl_iostream_eusart.h"
#include "sl_atomic.h"
#include "sl_clock_manager.h"
#include "em_device.h"
#include "sl_common.h"
#include "sl_core.h"

#if (_SILICON_LABS_32B_SERIES < 3)
#include "em_eusart.h"
#include "em_gpio.h"
#if !defined(SL_CATALOG_CLOCK_MANAGER_PRESENT)
#include "em_cmu.h"
#endif
#else
#include "sl_hal_eusart.h"
#include "sl_hal_gpio.h"
#endif

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
#include "sl_power_manager.h"
#endif

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/
// Compatibility layer for peripheral and em lib
#if (_SILICON_LABS_32B_SERIES < 3)
#define GPIO_PIN_MODE_SET       GPIO_PinModeSet
#define GPIO_MODE_PUSH_PULL     gpioModePushPull
#define GPIO_MODE_INPUT_PULL    gpioModeInputPull
#define GPIO_MODE_DISABLED      gpioModeDisabled
#define LDMA_PERIPH             LDMA
#define EUSART_UART_INIT_HF     EUSART_UartInitHf
#define EUSART_UART_INIT_LF     EUSART_UartInitLf
#define EUSART_INT_ENABLE       EUSART_IntEnable
#define EUSART_INT_DISABLE      EUSART_IntDisable
#define EUSART_INT_SET          EUSART_IntSet
#define EUSART_INT_CLEAR        EUSART_IntClear
#define EUSART_STATUS_GET       EUSART_StatusGet
#define EUSART_TX               EUSART_Tx
#define EUSART_ENABLE(eusart)   EUSART_Enable(eusart, eusartEnable)
#define EUSART_DISABLE(eusart)  EUSART_Enable(eusart, eusartDisable)
#define EUSART_HWFC_NONE        eusartHwFlowControlNone
#define EUSART_HWFC_CTS         eusartHwFlowControlCts
#define EUSART_HWFC_RTS         eusartHwFlowControlRts
#define EUSART_HWFC_CTS_RTS     eusartHwFlowControlCtsAndRts
#else
#define GPIO_PIN_MODE_SET       sl_hal_gpio_set_pin_mode
#define GPIO_MODE_PUSH_PULL     SL_HAL_GPIO_MODE_PUSH_PULL
#define GPIO_MODE_INPUT_PULL    SL_HAL_GPIO_MODE_INPUT_PULL
#define GPIO_MODE_DISABLED      SL_HAL_GPIO_MODE_DISABLED
#define LDMA_PERIPH             LDMA0
#define EUSART_UART_INIT_HF     sl_hal_eusart_init_uart_hf
#define EUSART_UART_INIT_LF     sl_hal_eusart_init_uart_lf
#define EUSART_INT_ENABLE       sl_hal_eusart_enable_interrupts
#define EUSART_INT_DISABLE      sl_hal_eusart_disable_interrupts
#define EUSART_INT_SET          sl_hal_eusart_set_interrupts
#define EUSART_INT_CLEAR        sl_hal_eusart_clear_interrupts
#define EUSART_STATUS_GET       sl_hal_eusart_get_status
#define EUSART_TX               sl_hal_eusart_tx
#define EUSART_ENABLE(eusart)        \
  {                                  \
    sl_hal_eusart_enable(eusart);    \
    sl_hal_eusart_enable_tx(eusart); \
    sl_hal_eusart_enable_rx(eusart); \
  }
#define EUSART_DISABLE          sl_hal_eusart_disable
#define EUSART_HWFC_NONE        SL_HAL_EUSART_HW_FLOW_CONTROL_NONE
#define EUSART_HWFC_CTS         SL_HAL_EUSART_HW_FLOW_CONTROL_CTS
#define EUSART_HWFC_RTS         SL_HAL_EUSART_HW_FLOW_CONTROL_RTS
#define EUSART_HWFC_CTS_RTS     SL_HAL_EUSART_HW_FLOW_CONTROL_CTS_RTS
#endif

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
static sl_power_manager_em_transition_event_handle_t on_power_manager_event_handle;

static void on_power_manager_event(sl_power_manager_em_t from,
                                   sl_power_manager_em_t to);

static sl_power_manager_em_transition_event_info_t on_power_manager_event_info =
{
  .event_mask = (SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2 | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3),
  .on_event = on_power_manager_event,
};

// List of EUSART streams
sl_slist_node_t *eusart_stream_list = NULL;
#endif

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t eusart_tx(void *context,
                             char c);

static void eusart_set_next_byte_detect(void *context);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
static void eusart_tx_completed(void *context, bool enable);
#endif

static sl_status_t eusart_deinit(void *context);

#if (_SILICON_LABS_32B_SERIES >= 3)
static inline sl_hal_eusart_parity_t iostream_to_hal_parity(sl_iostream_eusart_uart_parity_t parity)
{
  switch (parity) {
    case SL_IOSTREAM_EUSART_UART_NO_PARITY:
      return SL_HAL_EUSART_NO_PARITY;
    case SL_IOSTREAM_EUSART_UART_EVEN_PARITY:
      return SL_HAL_EUSART_NO_PARITY;
    case SL_IOSTREAM_EUSART_UART_ODD_PARITY:
      return SL_HAL_EUSART_NO_PARITY;
    default:
      // Unknown parity type
      EFM_ASSERT(false);
      return SL_HAL_EUSART_NO_PARITY;
  }
}

static inline sl_hal_eusart_stop_bits_t iostream_to_hal_stop(sl_iostream_eusart_uart_stop_bits_t stop)
{
  switch (stop) {
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_0P5:
      return SL_HAL_EUSART_STOP_BITS_0P5;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_1P5:
      return SL_HAL_EUSART_STOP_BITS_1P5;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_1:
      return SL_HAL_EUSART_STOP_BITS_1;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_2:
      return SL_HAL_EUSART_STOP_BITS_2;
    default:
      // Unknown stop bit type
      EFM_ASSERT(false);
      return SL_HAL_EUSART_STOP_BITS_1;
  }
}

static inline sl_hal_eusart_hw_flow_control_t iostream_to_hal_flow_control(sl_iostream_eusart_uart_flow_control_t fc)
{
  switch (fc) {
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_SOFT:
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_NONE:
      return SL_HAL_EUSART_HW_FLOW_CONTROL_NONE;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS:
      return SL_HAL_EUSART_HW_FLOW_CONTROL_CTS;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_RTS:
      return SL_HAL_EUSART_HW_FLOW_CONTROL_RTS;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS_RTS:
      return SL_HAL_EUSART_HW_FLOW_CONTROL_CTS_RTS;
    default:
      // Unknown flow control type
      EFM_ASSERT(false);
      return SL_HAL_EUSART_HW_FLOW_CONTROL_NONE;
  }
}

 #else
static inline EUSART_Parity_TypeDef iostream_to_hal_parity(sl_iostream_eusart_uart_parity_t parity)
{
  switch (parity) {
    case SL_IOSTREAM_EUSART_UART_NO_PARITY:
      return eusartNoParity;
    case SL_IOSTREAM_EUSART_UART_EVEN_PARITY:
      return eusartEvenParity;
    case SL_IOSTREAM_EUSART_UART_ODD_PARITY:
      return eusartOddParity;
    default:
      // Unknown parity type
      EFM_ASSERT(false);
      return eusartNoParity;
  }
}

static inline EUSART_Stopbits_TypeDef iostream_to_hal_stop(sl_iostream_eusart_uart_stop_bits_t stop)
{
  switch (stop) {
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_0P5:
      return eusartStopbits0p5;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_1P5:
      return eusartStopbits1p5;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_1:
      return eusartStopbits1;
    case SL_IOSTREAM_EUSART_UART_STOP_BITS_2:
      return eusartStopbits2;
    default:
      // Unknown stop bit type
      EFM_ASSERT(false);
      return eusartStopbits1;
  }
}

static inline EUSART_HwFlowControl_TypeDef iostream_to_hal_flow_control(sl_iostream_eusart_uart_flow_control_t fc)
{
  switch (fc) {
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_SOFT:
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_NONE:
      return eusartHwFlowControlNone;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS:
      return eusartHwFlowControlCts;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_RTS:
      return eusartHwFlowControlRts;
    case SL_IOSTREAM_EUSART_UART_FLOW_CTRL_CTS_RTS:
      return eusartHwFlowControlCtsAndRts;
    default:
      // Unknown flow control type
      EFM_ASSERT(false);
      return eusartHwFlowControlNone;
  }
}
#if !defined(SL_CATALOG_CLOCK_MANAGER_PRESENT)

#if defined(EUART_COUNT) && (EUART_COUNT > 0)
#define SL_IOSTREAM_EUSART_PERIPHERAL_NAME               EUART_COUNT
#define SL_IOSTREAM_EUSART_CLOCK_REF(periph_nbr)         SL_CONCAT_PASTER_3(cmuClock_, EUART, periph_nbr)
#else
#define SL_IOSTREAM_EUSART_PERIPHERAL_COUNT              EUSART_COUNT
#define SL_IOSTREAM_EUSART_CLOCK_REF(periph_nbr)         SL_CONCAT_PASTER_3(cmuClock_, EUSART, periph_nbr)
#endif

static inline CMU_Clock_TypeDef get_cmu_clock(uint8_t eusart_nbr)
{
  switch (eusart_nbr) {
    case 0:
      return SL_IOSTREAM_EUSART_CLOCK_REF(0);
    #if SL_IOSTREAM_EUSART_PERIPHERAL_COUNT > 1
    case 1:
      return SL_IOSTREAM_EUSART_CLOCK_REF(1);
    #if SL_IOSTREAM_EUSART_PERIPHERAL_COUNT > 2
    case 2:
      return SL_IOSTREAM_EUSART_CLOCK_REF(2);
    #if SL_IOSTREAM_EUSART_PERIPHERAL_COUNT > 3
    case 3:
      return SL_IOSTREAM_EUSART_CLOCK_REF(3);
    #if SL_IOSTREAM_EUSART_PERIPHERAL_COUNT > 4
    case 4:
      return SL_IOSTREAM_EUSART_CLOCK_REF(4);
    #endif // > 4
    #endif // > 3
    #endif // > 2
    #endif // > 1
    default:
      // Invalid EUSART peripheral number
      EFM_ASSERT(false);
      return SL_IOSTREAM_EUSART_CLOCK_REF(0);
  }
}

static inline void init_hf_clocks(sl_iostream_eusart_config_t *eusart_config)
{
  CMU_Clock_TypeDef eusart_clock = get_cmu_clock(eusart_config->eusart_nbr);
  CMU_ClockEnable(eusart_clock, true);
  // Use high frequency clock branch
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_9)
  CMU_ClockEnable(cmuClock_EM01GRPACLK, true);
  #if defined(EUART_PRESENT)
  CMU_CLOCK_SELECT_SET(EUART0CLK, EM01GRPACLK);
  #elif defined(EUSART_PRESENT)
  CMU_CLOCK_SELECT_SET(EUSART0CLK, EM01GRPACLK);
  #else
  #error Missing EU(S)ART peripheral
  #endif
  #elif (_SILICON_LABS_32B_SERIES_2_CONFIG >= 3)
  CMU_CLOCK_SELECT_SET(EM01GRPCCLK, HFRCODPLL);
  if (eusart_clock == cmuClock_EUSART0) {
    CMU_CLOCK_SELECT_SET(EUSART0CLK, EM01GRPCCLK);
  }
  #else
  #error Unsupported board configuration
  #endif
}

static inline void init_lf_clocks(sl_iostream_eusart_config_t *eusart_config)
{
  CMU_Clock_TypeDef eusart_clock = get_cmu_clock(eusart_config->eusart_nbr);
  CMU_ClockEnable(eusart_clock, true);
  // Use low frequency clock branch
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2) \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_7)  \
  || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_9)
  CMU_ClockEnable(cmuClock_EM23GRPACLK, true);
  #if defined(EUART_PRESENT)
  CMU_CLOCK_SELECT_SET(EUART0CLK, EM23GRPACLK);
  #elif defined(EUSART_PRESENT)
  CMU_CLOCK_SELECT_SET(EUSART0CLK, EM23GRPACLK);
  #else
  #error Missing EU(S)ART peripheral
  #endif
  #elif (_SILICON_LABS_32B_SERIES_2_CONFIG >= 3)
  CMU_ClockEnable(cmuClock_LFRCO, true);
  if (eusart_clock == cmuClock_EUSART0) {
    CMU_CLOCK_SELECT_SET(EUSART0CLK, LFRCO);
  }
  #else
  #error Unsupported board configuration
  #endif // _SILICON_LABS_32B_SERIES_2_CONFIG
}
#endif // SL_CATALOG_CLOCK_MANAGER_PRESENT
#endif // _SILICON_LABS_32B_SERIES < 3

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * EUSART Stream init
 ******************************************************************************/
sl_status_t sl_iostream_eusart_init(sl_iostream_uart_t *iostream_uart,
                                    sl_iostream_uart_config_t *uart_config,
                                    sl_iostream_eusart_config_t *eusart_config,
                                    sl_iostream_eusart_context_t *eusart_context)
{
  sl_status_t status;
  uint8_t em_req = 1;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_config->eusart);

  // Configure EUSART in UART
  #if (_SILICON_LABS_32B_SERIES >= 3)
  sl_hal_eusart_uart_config_t eusart_init = eusart_config->enable_high_frequency
                                            ? (sl_hal_eusart_uart_config_t)SL_HAL_EUSART_UART_INIT_DEFAULT_HF
                                            : (sl_hal_eusart_uart_config_t)SL_HAL_EUSART_UART_INIT_DEFAULT_LF;
  // Advanced Init structure
  sl_hal_eusart_uart_advanced_config_t advanced_init = SL_HAL_EUSART_UART_ADVANCED_INIT_DEFAULT;
  eusart_init.advanced_config = &advanced_init;
  // Compute clock div for baudrate
  uint32_t freq;
  sl_clock_branch_t clock_branch = sl_device_peripheral_get_clock_branch(eusart_config->eusart);
  status = sl_clock_manager_get_clock_branch_frequency(clock_branch, &freq);
  EFM_ASSERT(status == SL_STATUS_OK);

  eusart_init.clock_div = sl_hal_eusart_uart_calculate_clock_div(freq,
                                                                 eusart_config->baudrate,
                                                                 eusart_init.oversampling);
  eusart_init.parity = iostream_to_hal_parity(eusart_config->parity);
  eusart_init.stop_bits = iostream_to_hal_stop(eusart_config->stop_bits);
#if defined(_EUSART_CFG1_RXDMAWU_MASK)
  eusart_init.advanced_config->dma_wakeup_on_rx = true;
#endif
  eusart_init.advanced_config->hw_flow_control_mode = iostream_to_hal_flow_control(eusart_config->flow_control);
  #else
  EUSART_UartInit_TypeDef eusart_init;
  if (eusart_config->enable_high_frequency) {
    eusart_init = (EUSART_UartInit_TypeDef)EUSART_UART_INIT_DEFAULT_HF;

    #if !defined(SL_CATALOG_CLOCK_MANAGER_PRESENT)
    // Legacy clock initialization with em_cmu
    init_hf_clocks(eusart_config);
    #endif // SL_CATALOG_CLOCK_MANAGER_PRESENT
  } else {
    eusart_init = (EUSART_UartInit_TypeDef)EUSART_UART_INIT_DEFAULT_LF;

    #if !defined(SL_CATALOG_CLOCK_MANAGER_PRESENT)
    // Legacy clock initialization with em_cmu
    init_lf_clocks(eusart_config);
    #endif // SL_CATALOG_CLOCK_MANAGER_PRESENT
  }

  EUSART_AdvancedInit_TypeDef advanced_init = EUSART_ADVANCED_INIT_DEFAULT;
  eusart_init.baudrate = eusart_config->baudrate;
  eusart_init.parity = iostream_to_hal_parity(eusart_config->parity);
  eusart_init.stopbits = iostream_to_hal_stop(eusart_config->stop_bits);
  eusart_init.advancedSettings = &advanced_init;
  eusart_init.advancedSettings->dmaWakeUpOnRx = true;
  eusart_init.advancedSettings->hwFlowControl = iostream_to_hal_flow_control(eusart_config->flow_control);
  #endif // _SILICON_LABS_32B_SERIES >= 3

  if (eusart_config->enable_high_frequency) {
    em_req = 1;
  } else {
    em_req = 2;
  }

  status = sli_iostream_uart_context_init(iostream_uart,
                                          &eusart_context->context,
                                          uart_config,
                                          eusart_tx,
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
                                          eusart_tx_completed,
#else
                                          NULL,
#endif
                                          eusart_set_next_byte_detect,
                                          eusart_deinit,
                                          em_req,
                                          em_req);
  if (status != SL_STATUS_OK) {
    return status;
  }

  eusart_context->bus_clock = eusart_config->bus_clock;
  eusart_context->rx_pin = eusart_config->rx_pin;
  eusart_context->rx_port = eusart_config->rx_port;
  eusart_context->tx_pin = eusart_config->tx_pin;
  eusart_context->tx_port = eusart_config->tx_port;
  eusart_context->cts_pin = eusart_config->cts_pin;
  eusart_context->cts_port = eusart_config->cts_port;
  eusart_context->rts_pin = eusart_config->rts_pin;
  eusart_context->rts_port = eusart_config->rts_port;
  eusart_context->eusart = eusart_config->eusart;

  // Configure GPIO pins
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_GPIO);

  // Configure TX and RX GPIOs
#if (_SILICON_LABS_32B_SERIES >= 3)
  sl_gpio_t tx_gpio, rx_gpio;
  tx_gpio.port = eusart_config->tx_port;
  tx_gpio.pin = eusart_config->tx_pin;
  rx_gpio.port = eusart_config->rx_port;
  rx_gpio.pin = eusart_config->rx_pin;
  GPIO_PIN_MODE_SET(&tx_gpio, GPIO_MODE_PUSH_PULL, 1);
  GPIO_PIN_MODE_SET(&rx_gpio, GPIO_MODE_INPUT_PULL, 1);
#else
  GPIO_PIN_MODE_SET(eusart_config->tx_port, eusart_config->tx_pin, GPIO_MODE_PUSH_PULL, 1);
  GPIO_PIN_MODE_SET(eusart_config->rx_port, eusart_config->rx_pin, GPIO_MODE_INPUT_PULL, 1);
#endif

  // Configure EUSART for basic async operation
  eusart_context->flags = 0;

  // Configure EUSART peripheral clock
  sl_clock_manager_enable_bus_clock(eusart_config->bus_clock);

  if (eusart_config->enable_high_frequency) {
    EUSART_UART_INIT_HF(eusart_periph, &eusart_init);
    eusart_context->flags |= SLI_IOSTREAM_UART_FLAG_HIGH_FREQUENCY;
  } else {
    EUSART_UART_INIT_LF(eusart_periph, &eusart_init);
  }

  // Setup GPIO pin routing
#if defined(EUART_COUNT) && (EUART_COUNT > 0)
  GPIO->EUARTROUTE->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
  GPIO->EUARTROUTE->TXROUTE = (eusart_config->tx_port << _GPIO_EUART_TXROUTE_PORT_SHIFT)
                              | (eusart_config->tx_pin << _GPIO_EUART_TXROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE->RXROUTE = (eusart_config->rx_port << _GPIO_EUART_RXROUTE_PORT_SHIFT)
                              | (eusart_config->rx_pin << _GPIO_EUART_RXROUTE_PIN_SHIFT);
#else
  GPIO->EUSARTROUTE[eusart_config->port_index].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
  GPIO->EUSARTROUTE[eusart_config->port_index].TXROUTE =
    (eusart_config->tx_port << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
    | (eusart_config->tx_pin << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[eusart_config->port_index].RXROUTE =
    (eusart_config->rx_port << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
    | (eusart_config->rx_pin << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
#endif

  // Setup CTS/RTS
  if (eusart_config->flow_control == EUSART_HWFC_CTS
      || eusart_config->flow_control == EUSART_HWFC_CTS_RTS) {
    // Enable CTS pin
    eusart_context->flags |= SLI_IOSTREAM_UART_FLAG_CTS;
#if (_SILICON_LABS_32B_SERIES >= 3)
    sl_gpio_t cts_gpio;
    cts_gpio.port = eusart_config->cts_port;
    cts_gpio.pin = eusart_config->cts_pin;
    GPIO_PIN_MODE_SET(&cts_gpio, GPIO_MODE_INPUT_PULL, 0);
#else
    GPIO_PIN_MODE_SET(eusart_config->cts_port, eusart_config->cts_pin, GPIO_MODE_INPUT_PULL, 0);
#endif
  }

  if (eusart_config->flow_control == EUSART_HWFC_RTS
      || eusart_config->flow_control == EUSART_HWFC_CTS_RTS) {
    // Enable RTS pin
    eusart_context->flags |= SLI_IOSTREAM_UART_FLAG_RTS;
#if (_SILICON_LABS_32B_SERIES >= 3)
    sl_gpio_t rts_gpio;
    rts_gpio.port = eusart_config->rts_port;
    rts_gpio.pin = eusart_config->rts_pin;
    GPIO_PIN_MODE_SET(&rts_gpio, GPIO_MODE_PUSH_PULL, 0);
#else
    GPIO_PIN_MODE_SET(eusart_config->rts_port, eusart_config->rts_pin, GPIO_MODE_PUSH_PULL, 0);
#endif
  }

  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_CTS) {
#if defined(EUART_COUNT) && (EUART_COUNT > 0)
    GPIO->EUARTROUTE_SET->CTSROUTE = (eusart_config->cts_port << _GPIO_EUART_CTSROUTE_PORT_SHIFT)
                                     | (eusart_config->cts_pin << _GPIO_EUART_CTSROUTE_PIN_SHIFT);
#else
    GPIO->EUSARTROUTE_SET[eusart_config->port_index].CTSROUTE =
      (eusart_config->cts_port << _GPIO_EUSART_CTSROUTE_PORT_SHIFT)
      | (eusart_config->cts_pin << _GPIO_EUSART_CTSROUTE_PIN_SHIFT);
#endif
  }
  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_CTS) {
#if defined(EUART_COUNT) && (EUART_COUNT > 0)
    GPIO->EUARTROUTE_SET->ROUTEEN = GPIO_EUART_ROUTEEN_RTSPEN;
    GPIO->EUARTROUTE_SET->RTSROUTE = (eusart_config->rts_port << _GPIO_EUART_RTSROUTE_PORT_SHIFT)
                                     | (eusart_config->rts_pin << _GPIO_EUART_RTSROUTE_PIN_SHIFT);
#else
    GPIO->EUSARTROUTE_SET[eusart_config->port_index].ROUTEEN = GPIO_EUSART_ROUTEEN_RTSPEN;
    GPIO->EUSARTROUTE_SET[eusart_config->port_index].RTSROUTE =
      (eusart_config->rts_port << _GPIO_EUSART_RTSROUTE_PORT_SHIFT)
      | (eusart_config->rts_pin << _GPIO_EUSART_RTSROUTE_PIN_SHIFT);
#endif
  }

  // Clear the Interrupt Flag Register
  EUSART_INT_CLEAR(eusart_periph, EUSART_IF_RXFL);

  // Finally, enable EUSART peripheral
  EUSART_INT_ENABLE(eusart_periph, EUSART_IF_RXFL);
  EUSART_ENABLE(eusart_periph);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // Subscribe to notification to re-enable eusart after deepsleep.
  if (eusart_stream_list == NULL) {
    sl_power_manager_subscribe_em_transition_event(&on_power_manager_event_handle, &on_power_manager_event_info);
  }
  sl_slist_push(&eusart_stream_list, &eusart_context->node);
#endif

  return SL_STATUS_OK;
}
/**************************************************************************//**
 * @brief EUSART IRQ Handler
 *****************************************************************************/
void sl_iostream_eusart_irq_handler(sl_iostream_uart_t *iostream_uart)
{
  sl_iostream_eusart_context_t *eusart_context = (sl_iostream_eusart_context_t *) iostream_uart->stream.context;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);
  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  // Handle Transmit Complete Events
  if (eusart_periph->IF & EUSART_IF_TXC) {
    EUSART_INT_CLEAR(eusart_periph, EUSART_IF_TXC);
    // Check if the Status register has the TXC flag as well since the flag will clean itself
    // if other transmissions are queued contrary to the IF flag
    if ((EUSART_STATUS_GET(eusart_periph) & _EUSART_STATUS_TXC_MASK) != 0) {
      sli_uart_txc(&eusart_context->context);
    }
  }
  #endif

  // Handle RXFL events
  if (eusart_periph->IF & EUSART_IF_RXFL) {
  #if !defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Always wakeup the core up from RX IRQ
    eusart_context->context.sleep = SL_POWER_MANAGER_WAKEUP;
  #endif

    // Clear the IF
    eusart_periph->IF_CLR = EUSART_IF_RXFL;
    // Detected new byte, signal the core
    if (eusart_context->context.rx_data_available == false) {
      // Disable the IRQ until the RX Buffer is emptied, or becomes full
      EUSART_INT_DISABLE(eusart_periph, EUSART_IF_RXFL);
      sl_atomic_store(eusart_context->context.rx_data_available, true);
      #if defined(SL_CATALOG_KERNEL_PRESENT)
      // Unlock the read thread
      if (eusart_context->context.block) {
        if (osSemaphoreGetCount(eusart_context->context.read_signal) == 0) {
          osStatus_t status = osSemaphoreRelease(eusart_context->context.read_signal);
          EFM_ASSERT(status == osOK);
        }
      }
      #endif // SL_CATALOG_KERNEL_PRESENT
      return;
    }

    // Rx Buffer full, check if last byte is control character
    if (eusart_context->context.rx_buffer_full == true) {
      // Check if most recent byte is flow control (we will lose this data)
      if (eusart_context->context.sw_flow_control == true) {
        // Send XOFF to indicate RX buffer is full
        sl_atomic_store(eusart_context->context.remote_xon, false);
        EUSART_TX(eusart_periph, UARTXOFF);

        // Make sure RXFL stays enabled to avoid deadlock if both sides are full
        EUSART_INT_ENABLE(eusart_periph, EUSART_IF_RXFL);

        // Check if received byte is control char
        char dropped_byte;
        dropped_byte = (char)eusart_periph->RXDATA;

        if (dropped_byte == UARTXON) {
          sl_atomic_store(eusart_context->context.xon, true);
        } else if (dropped_byte == UARTXOFF) {
          sl_atomic_store(eusart_context->context.xon, false);
        }

        // Found most recent control character, set the scan pointer to the end of the received data
        if (dropped_byte == UARTXON || dropped_byte == UARTXOFF) {
          eusart_context->context.ctrl_char_scan_ptr = (uint8_t*)LDMA_PERIPH->CH[eusart_context->context.dma.channel].DST - 1;
        }
        // The byte is now lost...
        return;
      }
      // Can reach here if data was available and next byte detect was enabled (e.g. for sleep).
      // Disable RXFL IRQ to avoid looping in IRQ forever.
      EUSART_INT_DISABLE(eusart_periph, EUSART_IF_RXFL);
    }
  }
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
/***************************************************************************//**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t eusart_tx(void *context,
                             char c)
{
  sl_iostream_eusart_context_t *eusart_context = (sl_iostream_eusart_context_t *)context;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);
  EUSART_TX(eusart_periph, c);

#if defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
/* Wait until transmit buffer is empty */
  while (!(EUSART_STATUS_GET(eusart_periph) & EUSART_STATUS_TXC)) ;
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Enable EUSART Rx Data Valid (RXFL) Interrupt and set the RXFL IF to reflect
 * the current status of the RX FIFO.
 * Make sure the DMA is not running when calling this function.
 ******************************************************************************/
static void eusart_set_next_byte_detect(void *context)
{
  sl_iostream_eusart_context_t *eusart_context = (sl_iostream_eusart_context_t *)context;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  // Update the IF to reflect the STATUS register (IF is not updated automatically by hardware)
  if (eusart_periph->STATUS & EUSART_STATUS_RXFL) {
    EUSART_INT_SET(eusart_periph, EUSART_IF_RXFL);
  } else {
    EUSART_INT_CLEAR(eusart_periph, EUSART_IF_RXFL);
  }
  // Enable the IRQ handler
  EUSART_INT_ENABLE(eusart_periph, EUSART_IF_RXFL);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * EUSART Stream De-init.
 ******************************************************************************/
static sl_status_t eusart_deinit(void *context)
{
  sl_iostream_eusart_context_t *eusart_context = (sl_iostream_eusart_context_t *)context;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);

  // Wait until transfert is completed
  while (!(EUSART_STATUS_GET(eusart_periph) & EUSART_STATUS_TXFL)) ;

  #if (_SILICON_LABS_32B_SERIES >= 3)
  // De-Configure TX and RX GPIOs
  sl_gpio_t tx_gpio, rx_gpio, cts_gpio, rts_gpio;
  tx_gpio.port = eusart_context->tx_port;
  tx_gpio.pin = eusart_context->tx_pin;
  rx_gpio.port = eusart_context->rx_port;
  rx_gpio.pin = eusart_context->rx_pin;
  cts_gpio.port = eusart_context->cts_port;
  cts_gpio.pin = eusart_context->cts_pin;
  rts_gpio.port = eusart_context->rts_port;
  rts_gpio.pin = eusart_context->rts_pin;
  GPIO_PIN_MODE_SET(&tx_gpio, GPIO_MODE_DISABLED, 0);
  GPIO_PIN_MODE_SET(&rx_gpio, GPIO_MODE_DISABLED, 0);

  // De-Configure Flow Control GPIOs
  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_CTS) {
    GPIO_PIN_MODE_SET(&cts_gpio, GPIO_MODE_DISABLED, 0);
  }
  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_RTS) {
    GPIO_PIN_MODE_SET(&rts_gpio, GPIO_MODE_DISABLED, 0);
  }
  #else
  // De-Configure TX and RX GPIOs
  GPIO_PIN_MODE_SET(eusart_context->tx_port, eusart_context->tx_pin, GPIO_MODE_DISABLED, 0);
  GPIO_PIN_MODE_SET(eusart_context->rx_port, eusart_context->rx_pin, GPIO_MODE_DISABLED, 0);

  // De-Configure Flow Control GPIOs
  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_CTS) {
    GPIO_PIN_MODE_SET(eusart_context->cts_port, eusart_context->cts_pin, GPIO_MODE_DISABLED, 0);
  }
  if (eusart_context->flags & SLI_IOSTREAM_UART_FLAG_RTS) {
    GPIO_PIN_MODE_SET(eusart_context->rts_port, eusart_context->rts_pin, GPIO_MODE_DISABLED, 0);
  }
  #endif

  // Disable EUSART IRQ
  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
  EUSART_INT_DISABLE(eusart_periph, EUSART_IF_TXC);
  #endif
  EUSART_INT_DISABLE(eusart_periph, EUSART_IF_RXFL);

  // Disable EUSART peripheral
  EUSART_DISABLE(eusart_periph);

  // Disable EUSART Clock
  sl_clock_manager_disable_bus_clock(eusart_context->bus_clock);

  #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  // Subscribe to notification to re-enable eusart after deepsleep.
  sl_slist_remove(&eusart_stream_list, &eusart_context->node);
  if (eusart_stream_list == NULL) {
    sl_power_manager_unsubscribe_em_transition_event(&on_power_manager_event_handle);
  }
#endif

  return SL_STATUS_OK;
}

#if (defined(SL_CATALOG_POWER_MANAGER_PRESENT))
/***************************************************************************//**
 * Power Manager callback notification for EUSART.
 * It is used to re-enabled EUSART module after deepsleeping.
 ******************************************************************************/
static void on_power_manager_event(sl_power_manager_em_t from,
                                   sl_power_manager_em_t to)
{
  (void)from;

  if (to == SL_POWER_MANAGER_EM1
      || to == SL_POWER_MANAGER_EM0) {
    sl_iostream_eusart_context_t *eusart_context;

    SL_SLIST_FOR_EACH_ENTRY(eusart_stream_list, eusart_context, sl_iostream_eusart_context_t, node) {
      EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);
      EUSART_ENABLE(eusart_periph);
    }
  }
}
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_IOSTREAM_UART_FLUSH_TX_BUFFER)
/***************************************************************************//**
 * Enable/Disable EUSART Tx Complete (TXC) Interrupt
 ******************************************************************************/
static void eusart_tx_completed(void *context, bool enable)
{
  (void)context;
  (void)enable;

  sl_iostream_eusart_context_t *eusart_context = (sl_iostream_eusart_context_t *)context;
  EUSART_TypeDef* eusart_periph = sl_device_peripheral_eusart_get_base_addr(eusart_context->eusart);
  if (enable) {
    EUSART_INT_ENABLE(eusart_periph, EUSART_IF_TXC);
  } else {
    EUSART_INT_DISABLE(eusart_periph, EUSART_IF_TXC);
    EUSART_INT_CLEAR(eusart_periph, EUSART_IF_TXC);
  }
}
#endif
