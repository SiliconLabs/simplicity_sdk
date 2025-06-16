/**************************************************************************//**
 * @file
 * @brief Device Manager Peripheral Definition
 ******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#include "em_device.h"
#include "sl_device_peripheral.h"
#include "sl_device_clock.h"

/***************************************************************************//**
 * @addtogroup device_peripheral Device Abstraction Peripheral
 * @{
 ******************************************************************************/

#if defined(ACMP0_BASE)
// Define peripheral ACMP0.
const sl_peripheral_val_t sl_peripheral_val_acmp0 = { .base = ACMP0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_ACMP0 };
#endif

#if defined(ACMP1_BASE)
// Define peripheral ACMP1.
const sl_peripheral_val_t sl_peripheral_val_acmp1 = { .base = ACMP1_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_ACMP1 };
#endif

#if defined(AMUXCP0_BASE)
// Define peripheral AMUXCP0.
const sl_peripheral_val_t sl_peripheral_val_amuxcp0 = { .base = AMUXCP0_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                        .bus_clock = SL_BUS_CLOCK_AMUXCP0 };
#endif

#if defined(BURAM_BASE)
// Define peripheral BURAM.
const sl_peripheral_val_t sl_peripheral_val_buram = { .base = BURAM_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_BURAM };
#endif

#if defined(BURTC_BASE)
// Define peripheral BURTC.
const sl_peripheral_val_t sl_peripheral_val_burtc = { .base = BURTC_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_EM4GRPACLK,
                                                      .bus_clock = SL_BUS_CLOCK_BURTC };
#endif

#if defined(CMU_BASE)
// Define peripheral CMU.
const sl_peripheral_val_t sl_peripheral_val_cmu = { .base = CMU_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                    .bus_clock = SL_BUS_CLOCK_INVALID };
#endif

#if defined(DCDC_BASE)
// Define peripheral DCDC.
const sl_peripheral_val_t sl_peripheral_val_dcdc = { .base = DCDC_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                     .bus_clock = SL_BUS_CLOCK_DCDC };
#endif

#if defined(DMEM0_BASE)
// Define peripheral DMEM0.
const sl_peripheral_val_t sl_peripheral_val_dmem0 = { .base = DMEM0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                      .bus_clock = SL_BUS_CLOCK_DMEM };
#endif

#if defined(DMEM1_BASE)
// Define peripheral DMEM1.
const sl_peripheral_val_t sl_peripheral_val_dmem1 = { .base = DMEM1_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                      .bus_clock = SL_BUS_CLOCK_DMEM };
#endif

#if defined(DPLL0_BASE)
// Define peripheral DPLL0.
const sl_peripheral_val_t sl_peripheral_val_dpll0 = { .base = DPLL0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_DPLL0 };
#endif

#if defined(EMU_BASE)
// Define peripheral EMU.
const sl_peripheral_val_t sl_peripheral_val_emu = { .base = EMU_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                    .bus_clock = SL_BUS_CLOCK_INVALID };
#endif

#if defined(EUSART0_BASE)
// Define peripheral EUSART0.
const sl_peripheral_val_t sl_peripheral_val_eusart0 = { .base = EUSART0_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_EUSART0CLK,
                                                        .bus_clock = SL_BUS_CLOCK_EUSART0 };
#endif

#if defined(EUSART1_BASE)
// Define peripheral EUSART1.
const sl_peripheral_val_t sl_peripheral_val_eusart1 = { .base = EUSART1_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_EM01GRPCCLK,
                                                        .bus_clock = SL_BUS_CLOCK_EUSART1 };
#endif

#if defined(EUSART2_BASE)
// Define peripheral EUSART2.
const sl_peripheral_val_t sl_peripheral_val_eusart2 = { .base = EUSART2_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_EM01GRPCCLK,
                                                        .bus_clock = SL_BUS_CLOCK_EUSART2 };
#endif

#if defined(EUSART3_BASE)
// Define peripheral EUSART3.
const sl_peripheral_val_t sl_peripheral_val_eusart3 = { .base = EUSART3_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_EM01GRPCCLK,
                                                        .bus_clock = SL_BUS_CLOCK_EUSART3 };
#endif

#if defined(FSRCO_BASE)
// Define peripheral FSRCO.
const sl_peripheral_val_t sl_peripheral_val_fsrco = { .base = FSRCO_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_FSRCO };
#endif

#if defined(GPCRC_BASE)
// Define peripheral GPCRC0.
const sl_peripheral_val_t sl_peripheral_val_gpcrc0 = { .base = GPCRC_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                       .bus_clock = SL_BUS_CLOCK_GPCRC0 };
#endif

#if defined(GPIO_BASE)
// Define peripheral GPIO.
const sl_peripheral_val_t sl_peripheral_val_gpio = { .base = GPIO_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                     .bus_clock = SL_BUS_CLOCK_GPIO };
#endif

#if defined(HFRCO0_BASE)
// Define peripheral HFRCO0.
const sl_peripheral_val_t sl_peripheral_val_hfrco0 = { .base = HFRCO0_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                       .bus_clock = SL_BUS_CLOCK_HFRCO0 };
#endif

#if defined(HFRCOEM23_BASE)
// Define peripheral HFRCOEM23.
const sl_peripheral_val_t sl_peripheral_val_hfrcoem23 = { .base = HFRCOEM23_BASE,
                                                          .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                          .bus_clock = SL_BUS_CLOCK_HFRCOEM23 };
#endif

#if defined(HFXO0_BASE)
// Define peripheral HFXO0.
const sl_peripheral_val_t sl_peripheral_val_hfxo0 = { .base = HFXO0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_HFXO0 };
#endif

#if defined(HOSTMAILBOX_BASE)
// Define peripheral HOSTMAILBOX.
const sl_peripheral_val_t sl_peripheral_val_hostmailbox = { .base = HOSTMAILBOX_BASE,
                                                            .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                            .bus_clock = SL_BUS_CLOCK_HOSTMAILBOX };
#endif

#if defined(I2C0_BASE)
// Define peripheral I2C0.
const sl_peripheral_val_t sl_peripheral_val_i2c0 = { .base = I2C0_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_LSPCLK,
                                                     .bus_clock = SL_BUS_CLOCK_I2C0 };
#endif

#if defined(I2C1_BASE)
// Define peripheral I2C1.
const sl_peripheral_val_t sl_peripheral_val_i2c1 = { .base = I2C1_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                     .bus_clock = SL_BUS_CLOCK_I2C1 };
#endif

#if defined(I2C2_BASE)
// Define peripheral I2C2.
const sl_peripheral_val_t sl_peripheral_val_i2c2 = { .base = I2C2_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                     .bus_clock = SL_BUS_CLOCK_I2C2 };
#endif

#if defined(I2C3_BASE)
// Define peripheral I2C3.
const sl_peripheral_val_t sl_peripheral_val_i2c3 = { .base = I2C3_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                     .bus_clock = SL_BUS_CLOCK_I2C3 };
#endif

#if defined(IADC0_BASE)
// Define peripheral IADC0.
const sl_peripheral_val_t sl_peripheral_val_iadc0 = { .base = IADC0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_IADCCLK,
                                                      .bus_clock = SL_BUS_CLOCK_IADC0 };
#endif

#if defined(ICACHE0_BASE)
// Define peripheral ICACHE0.
const sl_peripheral_val_t sl_peripheral_val_icache0 = { .base = ICACHE0_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                        .bus_clock = SL_BUS_CLOCK_ICACHE0 };
#endif

#if defined(KEYSCAN_BASE)
// Define peripheral KEYSCAN.
const sl_peripheral_val_t sl_peripheral_val_keyscan = { .base = KEYSCAN_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                        .bus_clock = SL_BUS_CLOCK_KEYSCAN };
#endif

#if defined(LCD_BASE)
// Define peripheral LCD.
const sl_peripheral_val_t sl_peripheral_val_lcd = { .base = LCD_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_LCDCLK,
                                                    .bus_clock = SL_BUS_CLOCK_LCD };
#endif

#if defined(LCDRF_BASE)
// Define peripheral LCDRF.
const sl_peripheral_val_t sl_peripheral_val_lcdrf = { .base = LCDRF_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_INVALID };
#endif

#if defined(LDMA_BASE)
// Define peripheral LDMA0.
const sl_peripheral_val_t sl_peripheral_val_ldma0 = { .base = LDMA_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_HCLK,
                                                      .bus_clock = SL_BUS_CLOCK_LDMA0 };
#endif

#if defined(LDMAXBAR_BASE)
// Define peripheral LDMAXBAR0.
const sl_peripheral_val_t sl_peripheral_val_ldmaxbar0 = { .base = LDMAXBAR_BASE,
                                                          .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                          .bus_clock = SL_BUS_CLOCK_LDMAXBAR0 };
#endif

#if defined(LETIMER0_BASE)
// Define peripheral LETIMER0.
const sl_peripheral_val_t sl_peripheral_val_letimer0 = { .base = LETIMER0_BASE,
                                                         .clk_branch = SL_CLOCK_BRANCH_EM23GRPACLK,
                                                         .bus_clock = SL_BUS_CLOCK_LETIMER0 };
#endif

#if defined(LFRCO_BASE)
// Define peripheral LFRCO.
const sl_peripheral_val_t sl_peripheral_val_lfrco = { .base = LFRCO_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                      .bus_clock = SL_BUS_CLOCK_LFRCO };
#endif

#if defined(LFXO_BASE)
// Define peripheral LFXO.
const sl_peripheral_val_t sl_peripheral_val_lfxo = { .base = LFXO_BASE,
                                                     .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                     .bus_clock = SL_BUS_CLOCK_LFXO };
#endif

#if defined(MSC_BASE)
// Define peripheral MSC.
const sl_peripheral_val_t sl_peripheral_val_msc = { .base = MSC_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_HCLK,
                                                    .bus_clock = SL_BUS_CLOCK_MSC };
#endif

#if defined(MVP_BASE)
// Define peripheral MVP.
const sl_peripheral_val_t sl_peripheral_val_mvp = { .base = MVP_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_HCLK,
                                                    .bus_clock = SL_BUS_CLOCK_MVP };
#endif

#if defined(PCNT0_BASE)
// Define peripheral PCNT0.
const sl_peripheral_val_t sl_peripheral_val_pcnt0 = { .base = PCNT0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_PCNT0CLK,
                                                      .bus_clock = SL_BUS_CLOCK_PCNT0 };
#endif

#if defined(PRS_BASE)
// Define peripheral PRS.
const sl_peripheral_val_t sl_peripheral_val_prs = { .base = PRS_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                    .bus_clock = SL_BUS_CLOCK_PRS };
#endif

#if defined(RADIOAES_BASE)
// Define peripheral RADIOAES.
const sl_peripheral_val_t sl_peripheral_val_radioaes = { .base = RADIOAES_BASE,
                                                         .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                         .bus_clock = SL_BUS_CLOCK_RADIOAES };
#endif

#if defined(SEMAILBOX_HOST_BASE)
// Define peripheral SEMAILBOX.
const sl_peripheral_val_t sl_peripheral_val_semailbox = { .base = SEMAILBOX_HOST_BASE,
                                                          .clk_branch = SL_CLOCK_BRANCH_HCLK,
                                                          .bus_clock = SL_BUS_CLOCK_SEMAILBOX };
#endif

#if defined(SMU_BASE)
// Define peripheral SMU.
const sl_peripheral_val_t sl_peripheral_val_smu = { .base = SMU_BASE,
                                                    .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                    .bus_clock = SL_BUS_CLOCK_SMU };
#endif

#if defined(SYSCFG_BASE)
// Define peripheral SYSCFG.
const sl_peripheral_val_t sl_peripheral_val_syscfg = { .base = SYSCFG_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                       .bus_clock = SL_BUS_CLOCK_SYSCFG };
#endif

#if defined(SYSRTC0_BASE)
// Define peripheral SYSRTC0.
const sl_peripheral_val_t sl_peripheral_val_sysrtc0 = { .base = SYSRTC0_BASE,
                                                        .clk_branch = SL_CLOCK_BRANCH_SYSRTCCLK,
                                                        .bus_clock = SL_BUS_CLOCK_SYSRTC0 };
#endif

#if defined(TIMER0_BASE)
// Define peripheral TIMER0.
const sl_peripheral_val_t sl_peripheral_val_timer0 = { .base = TIMER0_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER0 };
#endif

#if defined(TIMER1_BASE)
// Define peripheral TIMER1.
const sl_peripheral_val_t sl_peripheral_val_timer1 = { .base = TIMER1_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER1 };
#endif

#if defined(TIMER2_BASE)
// Define peripheral TIMER2.
const sl_peripheral_val_t sl_peripheral_val_timer2 = { .base = TIMER2_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER2 };
#endif

#if defined(TIMER3_BASE)
// Define peripheral TIMER3.
const sl_peripheral_val_t sl_peripheral_val_timer3 = { .base = TIMER3_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER3 };
#endif

#if defined(TIMER4_BASE)
// Define peripheral TIMER4.
const sl_peripheral_val_t sl_peripheral_val_timer4 = { .base = TIMER4_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER4 };
#endif

#if defined(TIMER5_BASE)
// Define peripheral TIMER5.
const sl_peripheral_val_t sl_peripheral_val_timer5 = { .base = TIMER5_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER5 };
#endif

#if defined(TIMER6_BASE)
// Define peripheral TIMER6.
const sl_peripheral_val_t sl_peripheral_val_timer6 = { .base = TIMER6_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER6 };
#endif

#if defined(TIMER7_BASE)
// Define peripheral TIMER7.
const sl_peripheral_val_t sl_peripheral_val_timer7 = { .base = TIMER7_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER7 };
#endif

#if defined(TIMER8_BASE)
// Define peripheral TIMER8.
const sl_peripheral_val_t sl_peripheral_val_timer8 = { .base = TIMER8_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER8 };
#endif

#if defined(TIMER9_BASE)
// Define peripheral TIMER9.
const sl_peripheral_val_t sl_peripheral_val_timer9 = { .base = TIMER9_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_EM01GRPACLK,
                                                       .bus_clock = SL_BUS_CLOCK_TIMER9 };
#endif

#if defined(ULFRCO_BASE)
// Define peripheral ULFRCO.
const sl_peripheral_val_t sl_peripheral_val_ulfrco = { .base = ULFRCO_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                       .bus_clock = SL_BUS_CLOCK_ULFRCO };
#endif

#if defined(USART0_BASE)
// Define peripheral USART0.
const sl_peripheral_val_t sl_peripheral_val_usart0 = { .base = USART0_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_PCLK,
                                                       .bus_clock = SL_BUS_CLOCK_USART0 };
#endif

#if defined(USART1_BASE)
// Define peripheral USART1.
const sl_peripheral_val_t sl_peripheral_val_usart1 = { .base = USART1_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                       .bus_clock = SL_BUS_CLOCK_USART1 };
#endif

#if defined(USART2_BASE)
// Define peripheral USART2.
const sl_peripheral_val_t sl_peripheral_val_usart2 = { .base = USART2_BASE,
                                                       .clk_branch = SL_CLOCK_BRANCH_INVALID,
                                                       .bus_clock = SL_BUS_CLOCK_USART2 };
#endif

#if defined(VDAC0_BASE)
// Define peripheral VDAC0.
const sl_peripheral_val_t sl_peripheral_val_vdac0 = { .base = VDAC0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_VDAC0CLK,
                                                      .bus_clock = SL_BUS_CLOCK_VDAC0 };
#endif

#if defined(VDAC1_BASE)
// Define peripheral VDAC1.
const sl_peripheral_val_t sl_peripheral_val_vdac1 = { .base = VDAC1_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_VDAC1CLK,
                                                      .bus_clock = SL_BUS_CLOCK_VDAC1 };
#endif

#if defined(WDOG0_BASE)
// Define peripheral WDOG0.
const sl_peripheral_val_t sl_peripheral_val_wdog0 = { .base = WDOG0_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_WDOG0CLK,
                                                      .bus_clock = SL_BUS_CLOCK_WDOG0 };
#endif

#if defined(WDOG1_BASE)
// Define peripheral WDOG1.
const sl_peripheral_val_t sl_peripheral_val_wdog1 = { .base = WDOG1_BASE,
                                                      .clk_branch = SL_CLOCK_BRANCH_WDOG1CLK,
                                                      .bus_clock = SL_BUS_CLOCK_WDOG1 };
#endif

/** @} (end addtogroup device_peripheral) */
