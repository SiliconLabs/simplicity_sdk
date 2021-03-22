/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Ip6 APIs
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

#include <openthread/ip6.h>
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

extern bool OT_API_REAL_NAME(otIp6ArePrefixesEqual)(const otIp6Prefix * aFirst,const otIp6Prefix * aSecond);
extern bool OT_API_REAL_NAME(otIp6HasUnicastAddress)(otInstance * aInstance,const otIp6Address * aAddress);
extern bool OT_API_REAL_NAME(otIp6IsAddressEqual)(const otIp6Address * aFirst,const otIp6Address * aSecond);
extern bool OT_API_REAL_NAME(otIp6IsAddressUnspecified)(const otIp6Address * aAddress);
extern bool OT_API_REAL_NAME(otIp6IsEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otIp6IsMulticastPromiscuousEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otIp6IsReceiveFilterEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otIp6IsSlaacEnabled)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otIp6ProtoToString)(uint8_t aIpProto);
extern const otBorderRoutingCounters *OT_API_REAL_NAME(otIp6GetBorderRoutingCounters)(otInstance * aInstance);
extern const otNetifAddress *OT_API_REAL_NAME(otIp6GetUnicastAddresses)(otInstance * aInstance);
extern const otNetifMulticastAddress *OT_API_REAL_NAME(otIp6GetMulticastAddresses)(otInstance * aInstance);
extern const uint16_t *OT_API_REAL_NAME(otIp6GetUnsecurePorts)(otInstance * aInstance,uint8_t * aNumEntries);
extern otError OT_API_REAL_NAME(otIp6AddUnicastAddress)(otInstance * aInstance,const otNetifAddress * aAddress);
extern otError OT_API_REAL_NAME(otIp6AddUnsecurePort)(otInstance * aInstance,uint16_t aPort);
extern otError OT_API_REAL_NAME(otIp6AddressFromString)(const char * aString,otIp6Address * aAddress);
extern otError OT_API_REAL_NAME(otIp6PrefixFromString)(const char * aString,otIp6Prefix * aPrefix);
extern otError OT_API_REAL_NAME(otIp6RegisterMulticastListeners)(otInstance * aInstance,const otIp6Address * aAddresses,uint8_t aAddressNum,const uint32_t * aTimeout,otIp6RegisterMulticastListenersCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otIp6RemoveUnicastAddress)(otInstance * aInstance,const otIp6Address * aAddress);
extern otError OT_API_REAL_NAME(otIp6RemoveUnsecurePort)(otInstance * aInstance,uint16_t aPort);
extern otError OT_API_REAL_NAME(otIp6SelectSourceAddress)(otInstance * aInstance,otMessageInfo * aMessageInfo);
extern otError OT_API_REAL_NAME(otIp6Send)(otInstance * aInstance,otMessage * aMessage);
extern otError OT_API_REAL_NAME(otIp6SetEnabled)(otInstance * aInstance,bool aEnabled);
extern otError OT_API_REAL_NAME(otIp6SetMeshLocalIid)(otInstance * aInstance,const otIp6InterfaceIdentifier * aIid);
extern otError OT_API_REAL_NAME(otIp6SubscribeMulticastAddress)(otInstance * aInstance,const otIp6Address * aAddress);
extern otError OT_API_REAL_NAME(otIp6UnsubscribeMulticastAddress)(otInstance * aInstance,const otIp6Address * aAddress);
extern otMessage *OT_API_REAL_NAME(otIp6NewMessage)(otInstance * aInstance,const otMessageSettings * aSettings);
extern otMessage *OT_API_REAL_NAME(otIp6NewMessageFromBuffer)(otInstance * aInstance,const uint8_t * aData,uint16_t aDataLength,const otMessageSettings * aSettings);
extern uint8_t OT_API_REAL_NAME(otIp6PrefixMatch)(const otIp6Address * aFirst,const otIp6Address * aSecond);
extern void OT_API_REAL_NAME(otIp6AddressToString)(const otIp6Address * aAddress,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otIp6GetPrefix)(const otIp6Address * aAddress,uint8_t aLength,otIp6Prefix * aPrefix);
extern void OT_API_REAL_NAME(otIp6PrefixToString)(const otIp6Prefix * aPrefix,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otIp6RemoveAllUnsecurePorts)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otIp6ResetBorderRoutingCounters)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otIp6SetAddressCallback)(otInstance * aInstance,otIp6AddressCallback aCallback,void * aCallbackContext);
extern void OT_API_REAL_NAME(otIp6SetMulticastPromiscuousEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otIp6SetReceiveCallback)(otInstance * aInstance,otIp6ReceiveCallback aCallback,void * aCallbackContext);
extern void OT_API_REAL_NAME(otIp6SetReceiveFilterEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otIp6SetSlaacEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otIp6SetSlaacPrefixFilter)(otInstance * aInstance,otIp6SlaacPrefixFilter aFilter);
extern void OT_API_REAL_NAME(otIp6SockAddrToString)(const otSockAddr * aSockAddr,char * aBuffer,uint16_t aSize);

bool OT_API_WRAPPER_NAME(otIp6ArePrefixesEqual)(const otIp6Prefix * aFirst,const otIp6Prefix * aSecond)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6ArePrefixesEqual)(aFirst, aSecond);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6HasUnicastAddress)(otInstance * aInstance,const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6HasUnicastAddress)(aInstance, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsAddressEqual)(const otIp6Address * aFirst,const otIp6Address * aSecond)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsAddressEqual)(aFirst, aSecond);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsAddressUnspecified)(const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsAddressUnspecified)(aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsMulticastPromiscuousEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsMulticastPromiscuousEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsReceiveFilterEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsReceiveFilterEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otIp6IsSlaacEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp6IsSlaacEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otIp6ProtoToString)(uint8_t aIpProto)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otIp6ProtoToString)(aIpProto);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otBorderRoutingCounters *OT_API_WRAPPER_NAME(otIp6GetBorderRoutingCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otBorderRoutingCounters *ret = OT_API_REAL_NAME(otIp6GetBorderRoutingCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otNetifAddress *OT_API_WRAPPER_NAME(otIp6GetUnicastAddresses)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otNetifAddress *ret = OT_API_REAL_NAME(otIp6GetUnicastAddresses)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otNetifMulticastAddress *OT_API_WRAPPER_NAME(otIp6GetMulticastAddresses)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otNetifMulticastAddress *ret = OT_API_REAL_NAME(otIp6GetMulticastAddresses)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint16_t *OT_API_WRAPPER_NAME(otIp6GetUnsecurePorts)(otInstance * aInstance,uint8_t * aNumEntries)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint16_t *ret = OT_API_REAL_NAME(otIp6GetUnsecurePorts)(aInstance, aNumEntries);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6AddUnicastAddress)(otInstance * aInstance,const otNetifAddress * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6AddUnicastAddress)(aInstance, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6AddUnsecurePort)(otInstance * aInstance,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6AddUnsecurePort)(aInstance, aPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6AddressFromString)(const char * aString,otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6AddressFromString)(aString, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6PrefixFromString)(const char * aString,otIp6Prefix * aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6PrefixFromString)(aString, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6RegisterMulticastListeners)(otInstance * aInstance,const otIp6Address * aAddresses,uint8_t aAddressNum,const uint32_t * aTimeout,otIp6RegisterMulticastListenersCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6RegisterMulticastListeners)(aInstance, aAddresses, aAddressNum, aTimeout, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6RemoveUnicastAddress)(otInstance * aInstance,const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6RemoveUnicastAddress)(aInstance, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6RemoveUnsecurePort)(otInstance * aInstance,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6RemoveUnsecurePort)(aInstance, aPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6SelectSourceAddress)(otInstance * aInstance,otMessageInfo * aMessageInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6SelectSourceAddress)(aInstance, aMessageInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6Send)(otInstance * aInstance,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6Send)(aInstance, aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6SetEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6SetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6SetMeshLocalIid)(otInstance * aInstance,const otIp6InterfaceIdentifier * aIid)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6SetMeshLocalIid)(aInstance, aIid);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6SubscribeMulticastAddress)(otInstance * aInstance,const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6SubscribeMulticastAddress)(aInstance, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp6UnsubscribeMulticastAddress)(otInstance * aInstance,const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp6UnsubscribeMulticastAddress)(aInstance, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otIp6NewMessage)(otInstance * aInstance,const otMessageSettings * aSettings)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otIp6NewMessage)(aInstance, aSettings);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otIp6NewMessageFromBuffer)(otInstance * aInstance,const uint8_t * aData,uint16_t aDataLength,const otMessageSettings * aSettings)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otIp6NewMessageFromBuffer)(aInstance, aData, aDataLength, aSettings);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otIp6PrefixMatch)(const otIp6Address * aFirst,const otIp6Address * aSecond)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otIp6PrefixMatch)(aFirst, aSecond);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otIp6AddressToString)(const otIp6Address * aAddress,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6AddressToString)(aAddress, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6GetPrefix)(const otIp6Address * aAddress,uint8_t aLength,otIp6Prefix * aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6GetPrefix)(aAddress, aLength, aPrefix);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6PrefixToString)(const otIp6Prefix * aPrefix,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6PrefixToString)(aPrefix, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6RemoveAllUnsecurePorts)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6RemoveAllUnsecurePorts)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6ResetBorderRoutingCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6ResetBorderRoutingCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetAddressCallback)(otInstance * aInstance,otIp6AddressCallback aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetAddressCallback)(aInstance, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetMulticastPromiscuousEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetMulticastPromiscuousEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetReceiveCallback)(otInstance * aInstance,otIp6ReceiveCallback aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetReceiveCallback)(aInstance, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetReceiveFilterEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetReceiveFilterEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetSlaacEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetSlaacEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SetSlaacPrefixFilter)(otInstance * aInstance,otIp6SlaacPrefixFilter aFilter)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SetSlaacPrefixFilter)(aInstance, aFilter);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp6SockAddrToString)(const otSockAddr * aSockAddr,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp6SockAddrToString)(aSockAddr, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

