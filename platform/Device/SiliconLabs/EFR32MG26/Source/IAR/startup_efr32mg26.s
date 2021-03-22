;/**************************************************************************//**
; * @file     startup_efr32mg26.S
; * @brief    CMSIS-Core(M) Device Startup File for
; *           Device EFR32MG26
; * @version  V1.0.0
; * @date     08. April 2021
; ******************************************************************************
; * # License
; *
; * The licensor of this software is Silicon Laboratories Inc. Your use of this
; * software is governed by the terms of Silicon Labs Master Software License
; * Agreement (MSLA) available at
; * www.silabs.com/about-us/legal/master-software-license-agreement. This
; * software is Third Party Software licensed by Silicon Labs from a third party
; * and is governed by the sections of the MSLA applicable to Third Party
; * Software and the additional terms set forth below.
; *
; *****************************************************************************/
;/*
; * <b>Copyright 2009-2024 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

                MODULE   ?cstartup

                ;; Forward declaration of sections.
                SECTION  CSTACK:DATA:NOROOT(3)
                #if defined (SL_TRUSTZONE_SECURE)
                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                SECTION STACKSEAL:DATA:NOROOT(2)
                #endif // __ARM_FEATURE_CMSE
                #endif // SL_TRUSTZONE_SECURE

                SECTION  .intvec:CODE:NOROOT(9)

                EXTERN   __iar_program_start
                EXTERN   SystemInit
                PUBLIC   __vector_table
                PUBLIC   __vector_table_0x1c
                PUBLIC   __Vectors
                PUBLIC   __Vectors_End
                PUBLIC   __Vectors_Size
#define __INITIAL_SP     sfe(CSTACK)
#define __STACK_LIMIT    sfb(CSTACK)
                #if defined (SL_TRUSTZONE_SECURE)
                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define __STACK_SEAL     sfb(STACKSEAL)
                #endif // __ARM_FEATURE_CMSE
                #endif // SL_TRUSTZONE_SECURE

                DATA

__vector_table
                DCD      __INITIAL_SP                        ;     Top of Stack
                DCD      Reset_Handler                       ;     Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      MemManage_Handler                   ; -12 MPU Fault Handler
                DCD      BusFault_Handler                    ; -11 Bus Fault Handler
                DCD      UsageFault_Handler                  ; -10 Usage Fault Handler
__vector_table_0x1c
                DCD      SecureFault_Handler                 ;  -9 Security Fault Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      SVC_Handler                         ;  -5 SVC Handler
                DCD      DebugMon_Handler                    ;  -4 Debug Monitor Handler
                DCD      sl_app_properties                   ;     sl app properities Handler
                DCD      PendSV_Handler                      ;  -2 PendSV Handler
                DCD      SysTick_Handler                     ;  -1 SysTick Handler

                ; External Interrupts
                DCD      SMU_SECURE_IRQHandler   ; 0: SMU_SECURE Interrupt
                DCD      SMU_S_PRIVILEGED_IRQHandler; 1: SMU_S_PRIVILEGED Interrupt
                DCD      SMU_NS_PRIVILEGED_IRQHandler; 2: SMU_NS_PRIVILEGED Interrupt
                DCD      EMU_IRQHandler          ; 3: EMU Interrupt
                DCD      TIMER0_IRQHandler       ; 4: TIMER0 Interrupt
                DCD      TIMER1_IRQHandler       ; 5: TIMER1 Interrupt
                DCD      TIMER2_IRQHandler       ; 6: TIMER2 Interrupt
                DCD      TIMER3_IRQHandler       ; 7: TIMER3 Interrupt
                DCD      TIMER4_IRQHandler       ; 8: TIMER4 Interrupt
                DCD      TIMER5_IRQHandler       ; 9: TIMER5 Interrupt
                DCD      TIMER6_IRQHandler       ; 10: TIMER6 Interrupt
                DCD      TIMER7_IRQHandler       ; 11: TIMER7 Interrupt
                DCD      TIMER8_IRQHandler       ; 12: TIMER8 Interrupt
                DCD      TIMER9_IRQHandler       ; 13: TIMER9 Interrupt
                DCD      USART0_RX_IRQHandler    ; 14: USART0_RX Interrupt
                DCD      USART0_TX_IRQHandler    ; 15: USART0_TX Interrupt
                DCD      USART1_RX_IRQHandler    ; 16: USART1_RX Interrupt
                DCD      USART1_TX_IRQHandler    ; 17: USART1_TX Interrupt
                DCD      USART2_RX_IRQHandler    ; 18: USART2_RX Interrupt
                DCD      USART2_TX_IRQHandler    ; 19: USART2_TX Interrupt
                DCD      EUSART0_RX_IRQHandler   ; 20: EUSART0_RX Interrupt
                DCD      EUSART0_TX_IRQHandler   ; 21: EUSART0_TX Interrupt
                DCD      EUSART1_RX_IRQHandler   ; 22: EUSART1_RX Interrupt
                DCD      EUSART1_TX_IRQHandler   ; 23: EUSART1_TX Interrupt
                DCD      EUSART2_RX_IRQHandler   ; 24: EUSART2_RX Interrupt
                DCD      EUSART2_TX_IRQHandler   ; 25: EUSART2_TX Interrupt
                DCD      EUSART3_RX_IRQHandler   ; 26: EUSART3_RX Interrupt
                DCD      EUSART3_TX_IRQHandler   ; 27: EUSART3_TX Interrupt
                DCD      MVP_IRQHandler          ; 28: MVP Interrupt
                DCD      ICACHE0_IRQHandler      ; 29: ICACHE0 Interrupt
                DCD      BURTC_IRQHandler        ; 30: BURTC Interrupt
                DCD      LETIMER0_IRQHandler     ; 31: LETIMER0 Interrupt
                DCD      SYSCFG_IRQHandler       ; 32: SYSCFG Interrupt
                DCD      MPAHBRAM0_IRQHandler    ; 33: MPAHBRAM0 Interrupt
                DCD      MPAHBRAM1_IRQHandler    ; 34: MPAHBRAM1 Interrupt
                DCD      LDMA_IRQHandler         ; 35: LDMA Interrupt
                DCD      LFXO_IRQHandler         ; 36: LFXO Interrupt
                DCD      LFRCO_IRQHandler        ; 37: LFRCO Interrupt
                DCD      ULFRCO_IRQHandler       ; 38: ULFRCO Interrupt
                DCD      GPIO_ODD_IRQHandler     ; 39: GPIO_ODD Interrupt
                DCD      GPIO_EVEN_IRQHandler    ; 40: GPIO_EVEN Interrupt
                DCD      I2C0_IRQHandler         ; 41: I2C0 Interrupt
                DCD      I2C1_IRQHandler         ; 42: I2C1 Interrupt
                DCD      I2C2_IRQHandler         ; 43: I2C2 Interrupt
                DCD      I2C3_IRQHandler         ; 44: I2C3 Interrupt
                DCD      EMUDG_IRQHandler        ; 45: EMUDG Interrupt
                DCD      AGC_IRQHandler          ; 46: AGC Interrupt
                DCD      BUFC_IRQHandler         ; 47: BUFC Interrupt
                DCD      FRC_PRI_IRQHandler      ; 48: FRC_PRI Interrupt
                DCD      FRC_IRQHandler          ; 49: FRC Interrupt
                DCD      MODEM_IRQHandler        ; 50: MODEM Interrupt
                DCD      PROTIMER_IRQHandler     ; 51: PROTIMER Interrupt
                DCD      RAC_RSM_IRQHandler      ; 52: RAC_RSM Interrupt
                DCD      RAC_SEQ_IRQHandler      ; 53: RAC_SEQ Interrupt
                DCD      HOSTMAILBOX_IRQHandler  ; 54: HOSTMAILBOX Interrupt
                DCD      SYNTH_IRQHandler        ; 55: SYNTH Interrupt
                DCD      ACMP0_IRQHandler        ; 56: ACMP0 Interrupt
                DCD      ACMP1_IRQHandler        ; 57: ACMP1 Interrupt
                DCD      WDOG0_IRQHandler        ; 58: WDOG0 Interrupt
                DCD      WDOG1_IRQHandler        ; 59: WDOG1 Interrupt
                DCD      HFXO0_IRQHandler        ; 60: HFXO0 Interrupt
                DCD      HFRCO0_IRQHandler       ; 61: HFRCO0 Interrupt
                DCD      HFRCOEM23_IRQHandler    ; 62: HFRCOEM23 Interrupt
                DCD      CMU_IRQHandler          ; 63: CMU Interrupt
                DCD      AES_IRQHandler          ; 64: AES Interrupt
                DCD      IADC_IRQHandler         ; 65: IADC Interrupt
                DCD      MSC_IRQHandler          ; 66: MSC Interrupt
                DCD      DPLL0_IRQHandler        ; 67: DPLL0 Interrupt
                DCD      EMUEFP_IRQHandler       ; 68: EMUEFP Interrupt
                DCD      DCDC_IRQHandler         ; 69: DCDC Interrupt
                DCD      PCNT0_IRQHandler        ; 70: PCNT0 Interrupt
                DCD      SW0_IRQHandler          ; 71: SW0 Interrupt
                DCD      SW1_IRQHandler          ; 72: SW1 Interrupt
                DCD      SW2_IRQHandler          ; 73: SW2 Interrupt
                DCD      SW3_IRQHandler          ; 74: SW3 Interrupt
                DCD      KERNEL0_IRQHandler      ; 75: KERNEL0 Interrupt
                DCD      KERNEL1_IRQHandler      ; 76: KERNEL1 Interrupt
                DCD      M33CTI0_IRQHandler      ; 77: M33CTI0 Interrupt
                DCD      M33CTI1_IRQHandler      ; 78: M33CTI1 Interrupt
                DCD      FPUEXH_IRQHandler       ; 79: FPUEXH Interrupt
                DCD      SETAMPERHOST_IRQHandler ; 80: SETAMPERHOST Interrupt
                DCD      SEMBRX_IRQHandler       ; 81: SEMBRX Interrupt
                DCD      SEMBTX_IRQHandler       ; 82: SEMBTX Interrupt
                DCD      SYSRTC_APP_IRQHandler   ; 83: SYSRTC_APP Interrupt
                DCD      SYSRTC_SEQ_IRQHandler   ; 84: SYSRTC_SEQ Interrupt
                DCD      KEYSCAN_IRQHandler      ; 85: KEYSCAN Interrupt
                DCD      RFECA0_IRQHandler       ; 86: RFECA0 Interrupt
                DCD      RFECA1_IRQHandler       ; 87: RFECA1 Interrupt
                DCD      VDAC0_IRQHandler        ; 88: VDAC0 Interrupt
                DCD      VDAC1_IRQHandler        ; 89: VDAC1 Interrupt
                DCD      AHB2AHB0_IRQHandler     ; 90: AHB2AHB0 Interrupt
                DCD      AHB2AHB1_IRQHandler     ; 91: AHB2AHB1 Interrupt
                DCD      LCD_IRQHandler          ; 92: LCD Interrupt
                DS32    (387)  ; Interrupts 93 .. 480 are left out
__Vectors_End

__Vectors       EQU      __vector_table
__Vectors_Size  EQU      __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
                THUMB

; Reset Handler

                PUBWEAK  Reset_Handler
                SECTION  .text:CODE:REORDER:NOROOT(2)
Reset_Handler
                #if defined (SL_TRUSTZONE_SECURE)
                ldr      r0, =__STACK_LIMIT
                msr      msplim, r0

                #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
                ldr      r0, =__STACK_SEAL
                ldr      r1, =0xFEF5EDA5U
                strd     r1,r1,[r0,#0]
                #endif // __ARM_FEATURE_CMSE
                #endif // SL_TRUSTZONE_SECURE
                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__iar_program_start
                BX       R0

                PUBWEAK NMI_Handler
                PUBWEAK HardFault_Handler
                PUBWEAK MemManage_Handler
                PUBWEAK BusFault_Handler
                PUBWEAK UsageFault_Handler
                PUBWEAK SecureFault_Handler
                PUBWEAK DebugMon_Handler
                PUBWEAK SVC_Handler
                PUBWEAK sl_app_properties
                PUBWEAK PendSV_Handler
                PUBWEAK SysTick_Handler
                PUBWEAK SMU_SECURE_IRQHandler

                PUBWEAK SMU_S_PRIVILEGED_IRQHandler

                PUBWEAK SMU_NS_PRIVILEGED_IRQHandler

                PUBWEAK EMU_IRQHandler

                PUBWEAK TIMER0_IRQHandler

                PUBWEAK TIMER1_IRQHandler

                PUBWEAK TIMER2_IRQHandler

                PUBWEAK TIMER3_IRQHandler

                PUBWEAK TIMER4_IRQHandler

                PUBWEAK TIMER5_IRQHandler

                PUBWEAK TIMER6_IRQHandler

                PUBWEAK TIMER7_IRQHandler

                PUBWEAK TIMER8_IRQHandler

                PUBWEAK TIMER9_IRQHandler

                PUBWEAK USART0_RX_IRQHandler

                PUBWEAK USART0_TX_IRQHandler

                PUBWEAK USART1_RX_IRQHandler

                PUBWEAK USART1_TX_IRQHandler

                PUBWEAK USART2_RX_IRQHandler

                PUBWEAK USART2_TX_IRQHandler

                PUBWEAK EUSART0_RX_IRQHandler

                PUBWEAK EUSART0_TX_IRQHandler

                PUBWEAK EUSART1_RX_IRQHandler

                PUBWEAK EUSART1_TX_IRQHandler

                PUBWEAK EUSART2_RX_IRQHandler

                PUBWEAK EUSART2_TX_IRQHandler

                PUBWEAK EUSART3_RX_IRQHandler

                PUBWEAK EUSART3_TX_IRQHandler

                PUBWEAK MVP_IRQHandler

                PUBWEAK ICACHE0_IRQHandler

                PUBWEAK BURTC_IRQHandler

                PUBWEAK LETIMER0_IRQHandler

                PUBWEAK SYSCFG_IRQHandler

                PUBWEAK MPAHBRAM0_IRQHandler

                PUBWEAK MPAHBRAM1_IRQHandler

                PUBWEAK LDMA_IRQHandler

                PUBWEAK LFXO_IRQHandler

                PUBWEAK LFRCO_IRQHandler

                PUBWEAK ULFRCO_IRQHandler

                PUBWEAK GPIO_ODD_IRQHandler

                PUBWEAK GPIO_EVEN_IRQHandler

                PUBWEAK I2C0_IRQHandler

                PUBWEAK I2C1_IRQHandler

                PUBWEAK I2C2_IRQHandler

                PUBWEAK I2C3_IRQHandler

                PUBWEAK EMUDG_IRQHandler

                PUBWEAK AGC_IRQHandler

                PUBWEAK BUFC_IRQHandler

                PUBWEAK FRC_PRI_IRQHandler

                PUBWEAK FRC_IRQHandler

                PUBWEAK MODEM_IRQHandler

                PUBWEAK PROTIMER_IRQHandler

                PUBWEAK RAC_RSM_IRQHandler

                PUBWEAK RAC_SEQ_IRQHandler

                PUBWEAK HOSTMAILBOX_IRQHandler

                PUBWEAK SYNTH_IRQHandler

                PUBWEAK ACMP0_IRQHandler

                PUBWEAK ACMP1_IRQHandler

                PUBWEAK WDOG0_IRQHandler

                PUBWEAK WDOG1_IRQHandler

                PUBWEAK HFXO0_IRQHandler

                PUBWEAK HFRCO0_IRQHandler

                PUBWEAK HFRCOEM23_IRQHandler

                PUBWEAK CMU_IRQHandler

                PUBWEAK AES_IRQHandler

                PUBWEAK IADC_IRQHandler

                PUBWEAK MSC_IRQHandler

                PUBWEAK DPLL0_IRQHandler

                PUBWEAK EMUEFP_IRQHandler

                PUBWEAK DCDC_IRQHandler

                PUBWEAK PCNT0_IRQHandler

                PUBWEAK SW0_IRQHandler

                PUBWEAK SW1_IRQHandler

                PUBWEAK SW2_IRQHandler

                PUBWEAK SW3_IRQHandler

                PUBWEAK KERNEL0_IRQHandler

                PUBWEAK KERNEL1_IRQHandler

                PUBWEAK M33CTI0_IRQHandler

                PUBWEAK M33CTI1_IRQHandler

                PUBWEAK FPUEXH_IRQHandler

                PUBWEAK SETAMPERHOST_IRQHandler

                PUBWEAK SEMBRX_IRQHandler

                PUBWEAK SEMBTX_IRQHandler

                PUBWEAK SYSRTC_APP_IRQHandler

                PUBWEAK SYSRTC_SEQ_IRQHandler

                PUBWEAK KEYSCAN_IRQHandler

                PUBWEAK RFECA0_IRQHandler

                PUBWEAK RFECA1_IRQHandler

                PUBWEAK VDAC0_IRQHandler

                PUBWEAK VDAC1_IRQHandler

                PUBWEAK AHB2AHB0_IRQHandler

                PUBWEAK AHB2AHB1_IRQHandler

                PUBWEAK LCD_IRQHandler

                SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SecureFault_Handler
DebugMon_Handler
SVC_Handler
sl_app_properties
PendSV_Handler
SysTick_Handler
SMU_SECURE_IRQHandler

SMU_S_PRIVILEGED_IRQHandler

SMU_NS_PRIVILEGED_IRQHandler

EMU_IRQHandler

TIMER0_IRQHandler

TIMER1_IRQHandler

TIMER2_IRQHandler

TIMER3_IRQHandler

TIMER4_IRQHandler

TIMER5_IRQHandler

TIMER6_IRQHandler

TIMER7_IRQHandler

TIMER8_IRQHandler

TIMER9_IRQHandler

USART0_RX_IRQHandler

USART0_TX_IRQHandler

USART1_RX_IRQHandler

USART1_TX_IRQHandler

USART2_RX_IRQHandler

USART2_TX_IRQHandler

EUSART0_RX_IRQHandler

EUSART0_TX_IRQHandler

EUSART1_RX_IRQHandler

EUSART1_TX_IRQHandler

EUSART2_RX_IRQHandler

EUSART2_TX_IRQHandler

EUSART3_RX_IRQHandler

EUSART3_TX_IRQHandler

MVP_IRQHandler

ICACHE0_IRQHandler

BURTC_IRQHandler

LETIMER0_IRQHandler

SYSCFG_IRQHandler

MPAHBRAM0_IRQHandler

MPAHBRAM1_IRQHandler

LDMA_IRQHandler

LFXO_IRQHandler

LFRCO_IRQHandler

ULFRCO_IRQHandler

GPIO_ODD_IRQHandler

GPIO_EVEN_IRQHandler

I2C0_IRQHandler

I2C1_IRQHandler

I2C2_IRQHandler

I2C3_IRQHandler

EMUDG_IRQHandler

AGC_IRQHandler

BUFC_IRQHandler

FRC_PRI_IRQHandler

FRC_IRQHandler

MODEM_IRQHandler

PROTIMER_IRQHandler

RAC_RSM_IRQHandler

RAC_SEQ_IRQHandler

HOSTMAILBOX_IRQHandler

SYNTH_IRQHandler

ACMP0_IRQHandler

ACMP1_IRQHandler

WDOG0_IRQHandler

WDOG1_IRQHandler

HFXO0_IRQHandler

HFRCO0_IRQHandler

HFRCOEM23_IRQHandler

CMU_IRQHandler

AES_IRQHandler

IADC_IRQHandler

MSC_IRQHandler

DPLL0_IRQHandler

EMUEFP_IRQHandler

DCDC_IRQHandler

PCNT0_IRQHandler

SW0_IRQHandler

SW1_IRQHandler

SW2_IRQHandler

SW3_IRQHandler

KERNEL0_IRQHandler

KERNEL1_IRQHandler

M33CTI0_IRQHandler

M33CTI1_IRQHandler

FPUEXH_IRQHandler

SETAMPERHOST_IRQHandler

SEMBRX_IRQHandler

SEMBTX_IRQHandler

SYSRTC_APP_IRQHandler

SYSRTC_SEQ_IRQHandler

KEYSCAN_IRQHandler

RFECA0_IRQHandler

RFECA1_IRQHandler

VDAC0_IRQHandler

VDAC1_IRQHandler

AHB2AHB0_IRQHandler

AHB2AHB1_IRQHandler

LCD_IRQHandler

Default_Handler
                B        .

                END
