/***************************************************************************//**
 * @file
 * @brief cli_switch_on_off.c
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
#include "sl_cli.h"
#include "app_log.h"
#include "ev_man.h"
#include "events.h"

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
 * CLI - toggle_led: Toggle the LED0 on the expansion header
 *****************************************************************************/
void cli_toggle_led(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_log_info("Toggle the LED0 on the expansion header\r\n");
  zaf_event_distributor_enqueue_app_event(EVENT_APP_TOGGLE_LED);
}

/******************************************************************************
 * CLI - send_nif: Send Node Information Frame
 *****************************************************************************/
void cli_send_nif(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  app_log_info("Send Node Information Frame\r\n");
  zaf_event_distributor_enqueue_app_event(EVENT_APP_SEND_NIF);
}

#endif // SL_CATALOG_ZW_CLI_COMMON_PRESENT
