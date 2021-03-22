// Copyright 2015 Silicon Laboratories, Inc.                                *80*
//
#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"

// ------------------------------------------------------------------------------
// Plugin events
sl_zigbee_event_control_t sl_util_af_power_meter_cs5463_init_event_control;
sl_zigbee_event_control_t sl_util_af_power_meter_cs5463_read_event_control;

// ------------------------------------------------------------------------------
// Plugin private event handlers
void sl_util_af_power_meter_cs5463_init_event_handler(void)
{
}

void sl_util_af_power_meter_cs5463_read_event_handler(void)
{
}

// ------------------------------------------------------------------------------
// Plugin defined callbacks
// The init callback, which will be called by the framework on init.
void sl_util_af_power_meter_cs5463_init_cb(void)
{
}

void halPowerMeterInit(void)
{
}

uint32_t halGetVrmsMilliV(void)
{
  return 0;
}

uint32_t halGetCrmsMilliA(void)
{
  return 0;
}

uint32_t halGetApparentPowerMilliW(void)
{
  return 0;
}

int32_t halGetActivePowerMilliW(void)
{
  return 0;
}

int8_t halGetPowerFactor(void)
{
  return 0;
}

bool halPowerMeterCalibrateCurrentGain(uint16_t referenceCurrentMa)
{
  return true;
}

void halSetCurrentGain(uint16_t currentGain)
{
}

int16_t halGetPowerMeterTempCentiC(void)
{
  return 0;
}

uint8_t halGetPowerMeterStatus(void)
{
  return 0;
}
