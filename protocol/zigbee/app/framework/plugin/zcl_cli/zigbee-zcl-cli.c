/***************************************************************************//**
 * @file
 * @brief Adding ZCL CLI commands to the CLI.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "sl_cli_types.h"
#include "sl_cli_command.h"

extern sl_cli_command_group_t sl_cli_zcl_command_group;
extern const sl_cli_handle_t sl_cli_handles[];
extern const uint32_t sl_cli_handles_count;

// Api to add the zcl cli to the application.
void sli_zigbee_zcl_cli_init(uint8_t init_level)
{
  (void)init_level;

  // Add ZCL cli command handling for all cli instance handles
  for (uint8_t i = 0; i < sl_cli_handles_count; i++) {
    sl_cli_command_add_command_group(sl_cli_handles[i], &sl_cli_zcl_command_group);
  }
}
