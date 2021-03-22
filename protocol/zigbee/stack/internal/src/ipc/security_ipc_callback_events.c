/***************************************************************************//**
 * @file security_ipc_callback_events.c
 * @brief callback event handlers for security
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
// automatically generated from security.h.  Do not manually edit
#include "stack/internal/src/ipc/security_ipc_callback_events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_switch_network_key_handler(uint8_t sequenceNumber)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.switch_network_key_handler.sequenceNumber = sequenceNumber;
  cb_event->tag = SLI_ZIGBEE_STACK_SWITCH_NETWORK_KEY_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_zigbee_key_establishment_handler(sl_802154_long_addr_t partner,
                                                       sl_zigbee_key_status_t status)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  if (partner != NULL) {
    memmove(cb_event->data.zigbee_key_establishment_handler.partner, partner, sizeof(sl_802154_long_addr_t));
  }

  cb_event->data.zigbee_key_establishment_handler.status = status;
  cb_event->tag = SLI_ZIGBEE_STACK_ZIGBEE_KEY_ESTABLISHMENT_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_security_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_SWITCH_NETWORK_KEY_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_switch_network_key_handler(cb_event->data.switch_network_key_handler.sequenceNumber);
      break;

    case SLI_ZIGBEE_STACK_ZIGBEE_KEY_ESTABLISHMENT_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_zigbee_key_establishment_handler(cb_event->data.zigbee_key_establishment_handler.partner,
                                                 cb_event->data.zigbee_key_establishment_handler.status);
      break;

    default:
      /* do nothing */
      break;
  }
}
