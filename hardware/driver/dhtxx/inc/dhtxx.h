#ifndef DHTXX_H
#define DHTXX_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_dhtxx_config.h"

// DHT sensor types
typedef enum
{
    DHT_TYPE_DHT11 = 0,
    DHT_TYPE_DHT22 = 1
} DHT_Type_t;

typedef struct
{
    uint8_t humidity_int;
    uint8_t humidity_dec;
    int8_t temperature_int; // Changed to signed for DHT22 negative temperatures
    uint8_t temperature_dec;
    uint8_t checksum;
} DHT_Data_t;

// Backward compatibility typedef
typedef DHT_Data_t DHT11_Data_t;
typedef DHT_Data_t DHTXX_Data_t;

// Main API functions
void DHT_Init(void);
bool DHT_Read(DHT_Data_t *data);
void DHT_Print(const DHT_Data_t *data);

// Arduino-style API
float DHT_ReadHumidity(void);
float DHT_ReadTemperature(void);
float DHT_ReadTemperatureF(void);

// DHTXX API (same as DHT)

#endif // DHTXX_H
