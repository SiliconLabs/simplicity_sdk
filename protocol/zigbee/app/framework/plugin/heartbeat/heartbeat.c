/***************************************************************************//**
 * @file
 * @brief Simple code to blink a light to simulate a heartbeat.
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

#include "app/framework/include/af.h"
#include PLATFORM_HEADER

#include "heartbeat-config.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_simple_led_instances.h"
#else
// TODO: EMZIGBEE-6783
  #error "LED not properly instantiated"
#endif

static sl_zigbee_af_event_t heartbeat_event;
static sl_led_t *heartbeat_led;

static void heartbeat_event_handler(sl_zigbee_af_event_t *event)
{
  sl_led_toggle(heartbeat_led);
  sl_zigbee_af_event_set_delay_qs(&heartbeat_event,
                                  SL_ZIGBEE_AF_PLUGIN_HEARTBEAT_PERIOD_QS);
}

void sli_zigbee_af_heartbeat_init_callback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      sl_zigbee_af_event_init(&heartbeat_event, heartbeat_event_handler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_DONE:
    {
      // TODO: EMZIGBEE-6783
      heartbeat_led = (sl_led_t *)&sl_led_led0;
      sl_zigbee_af_event_set_active(&heartbeat_event);
      break;
    }
  }
}
