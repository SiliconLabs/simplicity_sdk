/*******************************************************************************
 * @file zw_power_manager.c
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

#include "zpal_power_manager.h"
#include "zw_power_manager_ids.h"

uint32_t zaf_radio_timeouts[ZPAL_PM_APP_RADIO_END];
static pm_domain_status_t zpal_pm_zaf_radio_status = { .permanent_bitfield = 0, .pm_timeouts = zaf_radio_timeouts, .length = ZPAL_PM_APP_RADIO_END };

uint32_t zaf_deep_sleep_timeouts[ZPAL_PM_APP_DEEP_SLEEP_END];
static pm_domain_status_t zpal_pm_zaf_deep_sleep_status = { .permanent_bitfield = 0, .pm_timeouts = zaf_deep_sleep_timeouts, .length = ZPAL_PM_APP_DEEP_SLEEP_END };

void zw_power_manager_init(void)
{
  zpal_pm_register_domain(ZPAL_PM_DOMAIN_APP, &zpal_pm_zaf_radio_status, &zpal_pm_zaf_deep_sleep_status);
}
