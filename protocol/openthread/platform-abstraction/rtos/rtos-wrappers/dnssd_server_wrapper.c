/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Dnssd Server APIs
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

#include <openthread/dnssd_server.h>
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

extern bool OT_API_REAL_NAME(otDnssdUpstreamQueryIsEnabled)(otInstance * aInstance);
extern const otDnssdCounters *OT_API_REAL_NAME(otDnssdGetCounters)(otInstance * aInstance);
extern const otDnssdQuery *OT_API_REAL_NAME(otDnssdGetNextQuery)(otInstance * aInstance,const otDnssdQuery * aQuery);
extern otDnssdQueryType OT_API_REAL_NAME(otDnssdGetQueryTypeAndName)(const otDnssdQuery * aQuery,char (* aNameOutput)[OT_DNS_MAX_NAME_SIZE]);
extern void OT_API_REAL_NAME(otDnssdQueryHandleDiscoveredHost)(otInstance * aInstance,const char * aHostFullName,otDnssdHostInfo * aHostInfo);
extern void OT_API_REAL_NAME(otDnssdQueryHandleDiscoveredServiceInstance)(otInstance * aInstance,const char * aServiceFullName,otDnssdServiceInstanceInfo * aInstanceInfo);
extern void OT_API_REAL_NAME(otDnssdQuerySetCallbacks)(otInstance * aInstance,otDnssdQuerySubscribeCallback aSubscribe,otDnssdQueryUnsubscribeCallback aUnsubscribe,void * aContext);
extern void OT_API_REAL_NAME(otDnssdUpstreamQuerySetEnabled)(otInstance * aInstance,bool aEnabled);

bool OT_API_WRAPPER_NAME(otDnssdUpstreamQueryIsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otDnssdUpstreamQueryIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otDnssdCounters *OT_API_WRAPPER_NAME(otDnssdGetCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otDnssdCounters *ret = OT_API_REAL_NAME(otDnssdGetCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otDnssdQuery *OT_API_WRAPPER_NAME(otDnssdGetNextQuery)(otInstance * aInstance,const otDnssdQuery * aQuery)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otDnssdQuery *ret = OT_API_REAL_NAME(otDnssdGetNextQuery)(aInstance, aQuery);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otDnssdQueryType OT_API_WRAPPER_NAME(otDnssdGetQueryTypeAndName)(const otDnssdQuery * aQuery,char (* aNameOutput)[OT_DNS_MAX_NAME_SIZE])
{
    sl_ot_rtos_acquire_stack_mutex();
    otDnssdQueryType ret = OT_API_REAL_NAME(otDnssdGetQueryTypeAndName)(aQuery, aNameOutput);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otDnssdQueryHandleDiscoveredHost)(otInstance * aInstance,const char * aHostFullName,otDnssdHostInfo * aHostInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnssdQueryHandleDiscoveredHost)(aInstance, aHostFullName, aHostInfo);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otDnssdQueryHandleDiscoveredServiceInstance)(otInstance * aInstance,const char * aServiceFullName,otDnssdServiceInstanceInfo * aInstanceInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnssdQueryHandleDiscoveredServiceInstance)(aInstance, aServiceFullName, aInstanceInfo);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otDnssdQuerySetCallbacks)(otInstance * aInstance,otDnssdQuerySubscribeCallback aSubscribe,otDnssdQueryUnsubscribeCallback aUnsubscribe,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnssdQuerySetCallbacks)(aInstance, aSubscribe, aUnsubscribe, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otDnssdUpstreamQuerySetEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnssdUpstreamQuerySetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

