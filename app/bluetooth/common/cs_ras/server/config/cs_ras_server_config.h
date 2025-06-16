/***************************************************************************//**
 * @file
 * @brief CS RAS Server configuration
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

#ifndef CS_RAS_SERVER_CONFIG_H
#define CS_RAS_SERVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> General

// <o CS_RAS_SERVER_CONFIG_TX_QUEUE_SIZE> TX queue size <2..100>
// <i> Size of TX queue for indications/notifications
// <i> Default: 2
#define CS_RAS_SERVER_CONFIG_TX_QUEUE_SIZE                                           2

// <q CS_RAS_SERVER_CONFIG_LOG_ENABLE> Log
// <i> Default: 0
#ifndef CS_RAS_SERVER_CONFIG_LOG_ENABLE
#define CS_RAS_SERVER_CONFIG_LOG_ENABLE                                              0
#endif

// <q CS_RAS_SERVER_CONFIG_LOG_DATA> Print data output
// <i> Default: 0
#ifndef CS_RAS_SERVER_CONFIG_LOG_DATA
#define CS_RAS_SERVER_CONFIG_LOG_DATA                                                0
#endif

// </h>

// <h> Supported features

// <q CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_REAL_TIME_RANGING_DATA> Real-Time Ranging Data
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_REAL_TIME_RANGING_DATA
#define CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_REAL_TIME_RANGING_DATA                  1
#endif

// <q CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_RETRIEVE_LOST_RANGING_DATA_SEGMENTS> Retrieve Lost Ranging Data Segments
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_RETRIEVE_LOST_RANGING_DATA_SEGMENTS
#define CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_RETRIEVE_LOST_RANGING_DATA_SEGMENTS     1
#endif

// <q CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_ABORT> Abort
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_ABORT
#define CS_RAS_SERVER_CONFIG_SUPPORT_FEATURE_ABORT                                   1
#endif

// </h>

// <h> Notification support

// <q CS_RAS_SERVER_CONFIG_DATA_READY_NOTIFICATIONS> Data Ready
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_DATA_READY_NOTIFICATIONS
#define CS_RAS_SERVER_CONFIG_DATA_READY_NOTIFICATIONS                                1
#endif

// <q CS_RAS_SERVER_CONFIG_OVERWRITTEN_NOTIFICATIONS> Data overwritten
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_OVERWRITTEN_NOTIFICATIONS
#define CS_RAS_SERVER_CONFIG_OVERWRITTEN_NOTIFICATIONS                               1
#endif

// </h>

// <h> Read support

// <q CS_RAS_SERVER_CONFIG_DATA_READY_READ> Data Ready
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_DATA_READY_READ
#define CS_RAS_SERVER_CONFIG_DATA_READY_READ                                         1
#endif

// <q CS_RAS_SERVER_CONFIG_OVERWRITTEN_READ> Data Overwritten
// <i> Default: 1
#ifndef CS_RAS_SERVER_CONFIG_OVERWRITTEN_READ
#define CS_RAS_SERVER_CONFIG_OVERWRITTEN_READ                                        1
#endif

// </h>

// <h> Timeout

// <o CS_RAS_SERVER_CONFIG_DATA_RETENTION_TIMEOUT_MS> Data retention [ms] <10..10000>
// <i> Timeout for data retention from Complete Ranging data to ACK or Retrieve Lost Segments
// <i> Default: 10000
#ifndef CS_RAS_SERVER_CONFIG_DATA_RETENTION_TIMEOUT_MS
#define CS_RAS_SERVER_CONFIG_DATA_RETENTION_TIMEOUT_MS                               10000
#endif

// </h>

// <h> Procedure length

// <o CS_PROCEDURE_MAX_LEN> Procedure maximum length [bytes]
// <i> Maximum length of a procedure stored by the ranging database
// <i> Default: 10000
#ifndef CS_PROCEDURE_MAX_LEN
#define CS_PROCEDURE_MAX_LEN                                                        10000
#endif
// </h>

// <h> Procedure per connection

// <o CS_RAS_PROCEDURE_PER_CONNECTION> Maximum concurrent procedures per connections <1..255>
// <i> Default: 6
#ifndef CS_RAS_PROCEDURE_PER_CONNECTION
#define CS_RAS_PROCEDURE_PER_CONNECTION 6u
#endif
// </h>

// <<< end of configuration section >>>

/** @} (end addtogroup cs_ras_server) */

#endif // CS_RAS_SERVER_CONFIG_H
