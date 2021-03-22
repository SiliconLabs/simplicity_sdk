/// Copyright 2017 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "../../plugin-common/eeprom-powerdown/eeprom-powerdown.h"

void sl_util_af_eeprom_powerdown_init_cb(void)
{
  sl_util_eeprom_power_down_init_cb();
}
