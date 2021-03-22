/***************************************************************************//**
 * @file sl_zigbee_dynamic_commissioning_ipc_callback_events.c
 * @brief callback event handlers for sl_zigbee_dynamic_commissioning
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
// automatically generated from sl_zigbee_dynamic_commissioning.h.  Do not manually edit
#include "stack/internal/src/ipc/sl_zigbee_dynamic_commissioning_ipc_callback_events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_dynamic_commissioning_alert_callback(sl_zigbee_address_info *ids,
                                                           sl_zigbee_dynamic_commissioning_event_t event)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));

  if (ids != NULL) {
    cb_event->data.dynamic_commissioning_alert_callback.ids = *ids;
  }

  cb_event->data.dynamic_commissioning_alert_callback.event = event;
  cb_event->tag = SLI_ZIGBEE_STACK_DYNAMIC_COMMISSIONING_ALERT_CALLBACK_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_sl_zigbee_dynamic_commissioning_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_DYNAMIC_COMMISSIONING_ALERT_CALLBACK_IPC_EVENT_TYPE:
      sl_zigbee_dynamic_commissioning_alert_callback(&cb_event->data.dynamic_commissioning_alert_callback.ids,
                                                     cb_event->data.dynamic_commissioning_alert_callback.event);
      break;

    default:
      /* do nothing */
      break;
  }
}
