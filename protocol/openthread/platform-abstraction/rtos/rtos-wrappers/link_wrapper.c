/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Link APIs
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

#include <openthread/link.h>
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

extern bool OT_API_REAL_NAME(otLinkIsActiveScanInProgress)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsCslEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsCslSupported)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsEnergyScanInProgress)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsInTransmitState)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsPromiscuous)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkIsRadioFilterEnabled)(otInstance * aInstance);
extern const otExtAddress *OT_API_REAL_NAME(otLinkGetExtendedAddress)(otInstance * aInstance);
extern const otMacCounters *OT_API_REAL_NAME(otLinkGetCounters)(otInstance * aInstance);
extern const uint32_t *OT_API_REAL_NAME(otLinkGetTxDirectRetrySuccessHistogram)(otInstance * aInstance,uint8_t * aNumberOfEntries);
extern const uint32_t *OT_API_REAL_NAME(otLinkGetTxIndirectRetrySuccessHistogram)(otInstance * aInstance,uint8_t * aNumberOfEntries);
extern int8_t OT_API_REAL_NAME(otLinkConvertLinkQualityToRss)(otInstance * aInstance,uint8_t aLinkQuality);
extern otError OT_API_REAL_NAME(otLinkActiveScan)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aScanDuration,otHandleActiveScanResult aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otLinkEnergyScan)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aScanDuration,otHandleEnergyScanResult aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otLinkFilterAddAddress)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern otError OT_API_REAL_NAME(otLinkFilterAddRssIn)(otInstance * aInstance,const otExtAddress * aExtAddress,int8_t aRss);
extern otError OT_API_REAL_NAME(otLinkFilterGetNextAddress)(otInstance * aInstance,otMacFilterIterator * aIterator,otMacFilterEntry * aEntry);
extern otError OT_API_REAL_NAME(otLinkFilterGetNextRssIn)(otInstance * aInstance,otMacFilterIterator * aIterator,otMacFilterEntry * aEntry);
extern otError OT_API_REAL_NAME(otLinkGetRegion)(otInstance * aInstance,uint16_t * aRegionCode);
extern otError OT_API_REAL_NAME(otLinkSendDataRequest)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkSendEmptyData)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkSetChannel)(otInstance * aInstance,uint8_t aChannel);
extern otError OT_API_REAL_NAME(otLinkSetCslChannel)(otInstance * aInstance,uint8_t aChannel);
extern otError OT_API_REAL_NAME(otLinkSetCslPeriod)(otInstance * aInstance,uint32_t aPeriod);
extern otError OT_API_REAL_NAME(otLinkSetCslTimeout)(otInstance * aInstance,uint32_t aTimeout);
extern otError OT_API_REAL_NAME(otLinkSetEnabled)(otInstance * aInstance,bool aEnable);
extern otError OT_API_REAL_NAME(otLinkSetExtendedAddress)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern otError OT_API_REAL_NAME(otLinkSetPanId)(otInstance * aInstance,otPanId aPanId);
extern otError OT_API_REAL_NAME(otLinkSetPollPeriod)(otInstance * aInstance,uint32_t aPollPeriod);
extern otError OT_API_REAL_NAME(otLinkSetPromiscuous)(otInstance * aInstance,bool aPromiscuous);
extern otError OT_API_REAL_NAME(otLinkSetRegion)(otInstance * aInstance,uint16_t aRegionCode);
extern otError OT_API_REAL_NAME(otLinkSetSupportedChannelMask)(otInstance * aInstance,uint32_t aChannelMask);
extern otMacFilterAddressMode OT_API_REAL_NAME(otLinkFilterGetAddressMode)(otInstance * aInstance);
extern otPanId OT_API_REAL_NAME(otLinkGetPanId)(otInstance * aInstance);
extern otShortAddress OT_API_REAL_NAME(otLinkGetShortAddress)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otLinkGetCcaFailureRate)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otLinkGetCslPeriod)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otLinkGetCslTimeout)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otLinkGetPollPeriod)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otLinkGetSupportedChannelMask)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otLinkConvertRssToLinkQuality)(otInstance * aInstance,int8_t aRss);
extern uint8_t OT_API_REAL_NAME(otLinkGetChannel)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otLinkGetCslChannel)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otLinkGetMaxFrameRetriesDirect)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otLinkGetMaxFrameRetriesIndirect)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkFilterClearAddresses)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkFilterClearAllRssIn)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkFilterClearDefaultRssIn)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkFilterRemoveAddress)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern void OT_API_REAL_NAME(otLinkFilterRemoveRssIn)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern void OT_API_REAL_NAME(otLinkFilterSetAddressMode)(otInstance * aInstance,otMacFilterAddressMode aMode);
extern void OT_API_REAL_NAME(otLinkFilterSetDefaultRssIn)(otInstance * aInstance,int8_t aRss);
extern void OT_API_REAL_NAME(otLinkGetFactoryAssignedIeeeEui64)(otInstance * aInstance,otExtAddress * aEui64);
extern void OT_API_REAL_NAME(otLinkResetCounters)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkResetTxRetrySuccessHistogram)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otLinkSetMaxFrameRetriesDirect)(otInstance * aInstance,uint8_t aMaxFrameRetriesDirect);
extern void OT_API_REAL_NAME(otLinkSetMaxFrameRetriesIndirect)(otInstance * aInstance,uint8_t aMaxFrameRetriesIndirect);
extern void OT_API_REAL_NAME(otLinkSetPcapCallback)(otInstance * aInstance,otLinkPcapCallback aPcapCallback,void * aCallbackContext);
extern void OT_API_REAL_NAME(otLinkSetRadioFilterEnabled)(otInstance * aInstance,bool aFilterEnabled);

bool OT_API_WRAPPER_NAME(otLinkIsActiveScanInProgress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsActiveScanInProgress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsCslEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsCslEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsCslSupported)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsCslSupported)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsEnergyScanInProgress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsEnergyScanInProgress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsInTransmitState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsInTransmitState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsPromiscuous)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsPromiscuous)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkIsRadioFilterEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkIsRadioFilterEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otExtAddress *OT_API_WRAPPER_NAME(otLinkGetExtendedAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otExtAddress *ret = OT_API_REAL_NAME(otLinkGetExtendedAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otMacCounters *OT_API_WRAPPER_NAME(otLinkGetCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otMacCounters *ret = OT_API_REAL_NAME(otLinkGetCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint32_t *OT_API_WRAPPER_NAME(otLinkGetTxDirectRetrySuccessHistogram)(otInstance * aInstance,uint8_t * aNumberOfEntries)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint32_t *ret = OT_API_REAL_NAME(otLinkGetTxDirectRetrySuccessHistogram)(aInstance, aNumberOfEntries);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint32_t *OT_API_WRAPPER_NAME(otLinkGetTxIndirectRetrySuccessHistogram)(otInstance * aInstance,uint8_t * aNumberOfEntries)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint32_t *ret = OT_API_REAL_NAME(otLinkGetTxIndirectRetrySuccessHistogram)(aInstance, aNumberOfEntries);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int8_t OT_API_WRAPPER_NAME(otLinkConvertLinkQualityToRss)(otInstance * aInstance,uint8_t aLinkQuality)
{
    sl_ot_rtos_acquire_stack_mutex();
    int8_t ret = OT_API_REAL_NAME(otLinkConvertLinkQualityToRss)(aInstance, aLinkQuality);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkActiveScan)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aScanDuration,otHandleActiveScanResult aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkActiveScan)(aInstance, aScanChannels, aScanDuration, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkEnergyScan)(otInstance * aInstance,uint32_t aScanChannels,uint16_t aScanDuration,otHandleEnergyScanResult aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkEnergyScan)(aInstance, aScanChannels, aScanDuration, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkFilterAddAddress)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkFilterAddAddress)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkFilterAddRssIn)(otInstance * aInstance,const otExtAddress * aExtAddress,int8_t aRss)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkFilterAddRssIn)(aInstance, aExtAddress, aRss);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkFilterGetNextAddress)(otInstance * aInstance,otMacFilterIterator * aIterator,otMacFilterEntry * aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkFilterGetNextAddress)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkFilterGetNextRssIn)(otInstance * aInstance,otMacFilterIterator * aIterator,otMacFilterEntry * aEntry)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkFilterGetNextRssIn)(aInstance, aIterator, aEntry);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkGetRegion)(otInstance * aInstance,uint16_t * aRegionCode)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkGetRegion)(aInstance, aRegionCode);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSendDataRequest)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSendDataRequest)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSendEmptyData)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSendEmptyData)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetChannel)(otInstance * aInstance,uint8_t aChannel)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetChannel)(aInstance, aChannel);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetCslChannel)(otInstance * aInstance,uint8_t aChannel)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetCslChannel)(aInstance, aChannel);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetCslPeriod)(otInstance * aInstance,uint32_t aPeriod)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetCslPeriod)(aInstance, aPeriod);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetCslTimeout)(otInstance * aInstance,uint32_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetCslTimeout)(aInstance, aTimeout);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetEnabled)(otInstance * aInstance,bool aEnable)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetEnabled)(aInstance, aEnable);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetExtendedAddress)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetExtendedAddress)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetPanId)(otInstance * aInstance,otPanId aPanId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetPanId)(aInstance, aPanId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetPollPeriod)(otInstance * aInstance,uint32_t aPollPeriod)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetPollPeriod)(aInstance, aPollPeriod);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetPromiscuous)(otInstance * aInstance,bool aPromiscuous)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetPromiscuous)(aInstance, aPromiscuous);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetRegion)(otInstance * aInstance,uint16_t aRegionCode)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetRegion)(aInstance, aRegionCode);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkSetSupportedChannelMask)(otInstance * aInstance,uint32_t aChannelMask)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkSetSupportedChannelMask)(aInstance, aChannelMask);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMacFilterAddressMode OT_API_WRAPPER_NAME(otLinkFilterGetAddressMode)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMacFilterAddressMode ret = OT_API_REAL_NAME(otLinkFilterGetAddressMode)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otPanId OT_API_WRAPPER_NAME(otLinkGetPanId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otPanId ret = OT_API_REAL_NAME(otLinkGetPanId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otShortAddress OT_API_WRAPPER_NAME(otLinkGetShortAddress)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otShortAddress ret = OT_API_REAL_NAME(otLinkGetShortAddress)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otLinkGetCcaFailureRate)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otLinkGetCcaFailureRate)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otLinkGetCslPeriod)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otLinkGetCslPeriod)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otLinkGetCslTimeout)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otLinkGetCslTimeout)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otLinkGetPollPeriod)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otLinkGetPollPeriod)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otLinkGetSupportedChannelMask)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otLinkGetSupportedChannelMask)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otLinkConvertRssToLinkQuality)(otInstance * aInstance,int8_t aRss)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otLinkConvertRssToLinkQuality)(aInstance, aRss);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otLinkGetChannel)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otLinkGetChannel)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otLinkGetCslChannel)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otLinkGetCslChannel)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otLinkGetMaxFrameRetriesDirect)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otLinkGetMaxFrameRetriesDirect)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otLinkGetMaxFrameRetriesIndirect)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otLinkGetMaxFrameRetriesIndirect)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otLinkFilterClearAddresses)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterClearAddresses)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterClearAllRssIn)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterClearAllRssIn)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterClearDefaultRssIn)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterClearDefaultRssIn)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterRemoveAddress)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterRemoveAddress)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterRemoveRssIn)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterRemoveRssIn)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterSetAddressMode)(otInstance * aInstance,otMacFilterAddressMode aMode)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterSetAddressMode)(aInstance, aMode);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkFilterSetDefaultRssIn)(otInstance * aInstance,int8_t aRss)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkFilterSetDefaultRssIn)(aInstance, aRss);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkGetFactoryAssignedIeeeEui64)(otInstance * aInstance,otExtAddress * aEui64)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkGetFactoryAssignedIeeeEui64)(aInstance, aEui64);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkResetCounters)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkResetCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkResetTxRetrySuccessHistogram)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkResetTxRetrySuccessHistogram)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkSetMaxFrameRetriesDirect)(otInstance * aInstance,uint8_t aMaxFrameRetriesDirect)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkSetMaxFrameRetriesDirect)(aInstance, aMaxFrameRetriesDirect);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkSetMaxFrameRetriesIndirect)(otInstance * aInstance,uint8_t aMaxFrameRetriesIndirect)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkSetMaxFrameRetriesIndirect)(aInstance, aMaxFrameRetriesIndirect);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkSetPcapCallback)(otInstance * aInstance,otLinkPcapCallback aPcapCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkSetPcapCallback)(aInstance, aPcapCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otLinkSetRadioFilterEnabled)(otInstance * aInstance,bool aFilterEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkSetRadioFilterEnabled)(aInstance, aFilterEnabled);
    sl_ot_rtos_release_stack_mutex();
}

