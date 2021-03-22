/***************************************************************************//**
 * @file
 * @brief cli_door_lock_keypad.c
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

#include <string.h>
#include "zaf_event_distributor_soc.h"
#include "CC_UserCode.h"
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
static void send_user_code_for_validation(char* user_code);
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
 * CLI - battery_report: Send Battery Report
 *****************************************************************************/
void cli_battery_report(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_log_info("Send Battery Report\r\n");
  zaf_event_distributor_enqueue_app_event(EVENT_APP_BATTERY_REPORT);
}

/******************************************************************************
 * CLI - enter_user_code: Enter User Code
 *****************************************************************************/
void cli_enter_user_code(sl_cli_command_arg_t *arguments)
{
  char* user_code = sl_cli_get_argument_string(arguments, 0);
  if (sizeof(user_code) < 4) {
    app_log_error("User code must be 4 digits\r\n");
    return;
  }
  app_log_info("Enter User Code: %s\r\n", user_code);
  send_user_code_for_validation(user_code);
}

/******************************************************************************
 * CLI - set_new_user_code: Set New User Code
 *****************************************************************************/
void cli_set_new_user_code(sl_cli_command_arg_t *arguments)
{
  char* user_code = sl_cli_get_argument_string(arguments, 0);
  if (sizeof(user_code) > USERCODE_MAX_LEN) {
    app_log_error("User code must be less than %d digits\r\n", USERCODE_MAX_LEN);
    return;
  }
  CC_UserCode_set_usercode(user_code);
  app_log_info("Set New User Code: %s\r\n", user_code);
}

/******************************************************************************
 * CLI - set_doorhandle_state: Set Doorhandle State
 *****************************************************************************/
void cli_set_doorhandle_state(sl_cli_command_arg_t *arguments)
{
  char* state = sl_cli_get_argument_string(arguments, 0);
  if (strcmp(state, "activate") == 0) {
    app_log_info("Activate Doorhandle\r\n");
    zaf_event_distributor_enqueue_app_event(EVENT_APP_DOORHANDLE_ACTIVATED);
  } else if (strcmp(state, "deactivate") == 0) {
    app_log_info("Deactivate Doorhandle\r\n");
    zaf_event_distributor_enqueue_app_event(EVENT_APP_DOORHANDLE_DEACTIVATED);
  } else {
    app_log_error("Invalid argument\r\n");
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static void send_user_code_for_validation(char* user_code)
{
  cc_user_code_event_validate_data_t user_code_event_validate_data;
  user_code_event_validate_data.id = 1;
  user_code_event_validate_data.data = (uint8_t *)user_code;
  user_code_event_validate_data.length = sizeof(user_code); // must be a 4 digit code

  const void *cc_data = &user_code_event_validate_data;
  zaf_event_distributor_enqueue_cc_event(COMMAND_CLASS_USER_CODE, CC_USER_CODE_EVENT_VALIDATE, cc_data);
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
