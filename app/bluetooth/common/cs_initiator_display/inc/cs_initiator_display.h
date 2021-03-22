/***************************************************************************//**
 * @file
 * @brief CS Initiator display API
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
#ifndef CS_INITIATOR_DISPLAY_H
#define CS_INITIATOR_DISPLAY_H

// -----------------------------------------------------------------------------
// Includes

#include "sl_bt_api.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
// Macros

#define CS_INITIATOR_DISPLAY_STATE_CONNECTED_TEXT       "ST: Connected"
#define CS_INITIATOR_DISPLAY_STATE_DISCONNECTED_TEXT    "ST: Disconnected"
#define CS_INITIATOR_DISPLAY_STATE_SCANNING_TEXT        "ST: Scanning..."
#define CS_INITIATOR_DISPLAY_STATE_INITIALIZED_TEXT     "ST: Initialized"

// -----------------------------------------------------------------------------
// Enums, structs, typedefs

/// UI rows
typedef enum {
  ROW_SYSTEM,
  ROW_ROLE,
  ROW_MODE,
  ROW_DISTANCE_TEXT,
  ROW_DISTANCE_VALUE,
  ROW_LIKELINESS_TEXT,
  ROW_LIKELINESS_VALUE,
  ROW_RSSI_DISTANCE_TEXT,
  ROW_RSSI_DISTANCE_VALUE,
  ROW_BIT_ERROR_RATE_TEXT,
  ROW_BIT_ERROR_RATE_VALUE,
  ROW_STATE
} cs_initiator_display_row_t;

/// UI alignment
typedef enum {
  CS_INITIATOR_DISPLAY_ALIGNMENT_LEFT = 0u,
  CS_INITIATOR_DISPLAY_ALIGNMENT_CENTER,
  CS_INITIATOR_DISPLAY_ALIGNMENT_RIGHT
} cs_initiator_display_alignment_t;

// -----------------------------------------------------------------------------
// Function declarations

/**************************************************************************//**
 * Initialize the display and the UI
 *****************************************************************************/
sl_status_t cs_initiator_display_init(void);

/**************************************************************************//**
 * Update the display
 *****************************************************************************/
void cs_initiator_display_update(void);

/**************************************************************************//**
 * Set the UI text alignment.
 *
 * @param[in] alignment text alignment to use
 *****************************************************************************/
void cs_initiator_display_set_alignment(cs_initiator_display_alignment_t align);

/**************************************************************************//**
 * Write text on the UI.
 *
 * @param[in] str pointer to the text to print
 * @param[in] row row to print the text on
 *****************************************************************************/
void cs_initiator_display_write_text(char *str, uint8_t row);

/**************************************************************************//**
 * Print the calculated value on the UI
 *
 * @param[in] value the calculated value
 * @param[in] row number of the row to print on, see cs_initiator_display_row_t
 * @param[in] unit unit string, in case of NULL no unit displayed
 *****************************************************************************/
void cs_initiator_display_print_value(float value, uint8_t row, char *unit);

// -----------------------------------------------------------------------------
// Event / callback declarations

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void cs_initiator_display_on_event(sl_bt_msg_t *evt);

#ifdef __cplusplus
}
#endif

#endif // CS_INITIATOR_DISPLAY_H
