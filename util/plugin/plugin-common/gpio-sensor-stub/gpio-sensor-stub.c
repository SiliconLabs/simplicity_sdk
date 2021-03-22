//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "event_queue/event-queue.h"

sli_zigbee_event_t sl_util_af_gpio_sensor_interrupt_event;
sli_zigbee_event_t sl_util_af_gpio_sensor_debounce_event;

void sl_util_af_gpio_sensor_interrupt_event_handler(sli_zigbee_event_t* event)
{
}

void sl_util_af_gpio_sensor_debounce_event_handler(sli_zigbee_event_t* event)
{
}

typedef enum {
  HAL_GPIO_SENSOR_ACTIVE = 0x01,
  HAL_GPIO_SENSOR_NOT_ACTIVE = 0x00,
} HalGpioSensorState;

void halGpioSensorInitialize(void)
{
}

HalGpioSensorState halGpioSensorGetSensorValue(void)
{
  return HAL_GPIO_SENSOR_ACTIVE;
}

void sl_util_af_gpio_sensor_init_cb(void)
{
}
