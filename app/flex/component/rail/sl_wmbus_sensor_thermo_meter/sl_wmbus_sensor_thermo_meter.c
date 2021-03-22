/***************************************************************************/ /**
 * @file sl_wmbus_sensor_thermo_meter.c
 * @brief Wireless M-Bus thermometer sensor implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_wmbus_sensor_core.h"
#include "sl_wmbus_sensor_thermo_meter.h"
#include "sl_wmbus_sensor_thermo_meter_config.h"
#include "sl_wmbus_support.h"
#include "sl_sleeptimer.h"
#include "sl_si70xx.h"
#include "sl_i2cspm_instances.h"
#include "app_assert.h"
#include "sl_wmbus_packet_assembler.h"

#if defined(SL_CATALOG_SEGMENT_LCD_DRIVER_PRESENT)
#include "sl_segmentlcd.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Enum for LCD print change
typedef enum {
  LCD_PRINT_TEMPERATURE,
  LCD_PRINT_HUMIDITY
} lcd_print_change_t;

/*******************************************************************************
 * @brief Value of the VIF (Value Information Field) for the WMBus sensor thermometer.
 *
 * Defines the value of the VIF for the WMBus sensor thermometer. The VIF is a field
 * in the WMBus frame that provides information about the type of measurement being transmitted.
 * In this case, the VIF value is set to 0x58 what means Celsius degree.
 ******************************************************************************/
#define SL_WMBUS_SENSOR_THERMO_METER_VIF_VALUE 0x58

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static void sl_wmbus_sensor_thermo_meter_sleeptimer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// Temerature
static int32_t temp_data = 0;
// Humidity
static uint32_t rh_data = 0;
/// Periodic Timer Handle
static sl_sleeptimer_timer_handle_t timer_handle;
/// Print temperature or humidity
static lcd_print_change_t lcd_print_type = LCD_PRINT_TEMPERATURE;

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Sleeptimer callback for the thermo meter functionality. Periodically:
 * - Measure temperature and humidity
 * - Update LCD
 *
 * @param handle Not used
 * @param data Not used
 * @returns None
 *****************************************************************************/
static void sl_wmbus_sensor_thermo_meter_sleeptimer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  (void)data;
  sl_wmbus_sensor_thermo_meter_measure();
}

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * WMBUS Thermometer initialization function.
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_thermo_meter_init(void)
{
  uint8_t device_id = 0;
  sl_status_t status = SL_STATUS_FAIL;
  // init temperature sensor
  if (!sl_si70xx_present(sl_i2cspm_sensor, SI7021_ADDR, &device_id)) {
    // wait a bit before re-trying
    // the si7021 sensor can take up to 80 ms (25 ms @25 deg C) to start up
    sl_sleeptimer_delay_millisecond(80);
    // init temperature sensor (2nd attempt)
    if (!sl_si70xx_present(sl_i2cspm_sensor, SI7021_ADDR, &device_id)) {
      return SL_STATUS_FAIL;
    }
  }

  status = sl_sleeptimer_start_periodic_timer_ms(&timer_handle,
                                                 SL_WMBUS_SENSOR_THERMO_METER_SLEEPTIMER_TIMEOUT,
                                                 sl_wmbus_sensor_thermo_meter_sleeptimer_callback,
                                                 NULL,
                                                 SL_WMBUS_SENSOR_THERMO_METER_SLEEPTIMER_PRITORITY,
                                                 0);
  app_assert_status_f(status,
                      "[E: 0x%04x]: Failed to start periodic sleeptimer\n",
                      (int)status);
  status = sl_wmbus_sensor_thermo_meter_measure();
  return status;
}

/*******************************************************************************
 * WMBUS Thermometer deinitialization function.
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_thermo_meter_deinit(void)
{
  return sl_sleeptimer_stop_timer(&timer_handle);
}

/*******************************************************************************
 * WMBUS Thermometer measure function function. Read sensor data.
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_thermo_meter_measure(void)
{
  if (!sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor,
                                     SI7021_ADDR,
                                     &rh_data,
                                     &temp_data)) {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 * WMBUS Thermometer packet assembrel function.
 ******************************************************************************/
sl_wmbus_sensor_data_t* sl_wmbus_sensor_thermo_meter_get_data(void)
{
  // Sensor data for the thermo meter
  static sl_wmbus_sensor_data_t sensor_data_thermo_meter = {
    .data = 0,
    .vif = SL_WMBUS_SENSOR_THERMO_METER_VIF_VALUE
  };
  sensor_data_thermo_meter.data = temp_data;
  return &sensor_data_thermo_meter;
}

/*******************************************************************************
 * WMBUS Thermometer LCD print function.
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_thermo_meter_print(void)
{
#if defined(SL_CATALOG_SEGMENT_LCD_DRIVER_PRESENT)
  if (lcd_print_type == LCD_PRINT_TEMPERATURE) {
    sl_segment_lcd_temp_display(temp_data);
  } else {
    sl_segment_lcd_number(rh_data);
    sl_segment_lcd_symbol(SL_LCD_SYMBOL_P2, 1);
  }
#endif
  return SL_STATUS_OK;
}

/*******************************************************************************
 * WMBUS Thermometer button pressed function.
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_thermo_meter_button_pressed(void)
{
  if (lcd_print_type == LCD_PRINT_TEMPERATURE) {
    lcd_print_type = LCD_PRINT_HUMIDITY;
  } else {
    lcd_print_type = LCD_PRINT_TEMPERATURE;
  }
  return SL_STATUS_OK;
}
