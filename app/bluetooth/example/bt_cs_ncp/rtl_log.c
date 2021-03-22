/***************************************************************************//**
 * @file
 * @brief Log RTL events (NCP target).
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

#include "sl_rtl_clib_api.h"
#include "sl_bt_api.h"
#include "sl_bt_version.h"
#include "sl_common.h"
#include "sl_power_manager.h"
#include "cs_acp.h"
#include "app_assert.h"
#include "app_log.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define LOG_DATA_BUFFER_MAX_SIZE 4096
#define LOG_EVENT_OVERHEAD       (sizeof(cs_acp_event_id_t) + sizeof(cs_acp_src_t) + 3)
#define LOG_EVENT_MAX_DATA       (UINT8_MAX - LOG_EVENT_OVERHEAD)

static void rtl_log_callback(uint8_t *log_data, size_t log_data_len);

static uint8_t log_data_buffer[LOG_DATA_BUFFER_MAX_SIZE];
static size_t log_data_buffer_len = 0;
static uint8_t *log_data_ptr;

void rtl_log_init(void)
{
  int ret;
  enum sl_rtl_error_code ec;
  const char hash[] = SL_BT_VERSION_HASH;
  sl_rtl_log_configure_params config_params = {
    .log_callback_function = rtl_log_callback,
    .sdk_version = { 0 },
    .command_line_options = { 0 }
  };

  ret = snprintf(config_params.sdk_version,
                 SL_RTL_LOG_SDK_VERSION_CHAR_ARRAY_MAX_SIZE,
                 "%u.%u.%u.%u-%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                 SL_BT_VERSION_MAJOR,
                 SL_BT_VERSION_MINOR,
                 SL_BT_VERSION_PATCH,
                 SL_BT_VERSION_BUILD,
                 hash[0], hash[1], hash[2], hash[3], hash[4],
                 hash[5], hash[6], hash[7], hash[8], hash[9],
                 hash[10], hash[11], hash[12], hash[13], hash[14],
                 hash[15], hash[16], hash[17], hash[18], hash[19]);
  app_assert(ret > 0 && ret < SL_RTL_LOG_SDK_VERSION_CHAR_ARRAY_MAX_SIZE,
             "failed to construct version string");

  ec = sl_rtl_log_init();
  app_assert(ec == SL_RTL_ERROR_SUCCESS, "sl_rtl_log_init failed");

  ec = sl_rtl_log_configure(&config_params);
  app_assert(ec == SL_RTL_ERROR_SUCCESS, "sl_rtl_log_configure failed");
}

void rtl_log_step(void)
{
  if (log_data_buffer_len == 0) {
    return;
  }

  // Avoid big buffer allocation on the stack by using static variable.
  static uint8_t evt_data[UINT8_MAX];
  uint8_t evt_data_len = (uint8_t)SL_MIN(log_data_buffer_len, (size_t)LOG_EVENT_MAX_DATA);
  cs_acp_event_t *evt = (cs_acp_event_t *)evt_data;
  evt->connection_id = SL_BT_INVALID_CONNECTION_HANDLE;
  evt->acp_evt_id = CS_ACP_EVT_LOG_DATA_ID;
  evt->data.log.src = LOG_SRC_RTL;
  memcpy(evt->data.log.log_data.data, log_data_ptr, evt_data_len);
  evt->data.log.log_data.len = evt_data_len;
  log_data_buffer_len -= evt_data_len;
  // Calculate remaining number of fragments using ceiling division.
  evt->data.log.fragments_left = (uint8_t)((log_data_buffer_len + LOG_EVENT_MAX_DATA - 1) / LOG_EVENT_MAX_DATA);
  if (log_data_ptr == log_data_buffer) {
    // First fragment
    evt->data.log.fragments_left |= CS_LOG_FIRST_FRAGMENT_MASK;
  }
  log_data_ptr += evt_data_len;
  sl_bt_send_evt_user_cs_service_message_to_host(evt_data_len + LOG_EVENT_OVERHEAD, evt_data);

  if (log_data_buffer_len == 0) {
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
  }
}

static void rtl_log_callback(uint8_t *log_data, size_t log_data_len)
{
  if (log_data_len > LOG_DATA_BUFFER_MAX_SIZE) {
    app_log_error("Log data buffer too small: %zu > %zu" APP_LOG_NL, log_data_len, LOG_DATA_BUFFER_MAX_SIZE);
    return;
  }
  if (log_data_buffer_len > 0) {
    app_log_error("Log data buffer busy" APP_LOG_NL);
    return;
  }

  memcpy(log_data_buffer, log_data, log_data_len);
  log_data_buffer_len = log_data_len;
  log_data_ptr = log_data_buffer;
  // Keep the MCU awake until all fragments are sent.
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
}
