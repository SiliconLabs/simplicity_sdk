/***************************************************************************/ /**
                                                                               * @file sl_modbusmaster_config.h
                                                                               * @brief ModbusMaster Driver Configuration
                                                                               ******************************************************************************/
#ifndef SL_MODBUSMASTER_CONFIG_H
#define SL_MODBUSMASTER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> ModbusMaster Configuration

// <o SL_MODBUSMASTER_TIMEOUT_MS> Modbus Timeout (ms) <1-10000>
// <i> Default: 1000
#define SL_MODBUSMASTER_TIMEOUT_MS 1000

// <o SL_MODBUSMASTER_GUARD_TIME_MS> Guard Time (ms) <0-1000>
// <i> Default: 10
#define SL_MODBUSMASTER_GUARD_TIME_MS 2

// <q SL_MODBUSMASTER_DEBUG_ENABLE> Enable Debug Output
// <i> Default: 0
#define SL_MODBUSMASTER_DEBUG_ENABLE 0

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio> SL_MODBUSMASTER_DE_RE
// $[GPIO_SL_MODBUSMASTER_DE_RE]
#define SL_MODBUSMASTER_DE_RE_PORT SL_GPIO_PORT_C
#define SL_MODBUSMASTER_DE_RE_PIN 1
// [GPIO_SL_MODBUSMASTER_DE_RE]$
// <<< sl:end pin_tool >>>

#endif // SL_MODBUSMASTER_CONFIG_H
