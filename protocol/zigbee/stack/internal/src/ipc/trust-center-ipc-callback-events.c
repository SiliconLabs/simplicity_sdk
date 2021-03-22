/***************************************************************************//**
 * @file trust-center-ipc-callback-events.c
 * @brief callback event handlers for trust-center
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
// automatically generated from trust-center.h.  Do not manually edit
#include "stack/internal/src/ipc/trust-center-ipc-callback-events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_trust_center_post_join_handler(sl_802154_short_addr_t newNodeId,
                                                     sl_802154_long_addr_t newNodeEui64,
                                                     sl_zigbee_device_update_t status,
                                                     sl_zigbee_join_decision_t policyDecision,
                                                     sl_802154_short_addr_t parentOfNewNode)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.trust_center_post_join_handler.newNodeId = newNodeId;

  if (newNodeEui64 != NULL) {
    memmove(cb_event->data.trust_center_post_join_handler.newNodeEui64, newNodeEui64, sizeof(sl_802154_long_addr_t));
  }

  cb_event->data.trust_center_post_join_handler.status = status;
  cb_event->data.trust_center_post_join_handler.policyDecision = policyDecision;
  cb_event->data.trust_center_post_join_handler.parentOfNewNode = parentOfNewNode;
  cb_event->tag = SLI_ZIGBEE_STACK_TRUST_CENTER_POST_JOIN_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_trust_center_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_TRUST_CENTER_POST_JOIN_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_trust_center_post_join_handler(cb_event->data.trust_center_post_join_handler.newNodeId,
                                               cb_event->data.trust_center_post_join_handler.newNodeEui64,
                                               cb_event->data.trust_center_post_join_handler.status,
                                               cb_event->data.trust_center_post_join_handler.policyDecision,
                                               cb_event->data.trust_center_post_join_handler.parentOfNewNode);
      break;

    default:
      /* do nothing */
      break;
  }
}
