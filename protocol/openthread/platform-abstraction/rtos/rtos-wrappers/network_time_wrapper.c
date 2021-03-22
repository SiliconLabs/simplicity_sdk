/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Network Time APIs
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

#include <openthread/network_time.h>
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

extern otError OT_API_REAL_NAME(otNetworkTimeSetSyncPeriod)(otInstance * aInstance,uint16_t aTimeSyncPeriod);
extern otError OT_API_REAL_NAME(otNetworkTimeSetXtalThreshold)(otInstance * aInstance,uint16_t aXTALThreshold);
extern otNetworkTimeStatus OT_API_REAL_NAME(otNetworkTimeGet)(otInstance * aInstance,uint64_t * aNetworkTime);
extern uint16_t OT_API_REAL_NAME(otNetworkTimeGetSyncPeriod)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otNetworkTimeGetXtalThreshold)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otNetworkTimeSyncSetCallback)(otInstance * aInstance,otNetworkTimeSyncCallbackFn aCallbackFn,void * aCallbackContext);

otError OT_API_WRAPPER_NAME(otNetworkTimeSetSyncPeriod)(otInstance * aInstance,uint16_t aTimeSyncPeriod)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetworkTimeSetSyncPeriod)(aInstance, aTimeSyncPeriod);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetworkTimeSetXtalThreshold)(otInstance * aInstance,uint16_t aXTALThreshold)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetworkTimeSetXtalThreshold)(aInstance, aXTALThreshold);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otNetworkTimeStatus OT_API_WRAPPER_NAME(otNetworkTimeGet)(otInstance * aInstance,uint64_t * aNetworkTime)
{
    sl_ot_rtos_acquire_stack_mutex();
    otNetworkTimeStatus ret = OT_API_REAL_NAME(otNetworkTimeGet)(aInstance, aNetworkTime);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otNetworkTimeGetSyncPeriod)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otNetworkTimeGetSyncPeriod)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otNetworkTimeGetXtalThreshold)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otNetworkTimeGetXtalThreshold)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otNetworkTimeSyncSetCallback)(otInstance * aInstance,otNetworkTimeSyncCallbackFn aCallbackFn,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetworkTimeSyncSetCallback)(aInstance, aCallbackFn, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
}

