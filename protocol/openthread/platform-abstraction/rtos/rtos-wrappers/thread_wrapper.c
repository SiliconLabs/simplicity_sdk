/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Thread APIs
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

#include <openthread/thread.h>
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

extern bool OT_API_REAL_NAME(otThreadIsAnycastLocateInProgress)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otThreadIsDiscoverInProgress)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otThreadIsSingleton)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otThreadDeviceRoleToString)(otDeviceRole aRole);
extern const char *OT_API_REAL_NAME(otThreadGetDomainName)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otThreadGetNetworkName)(otInstance * aInstance);
extern const otExtendedPanId *OT_API_REAL_NAME(otThreadGetExtendedPanId)(otInstance * aInstance);
extern const otIp6Address *OT_API_REAL_NAME(otThreadGetLinkLocalAllThreadNodesMulticastAddress)(otInstance * aInstance);
extern const otIp6Address *OT_API_REAL_NAME(otThreadGetLinkLocalIp6Address)(otInstance * aInstance);
extern const otIp6Address *OT_API_REAL_NAME(otThreadGetMeshLocalEid)(otInstance * aInstance);
extern const otIp6Address *OT_API_REAL_NAME(otThreadGetRealmLocalAllThreadNodesMulticastAddress)(otInstance * aInstance);
extern const otIp6Address *OT_API_REAL_NAME(otThreadGetRloc)(otInstance * aInstance);
extern const otIp6InterfaceIdentifier *OT_API_REAL_NAME(otThreadGetFixedDuaInterfaceIdentifier)(otInstance * aInstance);
extern const otIpCounters *OT_API_REAL_NAME(otThreadGetIp6Counters)(otInstance * aInstance);
extern const otMeshLocalPrefix *OT_API_REAL_NAME(otThreadGetMeshLocalPrefix)(otInstance * aInstance);
extern const otMleCounters *OT_API_REAL_NAME(otThreadGetMleCounters)(otInstance * aInstance);
extern const uint32_t *OT_API_REAL_NAME(otThreadGetTimeInQueueHistogram)(otInstance * aInstance,uint16_t * aNumBins,uint32_t * aBinInterval);
extern otDeviceRole OT_API_REAL_NAME(otThreadGetDeviceRole)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadBecomeChild)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadBecomeDetached)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadDetachGracefully)(otInstance * aInstance,otDetachGracefullyCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otThreadDiscover)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aPanId,bool aJoiner,bool aEnableEui64Filtering,otHandleActiveScanResult aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otThreadGetLeaderData)(otInstance * aInstance,otLeaderData * aLeaderData);
extern otError OT_API_REAL_NAME(otThreadGetLeaderRloc)(otInstance * aInstance,otIp6Address * aLeaderRloc);
extern otError OT_API_REAL_NAME(otThreadGetNextNeighborInfo)(otInstance * aInstance,otNeighborInfoIterator * aIterator,otNeighborInfo * aInfo);
extern otError OT_API_REAL_NAME(otThreadGetParentAverageRssi)(otInstance * aInstance,int8_t * aParentRssi);
extern otError OT_API_REAL_NAME(otThreadGetParentInfo)(otInstance * aInstance,otRouterInfo * aParentInfo);
extern otError OT_API_REAL_NAME(otThreadGetParentLastRssi)(otInstance * aInstance,int8_t * aLastRssi);
extern otError OT_API_REAL_NAME(otThreadGetServiceAloc)(otInstance * aInstance,uint8_t aServiceId,otIp6Address * aServiceAloc);
extern otError OT_API_REAL_NAME(otThreadLocateAnycastDestination)(otInstance * aInstance,const otIp6Address * aAnycastAddress,otThreadAnycastLocatorCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otThreadSearchForBetterParent)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadSendProactiveBackboneNotification)(otInstance * aInstance,otIp6Address * aTarget,otIp6InterfaceIdentifier * aMlIid,uint32_t aTimeSinceLastTransaction);
extern otError OT_API_REAL_NAME(otThreadSetDomainName)(otInstance * aInstance,const char * aDomainName);
extern otError OT_API_REAL_NAME(otThreadSetEnabled)(otInstance * aInstance,bool aEnabled);
extern otError OT_API_REAL_NAME(otThreadSetExtendedPanId)(otInstance * aInstance,const otExtendedPanId * aExtendedPanId);
extern otError OT_API_REAL_NAME(otThreadSetFixedDuaInterfaceIdentifier)(otInstance * aInstance,const otIp6InterfaceIdentifier * aIid);
extern otError OT_API_REAL_NAME(otThreadSetJoinerAdvertisement)(otInstance * aInstance,uint32_t aOui,const uint8_t * aAdvData,uint8_t aAdvDataLength);
extern otError OT_API_REAL_NAME(otThreadSetLinkMode)(otInstance * aInstance,otLinkModeConfig aConfig);
extern otError OT_API_REAL_NAME(otThreadSetMeshLocalPrefix)(otInstance * aInstance,const otMeshLocalPrefix * aMeshLocalPrefix);
extern otError OT_API_REAL_NAME(otThreadSetNetworkKey)(otInstance * aInstance,const otNetworkKey * aKey);
extern otError OT_API_REAL_NAME(otThreadSetNetworkKeyRef)(otInstance * aInstance,otNetworkKeyRef aKeyRef);
extern otError OT_API_REAL_NAME(otThreadSetNetworkName)(otInstance * aInstance,const char * aNetworkName);
extern otLinkModeConfig OT_API_REAL_NAME(otThreadGetLinkMode)(otInstance * aInstance);
extern otNetworkKeyRef OT_API_REAL_NAME(otThreadGetNetworkKeyRef)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otThreadGetKeySwitchGuardTime)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otThreadGetRloc16)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otThreadGetVersion)(void);
extern uint32_t OT_API_REAL_NAME(otThreadGetChildTimeout)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetKeySequenceCounter)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetMaxTimeInQueue)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otThreadGetPartitionId)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetLeaderRouterId)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otThreadGetLeaderWeight)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otConvertDurationInSecondsToString)(uint32_t aDuration,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otThreadGetNetworkKey)(otInstance * aInstance,otNetworkKey * aNetworkKey);
extern void OT_API_REAL_NAME(otThreadRegisterParentResponseCallback)(otInstance * aInstance,otThreadParentResponseCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otThreadResetIp6Counters)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otThreadResetMleCounters)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otThreadResetTimeInQueueStat)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otThreadSendAddressNotification)(otInstance * aInstance,otIp6Address * aDestination,otIp6Address * aTarget,otIp6InterfaceIdentifier * aMlIid);
extern void OT_API_REAL_NAME(otThreadSetChildTimeout)(otInstance * aInstance,uint32_t aTimeout);
extern void OT_API_REAL_NAME(otThreadSetDiscoveryRequestCallback)(otInstance * aInstance,otThreadDiscoveryRequestCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otThreadSetKeySequenceCounter)(otInstance * aInstance,uint32_t aKeySequenceCounter);
extern void OT_API_REAL_NAME(otThreadSetKeySwitchGuardTime)(otInstance * aInstance,uint16_t aKeySwitchGuardTime);

bool OT_API_WRAPPER_NAME(otThreadIsAnycastLocateInProgress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsAnycastLocateInProgress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otThreadIsDiscoverInProgress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsDiscoverInProgress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otThreadIsSingleton)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otThreadIsSingleton)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadDeviceRoleToString)(otDeviceRole aRole)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadDeviceRoleToString)(aRole);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadGetDomainName)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetDomainName)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadGetNetworkName)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetNetworkName)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otExtendedPanId *OT_API_WRAPPER_NAME(otThreadGetExtendedPanId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otExtendedPanId *ret = OT_API_REAL_NAME(otThreadGetExtendedPanId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otThreadGetLinkLocalAllThreadNodesMulticastAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otThreadGetLinkLocalAllThreadNodesMulticastAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otThreadGetLinkLocalIp6Address)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otThreadGetLinkLocalIp6Address)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otThreadGetMeshLocalEid)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otThreadGetMeshLocalEid)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otThreadGetRealmLocalAllThreadNodesMulticastAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otThreadGetRealmLocalAllThreadNodesMulticastAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6Address *OT_API_WRAPPER_NAME(otThreadGetRloc)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6Address *ret = OT_API_REAL_NAME(otThreadGetRloc)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIp6InterfaceIdentifier *OT_API_WRAPPER_NAME(otThreadGetFixedDuaInterfaceIdentifier)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIp6InterfaceIdentifier *ret = OT_API_REAL_NAME(otThreadGetFixedDuaInterfaceIdentifier)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otIpCounters *OT_API_WRAPPER_NAME(otThreadGetIp6Counters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otIpCounters *ret = OT_API_REAL_NAME(otThreadGetIp6Counters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otMeshLocalPrefix *OT_API_WRAPPER_NAME(otThreadGetMeshLocalPrefix)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otMeshLocalPrefix *ret = OT_API_REAL_NAME(otThreadGetMeshLocalPrefix)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otMleCounters *OT_API_WRAPPER_NAME(otThreadGetMleCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otMleCounters *ret = OT_API_REAL_NAME(otThreadGetMleCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint32_t *OT_API_WRAPPER_NAME(otThreadGetTimeInQueueHistogram)(otInstance * aInstance,uint16_t * aNumBins,uint32_t * aBinInterval)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint32_t *ret = OT_API_REAL_NAME(otThreadGetTimeInQueueHistogram)(aInstance, aNumBins, aBinInterval);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otDeviceRole OT_API_WRAPPER_NAME(otThreadGetDeviceRole)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otDeviceRole ret = OT_API_REAL_NAME(otThreadGetDeviceRole)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadBecomeChild)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadBecomeChild)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadBecomeDetached)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadBecomeDetached)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadDetachGracefully)(otInstance * aInstance,otDetachGracefullyCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadDetachGracefully)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadDiscover)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aPanId,bool aJoiner,bool aEnableEui64Filtering,otHandleActiveScanResult aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadDiscover)(aInstance, aScanChannels, aPanId, aJoiner, aEnableEui64Filtering, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetLeaderData)(otInstance * aInstance,otLeaderData * aLeaderData)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetLeaderData)(aInstance, aLeaderData);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetLeaderRloc)(otInstance * aInstance,otIp6Address * aLeaderRloc)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetLeaderRloc)(aInstance, aLeaderRloc);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetNextNeighborInfo)(otInstance * aInstance,otNeighborInfoIterator * aIterator,otNeighborInfo * aInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetNextNeighborInfo)(aInstance, aIterator, aInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetParentAverageRssi)(otInstance * aInstance,int8_t * aParentRssi)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetParentAverageRssi)(aInstance, aParentRssi);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetParentInfo)(otInstance * aInstance,otRouterInfo * aParentInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetParentInfo)(aInstance, aParentInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetParentLastRssi)(otInstance * aInstance,int8_t * aLastRssi)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetParentLastRssi)(aInstance, aLastRssi);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetServiceAloc)(otInstance * aInstance,uint8_t aServiceId,otIp6Address * aServiceAloc)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetServiceAloc)(aInstance, aServiceId, aServiceAloc);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadLocateAnycastDestination)(otInstance * aInstance,const otIp6Address * aAnycastAddress,otThreadAnycastLocatorCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadLocateAnycastDestination)(aInstance, aAnycastAddress, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSearchForBetterParent)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSearchForBetterParent)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSendProactiveBackboneNotification)(otInstance * aInstance,otIp6Address * aTarget,otIp6InterfaceIdentifier * aMlIid,uint32_t aTimeSinceLastTransaction)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSendProactiveBackboneNotification)(aInstance, aTarget, aMlIid, aTimeSinceLastTransaction);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetDomainName)(otInstance * aInstance,const char * aDomainName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetDomainName)(aInstance, aDomainName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetExtendedPanId)(otInstance * aInstance,const otExtendedPanId * aExtendedPanId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetExtendedPanId)(aInstance, aExtendedPanId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetFixedDuaInterfaceIdentifier)(otInstance * aInstance,const otIp6InterfaceIdentifier * aIid)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetFixedDuaInterfaceIdentifier)(aInstance, aIid);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetJoinerAdvertisement)(otInstance * aInstance,uint32_t aOui,const uint8_t * aAdvData,uint8_t aAdvDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetJoinerAdvertisement)(aInstance, aOui, aAdvData, aAdvDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetLinkMode)(otInstance * aInstance,otLinkModeConfig aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetLinkMode)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetMeshLocalPrefix)(otInstance * aInstance,const otMeshLocalPrefix * aMeshLocalPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetMeshLocalPrefix)(aInstance, aMeshLocalPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetNetworkKey)(otInstance * aInstance,const otNetworkKey * aKey)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetNetworkKey)(aInstance, aKey);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetNetworkKeyRef)(otInstance * aInstance,otNetworkKeyRef aKeyRef)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetNetworkKeyRef)(aInstance, aKeyRef);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetNetworkName)(otInstance * aInstance,const char * aNetworkName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetNetworkName)(aInstance, aNetworkName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otLinkModeConfig OT_API_WRAPPER_NAME(otThreadGetLinkMode)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otLinkModeConfig ret = OT_API_REAL_NAME(otThreadGetLinkMode)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otNetworkKeyRef OT_API_WRAPPER_NAME(otThreadGetNetworkKeyRef)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otNetworkKeyRef ret = OT_API_REAL_NAME(otThreadGetNetworkKeyRef)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otThreadGetKeySwitchGuardTime)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otThreadGetKeySwitchGuardTime)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otThreadGetRloc16)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otThreadGetRloc16)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otThreadGetVersion)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otThreadGetVersion)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetChildTimeout)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetChildTimeout)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetKeySequenceCounter)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetKeySequenceCounter)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetMaxTimeInQueue)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetMaxTimeInQueue)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otThreadGetPartitionId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otThreadGetPartitionId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetLeaderRouterId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetLeaderRouterId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otThreadGetLeaderWeight)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otThreadGetLeaderWeight)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otConvertDurationInSecondsToString)(uint32_t aDuration,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otConvertDurationInSecondsToString)(aDuration, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadGetNetworkKey)(otInstance * aInstance,otNetworkKey * aNetworkKey)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadGetNetworkKey)(aInstance, aNetworkKey);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadRegisterParentResponseCallback)(otInstance * aInstance,otThreadParentResponseCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadRegisterParentResponseCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadResetIp6Counters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadResetIp6Counters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadResetMleCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadResetMleCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadResetTimeInQueueStat)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadResetTimeInQueueStat)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSendAddressNotification)(otInstance * aInstance,otIp6Address * aDestination,otIp6Address * aTarget,otIp6InterfaceIdentifier * aMlIid)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSendAddressNotification)(aInstance, aDestination, aTarget, aMlIid);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetChildTimeout)(otInstance * aInstance,uint32_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetChildTimeout)(aInstance, aTimeout);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetDiscoveryRequestCallback)(otInstance * aInstance,otThreadDiscoveryRequestCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetDiscoveryRequestCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetKeySequenceCounter)(otInstance * aInstance,uint32_t aKeySequenceCounter)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetKeySequenceCounter)(aInstance, aKeySequenceCounter);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otThreadSetKeySwitchGuardTime)(otInstance * aInstance,uint16_t aKeySwitchGuardTime)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otThreadSetKeySwitchGuardTime)(aInstance, aKeySwitchGuardTime);
    sl_ot_rtos_release_stack_mutex();
}

