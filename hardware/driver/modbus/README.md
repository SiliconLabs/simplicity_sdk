# ModbusMaster RTU Driver for Silicon Labs SDK

A professional Modbus RTU master driver for Silicon Labs SDK with pin tool integration, automatic debug control, and comprehensive statistics tracking.

## 🚀 Features

- **Pin Tool Integration**: Configure DE/RE pins and settings via GUI
- **Automatic Debug Control**: Enable/disable debug output through configuration wizard
- **RS485 Support**: Full DE/RE pin control for half-duplex communication
- **Error Handling**: Comprehensive error detection (timeout, CRC, response validation)
- **Float Decoding**: Built-in support for IEEE 754 float register pairs
- **Multiple Register Types**: Support for holding registers (0x03) and input registers (0x04)

## 📁 File Structure

```
hardware/driver/modbus/
├── inc/modbusmaster.h                 # API header with function declarations
├── src/modbusmaster.c                 # Driver implementation
├── config/sl_modbusmaster_config.h    # Pin Tool configuration template
├── test/modbus_config_test.c          # Configuration test example
├── README.md                          # This documentation
└── ../component/modbus_driver.slcc    # Silicon Labs component definition
```

## 🔧 Installation & Setup

### Step 1: Add Component to Project

1. Open **Simplicity Studio** and your project
2. Go to **Software Components**
3. Search for **"ModbusMaster RTU"** or **"modbus"**
4. Install the **"ModbusMaster RTU - RS485 Communication"** component

### Step 2: Configure Pin Tool

1. Open **Pin Tool** in your project
2. Navigate to **Peripherals** → **ModbusMaster**
3. Configure the following settings:

#### GPIO Configuration
- **DE/RE Port**: Select GPIO port (e.g., `gpioPortD`)
- **DE/RE Pin**: Select pin number (e.g., `2` for PD02)

#### Communication Settings
- **Timeout**: Response timeout in milliseconds (default: 1000ms)
- **Guard Time**: Inter-frame delay in milliseconds (default: 5ms)
- **Debug Enable**: Check to enable debug output (default: enabled)

### Step 3: Configure UART/EUSART

Ensure your project has a UART/EUSART instance configured for RS485 communication:

1. **Pin Tool** → **Peripherals** → **EUSART/UART**
2. Configure TX/RX pins for your RS485 transceiver
3. Set baud rate (typically 9600, 19200, or 38400 for Modbus)

## 📝 API Reference

### Initialization Functions

```c
void ModbusMaster_begin(uint8_t slave_id);
void ModbusMaster_setStream(sl_iostream_t *stream);
void ModbusMaster_setDebug(uint8_t enable);
```

### Communication Functions

```c
bool ModbusMaster_readHoldingRegisters(uint16_t address, uint16_t quantity);
bool ModbusMaster_readInputRegisters(uint16_t address, uint16_t quantity);
uint16_t ModbusMaster_getResponseBuffer(uint16_t index);
uint8_t ModbusMaster_getLastError();
```

## 💡 Usage Example

### Basic Setup

```c
#include "modbusmaster.h"
#include "sl_iostream_handles.h"

void app_init(void) {
    // Set up iostream for ModbusMaster
    ModbusMaster_setStream(sl_iostream_mikroe_handle);
    
    // Initialize ModbusMaster (automatic pin and debug setup)
    ModbusMaster_begin(1);  // Slave address 1
    
    printf("ModbusMaster initialized!\n");
}
```

### Reading Sensors

```c
void read_sensor_data(void) {
    // Read voltage from holding register 140 (2 registers for float)
    if (ModbusMaster_readHoldingRegisters(140, 2)) {
        // Decode IEEE 754 float from register pair
        uint16_t reg0 = ModbusMaster_getResponseBuffer(0);
        uint16_t reg1 = ModbusMaster_getResponseBuffer(1);
        
        uint32_t u = ((uint32_t)reg1 << 16) | reg0;
        float voltage;
        memcpy(&voltage, &u, sizeof(voltage));
        
        printf("Voltage: %.2f V\n", voltage);
    } else {
        printf("Failed to read voltage: Error %d\n", ModbusMaster_getLastError());
    }
}
```

### Error Handling

```c
void handle_modbus_errors(uint8_t error) {
    switch (error) {
        case 1: printf("Timeout error\n"); break;
        case 2: printf("Response too short\n"); break;
        case 3: printf("CRC error\n"); break;
        case 4: printf("Slave/function mismatch\n"); break;
        default: printf("Unknown error: %d\n", error); break;
    }
}
```

## 📊 Communication Monitoring

```c
void display_communication_info(void) {
    printf("=== Communication Status ===\n");
    uint8_t last_error = ModbusMaster_getLastError();
    
    if (last_error == 0) {
        printf("Last Transaction: SUCCESS\n");
    } else {
        printf("Last Transaction: ERROR (%d)\n", last_error);
        switch (last_error) {
            case 1: printf("  - Timeout error\n"); break;
            case 2: printf("  - Response too short\n"); break;
            case 3: printf("  - CRC error\n"); break;
            case 4: printf("  - Slave/function mismatch\n"); break;
            default: printf("  - Unknown error\n"); break;
        }
    }
}
```

## 🔍 Debug Output

When debug is enabled via Pin Tool configuration, the driver provides detailed logging:

```
[ModbusMaster] Debug ENABLED via pin tool (SL_MODBUSMASTER_DEBUG_ENABLE=1)
[ModbusMaster] Begin: Slave ID=1
[ModbusMaster] DE/RE pin configured: Port=C, Pin=1
[ModbusMaster] reg=140, count=2
[ModbusMaster] TX: 01 03 00 8C 00 02 05 F8
[ModbusMaster] RX: 01 03 04 43 65 33 33 7E 4F (9 bytes)
[ModbusMaster] CRC OK, transaction successful
```

## ⚡ Complete Application Example

<details>
<summary>Click to expand full app.c example for energy meter reading</summary>

```c
#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "sl_iostream_init_eusart_instances.h"
#include "sl_iostream_uart.h"
#include "modbusmaster.h"
#include <stdio.h>
#include <string.h>
#include "em_gpio.h"
#include "sl_sleeptimer.h"

static float modbus_decode_float(uint16_t reg0, uint16_t reg1)
{
  // Device sends [reg0][reg1] but float is word-swapped -> [reg1][reg0]
  uint32_t u = ((uint32_t)reg1 << 16) | reg0;
  float f;
  memcpy(&f, &u, sizeof(f));
  return f;
}

void app_task(void *args)
{
  (void)args;
  printf("App started\n");

  // ✅ REMOVE THIS - PIN CONTROL IS NOW HANDLED BY THE DRIVER
  // GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); // Not needed anymore!
  // printf("GPIO PD02 (PWM) configured as output\n");

  // Set up ModbusMaster to use Mikroe EUSART
  sl_iostream_uart_set_read_block(sl_iostream_uart_mikroe_handle, false);
  ModbusMaster_setStream(sl_iostream_mikroe_handle);

  // ModbusMaster_begin() automatically:
  // - Configures the DE/RE pin from pin tool settings
  // - Sets debug mode based on pin tool configuration
  ModbusMaster_begin(1);    // slave address 1

  printf("ModbusMaster initialized with pin tool configuration\n");

  struct
  {
    uint16_t address;
    const char *label;
    const char *unit;
  } sensors[] = {
      {140, "Voltage", "V"},
      {148, "Current", "A"},
      {156, "Frequency", "Hz"},
      {100, "Power", "W"},
      {158, "Energy", "kWh"},
      {116, "PF", ""},
  };

  uint32_t cycle_count = 0;

  while (1)
  {
    cycle_count++;
    printf("\n=== Reading Cycle #%lu ===\n", (unsigned long)cycle_count);

    for (unsigned int i = 0; i < sizeof(sensors) / sizeof(sensors[0]); i++)
    {
      bool success = ModbusMaster_readHoldingRegisters(sensors[i].address, 2);
      if (success)
      {
        float value = modbus_decode_float(
            ModbusMaster_getResponseBuffer(0),
            ModbusMaster_getResponseBuffer(1));
        char buf[64];
        snprintf(buf, sizeof(buf), "%s : %.2f %s\r\n",
                 sensors[i].label, value, sensors[i].unit);
        sl_iostream_write(sl_iostream_vcom_handle, buf, strlen(buf));
      }
      else
      {
        uint8_t err = ModbusMaster_getLastError();
        char err_msg[64];
        switch (err)
        {
        case 1:
          snprintf(err_msg, sizeof(err_msg), "ERROR: %s read timeout\r\n", sensors[i].label);
          break;
        case 2:
          snprintf(err_msg, sizeof(err_msg), "ERROR: %s response too short\r\n", sensors[i].label);
          break;
        case 3:
          snprintf(err_msg, sizeof(err_msg), "ERROR: %s CRC error\r\n", sensors[i].label);
          break;
        case 4:
          snprintf(err_msg, sizeof(err_msg), "ERROR: %s slave/function mismatch\r\n", sensors[i].label);
          break;
        default:
          snprintf(err_msg, sizeof(err_msg), "ERROR: %s unknown error (%d)\r\n", sensors[i].label, err);
          break;
        }
        sl_iostream_write(sl_iostream_vcom_handle, err_msg, strlen(err_msg));
      }

      // Small delay between sensor reads
      sl_sleeptimer_delay_millisecond(100);
    }

    sl_iostream_write(sl_iostream_vcom_handle, "==============..........================\r\n\r\n", 42);

    sl_sleeptimer_delay_millisecond(10000);
  }
}
```

</details>

## 🛠️ Configuration Options

### Pin Tool Configuration Wizard

The driver uses Silicon Labs Pin Tool Configuration Wizard with the following options:

| Setting | Description | Default | Range |
|---------|-------------|---------|-------|
| DE/RE Port | GPIO port for DE/RE control | gpioPortC | Any GPIO port |
| DE/RE Pin | GPIO pin number | 1 | 0-15 |
| Timeout | Response timeout | 1000ms | 100-10000ms |
| Guard Time | Inter-frame delay | 10ms | 1-100ms |
| Debug Enable | Enable debug output | No | Yes/No |

### Configuration File Structure

```c
// sl_modbusmaster_config.h
#define SL_MODBUSMASTER_DE_RE_PORT    SL_GPIO_PORT_C
#define SL_MODBUSMASTER_DE_RE_PIN     1
#define SL_MODBUSMASTER_TIMEOUT_MS    1000
#define SL_MODBUSMASTER_GUARD_TIME_MS 10
#define SL_MODBUSMASTER_DEBUG_ENABLE  0
```

## 🔧 Troubleshooting

### Common Issues

#### 1. Communication Failures
- **Check RS485 wiring**: Ensure A/B lines are correctly connected
- **Verify termination**: Add 120Ω resistors at both ends of RS485 bus  
- **Check baud rate**: Ensure master and slave have matching baud rates
- **Increase timeout**: Try longer timeout values for slow devices

#### 2. CRC Errors
- **Electrical noise**: Check for proper shielding and grounding
- **Baud rate mismatch**: Verify communication speed settings
- **Cable quality**: Use proper RS485 cable (twisted pair)

#### 3. No Response (Timeout Errors)
- **Slave address**: Verify the slave device address is correct
- **DE/RE timing**: Check if guard time needs to be increased
- **Physical connection**: Verify TX/RX and DE/RE pin connections

#### 4. Compilation Errors
- **Missing includes**: Ensure all required headers are included
- **Component not installed**: Verify ModbusMaster component is added to project
- **Pin Tool configuration**: Ensure Pin Tool has been configured and generated

### Debug Tips

1. **Enable Debug Output**: Set `SL_MODBUSMASTER_DEBUG_ENABLE=1` in Pin Tool
2. **Check Error Codes**: Use `ModbusMaster_getLastError()` for specific error information
3. **Verify Frame Format**: Debug output shows exact TX/RX bytes for analysis
4. **Test with Simple Reads**: Start with single register reads to verify basic communication

## 📈 Performance Guidelines

### Optimal Settings
- **Baud Rate**: 9600-38400 for most industrial applications
- **Timeout**: 1000-2000ms for reliable communication  
- **Guard Time**: 10-20ms between transactions
- **Error Handling**: Always check return values and error codes

### Memory Usage
- **RAM Usage**: ~50 bytes for buffers and state variables
- **Flash Usage**: ~2KB for driver code
- **Stack Usage**: Minimal (<50 bytes per function call)

## 📚 Additional Resources

- **Modbus RTU Specification**: [Modbus Organization](https://modbus.org/)
- **Silicon Labs SDK Documentation**: Available in Simplicity Studio

## 🤝 Support

For issues and questions:
1. Check the troubleshooting section above
2. Verify your Pin Tool configuration
3. Enable debug output for detailed diagnostics
4. Check Silicon Labs community forums

---

**Version**: 1.0.1  
**Compatibility**: Silicon Labs SDK v5.x  
**Last Updated**: September 2025

