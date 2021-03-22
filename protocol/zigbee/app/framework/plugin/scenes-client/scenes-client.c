/***************************************************************************//**
 * @file
 * @brief Routines for the Scenes Client plugin, which implements the client
 *        side of the Scenes cluster.
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

#include "../../include/af.h"
#include "scenes-client.h"

#include "zap-cluster-command-parser.h"

bool sl_zigbee_af_scenes_cluster_add_scene_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_add_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_add_scene_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return sl_zigbee_af_scenes_client_parse_add_scene_response(cmd,
                                                             cmd_data.status,
                                                             cmd_data.groupId,
                                                             cmd_data.sceneId);
}

bool sl_zigbee_af_scenes_cluster_view_scene_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_view_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_view_scene_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return sl_zigbee_af_scenes_client_parse_view_scene_response(cmd,
                                                              cmd_data.status,
                                                              cmd_data.groupId,
                                                              cmd_data.sceneId,
                                                              cmd_data.transitionTime,
                                                              cmd_data.sceneName,
                                                              cmd_data.extensionFieldSets);
}

bool sl_zigbee_af_scenes_cluster_remove_scene_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_remove_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_remove_scene_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_scenes_cluster_println("RX: RemoveSceneResponse 0x%x, 0x%2x, 0x%x",
                                      cmd_data.status,
                                      cmd_data.groupId,
                                      cmd_data.sceneId);
  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_scenes_cluster_remove_all_scenes_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_remove_all_scenes_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_remove_all_scenes_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_scenes_cluster_println("RX: RemoveAllScenesResponse 0x%x, 0x%2x",
                                      cmd_data.status,
                                      cmd_data.groupId);
  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_scenes_cluster_store_scene_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_store_scene_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_store_scene_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_scenes_cluster_println("RX: StoreSceneResponse 0x%x, 0x%2x, 0x%x",
                                      cmd_data.status,
                                      cmd_data.groupId,
                                      cmd_data.sceneId);
  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_scenes_cluster_get_scene_membership_response_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_scenes_cluster_get_scene_membership_response_command_t cmd_data;

  if (zcl_decode_scenes_cluster_get_scene_membership_response_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_scenes_cluster_print("RX: GetSceneMembershipResponse 0x%x, 0x%x, 0x%2x",
                                    cmd_data.status,
                                    cmd_data.capacity,
                                    cmd_data.groupId);

  // Scene count and the scene list only appear in the payload if the status is
  // SUCCESS.
  if (cmd_data.status == SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    uint8_t i;
    sl_zigbee_af_scenes_cluster_print(", 0x%x,", cmd_data.sceneCount);
    for (i = 0; i < cmd_data.sceneCount; i++) {
      sl_zigbee_af_scenes_cluster_print(" [0x%x]", cmd_data.sceneList[i]);
    }
  }

  sl_zigbee_af_scenes_cluster_println("");
  sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_scenes_client_parse_add_scene_response(const sl_zigbee_af_cluster_command_t *cmd,
                                                         uint8_t status,
                                                         uint16_t groupId,
                                                         uint8_t sceneId)
{
  bool enhanced = (cmd->commandId == ZCL_ENHANCED_ADD_SCENE_COMMAND_ID);
  sl_zigbee_af_scenes_cluster_println("RX: %pAddSceneResponse 0x%x, 0x%2x, 0x%x",
                                      (enhanced ? "Enhanced" : ""),
                                      status,
                                      groupId,
                                      sceneId);
  sl_zigbee_af_send_default_response(cmd, SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

bool sl_zigbee_af_scenes_client_parse_view_scene_response(const sl_zigbee_af_cluster_command_t *cmd,
                                                          uint8_t status,
                                                          uint16_t groupId,
                                                          uint8_t sceneId,
                                                          uint16_t transitionTime,
                                                          const uint8_t *sceneName,
                                                          const uint8_t *extensionFieldSets)
{
  bool enhanced = (cmd->commandId == ZCL_ENHANCED_VIEW_SCENE_COMMAND_ID);

  sl_zigbee_af_scenes_cluster_print("RX: %pViewSceneResponse 0x%x, 0x%2x, 0x%x",
                                    (enhanced ? "Enhanced" : ""),
                                    status,
                                    groupId,
                                    sceneId);

  // Transition time, scene name, and the extension field sets only appear in
  // the payload if the status is SUCCESS.
  if (status == SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    uint16_t extensionFieldSetsLen = (sl_zigbee_af_current_command()->bufLen
                                      - (sl_zigbee_af_current_command()->payloadStartIndex
                                         + sizeof(status)
                                         + sizeof(groupId)
                                         + sizeof(sceneId)
                                         + sizeof(transitionTime)
                                         + sl_zigbee_af_string_length(sceneName) + 1));
    uint16_t extensionFieldSetsIndex = 0;

    sl_zigbee_af_scenes_cluster_print(", 0x%2x, \"", transitionTime);
    sl_zigbee_af_scenes_cluster_print_string(sceneName);
    sl_zigbee_af_scenes_cluster_print("\",");

    // Each extension field set contains at least a two-byte cluster id and a
    // one-byte length.
    while (extensionFieldSetsIndex + 3 <= extensionFieldSetsLen) {
      sl_zigbee_af_cluster_id_t clusterId;
      uint8_t length;
      clusterId = sl_zigbee_af_get_int16u(extensionFieldSets, extensionFieldSetsIndex, extensionFieldSetsLen);
      extensionFieldSetsIndex += 2;
      length = sl_zigbee_af_get_int8u(extensionFieldSets, extensionFieldSetsIndex, extensionFieldSetsLen);
      extensionFieldSetsIndex++;
      sl_zigbee_af_scenes_cluster_print(" [0x%2x 0x%x ", clusterId, length);
      if (extensionFieldSetsIndex + length <= extensionFieldSetsLen) {
        sl_zigbee_af_scenes_cluster_print_buffer(extensionFieldSets + extensionFieldSetsIndex, length, false);
      }
      sl_zigbee_af_scenes_cluster_print("]");
      sl_zigbee_af_scenes_cluster_flush();
      extensionFieldSetsIndex += length;
    }
  }

  sl_zigbee_af_scenes_cluster_println("");
  sl_zigbee_af_send_default_response(cmd, SL_ZIGBEE_ZCL_STATUS_SUCCESS);
  return true;
}

uint32_t sl_zigbee_af_scenes_cluster_client_command_parse(sl_service_opcode_t opcode,
                                                          sl_service_function_context_t *context)
{
  (void)opcode;

  sl_zigbee_af_cluster_command_t *cmd = (sl_zigbee_af_cluster_command_t *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_ADD_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_add_scene_response_cb(cmd);
        break;
      }
      case ZCL_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_get_scene_membership_response_cb(cmd);
        break;
      }
      case ZCL_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_remove_all_scenes_response_cb(cmd);
        break;
      }
      case ZCL_REMOVE_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_remove_scene_response_cb(cmd);
        break;
      }
      case ZCL_STORE_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_store_scene_response_cb(cmd);
        break;
      }
      case ZCL_VIEW_SCENE_RESPONSE_COMMAND_ID:
      {
        wasHandled = sl_zigbee_af_scenes_cluster_view_scene_response_cb(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? SL_ZIGBEE_ZCL_STATUS_SUCCESS
          : SL_ZIGBEE_ZCL_STATUS_UNSUP_COMMAND);
}
