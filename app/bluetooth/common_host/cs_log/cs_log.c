/***************************************************************************//**
 * @file
 * @brief CS application logger.
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

#include "app_log.h"
#include "sl_bt_version.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

char *cs_log_get_timestamp(void)
{
  static time_t raw_time = 0;
  static char timestamp[20] = "";

  if (raw_time == 0) {
    int ret;
    struct tm *time_info;

    time(&raw_time);
    time_info = localtime(&raw_time);

    // Create time stamp in the format YYYY-MM-DDThhmmss
    ret = snprintf(timestamp,
                   sizeof(timestamp),
                   "%04u-%02u-%02uT%02u%02u%02u",
                   time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
                   time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    if (ret <= 0 || ret >= sizeof(timestamp)) {
      // Failed to construct time stamp
      timestamp[0] = '\0';
    }
  }
  return timestamp;
}

void cs_log_create(int argc, char *argv[])
{
  FILE *cs_log_file = NULL;
  char log_file_name[64];
  char *timestamp = cs_log_get_timestamp();
  int ret = snprintf(log_file_name,
                     sizeof(log_file_name),
                     "cs_log_%s.jsonl",
                     timestamp);
  if (ret <= 0 || ret >= sizeof(log_file_name)) {
    app_log_error("Failed to construct CS log file name" APP_LOG_NL);
    return;
  }

  cs_log_file = fopen(log_file_name, "w");
  if (cs_log_file == NULL) {
    app_log_error("Failed to open CS log file %s: %s" APP_LOG_NL, log_file_name, strerror(errno));
    return;
  }

  fprintf(cs_log_file, "{");
  fprintf(cs_log_file, "\"cs_log_version\": \"1.0\"");
  fprintf(cs_log_file,
          ", \"bluetooth_stack_version\": \"%u.%u.%u+%u\"",
          SL_BT_VERSION_MAJOR,
          SL_BT_VERSION_MINOR,
          SL_BT_VERSION_PATCH,
          SL_BT_VERSION_BUILD);
  fprintf(cs_log_file, ", \"command_line_options\": \"%s", argv[0]);
  for (int arg = 1; arg < argc; arg++) {
    fprintf(cs_log_file, " %s", argv[arg]);
  }
  fprintf(cs_log_file, "\"");
  fprintf(cs_log_file, "}\n");

  ret = fclose(cs_log_file);
  if (ret != 0) {
    app_log_error("Failed to close CS log file" APP_LOG_NL);
  }
}
