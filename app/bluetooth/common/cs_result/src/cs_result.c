/***************************************************************************//**
 * @file
 * @brief CS result handler implementation.
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

// -----------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include <stdbool.h>
#include "sl_bt_api.h"
#include "sl_component_catalog.h"

#include "cs_result.h"

#include "cs_result_config.h"

#if defined(SL_CATALOG_APP_LOG_PRESENT) && CS_RESULT_LOG
#include "app_log.h"
#define PREFIX                  CS_RESULT_LOG_PREFIX " "
#define NL                      APP_LOG_NL
#define result_log_wrap(code)   code
#else
#define result_log_wrap(...)
#endif // defined(SL_CATALOG_APP_LOG_PRESENT) && CS_RESULT_LOG

// -----------------------------------------------------------------------------
// Macros

#define result_log_debug(...)    result_log_wrap(app_log_debug(PREFIX  __VA_ARGS__))
#define result_log_info(...)     result_log_wrap(app_log_info(PREFIX  __VA_ARGS__))
#define result_log_warning(...)  result_log_wrap(app_log_warning(PREFIX  __VA_ARGS__))
#define result_log_error(...)    result_log_wrap(app_log_error(PREFIX  __VA_ARGS__))
#define result_log_critical(...) result_log_wrap(app_log_critical(PREFIX  __VA_ARGS__))

// -----------------------------------------------------------------------------
// Internal function definitions

/******************************************************************************
 * Get field type size in bytes.
 *****************************************************************************/
uint8_t cs_result_get_type_length(cs_result_field_type_t field)
{
  switch (field) {
    case CS_RESULT_FIELD_DISTANCE_MAINMODE:
    case CS_RESULT_FIELD_DISTANCE_SUBMODE:
    case CS_RESULT_FIELD_DISTANCE_RAW_MAINMODE:
    case CS_RESULT_FIELD_DISTANCE_RAW_SUBMODE:
    case CS_RESULT_FIELD_LIKELINESS_SUBMODE:
    case CS_RESULT_FIELD_LIKELINESS_MAINMODE:
    case CS_RESULT_FIELD_DISTANCE_RSSI:
    case CS_RESULT_FIELD_VELOCITY_MAINMODE:
    case CS_RESULT_FIELD_VELOCITY_SUBMODE:
    case CS_RESULT_FIELD_BIT_ERROR_RATE:
      return sizeof(float);
    default:
      result_log_error("Unknown field type: 0x%x!" NL, field);
      return 0;
  }
}

/******************************************************************************
 * Initialize result data.
 *****************************************************************************/
void cs_result_initialize_results_data(cs_result_session_data_t *result_data)
{
  result_data->type_count = 0;
  result_data->last_type = 0;
  result_data->write_pos = 0;
  result_data->size = 0;
  result_log_debug("result data initialized!" NL);
}

/******************************************************************************
 * Append field type and value to the result sequence.
 *****************************************************************************/
sl_status_t cs_result_append_field(cs_result_session_data_t *result_data,
                                   cs_result_field_type_t target,
                                   uint8_t *target_value,
                                   uint8_t *buffer)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check if the new field fits into the result buffer or the type is valid
  result_log_debug("result_data->write_pos: %u, result_data->size: %u " NL,
                   result_data->write_pos,
                   result_data->size);
  if (result_data->write_pos + cs_result_get_type_length(target) + sizeof(cs_result_field_type_t) > CS_RESULT_MAX_BUFFER_SIZE) {
    sc = SL_STATUS_FULL;
    result_log_error("failed to add type 0x%x! Result buffer is full! "
                     "[result_data->write_pos: %u, buffer_size: %u, sc: 0x%x]" NL,
                     target,
                     (unsigned char)(result_data->write_pos + cs_result_get_type_length(target) + sizeof(cs_result_field_type_t)),
                     CS_RESULT_MAX_BUFFER_SIZE,
                     (unsigned int)sc);
  } else if (result_data->last_type > target) {
    sc = SL_STATUS_INVALID_TYPE;
    result_log_error("failed to add type 0x%x! The types must be in ascending order! "
                     "[last_type: 0x%x, sc: 0x%x]" NL,
                     target,
                     result_data->last_type,
                     (unsigned int)sc);
  } else {
    // Add the field type to the result buffer
    buffer[result_data->write_pos] = target;
    // Add the value after the field type
    memcpy(&buffer[result_data->write_pos + sizeof(cs_result_field_type_t)],
           target_value,
           cs_result_get_type_length(target));
    result_data->last_type = target;

    result_log_debug("field type 0x%x added to the result buffer "
                     "[write_pos: %u -> %u, size: %u bytes, last_type: 0x%x value: %f]" NL,
                     target,
                     result_data->write_pos,
                     (unsigned int)(result_data->write_pos + cs_result_get_type_length(target) + sizeof(cs_result_field_type_t)),
                     result_data->size,
                     result_data->last_type,
                     *(float *)target_value);

    result_data->write_pos +=
      cs_result_get_type_length(target) + sizeof(cs_result_field_type_t);
    result_data->size = result_data->write_pos;
    result_data->type_count++;
  }
  return sc;
}

/******************************************************************************
 * Extract field type and value from the result buffer.
 *****************************************************************************/
sl_status_t cs_result_extract_field(cs_result_session_data_t *result_data,
                                    cs_result_field_type_t target,
                                    uint8_t *buffer,
                                    uint8_t *target_value)
{
  bool found = false;
  sl_status_t sc = SL_STATUS_OK;
  uint8_t type = 0, data_length = 0;
  uint8_t idx = 0;

  // Check if the field type is valid
  if (result_data->size == 0) {
    sc = SL_STATUS_EMPTY;
    result_log_error("failed to extract type 0x%x, buffer is empty! [sc: 0x%x]" NL,
                     target,
                     (unsigned int)sc);
  }

  if (result_data->last_type > CS_RESULT_FIELD_BIT_ERROR_RATE) {
    sc = SL_STATUS_INVALID_TYPE;
    result_log_error("unknown type 0x%x! [sc: 0x%x]!" NL,
                     target,
                     (unsigned int)sc);
  }

  result_log_debug("Looking for target 0x%x ..." NL, target);
  while (idx < result_data->size) {
    type = buffer[idx];
    data_length = cs_result_get_type_length(type);
    if (type == target) {
      // found the target
      memcpy(target_value, &buffer[idx + sizeof(cs_result_field_type_t)], data_length);
      result_log_debug("found target 0x%x, value: %f [count: %u]" NL,
                       target,
                       *(float *)target_value,
                       result_data->type_count);
      found = true;
      // check if there are more types left in the buffer
      if (result_data->type_count > 1) {
        result_data->type_count--;
      } else {
        result_log_debug("extracted all types, reset buffer content and result data" NL);
        memset(buffer, 0, result_data->size);
        cs_result_initialize_results_data(result_data);
      }
      break;
    }
    idx += data_length + sizeof(cs_result_field_type_t);
  }

  if (!found) {
    sc = SL_STATUS_FAIL;
    result_log_error("could not found target 0x%x!" NL,
                     target);
  }
  return sc;
}

/******************************************************************************
 * Create result data for the result buffer.
 *****************************************************************************/
sl_status_t cs_result_create_session_data(uint8_t *buffer,
                                          uint8_t buffer_length,
                                          cs_result_session_data_t *result_data)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t i = 0;
  uint8_t type_size = 0;

  if (result_data == NULL || buffer == NULL) {
    sc = SL_STATUS_NULL_POINTER;
    result_log_error("failed to create result data! "
                     "NULL reference to the buffer or the result data! [sc: 0x%x]" NL,
                     (unsigned int)sc);
    return sc;
  }

  if (result_data->size > 0) {
    sc = SL_STATUS_FAIL;
    result_log_error("uninitialized result session data! "
                     "Aborting. [sc: 0x%x]" NL,
                     (unsigned int)sc);
    return sc;
  }

  result_log_debug("build result data ..." NL);
  // loop through the buffer content, search for type value pairs
  while (i < buffer_length) {
    type_size = cs_result_get_type_length(buffer[i]);
    // the types must be in ascending order
    if ((type_size > 0) && (buffer[i] > result_data->last_type || result_data->type_count == 0)) {
      result_log_debug("type 0x%x found at result[%03u] value: %f!" NL,
                       buffer[i],
                       i,
                       *(float *)&buffer[i + sizeof(cs_result_field_type_t)]);
      result_data->last_type = buffer[i];
      result_data->size += (type_size + sizeof(cs_result_field_type_t));
      result_data->type_count++;
      i += (type_size + sizeof(cs_result_field_type_t));
    } else {
      result_log_error("invalid type 0x%x at result[%03u]! Aborting." NL,
                       buffer[i],
                       i);
      sc = SL_STATUS_INVALID_TYPE;
      break;
    }
  }
  result_log_debug("result data ready - type_count: %u, last_type: %u, size: %u" NL,
                   result_data->type_count,
                   result_data->last_type,
                   result_data->size);
  return sc;
}
