/***************************************************************************//**
 * @file: multirail-demo-cli.h
 * @brief CLI routines to utilise a second instance of RAIL.
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

// CLI should only be available in SoC apps
#ifndef SL_ZIGBEE_AF_NCP

#include "app/framework/include/af.h"

#include "multirail-demo.h"

#include "app/util/serial/sl_zigbee_command_interpreter.h"

void sl_zigbee_af_multirail_demo_cli_init(sl_cli_command_arg_t *arguments)
{
  const int16_t power = sl_cli_get_argument_uint16(arguments, 0);

  if (sl_zigbee_af_multirail_demo_init(NULL, NULL, true, power, NULL, 0, 0xFFFF, NULL)) {
    sl_zigbee_af_core_println("OK");
  } else {
    sl_zigbee_af_core_println("Additional RAIL handle not intialized");
  }
}

/** Print the second RAIL handle.
 *
 * CLI arguments:
 * none
 */
void sl_zigbee_af_multirail_demo_cli_get_handle(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("%4X", (uint32_t)sl_zigbee_af_multirail_demo_get_handle);
}

/** Send a packet using the second RAIL instance.
 *
 * CLI arguments:
 * channel (uint8_t) Channel (11-26, or 0 to use the current Zigbee channel)
 * data (string) Packet data (empty string will send a default GPD packet)
 */
void sl_zigbee_af_multirail_demo_(sl_cli_command_arg_t *arguments)
{
  uint8_t dataLength;
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t *data = sl_zigbee_cli_get_argument_string_and_length(arguments, 1, &dataLength);
  if (dataLength == 0) {
    // Use a hard-coded, properly formatted GPD packet for demo purposes
    static const uint8_t buff[] = {
      0x2C, // packet length, sizeof buff - 1 (skip the packet length byte)
      0x01, 0x08, // IEEE frame control
      0x5A, // mac seq
      0xFF, 0xFF, // dst pan
      0xFF, 0xFF, // dst addr
      0x8C, // NWK FCF
      0x98, // Added a Ext NFC direction will be filled up
      0x78, 0x56, 0x34, 0x12, // SrcID
      0x01, 0x02, 0x03, 0x04, // FC
      0x00, // Gpd Command ("Identify")
      0xDE, 0xAD, 0xBE, 0xEF, // MIC

      // GPD_PAYLOAD_BYTES
      'B', 'r', 'e', 'k', 'e', 'k', 'e', ' ',
      'Q', 'u', 'a', 'c', 'k', ' ',
      'R', 'i', 'b', 'b', 'i', 't',

      0, 0 // CRC: will be filled up
    };

    data = (uint8_t*)buff;
    dataLength = sizeof(buff);
  }

  RAIL_Status_t status = sl_zigbee_af_multirail_demo_send(data, dataLength, channel, NULL, NULL);
  if (status == RAIL_STATUS_NO_ERROR) {
    sl_zigbee_af_core_println("OK");
  } else {
    sl_zigbee_af_core_println("Error %d", status);
  }
}

#endif // SL_ZIGBEE_AF_NCP
