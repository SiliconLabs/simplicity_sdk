/***************************************************************************//**
 * @file
 * @brief CS reflector configuration
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

#ifndef CS_REFLECTOR_CONFIG_H
#define CS_REFLECTOR_CONFIG_H

#include "sl_bluetooth_connection_config.h"

#define CS_REFLECTOR_MAX_CONNECTIONS SL_BT_CONFIG_MAX_CONNECTIONS

// <<< Use Configuration Wizard in Context Menu >>>

// <h> CS reflector configuration

// <o CS_REFLECTOR_CS_EVENT_BUF_SIZE> Size of the Channel Sounding event buffer [elements] <1..255>
// <i> Default: 16
// <i> Size of the Channel Sounding event buffer
#define CS_REFLECTOR_CS_EVENT_BUF_SIZE     (16)

// <o CS_REFLECTOR_MIN_TX_POWER_DBM> Minimum transmit power of the reflector radio [dBm] <-127..20>
// <i> Default: -3
// <i> Minimum transmit power of the reflector radio
#define CS_REFLECTOR_MIN_TX_POWER_DBM      (-3)

// <o CS_REFLECTOR_MAX_TX_POWER_DBM> Maximum transmit power of the reflector radio [dBm] <-127..20>
// <i> Default: 20
// <i> Maximum transmit power of the reflector radio
#define CS_REFLECTOR_MAX_TX_POWER_DBM      (20)

// <o CS_REFLECTOR_ANTENNA_OFFSET> Select antenna offset type
// <0=> Wireless antenna offset
// <1=> Wired antenna offset
// <i> Default: 0
#define CS_REFLECTOR_ANTENNA_OFFSET         0

// <q CS_REFLECTOR_LOG> Reflector logging
// <i> Default: 1
// <i> Enable reflector component logging
#define CS_REFLECTOR_LOG                    1

// </h>

// <<< end of configuration section >>>
#endif // CS_REFLECTOR_CONFIG_H
