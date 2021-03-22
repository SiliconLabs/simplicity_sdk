/***************************************************************************//**
 * @file high_datarate_phy_stack_interface.h
 * @brief Declarations for high bandwidth phy APIs that cross the
 * stack/application boundary.  This header is primarily intended for internal use;
 * see high_datarate_phy.h for declarations of the plugin APIs themselves.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef HIGH_DATARATE_PHY_STACK_H
#define HIGH_DATARATE_PHY_STACK_H

#include PLATFORM_HEADER
#include "sl_status.h"
#include "../../../../platform/radio/rail_lib/common/rail_types.h"
#include "sl_zigbee_types.h"

#define MAX_HIGH_DATARATE_PHY_PACKET_LENGTH 253
/**
 * @brief sends the given payload over the high datarate phy
 * @internal SL_ZIGBEE_IPC_ARGS
 * {# payload | length: ((payload[1] << 8) + payload[0] + 2) | max: MAX_HIGH_DATARATE_PHY_PACKET_LENGTH #}
 */
sl_status_t sl_mac_send_raw_high_datarate_phy_message(uint8_t nwk_index,
                                                      uint8_t *payload);

void sl_mac_lower_mac_set_high_datarate_csma_params(RAIL_CsmaConfig_t *csma_params);

void sl_mac_lower_mac_set_high_datarate_phy_radio_priorities(sl_zigbee_multiprotocol_priorities_t *priorities);

/**
 * @brief callback that is issued when the given packet is received over the high datarate phy
 * @internal SL_ZIGBEE_IPC_ARGS
 * {# packet | length: ((packet[1] << 8) + packet[0] + 2) | max: MAX_HIGH_DATARATE_PHY_PACKET_LENGTH #}
 */
void sl_mac_high_datarate_phy_rx_callback(uint8_t *packet,
                                          uint8_t linkQuality,
                                          int8_t rssi);

RAIL_Status_t sl_mac_set_mode_switch_sync_detect(bool enable_f);
/**
 * @brief callback that is issued when the given packet is transmitted over the high datarate phy
 * @internal SL_ZIGBEE_IPC_ARGS
 * {# packet_contents | length: (packet_length) | max: MAX_HIGH_DATARATE_PHY_PACKET_LENGTH #}
 */
void sl_mac_high_datarate_phy_tx_callback(uint8_t mac_index,
                                          sl_status_t status,
                                          uint16_t packet_length,
                                          uint8_t *packet_contents, uint8_t tag);
#endif //HIGH_DATARATE_PHY_STACK_H
