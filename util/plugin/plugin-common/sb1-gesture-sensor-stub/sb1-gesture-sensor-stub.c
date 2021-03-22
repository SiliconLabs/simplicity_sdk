//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "event_queue/event-queue.h"
#include "app/util/serial/command-interpreter2.h"

sli_zigbee_event_t sl_util_af_sb1_gesture_sensor_message_ready_event;

extern void sl_util_af_sb1_gesture_sensor_gesture_received_cb(
  uint8_t                                                                gesture,
  uint8_t                                                                buttonNum);

uint8_t halSb1GestureSensorMsgReady(void)
{
  return 0;
}

uint8_t halSb1GestureSensorCheckForMsg(void)
{
  return 0;
}

void sli_sb1_message_ready(void)
{
}

void sli_sb1_read_message(void)
{
}

void sl_util_af_sb1_gesture_sensor_message_ready_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_sb1_gesture_sensor_init_cb(void)
{
}

// ******************************************************************************
// plugin sb1 send-gest <gesture> <button>
// This will simulate a gesture being recognized on a button and cause a
// gesture received callback with matching parameters to be generated.  This
// function will perform no sanity checking to verify that the gesture or
// button are sane and exist on the attached sb1 gesture recognition sensor.
// ******************************************************************************
void sli_sb1_send_gesture(void)
{
  uint8_t gesture = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t button = (uint8_t)emberUnsignedCommandArgument(1);
  sl_util_af_sb1_gesture_sensor_gesture_received_cb(gesture, button);
}
