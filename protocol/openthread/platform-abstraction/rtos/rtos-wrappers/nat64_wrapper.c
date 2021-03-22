/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Nat64 APIs
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

#include <openthread/nat64.h>
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

extern bool OT_API_REAL_NAME(otIp4IsAddressEqual)(const otIp4Address * aFirst,const otIp4Address * aSecond);
extern otError OT_API_REAL_NAME(otIp4AddressFromString)(const char * aString,otIp4Address * aAddress);
extern otError OT_API_REAL_NAME(otIp4CidrFromString)(const char * aString,otIp4Cidr * aCidr);
extern otError OT_API_REAL_NAME(otIp4FromIp4MappedIp6Address)(const otIp6Address * aIp6Address,otIp4Address * aIp4Address);
extern otError OT_API_REAL_NAME(otNat64GetCidr)(otInstance * aInstance,otIp4Cidr * aCidr);
extern otError OT_API_REAL_NAME(otNat64GetNextAddressMapping)(otInstance * aInstance,otNat64AddressMappingIterator * aIterator,otNat64AddressMapping * aMapping);
extern otError OT_API_REAL_NAME(otNat64Send)(otInstance * aInstance,otMessage * aMessage);
extern otError OT_API_REAL_NAME(otNat64SetIp4Cidr)(otInstance * aInstance,const otIp4Cidr * aCidr);
extern otError OT_API_REAL_NAME(otNat64SynthesizeIp6Address)(otInstance * aInstance,const otIp4Address * aIp4Address,otIp6Address * aIp6Address);
extern otMessage *OT_API_REAL_NAME(otIp4NewMessage)(otInstance * aInstance,const otMessageSettings * aSettings);
extern otNat64State OT_API_REAL_NAME(otNat64GetPrefixManagerState)(otInstance * aInstance);
extern otNat64State OT_API_REAL_NAME(otNat64GetTranslatorState)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otIp4AddressToString)(const otIp4Address * aAddress,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otIp4CidrToString)(const otIp4Cidr * aCidr,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otIp4ExtractFromIp6Address)(uint8_t aPrefixLength,const otIp6Address * aIp6Address,otIp4Address * aIp4Address);
extern void OT_API_REAL_NAME(otIp4ToIp4MappedIp6Address)(const otIp4Address * aIp4Address,otIp6Address * aIp6Address);
extern void OT_API_REAL_NAME(otNat64GetCounters)(otInstance * aInstance,otNat64ProtocolCounters * aCounters);
extern void OT_API_REAL_NAME(otNat64GetErrorCounters)(otInstance * aInstance,otNat64ErrorCounters * aCounters);
extern void OT_API_REAL_NAME(otNat64InitAddressMappingIterator)(otInstance * aInstance,otNat64AddressMappingIterator * aIterator);
extern void OT_API_REAL_NAME(otNat64SetEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otNat64SetReceiveIp4Callback)(otInstance * aInstance,otNat64ReceiveIp4Callback aCallback,void * aContext);

bool OT_API_WRAPPER_NAME(otIp4IsAddressEqual)(const otIp4Address * aFirst,const otIp4Address * aSecond)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otIp4IsAddressEqual)(aFirst, aSecond);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp4AddressFromString)(const char * aString,otIp4Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp4AddressFromString)(aString, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp4CidrFromString)(const char * aString,otIp4Cidr * aCidr)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp4CidrFromString)(aString, aCidr);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otIp4FromIp4MappedIp6Address)(const otIp6Address * aIp6Address,otIp4Address * aIp4Address)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otIp4FromIp4MappedIp6Address)(aIp6Address, aIp4Address);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNat64GetCidr)(otInstance * aInstance,otIp4Cidr * aCidr)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNat64GetCidr)(aInstance, aCidr);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNat64GetNextAddressMapping)(otInstance * aInstance,otNat64AddressMappingIterator * aIterator,otNat64AddressMapping * aMapping)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNat64GetNextAddressMapping)(aInstance, aIterator, aMapping);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNat64Send)(otInstance * aInstance,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNat64Send)(aInstance, aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNat64SetIp4Cidr)(otInstance * aInstance,const otIp4Cidr * aCidr)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNat64SetIp4Cidr)(aInstance, aCidr);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNat64SynthesizeIp6Address)(otInstance * aInstance,const otIp4Address * aIp4Address,otIp6Address * aIp6Address)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNat64SynthesizeIp6Address)(aInstance, aIp4Address, aIp6Address);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otIp4NewMessage)(otInstance * aInstance,const otMessageSettings * aSettings)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otIp4NewMessage)(aInstance, aSettings);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otNat64State OT_API_WRAPPER_NAME(otNat64GetPrefixManagerState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otNat64State ret = OT_API_REAL_NAME(otNat64GetPrefixManagerState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otNat64State OT_API_WRAPPER_NAME(otNat64GetTranslatorState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otNat64State ret = OT_API_REAL_NAME(otNat64GetTranslatorState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otIp4AddressToString)(const otIp4Address * aAddress,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp4AddressToString)(aAddress, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp4CidrToString)(const otIp4Cidr * aCidr,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp4CidrToString)(aCidr, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp4ExtractFromIp6Address)(uint8_t aPrefixLength,const otIp6Address * aIp6Address,otIp4Address * aIp4Address)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp4ExtractFromIp6Address)(aPrefixLength, aIp6Address, aIp4Address);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otIp4ToIp4MappedIp6Address)(const otIp4Address * aIp4Address,otIp6Address * aIp6Address)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otIp4ToIp4MappedIp6Address)(aIp4Address, aIp6Address);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNat64GetCounters)(otInstance * aInstance,otNat64ProtocolCounters * aCounters)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNat64GetCounters)(aInstance, aCounters);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNat64GetErrorCounters)(otInstance * aInstance,otNat64ErrorCounters * aCounters)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNat64GetErrorCounters)(aInstance, aCounters);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNat64InitAddressMappingIterator)(otInstance * aInstance,otNat64AddressMappingIterator * aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNat64InitAddressMappingIterator)(aInstance, aIterator);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNat64SetEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNat64SetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNat64SetReceiveIp4Callback)(otInstance * aInstance,otNat64ReceiveIp4Callback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNat64SetReceiveIp4Callback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

