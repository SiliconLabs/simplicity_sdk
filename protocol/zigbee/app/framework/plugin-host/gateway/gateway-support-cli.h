/***************************************************************************//**
 * @file
 * @brief Gateway specific behavior for a host application.
 *  In this case we assume our application is running on
 *   a PC with Unix library support, connected to an NCP via serial uart.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

extern sl_zigbee_command_entry_t sl_zigbee_af_gateway_commands[];

#if defined(GATEWAY_APP)
  #define SL_ZIGBEE_AF_PLUGIN_GATEWAY_COMMANDS                    \
  sl_zigbee_command_entry_sub_menu("gateway",                     \
                                   sl_zigbee_af_gateway_commands, \
                                   "Commands for the Linux host application"),

#else
  #define SL_ZIGBEE_AF_PLUGIN_GATEWAY_COMMANDS

#endif
