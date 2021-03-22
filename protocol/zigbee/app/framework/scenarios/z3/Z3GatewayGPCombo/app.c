/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"

#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "stack/include/trust-center.h"
#include "zap-cluster-command-parser.h"
#include <stdlib.h>
#include "xncp-sample-custom-ezsp-protocol.h"
#include "stack/include/zigbee-security-manager.h"

// The number of tokens that can be written using sl_zigbee_ezsp_set_token and read using
// sl_zigbee_ezsp_get_token.
#define MFGSAMP_NUM_EZSP_TOKENS 8
// The size of the tokens that can be written using sl_zigbee_ezsp_set_token and read using
// sl_zigbee_ezsp_get_token.
#define MFGSAMP_EZSP_TOKEN_SIZE 8
// The number of manufacturing tokens.
#define MFGSAMP_NUM_EZSP_MFG_TOKENS 11
// The size of the largest EZSP Mfg token, SL_ZIGBEE_EZSP_MFG_CBKE_DATA. Please refer to
// app/util/ezsp/ezsp-enum.h.
#define MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE 92

#define GREEN_POWER_MAX_FRAME_LENGTH 70
#define GREEN_POWER_COMMAND_INDEX 0

extern sl_status_t sl_zigbee_af_trust_center_start_network_key_update(void);

//----------------------
// ZCL commands handling

static void ias_ace_cluster_arm_command_handler(uint8_t armMode,
                                                uint8_t* armDisarmCode,
                                                uint8_t zoneId)
{
  uint16_t armDisarmCodeLength = sl_zigbee_af_string_length(armDisarmCode);
  sl_802154_short_addr_t sender = sli_zigbee_af_get_sender();
  uint16_t i;

  sl_zigbee_app_debug_print("IAS ACE Arm Received %04X", armMode);

  // Start i at 1 to skip over leading character in the byte array as it is the
  // length byte
  for (i = 1; i < armDisarmCodeLength; i++) {
    sl_zigbee_app_debug_print("%c", armDisarmCode[i]);
  }
  sl_zigbee_app_debug_println(" %02X", zoneId);

  sl_zigbee_af_fill_command_ias_ace_cluster_arm_response(armMode);
  sl_zigbee_af_send_command_unicast(SL_ZIGBEE_OUTGOING_DIRECT, sender);
}

static void ias_ace_cluster_bypass_command_handler(uint8_t numberOfZones,
                                                   uint8_t* zoneIds,
                                                   uint8_t* armDisarmCode)
{
  sl_802154_short_addr_t sender = sli_zigbee_af_get_sender();
  uint8_t i;

  sl_zigbee_app_debug_print("IAS ACE Cluster Bypass for zones ");

  for (i = 0; i < numberOfZones; i++) {
    sl_zigbee_app_debug_print("%d ", zoneIds[i]);
  }
  sl_zigbee_app_debug_println("");

  sl_zigbee_af_fill_command_ias_ace_cluster_bypass_response(numberOfZones,
                                                            zoneIds,
                                                            numberOfZones);
  sl_zigbee_af_send_command_unicast(SL_ZIGBEE_OUTGOING_DIRECT, sender);
}

static uint32_t zcl_ias_ace_cluster_server_command_handler(sl_service_opcode_t opcode,
                                                           sl_service_function_context_t *context)
{
  (void)opcode;

  sl_zigbee_af_cluster_command_t *cmd = (sl_zigbee_af_cluster_command_t *)context->data;

  switch (cmd->commandId) {
    case ZCL_ARM_COMMAND_ID:
    {
      sl_zcl_ias_ace_cluster_arm_command_t cmd_data;

      if (zcl_decode_ias_ace_cluster_arm_command(cmd, &cmd_data)
          != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
        return SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND;
      }

      ias_ace_cluster_arm_command_handler(cmd_data.armMode,
                                          cmd_data.armDisarmCode,
                                          cmd_data.zoneId);
      return SL_ZIGBEE_ZCL_STATUS_SUCCESS;
    }
    case ZCL_BYPASS_COMMAND_ID:
    {
      sl_zcl_ias_ace_cluster_bypass_command_t cmd_data;

      if (zcl_decode_ias_ace_cluster_bypass_command(cmd, &cmd_data)
          != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
        return SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND;
      }

      ias_ace_cluster_bypass_command_handler(cmd_data.numberOfZones,
                                             cmd_data.zoneIds,
                                             cmd_data.armDisarmCode);

      return SL_ZIGBEE_ZCL_STATUS_SUCCESS;
    }
  }

  return SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND;
}

//----------------------
// Implemented Callbacks

/** @brief Init
 * Application init function
 */
void sl_zigbee_af_main_init_cb(void)
{
  // Subscribe to ZCL commands for the IAS_ACE cluster, server side.
  sl_zigbee_subscribe_to_zcl_commands(ZCL_IAS_ACE_CLUSTER_ID,
                                      0xFFFF,
                                      ZCL_DIRECTION_CLIENT_TO_SERVER,
                                      zcl_ias_ace_cluster_server_command_handler);
}

/** @brief Green power server notification forward callback
 *
 * This function is called by the green power server plugin to notify the
 * application of a Green Power Gp Notification of an incoming gpd command.
 * Return true to handle in application.
 *
 * @returns true if application handled it and plugin will not process it anymore.
 * else return false to process the notification by the plugin  Ver.: always
 */
bool sl_zigbee_af_green_power_cluster_gp_notification_forward_cb(uint16_t options,
                                                                 sl_zigbee_gp_address_t * addr,
                                                                 uint32_t gpdSecurityFrameCounter,
                                                                 uint8_t gpdCommandId,
                                                                 uint8_t * gpdCommandPayload,
                                                                 uint16_t gppShortAddress,
                                                                 uint8_t  gppDistance)
{
  sl_zigbee_app_debug_println("Received Notification GpdAdd [AppId = %d, SrcId = 0x%04X] for Cmd = %d withRxAfterTx = %d",
                              addr->applicationId,
                              addr->id.sourceId,
                              gpdCommandId,
                              (options & 0x0800) ? true : false);
  return false;
}

// EZSP Custom Frame handler for messages from host
void sl_zigbee_ezsp_custom_frame_handler(int8u payloadLength,
                                         int8u* payload)
{
  sl_zigbee_app_debug_print("EZSP Custom Frame : ");
  sl_zigbee_app_debug_print_buffer(payload, payloadLength, true);
  sl_zigbee_app_debug_println("");
}

//-------------------------------------
// Custom CLI commands and related code

// The manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h.
// The names are enumerated here to make it easier for the user.
const char * ezspMfgTokenNames[] =
{
  "SL_ZIGBEE_EZSP_MFG_CUSTOM_VERSION...",
  "SL_ZIGBEE_EZSP_MFG_STRING...........",
  "SL_ZIGBEE_EZSP_MFG_BOARD_NAME.......",
  "SL_ZIGBEE_EZSP_MFG_MANUF_ID.........",
  "SL_ZIGBEE_EZSP_MFG_PHY_CONFIG.......",
  "SL_ZIGBEE_EZSP_MFG_BOOTLOAD_AES_KEY.",
  "SL_ZIGBEE_EZSP_MFG_ASH_CONFIG.......",
  "SL_ZIGBEE_EZSP_MFG_SL_ZIGBEE_EZSP_STORAGE.....",
  "SL_ZIGBEE_EZSP_STACK_CAL_DATA.......",
  "SL_ZIGBEE_EZSP_MFG_CBKE_DATA........",
  "SL_ZIGBEE_EZSP_MFG_INSTALLATION_CODE"
};

// Called to dump all of the tokens. This dumps the indices, the names,
// and the values using sl_zigbee_ezsp_get_token and sl_zigbee_ezsp_get_mfg_token. The indices
// are used for read and write functions below.
void mfgappTokenDump(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_status_t status;
  uint8_t tokenData[MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE];
  uint8_t index, i, tokenLength;

  // first go through the tokens accessed using sl_zigbee_ezsp_get_token
  sl_zigbee_app_debug_println("(data shown little endian)");
  sl_zigbee_app_debug_println("Tokens:");
  sl_zigbee_app_debug_println("idx  value:");
  for (index = 0; index < MFGSAMP_NUM_EZSP_TOKENS; index++) {
    // get the token data here
    status = sl_zigbee_ezsp_get_token(index, tokenData);
    sl_zigbee_app_debug_print("[%d]", index);
    if (status == SL_STATUS_OK) {
      // Print out the token data
      for (i = 0; i < MFGSAMP_EZSP_TOKEN_SIZE; i++) {
        sl_zigbee_app_debug_print(" %02X", tokenData[i]);
      }
      sl_zigbee_app_debug_println("");
    } else {
      // handle when sl_zigbee_ezsp_get_token returns an error
      sl_zigbee_app_debug_println(" ... error 0x%x ...", status);
    }
  }

  // now go through the tokens accessed using sl_zigbee_ezsp_get_mfg_token
  // the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
  // this file contains an array (ezspMfgTokenNames) representing the names.
  sl_zigbee_app_debug_println("Manufacturing Tokens:");
  sl_zigbee_app_debug_println("idx  token name                 len   value");
  for (index = 0; index < MFGSAMP_NUM_EZSP_MFG_TOKENS; index++) {
    // sl_zigbee_ezsp_get_mfg_token returns a length, be careful to only access
    // valid token indices.
    tokenLength = sl_zigbee_ezsp_get_mfg_token(index, tokenData);
    sl_zigbee_app_debug_println("[%x] %s: 0x%x:",
                                index, ezspMfgTokenNames[index], tokenLength);

    // Print out the token data
    for (i = 0; i < tokenLength; i++) {
      if ((i != 0) && ((i % 8) == 0)) {
        sl_zigbee_app_debug_println("");
        sl_zigbee_app_debug_print("                                    :");
      }
      sl_zigbee_app_debug_print(" %02X", tokenData[i]);
    }
    sl_zigbee_app_debug_println("");
  }
  sl_zigbee_app_debug_println("");
}

void changeNwkKeyCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_status_t status = sl_zigbee_af_trust_center_start_network_key_update();

  if (status != SL_STATUS_OK) {
    sl_zigbee_app_debug_println("Change Key Error %X", status);
  } else {
    sl_zigbee_app_debug_println("Change Key Success");
  }
}

static void dcPrintKey(uint8_t label, uint8_t *key)
{
  uint8_t i;
  sl_zigbee_app_debug_println("key %x: ", label);
  for (i = 0; i < SL_ZIGBEE_ENCRYPTION_KEY_SIZE; i++) {
    sl_zigbee_app_debug_print("%02X", key[i]);
  }
  sl_zigbee_app_debug_println("");
}

void printNextKeyCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_status_t status;
  sl_zigbee_sec_man_context_t context;
  sl_zigbee_sec_man_key_t plaintext_key;

  sl_zigbee_sec_man_init_context(&context);

  context.core_key_type = SL_ZB_SEC_MAN_KEY_TYPE_NETWORK;
  context.key_index = 1;

  status = sl_zigbee_sec_man_export_key(&context, &plaintext_key);

  if (status != SL_STATUS_OK) {
    sl_zigbee_app_debug_println("Error getting key");
  } else {
    dcPrintKey(1, plaintext_key.key);
  }
}

void versionCommand(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_app_debug_println("Version:  0.1 Alpha");
  sl_zigbee_app_debug_println(" %s", __DATE__);
  sl_zigbee_app_debug_println(" %s", __TIME__);
  sl_zigbee_app_debug_println("");
#ifdef SL_ZIGBEE_TEST
  sl_zigbee_app_debug_println("Print formatter test : 0x%x=0x12, 0x%02x=0x1234 0x%04x=0x12345678",
                              0x12, 0x1234, 0x12345678);
#endif
}

void setTxPowerCommand(sl_cli_command_arg_t *arguments)
{
  int8_t dBm = sl_cli_get_argument_int8(arguments, 0);

  sl_zigbee_set_radio_power(dBm);
}

/* This sample application demostrates an NCP using a custom protocol to
 * communicate with the host. As an example protocol, the NCP has defined
 * commands so that the host can drive Green Power on the NCP.
 *
 * The host sends custom EZSP commands to the NCP, and the NCP acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the ncp-uart-hw-gp
 * sample application in the NCP Application Framework.
 */
static void sendGreenPowerCommand(uint8_t command,
                                  uint8_t* possibleParam,
                                  uint8_t paramLength,
                                  bool replyExpected)
{
  sl_status_t status;
  uint8_t commandLength = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  uint8_t replyLength = GREEN_POWER_MAX_FRAME_LENGTH;
  uint8_t replyPayload[GREEN_POWER_MAX_FRAME_LENGTH];

  if ((possibleParam != NULL) && (paramLength > 0)) {
    commandLength = paramLength;
    if (paramLength > GREEN_POWER_MAX_FRAME_LENGTH) {
      commandLength = GREEN_POWER_MAX_FRAME_LENGTH;
    }
    memcpy(commandPayload, possibleParam, commandLength);
  } else {
    return;
  }
  // Send the command to the NCP.
  status = sl_zigbee_ezsp_custom_frame(commandLength,
                                       commandPayload,
                                       &replyLength,
                                       replyPayload);
  sl_zigbee_app_debug_println("Send custom frame Command Id = 0x%X Status = 0x%X", command, status);
  sl_zigbee_app_debug_println("replyExpected = 0x%X replyLength = 0x%X", replyExpected, replyLength);
  // If we were expecting a response, display it.
  if (replyExpected
      && status == 0
      && replyLength > 0) {
    sl_zigbee_app_debug_print("Response : ");
    sl_zigbee_app_debug_print_buffer(replyPayload, replyLength, true);
    sl_zigbee_app_debug_println("");
  }
}

void gpAppGpTxQueueInit(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx++] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueSet(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  size_t dataLength = 0;
  uint8_t *data;

  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  commandPayload[idx++] = sl_cli_get_argument_uint8(arguments, 2); // command Id

  data = sl_cli_get_argument_hex(arguments, 3, &dataLength);
  (void) memcpy(&commandPayload[idx], data, dataLength);
  idx += dataLength;

  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueRemove(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_REMOVE_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

void gpAppGpTxQueueGet(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = sl_cli_get_argument_uint32(arguments, 1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}

void gpAppGpSetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE_MAX_SIZE;
  idx += sizeof(uint8_t);
  uint16_t maxTxGpQueueSize = sl_cli_get_argument_uint16(arguments, 0);
  commandPayload[idx++] = maxTxGpQueueSize >> 0;
  commandPayload[idx++] = maxTxGpQueueSize >> 8;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}
void gpAppGpGetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_MAX_SIZE;
  idx += sizeof(uint8_t);
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}
void gpAppGpGetTxQueueCount(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_COUNT;
  idx += sizeof(uint8_t);
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}

void gpAppGpSendGpRaw(sl_cli_command_arg_t *arguments)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  size_t dataLength = 0;
  uint8_t *data;

  commandPayload[idx] = SL_ZIGBEE_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW;
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 0); // Channel
  idx += sizeof(uint8_t);
  commandPayload[idx] = sl_cli_get_argument_uint8(arguments, 1); // delay
  idx += sizeof(uint8_t);

  data = sl_cli_get_argument_hex(arguments, 3, &dataLength);
  (void) memcpy(&commandPayload[idx], data, dataLength);
  idx += dataLength;

  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

#ifdef SL_CATALOG_ZIGBEE_AF_SUPPORT_PRESENT
bool sl_zigbee_af_get_endpoint_info_cb(int8u endpoint,
                                       int8u* returnNetworkIndex,
                                       sl_zigbee_af_endpoint_info_struct_t* returnEndpointInfo)
{
  // In case GP endpoint is located on the NCP, the host has no way
  // to know what networkIndex and profileId that endpoint is configured.
  // User has to manually provide that data.
  if (endpoint == 242) {
    *returnNetworkIndex = 0;
    returnEndpointInfo->profileId = 0xA1E0;
    return true;
  }
  return false;
}
#endif // SL_CATALOG_ZIGBEE_AF_SUPPORT_PRESENT

void sl_zigbee_reset_zigbee_token(sl_cli_command_arg_t *arguments)
{
  bool exclude_outgoing_fc = sl_cli_get_argument_uint8(arguments, 0) != 0 ? true : false;
  bool exclude_boot_counter = sl_cli_get_argument_uint8(arguments, 1) != 0 ? true : false;
  sl_zigbee_token_factory_reset(exclude_outgoing_fc, exclude_boot_counter);
  sl_zigbee_app_debug_println("Done");
}
