/***************************************************************************//**
 * @file
 * @brief OTS Object Server overrided configuration header for ESL image OTS
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_H
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_H

#include "sl_bt_ots_datatypes.h"
#include "esl_tag_image_config.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> General Settings

// <o ESL_TAG_OTS_TRANSFER_MAX_PDU_SIZE> Protocol Data Unit size <22-250>
// <i> Default: 250
// <i> Size of Protocol Data Unit
#define ESL_TAG_OTS_TRANSFER_MAX_PDU_SIZE                                   250

// <o ESL_TAG_OTS_TRANSFER_MAX_SDU_SIZE> Serivice Data Unit size <23-250>
// <i> Default: 250
// <i> Size of Service Data Unit - must be equal to or grater than PDU size.
#define ESL_TAG_OTS_TRANSFER_MAX_SDU_SIZE                                   250

// </h>

// <h> Object Tranfer Serivice Properties
// <o SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MAX_CONCURRENCY> Maximum Concurrency <1-4>
// <i> Default: 2
// <i> Defines maximum number of concurrent client connections
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MAX_CONCURRENCY                      1

// <e SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MULTIPLE_OBJECTS> Allow multiple objects
// <i> Allow multiple objects or single object in this server.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MULTIPLE_OBJECTS                     1

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_LIST_FILTER_SUPPORT> Support Object List Filter
// <i> Include Object List Filter.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_LIST_FILTER_SUPPORT           0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_DLO_SUPPORT> Support Directory Listing Object
// <i> Include Directory Listing Object (DLO) support.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_DLO_SUPPORT                          0

// </e>

// <e SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_SUPPORT> Support Time
// <i> Support Time for OTS
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_SUPPORT                         0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_FIRST_CREATED_SUPPORT> Object First Created
// <i> Support Object Frist Created.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_FIRST_CREATED_SUPPORT           1

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_LAST_MODIFIED_SUPPORT> Object Last Modified
// <i> Support Object Last Modified.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_LAST_MODIFIED_SUPPORT           1

// </e>

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_CHANGED_SUPPORT> Support Object Changed
// <i> Support Object Changed functionality.
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_CHANGED_SUPPORT               0

// <o SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_INDICATION_QUEUE_SIZE> Indication Queue size <1-10>
// <i> Default: 2
// <i> Size of indication Queue
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_INDICATION_QUEUE_SIZE                2

// <h> OTS Features

// <h> OACP Features

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CREATE> Create
// <i> Enable Create opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CREATE              0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_DELETE> Delete
// <i> Enable Delete opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_DELETE              0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CHECKSUM> Calculate Checksum
// <i> Enable Calculate opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CHECKSUM            0

// <e SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_EXECUTE> Execute
// <i> Enable Execute opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_EXECUTE             0

// <o SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OACP_EXECUTE_PARAMETER_LENGTH_MAX> Execute object data size <0-240>
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OACP_EXECUTE_PARAMETER_LENGTH_MAX    0

// </e>

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_READ> Read
// <i> Enable Read opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_READ                0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_WRITE> Write
// <i> Enable Write opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_WRITE               1

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_APPEND> Append
// <i> Enable Appending to objects
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_APPEND              0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_TRUNCATE> Truncate
// <i> Enable Truncation of objects
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_TRUNCATE            1

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_PATCH> Patch
// <i> Enable Patching of objects
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_PATCH               0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_ABORT> Abort
// <i> Enable Abort opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_ABORT               1

// </h>

// <h> OLCP Features

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_GOTO> Go To
// <i> Enable Go To opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_GOTO                1

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_ORDER> Order
// <i> Enable Order opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_ORDER               0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_NUMBER> Request Number Of Objects
// <i> Enable Request Number Of Objects opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_NUMBER              0

// <q SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_CLEAR> Clear Marking
// <i> Enable Clear Marking opcode
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_CLEAR               0

// </h>

// </h>

// </h>

// <<< end of configuration section >>>

#if SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MULTIPLE_OBJECTS == 0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_DLO_SUPPORT
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_DLO_SUPPORT                          0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_LIST_FILTER_SUPPORT
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OBJECT_LIST_FILTER_SUPPORT           0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_GOTO
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_GOTO                0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_ORDER
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_ORDER               0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_NUMBER
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_NUMBER              0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_CLEAR
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_CLEAR               0
#endif // SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_MULTIPLE_OBJECTS

#if SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_SUPPORT == 0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_FIRST_CREATED_SUPPORT
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_FIRST_CREATED_SUPPORT           0
#undef SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_LAST_MODIFIED_SUPPORT
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_LAST_MODIFIED_SUPPORT           0
#endif // SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_TIME_SUPPORT

#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP                                                                                 \
  ( (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CREATE     << SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_BIT_POS)             \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_DELETE   << SL_BT_OTS_OACP_FEATURE_DELETE_OP_CODE_SUPPORTED_BIT_POS)             \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_CHECKSUM << SL_BT_OTS_OACP_FEATURE_CALCULATE_CHECKSUM_OP_CODE_SUPPORTED_BIT_POS) \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_EXECUTE  << SL_BT_OTS_OACP_FEATURE_EXECUTE_OP_CODE_SUPPORTED_BIT_POS)            \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_READ     << SL_BT_OTS_OACP_FEATURE_READ_OP_CODE_SUPPORTED_BIT_POS)               \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_WRITE    << SL_BT_OTS_OACP_FEATURE_WRITE_OP_CODE_SUPPORTED_BIT_POS)              \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_APPEND   << SL_BT_OTS_OACP_FEATURE_APPEND_ADDITIONAL_DATA_SUPPORTED_BIT_POS)     \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_TRUNCATE << SL_BT_OTS_OACP_FEATURE_TRUNCATION_OF_OBJECTS_SUPPORTED_BIT_POS)      \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_PATCH    << SL_BT_OTS_OACP_FEATURE_PATCHING_OF_OBJECTS_SUPPORTED_BIT_POS)        \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OACP_ABORT    << SL_BT_OTS_OACP_FEATURE_ABORT_OP_CODE_SUPPORTED_BIT_POS))

#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP                                                                                        \
  ( (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_GOTO       << SL_BT_OTS_OLCP_FEATURE_GO_TO_OP_CODE_SUPPORTED_BIT_POS)                     \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_ORDER    << SL_BT_OTS_OLCP_FEATURE_ORDER_OP_CODE_SUPPORTED_BIT_POS)                     \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_NUMBER   << SL_BT_OTS_OLCP_FEATURE_REQUEST_NUMBER_OF_OBJECTS_OP_CODE_SUPPORTED_BIT_POS) \
    | (SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OTS_FEATURE_OLCP_CLEAR    << SL_BT_OTS_OLCP_FEATURE_CLEAR_MARKING_OP_CODE_SUPPORTED_BIT_POS))

#if SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OACP_EXECUTE_PARAMETER_LENGTH_MAX > (SL_BT_OTS_INDICATION_SIZE_MAX - SL_BT_OTS_INDICATION_HEADER)
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_INDICATION_SIZE_MAX  (SL_BT_OTS_INDICATION_OVERHEAD \
                                                                + SL_BT_OTS_INDICATION_HEADER \
                                                                + SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_OACP_EXECUTE_PARAMETER_LENGTH_MAX)
#else
#define SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_INDICATION_SIZE_MAX  (SL_BT_OTS_INDICATION_OVERHEAD + SL_BT_OTS_INDICATION_SIZE_MAX)
#endif

#define ESL_TAG_OTS_OBJECT_COUNT  (ESL_TAG_MAX_IMAGES)
#endif // SL_BT_OTS_SERVER_ESL_IMAGE_CONFIG_H
