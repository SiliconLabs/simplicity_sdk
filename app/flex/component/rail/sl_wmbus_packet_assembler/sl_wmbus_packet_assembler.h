/***************************************************************************//**
 * @file sl_wmbus_packet_assembler.h
 * @brief WMBus packet assembler.
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

#ifndef SL_WMBUS_PACKET_ASSEMBLER_H
#define SL_WMBUS_PACKET_ASSEMBLER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "sl_wmbus_support.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/*******************************************************************************
 * @brief Structure representing a WMBus sensor data.
 *
 * This structure contains the value information field and the data field.
 ******************************************************************************/
typedef struct sl_wmbus_sensor_data{
  uint32_t vif;
  uint32_t data;
  struct sl_wmbus_sensor_data* next;
} sl_wmbus_sensor_data_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/*******************************************************************************
 * @brief Sets up a WMBus frame.
 *
 * Sets up a WMBus frame by populating the provided buffer with the necessary information.
 *
 * @param buffer The buffer to store the WMBus frame.
 * @param access_number The access number of the frame.
 * @param accessibility The accessibility of the frame.
 * @param new_device_type The device type of the frame.
 * @param data An array of data to be included in the frame.
 * @param data_length The length of the data array.
 * @param vif The VIF (Value Information Field) of the frame.
 * @param periodic Indicates whether the frame is periodic or not.
 * @param encrypt Indicates whether the frame should be encrypted or not.
 * @return The length of the assembled frame.
 ******************************************************************************/
uint16_t sl_wmbus_setup_frame(uint8_t *buffer, uint8_t access_number, WMBUS_accessibility_t accessibility, WMBUS_device_type_t new_device_type, sl_wmbus_sensor_data_t* sensors_data, bool periodic, bool encrypt);

#endif // SL_WMBUS_PACKET_ASSEMBLER_H
