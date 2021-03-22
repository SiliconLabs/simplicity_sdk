/***************************************************************************//**
 * @file
 * @brief cli_power_strip.c
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
#include "CC_MultilevelSwitch_Support.h"
#include "sl_cli.h"
#include "app_log.h"
#include "ev_man.h"
#include "events.h"

#define DIMMING_TRANSITION_PERIOD_SEC 1 //Time [s] required for transition between 2 values, 0 means instant transition
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
 * CLI - toggle_endpoint: Toggle the endpoint 1 or 2
 *****************************************************************************/
void cli_toggle_endpoint(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  if (endpoint == 1) {
    app_log_info("Toggle endpoint 1\r\n");
    zaf_event_distributor_enqueue_app_event(EVENT_APP_OUTLET1_TOGGLE);
  } else if (endpoint == 2) {
    app_log_info("Toggle endpoint 2\r\n");
    zaf_event_distributor_enqueue_app_event(EVENT_APP_OUTLET2_DIMMER_SHORT_PRESS);
  } else {
    app_log_error("Invalid endpoint\r\n");
  }
}

/******************************************************************************
 * CLI - dim_endpoint: Dimm the endpoint 2
 *****************************************************************************/
void cli_dim_endpoint(sl_cli_command_arg_t *arguments)
{
  uint8_t dimming_rate_percent = sl_cli_get_argument_uint8(arguments, 0);
  if (dimming_rate_percent > 100) {
    app_log_error("Invalid dimming rate\r\n");
    return;
  }
  app_log_info("Dimming endpoint 2 to %d%%\r\n", dimming_rate_percent);
  cc_multilevel_switch_t *switches;
  switches = cc_multilevel_switch_support_config_get_switches();
  cc_multilevel_switch_set_level(&switches[0], dimming_rate_percent, DIMMING_TRANSITION_PERIOD_SEC);
}

/******************************************************************************
 * CLI - cli_toggle_notification_sending: Toggle the notification sending
 *****************************************************************************/
void cli_toggle_notification_sending(sl_cli_command_arg_t *arguments)
{
  static bool notification_sending = false;
  (void) arguments;

  if (notification_sending) {
    app_log_info("Stop sending Overload detected notification\r\n");
    notification_sending = false;
  } else {
    app_log_info("Start sending Overload detected notification\r\n");
    notification_sending = true;
  }
  zaf_event_distributor_enqueue_app_event(EVENT_APP_NOTIFICATION_TOGGLE);
}

#endif // SL_CATALOG_ZW_CLI_COMMON_PRESENT
