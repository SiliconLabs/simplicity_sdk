/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Backbone Router Ftd APIs
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

#include <openthread/backbone_router_ftd.h>
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

extern otBackboneRouterState OT_API_REAL_NAME(otBackboneRouterGetState)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBackboneRouterGetDomainPrefix)(otInstance * aInstance,otBorderRouterConfig * aConfig);
extern otError OT_API_REAL_NAME(otBackboneRouterGetNdProxyInfo)(otInstance * aInstance,const otIp6Address * aDua,otBackboneRouterNdProxyInfo * aNdProxyInfo);
extern otError OT_API_REAL_NAME(otBackboneRouterMulticastListenerAdd)(otInstance * aInstance,const otIp6Address * aAddress,uint32_t aTimeout);
extern otError OT_API_REAL_NAME(otBackboneRouterMulticastListenerGetNext)(otInstance * aInstance,otBackboneRouterMulticastListenerIterator * aIterator,otBackboneRouterMulticastListenerInfo * aListenerInfo);
extern otError OT_API_REAL_NAME(otBackboneRouterRegister)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBackboneRouterSetConfig)(otInstance * aInstance,const otBackboneRouterConfig * aConfig);
extern uint8_t OT_API_REAL_NAME(otBackboneRouterGetRegistrationJitter)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otBackboneRouterConfigNextDuaRegistrationResponse)(otInstance * aInstance,const otIp6InterfaceIdentifier * aMlIid,uint8_t aStatus);
extern void OT_API_REAL_NAME(otBackboneRouterConfigNextMulticastListenerRegistrationResponse)(otInstance * aInstance,uint8_t aStatus);
extern void OT_API_REAL_NAME(otBackboneRouterGetConfig)(otInstance * aInstance,otBackboneRouterConfig * aConfig);
extern void OT_API_REAL_NAME(otBackboneRouterMulticastListenerClear)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otBackboneRouterSetDomainPrefixCallback)(otInstance * aInstance,otBackboneRouterDomainPrefixCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otBackboneRouterSetEnabled)(otInstance * aInstance,bool aEnable);
extern void OT_API_REAL_NAME(otBackboneRouterSetMulticastListenerCallback)(otInstance * aInstance,otBackboneRouterMulticastListenerCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otBackboneRouterSetNdProxyCallback)(otInstance * aInstance,otBackboneRouterNdProxyCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otBackboneRouterSetRegistrationJitter)(otInstance * aInstance,uint8_t aJitter);

otBackboneRouterState OT_API_WRAPPER_NAME(otBackboneRouterGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBackboneRouterState ret = OT_API_REAL_NAME(otBackboneRouterGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterGetDomainPrefix)(otInstance * aInstance,otBorderRouterConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterGetDomainPrefix)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterGetNdProxyInfo)(otInstance * aInstance,const otIp6Address * aDua,otBackboneRouterNdProxyInfo * aNdProxyInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterGetNdProxyInfo)(aInstance, aDua, aNdProxyInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterMulticastListenerAdd)(otInstance * aInstance,const otIp6Address * aAddress,uint32_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterMulticastListenerAdd)(aInstance, aAddress, aTimeout);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterMulticastListenerGetNext)(otInstance * aInstance,otBackboneRouterMulticastListenerIterator * aIterator,otBackboneRouterMulticastListenerInfo * aListenerInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterMulticastListenerGetNext)(aInstance, aIterator, aListenerInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterRegister)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterRegister)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBackboneRouterSetConfig)(otInstance * aInstance,const otBackboneRouterConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBackboneRouterSetConfig)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otBackboneRouterGetRegistrationJitter)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otBackboneRouterGetRegistrationJitter)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otBackboneRouterConfigNextDuaRegistrationResponse)(otInstance * aInstance,const otIp6InterfaceIdentifier * aMlIid,uint8_t aStatus)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterConfigNextDuaRegistrationResponse)(aInstance, aMlIid, aStatus);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterConfigNextMulticastListenerRegistrationResponse)(otInstance * aInstance,uint8_t aStatus)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterConfigNextMulticastListenerRegistrationResponse)(aInstance, aStatus);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterGetConfig)(otInstance * aInstance,otBackboneRouterConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterGetConfig)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterMulticastListenerClear)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterMulticastListenerClear)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterSetDomainPrefixCallback)(otInstance * aInstance,otBackboneRouterDomainPrefixCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterSetDomainPrefixCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterSetEnabled)(otInstance * aInstance,bool aEnable)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterSetEnabled)(aInstance, aEnable);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterSetMulticastListenerCallback)(otInstance * aInstance,otBackboneRouterMulticastListenerCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterSetMulticastListenerCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterSetNdProxyCallback)(otInstance * aInstance,otBackboneRouterNdProxyCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterSetNdProxyCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBackboneRouterSetRegistrationJitter)(otInstance * aInstance,uint8_t aJitter)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBackboneRouterSetRegistrationJitter)(aInstance, aJitter);
    sl_ot_rtos_release_stack_mutex();
}

