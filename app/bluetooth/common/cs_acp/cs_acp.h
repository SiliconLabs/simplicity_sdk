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

/***********************************************************************************************//**
 * @addtogroup cs_acp
 * @{
 **************************************************************************************************/

#include "sl_bgapi.h"
#include "sl_common.h"
#include "sl_component_catalog.h"

#ifdef SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT
#include "cs_initiator_client.h"
#endif // SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT

#ifdef SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT
#include "cs_reflector.h"
#endif // SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT

#include "sl_enum.h"

SL_ENUM(cs_acp_cmd_id_t) {
  CS_ACP_CMD_CREATE_INITIATOR = 0,
  CS_ACP_CMD_CREATE_REFLECTOR = 1,
  CS_ACP_CMD_INITIATOR_ACTION = 2,
  CS_ACP_CMD_REFLECTOR_ACTION = 3,
  CS_ACP_CMD_ANTENNA_CONFIGURE = 4,
  CS_ACP_CMD_ENABLE_TRACE = 5
};

SL_ENUM(cs_acp_initiator_action_t) {
  CS_ACP_ACTION_DELETE_INITIATOR = 0
};

SL_ENUM(cs_acp_reflector_action_t) {
  CS_ACP_ACTION_DELETE_REFLECTOR = 0
};

#ifdef SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT
// Create initiator
SL_PACK_START(1)
typedef struct {
  uint8_t connection_id;
  cs_initiator_config_t initiator_config;
  rtl_config_t rtl_config;
  uint8_t extended_result;
} SL_ATTRIBUTE_PACKED cs_acp_create_initiator_cmd_data_t;
SL_PACK_END()

// Initiator action
SL_PACK_START(1)
typedef struct {
  uint8_t connection_id;
  cs_acp_initiator_action_t initiator_action;
} SL_ATTRIBUTE_PACKED cs_acp_initiator_action_cmd_data_t;
SL_PACK_END()

#endif // SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT

#ifdef SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT
// Create reflector
SL_PACK_START(1)
typedef struct create_reflector_cmd_data {
  uint8_t connection_id;
  cs_reflector_config_t reflector_config;
} SL_ATTRIBUTE_PACKED cs_acp_create_reflector_cmd_data_t;
SL_PACK_END()

// Reflector action
typedef struct {
  uint8_t connection_id;
  cs_acp_reflector_action_t reflector_action;
} SL_ATTRIBUTE_PACKED cs_acp_reflector_action_cmd_data_t;
SL_PACK_END()

#endif // SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT

// ACP commands from host to target
SL_PACK_START(1)
typedef struct {
  cs_acp_cmd_id_t cmd_id;
  union {
#ifdef SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT
    cs_acp_create_initiator_cmd_data_t initiator_cmd_data;
    cs_acp_initiator_action_cmd_data_t initiator_action_data;
#endif // SL_CATALOG_CS_INITIATOR_CLIENT_PRESENT
#ifdef SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT
    cs_acp_create_reflector_cmd_data_t reflector_cmd_data;
    cs_acp_reflector_action_cmd_data_t reflector_action_data;
#endif // SL_CATALOG_CS_REFLECTOR_CONFIG_PRESENT
    uint8_t antenna_config_wired;
    uint8_t enable_trace;
  } data;
} SL_ATTRIBUTE_PACKED cs_acp_cmd_t;
SL_PACK_END()

// ACP events. Sent from the target to the host asynchronously.

// Event IDs
SL_ENUM(cs_acp_event_id_t) {
  CS_ACP_EVT_RESULT_ID = 0,
  CS_ACP_EVT_STATUS_ID = 1,
  CS_ACP_EVT_INTERMEDIATE_RESULT_ID = 2,
  CS_ACP_EVT_EXTENDED_RESULT_ID = 3
};

// Result event data
SL_PACK_START(1)
typedef struct {
  float distance;
  float rssi_distance;
  float likeliness;
  float bit_error_rate;
} SL_ATTRIBUTE_PACKED cs_acp_result_evt_t;
SL_PACK_END()

// Intermediate result event data
SL_PACK_START(1)
typedef struct {
  float progress_percentage;
} SL_ATTRIBUTE_PACKED cs_acp_intermediate_result_evt_t;
SL_PACK_END()

// Result event data extended with CS procedure (fragmented)
SL_PACK_START(1)
typedef struct {
  uint8_t fragments_left;
  uint8array fragment;
} SL_ATTRIBUTE_PACKED cs_acp_extended_result_evt_t;
SL_PACK_END()

// The MSB of the fragments_left field indicates the start of a fragmented message
#define CS_ACP_FIRST_FRAGMENT_MASK 0x80
#define CS_ACP_FRAGMENTS_LEFT_MASK 0x7F

// Status change
SL_PACK_START(1)
typedef struct {
  sl_status_t sc;
  uint8_t error;
} SL_ATTRIBUTE_PACKED cs_acp_status_t;
SL_PACK_END()

// Event data
SL_PACK_START(1)
typedef struct {
  uint8_t connection_id;
  cs_acp_event_id_t acp_evt_id;
  union {
    cs_acp_result_evt_t result;
    cs_acp_intermediate_result_evt_t intermediate_result;
    cs_acp_extended_result_evt_t ext_result;
    cs_acp_status_t stat;
  } data;
} SL_ATTRIBUTE_PACKED cs_acp_event_t;
SL_PACK_END()

/** @} (end addtogroup cs_acp) */
#endif
