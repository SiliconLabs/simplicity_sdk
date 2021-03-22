/***************************************************************************//**
 * @file
 * @brief CS Ranging Service
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

#ifndef CS_RAS_H
#define CS_RAS_H

#include "sl_common.h"
#include "sl_bgapi.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Macros

// RAS Service
// bbb99e70-fff7-46cf-abc7-2d32c71820f3
#define RAS_SERVICE_UUID                              { 0xf3, 0x20, 0x18, 0xc7, 0x32, 0x2d, 0xc7, 0xab, \
                                                        0xcf, 0x46, 0xf7, 0xff, 0x70, 0x9e, 0xb9, 0xbb }
// RAS Control Point Characteristic
// 929f95ec-1391-4303-9b49-3c22a418be89
#define RAS_CONTROL_POINT_CHARACTERISTIC_UUID         { 0x89, 0xbe, 0x18, 0xa4, 0x22, 0x3c, 0x49, 0x9b, \
                                                        0x03, 0x43, 0x91, 0x13, 0xec, 0x95, 0x9f, 0x92 }
// RAS Procedure Enable Data Characteristic
// 929f95ec-1391-4303-9b49-3c22a418be90
#define RAS_PROCEDURE_ENABLE_DATA_CHARACTERISTIC_UUID { 0x90, 0xbe, 0x18, 0xa4, 0x22, 0x3c, 0x49, 0x9b, \
                                                        0x03, 0x43, 0x91, 0x13, 0xec, 0x95, 0x9f, 0x92 }
// RAS Subevent Ranging Data Characteristic
// 929f95ec-1391-4303-9b49-3c22a418be91
#define RAS_SE_RANGING_DATA_CHARACTERISTIC_UUID       { 0x91, 0xbe, 0x18, 0xa4, 0x22, 0x3c, 0x49, 0x9b, \
                                                        0x03, 0x43, 0x91, 0x13, 0xec, 0x95, 0x9f, 0x92 }

// -------------------------------
// Enums, structs, typedefs

typedef enum {
  CS_RAS_OPCODE_RANGING_DATA_READY_INDICATION = 0x00,
  CS_RAS_OPCODE_RANGING_DATA_OVERWRITTEN_INDICATION  = 0x01,
  CS_RAS_OPCODE_RANGING_DATA_GET_COMMAND = 0x02,
  CS_RAS_OPCODE_RANGING_DATA_DELETE_COMMAND = 0x04,
  CS_RAS_OPCODE_INVALID = 0xFF,
} ras_opcode_t;

typedef struct {
  ras_opcode_t opcode;
  bool periodic_notification_mode_set_enabled;
} ras_control_point_parse_result;

typedef PACKSTRUCT (struct  {
  uint8_t opcode;
  uint16_t procedure_index;
  uint8_t number_of_subevents;
  uint16_t subevent_index;
  uint16_t subevent_index_data_size;
}) ras_ranging_data_ready_indication_t;

typedef PACKSTRUCT (struct {
  uint8_t opcode;
  uint16_t procedure_index;
  uint8_t subevent_index;
  uint16_t subevent_ranging_data_offset;
  uint8_t max_number_of_notifications;
  uint8_t keep_notification;
  uint8_t antenna_path_index_filter;
}) ras_ranging_data_get_command_t;

// -----------------------------------------------------------------------------
// Public functions

/**************************************************************************//**
 * CS RAS set periodic notification.
 * @param[in] state bool value to set/disable notification mode
 *****************************************************************************/
void cs_ras_periodic_notification_mode_set_enabled(bool state);

/**************************************************************************//**
 * CS RAS get periodic notification state.
 * @return notification state
 *****************************************************************************/
bool cs_ras_periodic_notification_mode_enabled(void);

/**************************************************************************//**
 * CS initiator init function that sets the first discovery and
 * initiator config and register the selected callback function pointer as a
 * callback for the distance measurement.
 * @param[in] message pointer to the RAS message
 * @param[in] message_len length of the message
 * @return ras_control_point_parse_result struct with the results
 *****************************************************************************/
ras_control_point_parse_result cs_ras_parse_control_point_message(uint8_t *message, uint8_t message_len);

/**************************************************************************//**
 * Creates a response message to the data received in the
 * RAS control point characteristic
 *
 * @param[in] data pointer to the data received in the Control Point
 * @param[in] len length of the data received in the Control Point
 * @param[in] enable_periodic_notification_mode set to true to enable periodic
 *            notifications in the response
 * @param[out] cmd pointer to the response message to be filled
 *
 * @return status code
 *****************************************************************************/
sl_status_t cs_ras_create_control_point_response(uint8_t *data,
                                                 uint8_t len,
                                                 bool enable_periodic_notification_mode,
                                                 ras_ranging_data_get_command_t *cmd);

#ifdef __cplusplus
};
#endif

#endif // CS_RAS_H
