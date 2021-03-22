/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Srp Client APIs
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

#include <openthread/srp_client.h>
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

extern bool OT_API_REAL_NAME(otSrpClientIsAutoStartModeEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otSrpClientIsRunning)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otSrpClientIsServiceKeyRecordEnabled)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otSrpClientGetDomainName)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otSrpClientItemStateToString)(otSrpClientItemState aItemState);
extern const otSockAddr *OT_API_REAL_NAME(otSrpClientGetServerAddress)(otInstance * aInstance);
extern const otSrpClientHostInfo *OT_API_REAL_NAME(otSrpClientGetHostInfo)(otInstance * aInstance);
extern const otSrpClientService *OT_API_REAL_NAME(otSrpClientGetServices)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otSrpClientAddService)(otInstance * aInstance,otSrpClientService * aService);
extern otError OT_API_REAL_NAME(otSrpClientClearService)(otInstance * aInstance,otSrpClientService * aService);
extern otError OT_API_REAL_NAME(otSrpClientEnableAutoHostAddress)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otSrpClientRemoveHostAndServices)(otInstance * aInstance,bool aRemoveKeyLease,bool aSendUnregToServer);
extern otError OT_API_REAL_NAME(otSrpClientRemoveService)(otInstance * aInstance,otSrpClientService * aService);
extern otError OT_API_REAL_NAME(otSrpClientSetDomainName)(otInstance * aInstance,const char * aName);
extern otError OT_API_REAL_NAME(otSrpClientSetHostAddresses)(otInstance * aInstance,const otIp6Address * aIp6Addresses,uint8_t aNumAddresses);
extern otError OT_API_REAL_NAME(otSrpClientSetHostName)(otInstance * aInstance,const char * aName);
extern otError OT_API_REAL_NAME(otSrpClientStart)(otInstance * aInstance,const otSockAddr * aServerSockAddr);
extern uint32_t OT_API_REAL_NAME(otSrpClientGetKeyLeaseInterval)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otSrpClientGetLeaseInterval)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otSrpClientGetTtl)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otSrpClientClearHostAndServices)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otSrpClientDisableAutoStartMode)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otSrpClientEnableAutoStartMode)(otInstance * aInstance,otSrpClientAutoStartCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otSrpClientSetCallback)(otInstance * aInstance,otSrpClientCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otSrpClientSetKeyLeaseInterval)(otInstance * aInstance,uint32_t aInterval);
extern void OT_API_REAL_NAME(otSrpClientSetLeaseInterval)(otInstance * aInstance,uint32_t aInterval);
extern void OT_API_REAL_NAME(otSrpClientSetServiceKeyRecordEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otSrpClientSetTtl)(otInstance * aInstance,uint32_t aTtl);
extern void OT_API_REAL_NAME(otSrpClientStop)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otSrpClientIsAutoStartModeEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpClientIsAutoStartModeEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpClientIsRunning)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpClientIsRunning)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpClientIsServiceKeyRecordEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpClientIsServiceKeyRecordEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpClientGetDomainName)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpClientGetDomainName)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpClientItemStateToString)(otSrpClientItemState aItemState)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpClientItemStateToString)(aItemState);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSockAddr *OT_API_WRAPPER_NAME(otSrpClientGetServerAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSockAddr *ret = OT_API_REAL_NAME(otSrpClientGetServerAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpClientHostInfo *OT_API_WRAPPER_NAME(otSrpClientGetHostInfo)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpClientHostInfo *ret = OT_API_REAL_NAME(otSrpClientGetHostInfo)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpClientService *OT_API_WRAPPER_NAME(otSrpClientGetServices)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpClientService *ret = OT_API_REAL_NAME(otSrpClientGetServices)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientAddService)(otInstance * aInstance,otSrpClientService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientAddService)(aInstance, aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientClearService)(otInstance * aInstance,otSrpClientService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientClearService)(aInstance, aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientEnableAutoHostAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientEnableAutoHostAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientRemoveHostAndServices)(otInstance * aInstance,bool aRemoveKeyLease,bool aSendUnregToServer)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientRemoveHostAndServices)(aInstance, aRemoveKeyLease, aSendUnregToServer);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientRemoveService)(otInstance * aInstance,otSrpClientService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientRemoveService)(aInstance, aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientSetDomainName)(otInstance * aInstance,const char * aName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientSetDomainName)(aInstance, aName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientSetHostAddresses)(otInstance * aInstance,const otIp6Address * aIp6Addresses,uint8_t aNumAddresses)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientSetHostAddresses)(aInstance, aIp6Addresses, aNumAddresses);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientSetHostName)(otInstance * aInstance,const char * aName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientSetHostName)(aInstance, aName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpClientStart)(otInstance * aInstance,const otSockAddr * aServerSockAddr)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpClientStart)(aInstance, aServerSockAddr);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otSrpClientGetKeyLeaseInterval)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otSrpClientGetKeyLeaseInterval)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otSrpClientGetLeaseInterval)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otSrpClientGetLeaseInterval)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otSrpClientGetTtl)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otSrpClientGetTtl)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otSrpClientClearHostAndServices)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientClearHostAndServices)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientDisableAutoStartMode)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientDisableAutoStartMode)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientEnableAutoStartMode)(otInstance * aInstance,otSrpClientAutoStartCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientEnableAutoStartMode)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientSetCallback)(otInstance * aInstance,otSrpClientCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientSetCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientSetKeyLeaseInterval)(otInstance * aInstance,uint32_t aInterval)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientSetKeyLeaseInterval)(aInstance, aInterval);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientSetLeaseInterval)(otInstance * aInstance,uint32_t aInterval)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientSetLeaseInterval)(aInstance, aInterval);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientSetServiceKeyRecordEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientSetServiceKeyRecordEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientSetTtl)(otInstance * aInstance,uint32_t aTtl)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientSetTtl)(aInstance, aTtl);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

