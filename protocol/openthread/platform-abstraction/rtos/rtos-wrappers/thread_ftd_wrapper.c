/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Thread Ftd APIs
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

#include <openthread/thread_ftd.h>
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

extern bool OT_API_REAL_NAME(otThreadIsRouterEligible)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otThreadIsRouterIdAllocated)(otInstance * aInstance,uint8_t aRouterId);
extern bool OT_API_REAL_NAME(otThreadIsTmfOriginFilterEnabled)(otInstance * aInstance);
extern const otDeviceProperties *OT_API_REAL_NAME(otThreadGetDeviceProperties)(otInstance * aInstance);
extern int8_t OT_API_REAL_NAME(otThreadGetParentPriority)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadBecomeLeader)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadBecomeRouter)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadGetChildInfoById)(otInstance * aInstance,uint16_t aChildId,otChildInfo * aChildInfo);
extern otError OT_API_REAL_NAME(otThreadGetChildInfoByIndex)(otInstance * aInstance,uint16_t aChildIndex,otChildInfo * aChildInfo);
extern otError OT_API_REAL_NAME(otThreadGetChildNextIp6Address)(otInstance * aInstance,uint16_t aChildIndex,otChildIp6AddressIterator * aIterator,otIp6Address * aAddress);
extern otError OT_API_REAL_NAME(otThreadGetNextCacheEntry)(otInstance * aInstance,otCacheEntryInfo * aEntryInfo,otCacheEntryIterator * aIterator);
extern otError OT_API_REAL_NAME(otThreadGetRouterInfo)(otInstance * aInstance,uint16_t aRouterId,otRouterInfo * aRouterInfo);
extern otError OT_API_REAL_NAME(otThreadReleaseRouterId)(otInstance * aInstance,uint8_t aRouterId);
extern otError OT_API_REAL_NAME(otThreadSetChildRouterLinks)(otInstance * aInstance,uint8_t aChildRouterLinks);
extern otError OT_API_REAL_NAME(otThreadSetJoinerUdpPort)(otInstance * aInstance,uint16_t aJoinerUdpPort);
extern otError OT_API_REAL_NAME(otThreadSetMaxAllowedChildren)(otInstance * aInstance,uint16_t aMaxChildren);
extern otError OT_API_REAL_NAME(otThreadSetMaxChildIpAddresses)(otInstance * aInstance,uint8_t aMaxIpAddresses);
extern otError OT_API_REAL_NAME(otThreadSetParentPriority)(otInstance * aInstance,int8_t aParentPriority);
extern otError OT_API_REAL_NAME(otThreadSetPreferredRouterId)(otInstance * aInstance,uint8_t aRouterId);
extern otError OT_API_REAL_NAME(otThreadSetPskc)(otInstance * aInstance,const otPskc * aPskc);
extern otError OT_API_REAL_NAME(otThreadSetPskcRef)(otInstance * aInstance,otPskcRef aKeyRef);
extern otError OT_API_REAL_NAME(otThreadSetRouterEligible)(otInstance * aInstance,bool aEligible);
extern otError OT_API_REAL_NAME(otThreadSetRouterIdRange)(otInstance * aInstance,uint8_t aMinRouterId,uint8_t aMaxRouterId);
extern otPskcRef OT_API_REAL_NAME(otThreadGetPskcRef)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otThreadGetJoinerUdpPort)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otThreadGetMaxAllowedChildren)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetAdvertisementTrickleIntervalMax)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetContextIdReuseDelay)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetPreferredLeaderPartitionId)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetChildRouterLinks)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetLocalLeaderWeight)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetMaxChildIpAddresses)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetMaxRouterId)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetNetworkIdTimeout)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetRouterDowngradeThreshold)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetRouterIdSequence)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetRouterSelectionJitter)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetRouterUpgradeThreshold)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otThreadGetNextHopAndPathCost)(otInstance * aInstance,uint16_t aDestRloc16,uint16_t * aNextHopRloc16,uint8_t * aPathCost);
extern void OT_API_REAL_NAME(otThreadGetPskc)(otInstance * aInstance,otPskc * aPskc);
extern void OT_API_REAL_NAME(otThreadGetRouterIdRange)(otInstance * aInstance,uint8_t * aMinRouterId,uint8_t * aMaxRouterId);
extern void OT_API_REAL_NAME(otThreadRegisterNeighborTableCallback)(otInstance * aInstance,otNeighborTableCallback aCallback);
extern void OT_API_REAL_NAME(otThreadSetCcmEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otThreadSetContextIdReuseDelay)(otInstance * aInstance,uint32_t aDelay);
extern void OT_API_REAL_NAME(otThreadSetDeviceProperties)(otInstance * aInstance,const otDeviceProperties * aDeviceProperties);
extern void OT_API_REAL_NAME(otThreadSetLocalLeaderWeight)(otInstance * aInstance,uint8_t aWeight);
extern void OT_API_REAL_NAME(otThreadSetNetworkIdTimeout)(otInstance * aInstance,uint8_t aTimeout);
extern void OT_API_REAL_NAME(otThreadSetPreferredLeaderPartitionId)(otInstance * aInstance,uint32_t aPartitionId);
extern void OT_API_REAL_NAME(otThreadSetRouterDowngradeThreshold)(otInstance * aInstance,uint8_t aThreshold);
extern void OT_API_REAL_NAME(otThreadSetRouterSelectionJitter)(otInstance * aInstance,uint8_t aRouterJitter);
extern void OT_API_REAL_NAME(otThreadSetRouterUpgradeThreshold)(otInstance * aInstance,uint8_t aThreshold);
extern void OT_API_REAL_NAME(otThreadSetSteeringData)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern void OT_API_REAL_NAME(otThreadSetThreadVersionCheckEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otThreadSetTmfOriginFilterEnabled)(otInstance * aInstance,bool aEnabled);

bool OT_API_WRAPPER_NAME(otThreadIsRouterEligible)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsRouterEligible)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otThreadIsRouterIdAllocated)(otInstance * aInstance,uint8_t aRouterId)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsRouterIdAllocated)(aInstance, aRouterId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otThreadIsTmfOriginFilterEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsTmfOriginFilterEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otDeviceProperties *OT_API_WRAPPER_NAME(otThreadGetDeviceProperties)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otDeviceProperties *ret = OT_API_REAL_NAME(otThreadGetDeviceProperties)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int8_t OT_API_WRAPPER_NAME(otThreadGetParentPriority)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    int8_t ret = OT_API_REAL_NAME(otThreadGetParentPriority)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadBecomeLeader)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadBecomeLeader)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadBecomeRouter)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadBecomeRouter)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetChildInfoById)(otInstance * aInstance,uint16_t aChildId,otChildInfo * aChildInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetChildInfoById)(aInstance, aChildId, aChildInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetChildInfoByIndex)(otInstance * aInstance,uint16_t aChildIndex,otChildInfo * aChildInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetChildInfoByIndex)(aInstance, aChildIndex, aChildInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetChildNextIp6Address)(otInstance * aInstance,uint16_t aChildIndex,otChildIp6AddressIterator * aIterator,otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetChildNextIp6Address)(aInstance, aChildIndex, aIterator, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetNextCacheEntry)(otInstance * aInstance,otCacheEntryInfo * aEntryInfo,otCacheEntryIterator * aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetNextCacheEntry)(aInstance, aEntryInfo, aIterator);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetRouterInfo)(otInstance * aInstance,uint16_t aRouterId,otRouterInfo * aRouterInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetRouterInfo)(aInstance, aRouterId, aRouterInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadReleaseRouterId)(otInstance * aInstance,uint8_t aRouterId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadReleaseRouterId)(aInstance, aRouterId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetChildRouterLinks)(otInstance * aInstance,uint8_t aChildRouterLinks)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetChildRouterLinks)(aInstance, aChildRouterLinks);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetJoinerUdpPort)(otInstance * aInstance,uint16_t aJoinerUdpPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetJoinerUdpPort)(aInstance, aJoinerUdpPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetMaxAllowedChildren)(otInstance * aInstance,uint16_t aMaxChildren)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetMaxAllowedChildren)(aInstance, aMaxChildren);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetMaxChildIpAddresses)(otInstance * aInstance,uint8_t aMaxIpAddresses)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetMaxChildIpAddresses)(aInstance, aMaxIpAddresses);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetParentPriority)(otInstance * aInstance,int8_t aParentPriority)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetParentPriority)(aInstance, aParentPriority);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetPreferredRouterId)(otInstance * aInstance,uint8_t aRouterId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetPreferredRouterId)(aInstance, aRouterId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetPskc)(otInstance * aInstance,const otPskc * aPskc)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetPskc)(aInstance, aPskc);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetPskcRef)(otInstance * aInstance,otPskcRef aKeyRef)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetPskcRef)(aInstance, aKeyRef);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetRouterEligible)(otInstance * aInstance,bool aEligible)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetRouterEligible)(aInstance, aEligible);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetRouterIdRange)(otInstance * aInstance,uint8_t aMinRouterId,uint8_t aMaxRouterId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetRouterIdRange)(aInstance, aMinRouterId, aMaxRouterId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otPskcRef OT_API_WRAPPER_NAME(otThreadGetPskcRef)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otPskcRef ret = OT_API_REAL_NAME(otThreadGetPskcRef)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otThreadGetJoinerUdpPort)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otThreadGetJoinerUdpPort)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otThreadGetMaxAllowedChildren)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otThreadGetMaxAllowedChildren)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetAdvertisementTrickleIntervalMax)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetAdvertisementTrickleIntervalMax)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetContextIdReuseDelay)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetContextIdReuseDelay)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetPreferredLeaderPartitionId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetPreferredLeaderPartitionId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetChildRouterLinks)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetChildRouterLinks)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetLocalLeaderWeight)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetLocalLeaderWeight)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetMaxChildIpAddresses)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetMaxChildIpAddresses)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetMaxRouterId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetMaxRouterId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetNetworkIdTimeout)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetNetworkIdTimeout)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetRouterDowngradeThreshold)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetRouterDowngradeThreshold)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetRouterIdSequence)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetRouterIdSequence)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetRouterSelectionJitter)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetRouterSelectionJitter)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetRouterUpgradeThreshold)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetRouterUpgradeThreshold)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otThreadGetNextHopAndPathCost)(otInstance * aInstance,uint16_t aDestRloc16,uint16_t * aNextHopRloc16,uint8_t * aPathCost)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadGetNextHopAndPathCost)(aInstance, aDestRloc16, aNextHopRloc16, aPathCost);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadGetPskc)(otInstance * aInstance,otPskc * aPskc)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadGetPskc)(aInstance, aPskc);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadGetRouterIdRange)(otInstance * aInstance,uint8_t * aMinRouterId,uint8_t * aMaxRouterId)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadGetRouterIdRange)(aInstance, aMinRouterId, aMaxRouterId);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadRegisterNeighborTableCallback)(otInstance * aInstance,otNeighborTableCallback aCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadRegisterNeighborTableCallback)(aInstance, aCallback);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetCcmEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetCcmEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetContextIdReuseDelay)(otInstance * aInstance,uint32_t aDelay)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetContextIdReuseDelay)(aInstance, aDelay);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetDeviceProperties)(otInstance * aInstance,const otDeviceProperties * aDeviceProperties)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetDeviceProperties)(aInstance, aDeviceProperties);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetLocalLeaderWeight)(otInstance * aInstance,uint8_t aWeight)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetLocalLeaderWeight)(aInstance, aWeight);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetNetworkIdTimeout)(otInstance * aInstance,uint8_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetNetworkIdTimeout)(aInstance, aTimeout);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetPreferredLeaderPartitionId)(otInstance * aInstance,uint32_t aPartitionId)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetPreferredLeaderPartitionId)(aInstance, aPartitionId);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetRouterDowngradeThreshold)(otInstance * aInstance,uint8_t aThreshold)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetRouterDowngradeThreshold)(aInstance, aThreshold);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetRouterSelectionJitter)(otInstance * aInstance,uint8_t aRouterJitter)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetRouterSelectionJitter)(aInstance, aRouterJitter);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetRouterUpgradeThreshold)(otInstance * aInstance,uint8_t aThreshold)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetRouterUpgradeThreshold)(aInstance, aThreshold);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetSteeringData)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetSteeringData)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetThreadVersionCheckEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetThreadVersionCheckEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetTmfOriginFilterEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetTmfOriginFilterEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

