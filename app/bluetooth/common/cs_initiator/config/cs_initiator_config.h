/***************************************************************************//**
 * @file
 * @brief CS initiator component configuration
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

#ifndef CS_INITIATOR_CONFIG_H
#define CS_INITIATOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> CS initiator component configuration

// <o CS_INITIATOR_MAX_CONNECTIONS> Specify maximum initiator connections <1..4>
// <i> Default: 1
#ifndef CS_INITIATOR_MAX_CONNECTIONS
#define CS_INITIATOR_MAX_CONNECTIONS                  (1)
#endif

// <o CS_INITIATOR_MAX_SUBEVENT_PER_PROC> Specify maximum subevents per procedure <1..6>
// <i> Default: 6
#ifndef CS_INITIATOR_MAX_SUBEVENT_PER_PROC
#define CS_INITIATOR_MAX_SUBEVENT_PER_PROC            (6)
#endif

// <o CS_INITIATOR_MAX_STEP_DATA_LEN> Specify maximum step data length <32..2048>
// <i> Default: 2048
#ifndef CS_INITIATOR_MAX_STEP_DATA_LEN
#define CS_INITIATOR_MAX_STEP_DATA_LEN                (2048)
#endif

// <q CS_INITIATOR_LOG> Enable initiator log
// <i> Default: 1
// <i> Enable Initiator event buffer managers own log
#ifndef CS_INITIATOR_LOG
#define CS_INITIATOR_LOG                       (1)
#endif

// <o CS_INITIATOR_CS_EVENT_BUF_SIZE> Size of the Channel Sounding event buffer [elements] <1..255>
// <i> Default: 16
// <i> Size of the Channel Sounding event buffer
#ifndef CS_INITIATOR_CS_EVENT_BUF_SIZE
#define CS_INITIATOR_CS_EVENT_BUF_SIZE                (16)
#endif

// <o CS_INITIATOR_MIN_INTERVAL> Minimum delay between CS measurements [connection events] <1..255>
// <i> Default: 30
// <i> Minimum duration in number of connection events between consecutive CS measurement procedures
#ifndef CS_INITIATOR_MIN_INTERVAL
#define CS_INITIATOR_MIN_INTERVAL                     (30)
#endif

// <o CS_INITIATOR_MAX_INTERVAL> Maximum delay between CS measurements [connection events] <1..255>
// <i> Default: 30
// <i> Maximum duration in number of connection events between consecutive CS measurement procedures
#ifndef CS_INITIATOR_MAX_INTERVAL
#define CS_INITIATOR_MAX_INTERVAL                     (30)
#endif

// <o CS_INITIATOR_ANTENNA_OFFSET> Specify antenna offset
// <0=> Wireless antenna offset
// <1=> Wired antenna offset
// <i> Default: 0
#ifndef CS_INITIATOR_ANTENNA_OFFSET
#define CS_INITIATOR_ANTENNA_OFFSET                   (0)
#endif

// <e CS_INITIATOR_ERROR_TIMER_USED> Use error timer
// <i> Enable usage of error timer
// <i> Default: On
#ifndef CS_INITIATOR_ERROR_TIMER_USED
#define CS_INITIATOR_ERROR_TIMER_USED                 (1)
#endif

// <o CS_INITIATOR_ERROR_TIMEOUT_MS> Error timeout [msec] <100-5000>

// <i> Timeout value in order to avoid stuck in error state indefinitely.
// <i> Once the time elapse the initiator instances' error callback executes to
// <i> inform the user about the issue.
// <i> Default: 3000
#define CS_INITIATOR_ERROR_TIMEOUT_MS     3000

// </e>

// </h>

// <<< end of configuration section >>>

#endif // CS_INITIATOR_CONFIG_H
