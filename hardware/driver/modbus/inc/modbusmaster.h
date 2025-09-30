#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_modbusmaster_config.h"

#define MODBUS_MAX_BUFFER 64

// Runtime debug flag for Modbus logs
extern int modbus_debug_flag;
void ModbusMaster_setDebug(int flag);

// User must call this once at startup
void ModbusMaster_begin(uint8_t slave_id);

// Buffer management functions
uint16_t ModbusMaster_getResponseBuffer(uint8_t index);
void ModbusMaster_clearResponseBuffer(void);
uint8_t ModbusMaster_setTransmitBuffer(uint8_t index, uint16_t value);
void ModbusMaster_clearTransmitBuffer(void);

// Modbus function codes
bool ModbusMaster_readHoldingRegisters(uint16_t reg, uint16_t count);
bool ModbusMaster_readInputRegisters(uint16_t reg, uint16_t count);

// Set RS485 DE/RE control pin (user must implement this for their board)
void ModbusMaster_setTransmit(bool enable);

// Set EUSART handle (user must provide sl_iostream_t* for their instance)
void ModbusMaster_setStream(void *stream);

// Get last error code (0=success, 1=timeout, 2=short response, 3=CRC error, 4=slave/function mismatch)
uint8_t ModbusMaster_getLastError(void);

#endif // MODBUSMASTER_H