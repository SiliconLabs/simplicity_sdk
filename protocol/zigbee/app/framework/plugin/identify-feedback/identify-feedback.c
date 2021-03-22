/***************************************************************************//**
 * @file
 * @brief Routines for the Identify Feedback plugin, which implements the
 *        feedback component of the Identify cluster.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// this file contains all the common includes for clusters in the util
#include "app/framework/include/af.h"

#include "app/framework/util/common.h"
#if !defined(EZSP_HOST)
#include "hal/hal.h"
#endif // !defined(EZSP_HOST)

#include "identify-feedback-config.h"
sl_zigbee_af_event_t sl_zigbee_af_identify_feedback_provide_feedback_event;
#define provideFeedbackEventControl (&sl_zigbee_af_identify_feedback_provide_feedback_event)
void sl_zigbee_af_identify_feedback_provide_feedback_event_handler(sl_zigbee_af_event_t * event);
#if (SL_ZIGBEE_AF_PLUGIN_IDENTIFY_FEEDBACK_LED_FEEDBACK == 1)
#define LED_FEEDBACK
#endif

static bool identifyTable[SL_ZIGBEE_ZCL_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT];

void sli_zigbee_af_identify_init_callback(uint8_t init_level)
{
  (void)init_level;

  sl_zigbee_af_event_init(provideFeedbackEventControl,
                          sl_zigbee_af_identify_feedback_provide_feedback_event_handler);
}
void sl_zigbee_af_identify_feedback_provide_feedback_event_handler(sl_zigbee_af_event_t * event)
{
#if !defined(EZSP_HOST)
#ifdef LED_FEEDBACK
  halToggleLed(BOARDLED0);
  halToggleLed(BOARDLED1);
  halToggleLed(BOARDLED2);
  halToggleLed(BOARDLED3);
#endif // LED_FEEDBACK
#endif

  sl_zigbee_af_event_set_delay_ms(provideFeedbackEventControl,
                                  MILLISECOND_TICKS_PER_SECOND);
}

void sl_zigbee_af_identify_start_feedback_cb(uint8_t endpoint,
                                             uint16_t identifyTime)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_IDENTIFY_CLUSTER_ID);

  if (ep == SL_ZIGBEE_AF_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_identify_cluster_println("ERR: invalid endpoint supplied for identification.");
    return;
  }

  sl_zigbee_af_identify_cluster_println("Starting identifying on endpoint 0x%x, identify time is %d sec",
                                        endpoint,
                                        identifyTime);

  identifyTable[ep] = true;

  // This initialization is needed because this callback is invoked in the
  // component init callback, so it may occur before this component init callback.
  sl_zigbee_af_event_init(provideFeedbackEventControl,
                          sl_zigbee_af_identify_feedback_provide_feedback_event_handler);
  sl_zigbee_af_event_set_delay_ms(provideFeedbackEventControl,
                                  MILLISECOND_TICKS_PER_SECOND);
}

void sl_zigbee_af_identify_stop_feedback_cb(uint8_t endpoint)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_IDENTIFY_CLUSTER_ID);
  uint8_t i;

  if (ep == SL_ZIGBEE_AF_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_identify_cluster_println("ERR: invalid endpoint supplied for identification.");
    return;
  }

  sl_zigbee_af_identify_cluster_println("Stopping identifying on endpoint 0x%x", endpoint);

  identifyTable[ep] = false;

  for (i = 0; i < SL_ZIGBEE_ZCL_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT; i++) {
    if (identifyTable[i]) {
      return;
    }
  }

  sl_zigbee_af_identify_cluster_println("No endpoints identifying; stopping identification feedback.");
  sl_zigbee_af_event_set_inactive(provideFeedbackEventControl);
}
