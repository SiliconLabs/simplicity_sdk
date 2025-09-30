/*
 * ModbusMaster RTU Driver for Silicon Labs SDK
 * Supports reading holding registers and input registers with configurable DE/RE pin control
 */
#include "modbusmaster.h"
#include <string.h>
#include "sl_iostream.h"
#include <stdio.h>
#include "sl_sleeptimer.h"
#include "em_gpio.h"
#include "sl_status.h"
#include "sl_modbusmaster_config.h"

#define MODBUS_RTU_MAX_FRAME 256

// Use configured DE/RE pin from pin tool
#define MODBUS_DE_PORT SL_MODBUSMASTER_DE_RE_PORT
#define MODBUS_DE_PIN SL_MODBUSMASTER_DE_RE_PIN

int modbus_debug_flag = 0;
void ModbusMaster_setDebug(int flag) { modbus_debug_flag = flag; }

static uint8_t _slave_id = 1;
static void *_stream = NULL;
static uint8_t _last_error = 0;

#define MODBUS_MAX_BUFFER 64
static uint16_t _responseBuffer[MODBUS_MAX_BUFFER];
static uint16_t _transmitBuffer[MODBUS_MAX_BUFFER];

// Buffer management
uint16_t ModbusMaster_getResponseBuffer(uint8_t index)
{
    if (index < MODBUS_MAX_BUFFER)
        return _responseBuffer[index];
    return 0xFFFF;
}
void ModbusMaster_clearResponseBuffer(void)
{
    for (int i = 0; i < MODBUS_MAX_BUFFER; i++)
        _responseBuffer[i] = 0;
}
uint8_t ModbusMaster_setTransmitBuffer(uint8_t index, uint16_t value)
{
    if (index < MODBUS_MAX_BUFFER)
    {
        _transmitBuffer[index] = value;
        return 0;
    }
    return 2;
}
void ModbusMaster_clearTransmitBuffer(void)
{
    for (int i = 0; i < MODBUS_MAX_BUFFER; i++)
        _transmitBuffer[i] = 0;
}

// Decode Modbus 2x16-bit registers into a float (word-swapped IEEE754)
// static float modbus_decode_float(uint16_t reg0, uint16_t reg1)
//{
//    uint32_t u = ((uint32_t)reg1 << 16) | reg0;
//    float f;
//    memcpy(&f, &u, sizeof(f));
//    return f;
//}

void ModbusMaster_setStream(void *stream)
{
    _stream = stream;
}

void ModbusMaster_begin(uint8_t slave_id)
{
    _slave_id = slave_id;


    // Set debug mode from pin tool configuration
    ModbusMaster_setDebug(SL_MODBUSMASTER_DEBUG_ENABLE);
    if (SL_MODBUSMASTER_DEBUG_ENABLE)
    {
        printf("[ModbusMaster] Debug ENABLED via pin tool (SL_MODBUSMASTER_DEBUG_ENABLE=%d)\n", SL_MODBUSMASTER_DEBUG_ENABLE);
    }
    else
    {
        printf("[ModbusMaster] Debug DISABLED via pin tool (SL_MODBUSMASTER_DEBUG_ENABLE=%d)\n", SL_MODBUSMASTER_DEBUG_ENABLE);
    }

    // Initialize DE/RE pin as output, default to receive mode
    GPIO_PinModeSet(MODBUS_DE_PORT, MODBUS_DE_PIN, gpioModePushPull, 0);
}

uint8_t ModbusMaster_getLastError(void)
{
    return _last_error;
}

// Statistics functions


// RS485 DE/RE control using pin tool configuration
void ModbusMaster_setTransmit(bool enable)
{
    if (modbus_debug_flag)
    {
        printf("[RS485] Setting transmit: %s\n", enable ? "TRUE" : "FALSE");
    }
    if (enable)
    {
        GPIO_PinOutSet(MODBUS_DE_PORT, MODBUS_DE_PIN); // Transmit
    }
    else
    {
        GPIO_PinOutClear(MODBUS_DE_PORT, MODBUS_DE_PIN); // Receive
    }
}

// CRC16 calculation (Modbus RTU)
static uint16_t _crc16(const uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < len; pos++)
    {
        crc ^= (uint16_t)buf[pos];
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// Decode Modbus 2x16-bit registers into a float (word-swapped IEEE754)
static float _modbus_decode_float(uint16_t reg0, uint16_t reg1)
{
    uint32_t u = ((uint32_t)reg1 << 16) | reg0; // swap words
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

// Send and receive a Modbus RTU frame (function 0x03 only)
// Helper: check elapsed ms using sleeptimer
static uint32_t _elapsed_ms(uint32_t start_tick)
{
    uint32_t now = sl_sleeptimer_get_tick_count();
    uint32_t elapsed_ticks = now - start_tick; // safe wraparound
    return sl_sleeptimer_tick_to_ms(elapsed_ticks);
}

// Send and receive a Modbus RTU frame (function 0x03 only)
bool ModbusMaster_readHoldingRegisters(uint16_t reg, uint16_t count)
{
    ModbusMaster_clearResponseBuffer();
    extern void *sl_iostream_vcom_handle;
    char dbg_buf[128];

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf),
                 "[ModbusMaster] reg=%d, count=%d\n", reg, count);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    if (!_stream)
    {
        _last_error = 2;
        return false;
    }

    // Build request frame
    uint8_t req[8];
    req[0] = _slave_id;
    req[1] = 0x03;
    req[2] = reg >> 8;
    req[3] = reg & 0xFF;
    req[4] = count >> 8;
    req[5] = count & 0xFF;
    uint16_t crc = _crc16(req, 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Sending request: ");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        for (int i = 0; i < 8; ++i)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "%02X ", req[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        snprintf(dbg_buf, sizeof(dbg_buf), "\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    // Drain any stale RX bytes before sending (avoid echo/old data)
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Flushing stale RX before TX...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    {
        uint8_t dump;
        size_t n = 0;
        while (sl_iostream_read(_stream, &dump, 1, &n) == SL_STATUS_OK && n == 1)
        {
            // discard
        }
    }

    // Send request
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Setting transmit mode...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    ModbusMaster_setTransmit(true);
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Writing to stream...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    sl_iostream_write(_stream, req, 8);
    for (volatile int i = 0; i < 50000; i++)
        ; // short delay
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Setting receive mode...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    ModbusMaster_setTransmit(false);
    // Guard time after DE low (~2 char times at 9600 bps)
    sl_sleeptimer_delay_millisecond(SL_MODBUSMASTER_GUARD_TIME_MS);

    // Receive loop with timeout
    uint8_t resp[MODBUS_RTU_MAX_FRAME];
    uint16_t resp_len = 0;
    bool got_response = false;
    int expected_len = 0;
    uint32_t timeout_ms = SL_MODBUSMASTER_TIMEOUT_MS;
    uint32_t start = sl_sleeptimer_get_tick_count();

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Receive mode set, waiting...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Waiting for response...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    while (resp_len < MODBUS_RTU_MAX_FRAME && _elapsed_ms(start) < timeout_ms)
    {
        uint8_t c;
        size_t bytes_read = 0;
        sl_status_t st = sl_iostream_read(_stream, &c, 1, &bytes_read);

        if (st == SL_STATUS_OK && bytes_read == 1)
        {
            // Enforce Modbus RTU header to avoid matching our own echo
            if (resp_len == 0)
            {
                if (c != _slave_id)
                {
                    // ignore junk byte
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                }
            }
            else if (resp_len == 1)
            {
                if (c != 0x03)
                {
                    // not the expected function, resync
                    resp_len = 0;
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                }
            }
            else if (resp_len == 2)
            {
                uint8_t expected_bytes = (uint8_t)(count * 2);
                if (c != expected_bytes)
                {
                    // third byte in a valid response is byte count; if not matching, resync
                    resp_len = 0;
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                    expected_len = c + 5; // addr + func + bytecount + data + CRC(2)
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Expected total bytes: %d\n", expected_len);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                }
            }
            else
            {
                // normal payload/CRC accumulation
                resp[resp_len++] = c;
                got_response = true;
                if (modbus_debug_flag)
                {
                    snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                    sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                }
                if (expected_len > 0 && resp_len >= expected_len)
                {
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Full response received (%d bytes)\n", resp_len);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                    break;
                }
            }
        }
        else
        {
            // No data yet, small delay to prevent tight loop
            sl_sleeptimer_delay_millisecond(SL_MODBUSMASTER_GUARD_TIME_MS);
        }
    }

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] LOOP END\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] === RESPONSE LOOP COMPLETE ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] === PRINTING RECEIVED BYTES ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Received bytes: ");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        for (int i = 0; i < (int)resp_len; ++i)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "%02X ", resp[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        snprintf(dbg_buf, sizeof(dbg_buf), "\n[ModbusMaster] === BYTES PRINTED ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    if (resp_len < 5)
    {
        uint32_t elapsed = _elapsed_ms(start);
        if (!got_response)
        {
            if (modbus_debug_flag)
            {
                snprintf(dbg_buf, sizeof(dbg_buf),
                         "[ModbusMaster] ERROR: Timeout/no response (elapsed=%lu ms, resp_len=%u)\n",
                         (unsigned long)elapsed, (unsigned)resp_len);
                sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
            }
            _last_error = 1; // timeout
        }
        else
        {
            if (modbus_debug_flag)
            {
                snprintf(dbg_buf, sizeof(dbg_buf),
                         "[ModbusMaster] ERROR: Short/partial frame (elapsed=%lu ms, resp_len=%u)\n",
                         (unsigned long)elapsed, (unsigned)resp_len);
                sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
            }
            _last_error = 2; // short frame
        }
        return false;
    }

    // CRC check
    uint16_t crc_resp = resp[resp_len - 2] | (resp[resp_len - 1] << 8);
    uint16_t crc_calc = _crc16(resp, resp_len - 2);
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] CRC received: 0x%04X, calculated: 0x%04X\n", crc_resp, crc_calc);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    if (crc_calc != crc_resp)
    {
        _last_error = 3;
        if (modbus_debug_flag)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] ERROR: CRC mismatch. Calc: 0x%04X, Recv: 0x%04X\n", crc_calc, crc_resp);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        return false;
    }

    if (resp[0] != _slave_id || resp[1] != 0x03)
    {
        _last_error = 4;
        if (modbus_debug_flag)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] ERROR: Slave or function mismatch. Expected: slave=%d func=0x03, Got: slave=%d func=0x%02X\n",
                     _slave_id, resp[0], resp[1]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        return false;
    }

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Parsing %d registers:\n", count);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    for (int i = 0; i < count; i++)
    {
        _responseBuffer[i] = (resp[3 + 2 * i] << 8) | resp[4 + 2 * i];
        if (modbus_debug_flag)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Register %d: 0x%04X (%d)\n", i, _responseBuffer[i], _responseBuffer[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
    }
    if (modbus_debug_flag && count == 2)
    {
        float value = _modbus_decode_float(_responseBuffer[0], _responseBuffer[1]);
        int rounded = (int)(value + 0.5f);
        snprintf(dbg_buf, sizeof(dbg_buf), ">>> Decoded float value: %.4f\n", value);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), ">>> Rounded integer value: %d\n", rounded);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    _last_error = 0;
    return true;
}

// Send and receive a Modbus RTU frame (function 0x04: input registers)
bool ModbusMaster_readInputRegisters(uint16_t reg, uint16_t count)
{
    ModbusMaster_clearResponseBuffer();
    extern void *sl_iostream_vcom_handle;
    char dbg_buf[128];

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf),
                 "[ModbusMaster] reg=%d, count=%d\n", reg, count);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    if (!_stream)
    {
        _last_error = 2;
        return false;
    }

    // Build request
    uint8_t req[8];
    req[0] = _slave_id;
    req[1] = 0x04;
    req[2] = reg >> 8;
    req[3] = reg & 0xFF;
    req[4] = count >> 8;
    req[5] = count & 0xFF;
    uint16_t crc = _crc16(req, 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Sending request: ");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        for (int i = 0; i < 8; ++i)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "%02X ", req[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        snprintf(dbg_buf, sizeof(dbg_buf), "\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    // Drain any stale RX bytes before sending (avoid echo/old data)
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Flushing stale RX before TX...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    {
        uint8_t dump;
        size_t n = 0;
        while (sl_iostream_read(_stream, &dump, 1, &n) == SL_STATUS_OK && n == 1)
        {
            // discard
        }
    }

    // Send request
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Setting transmit mode...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    ModbusMaster_setTransmit(true);
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Writing to stream...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    sl_iostream_write(_stream, req, 8);

    for (volatile int i = 0; i < 50000; i++)
        ;
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Setting receive mode...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    ModbusMaster_setTransmit(false);
    // Guard time after DE low (~2 char times at 9600 bps)
    sl_sleeptimer_delay_millisecond(SL_MODBUSMASTER_GUARD_TIME_MS);

    // Receive loop
    uint8_t resp[MODBUS_RTU_MAX_FRAME];
    uint16_t resp_len = 0;
    bool got_response = false;
    int expected_len = 0;
    uint32_t timeout_ms = SL_MODBUSMASTER_TIMEOUT_MS;
    uint32_t start = sl_sleeptimer_get_tick_count();

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Receive mode set, waiting...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Waiting for response...\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    while (resp_len < MODBUS_RTU_MAX_FRAME && _elapsed_ms(start) < timeout_ms)
    {
        uint8_t c;
        size_t bytes_read = 0;
        sl_status_t st = sl_iostream_read(_stream, &c, 1, &bytes_read);

        if (st == SL_STATUS_OK && bytes_read == 1)
        {
            if (resp_len == 0)
            {
                if (c != _slave_id)
                {
                    // ignore junk byte
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                }
            }
            else if (resp_len == 1)
            {
                if (c != 0x04)
                {
                    resp_len = 0; // resync
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                }
            }
            else if (resp_len == 2)
            {
                uint8_t expected_bytes = (uint8_t)(count * 2);
                if (c != expected_bytes)
                {
                    resp_len = 0; // resync
                }
                else
                {
                    resp[resp_len++] = c;
                    got_response = true;
                    expected_len = c + 5;
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Expected total bytes: %d\n", expected_len);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                }
            }
            else
            {
                resp[resp_len++] = c;
                got_response = true;
                if (modbus_debug_flag)
                {
                    snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Got byte %d: 0x%02X\n", resp_len, c);
                    sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                }
                if (expected_len > 0 && resp_len >= expected_len)
                {
                    if (modbus_debug_flag)
                    {
                        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Full response received (%d bytes)\n", resp_len);
                        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
                    }
                    break;
                }
            }
        }
        else
        {
            // No data yet, small delay to prevent tight loop
            sl_sleeptimer_delay_millisecond(SL_MODBUSMASTER_GUARD_TIME_MS);
        }
    }

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] LOOP END\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] === RESPONSE LOOP COMPLETE ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] === PRINTING RECEIVED BYTES ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Received bytes: ");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        for (int i = 0; i < (int)resp_len; ++i)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "%02X ", resp[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        snprintf(dbg_buf, sizeof(dbg_buf), "\n[ModbusMaster] === BYTES PRINTED ===\n");
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }

    if (resp_len < 5)
    {
        uint32_t elapsed = _elapsed_ms(start);
        if (!got_response)
        {
            if (modbus_debug_flag)
            {
                snprintf(dbg_buf, sizeof(dbg_buf),
                         "[ModbusMaster] ERROR: Timeout/no response (elapsed=%lu ms, resp_len=%u)\n",
                         (unsigned long)elapsed, (unsigned)resp_len);
                sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
            }
            _last_error = 1; // timeout
        }
        else
        {
            if (modbus_debug_flag)
            {
                snprintf(dbg_buf, sizeof(dbg_buf),
                         "[ModbusMaster] ERROR: Short/partial frame (elapsed=%lu ms, resp_len=%u)\n",
                         (unsigned long)elapsed, (unsigned)resp_len);
                sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
            }
            _last_error = 2; // short frame
        }
        return false;
    }

    uint16_t crc_resp = resp[resp_len - 2] | (resp[resp_len - 1] << 8);
    uint16_t crc_calc = _crc16(resp, resp_len - 2);
    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] CRC received: 0x%04X, calculated: 0x%04X\n", crc_resp, crc_calc);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    if (crc_calc != crc_resp)
    {
        _last_error = 3;
        if (modbus_debug_flag)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] ERROR: CRC mismatch. Calc: 0x%04X, Recv: 0x%04X\n", crc_calc, crc_resp);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
        return false;
    }


    if (resp[0] != _slave_id || resp[1] != 0x04)
    {
        _last_error = 4;
        return false;
    }

    if (modbus_debug_flag)
    {
        snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Parsing %d registers:\n", count);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    for (int i = 0; i < count; i++)
    {
        _responseBuffer[i] = (resp[3 + 2 * i] << 8) | resp[4 + 2 * i];
        if (modbus_debug_flag)
        {
            snprintf(dbg_buf, sizeof(dbg_buf), "[ModbusMaster] Register %d: 0x%04X (%d)\n", i, _responseBuffer[i], _responseBuffer[i]);
            sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        }
    }
    if (modbus_debug_flag && count == 2)
    {
        float value = _modbus_decode_float(_responseBuffer[0], _responseBuffer[1]);
        int rounded = (int)(value + 0.5f);
        snprintf(dbg_buf, sizeof(dbg_buf), ">>> Decoded float value: %.4f\n", value);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
        snprintf(dbg_buf, sizeof(dbg_buf), ">>> Rounded integer value: %d\n", rounded);
        sl_iostream_write(sl_iostream_vcom_handle, dbg_buf, strlen(dbg_buf));
    }
    _last_error = 0;
    return true;
}
