/***************************************************************************//**
 * @file
 * @brief CS Antenna
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

#include <stdint.h>
#include "cs_antenna.h"
#include "sl_bt_api.h"
#include "sl_rail_util_cs_antenna_offset_config.h"

static const int16_t cs_antenna_offset_wireless_cm[SL_RAIL_UTIL_CS_ANTENNA_COUNT] =
  SL_RAIL_UTIL_CS_ANTENNA_OFFSET_WIRELESS_CM;
static const int16_t cs_antenna_offset_wired_cm[SL_RAIL_UTIL_CS_ANTENNA_COUNT] =
  SL_RAIL_UTIL_CS_ANTENNA_OFFSET_WIRED_CM;

sl_status_t cs_antenna_configure(bool wired)
{
  const int16_t *antenna_offset = wired ? cs_antenna_offset_wired_cm : cs_antenna_offset_wireless_cm;
  // Regardless of SL_RAIL_UTIL_CS_ANTENNA_COUNT, only 1 antenna is supported for now.
  return sl_bt_cs_set_antenna_configuration(sizeof(int16_t),
                                            (uint8_t *)antenna_offset);
}
