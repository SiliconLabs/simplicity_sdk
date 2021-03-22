/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Channel Manager APIs
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

#include <openthread/channel_manager.h>
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

extern bool OT_API_REAL_NAME(otChannelManagerGetAutoChannelSelectionEnabled)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otChannelManagerGetAutoCslChannelSelectionEnabled)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otChannelManagerRequestChannelSelect)(otInstance * aInstance,bool aSkipQualityCheck);
extern otError OT_API_REAL_NAME(otChannelManagerRequestCslChannelSelect)(otInstance * aInstance,bool aSkipQualityCheck);
extern otError OT_API_REAL_NAME(otChannelManagerSetAutoChannelSelectionInterval)(otInstance * aInstance,uint32_t aInterval);
extern otError OT_API_REAL_NAME(otChannelManagerSetDelay)(otInstance * aInstance,uint16_t aDelay);
extern uint16_t OT_API_REAL_NAME(otChannelManagerGetCcaFailureRateThreshold)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otChannelManagerGetDelay)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otChannelManagerGetAutoChannelSelectionInterval)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otChannelManagerGetFavoredChannels)(otInstance * aInstance);
extern uint32_t OT_API_REAL_NAME(otChannelManagerGetSupportedChannels)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otChannelManagerGetRequestedChannel)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otChannelManagerRequestChannelChange)(otInstance * aInstance,uint8_t aChannel);
extern void OT_API_REAL_NAME(otChannelManagerSetAutoChannelSelectionEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otChannelManagerSetAutoCslChannelSelectionEnabled)(otInstance * aInstance,bool aEnabled);
extern void OT_API_REAL_NAME(otChannelManagerSetCcaFailureRateThreshold)(otInstance * aInstance,uint16_t aThreshold);
extern void OT_API_REAL_NAME(otChannelManagerSetFavoredChannels)(otInstance * aInstance,uint32_t aChannelMask);
extern void OT_API_REAL_NAME(otChannelManagerSetSupportedChannels)(otInstance * aInstance,uint32_t aChannelMask);

bool OT_API_WRAPPER_NAME(otChannelManagerGetAutoChannelSelectionEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otChannelManagerGetAutoChannelSelectionEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otChannelManagerGetAutoCslChannelSelectionEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otChannelManagerGetAutoCslChannelSelectionEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otChannelManagerRequestChannelSelect)(otInstance * aInstance,bool aSkipQualityCheck)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otChannelManagerRequestChannelSelect)(aInstance, aSkipQualityCheck);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otChannelManagerRequestCslChannelSelect)(otInstance * aInstance,bool aSkipQualityCheck)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otChannelManagerRequestCslChannelSelect)(aInstance, aSkipQualityCheck);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otChannelManagerSetAutoChannelSelectionInterval)(otInstance * aInstance,uint32_t aInterval)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otChannelManagerSetAutoChannelSelectionInterval)(aInstance, aInterval);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otChannelManagerSetDelay)(otInstance * aInstance,uint16_t aDelay)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otChannelManagerSetDelay)(aInstance, aDelay);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otChannelManagerGetCcaFailureRateThreshold)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otChannelManagerGetCcaFailureRateThreshold)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otChannelManagerGetDelay)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otChannelManagerGetDelay)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otChannelManagerGetAutoChannelSelectionInterval)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otChannelManagerGetAutoChannelSelectionInterval)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otChannelManagerGetFavoredChannels)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otChannelManagerGetFavoredChannels)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otChannelManagerGetSupportedChannels)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otChannelManagerGetSupportedChannels)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otChannelManagerGetRequestedChannel)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otChannelManagerGetRequestedChannel)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otChannelManagerRequestChannelChange)(otInstance * aInstance,uint8_t aChannel)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerRequestChannelChange)(aInstance, aChannel);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otChannelManagerSetAutoChannelSelectionEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerSetAutoChannelSelectionEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otChannelManagerSetAutoCslChannelSelectionEnabled)(otInstance * aInstance,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerSetAutoCslChannelSelectionEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otChannelManagerSetCcaFailureRateThreshold)(otInstance * aInstance,uint16_t aThreshold)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerSetCcaFailureRateThreshold)(aInstance, aThreshold);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otChannelManagerSetFavoredChannels)(otInstance * aInstance,uint32_t aChannelMask)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerSetFavoredChannels)(aInstance, aChannelMask);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otChannelManagerSetSupportedChannels)(otInstance * aInstance,uint32_t aChannelMask)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otChannelManagerSetSupportedChannels)(aInstance, aChannelMask);
    sl_ot_rtos_release_stack_mutex();
}

