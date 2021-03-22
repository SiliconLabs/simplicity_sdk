/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Netdata APIs
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

#include <openthread/netdata.h>
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

extern bool OT_API_REAL_NAME(otNetDataContainsOmrPrefix)(otInstance * aInstance,const otIp6Prefix * aPrefix);
extern otError OT_API_REAL_NAME(otNetDataGet)(otInstance * aInstance,bool aStable,uint8_t * aData,uint8_t * aDataLength);
extern otError OT_API_REAL_NAME(otNetDataGetNextLowpanContextInfo)(otInstance * aInstance,otNetworkDataIterator * aIterator,otLowpanContextInfo * aContextInfo);
extern otError OT_API_REAL_NAME(otNetDataGetNextOnMeshPrefix)(otInstance * aInstance,otNetworkDataIterator * aIterator,otBorderRouterConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataGetNextRoute)(otInstance * aInstance,otNetworkDataIterator * aIterator,otExternalRouteConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataGetNextService)(otInstance * aInstance,otNetworkDataIterator * aIterator,otServiceConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataSteeringDataCheckJoiner)(otInstance * aInstance,const otExtAddress * aEui64);
extern otError OT_API_REAL_NAME(otNetDataSteeringDataCheckJoinerWithDiscerner)(otInstance * aInstance,const struct otJoinerDiscerner * aDiscerner);
extern uint8_t OT_API_REAL_NAME(otNetDataGetLength)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otNetDataGetMaxLength)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otNetDataGetStableVersion)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otNetDataGetVersion)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otNetDataGetCommissioningDataset)(otInstance * aInstance,otCommissioningDataset * aDataset);
extern void OT_API_REAL_NAME(otNetDataResetMaxLength)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otNetDataContainsOmrPrefix)(otInstance * aInstance,const otIp6Prefix * aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otNetDataContainsOmrPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataGet)(otInstance * aInstance,bool aStable,uint8_t * aData,uint8_t * aDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataGet)(aInstance, aStable, aData, aDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataGetNextLowpanContextInfo)(otInstance * aInstance,otNetworkDataIterator * aIterator,otLowpanContextInfo * aContextInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataGetNextLowpanContextInfo)(aInstance, aIterator, aContextInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataGetNextOnMeshPrefix)(otInstance * aInstance,otNetworkDataIterator * aIterator,otBorderRouterConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataGetNextOnMeshPrefix)(aInstance, aIterator, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataGetNextRoute)(otInstance * aInstance,otNetworkDataIterator * aIterator,otExternalRouteConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataGetNextRoute)(aInstance, aIterator, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataGetNextService)(otInstance * aInstance,otNetworkDataIterator * aIterator,otServiceConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataGetNextService)(aInstance, aIterator, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataSteeringDataCheckJoiner)(otInstance * aInstance,const otExtAddress * aEui64)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataSteeringDataCheckJoiner)(aInstance, aEui64);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataSteeringDataCheckJoinerWithDiscerner)(otInstance * aInstance,const struct otJoinerDiscerner * aDiscerner)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataSteeringDataCheckJoinerWithDiscerner)(aInstance, aDiscerner);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otNetDataGetLength)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otNetDataGetLength)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otNetDataGetMaxLength)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otNetDataGetMaxLength)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otNetDataGetStableVersion)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otNetDataGetStableVersion)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otNetDataGetVersion)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otNetDataGetVersion)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otNetDataGetCommissioningDataset)(otInstance * aInstance,otCommissioningDataset * aDataset)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataGetCommissioningDataset)(aInstance, aDataset);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataResetMaxLength)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataResetMaxLength)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

