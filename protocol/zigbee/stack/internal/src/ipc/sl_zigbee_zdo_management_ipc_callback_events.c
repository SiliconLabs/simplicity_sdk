/***************************************************************************//**
 * @file sl_zigbee_zdo_management_ipc_callback_events.c
 * @brief callback event handlers for sl_zigbee_zdo_management
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
// automatically generated from sl_zigbee_zdo_management.h.  Do not manually edit
#include "stack/internal/src/ipc/sl_zigbee_zdo_management_ipc_callback_events.h"
#include "stack/internal/src/ipc/zigbee_ipc_callback_events.h"
extern void sl_zigbee_wakeup_common_task(void);

void sli_zigbee_stack_beacon_survey_complete_callback(sl_zigbee_zdo_status_t status,
                                                      sl_zigbee_beacon_survey_results_t *survey_results,
                                                      uint8_t potential_parent_count,
                                                      sl_zigbee_potential_parent_t *potential_parents,
                                                      uint16_t pan_id_conflicts)
{
  sl_zigbee_stack_cb_event_t *cb_event = (sl_zigbee_stack_cb_event_t *) malloc(sizeof(sl_zigbee_stack_cb_event_t));
  cb_event->data.beacon_survey_complete_callback.status = status;

  if (survey_results != NULL) {
    cb_event->data.beacon_survey_complete_callback.survey_results = *survey_results;
  }

  cb_event->data.beacon_survey_complete_callback.potential_parent_count = potential_parent_count;

  if (potential_parents != NULL) {
    memmove(cb_event->data.beacon_survey_complete_callback.potential_parents, potential_parents, sizeof(sl_zigbee_potential_parent_t) * (potential_parent_count));
  }

  cb_event->data.beacon_survey_complete_callback.pan_id_conflicts = pan_id_conflicts;
  cb_event->tag = SLI_ZIGBEE_STACK_BEACON_SURVEY_COMPLETE_CALLBACK_IPC_EVENT_TYPE;
  sl_event_publish(&sli_zigbee_ipc_publisher, SL_EVENT_CLASS_ZIGBEE, 1 /*priority*/, cb_event);
  sl_zigbee_wakeup_common_task();
}

void sli_zigbee_sl_zigbee_zdo_management_process_ipc_event(sl_zigbee_stack_cb_event_t *cb_event)
{
  switch (cb_event->tag) {
    case SLI_ZIGBEE_STACK_BEACON_SURVEY_COMPLETE_CALLBACK_IPC_EVENT_TYPE:
      sl_zigbee_beacon_survey_complete_callback(cb_event->data.beacon_survey_complete_callback.status,
                                                &cb_event->data.beacon_survey_complete_callback.survey_results,
                                                cb_event->data.beacon_survey_complete_callback.potential_parent_count,
                                                cb_event->data.beacon_survey_complete_callback.potential_parents,
                                                cb_event->data.beacon_survey_complete_callback.pan_id_conflicts);
      break;

    default:
      /* do nothing */
      break;
  }
}