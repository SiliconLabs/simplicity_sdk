/***************************************************************************//**
 * @file stack-info-ipc-callback-events.c
 * @brief callback event handlers for stack-info
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
// automatically generated from stack-info.h.  Do not manually edit
#include "stack/internal/src/ipc/stack-info-ipc-callback-events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_radio_needs_calibrating_handler(void)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  cb_event->tag = SLI_ZIGBEE_STACK_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_stack_status_handler(sl_status_t status)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.stack_status_handler.status = status;
  cb_event->tag = SLI_ZIGBEE_STACK_STACK_STATUS_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_stack_token_changed_handler(uint16_t tokenAddress)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.stack_token_changed_handler.tokenAddress = tokenAddress;
  cb_event->tag = SLI_ZIGBEE_STACK_STACK_TOKEN_CHANGED_HANDLER_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_stack_info_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_radio_needs_calibrating_handler();
      break;

    case SLI_ZIGBEE_STACK_STACK_STATUS_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_stack_status_handler(cb_event->data.stack_status_handler.status);
      break;

    case SLI_ZIGBEE_STACK_STACK_TOKEN_CHANGED_HANDLER_IPC_EVENT_TYPE:
      sl_zigbee_stack_token_changed_handler(cb_event->data.stack_token_changed_handler.tokenAddress);
      break;

    default:
      /* do nothing */
      break;
  }
}
