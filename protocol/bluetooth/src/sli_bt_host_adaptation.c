/***************************************************************************//**
 * @brief System adaptation layer used internally by Bluetooth host stack
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sli_bt_host_adaptation.h"
#include "sl_bt_host_adaptation_config.h"
#include "sl_bt_api.h"
#include "sl_assert.h"
#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
#include "btl_interface.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_bt_rtos_adaptation.h"

// When a kernel is present, the task wakeups are handled by RTOS adaptation
sli_bt_linklayer_wakeup_t *const sli_bt_host_adaptation_linklayer_wakeup = sli_bt_rtos_ll_callback;
sli_bt_host_wakeup_t *const sli_bt_host_adaptation_host_wakeup = sli_bt_rtos_stack_callback;

#else // defined(SL_CATALOG_KERNEL_PRESENT)

// When a kernel is not present, the host task wakeup is not needed, as the
// baremetal main loop will service the host stack. The linklayer task is
// handled as a PendSV interrupt.

/**
 * Override @ref PendSV_Handler for the Link Layer task when Bluetooth runs
 * in baremetal mode. The override must not exist when Bluetooth runs in an RTOS
 * where the link layer task runs in a thread.
 */
void PendSV_Handler()
{
  sl_bt_priority_handle();
}

static void sli_bt_trigger_pendsv_interrupt(void)
{
  // Trigger the PendSV interrupt
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

sli_bt_linklayer_wakeup_t *const sli_bt_host_adaptation_linklayer_wakeup = sli_bt_trigger_pendsv_interrupt;
sli_bt_host_wakeup_t *const sli_bt_host_adaptation_host_wakeup = NULL;

#endif // defined(SL_CATALOG_KERNEL_PRESENT)

// Initialize the device interrupts in a baremetal app
void sli_bt_host_adaptation_init_interrupts(void)
{
  // The Bluetooth host stack system adaptation initializates the interrupt
  // configuration only when an RTOS kernel is not present in the application,
  // i.e. when the application is a baremetal application. When an RTOS is
  // present, the interrupts are configured by the RTOS.
#if !defined(SL_CATALOG_KERNEL_PRESENT)

  // Perform a compile-time validity check for the configuration. If BASEPRI is
  // used as the method for atomic sections, the atomic level must be higher
  // than or equal to the priority of the radio and link layer interrupts to
  // ensure proper atomic sections in the Bluetooth stack. A lower number means
  // higher priority.
#if (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
#if (CORE_ATOMIC_BASE_PRIORITY_LEVEL > SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY)
#error Bluetooth EFR32 HAL: Invalid configuration CORE_ATOMIC_BASE_PRIORITY_LEVEL > SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY
#endif
#if (CORE_ATOMIC_BASE_PRIORITY_LEVEL > SL_BT_HOST_ADAPTATION_LINKLAYER_IRQ_PRIORITY)
#error Bluetooth EFR32 HAL: Invalid configuration CORE_ATOMIC_BASE_PRIORITY_LEVEL > SL_BT_HOST_ADAPTATION_LINKLAYER_IRQ_PRIORITY
#endif
#endif // (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)

  // Set the priorities to the NVIC
  NVIC_SetPriority(FRC_PRI_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(FRC_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(MODEM_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(RAC_SEQ_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(RAC_RSM_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(BUFC_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(PROTIMER_IRQn, SL_BT_HOST_ADAPTATION_RADIO_IRQ_PRIORITY);
  NVIC_SetPriority(PendSV_IRQn, SL_BT_HOST_ADAPTATION_LINKLAYER_IRQ_PRIORITY);

  // Clear the PendSV interrupt
  NVIC_ClearPendingIRQ(PendSV_IRQn);

#endif // !defined(SL_CATALOG_KERNEL_PRESENT)
}

// Get the hardware version information
void sli_bt_host_adaptation_get_hardware_version(uint8_t *hw_major_version,
                                                 uint8_t *hw_minor_version)
{
  SYSTEM_ChipRevision_TypeDef rev;
  SYSTEM_ChipRevisionGet(&rev);
  *hw_major_version = rev.major;
  *hw_minor_version = rev.minor;
}

// Get the bootloader version information
sl_status_t sli_bt_host_adaptation_get_bootloader_version(uint32_t *bootloader_version)
{
  EFM_ASSERT(bootloader_version != NULL);

#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
  // Bootloader is available. Query the version from there.
  BootloaderInformation_t btl_info;
  memset(&btl_info, 0, sizeof(btl_info));
  bootloader_getInfo(&btl_info);
  *bootloader_version = btl_info.version;
  return SL_STATUS_OK;
#else // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT
  // Bootloader is not available
  *bootloader_version = 0;
  return SL_STATUS_NOT_AVAILABLE;
#endif // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT
}

// Reset the chip
void sli_bt_host_adaptation_chip_reset(void)
{
  CHIP_Reset();
}
