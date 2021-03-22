//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "event_queue/event-queue.h"

// Events
sli_zigbee_event_t sl_util_af_battery_monitor_read_adc_event;

uint16_t halGetBatteryVoltageMilliV(void)
{
  return 0;
}

void halBatteryMonitorInitialize(void)
{
}

void sl_util_af_battery_monitor_read_adc_event_handler(sli_zigbee_event_t *event)
{
}

void sl_util_af_battery_monitor_init_cb(void)
{
}
