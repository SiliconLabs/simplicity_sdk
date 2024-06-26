/***************************************************************************//**
 * @file
 * @brief Routines to print info about the security keys on the device.
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

#ifndef SILABS_SECURITY_CLI_H
#define SILABS_SECURITY_CLI_H

void changeKeyCommand(void);
void printKeyInfo(void);

#ifndef SL_ZIGBEE_AF_HAS_SECURITY_PROFILE_NONE
#endif

extern sl_zigbee_key_data_t cliPreconfiguredLinkKey;
extern sl_zigbee_key_data_t cliNetworkKey;

#endif // SILABS_SECURITY_CLI_H
