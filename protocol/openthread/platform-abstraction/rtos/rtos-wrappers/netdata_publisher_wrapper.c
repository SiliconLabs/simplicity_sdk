/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Netdata Publisher APIs
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

#include <openthread/netdata_publisher.h>
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

extern bool OT_API_REAL_NAME(otNetDataIsDnsSrpServiceAdded)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otNetDataIsPrefixAdded)(otInstance * aInstance,const otIp6Prefix * aPrefix);
extern otError OT_API_REAL_NAME(otNetDataPublishExternalRoute)(otInstance * aInstance,const otExternalRouteConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataPublishOnMeshPrefix)(otInstance * aInstance,const otBorderRouterConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataReplacePublishedExternalRoute)(otInstance * aInstance,const otIp6Prefix * aPrefix,const otExternalRouteConfig * aConfig);
extern otError OT_API_REAL_NAME(otNetDataUnpublishPrefix)(otInstance * aInstance,const otIp6Prefix * aPrefix);
extern void OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceAnycast)(otInstance * aInstance,uint8_t aSequenceNUmber);
extern void OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceUnicast)(otInstance * aInstance,const otIp6Address * aAddress,uint16_t aPort);
extern void OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceUnicastMeshLocalEid)(otInstance * aInstance,uint16_t aPort);
extern void OT_API_REAL_NAME(otNetDataSetDnsSrpServicePublisherCallback)(otInstance * aInstance,otNetDataDnsSrpServicePublisherCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otNetDataSetPrefixPublisherCallback)(otInstance * aInstance,otNetDataPrefixPublisherCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otNetDataUnpublishDnsSrpService)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otNetDataIsDnsSrpServiceAdded)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otNetDataIsDnsSrpServiceAdded)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otNetDataIsPrefixAdded)(otInstance * aInstance,const otIp6Prefix * aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otNetDataIsPrefixAdded)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataPublishExternalRoute)(otInstance * aInstance,const otExternalRouteConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataPublishExternalRoute)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataPublishOnMeshPrefix)(otInstance * aInstance,const otBorderRouterConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataPublishOnMeshPrefix)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataReplacePublishedExternalRoute)(otInstance * aInstance,const otIp6Prefix * aPrefix,const otExternalRouteConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataReplacePublishedExternalRoute)(aInstance, aPrefix, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otNetDataUnpublishPrefix)(otInstance * aInstance,const otIp6Prefix * aPrefix)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNetDataUnpublishPrefix)(aInstance, aPrefix);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otNetDataPublishDnsSrpServiceAnycast)(otInstance * aInstance,uint8_t aSequenceNUmber)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceAnycast)(aInstance, aSequenceNUmber);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataPublishDnsSrpServiceUnicast)(otInstance * aInstance,const otIp6Address * aAddress,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceUnicast)(aInstance, aAddress, aPort);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataPublishDnsSrpServiceUnicastMeshLocalEid)(otInstance * aInstance,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataPublishDnsSrpServiceUnicastMeshLocalEid)(aInstance, aPort);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataSetDnsSrpServicePublisherCallback)(otInstance * aInstance,otNetDataDnsSrpServicePublisherCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataSetDnsSrpServicePublisherCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataSetPrefixPublisherCallback)(otInstance * aInstance,otNetDataPrefixPublisherCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataSetPrefixPublisherCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNetDataUnpublishDnsSrpService)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNetDataUnpublishDnsSrpService)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

