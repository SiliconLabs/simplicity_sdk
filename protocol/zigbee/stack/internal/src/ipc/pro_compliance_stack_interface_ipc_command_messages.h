/***************************************************************************//**
 * @file pro_compliance_stack_interface_ipc_command_messages.h
 * @brief defines structured format for 'pro_compliance_stack_interface' ipc messages
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
// automatically generated from pro_compliance_stack_interface.h.  Do not manually edit
#ifndef PRO_COMPLIANCE_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H
#define PRO_COMPLIANCE_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H

#include "stack/include/pro_compliance_stack_interface.h"
#include "stack/internal/inc/pro_compliance_stack_interface_internal_def.h"

typedef struct {
  sl_802154_long_addr_t eui64;
} sli_mac_stack_find_child_short_id_ipc_req_t;

typedef struct {
  sl_802154_short_addr_t result;
} sli_mac_stack_find_child_short_id_ipc_rsp_t;

typedef struct {
  sli_mac_stack_find_child_short_id_ipc_req_t request;
  sli_mac_stack_find_child_short_id_ipc_rsp_t response;
} sli_mac_stack_find_child_short_id_ipc_msg_t;

typedef struct {
  uint8_t childIndex;
} sli_mac_stack_get_child_info_flags_ipc_req_t;

typedef struct {
  uint64_t result;
} sli_mac_stack_get_child_info_flags_ipc_rsp_t;

typedef struct {
  sli_mac_stack_get_child_info_flags_ipc_req_t request;
  sli_mac_stack_get_child_info_flags_ipc_rsp_t response;
} sli_mac_stack_get_child_info_flags_ipc_msg_t;

typedef struct {
  uint8_t nwk_index;
} sli_mac_stack_indirect_purge_ipc_req_t;

typedef struct {
  sli_mac_stack_indirect_purge_ipc_req_t request;
} sli_mac_stack_indirect_purge_ipc_msg_t;

typedef struct {
  uint8_t mac_index;
} sli_mac_stack_kickstart_ipc_req_t;

typedef struct {
  sli_mac_stack_kickstart_ipc_req_t request;
} sli_mac_stack_kickstart_ipc_msg_t;

typedef struct {
  uint8_t mac_index;
} sli_mac_stack_lower_mac_radio_is_on_ipc_req_t;

typedef struct {
  bool result;
} sli_mac_stack_lower_mac_radio_is_on_ipc_rsp_t;

typedef struct {
  sli_mac_stack_lower_mac_radio_is_on_ipc_req_t request;
  sli_mac_stack_lower_mac_radio_is_on_ipc_rsp_t response;
} sli_mac_stack_lower_mac_radio_is_on_ipc_msg_t;

typedef struct {
  uint8_t mac_index;
  uint8_t channel;
} sli_mac_stack_lower_mac_set_radio_channel_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_lower_mac_set_radio_channel_ipc_rsp_t;

typedef struct {
  sli_mac_stack_lower_mac_set_radio_channel_ipc_req_t request;
  sli_mac_stack_lower_mac_set_radio_channel_ipc_rsp_t response;
} sli_mac_stack_lower_mac_set_radio_channel_ipc_msg_t;

typedef struct {
  uint8_t mac_index;
  uint8_t mode;
} sli_mac_stack_lower_mac_set_radio_idle_mode_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_lower_mac_set_radio_idle_mode_ipc_rsp_t;

typedef struct {
  sli_mac_stack_lower_mac_set_radio_idle_mode_ipc_req_t request;
  sli_mac_stack_lower_mac_set_radio_idle_mode_ipc_rsp_t response;
} sli_mac_stack_lower_mac_set_radio_idle_mode_ipc_msg_t;

typedef struct {
  uint8_t mac_index;
  int8_t power;
} sli_mac_stack_lower_mac_set_radio_power_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_lower_mac_set_radio_power_ipc_rsp_t;

typedef struct {
  sli_mac_stack_lower_mac_set_radio_power_ipc_req_t request;
  sli_mac_stack_lower_mac_set_radio_power_ipc_rsp_t response;
} sli_mac_stack_lower_mac_set_radio_power_ipc_msg_t;

typedef struct {
  bool isCoordinator;
} sli_mac_stack_set_coordinator_ipc_req_t;

typedef struct {
  sli_mac_stack_set_coordinator_ipc_req_t request;
} sli_mac_stack_set_coordinator_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t parentId;
  uint16_t panId;
} sli_mac_stack_test_associate_command_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_test_associate_command_ipc_rsp_t;

typedef struct {
  sli_mac_stack_test_associate_command_ipc_req_t request;
  sli_mac_stack_test_associate_command_ipc_rsp_t response;
} sli_mac_stack_test_associate_command_ipc_msg_t;

typedef struct {
  uint8_t macCommandLength;
  uint8_t macCommand[MAX_IPC_VEC_ARG_CAPACITY];
} sli_mac_stack_test_send_mac_command_ipc_req_t;

typedef struct {
  sli_mac_stack_test_send_mac_command_ipc_req_t request;
} sli_mac_stack_test_send_mac_command_ipc_msg_t;

typedef struct {
  uint8_t channel;
} sli_mac_stack_test_set_nwk_radio_params_channel_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_test_set_nwk_radio_params_channel_ipc_rsp_t;

typedef struct {
  sli_mac_stack_test_set_nwk_radio_params_channel_ipc_req_t request;
  sli_mac_stack_test_set_nwk_radio_params_channel_ipc_rsp_t response;
} sli_mac_stack_test_set_nwk_radio_params_channel_ipc_msg_t;

typedef struct {
  uint8_t network_index;
  sl_802154_long_addr_t eui;
} sli_mac_stack_test_set_nwk_radio_params_eui_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_test_set_nwk_radio_params_eui_ipc_rsp_t;

typedef struct {
  sli_mac_stack_test_set_nwk_radio_params_eui_ipc_req_t request;
  sli_mac_stack_test_set_nwk_radio_params_eui_ipc_rsp_t response;
} sli_mac_stack_test_set_nwk_radio_params_eui_ipc_msg_t;

typedef struct {
  int8_t power;
} sli_mac_stack_test_set_tx_power_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_mac_stack_test_set_tx_power_ipc_rsp_t;

typedef struct {
  sli_mac_stack_test_set_tx_power_ipc_req_t request;
  sli_mac_stack_test_set_tx_power_ipc_rsp_t response;
} sli_mac_stack_test_set_tx_power_ipc_msg_t;

typedef struct {
  uint8_t timeout;
} sli_zigbee_stack_set_end_device_poll_timeout_ipc_req_t;

typedef struct {
  sli_zigbee_stack_set_end_device_poll_timeout_ipc_req_t request;
} sli_zigbee_stack_set_end_device_poll_timeout_ipc_msg_t;

typedef struct {
  uint16_t state;
} sli_zigbee_stack_set_packet_validate_library_state_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_set_packet_validate_library_state_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_set_packet_validate_library_state_ipc_req_t request;
  sli_zigbee_stack_set_packet_validate_library_state_ipc_rsp_t response;
} sli_zigbee_stack_set_packet_validate_library_state_ipc_msg_t;

typedef struct {
  uint16_t panId;
} sli_zigbee_stack_set_pan_id_ipc_req_t;

typedef struct {
  sli_zigbee_stack_set_pan_id_ipc_req_t request;
} sli_zigbee_stack_set_pan_id_ipc_msg_t;

typedef struct {
  boolean set_value;
  uint8_t nwk_index;
} sli_zigbee_stack_set_pan_id_conflict_report_ipc_req_t;

typedef struct {
  sli_zigbee_stack_set_pan_id_conflict_report_ipc_req_t request;
} sli_zigbee_stack_set_pan_id_conflict_report_ipc_msg_t;

typedef struct {
  sl_802154_long_addr_t eui64;
} sli_zigbee_stack_test_aps_key_in_sync_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_aps_key_in_sync_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_aps_key_in_sync_ipc_req_t request;
  sli_zigbee_stack_test_aps_key_in_sync_ipc_rsp_t response;
} sli_zigbee_stack_test_aps_key_in_sync_ipc_msg_t;

typedef struct {
  sl_802154_long_addr_t eui64;
  bool setSync;
} sli_zigbee_stack_test_aps_key_set_sync_status_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_aps_key_set_sync_status_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_aps_key_set_sync_status_ipc_req_t request;
  sli_zigbee_stack_test_aps_key_set_sync_status_ipc_rsp_t response;
} sli_zigbee_stack_test_aps_key_set_sync_status_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destShort;
  sl_zigbee_sec_man_context_t context;
  uint8_t cmdoptions;
} sli_zigbee_stack_test_custom_send_security_challenge_request_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_custom_send_security_challenge_request_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_custom_send_security_challenge_request_ipc_req_t request;
  sli_zigbee_stack_test_custom_send_security_challenge_request_ipc_rsp_t response;
} sli_zigbee_stack_test_custom_send_security_challenge_request_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t discoveryNodeId;
  bool reportKids;
  uint8_t childStartIndex;
  uint8_t sourceEndpoint;
  sl_zigbee_aps_option_t options;
  sl_802154_short_addr_t targetNodeIdOfRequest;
} sli_zigbee_stack_test_ieee_address_request_to_target_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_ieee_address_request_to_target_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_ieee_address_request_to_target_ipc_req_t request;
  sli_zigbee_stack_test_ieee_address_request_to_target_ipc_rsp_t response;
} sli_zigbee_stack_test_ieee_address_request_to_target_ipc_msg_t;

typedef struct {
  uint8_t command;
  uint8_t eui64List[MAX_IPC_VEC_ARG_CAPACITY];
  uint8_t counts;
} sli_zigbee_stack_test_join_list_add_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_join_list_add_ipc_req_t request;
} sli_zigbee_stack_test_join_list_add_ipc_msg_t;

typedef struct {
  uint8_t startIndex;
} sli_zigbee_stack_test_join_list_request_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_join_list_request_ipc_req_t request;
} sli_zigbee_stack_test_join_list_request_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destination;
  uint8_t commandFrame;
  uint8_t length;
  bool tryToInsertLongDest;
  sl_802154_long_addr_t destinationEui;
} sli_zigbee_stack_test_network_send_command_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_network_send_command_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_network_send_command_ipc_req_t request;
  sli_zigbee_stack_test_network_send_command_ipc_rsp_t response;
} sli_zigbee_stack_test_network_send_command_ipc_msg_t;

typedef struct {
  uint32_t scanChannels;
  uint8_t scanDuration;
} sli_zigbee_stack_test_perform_raw_active_scan_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_perform_raw_active_scan_ipc_req_t request;
} sli_zigbee_stack_test_perform_raw_active_scan_ipc_msg_t;

typedef struct {
  uint8_t mask;
} sli_zigbee_stack_test_reset_frame_counter_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_reset_frame_counter_ipc_req_t request;
} sli_zigbee_stack_test_reset_frame_counter_ipc_msg_t;

typedef struct {
  uint16_t newShortId;
  sl_802154_long_addr_t newLongId;
  bool apsEncryption;
  uint8_t deviceStatus;
} sli_zigbee_stack_test_send_device_update_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_send_device_update_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_device_update_ipc_req_t request;
  sli_zigbee_stack_test_send_device_update_ipc_rsp_t response;
} sli_zigbee_stack_test_send_device_update_ipc_msg_t;

typedef struct {
  uint16_t destId;
  sl_802154_long_addr_t destEui;
} sli_zigbee_stack_test_send_leave_request_command_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_send_leave_request_command_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_leave_request_command_ipc_req_t request;
  sli_zigbee_stack_test_send_leave_request_command_ipc_rsp_t response;
} sli_zigbee_stack_test_send_leave_request_command_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t targetNodeId;
  sl_802154_long_addr_t targetEui64;
  uint8_t keyType;
  sl_zigbee_key_data_t key;
  bool useApsEncryption;
} sli_zigbee_stack_test_send_link_key_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_send_link_key_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_link_key_ipc_req_t request;
  sli_zigbee_stack_test_send_link_key_ipc_rsp_t response;
} sli_zigbee_stack_test_send_link_key_ipc_msg_t;

typedef struct {
  uint8_t cmd_id;
  sl_802154_long_addr_t longId;
  sl_802154_short_addr_t oldShortId;
  sl_802154_short_addr_t newShortId;
  bool useNwkSecurity;
  uint8_t status;
  bool reallySend;
} sli_zigbee_stack_test_send_network_rejoin_command_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_send_network_rejoin_command_ipc_req_t request;
} sli_zigbee_stack_test_send_network_rejoin_command_ipc_msg_t;

typedef struct {
  uint8_t requestedTimeoutValue;
} sli_zigbee_stack_test_send_network_timeout_request_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_send_network_timeout_request_ipc_req_t request;
} sli_zigbee_stack_test_send_network_timeout_request_ipc_msg_t;

typedef struct {
  uint16_t destId;
  sl_802154_long_addr_t destEui;
  sl_802154_long_addr_t deviceToRemoveEui;
  bool sendNonEncrypted;
} sli_zigbee_stack_test_send_remove_device_command_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_send_remove_device_command_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_remove_device_command_ipc_req_t request;
  sli_zigbee_stack_test_send_remove_device_command_ipc_rsp_t response;
} sli_zigbee_stack_test_send_remove_device_command_ipc_msg_t;

typedef struct {
  uint8_t command;
  uint8_t updateId;
  uint16_t panId;
} sli_zigbee_stack_test_send_report_or_update_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_send_report_or_update_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_report_or_update_ipc_req_t request;
  sli_zigbee_stack_test_send_report_or_update_ipc_rsp_t response;
} sli_zigbee_stack_test_send_report_or_update_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destination;
  sl_802154_short_addr_t target;
  uint8_t errorCode;
  uint8_t payload;
  uint8_t payload_len;
} sli_zigbee_stack_test_send_route_error_payload_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_send_route_error_payload_ipc_req_t request;
} sli_zigbee_stack_test_send_route_error_payload_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destination;
  sl_802154_short_addr_t target;
  uint8_t errorCode;
  uint8_t payload;
  uint8_t payload_len;
} sli_zigbee_stack_test_send_route_error_payload_no_network_encryption_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_send_route_error_payload_no_network_encryption_ipc_req_t request;
} sli_zigbee_stack_test_send_route_error_payload_no_network_encryption_ipc_msg_t;

typedef struct {
  uint16_t newDeviceShortId;
  sl_802154_long_addr_t newDeviceLongId;
  uint32_t apsEncryption;
  uint8_t deviceStatus;
} sli_zigbee_stack_test_send_update_device_command_ipc_req_t;

typedef struct {
  bool result;
} sli_zigbee_stack_test_send_update_device_command_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_send_update_device_command_ipc_req_t request;
  sli_zigbee_stack_test_send_update_device_command_ipc_rsp_t response;
} sli_zigbee_stack_test_send_update_device_command_ipc_msg_t;

typedef struct {
  uint8_t stackProfile;
  uint8_t nodeType;
  uint8_t channel;
  int8_t power;
  uint16_t nodeId;
  uint16_t panId;
  sl_802154_long_addr_t extendedPanId;
} sli_zigbee_stack_test_set_network_tokens_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_set_network_tokens_ipc_req_t request;
} sli_zigbee_stack_test_set_network_tokens_ipc_msg_t;

typedef struct {
  uint16_t shortId;
  uint8_t sourceEUI64;
  sl_802154_long_addr_t deviceAnnounceEui;
  uint8_t capabilities;
} sli_zigbee_stack_test_spoof_device_announcement_ipc_req_t;

typedef struct {
  sli_zigbee_stack_test_spoof_device_announcement_ipc_req_t request;
} sli_zigbee_stack_test_spoof_device_announcement_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destination;
  bool encrypt;
  uint8_t eui64_list[MAX_IPC_VEC_ARG_CAPACITY];
  uint8_t counts;
} sli_zigbee_stack_test_zdo_generate_clear_all_bindings_req_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_zdo_generate_clear_all_bindings_req_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_zdo_generate_clear_all_bindings_req_ipc_req_t request;
  sli_zigbee_stack_test_zdo_generate_clear_all_bindings_req_ipc_rsp_t response;
} sli_zigbee_stack_test_zdo_generate_clear_all_bindings_req_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t dest;
  sl_zigbee_aps_option_t aps_options;
  sl_802154_long_addr_t target;
} sli_zigbee_stack_test_zdo_generate_get_authentication_level_req_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_zdo_generate_get_authentication_level_req_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_zdo_generate_get_authentication_level_req_ipc_req_t request;
  sli_zigbee_stack_test_zdo_generate_get_authentication_level_req_ipc_rsp_t response;
} sli_zigbee_stack_test_zdo_generate_get_authentication_level_req_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t destination;
  bool encrypt;
  uint8_t eui64_list[MAX_IPC_VEC_ARG_CAPACITY];
  uint8_t counts;
} sli_zigbee_stack_test_zdo_generate_security_decommission_req_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_test_zdo_generate_security_decommission_req_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_test_zdo_generate_security_decommission_req_ipc_req_t request;
  sli_zigbee_stack_test_zdo_generate_security_decommission_req_ipc_rsp_t response;
} sli_zigbee_stack_test_zdo_generate_security_decommission_req_ipc_msg_t;

typedef struct {
  sl_802154_short_addr_t childId;
  uint8_t options;
} sli_zigbee_stack_zigbee_remove_child_ipc_req_t;

typedef struct {
  sl_status_t result;
} sli_zigbee_stack_zigbee_remove_child_ipc_rsp_t;

typedef struct {
  sli_zigbee_stack_zigbee_remove_child_ipc_req_t request;
  sli_zigbee_stack_zigbee_remove_child_ipc_rsp_t response;
} sli_zigbee_stack_zigbee_remove_child_ipc_msg_t;

#endif // PRO_COMPLIANCE_STACK_INTERFACE_IPC_COMMAND_MESSAGES_H
