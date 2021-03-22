/***************************************************************************//**
 * @file
 * @brief CS initiator API.
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

#ifndef CS_INITIATOR_H
#define CS_INITIATOR_H

// -----------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include "sl_bt_api.h"
#include "sl_rtl_clib_api.h"

#include "cs_initiator_config.h"
#include "cs_initiator_configurator.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Type definitions

/***************************************************************************//**
 * Initiator error callback type
 * @param[in] conn_handle connection handle.
 * @param[in] evt         error event.
 * @param[in] sc          status code.
 ******************************************************************************/
typedef void (*cs_error_cb_t)(uint8_t conn_handle, cs_error_event_t evt, sl_status_t sc);

/***************************************************************************//**
 * Initiator result callback type
 * @param[in] result pointer to the result structure.
 * @param[in] user_data pointer to additional user data.
 ******************************************************************************/
typedef void (*cs_result_cb_t)(const cs_result_t *result, const void *user_data);

// cs procedure triggering
typedef enum {
  CS_PROCEDURE_ACTION_CONTINUE = 0u,
  CS_PROCEDURE_ACTION_TRIGGER_RESET,
  CS_PROCEDURE_ACTION_TRIGGER_START,
  CS_PROCEDURE_ACTION_TRIGGER_STOP
} cs_procedure_action_t;

// -----------------------------------------------------------------------------
// Function declarations

/******************************************************************************
 * Get pointer to cs_procedure.
 * @param[in] conn_handle connection handle
 * @return Returns the current channel sounding procedure parameter struct.
 *****************************************************************************/
sl_rtl_cs_procedure *cs_initiator_get_procedure(const uint8_t conn_handle);

/**************************************************************************//**
 * Extract result from the event.
 * @param[in] conn_handle connection handle
 * @param[in] initiator_part true if initiator data is used
 * @param[in] evt bluetooth event
 * @return action for the ongoing procedure
 *****************************************************************************/
cs_procedure_action_t extract_cs_result_data(const uint8_t conn_handle,
                                             bool initiator_part,
                                             sl_bt_evt_cs_result_t *evt);

/**************************************************************************//**
 * Identify CS initiator based on the connection handle.
 * @param[in] conn_handle connection handle
 *
 * @return true if the handle has an associated initiator instance.
 *****************************************************************************/
bool cs_initiator_identify(const uint8_t conn_handle);

/**************************************************************************//**
 * Create CS initiator instance for the given connection handle.
 * @param[in] conn_handle connection handle
 * @param[in] initiator_config pointer to the initiator config
 * @param[in] result_cb callback for result
 * @param[in] error_cb callback for error
 *
 * @return status of the operation.
 *****************************************************************************/
sl_status_t cs_initiator_create(const uint8_t         conn_handle,
                                cs_initiator_config_t *initiator_config,
                                const rtl_config_t          *rtl_config,
                                cs_result_cb_t        result_cb,
                                cs_error_cb_t         error_cb);

/**************************************************************************//**
 * Create and configure initiator instances.
 *****************************************************************************/
void cs_initiator_init(void);

/**************************************************************************//**
 * Delete CS initiator instance associated with the given connection handle.
 * @param[in] conn_handle connection handle
 *
 * @return status of the operation.
 *****************************************************************************/
sl_status_t cs_initiator_delete(const uint8_t conn_handle);

/**************************************************************************//**
 * Deinitialization function of CS initiator component.
 *****************************************************************************/
void cs_initiator_deinit(void);

/**************************************************************************//**
 * Periodically called step function.
 *****************************************************************************/
void cs_initiator_step(void);

// -----------------------------------------------------------------------------
// Event / callback declarations

/**************************************************************************//**
 * Bluetooth stack event handler of the initiator events.
 * @param[in] evt Event coming from the Bluetooth stack.
 * @return true to send the event to the host in NCP case.
 *****************************************************************************/
bool cs_initiator_on_event(sl_bt_msg_t *evt);

#ifdef __cplusplus
};
#endif

#endif // CS_INITIATOR_H
