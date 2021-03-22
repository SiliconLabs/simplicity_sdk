/***************************************************************************//**
 * @file
 * @brief cli_multilevel_sensor.c
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_component_catalog.h"

#ifdef SL_CATALOG_ZW_CLI_COMMON_PRESENT

#include "zaf_event_distributor_soc.h"
#include <stdint.h>
#include "sl_cli.h"
#include "app_log.h"
#include "app_cli.h"
#include "ev_man.h"
#include "events.h"
#include "zpal_power_manager.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * CLI - send_battery_and_sensor_report: Send battery and sensor report
 *****************************************************************************/
void cli_send_battery_and_sensor_report(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_log_info("Sending battery and sensor report\r\n");
  zaf_event_distributor_enqueue_app_event(EVENT_APP_SEND_BATTERY_LEVEL_AND_SENSOR_REPORT);
}

/******************************************************************************
 * CLI - enable_sleeping: Enabling the device to go into sleep mode
 *****************************************************************************/
void cli_enable_sleeping(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_log_info("Enable sleeping\r\n");
  cli_util_prevent_sleeping(false);
}

/******************************************************************************
 * CLI - Util Preventing the application to go into sleep mode to keep
 * the CLI alive
 *****************************************************************************/
void cli_util_prevent_sleeping(bool is_prevent)
{
  static zpal_pm_handle_t pm_handle = NULL;

  if ((true == is_prevent) && (pm_handle == NULL)) {
    pm_handle  = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);
    zpal_pm_stay_awake(pm_handle, 0);
  } else {
    zpal_pm_cancel(pm_handle);
    pm_handle = NULL;
  }
}

#endif // SL_CATALOG_ZW_CLI_COMMON_PRESENT
