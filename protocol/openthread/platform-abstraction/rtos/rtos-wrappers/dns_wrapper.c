/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Dns APIs
 *   allowing access to the thread stack in a multi-threaded environment.
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

#include <openthread/dns.h>
#include "sl_ot_rtos_adaptation.h"

#if defined(__GNUC__)
    #define REAL_NAME(function)             __real_##function
    #define WRAPPER_NAME(function)          __wrap_##function
    #define OT_API_REAL_NAME(function)      REAL_NAME(function)
    #define OT_API_WRAPPER_NAME(function)   WRAPPER_NAME(function)
// #elif defined(__IAR_SYSTEMS_ICC__)
//     #define REAL_NAME(function)             $Super$$##function
//     #define WRAPPER_NAME(function)          $Sub$$##function
//     #define OT_API_REAL_NAME(function)      $Super$$__iar_dl##function
//     #define OT_API_WRAPPER_NAME(function)   $Sub$$__iar_dl##function
#else
    #error Unsupported compiler
#endif

extern bool OT_API_REAL_NAME(otDnsIsNameCompressionEnabled)(void);
extern otError OT_API_REAL_NAME(otDnsEncodeTxtData)(const otDnsTxtEntry * aTxtEntries,uint16_t aNumTxtEntries,uint8_t * aTxtData,uint16_t * aTxtDataLength);
extern otError OT_API_REAL_NAME(otDnsGetNextTxtEntry)(otDnsTxtEntryIterator * aIterator,otDnsTxtEntry * aEntry);
extern void OT_API_REAL_NAME(otDnsInitTxtEntryIterator)(otDnsTxtEntryIterator * aIterator,const uint8_t * aTxtData,uint16_t aTxtDataLength);
extern void OT_API_REAL_NAME(otDnsSetNameCompressionEnabled)(bool aEnabled);

bool OT_API_WRAPPER_NAME(otDnsIsNameCompressionEnabled)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otDnsIsNameCompressionEnabled)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsEncodeTxtData)(const otDnsTxtEntry * aTxtEntries,uint16_t aNumTxtEntries,uint8_t * aTxtData,uint16_t * aTxtDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsEncodeTxtData)(aTxtEntries, aNumTxtEntries, aTxtData, aTxtDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsGetNextTxtEntry)(otDnsTxtEntryIterator * aIterator,otDnsTxtEntry * aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsGetNextTxtEntry)(aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otDnsInitTxtEntryIterator)(otDnsTxtEntryIterator * aIterator,const uint8_t * aTxtData,uint16_t aTxtDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnsInitTxtEntryIterator)(aIterator, aTxtData, aTxtDataLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otDnsSetNameCompressionEnabled)(bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnsSetNameCompressionEnabled)(aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

