/***************************************************************************//**
 * @file
 * @brief Code to handle meter snapshot client behavior.
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
#include "app/framework/util/common.h"

#include "zap-cluster-command-parser.h"

#ifndef fieldLength
  #define fieldLength(field) \
  (sl_zigbee_af_current_command()->bufLen - (field - sl_zigbee_af_current_command()->buffer))
#endif

bool sl_zigbee_af_simple_metering_cluster_schedule_snapshot_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_simple_metering_cluster_schedule_snapshot_response_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_schedule_snapshot_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_simple_metering_cluster_println("RX: ScheduleSnapshotResponse 0x%4x",
                                               cmd_data.issuerEventId);

  // according to the documentation, the payload comprises of two uint8_t types
  sl_zigbee_af_simple_metering_cluster_println("    Payload: 0x%x, 0x%x",
                                               cmd_data.snapshotResponsePayload[0],
                                               cmd_data.snapshotResponsePayload[2]);

  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_simple_metering_cluster_take_snapshot_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_simple_metering_cluster_take_snapshot_response_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_take_snapshot_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }
  sl_zigbee_af_simple_metering_cluster_println("RX: TakeSnapshotResponse 0x%4x, 0x%x",
                                               cmd_data.snapshotId,
                                               cmd_data.snapshotConfirmation);

  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_simple_metering_cluster_publish_snapshot_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_simple_metering_cluster_publish_snapshot_command_t cmd_data;

  if (zcl_decode_simple_metering_cluster_publish_snapshot_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }
  uint8_t i;
  uint16_t snapshotPayloadLength = fieldLength(cmd_data.snapshotPayload);

  sl_zigbee_af_simple_metering_cluster_println("RX: PublishSnapshotResponse 0x%4x, 0x%4x, 0x%x, 0x%x, 0x%x, 0x%4x, 0x%x",
                                               cmd_data.snapshotId,
                                               cmd_data.snapshotTime,
                                               cmd_data.totalSnapshotsFound,
                                               cmd_data.commandIndex,
                                               cmd_data.totalCommands,
                                               cmd_data.snapshotCause,
                                               cmd_data.snapshotPayloadType);

  // Documentation only mentions payload type for debt/credit status, which comprises of 6 int32 types
  // so I am printing rows of four int8 values at a time, until the end of payload
  // Not passing proper payload structure may result in garbage print values
  if (cmd_data.snapshotPayloadType == SL_ZIGBEE_ZCL_PREPAY_SNAPSHOT_PAYLOAD_TYPE_DEBT_CREDIT_STATUS) {
    sl_zigbee_af_simple_metering_cluster_println("    Payload:");

    for (i = 0; i < snapshotPayloadLength; i += 4) {
      sl_zigbee_af_simple_metering_cluster_println("            0x%x 0x%x 0x%x 0x%x",
                                                   cmd_data.snapshotPayload[i],
                                                   cmd_data.snapshotPayload[i + 1],
                                                   cmd_data.snapshotPayload[i + 2],
                                                   cmd_data.snapshotPayload[i + 3]);
    }
  }

  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

sl_zigbee_af_status_t sl_zigbee_af_simple_metering_cluster_client_snapshot_command_parse(sl_service_opcode_t opcode,
                                                                                         sl_service_function_context_t *context)
{
  (void)opcode;

  sl_zigbee_af_cluster_command_t *cmd = (sl_zigbee_af_cluster_command_t *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_SCHEDULE_SNAPSHOT_RESPONSE_COMMAND_ID:
        wasHandled = sl_zigbee_af_simple_metering_cluster_schedule_snapshot_response_cb(cmd);
        break;
      case ZCL_TAKE_SNAPSHOT_RESPONSE_COMMAND_ID:
        wasHandled = sl_zigbee_af_simple_metering_cluster_take_snapshot_response_cb(cmd);
        break;
      case ZCL_PUBLISH_SNAPSHOT_COMMAND_ID:
        wasHandled = sl_zigbee_af_simple_metering_cluster_publish_snapshot_cb(cmd);
        break;
      default:
        break;
    }
  }
  return ((wasHandled)
          ? SL_ZIGBEE_ZCL_STATUS_SUCCESS
          : SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND);
}
