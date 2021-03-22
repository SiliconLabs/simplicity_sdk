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

#ifndef SLI_BT_HOST_ADAPTATION_H
#define SLI_BT_HOST_ADAPTATION_H

#include <stdint.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup sli_bt_host_adaptation
 * @{
 *
 * @brief System adaptation layer used internally by Bluetooth host stack
 *
 * The functions provided by this file are used by the Bluetooth host stack to
 * perform operations that cannot be performed using device-agnostic APIs from
 * within the Bluetooth libraries. These are typically operations that depend on
 * the target device or the Platform configuration. The application does not
 * need to and must not call these functions directly.
 *
 ******************************************************************************/

/**
 * @brief Function prototype for waking up the Bluetooth link layer task
 */
typedef void sli_bt_linklayer_wakeup_t (void);

/**
 * @brief Function prototype for waking up the Bluetooth host task
 */
typedef void sli_bt_host_wakeup_t (void);

/**
 * @brief Pointer to the function that wakes up the Bluetooth linklayer task
 */
extern sli_bt_linklayer_wakeup_t *const sli_bt_host_adaptation_linklayer_wakeup;

/**
 * @brief Pointer to the function that wakes up the Bluetooth host task
 */
extern sli_bt_host_wakeup_t *const sli_bt_host_adaptation_host_wakeup;

/**
 * @brief Initialize the device interrupts in a baremetal app
 *
 * This is called once during the boot time initialization.
 */
void sli_bt_host_adaptation_init_interrupts(void);

/**
 * @brief Get the hardware version information
 *
 * @param[out] hw_major_version Set to the HW revision major version number
 * @param[out] hw_major_version Set to the HW revision minor version number
 */
void sli_bt_host_adaptation_get_hardware_version(uint8_t *hw_major_version,
                                                 uint8_t *hw_minor_version);

/**
 * @brief Get the bootloader version information
 *
 * @param[out] bootloader_version Set to the bootloader version number if a
 *   bootloader is available
 *
 * @return SL_STATUS_OK if a bootloader is available and the version was
 *   obtained. SL_STATUS_NOT_AVAILABLE if the application does not use a
 *   bootloader.
 */
sl_status_t sli_bt_host_adaptation_get_bootloader_version(uint32_t *bootloader_version);

/**
 * @brief Reset the chip
 *
 * This function never returns.
 */
void sli_bt_host_adaptation_chip_reset(void);

/** @} end sli_bt_host_adaptation */

#ifdef __cplusplus
}
#endif

#endif // SLI_BT_HOST_ADAPTATION_H
