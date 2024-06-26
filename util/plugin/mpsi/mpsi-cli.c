/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "mpsi-cli.h"

#ifdef EMBER_AF_API_AF_HEADER
#include EMBER_AF_API_AF_HEADER
#endif

#if defined (EMBER_STACK_ZIGBEE)
#include "app/util/serial/command-interpreter2.h"
sl_zigbee_command_entry_t sl_util_af_mpsi_commands[] = { MPSI_COMMAND_LIST };
#elif defined (EMBER_STACK_BLE)
#include COMMON_HEADER
#include "command_interpreter.h"
#else
#error "MPSI CLI: Unsupported stack!"
#endif // EMBER_STACK_ZIGBEE

#include "mpsi.h"

void sli_mpsi_cli_receive_message(CLI_HANDLER_PARAM_LIST)
{
  MpsiMessage_t mpsiMessage;
  uint8_t status;

  mpsiMessage.destinationAppId =
    (uint8_t)sli_mpsi_cli_unsigned_command_argument(0);
  mpsiMessage.messageId =
    (uint16_t)sli_mpsi_cli_unsigned_command_argument(1);
  mpsiMessage.payloadLength =
    (uint8_t)sli_mpsi_cli_unsigned_command_argument(2);
  (void)sli_mpsi_cli_copy_string_argument(3,
                                          mpsiMessage.payload,
                                          mpsiMessage.payloadLength,
                                          false);

  status = sl_util_af_mpsi_receive_message(&mpsiMessage);

  if (MPSI_SUCCESS != status) {
    mpsiPrintln("MPSI CLI: error receiving MPSI message (0x%x, 0x%2x, %d byte%s"
                "): error %d",
                mpsiMessage.destinationAppId,
                mpsiMessage.messageId,
                mpsiMessage.payloadLength,
                mpsiMessage.payloadLength == 1 ? "" : "s",
                status);
  }
}

void sli_mpsi_cli_send_message(CLI_HANDLER_PARAM_LIST)
{
  MpsiMessage_t mpsiMessage;
  uint8_t status;

  mpsiMessage.destinationAppId =
    (uint8_t)sli_mpsi_cli_unsigned_command_argument(0);
  mpsiMessage.messageId =
    (uint16_t)sli_mpsi_cli_unsigned_command_argument(1);
  mpsiMessage.payloadLength =
    (uint8_t)sli_mpsi_cli_unsigned_command_argument(2);
  (void)sli_mpsi_cli_copy_string_argument(3,
                                          mpsiMessage.payload,
                                          mpsiMessage.payloadLength,
                                          false);

  status = sl_util_af_mpsi_send_message(&mpsiMessage);

  if (MPSI_SUCCESS != status) {
    mpsiPrintln("MPSI CLI: error sending MPSI message (0x%x, 0x%2x, %d byte%s):"
                " error %d",
                mpsiMessage.destinationAppId,
                mpsiMessage.messageId,
                mpsiMessage.payloadLength,
                mpsiMessage.payloadLength == 1 ? "" : "s",
                status);
  }
}
