# ModbusMaster RTU Driver for Silicon Labs SDK

A professional Modbus RTU master driver for Silicon Labs SDK with pin tool integration, automatic debug control, and comprehensive statistics tracking.

## 🚀 Features

- **Pin Tool Integration**: Configure DE/RE pins and settings via GUI
- **Automatic Debug Control**: Enable/disable debug output through configuration wizard
- **Statistics Tracking**: Monitor communication performance and efficiency  
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

### Statistics Functions

```c
uint32_t ModbusMaster_getTotalTransactions(void);
uint32_t ModbusMaster_getTotalBytesSent(void);
uint32_t ModbusMaster_getTotalBytesReceived(void);
void ModbusMaster_resetStatistics(void);
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

## 📊 Statistics Monitoring

```c
void display_communication_stats(void) {
    printf("=== Communication Statistics ===\n");
    printf("Total Transactions: %lu\n", ModbusMaster_getTotalTransactions());
    printf("Total Bytes Sent: %lu\n", ModbusMaster_getTotalBytesSent());
    printf("Total Bytes Received: %lu\n", ModbusMaster_getTotalBytesReceived());
    
    // Calculate efficiency
    uint32_t total = ModbusMaster_getTotalBytesSent() + ModbusMaster_getTotalBytesReceived();
    if (total > 0) {
        float efficiency = (float)(ModbusMaster_getTotalBytesReceived() * 100) / total;
        printf("Communication Efficiency: %.1f%%\n", efficiency);
    }
}
```

## 🔍 Debug Output

When debug is enabled via Pin Tool configuration, the driver provides detailed logging:

```
[ModbusMaster] Debug ENABLED via pin tool (SL_MODBUSMASTER_DEBUG_ENABLE=1)
[ModbusMaster] Begin: Slave ID=1
[ModbusMaster] DE/RE pin configured: Port=3, Pin=2
[ModbusMaster] readHoldingRegisters: addr=140, qty=2
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

  // ⭐ ModbusMaster_begin() now automatically:
  // - Configures the DE/RE pin (PD02) from pin tool settings
  // - Initializes statistics tracking
  // - Sets debug mode based on pin tool configuration
  ModbusMaster_begin(1);    // slave address 1

  // 🆕 OPTIONAL: Enable debug output (if not enabled via pin tool)
  // ModbusMaster_setDebug(1);  // Enable verbose debug logging

  printf("ModbusMaster initialized with enhanced features\n");

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

    // 🆕 DISPLAY COMMUNICATION STATISTICS
    printf("\n=== Communication Statistics ===\n");
    printf("Total Transactions: %lu\n", (unsigned long)ModbusMaster_getTotalTransactions());
    printf("Total Bytes Sent: %lu\n", (unsigned long)ModbusMaster_getTotalBytesSent());
    printf("Total Bytes Received: %lu\n", (unsigned long)ModbusMaster_getTotalBytesReceived());

    // Calculate efficiency
    uint32_t total_bytes = ModbusMaster_getTotalBytesSent() + ModbusMaster_getTotalBytesReceived();
    if (total_bytes > 0) {
      printf("Communication Efficiency: %.1f%% successful\n",
             (float)(ModbusMaster_getTotalBytesReceived() * 100) / total_bytes);
    }

    sl_iostream_write(sl_iostream_vcom_handle, "==============..........================\r\n\r\n", 42);

    // 🆕 OPTIONAL: Reset statistics every 10 cycles to prevent overflow
    if (cycle_count % 10 == 0) {
      printf("Resetting statistics after 10 cycles...\n");
      ModbusMaster_resetStatistics();
    }

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
| DE/RE Port | GPIO port for DE/RE control | gpioPortA | Any GPIO port |
| DE/RE Pin | GPIO pin number | 0 | 0-15 |
| Timeout | Response timeout | 1000ms | 100-10000ms |
| Guard Time | Inter-frame delay | 5ms | 1-100ms |
| Debug Enable | Enable debug output | Yes | Yes/No |

### Configuration File Structure

```c
// sl_modbusmaster_config.h
#define SL_MODBUSMASTER_DE_PORT       gpioPortD
#define SL_MODBUSMASTER_DE_PIN        2
#define SL_MODBUSMASTER_TIMEOUT_MS    1000
#define SL_MODBUSMASTER_GUARD_TIME_MS 5
#define SL_MODBUSMASTER_DEBUG_ENABLE  1
```

## 🔧 Troubleshooting

### Common Issues

#### 1. Communication Failures (Low Efficiency < 80%)
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
2. **Monitor Statistics**: Use statistics functions to track communication health
3. **Check Error Codes**: Use `ModbusMaster_getLastError()` for specific error information
4. **Verify Frame Format**: Debug output shows exact TX/RX bytes for analysis

## 📈 Performance Guidelines

### Optimal Settings
- **Baud Rate**: 9600-38400 for most industrial applications
- **Timeout**: 1000-2000ms for reliable communication  
- **Guard Time**: 5-10ms between transactions
- **Efficiency Target**: >95% for good communication quality

### Memory Usage
- **RAM Usage**: ~100 bytes for buffers and statistics
- **Flash Usage**: ~2KB for driver code
- **Stack Usage**: Minimal (<50 bytes per function call)

## 📚 Additional Resources

- **Modbus RTU Specification**: [Modbus Organization](https://modbus.org/)
- **Silicon Labs SDK Documentation**: Available in Simplicity Studio
- **RS485 Design Guide**: Check Silicon Labs application notes
- **Pin Tool User Guide**: Simplicity Studio documentation

## 🤝 Support

For issues and questions:
1. Check the troubleshooting section above
2. Verify your Pin Tool configuration
3. Enable debug output for detailed diagnostics
4. Check Silicon Labs community forums

---

**Version**: 1.0  
**Compatibility**: Silicon Labs SDK v5.x  
**License**: Silicon Labs License Agreement




https://stagb.in/emv1.1