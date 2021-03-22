/***************************************************************************//**
 * @file
 * @brief Log RTL events.
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

#include "rtl_log.h"
#include "sl_rtl_clib_api.h"
#include "sl_bt_version.h"
#include "app_log.h"
#include "app_assert.h"
#include "cs_log.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static FILE *log_file = NULL;

static void rtl_log_callback(uint8_t *log_data, size_t log_data_len);

void rtl_log_init(void)
{
  char log_file_name[64];
  char *timestamp;
  int ret;
  enum sl_rtl_error_code ec;
  const uint8_t hash[] = SL_BT_VERSION_HASH;

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
             "failed to construct version string, %d", ret);

  ec = sl_rtl_log_init();
  app_assert(ec == SL_RTL_ERROR_SUCCESS, "sl_rtl_log_init failed");

  ec = sl_rtl_log_configure(&config_params);
  app_assert(ec == SL_RTL_ERROR_SUCCESS, "sl_rtl_log_configure failed");

  timestamp = cs_log_get_timestamp();
  ret = snprintf(log_file_name,
                 sizeof(log_file_name),
                 "rtl_log_%s.jsonl",
                 timestamp);
  if (ret <= 0 || ret >= sizeof(log_file_name)) {
    app_log_error("Failed to construct RTL log file name" APP_LOG_NL);
    return;
  }

  log_file = fopen(log_file_name, "wb");
  if (log_file == NULL) {
    app_log_error("Failed to open RTL log file %s: %s" APP_LOG_NL, log_file_name, strerror(errno));
  }
}

static void rtl_log_callback(uint8_t *log_data, size_t log_data_len)
{
  if (log_file != NULL) {
    fwrite(log_data, sizeof(uint8_t), log_data_len, log_file);
  } else {
    app_log("[RTL] %s" APP_LOG_NL, (char *)log_data);
  }
}

void rtl_log_deinit(void)
{
  if (log_file == NULL) {
    // No file to be closed
    return;
  }
  int ret = fclose(log_file);
  if (ret != 0) {
    app_log_error("Failed to close RTL log file" APP_LOG_NL);
  }
}
