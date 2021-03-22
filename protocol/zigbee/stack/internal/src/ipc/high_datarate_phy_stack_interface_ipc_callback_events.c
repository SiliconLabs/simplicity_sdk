/***************************************************************************//**
 * @file high_datarate_phy_stack_interface_ipc_callback_events.c
 * @brief callback event handlers for high_datarate_phy_stack_interface
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
#include "stack/internal/src/ipc/high_datarate_phy_stack_interface_ipc_callback_events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_mac_stack_high_datarate_phy_rx_callback(uint8_t *packet,
                                                 uint8_t linkQuality,
                                                 int8_t rssi)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  if (packet != NULL) {
    memmove(cb_event->data.high_datarate_phy_rx_callback.packet, packet, sizeof(uint8_t) * (((packet[1] << 8) + packet[0] + 2)));
  }

  cb_event->data.high_datarate_phy_rx_callback.linkQuality = linkQuality;
  cb_event->data.high_datarate_phy_rx_callback.rssi = rssi;
  cb_event->tag = SLI_MAC_STACK_HIGH_DATARATE_PHY_RX_CALLBACK_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_mac_stack_high_datarate_phy_tx_callback(uint8_t mac_index,
                                                 sl_status_t status,
                                                 uint16_t packet_length,
                                                 uint8_t *packet_contents,
                                                 uint8_t tag)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.high_datarate_phy_tx_callback.mac_index = mac_index;
  cb_event->data.high_datarate_phy_tx_callback.status = status;
  cb_event->data.high_datarate_phy_tx_callback.packet_length = packet_length;

  if (packet_contents != NULL) {
    memmove(cb_event->data.high_datarate_phy_tx_callback.packet_contents, packet_contents, sizeof(uint8_t) * ((packet_length)));
  }

  cb_event->data.high_datarate_phy_tx_callback.tag = tag;
  cb_event->tag = SLI_MAC_STACK_HIGH_DATARATE_PHY_TX_CALLBACK_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_high_datarate_phy_stack_interface_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_MAC_STACK_HIGH_DATARATE_PHY_RX_CALLBACK_IPC_EVENT_TYPE:
      sl_mac_high_datarate_phy_rx_callback(cb_event->data.high_datarate_phy_rx_callback.packet,
                                           cb_event->data.high_datarate_phy_rx_callback.linkQuality,
                                           cb_event->data.high_datarate_phy_rx_callback.rssi);
      break;

    case SLI_MAC_STACK_HIGH_DATARATE_PHY_TX_CALLBACK_IPC_EVENT_TYPE:
      sl_mac_high_datarate_phy_tx_callback(cb_event->data.high_datarate_phy_tx_callback.mac_index,
                                           cb_event->data.high_datarate_phy_tx_callback.status,
                                           cb_event->data.high_datarate_phy_tx_callback.packet_length,
                                           cb_event->data.high_datarate_phy_tx_callback.packet_contents,
                                           cb_event->data.high_datarate_phy_tx_callback.tag);
      break;

    default:
      /* do nothing */
      break;
  }
}
