#ifndef HAL_CONFIG_BOARD_H
#define HAL_CONFIG_BOARD_H

#include "em_device.h"
#include "hal-config-types.h"

// This file is auto-generated by Hardware Configurator in Simplicity Studio.
// Any content between $[ and ]$ will be replaced whenever the file is regenerated.
// Content outside these regions will be preserved.

// $[ANTDIV]
// [ANTDIV]$

// $[BTL_BUTTON]

#define BSP_BTL_BUTTON_PIN                            (0U)
#define BSP_BTL_BUTTON_PORT                           (gpioPortB)

// [BTL_BUTTON]$

// $[BUTTON]
#define BSP_BUTTON_PRESENT                            (1)

#define BSP_BUTTON0_PIN                               (7U)
#define BSP_BUTTON0_PORT                              (gpioPortC)

#define BSP_BUTTON_COUNT                              (1U)
#define BSP_BUTTON_INIT                               { { BSP_BUTTON0_PORT, BSP_BUTTON0_PIN } }
#define BSP_BUTTON_GPIO_DOUT                          (HAL_GPIO_DOUT_LOW)
#define BSP_BUTTON_GPIO_MODE                          (HAL_GPIO_MODE_INPUT)
// [BUTTON]$

// $[CMU]
#define BSP_CLK_HFXO_CTUNE                            (121)
#define BSP_CLK_LFXO_CTUNE                            (42)
// [CMU]$

// $[COEX]
// [COEX]$

// $[DCDC]
// [DCDC]$

// $[EMU]
// [EMU]$

// $[EUART0]
#define PORTIO_EUART0_RX_PIN                          (6U)
#define PORTIO_EUART0_RX_PORT                         (gpioPortA)

#define PORTIO_EUART0_TX_PIN                          (5U)
#define PORTIO_EUART0_TX_PORT                         (gpioPortA)

#define BSP_EUART0_TX_PIN                             (5U)
#define BSP_EUART0_TX_PORT                            (gpioPortA)

#define BSP_EUART0_RX_PIN                             (6U)
#define BSP_EUART0_RX_PORT                            (gpioPortA)

// [EUART0]$

// $[EXTFLASH]
// [EXTFLASH]$

// $[EZRADIOPRO]
// [EZRADIOPRO]$

// $[FEM]
// [FEM]$

// $[GPIO]
#define PORTIO_GPIO_SWV_PIN                           (3U)
#define PORTIO_GPIO_SWV_PORT                          (gpioPortA)

#define BSP_TRACE_SWO_PIN                             (3U)
#define BSP_TRACE_SWO_PORT                            (gpioPortA)

// [GPIO]$

// $[I2C0]
// [I2C0]$

// $[I2C1]
#define PORTIO_I2C1_SCL_PIN                           (2U)
#define PORTIO_I2C1_SCL_PORT                          (gpioPortD)

#define PORTIO_I2C1_SDA_PIN                           (3U)
#define PORTIO_I2C1_SDA_PORT                          (gpioPortD)

#define BSP_I2C1_SCL_PIN                              (2U)
#define BSP_I2C1_SCL_PORT                             (gpioPortD)

#define BSP_I2C1_SDA_PIN                              (3U)
#define BSP_I2C1_SDA_PORT                             (gpioPortD)
// [I2C1]$

// $[I2CSENSOR]
// [I2CSENSOR]$

// $[IADC0]
// [IADC0]$

// $[IOEXP]
// [IOEXP]$

// $[LED]
#define BSP_LED_PRESENT                               (1)

#define BSP_LED0_PIN                                  (4U)
#define BSP_LED0_PORT                                 (gpioPortA)

#define BSP_LED_COUNT                                 (1U)
#define BSP_LED_INIT                                  { { BSP_LED0_PORT, BSP_LED0_PIN } }
#define BSP_LED_POLARITY                              (0)
// [LED]$

// $[LETIMER0]
// [LETIMER0]$

// $[MODEM]
// [MODEM]$

// $[PA]

#define BSP_PA_VOLTAGE                                (1800U)
// [PA]$

// $[PDM]
// [PDM]$

// $[PORTIO]
// [PORTIO]$

// $[PRS]
// [PRS]$

// $[PTI]
#define PORTIO_PTI_DFRAME_PIN                         (5U)
#define PORTIO_PTI_DFRAME_PORT                        (gpioPortC)

#define PORTIO_PTI_DOUT_PIN                           (4U)
#define PORTIO_PTI_DOUT_PORT                          (gpioPortC)

#define BSP_PTI_DFRAME_PIN                            (5U)
#define BSP_PTI_DFRAME_PORT                           (gpioPortC)

#define BSP_PTI_DOUT_PIN                              (4U)
#define BSP_PTI_DOUT_PORT                             (gpioPortC)

// [PTI]$

// $[SERIAL]
#define BSP_SERIAL_APP_PORT                           (HAL_SERIAL_PORT_USART1)
#define BSP_SERIAL_APP_TX_PIN                         (5U)
#define BSP_SERIAL_APP_TX_PORT                        (gpioPortA)

#define BSP_SERIAL_APP_RX_PIN                         (6U)
#define BSP_SERIAL_APP_RX_PORT                        (gpioPortA)

#define BSP_SERIAL_APP_CTS_PIN                        (8U)
#define BSP_SERIAL_APP_CTS_PORT                       (gpioPortA)

#define BSP_SERIAL_APP_RTS_PIN                        (7U)
#define BSP_SERIAL_APP_RTS_PORT                       (gpioPortA)

// [SERIAL]$

// $[SPIDISPLAY]
// [SPIDISPLAY]$

// $[SPINCP]
// [SPINCP]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[UARTNCP]
#define BSP_UARTNCP_USART_PORT                        (HAL_SERIAL_PORT_USART1)
#define BSP_UARTNCP_TX_PIN                            (5U)
#define BSP_UARTNCP_TX_PORT                           (gpioPortA)

#define BSP_UARTNCP_RX_PIN                            (6U)
#define BSP_UARTNCP_RX_PORT                           (gpioPortA)

#define BSP_UARTNCP_CTS_PIN                           (8U)
#define BSP_UARTNCP_CTS_PORT                          (gpioPortA)

#define BSP_UARTNCP_RTS_PIN                           (7U)
#define BSP_UARTNCP_RTS_PORT                          (gpioPortA)

// [UARTNCP]$

// $[USART0]
#define PORTIO_USART0_CLK_PIN                         (2U)
#define PORTIO_USART0_CLK_PORT                        (gpioPortC)

#define PORTIO_USART0_CS_PIN                          (3U)
#define PORTIO_USART0_CS_PORT                         (gpioPortC)

#define PORTIO_USART0_RX_PIN                          (1U)
#define PORTIO_USART0_RX_PORT                         (gpioPortC)

#define PORTIO_USART0_TX_PIN                          (0U)
#define PORTIO_USART0_TX_PORT                         (gpioPortC)

#define BSP_USART0_MOSI_PIN                           (0U)
#define BSP_USART0_MOSI_PORT                          (gpioPortC)

#define BSP_USART0_MISO_PIN                           (1U)
#define BSP_USART0_MISO_PORT                          (gpioPortC)

#define BSP_USART0_CLK_PIN                            (2U)
#define BSP_USART0_CLK_PORT                           (gpioPortC)

#define BSP_USART0_CS_PIN                             (3U)
#define BSP_USART0_CS_PORT                            (gpioPortC)

// [USART0]$

// $[USART1]
#define PORTIO_USART1_CTS_PIN                         (8U)
#define PORTIO_USART1_CTS_PORT                        (gpioPortA)

#define PORTIO_USART1_RTS_PIN                         (7U)
#define PORTIO_USART1_RTS_PORT                        (gpioPortA)

#define PORTIO_USART1_RX_PIN                          (6U)
#define PORTIO_USART1_RX_PORT                         (gpioPortA)

#define PORTIO_USART1_TX_PIN                          (5U)
#define PORTIO_USART1_TX_PORT                         (gpioPortA)

#define BSP_USART1_TX_PIN                             (5U)
#define BSP_USART1_TX_PORT                            (gpioPortA)

#define BSP_USART1_RX_PIN                             (6U)
#define BSP_USART1_RX_PORT                            (gpioPortA)

#define BSP_USART1_CTS_PIN                            (8U)
#define BSP_USART1_CTS_PORT                           (gpioPortA)

#define BSP_USART1_RTS_PIN                            (7U)
#define BSP_USART1_RTS_PORT                           (gpioPortA)

// [USART1]$

// $[VCOM]
// [VCOM]$

// $[VUART]
// [VUART]$

// $[WDOG]
// [WDOG]$

#if defined(_SILICON_LABS_MODULE)
#include "sl_module.h"
#endif

#endif /* HAL_CONFIG_BOARD_H */
