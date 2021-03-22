/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include "sl_common.h"
#include "app.h"
#include "rtl_log.h"
#include "em_gpio.h"
#include "sl_bt_api.h"
#include "sl_status.h"
#include "sl_ncp.h"
#include "cs_acp.h"
#include "cs_initiator.h"
#include "cs_antenna.h"

#define RESULT_MSG_LEN (sizeof(uint8_t) + sizeof(cs_acp_event_id_t) + sizeof(cs_acp_result_evt_t))
#define ERROR_MSG_LEN (sizeof(uint8_t) + sizeof(cs_acp_event_id_t) + sizeof(cs_acp_status_t))

static void cs_on_result(const cs_result_t *result, const void *user_data);
static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc);
static sl_status_t handle_initiator_action(const cs_acp_initiator_action_cmd_data_t *initiator_action_data);

/***************************************************************************//**
 * Application Init.
 ******************************************************************************/
SL_WEAK void app_init(void)
{
  rtl_log_init();
  cs_initiator_init();
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  rtl_log_step();
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

void sl_ncp_user_cs_cmd_message_to_target_cb(void *data)
{
  cs_acp_cmd_t *cs_cmd;
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;

  uint8array *data_arr = (uint8array *)data;
  cs_cmd = (cs_acp_cmd_t *)(data_arr->data);

  switch (cs_cmd->cmd_id) {
#ifdef SL_CATALOG_CS_INITIATOR_PRESENT
    case CS_ACP_CMD_CREATE_INITIATOR:
      sc = cs_initiator_create(cs_cmd->data.initiator_cmd_data.connection_id,
                               &cs_cmd->data.initiator_cmd_data.initiator_config,
                               &cs_cmd->data.initiator_cmd_data.rtl_config,
                               cs_on_result,
                               cs_on_error);
      break;
    case CS_ACP_CMD_INITIATOR_ACTION:
      sc = handle_initiator_action(&cs_cmd->data.initiator_action_data);
      break;
#endif // SL_CATALOG_CS_INITIATOR_PRESENT
#ifdef SL_CATALOG_CS_REFLECTOR_PRESENT
    case CS_ACP_CMD_CREATE_REFLECTOR:
      sc = cs_reflector_create(cs_cmd->data.reflector_cmd_data.connection_id,
                               &cs_cmd->data.reflector_cmd_data.reflector_config);
      break;
    case CS_ACP_CMD_REFLECTOR_ACTION:
      if (cs_cmd->data.reflector_action_data.reflector_action
          == CS_ACP_ACTION_DELETE_REFLECTOR) {
        sc = cs_reflector_delete(cs_cmd->data.reflector_action_data.connection_id);
      }
      break;
#endif // SL_CATALOG_CS_INITIATOR_PRESENT
    case CS_ACP_CMD_ANTENNA_CONFIGURE:
      sc = cs_antenna_configure((bool)cs_cmd->data.antenna_config_wired);
    default:
      // Unknown command, leave the default value of sc unchanged.
      break;
  }
  sl_bt_send_rsp_user_cs_service_message_to_target((uint16_t)sc, 0, NULL);
}

static void cs_on_result(const cs_result_t *result, const void *user_data)
{
  (void)user_data;

  cs_acp_event_t cs_user_event;

  cs_user_event.acp_evt_id = CS_ACP_EVT_RESULT_ID;
  cs_user_event.connection_id = result->connection;
  cs_user_event.data.result.estimated_distance_mm = result->distance;
  cs_user_event.data.result.bit_error_rate = result->cs_bit_error_rate;
  cs_user_event.data.result.rssi_distance_mm = result->rssi_distance;
  cs_user_event.data.result.likeliness = result->likeliness;

  sl_bt_send_evt_user_cs_service_message_to_host(RESULT_MSG_LEN, (uint8_t *)&cs_user_event);
}

static void cs_on_error(uint8_t conn_handle, cs_error_event_t err_evt, sl_status_t sc)
{
  cs_acp_event_t cs_user_event;

  cs_user_event.acp_evt_id = CS_ACP_EVT_STATUS_ID;
  cs_user_event.connection_id = conn_handle;
  cs_user_event.data.stat.sc = sc;
  cs_user_event.data.stat.error = err_evt;
  cs_user_event.data.stat.src = LOG_SRC_INITIATOR;

  sl_bt_send_evt_user_cs_service_message_to_host(ERROR_MSG_LEN, (uint8_t *)&cs_user_event);
}

static sl_status_t handle_initiator_action(const cs_acp_initiator_action_cmd_data_t *action)
{
  sl_status_t sc = SL_STATUS_FAIL;

  switch (action->initiator_action) {
    case CS_ACP_ACTION_DELETE_INITIATOR:
      sc = cs_initiator_delete(action->connection_id);
      break;
    default:
      break;
  }

  return sc;
}
