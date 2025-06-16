/***************************************************************************//**
 * @file sl_wisun_rf_test.c
 * @brief Wi-SUN RF test API
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>
#include <stdint.h>
#include "sl_rail.h"
#include "sl_status.h"
#include "sl_wisun_api.h"
#include "sl_wisun_rf_test.h"
#include "sli_wisun_internal_api.h"

#define SLI_WISUN_ERROR_CHECK_SET_STATUS(__result, __value)\
do {\
  if (!(__result)){\
    status = __value;\
    goto error_handler;\
  }\
} while(0)

#define SLI_WISUN_ERROR_SET_STATUS(__value)\
do {\
  status = __value;\
  goto error_handler;\
} while(0)

#define RF_TEST_NOT_RUNNING (SL_RAIL_STREAM_MODES_COUNT)
static sl_rail_stream_mode_t rf_test_running = RF_TEST_NOT_RUNNING;
static sl_rail_tx_power_level_t stack_tx_power;
static int8_t test_tx_power;


static sl_status_t start_rf_test(uint16_t channel, sl_rail_stream_mode_t mode);
static sl_status_t stop_rf_test(sl_rail_stream_mode_t mode);

sl_status_t sl_wisun_start_stream(uint16_t channel)
{
  return start_rf_test(channel, SL_RAIL_STREAM_PN9_STREAM);
}

sl_status_t sl_wisun_stop_stream()
{
  return stop_rf_test(SL_RAIL_STREAM_PN9_STREAM);
}

sl_status_t sl_wisun_start_tone(uint16_t channel)
{
  return start_rf_test(channel, SL_RAIL_STREAM_CARRIER_WAVE);
}

sl_status_t sl_wisun_stop_tone()
{
  return stop_rf_test(SL_RAIL_STREAM_CARRIER_WAVE);
}

bool sl_wisun_is_running_rf_test()
{
  return rf_test_running != RF_TEST_NOT_RUNNING;
}

sl_status_t sl_wisun_set_test_tx_power(int8_t tx_power)
{
  test_tx_power = tx_power;
  return SL_STATUS_OK;
}

static sl_status_t start_rf_test(uint16_t channel, sl_rail_stream_mode_t mode)
{
  sl_rail_status_t rail_status;
  sl_status_t status;
  sl_wisun_join_state_t join_state;
  sl_rail_handle_t rail_handle;

  status = sli_wisun_get_rail_handle(&rail_handle);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_STATUS_OK == status, SL_STATUS_NOT_READY);

  SLI_WISUN_ERROR_CHECK_SET_STATUS(RF_TEST_NOT_RUNNING == rf_test_running, SL_STATUS_BUSY);

  rail_status = sl_rail_is_valid_channel(rail_handle, channel);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_RAIL_STATUS_NO_ERROR == rail_status, SL_STATUS_INVALID_PARAMETER);

  // checking that we are not connected or joining.
  status = sl_wisun_get_join_state(&join_state);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_WISUN_JOIN_STATE_DISCONNECTED == join_state, SL_STATUS_NETWORK_UP);

  // Backup stack Tx Power
  stack_tx_power = sl_rail_get_tx_power(rail_handle);

  rail_status = sl_rail_set_tx_power_dbm(rail_handle, 10*test_tx_power);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_RAIL_STATUS_NO_ERROR == rail_status, SL_STATUS_FAIL);

  rail_status = sl_rail_start_tx_stream(rail_handle, channel, mode, SL_RAIL_TX_OPTIONS_DEFAULT);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_RAIL_STATUS_NO_ERROR == rail_status, SL_STATUS_FAIL);

  rf_test_running = mode;

  status = SL_STATUS_OK;
error_handler:
  return status;
}

static sl_status_t stop_rf_test(sl_rail_stream_mode_t mode)
{
  sl_rail_status_t rail_status;
  sl_status_t status;
  sl_rail_handle_t rail_handle;

  status = sli_wisun_get_rail_handle(&rail_handle);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_STATUS_OK == status, SL_STATUS_NOT_READY);

  SLI_WISUN_ERROR_CHECK_SET_STATUS(mode == rf_test_running, SL_STATUS_INVALID_STATE);

  rail_status = sl_rail_stop_tx_stream(rail_handle);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_RAIL_STATUS_NO_ERROR == rail_status, SL_STATUS_FAIL);

  rail_status = sl_rail_set_tx_power_dbm(rail_handle, stack_tx_power);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(SL_RAIL_STATUS_NO_ERROR == rail_status, SL_STATUS_FAIL);

  rf_test_running = RF_TEST_NOT_RUNNING;

  status = SL_STATUS_OK;
error_handler:
  return status;
}
