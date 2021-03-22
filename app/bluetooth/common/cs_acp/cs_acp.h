/***************************************************************************//**
 * @file
 * @brief CS Application Co-Processor types
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

#ifndef CS_ACP_H
#define CS_ACP_H

#include "sl_bgapi.h"
#include "sl_component_catalog.h"

#ifdef SL_CATALOG_CS_INITIATOR_PRESENT
#include "cs_initiator_configurator.h"
#endif // SL_CATALOG_CS_INITIATOR_PRESENT

#ifdef SL_CATALOG_CS_REFLECTOR_PRESENT
#include "cs_reflector.h"
#endif // SL_CATALOG_CS_REFLECTOR_PRESENT

#include "sl_enum.h"

SL_ENUM(cs_acp_cmd_id_t) {
  CS_ACP_CMD_CREATE_INITIATOR = 0,
  CS_ACP_CMD_CREATE_REFLECTOR = 1,
  CS_ACP_CMD_INITIATOR_ACTION = 2,
  CS_ACP_CMD_REFLECTOR_ACTION = 3,
  CS_ACP_CMD_ANTENNA_CONFIGURE = 4
};

SL_ENUM(cs_acp_initiator_action_t) {
  CS_ACP_ACTION_DELETE_INITIATOR = 0
};

SL_ENUM(cs_acp_reflector_action_t) {
  CS_ACP_ACTION_DELETE_REFLECTOR = 0
};

#ifdef SL_CATALOG_CS_INITIATOR_PRESENT
// Create initiator
typedef PACKSTRUCT (struct {
  uint8_t connection_id;
  cs_initiator_config_t initiator_config;
  rtl_config_t rtl_config;
}) cs_acp_create_initiator_cmd_data_t;

// Initiator action
typedef PACKSTRUCT (struct {
  uint8_t connection_id;
  cs_acp_initiator_action_t initiator_action;
}) cs_acp_initiator_action_cmd_data_t;

#endif // SL_CATALOG_CS_INITIATOR_PRESENT

#ifdef SL_CATALOG_CS_REFLECTOR_PRESENT
// Create reflector
typedef PACKSTRUCT (struct create_reflector_cmd_data {
  uint8_t connection_id;
  cs_reflector_config_t reflector_config;
}) cs_acp_create_reflector_cmd_data_t;

// Reflector action
typedef PACKSTRUCT (struct {
  uint8_t connection_id;
  cs_acp_reflector_action_t reflector_action;
}) cs_acp_reflector_action_cmd_data_t;

#endif // SL_CATALOG_CS_REFLECTOR_PRESENT

// ACP commands from host to target
typedef PACKSTRUCT (struct {
  cs_acp_cmd_id_t cmd_id;
  union {
#ifdef SL_CATALOG_CS_INITIATOR_PRESENT
    cs_acp_create_initiator_cmd_data_t initiator_cmd_data;
    cs_acp_initiator_action_cmd_data_t initiator_action_data;
#endif // SL_CATALOG_CS_INITIATOR_PRESENT
#ifdef SL_CATALOG_CS_REFLECTOR_PRESENT
    cs_acp_create_reflector_cmd_data_t reflector_cmd_data;
    cs_acp_reflector_action_cmd_data_t reflector_action_data;
#endif // SL_CATALOG_CS_REFLECTOR_PRESENT
    uint8_t antenna_config_wired;
  } data;
}) cs_acp_cmd_t;

// ACP events. Sent from the target to the host asynchronously.

// Event IDs
SL_ENUM(cs_acp_event_id_t) {
  CS_ACP_EVT_RESULT_ID = 0,
  CS_ACP_EVT_LOG_DATA_ID = 1,
  CS_ACP_EVT_STATUS_ID = 2
};

// Log source
SL_ENUM(cs_acp_src_t) {
  LOG_SRC_RTL = 0,
  LOG_SRC_INITIATOR = 1,
  LOG_SRC_REFLECTOR = 2
};

// Result event data
typedef PACKSTRUCT (struct {
  float estimated_distance_mm;
  float rssi_distance_mm;
  float likeliness;
  uint8_t bit_error_rate;
}) cs_acp_result_evt_t;

// Log event data
typedef PACKSTRUCT (struct {
  cs_acp_src_t src;
  uint8_t fragments_left;
  uint8array log_data;
}) cs_acp_log_evt_t;

// The MSB of the fragments_left field indicates the start of a log message
#define CS_LOG_FIRST_FRAGMENT_MASK 0x80
#define CS_LOG_FRAGMENTS_LEFT_MASK 0x7F

// Status change
typedef PACKSTRUCT (struct {
  cs_acp_src_t src;
  sl_status_t sc;
  cs_error_event_t error;
}) cs_acp_status_t;

// Event data
typedef PACKSTRUCT (struct {
  uint8_t connection_id;
  cs_acp_event_id_t acp_evt_id;
  union {
    cs_acp_result_evt_t result;
    cs_acp_status_t stat;
    cs_acp_log_evt_t log;
  } data;
}) cs_acp_event_t;

#endif
