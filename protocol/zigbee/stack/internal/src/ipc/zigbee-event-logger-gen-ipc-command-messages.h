/***************************************************************************//**
 * @file zigbee-event-logger-gen-ipc-command-messages.h
 * @brief defines structured format for 'zigbee-event-logger-gen' ipc messages
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
// automatically generated from zigbee-event-logger-gen.h.  Do not manually edit
#ifndef ZIGBEE_EVENT_LOGGER_GEN_IPC_COMMAND_MESSAGES_H
#define ZIGBEE_EVENT_LOGGER_GEN_IPC_COMMAND_MESSAGES_H

#include "stack/include/zigbee-event-logger-gen.h"
#include "stack/internal/inc/zigbee-event-logger-gen-internal-def.h"

typedef struct {
  uint32_t bootCount;
  uint16_t resetType;
} sli_zigbee_stack_af_zig_bee_event_logger_add_boot_event_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_boot_event_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_boot_event_ipc_msg_t;

typedef struct {
  uint8_t page;
  uint8_t channel;
} sli_zigbee_stack_af_zig_bee_event_logger_add_channel_change_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_channel_change_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_channel_change_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t childNodeId;
  sl_802154_long_addr_t childEui64;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_added_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_child_added_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_added_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t childNodeId;
  sl_802154_long_addr_t childEui64;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_removed_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_child_removed_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_removed_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t childNodeId;
  sl_802154_long_addr_t childEui64;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_timed_out_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_child_timed_out_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_child_timed_out_ipc_msg_t;

typedef struct {
  sl_zigbee_duty_cycle_state_t state;
  sl_zigbee_duty_cycle_hecto_pct_t limitThreshold;
  sl_zigbee_duty_cycle_hecto_pct_t criticalThreshold;
  sl_zigbee_duty_cycle_hecto_pct_t suspendedLimit;
} sli_zigbee_stack_af_zig_bee_event_logger_add_duty_cycle_change_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_duty_cycle_change_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_duty_cycle_change_ipc_msg_t;

typedef struct {
  sl_802154_pan_id_t panId;
  uint8_t channelNumber;
  uint8_t page;
  uint8_t extendedPanId;
} sli_zigbee_stack_af_zig_bee_event_logger_add_join_network_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_join_network_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_join_network_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t sourceNode;
  sl_zigbee_leave_reason_t reason;
} sli_zigbee_stack_af_zig_bee_event_logger_add_leave_with_rejoin_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_leave_with_rejoin_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_leave_with_rejoin_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t sourceNode;
  sl_zigbee_leave_reason_t reason;
} sli_zigbee_stack_af_zig_bee_event_logger_add_leave_without_rejoin_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_leave_without_rejoin_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_leave_without_rejoin_ipc_msg_t;

typedef struct {
  uint8_t sequence;
} sli_zigbee_stack_af_zig_bee_event_logger_add_network_key_sequence_change_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_network_key_sequence_change_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_network_key_sequence_change_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t parentNodeId;
  sl_802154_long_addr_t parentEui64;
} sli_zigbee_stack_af_zig_bee_event_logger_add_parent_change_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_parent_change_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_parent_change_ipc_msg_t;

typedef struct {
  uint8_t resetCode;
  uint8_t extendedReason;
} sli_zigbee_stack_af_zig_bee_event_logger_add_reset_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_reset_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_reset_ipc_msg_t;

typedef struct {
  uint32_t channelMask;
  sl_zigbee_rejoin_reason_t reason;
} sli_zigbee_stack_af_zig_bee_event_logger_add_secure_rejoin_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_secure_rejoin_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_secure_rejoin_ipc_msg_t;

typedef struct {
  sl_status_t status;
} sli_zigbee_stack_af_zig_bee_event_logger_add_stack_status_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_stack_status_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_stack_status_ipc_msg_t;

typedef struct {
  uint32_t utcTime;
  uint32_t secondsSinceBoot;
} sli_zigbee_stack_af_zig_bee_event_logger_add_time_sync_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_time_sync_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_time_sync_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t nodeId;
  uint8_t decision;
} sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_join_decision_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_join_decision_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_join_decision_ipc_msg_t;

typedef struct {
  uint32_t channelMask;
  sl_zigbee_rejoin_reason_t reason;
} sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_rejoin_ipc_req_t;

typedef struct {
  sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_rejoin_ipc_req_t request;
} sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_rejoin_ipc_msg_t;

#endif // ZIGBEE_EVENT_LOGGER_GEN_IPC_COMMAND_MESSAGES_H
