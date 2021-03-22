/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Dns Client APIs
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

#include <openthread/dns_client.h>
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

extern const otDnsQueryConfig *OT_API_REAL_NAME(otDnsClientGetDefaultConfig)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otDnsAddressResponseGetAddress)(const otDnsAddressResponse * aResponse,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl);
extern otError OT_API_REAL_NAME(otDnsAddressResponseGetHostName)(const otDnsAddressResponse * aResponse,char * aNameBuffer,uint16_t aNameBufferSize);
extern otError OT_API_REAL_NAME(otDnsBrowseResponseGetHostAddress)(const otDnsBrowseResponse * aResponse,const char * aHostName,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl);
extern otError OT_API_REAL_NAME(otDnsBrowseResponseGetServiceInfo)(const otDnsBrowseResponse * aResponse,const char * aInstanceLabel,otDnsServiceInfo * aServiceInfo);
extern otError OT_API_REAL_NAME(otDnsBrowseResponseGetServiceInstance)(const otDnsBrowseResponse * aResponse,uint16_t aIndex,char * aLabelBuffer,uint8_t aLabelBufferSize);
extern otError OT_API_REAL_NAME(otDnsBrowseResponseGetServiceName)(const otDnsBrowseResponse * aResponse,char * aNameBuffer,uint16_t aNameBufferSize);
extern otError OT_API_REAL_NAME(otDnsClientBrowse)(otInstance * aInstance,const char * aServiceName,otDnsBrowseCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig);
extern otError OT_API_REAL_NAME(otDnsClientResolveAddress)(otInstance * aInstance,const char * aHostName,otDnsAddressCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig);
extern otError OT_API_REAL_NAME(otDnsClientResolveIp4Address)(otInstance * aInstance,const char * aHostName,otDnsAddressCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig);
extern otError OT_API_REAL_NAME(otDnsClientResolveService)(otInstance * aInstance,const char * aInstanceLabel,const char * aServiceName,otDnsServiceCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig);
extern otError OT_API_REAL_NAME(otDnsClientResolveServiceAndHostAddress)(otInstance * aInstance,const char * aInstanceLabel,const char * aServiceName,otDnsServiceCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig);
extern otError OT_API_REAL_NAME(otDnsServiceResponseGetHostAddress)(const otDnsServiceResponse * aResponse,const char * aHostName,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl);
extern otError OT_API_REAL_NAME(otDnsServiceResponseGetServiceInfo)(const otDnsServiceResponse * aResponse,otDnsServiceInfo * aServiceInfo);
extern otError OT_API_REAL_NAME(otDnsServiceResponseGetServiceName)(const otDnsServiceResponse * aResponse,char * aLabelBuffer,uint8_t aLabelBufferSize,char * aNameBuffer,uint16_t aNameBufferSize);
extern void OT_API_REAL_NAME(otDnsClientSetDefaultConfig)(otInstance * aInstance,const otDnsQueryConfig * aConfig);

const otDnsQueryConfig *OT_API_WRAPPER_NAME(otDnsClientGetDefaultConfig)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otDnsQueryConfig *ret = OT_API_REAL_NAME(otDnsClientGetDefaultConfig)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsAddressResponseGetAddress)(const otDnsAddressResponse * aResponse,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsAddressResponseGetAddress)(aResponse, aIndex, aAddress, aTtl);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsAddressResponseGetHostName)(const otDnsAddressResponse * aResponse,char * aNameBuffer,uint16_t aNameBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsAddressResponseGetHostName)(aResponse, aNameBuffer, aNameBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsBrowseResponseGetHostAddress)(const otDnsBrowseResponse * aResponse,const char * aHostName,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsBrowseResponseGetHostAddress)(aResponse, aHostName, aIndex, aAddress, aTtl);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsBrowseResponseGetServiceInfo)(const otDnsBrowseResponse * aResponse,const char * aInstanceLabel,otDnsServiceInfo * aServiceInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsBrowseResponseGetServiceInfo)(aResponse, aInstanceLabel, aServiceInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsBrowseResponseGetServiceInstance)(const otDnsBrowseResponse * aResponse,uint16_t aIndex,char * aLabelBuffer,uint8_t aLabelBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsBrowseResponseGetServiceInstance)(aResponse, aIndex, aLabelBuffer, aLabelBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsBrowseResponseGetServiceName)(const otDnsBrowseResponse * aResponse,char * aNameBuffer,uint16_t aNameBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsBrowseResponseGetServiceName)(aResponse, aNameBuffer, aNameBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsClientBrowse)(otInstance * aInstance,const char * aServiceName,otDnsBrowseCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsClientBrowse)(aInstance, aServiceName, aCallback, aContext, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsClientResolveAddress)(otInstance * aInstance,const char * aHostName,otDnsAddressCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsClientResolveAddress)(aInstance, aHostName, aCallback, aContext, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsClientResolveIp4Address)(otInstance * aInstance,const char * aHostName,otDnsAddressCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsClientResolveIp4Address)(aInstance, aHostName, aCallback, aContext, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsClientResolveService)(otInstance * aInstance,const char * aInstanceLabel,const char * aServiceName,otDnsServiceCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsClientResolveService)(aInstance, aInstanceLabel, aServiceName, aCallback, aContext, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsClientResolveServiceAndHostAddress)(otInstance * aInstance,const char * aInstanceLabel,const char * aServiceName,otDnsServiceCallback aCallback,void * aContext,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsClientResolveServiceAndHostAddress)(aInstance, aInstanceLabel, aServiceName, aCallback, aContext, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsServiceResponseGetHostAddress)(const otDnsServiceResponse * aResponse,const char * aHostName,uint16_t aIndex,otIp6Address * aAddress,uint32_t * aTtl)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsServiceResponseGetHostAddress)(aResponse, aHostName, aIndex, aAddress, aTtl);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsServiceResponseGetServiceInfo)(const otDnsServiceResponse * aResponse,otDnsServiceInfo * aServiceInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsServiceResponseGetServiceInfo)(aResponse, aServiceInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otDnsServiceResponseGetServiceName)(const otDnsServiceResponse * aResponse,char * aLabelBuffer,uint8_t aLabelBufferSize,char * aNameBuffer,uint16_t aNameBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otDnsServiceResponseGetServiceName)(aResponse, aLabelBuffer, aLabelBufferSize, aNameBuffer, aNameBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otDnsClientSetDefaultConfig)(otInstance * aInstance,const otDnsQueryConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otDnsClientSetDefaultConfig)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
}

