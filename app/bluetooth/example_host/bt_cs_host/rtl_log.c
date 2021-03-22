/***************************************************************************//**
 * @file
 * @brief Log RTL events (NCP host).
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
#include "app_log.h"
#include "app_assert.h"
#include "cs_acp.h"
#include "cs_log.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static FILE *log_file = NULL;
static uint8_t *log_data_buffer = NULL;
static size_t log_data_buffer_len = 0;
static size_t log_data_size = 0;
static uint8_t expected_fragment = CS_LOG_FIRST_FRAGMENT_MASK;

void rtl_log_init(void)
{
  char log_file_name[64];
  char *timestamp = cs_log_get_timestamp();
  int ret;

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

void rtl_log_on_event(cs_acp_log_evt_t *log_evt)
{
  if (((expected_fragment & CS_LOG_FIRST_FRAGMENT_MASK) != (log_evt->fragments_left & CS_LOG_FIRST_FRAGMENT_MASK))
      || (((expected_fragment & CS_LOG_FIRST_FRAGMENT_MASK) == 0) && (log_evt->fragments_left != expected_fragment))) {
    app_log_error("Unexpected log fragment: %u, expected: %u" APP_LOG_NL,
                  log_evt->fragments_left,
                  expected_fragment);
    // Wait for the start of the next message.
    expected_fragment = CS_LOG_FIRST_FRAGMENT_MASK;
    return;
  }

  if (log_evt->fragments_left & CS_LOG_FIRST_FRAGMENT_MASK) {
    log_data_size = 0;
    // Calculate expected total size for the worst case.
    size_t expected_length = UINT8_MAX * ((log_evt->fragments_left & CS_LOG_FRAGMENTS_LEFT_MASK) + 1);
    if (expected_length > log_data_buffer_len) {
      if (log_data_buffer != NULL) {
        free(log_data_buffer);
        log_data_buffer_len = 0;
      }
      log_data_buffer = malloc(expected_length);
      if (log_data_buffer == NULL) {
        app_log_error("Log buffer allocation failed" APP_LOG_NL);
        return;
      }
      log_data_buffer_len = expected_length;
    }
  }

  memcpy(log_data_buffer + log_data_size, log_evt->log_data.data, log_evt->log_data.len);
  log_data_size += log_evt->log_data.len;

  if ((log_evt->fragments_left & CS_LOG_FRAGMENTS_LEFT_MASK) == 0) {
    // Last fragment has arrived
    if (log_file != NULL) {
      fwrite(log_data_buffer, sizeof(uint8_t), log_data_size, log_file);
    }
    expected_fragment = CS_LOG_FIRST_FRAGMENT_MASK;
  } else {
    expected_fragment = (log_evt->fragments_left & CS_LOG_FRAGMENTS_LEFT_MASK) - 1;
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
