/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Link Metrics APIs
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

#include <openthread/link_metrics.h>
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

extern bool OT_API_REAL_NAME(otLinkMetricsManagerIsEnabled)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otLinkMetricsConfigEnhAckProbing)(otInstance * aInstance,const otIp6Address * aDestination,otLinkMetricsEnhAckFlags aEnhAckFlags,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsMgmtResponseCallback aCallback,void * aCallbackContext,otLinkMetricsEnhAckProbingIeReportCallback aEnhAckCallback,void * aEnhAckCallbackContext);
extern otError OT_API_REAL_NAME(otLinkMetricsConfigForwardTrackingSeries)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,otLinkMetricsSeriesFlags aSeriesFlags,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsMgmtResponseCallback aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otLinkMetricsManagerGetMetricsValueByExtAddr)(otInstance * aInstance,const otExtAddress * aExtAddress,otLinkMetricsValues * aLinkMetricsValues);
extern otError OT_API_REAL_NAME(otLinkMetricsQuery)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsReportCallback aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otLinkMetricsSendLinkProbe)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,uint8_t aLength);
extern void OT_API_REAL_NAME(otLinkMetricsManagerSetEnabled)(otInstance * aInstance,bool aEnable);

bool OT_API_WRAPPER_NAME(otLinkMetricsManagerIsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otLinkMetricsManagerIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkMetricsConfigEnhAckProbing)(otInstance * aInstance,const otIp6Address * aDestination,otLinkMetricsEnhAckFlags aEnhAckFlags,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsMgmtResponseCallback aCallback,void * aCallbackContext,otLinkMetricsEnhAckProbingIeReportCallback aEnhAckCallback,void * aEnhAckCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkMetricsConfigEnhAckProbing)(aInstance, aDestination, aEnhAckFlags, aLinkMetricsFlags, aCallback, aCallbackContext, aEnhAckCallback, aEnhAckCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkMetricsConfigForwardTrackingSeries)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,otLinkMetricsSeriesFlags aSeriesFlags,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsMgmtResponseCallback aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkMetricsConfigForwardTrackingSeries)(aInstance, aDestination, aSeriesId, aSeriesFlags, aLinkMetricsFlags, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkMetricsManagerGetMetricsValueByExtAddr)(otInstance * aInstance,const otExtAddress * aExtAddress,otLinkMetricsValues * aLinkMetricsValues)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkMetricsManagerGetMetricsValueByExtAddr)(aInstance, aExtAddress, aLinkMetricsValues);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkMetricsQuery)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,const otLinkMetrics * aLinkMetricsFlags,otLinkMetricsReportCallback aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkMetricsQuery)(aInstance, aDestination, aSeriesId, aLinkMetricsFlags, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otLinkMetricsSendLinkProbe)(otInstance * aInstance,const otIp6Address * aDestination,uint8_t aSeriesId,uint8_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otLinkMetricsSendLinkProbe)(aInstance, aDestination, aSeriesId, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otLinkMetricsManagerSetEnabled)(otInstance * aInstance,bool aEnable)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otLinkMetricsManagerSetEnabled)(aInstance, aEnable);
    sl_ot_rtos_release_stack_mutex();
}

