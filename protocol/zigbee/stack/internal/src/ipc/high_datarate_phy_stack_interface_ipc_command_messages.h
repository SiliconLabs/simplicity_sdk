/***************************************************************************//**
 * @file high_datarate_phy_stack_interface_ipc_command_messages.h
 * @brief defines structured format for 'high_datarate_phy_stack_interface' ipc messages
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
// automatically generated from high_datarate_phy_stack_interface.h.  Do not manually edit
#ifndef HIGH_DATARATE_PHY_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H
#define HIGH_DATARATE_PHY_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H

#include "stack/include/high_datarate_phy_stack_interface.h"
#include "stack/internal/inc/high_datarate_phy_stack_interface_internal_def.h"

typedef struct {
  RAIL_CsmaConfig_t csma_params;
} sli_mac_stack_lower_mac_set_high_datarate_csma_params_ipc_req_t;

typedef struct {
  sli_mac_stack_lower_mac_set_high_datarate_csma_params_ipc_req_t request;
} sli_mac_stack_lower_mac_set_high_datarate_csma_params_ipc_msg_t;

typedef struct {
  sl_zigbee_multiprotocol_priorities_t priorities;
} sli_mac_stack_lower_mac_set_high_datarate_phy_radio_priorities_ipc_req_t;

typedef struct {
  sli_mac_stack_lower_mac_set_high_datarate_phy_radio_priorities_ipc_req_t request;
} sli_mac_stack_lower_mac_set_high_datarate_phy_radio_priorities_ipc_msg_t;

typedef struct {
  uint8_t nwk_index;
  uint8_t payload[MAX_HIGH_DATARATE_PHY_PACKET_LENGTH];
} sli_mac_stack_send_raw_high_datarate_phy_message_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_send_raw_high_datarate_phy_message_ipc_rsp_t;

typedef struct {
  sli_mac_stack_send_raw_high_datarate_phy_message_ipc_req_t request;
  sli_mac_stack_send_raw_high_datarate_phy_message_ipc_rsp_t response;
} sli_mac_stack_send_raw_high_datarate_phy_message_ipc_msg_t;

typedef struct {
  bool enable_f;
} sli_mac_stack_set_mode_switch_sync_detect_ipc_req_t;

typedef struct {
  RAIL_Status_t result;
} sli_mac_stack_set_mode_switch_sync_detect_ipc_rsp_t;

typedef struct {
  sli_mac_stack_set_mode_switch_sync_detect_ipc_req_t request;
  sli_mac_stack_set_mode_switch_sync_detect_ipc_rsp_t response;
} sli_mac_stack_set_mode_switch_sync_detect_ipc_msg_t;

#endif // HIGH_DATARATE_PHY_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H
