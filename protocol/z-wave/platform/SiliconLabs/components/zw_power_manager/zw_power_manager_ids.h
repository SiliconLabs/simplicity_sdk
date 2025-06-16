/*******************************************************************************
 * @file zw_power_manager_ids.h
 * @brief This file contains application specific power manager IDs and abstraction
 *******************************************************************************
 * # License
 * <b> Copyright 2025 Silicon Laboratories Inc. www.silabs.com </b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of the Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 * By installing, copying or otherwise using this software, you agree to the
 * terms of the MSLA.
 *
 ******************************************************************************/

#ifndef ZW_POWER_MANAGER_H
#define ZW_POWER_MANAGER_H

/******************************************************************************
*              APPLICATION MODULE IDs - MODIFY WITH CAUTION
******************************************************************************/
#include "zpal_power_manager.h"

// @note to prevent undefined behavior `END` of enum needs to be not NULL (used for array size)

/* Z-Wave Application Power Manager IDs */
typedef enum {
  ZPAL_PM_APP_RADIO_APPLICATION_ID,
  ZPAL_PM_APP_RADIO_ZAF_CC_WAKEUP_ID,
  ZPAL_PM_APP_RADIO_ZAF_CC_USER_CREDENTIAL_ID,
  ZPAL_PM_APP_RADIO_ZAF_CC_OTA_ID,
  ZPAL_PM_APP_RADIO_ZAF_TRANSPORT_MULTICAST_ID,
  ZPAL_PM_APP_RADIO_ZAF_COMMON_HELPER_ID,
  ZPAL_PM_APP_RADIO_CLI_ID,
  ZPAL_PM_APP_RADIO_BUTTON_ID,
  /* Do not modify above this point */

  // Add new Lock IDs here

  /* Do not modify below this point */
  ZPAL_PM_APP_RADIO_END,
} ZPAL_PM_AppRadioID;

typedef enum {
  ZPAL_PM_APP_DEEP_SLEEP_APPLICATION_ID,
  ZPAL_PM_APP_DEEP_SLEEP_BOARD_INDICATOR_ID,
  /* Do not modify above this point */

  // Add new Lock IDs here

  /* Do not modify below this point */
  ZPAL_PM_APP_DEEP_SLEEP_END,
} ZPAL_PM_AppDeepSleepID;

// Wrapper functions for the application domain
#define zw_power_manager_lock(type, timeout, id) zpal_pm_lock(type, ZPAL_PM_DOMAIN_APP, timeout, id)
#define zw_power_manager_relock(type, timeout, id) zpal_pm_relock(type, ZPAL_PM_DOMAIN_APP, timeout, id)
#define zw_power_manager_get_state(type, id, state) zpal_pm_lock_is_active(type, ZPAL_PM_DOMAIN_APP, id, state)
#define zw_power_manager_lock_cancel(type, id) zpal_pm_lock_cancel(type, ZPAL_PM_DOMAIN_APP, id)

void zw_power_manager_init(void);

#endif // ZW_POWER_MANAGER_H
