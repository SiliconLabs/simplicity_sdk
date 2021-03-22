/***************************************************************************//**
 * @file network-formation-ipc-callback-events.c
 * @brief callback event handlers for network-formation
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
// automatically generated from network-formation.h.  Do not manually edit
#include "stack/internal/src/ipc/network-formation-ipc-callback-events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_energy_scan_result_handler(uint8_t channel,
                                                 int8_t maxRssiValue)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.energy_scan_result_handler.channel = channel;
  cb_event->data.energy_scan_result_handler.maxRssiValue = maxRssiValue;
  cb_event->tag = SLI_ZIGBEE_STACK_ENERGY_SCAN_RESULT_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_network_found_handler(sl_zigbee_zigbee_network_t *networkFound,
                                            uint8_t lqi,
                                            int8_t rssi)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  if (networkFound != NULL) {
    cb_event->data.network_found_handler.networkFound = *networkFound;
  }

  cb_event->data.network_found_handler.lqi = lqi;
  cb_event->data.network_found_handler.rssi = rssi;
  cb_event->tag = SLI_ZIGBEE_STACK_NETWORK_FOUND_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_orphan_notification_handler(sl_802154_long_addr_t longId)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  if (longId != NULL) {
    memmove(cb_event->data.orphan_notification_handler.longId, longId, sizeof(sl_802154_long_addr_t));
  }

  cb_event->tag = SLI_ZIGBEE_STACK_ORPHAN_NOTIFICATION_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_scan_complete_handler(uint8_t channel,
                                            sl_status_t status)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.scan_complete_handler.channel = channel;
  cb_event->data.scan_complete_handler.status = status;
  cb_event->tag = SLI_ZIGBEE_STACK_SCAN_COMPLETE_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_unused_pan_id_found_handler(sl_802154_pan_id_t panId,
                                                  uint8_t channel)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.unused_pan_id_found_handler.panId = panId;
  cb_event->data.unused_pan_id_found_handler.channel = channel;
  cb_event->tag = SLI_ZIGBEE_STACK_UNUSED_PAN_ID_FOUND_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_network_formation_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_ENERGY_SCAN_RESULT_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_energy_scan_result_handler(cb_event->data.energy_scan_result_handler.channel,
                                           cb_event->data.energy_scan_result_handler.maxRssiValue);
      break;

    case SLI_ZIGBEE_STACK_NETWORK_FOUND_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_network_found_handler(&cb_event->data.network_found_handler.networkFound,
                                      cb_event->data.network_found_handler.lqi,
                                      cb_event->data.network_found_handler.rssi);
      break;

    case SLI_ZIGBEE_STACK_ORPHAN_NOTIFICATION_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_orphan_notification_handler(cb_event->data.orphan_notification_handler.longId);
      break;

    case SLI_ZIGBEE_STACK_SCAN_COMPLETE_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_scan_complete_handler(cb_event->data.scan_complete_handler.channel,
                                      cb_event->data.scan_complete_handler.status);
      break;

    case SLI_ZIGBEE_STACK_UNUSED_PAN_ID_FOUND_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_unused_pan_id_found_handler(cb_event->data.unused_pan_id_found_handler.panId,
                                            cb_event->data.unused_pan_id_found_handler.channel);
      break;

    default:
      /* do nothing */
      break;
  }
}
