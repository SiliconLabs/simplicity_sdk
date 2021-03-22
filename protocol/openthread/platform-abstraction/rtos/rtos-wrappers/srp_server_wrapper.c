/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Srp Server APIs
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

#include <openthread/srp_server.h>
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

extern bool OT_API_REAL_NAME(otSrpServerHostIsDeleted)(const otSrpServerHost * aHost);
extern bool OT_API_REAL_NAME(otSrpServerHostMatchesFullName)(const otSrpServerHost * aHost,const char * aFullName);
extern bool OT_API_REAL_NAME(otSrpServerIsAutoEnableMode)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otSrpServerServiceHasSubTypeServiceName)(const otSrpServerService * aService,const char * aSubTypeServiceName);
extern bool OT_API_REAL_NAME(otSrpServerServiceIsDeleted)(const otSrpServerService * aService);
extern bool OT_API_REAL_NAME(otSrpServerServiceMatchesInstanceName)(const otSrpServerService * aService,const char * aInstanceName);
extern bool OT_API_REAL_NAME(otSrpServerServiceMatchesServiceName)(const otSrpServerService * aService,const char * aServiceName);
extern const char *OT_API_REAL_NAME(otSrpServerGetDomain)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otSrpServerHostGetFullName)(const otSrpServerHost * aHost);
extern const char *OT_API_REAL_NAME(otSrpServerServiceGetInstanceLabel)(const otSrpServerService * aService);
extern const char *OT_API_REAL_NAME(otSrpServerServiceGetInstanceName)(const otSrpServerService * aService);
extern const char *OT_API_REAL_NAME(otSrpServerServiceGetServiceName)(const otSrpServerService * aService);
extern const char *OT_API_REAL_NAME(otSrpServerServiceGetSubTypeServiceNameAt)(const otSrpServerService * aService,uint16_t aIndex);
extern const otIp6Address *OT_API_REAL_NAME(otSrpServerHostGetAddresses)(const otSrpServerHost * aHost,uint8_t * aAddressesNum);
extern const otSrpServerHost *OT_API_REAL_NAME(otSrpServerGetNextHost)(otInstance * aInstance,const otSrpServerHost * aHost);
extern const otSrpServerHost *OT_API_REAL_NAME(otSrpServerServiceGetHost)(const otSrpServerService * aService);
extern const otSrpServerResponseCounters *OT_API_REAL_NAME(otSrpServerGetResponseCounters)(otInstance * aInstance);
extern const otSrpServerService *OT_API_REAL_NAME(otSrpServerHostGetNextService)(const otSrpServerHost * aHost,const otSrpServerService * aService);
extern const uint8_t *OT_API_REAL_NAME(otSrpServerServiceGetTxtData)(const otSrpServerService * aService,uint16_t * aDataLength);
extern otError OT_API_REAL_NAME(otSrpServerParseSubTypeServiceName)(const char * aSubTypeServiceName,char * aLabel,uint8_t aLabelSize);
extern otError OT_API_REAL_NAME(otSrpServerSetAddressMode)(otInstance * aInstance,otSrpServerAddressMode aMode);
extern otError OT_API_REAL_NAME(otSrpServerSetAnycastModeSequenceNumber)(otInstance * aInstance,uint8_t aSequenceNumber);
extern otError OT_API_REAL_NAME(otSrpServerSetDomain)(otInstance * aInstance,const char * aDomain);
extern otError OT_API_REAL_NAME(otSrpServerSetLeaseConfig)(otInstance * aInstance,const otSrpServerLeaseConfig * aLeaseConfig);
extern otError OT_API_REAL_NAME(otSrpServerSetTtlConfig)(otInstance * aInstance,const otSrpServerTtlConfig * aTtlConfig);
extern otSrpServerAddressMode OT_API_REAL_NAME(otSrpServerGetAddressMode)(otInstance * aInstance);
extern otSrpServerState OT_API_REAL_NAME(otSrpServerGetState)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otSrpServerGetPort)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otSrpServerServiceGetNumberOfSubTypes)(const otSrpServerService * aService);
extern uint16_t OT_API_REAL_NAME(otSrpServerServiceGetPort)(const otSrpServerService * aService);
extern uint16_t OT_API_REAL_NAME(otSrpServerServiceGetPriority)(const otSrpServerService * aService);
extern uint16_t OT_API_REAL_NAME(otSrpServerServiceGetWeight)(const otSrpServerService * aService);
extern uint32_t OT_API_REAL_NAME(otSrpServerServiceGetTtl)(const otSrpServerService * aService);
extern uint8_t OT_API_REAL_NAME(otSrpServerGetAnycastModeSequenceNumber)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otSrpServerGetLeaseConfig)(otInstance * aInstance,otSrpServerLeaseConfig * aLeaseConfig);
extern void OT_API_REAL_NAME(otSrpServerGetTtlConfig)(otInstance * aInstance,otSrpServerTtlConfig * aTtlConfig);
extern void OT_API_REAL_NAME(otSrpServerHandleServiceUpdateResult)(otInstance * aInstance,otSrpServerServiceUpdateId aId,otError aError);
extern void OT_API_REAL_NAME(otSrpServerHostGetLeaseInfo)(const otSrpServerHost * aHost,otSrpServerLeaseInfo * aLeaseInfo);
extern void OT_API_REAL_NAME(otSrpServerServiceGetLeaseInfo)(const otSrpServerService * aService,otSrpServerLeaseInfo * aLeaseInfo);
extern void OT_API_REAL_NAME(otSrpServerSetAutoEnableMode)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otSrpServerSetEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otSrpServerSetServiceUpdateHandler)(otInstance * aInstance,otSrpServerServiceUpdateHandler aServiceHandler,void * aContext);

bool OT_API_WRAPPER_NAME(otSrpServerHostIsDeleted)(const otSrpServerHost * aHost)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerHostIsDeleted)(aHost);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerHostMatchesFullName)(const otSrpServerHost * aHost,const char * aFullName)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerHostMatchesFullName)(aHost, aFullName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerIsAutoEnableMode)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerIsAutoEnableMode)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerServiceHasSubTypeServiceName)(const otSrpServerService * aService,const char * aSubTypeServiceName)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerServiceHasSubTypeServiceName)(aService, aSubTypeServiceName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerServiceIsDeleted)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerServiceIsDeleted)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerServiceMatchesInstanceName)(const otSrpServerService * aService,const char * aInstanceName)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerServiceMatchesInstanceName)(aService, aInstanceName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otSrpServerServiceMatchesServiceName)(const otSrpServerService * aService,const char * aServiceName)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otSrpServerServiceMatchesServiceName)(aService, aServiceName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerGetDomain)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerGetDomain)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerHostGetFullName)(const otSrpServerHost * aHost)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerHostGetFullName)(aHost);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerServiceGetInstanceLabel)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerServiceGetInstanceLabel)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerServiceGetInstanceName)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerServiceGetInstanceName)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerServiceGetServiceName)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerServiceGetServiceName)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otSrpServerServiceGetSubTypeServiceNameAt)(const otSrpServerService * aService,uint16_t aIndex)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otSrpServerServiceGetSubTypeServiceNameAt)(aService, aIndex);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otSrpServerHostGetAddresses)(const otSrpServerHost * aHost,uint8_t * aAddressesNum)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otSrpServerHostGetAddresses)(aHost, aAddressesNum);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpServerHost *OT_API_WRAPPER_NAME(otSrpServerGetNextHost)(otInstance * aInstance,const otSrpServerHost * aHost)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpServerHost *ret = OT_API_REAL_NAME(otSrpServerGetNextHost)(aInstance, aHost);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpServerHost *OT_API_WRAPPER_NAME(otSrpServerServiceGetHost)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpServerHost *ret = OT_API_REAL_NAME(otSrpServerServiceGetHost)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpServerResponseCounters *OT_API_WRAPPER_NAME(otSrpServerGetResponseCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpServerResponseCounters *ret = OT_API_REAL_NAME(otSrpServerGetResponseCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSrpServerService *OT_API_WRAPPER_NAME(otSrpServerHostGetNextService)(const otSrpServerHost * aHost,const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSrpServerService *ret = OT_API_REAL_NAME(otSrpServerHostGetNextService)(aHost, aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint8_t *OT_API_WRAPPER_NAME(otSrpServerServiceGetTxtData)(const otSrpServerService * aService,uint16_t * aDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint8_t *ret = OT_API_REAL_NAME(otSrpServerServiceGetTxtData)(aService, aDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerParseSubTypeServiceName)(const char * aSubTypeServiceName,char * aLabel,uint8_t aLabelSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerParseSubTypeServiceName)(aSubTypeServiceName, aLabel, aLabelSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerSetAddressMode)(otInstance * aInstance,otSrpServerAddressMode aMode)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerSetAddressMode)(aInstance, aMode);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerSetAnycastModeSequenceNumber)(otInstance * aInstance,uint8_t aSequenceNumber)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerSetAnycastModeSequenceNumber)(aInstance, aSequenceNumber);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerSetDomain)(otInstance * aInstance,const char * aDomain)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerSetDomain)(aInstance, aDomain);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerSetLeaseConfig)(otInstance * aInstance,const otSrpServerLeaseConfig * aLeaseConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerSetLeaseConfig)(aInstance, aLeaseConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSrpServerSetTtlConfig)(otInstance * aInstance,const otSrpServerTtlConfig * aTtlConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSrpServerSetTtlConfig)(aInstance, aTtlConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otSrpServerAddressMode OT_API_WRAPPER_NAME(otSrpServerGetAddressMode)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otSrpServerAddressMode ret = OT_API_REAL_NAME(otSrpServerGetAddressMode)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otSrpServerState OT_API_WRAPPER_NAME(otSrpServerGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otSrpServerState ret = OT_API_REAL_NAME(otSrpServerGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otSrpServerGetPort)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otSrpServerGetPort)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otSrpServerServiceGetNumberOfSubTypes)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otSrpServerServiceGetNumberOfSubTypes)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otSrpServerServiceGetPort)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otSrpServerServiceGetPort)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otSrpServerServiceGetPriority)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otSrpServerServiceGetPriority)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otSrpServerServiceGetWeight)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otSrpServerServiceGetWeight)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otSrpServerServiceGetTtl)(const otSrpServerService * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otSrpServerServiceGetTtl)(aService);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otSrpServerGetAnycastModeSequenceNumber)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otSrpServerGetAnycastModeSequenceNumber)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otSrpServerGetLeaseConfig)(otInstance * aInstance,otSrpServerLeaseConfig * aLeaseConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerGetLeaseConfig)(aInstance, aLeaseConfig);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerGetTtlConfig)(otInstance * aInstance,otSrpServerTtlConfig * aTtlConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerGetTtlConfig)(aInstance, aTtlConfig);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerHandleServiceUpdateResult)(otInstance * aInstance,otSrpServerServiceUpdateId aId,otError aError)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerHandleServiceUpdateResult)(aInstance, aId, aError);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerHostGetLeaseInfo)(const otSrpServerHost * aHost,otSrpServerLeaseInfo * aLeaseInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerHostGetLeaseInfo)(aHost, aLeaseInfo);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerServiceGetLeaseInfo)(const otSrpServerService * aService,otSrpServerLeaseInfo * aLeaseInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerServiceGetLeaseInfo)(aService, aLeaseInfo);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerSetAutoEnableMode)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerSetAutoEnableMode)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerSetEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerSetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpServerSetServiceUpdateHandler)(otInstance * aInstance,otSrpServerServiceUpdateHandler aServiceHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpServerSetServiceUpdateHandler)(aInstance, aServiceHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
}

