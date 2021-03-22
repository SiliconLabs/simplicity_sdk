//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "event_queue/event-queue.h"

sli_zigbee_event_t sl_util_af_button_interface_button0_pressed_event;
sli_zigbee_event_t sl_util_af_button_interface_button0_released_event;
sli_zigbee_event_t sl_util_af_button_interface_button1_pressed_event;
sli_zigbee_event_t sl_util_af_button_interface_button1_released_event;
sli_zigbee_event_t sl_util_af_button_interface_button_timeout_event;

void sl_util_af_button_interface_button_timeout_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_button_interface_button0_pressed_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_button_interface_button0_released_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_button_interface_button1_pressed_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_button_interface_button1_released_event_handler(sli_zigbee_event_t* event)
{
}

typedef enum {
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_LO = 0x00,
  EMBER_AF_BUTTON_INTERFACE_POLARITY_ACTIVE_HI = 0x01,
} HalButtonInterfacePolarity;

uint8_t halPluginButtonInterfaceButtonPoll(uint8_t button)
{
}

void halPluginButtonInterfaceSetButtonPolarity(
  uint8_t                    button,
  HalButtonInterfacePolarity polarity)
{
}

void sl_util_af_hal_button_isr_cb(uint8_t button, uint8_t state)
{
}
