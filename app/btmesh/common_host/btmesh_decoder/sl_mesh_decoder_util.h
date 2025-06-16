/***************************************************************************//**
 * @file sl_mesh_decoder_util.h
 * @brief Bluetooth Mesh Decode component utility header file
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_MESH_DECODER_UTIL_H
#define SL_MESH_DECODER_UTIL_H

// -----------------------------------------------------------------------------
// Includes
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
// Function declarations

/***************************************************************************//**
 * @brief Converts a sensor sampling function identifier into a
 *        human-readable string.
 *
 * @param[in] sampling_function The identifier of the sensor sampling function.
 * @return A constant character pointer to the string
 *         representation of the sampling function.
 ******************************************************************************/
const char* sl_mesh_decoder_util_get_sampling_function_str(uint8_t sampling_function);

/***************************************************************************//**
 * @brief Converts a sensor setting access value into a human-readable string.
 *
 * @param setting_access The sensor setting access value.
 * @return A constant character pointer to the string representation of the
 *         sensor setting access.
 ******************************************************************************/
const char* sl_mesh_decoder_util_get_sensor_setting_access_str(uint8_t setting_access);

/***************************************************************************//**
 * @brief Gets the size of the sensor data based on the property ID.
 *
 * This function retrieves the size of the sensor data associated with a given
 * property ID. The size is determined based on the property ID and is used to
 * interpret the sensor data correctly.
 *
 * @param property_id The property ID for which to get the sensor data size.
 * @return The size of the sensor data in bytes.
 ******************************************************************************/
uint8_t sl_mesh_decoder_util_get_sensor_data_size(uint16_t property_id);

/***************************************************************************//**
 * @brief Gets the string representation of a property ID.
 *
 * @param property_id The property ID for which to get the string representation.
 * @return A constant character pointer to the string representation of the
 *         property ID.
 ******************************************************************************/
const char* sl_mesh_decoder_util_get_property_str(uint16_t property_id);

#ifdef __cplusplus
}
#endif

#endif // SL_MESH_DECODER_UTIL_H
