/***************************************************************************//**
 * @file high_datarate_phy_stack_interface_internal_def.h
 * @brief internal names for 'high_datarate_phy_stack_interface' declarations
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
#ifndef HIGH_DATARATE_PHY_STACK_INTERFACE_INTERNAL_DEF_H
#define HIGH_DATARATE_PHY_STACK_INTERFACE_INTERNAL_DEF_H

#include "stack/include/high_datarate_phy_stack_interface.h"

// Command Indirection

void sli_mac_stack_lower_mac_set_high_datarate_csma_params(RAIL_CsmaConfig_t *csma_params);

void sli_mac_stack_lower_mac_set_high_datarate_phy_radio_priorities(sl_zigbee_multiprotocol_priorities_t *priorities);

sl_status_t sli_mac_stack_send_raw_high_datarate_phy_message(uint8_t nwk_index,
                                                             uint8_t *payload);

RAIL_Status_t sli_mac_stack_set_mode_switch_sync_detect(bool enable_f);

// Callback Indirection

void sli_mac_stack_high_datarate_phy_rx_callback(uint8_t *packet,
                                                 uint8_t linkQuality,
                                                 int8_t rssi);

void sli_mac_stack_high_datarate_phy_tx_callback(uint8_t mac_index,
                                                 sl_status_t status,
                                                 uint16_t packet_length,
                                                 uint8_t *packet_contents,
                                                 uint8_t tag);

#endif // HIGH_DATARATE_PHY_STACK_INTERFACE_INTERNAL_DEF_H
