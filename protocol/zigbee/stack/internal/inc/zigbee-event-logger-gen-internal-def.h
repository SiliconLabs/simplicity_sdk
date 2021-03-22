/***************************************************************************//**
 * @file zigbee-event-logger-gen-internal-def.h
 * @brief internal names for 'zigbee-event-logger-gen' declarations
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
#ifndef ZIGBEE_EVENT_LOGGER_GEN_INTERNAL_DEF_H
#define ZIGBEE_EVENT_LOGGER_GEN_INTERNAL_DEF_H

#include "stack/include/zigbee-event-logger-gen.h"

// Command Indirection

void sli_zigbee_stack_af_zig_bee_event_logger_add_boot_event(uint32_t bootCount,
                                                             uint16_t resetType);

void sli_zigbee_stack_af_zig_bee_event_logger_add_channel_change(uint8_t page,
                                                                 uint8_t channel);

void sli_zigbee_stack_af_zig_bee_event_logger_add_child_added(sl_802154_short_addr_t childNodeId,
                                                              sl_802154_long_addr_t childEui64);

void sli_zigbee_stack_af_zig_bee_event_logger_add_child_removed(sl_802154_short_addr_t childNodeId,
                                                                sl_802154_long_addr_t childEui64);

void sli_zigbee_stack_af_zig_bee_event_logger_add_child_timed_out(sl_802154_short_addr_t childNodeId,
                                                                  sl_802154_long_addr_t childEui64);

void sli_zigbee_stack_af_zig_bee_event_logger_add_duty_cycle_change(sl_zigbee_duty_cycle_state_t state,
                                                                    sl_zigbee_duty_cycle_hecto_pct_t limitThreshold,
                                                                    sl_zigbee_duty_cycle_hecto_pct_t criticalThreshold,
                                                                    sl_zigbee_duty_cycle_hecto_pct_t suspendedLimit);

void sli_zigbee_stack_af_zig_bee_event_logger_add_join_network(sl_802154_pan_id_t panId,
                                                               uint8_t channelNumber,
                                                               uint8_t page,
                                                               uint8_t *extendedPanId);

void sli_zigbee_stack_af_zig_bee_event_logger_add_leave_with_rejoin(sl_802154_short_addr_t sourceNode,
                                                                    sl_zigbee_leave_reason_t reason);

void sli_zigbee_stack_af_zig_bee_event_logger_add_leave_without_rejoin(sl_802154_short_addr_t sourceNode,
                                                                       sl_zigbee_leave_reason_t reason);

void sli_zigbee_stack_af_zig_bee_event_logger_add_network_key_sequence_change(uint8_t sequence);

void sli_zigbee_stack_af_zig_bee_event_logger_add_parent_change(sl_802154_short_addr_t parentNodeId,
                                                                sl_802154_long_addr_t parentEui64);

void sli_zigbee_stack_af_zig_bee_event_logger_add_reset(uint8_t resetCode,
                                                        uint8_t extendedReason);

void sli_zigbee_stack_af_zig_bee_event_logger_add_secure_rejoin(uint32_t channelMask,
                                                                sl_zigbee_rejoin_reason_t reason);

void sli_zigbee_stack_af_zig_bee_event_logger_add_stack_status(sl_status_t status);

void sli_zigbee_stack_af_zig_bee_event_logger_add_time_sync(uint32_t utcTime,
                                                            uint32_t secondsSinceBoot);

void sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_join_decision(sl_802154_short_addr_t nodeId,
                                                                             uint8_t decision);

void sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_link_key_change(void);

void sli_zigbee_stack_af_zig_bee_event_logger_add_trust_center_rejoin(uint32_t channelMask,
                                                                      sl_zigbee_rejoin_reason_t reason);

#endif // ZIGBEE_EVENT_LOGGER_GEN_INTERNAL_DEF_H
