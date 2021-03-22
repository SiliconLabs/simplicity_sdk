/***************************************************************************//**
 * @file sl_wmbus_sensor_core.c
 * @brief wMBus sensor core component
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

#ifndef SL_WMBUS_SENSOR_CORE_H
#define SL_WMBUS_SENSOR_CORE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "rail.h"
#include "sl_status.h"
#include "sl_wmbus_packet_assembler.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef sl_status_t (*sl_wmbus_sensor_init_t)();
typedef sl_status_t (*sl_wmbus_sensor_deinit_t)();
typedef sl_status_t (*sl_wmbus_sensor_measure_t)();
typedef sl_wmbus_sensor_data_t* (*sl_wmbus_sensor_get_data_t)();
typedef sl_status_t (*sl_wmbus_sensor_print_t)();
typedef sl_status_t (*sl_wmbus_sensor_button_pressed_t)();

/*******************************************************************************
 * @brief Structure representing a WMBus sensor.
 *
 * This structure contains function pointers to initialize the sensor, measure data,
 * get data, and print data on an LCD display.
 ******************************************************************************/
typedef struct {
  uint8_t id;                                               /**< Sensor ID */
  const char* name;                                         /**< Name of the sensor */
  sl_wmbus_sensor_init_t init_fnc_ptr;                      /**< Function pointer to initialize the sensor */
  sl_wmbus_sensor_init_t deinit_fnc_ptr;                    /**< Function pointer to deinitialize the sensor */
  sl_wmbus_sensor_measure_t measure_fnc_ptr;                /**< Function pointer to measure data */
  sl_wmbus_sensor_get_data_t get_data_fnc_ptr;              /**< Function pointer to get data */
  sl_wmbus_sensor_print_t print_fnc_ptr;                    /**< Function pointer to print data on an LCD display */
  sl_wmbus_sensor_button_pressed_t button_pressed_fnc_ptr;  /**< Function pointer to handle button press */
} sl_wmbus_sensor_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*******************************************************************************
 * @brief Get the active sensor.
 *
 * @param None
 *
 * @returns sl_wmbus_sensor_t*: Pointer to the active sensor
 ******************************************************************************/
sl_wmbus_sensor_t* sl_wmbus_sensor_core_get_active_sensor(void);

/*******************************************************************************
 * @brief Set the active sensor.
 *
 * @param id: Sensor ID
 *
 * @returns sl_status_t: SL_STATUS_OK if successful, SL_STATUS_INVALID_PARAMETER
 *                       if the sensor ID is invalid
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_core_set_active_sensor(uint8_t id);

/*******************************************************************************
 * @brief Get the list of available sensors.
 *
 * @param None
 *
 * @returns sl_wmbus_sensor_t*: Pointer to the list of available sensors
 ******************************************************************************/
sl_wmbus_sensor_t* sl_wmbus_sensor_core_list_avaiable_sensors(void);

/*******************************************************************************
 * @brief Get the number of available sensors.
 *
 * @param None
 *
 * @returns size_t: Number of available sensors
 ******************************************************************************/
size_t sl_wmbus_sensor_core_get_sensor_count(void);

/*******************************************************************************
 * @brief Initialize the WMBus sensor core.
 *
 * Calls the initialization function of the active sensor.
 *
 * @param None
 *
 * @return sl_status_t: SL_STATUS_OK if successful, SL_STATUS_FAILED if error
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_core_init(void);

/*******************************************************************************
 * @brief Initialize the WMBus sensor core LCD.
 *
 * @param None
 ******************************************************************************/
void sl_wmbus_sensor_core_lcd_init(void);

/*******************************************************************************
 * @brief Process the WMBus sensor core.
 *
 * Calls the packet assembler functions of the active sensor.
 * It also passes the tx_buffer, access_number, and WMBus accessibility to the packet assembler function.
 *
 * @param tx_buffer The buffer to store the transmitted data.
 * @param access_number The access number for the WMBus packet.
 * @return The result of the packet assembler function.
 ******************************************************************************/
uint16_t sl_wmbus_sensor_core_process(uint8_t* tx_buffer, uint8_t* access_number);

#endif // SL_WMBUS_SENSOR_CORE_H
