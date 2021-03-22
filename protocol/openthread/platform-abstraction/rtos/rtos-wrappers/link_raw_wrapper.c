/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Link Raw APIs
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

#include <openthread/link_raw.h>
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

extern bool OT_API_REAL_NAME(otLinkRawGetPromiscuous)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otLinkRawIsEnabled)(otInstance * aInstance);
extern int8_t OT_API_REAL_NAME(otLinkRawGetRssi)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkRawEnergyScan)(otInstance * aInstance,uint8_t aScanChannel,uint16_t aScanDuration,otLinkRawEnergyScanDone aCallback);
extern otError OT_API_REAL_NAME(otLinkRawReceive)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkRawSetMacFrameCounter)(otInstance * aInstance,uint32_t aMacFrameCounter);
extern otError OT_API_REAL_NAME(otLinkRawSetMacFrameCounterIfLarger)(otInstance * aInstance,uint32_t aMacFrameCounter);
extern otError OT_API_REAL_NAME(otLinkRawSetMacKey)(otInstance * aInstance,uint8_t aKeyIdMode,uint8_t aKeyId,const otMacKey * aPrevKey,const otMacKey * aCurrKey,const otMacKey * aNextKey);
extern otError OT_API_REAL_NAME(otLinkRawSetPromiscuous)(otInstance * aInstance,bool aEnable);
extern otError OT_API_REAL_NAME(otLinkRawSetReceiveDone)(otInstance * aInstance,otLinkRawReceiveDone aCallback);
extern otError OT_API_REAL_NAME(otLinkRawSetShortAddress)(otInstance * aInstance,uint16_t aShortAddress);
extern otError OT_API_REAL_NAME(otLinkRawSleep)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchAddExtEntry)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchAddShortEntry)(otInstance * aInstance,uint16_t aShortAddress);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchClearExtEntries)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchClearExtEntry)(otInstance * aInstance,const otExtAddress * aExtAddress);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchClearShortEntries)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchClearShortEntry)(otInstance * aInstance,uint16_t aShortAddress);
extern otError OT_API_REAL_NAME(otLinkRawSrcMatchEnable)(otInstance * aInstance,bool aEnable);
extern otError OT_API_REAL_NAME(otLinkRawTransmit)(otInstance * aInstance,otLinkRawTransmitDone aCallback);
extern otRadioCaps OT_API_REAL_NAME(otLinkRawGetCaps)(otInstance * aInstance);
extern otRadioFrame *OT_API_REAL_NAME(otLinkRawGetTransmitBuffer)(otInstance * aInstance);
extern uint64_t OT_API_REAL_NAME(otLinkRawGetRadioTime)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otLinkRawGetPromiscuous)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkRawGetPromiscuous)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otLinkRawIsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkRawIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int8_t OT_API_WRAPPER_NAME(otLinkRawGetRssi)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    int8_t ret = OT_API_REAL_NAME(otLinkRawGetRssi)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawEnergyScan)(otInstance * aInstance,uint8_t aScanChannel,uint16_t aScanDuration,otLinkRawEnergyScanDone aCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawEnergyScan)(aInstance, aScanChannel, aScanDuration, aCallback);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawReceive)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawReceive)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetMacFrameCounter)(otInstance * aInstance,uint32_t aMacFrameCounter)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetMacFrameCounter)(aInstance, aMacFrameCounter);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetMacFrameCounterIfLarger)(otInstance * aInstance,uint32_t aMacFrameCounter)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetMacFrameCounterIfLarger)(aInstance, aMacFrameCounter);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetMacKey)(otInstance * aInstance,uint8_t aKeyIdMode,uint8_t aKeyId,const otMacKey * aPrevKey,const otMacKey * aCurrKey,const otMacKey * aNextKey)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetMacKey)(aInstance, aKeyIdMode, aKeyId, aPrevKey, aCurrKey, aNextKey);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetPromiscuous)(otInstance * aInstance,bool aEnable)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetPromiscuous)(aInstance, aEnable);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetReceiveDone)(otInstance * aInstance,otLinkRawReceiveDone aCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetReceiveDone)(aInstance, aCallback);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSetShortAddress)(otInstance * aInstance,uint16_t aShortAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSetShortAddress)(aInstance, aShortAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSleep)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSleep)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchAddExtEntry)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchAddExtEntry)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchAddShortEntry)(otInstance * aInstance,uint16_t aShortAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchAddShortEntry)(aInstance, aShortAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchClearExtEntries)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchClearExtEntries)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchClearExtEntry)(otInstance * aInstance,const otExtAddress * aExtAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchClearExtEntry)(aInstance, aExtAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchClearShortEntries)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchClearShortEntries)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchClearShortEntry)(otInstance * aInstance,uint16_t aShortAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchClearShortEntry)(aInstance, aShortAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawSrcMatchEnable)(otInstance * aInstance,bool aEnable)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawSrcMatchEnable)(aInstance, aEnable);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkRawTransmit)(otInstance * aInstance,otLinkRawTransmitDone aCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkRawTransmit)(aInstance, aCallback);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otRadioCaps OT_API_WRAPPER_NAME(otLinkRawGetCaps)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otRadioCaps ret = OT_API_REAL_NAME(otLinkRawGetCaps)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otRadioFrame *OT_API_WRAPPER_NAME(otLinkRawGetTransmitBuffer)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otRadioFrame *ret = OT_API_REAL_NAME(otLinkRawGetTransmitBuffer)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint64_t OT_API_WRAPPER_NAME(otLinkRawGetRadioTime)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint64_t ret = OT_API_REAL_NAME(otLinkRawGetRadioTime)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

