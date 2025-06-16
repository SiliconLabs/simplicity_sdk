/*******************************************************************************
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Border Routing APIs
 *   allowing access to the thread stack in a multi-threaded environment.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_ot_rtos_adaptation.h"
#include <openthread/border_routing.h>

#if defined(__GNUC__)
#define REAL_NAME(function) __real_##function
#define WRAPPER_NAME(function) __wrap_##function
#define OT_API_REAL_NAME(function) REAL_NAME(function)
#define OT_API_WRAPPER_NAME(function) WRAPPER_NAME(function)
// #elif defined(__IAR_SYSTEMS_ICC__)
//     #define REAL_NAME(function)             $Super$$##function
//     #define WRAPPER_NAME(function)          $Sub$$##function
//     #define OT_API_REAL_NAME(function)      $Super$$__iar_dl##function
//     #define OT_API_WRAPPER_NAME(function)   $Sub$$__iar_dl##function
#else
#error Unsupported compiler
#endif

extern bool                        OT_API_REAL_NAME(otBorderRoutingIsMultiAilDetected)(otInstance *aInstance);
extern otBorderRoutingDhcp6PdState OT_API_REAL_NAME(otBorderRoutingDhcp6PdGetState)(otInstance *aInstance);
extern otBorderRoutingOmrConfig    OT_API_REAL_NAME(otBorderRoutingGetOmrConfig)(otInstance        *aInstance,
                                                                              otIp6Prefix       *aOmrPrefix,
                                                                              otRoutePreference *aPreference);
extern otBorderRoutingState        OT_API_REAL_NAME(otBorderRoutingGetState)(otInstance *aInstance);
extern otError                     OT_API_REAL_NAME(otBorderRoutingGetFavoredNat64Prefix)(otInstance        *aInstance,
                                                                      otIp6Prefix       *aPrefix,
                                                                      otRoutePreference *aPreference);
extern otError                     OT_API_REAL_NAME(otBorderRoutingGetFavoredOmrPrefix)(otInstance        *aInstance,
                                                                    otIp6Prefix       *aPrefix,
                                                                    otRoutePreference *aPreference);
extern otError OT_API_REAL_NAME(otBorderRoutingGetFavoredOnLinkPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix);
extern otError OT_API_REAL_NAME(otBorderRoutingGetNat64Prefix)(otInstance *aInstance, otIp6Prefix *aPrefix);
extern otError OT_API_REAL_NAME(otBorderRoutingGetNextPeerBrEntry)(otInstance                           *aInstance,
                                                                   otBorderRoutingPrefixTableIterator   *aIterator,
                                                                   otBorderRoutingPeerBorderRouterEntry *aEntry);
extern otError OT_API_REAL_NAME(otBorderRoutingGetNextPrefixTableEntry)(otInstance                         *aInstance,
                                                                        otBorderRoutingPrefixTableIterator *aIterator,
                                                                        otBorderRoutingPrefixTableEntry    *aEntry);
extern otError OT_API_REAL_NAME(otBorderRoutingGetNextRdnssAddrEntry)(otInstance                         *aInstance,
                                                                      otBorderRoutingPrefixTableIterator *aIterator,
                                                                      otBorderRoutingRdnssAddrEntry      *aEntry);
extern otError OT_API_REAL_NAME(otBorderRoutingGetNextRouterEntry)(otInstance                         *aInstance,
                                                                   otBorderRoutingPrefixTableIterator *aIterator,
                                                                   otBorderRoutingRouterEntry         *aEntry);
extern otError OT_API_REAL_NAME(otBorderRoutingGetOmrPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix);
extern otError OT_API_REAL_NAME(otBorderRoutingGetOnLinkPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix);
extern otError OT_API_REAL_NAME(otBorderRoutingGetPdOmrPrefix)(otInstance                      *aInstance,
                                                               otBorderRoutingPrefixTableEntry *aPrefixInfo);
extern otError OT_API_REAL_NAME(otBorderRoutingGetPdProcessedRaInfo)(otInstance          *aInstance,
                                                                     otPdProcessedRaInfo *aPdProcessedRaInfo);
extern otError OT_API_REAL_NAME(otBorderRoutingInit)(otInstance *aInstance,
                                                     uint32_t    aInfraIfIndex,
                                                     bool        aInfraIfIsRunning);
extern otError OT_API_REAL_NAME(otBorderRoutingSetEnabled)(otInstance *aInstance, bool aEnabled);
extern otError OT_API_REAL_NAME(otBorderRoutingSetExtraRouterAdvertOptions)(otInstance    *aInstance,
                                                                            const uint8_t *aOptions,
                                                                            uint16_t       aLength);
extern otError OT_API_REAL_NAME(otBorderRoutingSetOmrConfig)(otInstance              *aInstance,
                                                             otBorderRoutingOmrConfig aConfig,
                                                             const otIp6Prefix       *aOmrPrefix,
                                                             otRoutePreference        aPreference);
extern otRoutePreference OT_API_REAL_NAME(otBorderRoutingGetRouteInfoOptionPreference)(otInstance *aInstance);
extern otRoutePreference OT_API_REAL_NAME(otBorderRoutingGetRoutePreference)(otInstance *aInstance);
extern uint16_t          OT_API_REAL_NAME(otBorderRoutingCountPeerBrs)(otInstance *aInstance, uint32_t *aMinAge);
extern void              OT_API_REAL_NAME(otBorderRoutingClearRouteInfoOptionPreference)(otInstance *aInstance);
extern void              OT_API_REAL_NAME(otBorderRoutingClearRoutePreference)(otInstance *aInstance);
extern void              OT_API_REAL_NAME(otBorderRoutingDhcp6PdSetEnabled)(otInstance *aInstance, bool aEnabled);
extern void              OT_API_REAL_NAME(otBorderRoutingDhcp6PdSetRequestCallback)(otInstance                           *aInstance,
                                                                       otBorderRoutingRequestDhcp6PdCallback aCallback,
                                                                       void                                 *aContext);
extern void              OT_API_REAL_NAME(otBorderRoutingPrefixTableInitIterator)(otInstance                         *aInstance,
                                                                     otBorderRoutingPrefixTableIterator *aIterator);
extern void              OT_API_REAL_NAME(otBorderRoutingSetMultiAilCallback)(otInstance                     *aInstance,
                                                                 otBorderRoutingMultiAilCallback aCallback,
                                                                 void                           *aContext);
extern void OT_API_REAL_NAME(otBorderRoutingSetOnLinkPrefix)(otInstance *aInstance, const otIp6Prefix *aPrefix);
extern void OT_API_REAL_NAME(otBorderRoutingSetRdnssAddrCallback)(otInstance                      *aInstance,
                                                                  otBorderRoutingRdnssAddrCallback aCallback,
                                                                  void                            *aContext);
extern void OT_API_REAL_NAME(otBorderRoutingSetRouteInfoOptionPreference)(otInstance       *aInstance,
                                                                          otRoutePreference aPreference);
extern void OT_API_REAL_NAME(otBorderRoutingSetRoutePreference)(otInstance *aInstance, otRoutePreference aPreference);

bool OT_API_WRAPPER_NAME(otBorderRoutingIsMultiAilDetected)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBorderRoutingIsMultiAilDetected)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otBorderRoutingDhcp6PdState OT_API_WRAPPER_NAME(otBorderRoutingDhcp6PdGetState)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBorderRoutingDhcp6PdState ret = OT_API_REAL_NAME(otBorderRoutingDhcp6PdGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otBorderRoutingOmrConfig OT_API_WRAPPER_NAME(otBorderRoutingGetOmrConfig)(otInstance        *aInstance,
                                                                          otIp6Prefix       *aOmrPrefix,
                                                                          otRoutePreference *aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBorderRoutingOmrConfig ret = OT_API_REAL_NAME(otBorderRoutingGetOmrConfig)(aInstance, aOmrPrefix, aPreference);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otBorderRoutingState OT_API_WRAPPER_NAME(otBorderRoutingGetState)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBorderRoutingState ret = OT_API_REAL_NAME(otBorderRoutingGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetFavoredNat64Prefix)(otInstance        *aInstance,
                                                                  otIp6Prefix       *aPrefix,
                                                                  otRoutePreference *aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetFavoredNat64Prefix)(aInstance, aPrefix, aPreference);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetFavoredOmrPrefix)(otInstance        *aInstance,
                                                                otIp6Prefix       *aPrefix,
                                                                otRoutePreference *aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetFavoredOmrPrefix)(aInstance, aPrefix, aPreference);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetFavoredOnLinkPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetFavoredOnLinkPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetNat64Prefix)(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetNat64Prefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetNextPeerBrEntry)(otInstance                           *aInstance,
                                                               otBorderRoutingPrefixTableIterator   *aIterator,
                                                               otBorderRoutingPeerBorderRouterEntry *aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetNextPeerBrEntry)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetNextPrefixTableEntry)(otInstance                         *aInstance,
                                                                    otBorderRoutingPrefixTableIterator *aIterator,
                                                                    otBorderRoutingPrefixTableEntry    *aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetNextPrefixTableEntry)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetNextRdnssAddrEntry)(otInstance                         *aInstance,
                                                                  otBorderRoutingPrefixTableIterator *aIterator,
                                                                  otBorderRoutingRdnssAddrEntry      *aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetNextRdnssAddrEntry)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetNextRouterEntry)(otInstance                         *aInstance,
                                                               otBorderRoutingPrefixTableIterator *aIterator,
                                                               otBorderRoutingRouterEntry         *aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetNextRouterEntry)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetOmrPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetOmrPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetOnLinkPrefix)(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetOnLinkPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetPdOmrPrefix)(otInstance                      *aInstance,
                                                           otBorderRoutingPrefixTableEntry *aPrefixInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetPdOmrPrefix)(aInstance, aPrefixInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingGetPdProcessedRaInfo)(otInstance          *aInstance,
                                                                 otPdProcessedRaInfo *aPdProcessedRaInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingGetPdProcessedRaInfo)(aInstance, aPdProcessedRaInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingInit)(otInstance *aInstance, uint32_t aInfraIfIndex, bool aInfraIfIsRunning)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingInit)(aInstance, aInfraIfIndex, aInfraIfIsRunning);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingSetEnabled)(otInstance *aInstance, bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingSetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingSetExtraRouterAdvertOptions)(otInstance    *aInstance,
                                                                        const uint8_t *aOptions,
                                                                        uint16_t       aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingSetExtraRouterAdvertOptions)(aInstance, aOptions, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderRoutingSetOmrConfig)(otInstance              *aInstance,
                                                         otBorderRoutingOmrConfig aConfig,
                                                         const otIp6Prefix       *aOmrPrefix,
                                                         otRoutePreference        aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderRoutingSetOmrConfig)(aInstance, aConfig, aOmrPrefix, aPreference);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otRoutePreference OT_API_WRAPPER_NAME(otBorderRoutingGetRouteInfoOptionPreference)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otRoutePreference ret = OT_API_REAL_NAME(otBorderRoutingGetRouteInfoOptionPreference)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otRoutePreference OT_API_WRAPPER_NAME(otBorderRoutingGetRoutePreference)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otRoutePreference ret = OT_API_REAL_NAME(otBorderRoutingGetRoutePreference)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otBorderRoutingCountPeerBrs)(otInstance *aInstance, uint32_t *aMinAge)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otBorderRoutingCountPeerBrs)(aInstance, aMinAge);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otBorderRoutingClearRouteInfoOptionPreference)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingClearRouteInfoOptionPreference)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingClearRoutePreference)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingClearRoutePreference)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingDhcp6PdSetEnabled)(otInstance *aInstance, bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingDhcp6PdSetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingDhcp6PdSetRequestCallback)(otInstance                           *aInstance,
                                                                   otBorderRoutingRequestDhcp6PdCallback aCallback,
                                                                   void                                 *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingDhcp6PdSetRequestCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingPrefixTableInitIterator)(otInstance                         *aInstance,
                                                                 otBorderRoutingPrefixTableIterator *aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingPrefixTableInitIterator)(aInstance, aIterator);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingSetMultiAilCallback)(otInstance                     *aInstance,
                                                             otBorderRoutingMultiAilCallback aCallback,
                                                             void                           *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingSetMultiAilCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingSetOnLinkPrefix)(otInstance *aInstance, const otIp6Prefix *aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingSetOnLinkPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingSetRdnssAddrCallback)(otInstance                      *aInstance,
                                                              otBorderRoutingRdnssAddrCallback aCallback,
                                                              void                            *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingSetRdnssAddrCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingSetRouteInfoOptionPreference)(otInstance       *aInstance,
                                                                      otRoutePreference aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingSetRouteInfoOptionPreference)(aInstance, aPreference);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderRoutingSetRoutePreference)(otInstance *aInstance, otRoutePreference aPreference)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderRoutingSetRoutePreference)(aInstance, aPreference);
    sl_ot_rtos_release_stack_mutex();
}
