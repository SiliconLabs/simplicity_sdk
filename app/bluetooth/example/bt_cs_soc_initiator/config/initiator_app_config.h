/***************************************************************************//**
 * @file
 * @brief CS initiator example configuration
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

#ifndef INITIATOR_CONFIG
#define INITIATOR_CONFIG

#include "cs_initiator_config.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <o MEASUREMENT_MODE> Specify measurement mode.
// <i> Specify measurement mode.
// <CS_INITIATOR_MODE_DYNAMIC=> Button dependent
// <CS_INITIATOR_MODE_RTT=> Round Trip Time
// <CS_INITIATOR_MODE_PBR=> Phase Based Ranging
// <i> Default: CS_INITIATOR_MODE_DYNAMIC
#define MEASUREMENT_MODE     CS_INITIATOR_MODE_DYNAMIC
// </o>

// <q CS_RSSI_MEASUREMENT_ENABLED> Specifies whether distance should also be measured based on RSSI
// <i> Enable/disable RSSI distance measurement
// <i> 0 - RSSI distance measurement is disabled
// <i> 1 - RSSI distance measurement is enabled
// <i> Default: 1
#define CS_RSSI_MEASUREMENT_ENABLED              (1)
// </q>

// <o CS_RSSI_TX_POWER> Reference RSSI value of the Tx device at 1.0 m distance in dBm <-100-20>
// <i> Specifes Tx device RSSI at 1m in dBm
// <i> Default: -40
#define CS_RSSI_TX_POWER                         (-40)

// <<< end of configuration section >>>

#endif // INITIATOR_CONFIG
