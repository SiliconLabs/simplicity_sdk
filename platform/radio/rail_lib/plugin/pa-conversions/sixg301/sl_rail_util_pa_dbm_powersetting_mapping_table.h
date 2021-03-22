/***************************************************************************//**
 * @file
 * @brief PA power conversion curves used by Silicon Labs PA power conversion
 *   functions.
 * @details This file contains the curves needed convert PA power levels to
 *   dBm powers.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __PA_DBM_POWERSETTING_MAPPING_SIXG301_H_
#define __PA_DBM_POWERSETTING_MAPPING_SIXG301_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RAIL_PA_CURVES_COMMON_INTERFACE_10DBM_NUM_VALUES  (21U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_10DBM_STEP_DDBM   (10U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_10DBM_MAX_POWER_DDBM     (100U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_10DBM_MIN_POWER_DDBM     (-100)
#define RAIL_PA_CURVES_COMMON_INTERFACE_10DBM_CURVES \
  {                                                  \
    0x1 /* -10.0 dBm */,                             \
    0x4 /* -9.0 dBm */,                              \
    0xa /* -8.0 dBm */,                              \
    0xe /* -7.0 dBm */,                              \
    0x13 /* -6.0 dBm */,                             \
    0x17 /* -5.0 dBm */,                             \
    0x1c /* -4.0 dBm */,                             \
    0x21 /* -3.0 dBm */,                             \
    0x27 /* -2.0 dBm */,                             \
    0x2a /* -1.0 dBm */,                             \
    0x2f /* 0.0 dBm */,                              \
    0x35 /* 1.0 dBm */,                              \
    0x39 /* 2.0 dBm */,                              \
    0x3d /* 3.0 dBm */,                              \
    0x42 /* 4.0 dBm */,                              \
    0x47 /* 5.0 dBm */,                              \
    0x4c /* 6.0 dBm */,                              \
    0x50 /* 7.0 dBm */,                              \
    0x55 /* 8.0 dBm */,                              \
    0x5a /* 9.0 dBm */,                              \
    0x5f /* 10.0 dBm */                              \
  }

#define RAIL_PA_CURVES_COMMON_INTERFACE_0DBM_NUM_VALUES  (20U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_0DBM_STEP_DDBM   (10U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_0DBM_MAX_POWER_DDBM     (0U)
#define RAIL_PA_CURVES_COMMON_INTERFACE_0DBM_MIN_POWER_DDBM     (-190)
#define RAIL_PA_CURVES_COMMON_INTERFACE_0DBM_CURVES \
  {                                                 \
    0x9 /* -19.0 dBm */,                            \
    0xa /* -18.0 dBm */,                            \
    0xd /* -17.0 dBm */,                            \
    0x10 /* -16.0 dBm */,                           \
    0x13 /* -15.0 dBm */,                           \
    0x19 /* -14.0 dBm */,                           \
    0x1f /* -13.0 dBm */,                           \
    0x25 /* -12.0 dBm */,                           \
    0x2b /* -11.0 dBm */,                           \
    0x34 /* -10.0 dBm */,                           \
    0x3c /* -9.0 dBm */,                            \
    0x43 /* -8.0 dBm */,                            \
    0x4a /* -7.0 dBm */,                            \
    0x50 /* -6.0 dBm */,                            \
    0x54 /* -5.0 dBm */,                            \
    0x57 /* -4.0 dBm */,                            \
    0x5a /* -3.0 dBm */,                            \
    0x5d /* -2.0 dBm */,                            \
    0x5e /* -1.0 dBm */,                            \
    0x5f /* 0.0 dBm */                              \
  }

#endif
// *INDENT-OFF*

#ifdef __cplusplus
}
#endif
