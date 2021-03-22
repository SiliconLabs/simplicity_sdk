/***************************************************************************//**
 * @file sl_wmbus_sensor_virtual_water_meter.h
 * @brief Header file for the Wireless M-Bus virtual water meter sensor.
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

#ifndef SL_WMBUS_SENSOR_VIRTUAL_WATER_METER_H
#define SL_WMBUS_SENSOR_VIRTUAL_WATER_METER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*******************************************************************************
 * @brief WMBUS Virtual Water Meter initialization function.
 *
 * @param None
 *
 * @returns sl_status_t:  SL_STATUS_OK if init was succesfully
 *                        SL_STATUS_FAIL if init failed
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_virtual_water_meter_init(void);

/*******************************************************************************
 * @brief WMBUS Virtual Water Meter deinitialization function.
 *
 * @param None
 *
 * @returns sl_status_t:  SL_STATUS_OK if init was succesfully
 *                        SL_STATUS_FAIL if init failed
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_virtual_water_meter_deinit(void);

/*******************************************************************************
 * @brief WMBUS Virtual Water Meter measure function.
 *
 * @param None
 *
 * @returns sl_status_t:  SL_STATUS_OK if measure was succesfully
 *                        SL_STATUS_FAIL if measure failed
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_virtual_water_meter_measure(void);

/*******************************************************************************
 * @brief Retrieves the sensor data for the WMBus virtual water meter sensor.
 *
 * This function returns a pointer to the sl_wmbus_sensor_data_t structure that contains
 * the sensor data for the WMBus virtual water meter sensor.
 *
 * @param None
 *
 * @returns sl_wmbus_sensor_data_t*: Pointer to the sensor data for the
 *                                   WMBus virtual water meter sensor
 ******************************************************************************/
sl_wmbus_sensor_data_t* sl_wmbus_sensor_virtual_water_meter_get_data(void);

/*******************************************************************************
 * @brief WMBUS Virtual Water Meter LCD print function.
 *
 * @param None
 *
 * @returns sl_status_t:  SL_STATUS_OK if LCD print was succesfully
 *                        SL_STATUS_FAIL if LCD print failed
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_virtual_water_meter_print(void);

/*******************************************************************************
 * @brief WMBUS Virtual Water Meter button pressed function.
 *
 * @param None
 *
 * @returns sl_status_t:  SL_STATUS_OK if button pressed was succesfully
 *                        SL_STATUS_FAIL if button pressed failed
 ******************************************************************************/
sl_status_t sl_wmbus_sensor_virtual_water_meter_button_pressed(void);

#endif // SL_WMBUS_SENSOR_VIRTUAL_WATER_METER_H
