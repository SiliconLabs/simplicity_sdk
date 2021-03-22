/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Utility Client plugin.
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

#include "app/framework/include/af.h"

#include "zap-cluster-command-parser.h"

bool sl_zigbee_af_zll_commissioning_cluster_endpoint_information_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_zll_commissioning_cluster_endpoint_information_command_t cmd_data;

  if (zcl_decode_zll_commissioning_cluster_endpoint_information_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_zll_commissioning_cluster_print("RX: EndpointInformation ");
  sl_zigbee_af_zll_commissioning_cluster_debug_exec(sl_zigbee_af_print_big_endian_eui64(cmd_data.ieeeAddress));
  sl_zigbee_af_zll_commissioning_cluster_println(", 0x%2x, 0x%x, 0x%2x, 0x%2x, 0x%x",
                                                 cmd_data.networkAddress,
                                                 cmd_data.endpointId,
                                                 cmd_data.profileId,
                                                 cmd_data.deviceId,
                                                 cmd_data.version);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_zll_commissioning_cluster_get_group_identifiers_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_zll_commissioning_cluster_get_group_identifiers_response_command_t cmd_data;
  uint16_t groupInformationRecordListLen;
  uint16_t groupInformationRecordListIndex;
  uint8_t i;

  if (zcl_decode_zll_commissioning_cluster_get_group_identifiers_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }
  groupInformationRecordListLen = (sl_zigbee_af_current_command()->bufLen
                                   - (sl_zigbee_af_current_command()->payloadStartIndex
                                      + sizeof(cmd_data.total)
                                      + sizeof(cmd_data.startIndex)
                                      + sizeof(cmd_data.count)));
  groupInformationRecordListIndex = 0;

  sl_zigbee_af_zll_commissioning_cluster_print("RX: GetGroupIdentifiersResponse 0x%x, 0x%x, 0x%x,",
                                               cmd_data.total,
                                               cmd_data.startIndex,
                                               cmd_data.count);

  for (i = 0; i < cmd_data.count; i++) {
    uint16_t groupId;
    uint8_t groupType;
    groupId = sl_zigbee_af_get_int16u(cmd_data.groupInformationRecordList, groupInformationRecordListIndex, groupInformationRecordListLen);
    groupInformationRecordListIndex += 2;
    groupType = sl_zigbee_af_get_int8u(cmd_data.groupInformationRecordList, groupInformationRecordListIndex, groupInformationRecordListLen);
    groupInformationRecordListIndex++;
    sl_zigbee_af_zll_commissioning_cluster_print(" [0x%2x 0x%x]", groupId, groupType);
  }
  sl_zigbee_af_zll_commissioning_cluster_println("");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_zll_commissioning_cluster_get_endpoint_list_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  (void)cmd;

#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_zll_commissioning_cluster_get_endpoint_list_response_command_t cmd_data;
  uint16_t endpointInformationRecordListLen;
  uint16_t endpointInformationRecordListIndex;
  uint8_t i;

  if (zcl_decode_zll_commissioning_cluster_get_endpoint_list_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  endpointInformationRecordListLen = (sl_zigbee_af_current_command()->bufLen
                                      - (sl_zigbee_af_current_command()->payloadStartIndex
                                         + sizeof(cmd_data.total)
                                         + sizeof(cmd_data.startIndex)
                                         + sizeof(cmd_data.count)));
  endpointInformationRecordListIndex = 0;

  sl_zigbee_af_zll_commissioning_cluster_print("RX: GetEndpointListResponse 0x%x, 0x%x, 0x%x,",
                                               cmd_data.total,
                                               cmd_data.startIndex,
                                               cmd_data.count);

  for (i = 0; i < cmd_data.count; i++) {
    uint16_t networkAddress;
    uint8_t endpointId;
    uint16_t profileId;
    uint16_t deviceId;
    uint8_t version;
    networkAddress = sl_zigbee_af_get_int16u(cmd_data.endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    endpointId = sl_zigbee_af_get_int8u(cmd_data.endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex++;
    profileId = sl_zigbee_af_get_int16u(cmd_data.endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    deviceId = sl_zigbee_af_get_int16u(cmd_data.endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    version = sl_zigbee_af_get_int8u(cmd_data.endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex++;
    sl_zigbee_af_zll_commissioning_cluster_print(" [0x%2x 0x%x 0x%2x 0x%2x 0x%x]",
                                                 networkAddress,
                                                 endpointId,
                                                 profileId,
                                                 deviceId,
                                                 version);
  }

  sl_zigbee_af_zll_commissioning_cluster_println("");
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  sl_zigbee_af_send_default_response(sl_zigbee_af_current_command(), SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

uint32_t sl_zigbee_af_zll_commissioning_cluster_client_command_parse(sl_service_opcode_t opcode,
                                                                     sl_service_function_context_t *context)
{
  (void)opcode;

  sl_zigbee_af_cluster_command_t *cmd = (sl_zigbee_af_cluster_command_t *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_ENDPOINT_INFORMATION_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_zll_commissioning_cluster_endpoint_information_cb(cmd);
        break;
      }
      case ZCL_GET_GROUP_IDENTIFIERS_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_zll_commissioning_cluster_get_group_identifiers_response_cb(cmd);
        break;
      }
      case ZCL_GET_ENDPOINT_LIST_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_zll_commissioning_cluster_get_endpoint_list_response_cb(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? SL_ZIGBEE_ZCL_STATUS_SUCCESS
          : SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND);
}
