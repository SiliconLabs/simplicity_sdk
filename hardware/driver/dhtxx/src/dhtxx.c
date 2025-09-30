/*
 * DHT11/DHT22 reader using DWT cycle counter for microsecond timing.
 * Measures pulse widths (in us) to distinguish 0 vs 1.
 * Supports both DHT11 and DHT22 sensors with pin tool configuration.
 */
#include "dhtxx.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_device.h"
#include "em_system.h"
#include <stdio.h>
#include "sl_dhtxx_config.h"

// Pin configuration from pin tool - cast to proper GPIO types
#define DHT_PORT ((GPIO_Port_TypeDef)SL_DHTXX_DATA_PORT)
#define DHT_PIN SL_DHTXX_DATA_PIN

// Sensor type from configuration
#define DHT_TYPE SL_DHTXX_SENSOR_TYPE

// Timing constants for DHT11 and DHT22
#define DHT11_START_SIGNAL_LOW_MS 20
#define DHT22_START_SIGNAL_LOW_MS 1
#define DHT_BIT_THRESHOLD_US 50

static uint32_t cycles_per_us;

static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    cycles_per_us = SystemCoreClock / 1000000U;
}

static inline uint32_t micros(void)
{
    return (uint32_t)(DWT->CYCCNT / (cycles_per_us ? cycles_per_us : 1));
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = micros();
    while ((micros() - start) < (ms * 1000U))
    {
        __NOP();
    }
}

void DHT_Init(void)
{

    dwt_init();
    GPIO_PinModeSet(DHT_PORT, DHT_PIN, gpioModeInputPull, 1);
}

static inline void dht_drive_low(void)
{
    GPIO_PinModeSet(DHT_PORT, DHT_PIN, gpioModePushPull, 0);
    GPIO_PinOutClear(DHT_PORT, DHT_PIN);
}

static inline void dht_release_input(void)
{
    GPIO_PinModeSet(DHT_PORT, DHT_PIN, gpioModeInputPull, 1);
}

static bool wait_level(uint32_t level, uint32_t timeout_us)
{
    uint32_t start = micros();
    while (GPIO_PinInGet(DHT_PORT, DHT_PIN) != level)
    {
        if ((micros() - start) > timeout_us)
            return false;
    }
    return true;
}

bool DHT_Read(DHT_Data_t *data)
{
    uint8_t bytes[5] = {0};
    const int max_retries = 3;

    dht_release_input();

    for (int attempt = 0; attempt < max_retries; ++attempt)
    {
        if (attempt > 0)
        {
            for (volatile int i = 0; i < 200000; ++i)
                __NOP();
        }

        dht_drive_low();
        // Use different start signal timing based on DHT type
        if (DHT_TYPE == DHT11)
            delay_ms(DHT11_START_SIGNAL_LOW_MS);
        else
            delay_ms(DHT22_START_SIGNAL_LOW_MS);

        GPIO_PinModeSet(DHT_PORT, DHT_PIN, gpioModePushPull, 1);
        for (volatile int i = 0; i < 80; ++i)
            __NOP();
        dht_release_input();

        if (!wait_level(0, 120))
        {
            continue;
        }
        if (!wait_level(1, 120))
        {
            continue;
        }

        bool ok = true;
        for (int b = 0; b < 5 && ok; ++b)
        {
            bytes[b] = 0;
            for (int bit = 0; bit < 8; ++bit)
            {
                if (!wait_level(0, 80))
                {
                    ok = false;
                    break;
                }
                if (!wait_level(1, 120))
                {
                    ok = false;
                    break;
                }
                uint32_t t0 = micros();
                if (!wait_level(0, 300))
                {
                    ok = false;
                    break;
                }
                uint32_t t_high = micros() - t0;
                if (t_high > DHT_BIT_THRESHOLD_US)
                    bytes[b] |= (1 << (7 - bit));
            }
        }
        if (!ok)
            continue;

        // Verify checksum
        uint8_t sum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
        if (sum != bytes[4])
            continue;

        // Parse data based on DHT type
        if (DHT_TYPE == DHT11)
        {
            // DHT11 format: integer and decimal parts are separate bytes
            data->humidity_int = bytes[0];
            data->humidity_dec = bytes[1];
            data->temperature_int = bytes[2];
            data->temperature_dec = bytes[3];
            data->checksum = bytes[4];
        }
        else // DHT22
        {
            // DHT22 format: 16-bit values with decimal scaling
            uint16_t rawHumidity = (bytes[0] << 8) | bytes[1];
            uint16_t rawTemp = (bytes[2] << 8) | bytes[3];

            // Handle negative temperatures for DHT22
            if (rawTemp & 0x8000)
            {
                rawTemp &= 0x7FFF;
                data->temperature_int = -(rawTemp / 10);
                data->temperature_dec = rawTemp % 10;
            }
            else
            {
                data->temperature_int = rawTemp / 10;
                data->temperature_dec = rawTemp % 10;
            }

            data->humidity_int = rawHumidity / 10;
            data->humidity_dec = rawHumidity % 10;
            data->checksum = bytes[4];
        }

        return true;
    }
    return false;
}

void DHT_Print(const DHT_Data_t *data)
{
    printf("Humidity: %d.%d%%\r\n", data->humidity_int, data->humidity_dec);
    printf("Temperature: %d.%dC\r\n", data->temperature_int, data->temperature_dec);
}

// Arduino-style API implementations
float DHT_ReadHumidity(void)
{
    DHT_Data_t data;
    if (DHT_Read(&data))
    {
        return (float)data.humidity_int + (data.humidity_dec / 10.0f);
    }
    return -1.0f; // error
}

float DHT_ReadTemperature(void)
{
    DHT_Data_t data;
    if (DHT_Read(&data))
    {
        return (float)data.temperature_int + (data.temperature_dec / 10.0f);
    }
    return -100.0f; // error
}

float DHT_ReadTemperatureF(void)
{
    float c = DHT_ReadTemperature();
    if (c > -99.0f)
    {
        return c * 1.8f + 32.0f;
    }
    return -100.0f; // error
}
