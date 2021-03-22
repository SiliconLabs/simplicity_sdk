/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Netdiag APIs
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

#include <openthread/netdiag.h>
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

extern const char *OT_API_REAL_NAME(otThreadGetVendorAppUrl)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otThreadGetVendorModel)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otThreadGetVendorName)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otThreadGetVendorSwVersion)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otThreadGetNextDiagnosticTlv)(const otMessage * aMessage,otNetworkDiagIterator * aIterator,otNetworkDiagTlv * aNetworkDiagTlv);
extern otError OT_API_REAL_NAME(otThreadSendDiagnosticGet)(otInstance * aInstance,const otIp6Address * aDestination,const uint8_t aTlvTypes[],uint8_t aCount,otReceiveDiagnosticGetCallback aCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otThreadSendDiagnosticReset)(otInstance * aInstance,const otIp6Address * aDestination,const uint8_t aTlvTypes[],uint8_t aCount);
extern otError OT_API_REAL_NAME(otThreadSetVendorAppUrl)(otInstance * aInstance,const char * aVendorAppUrl);
extern otError OT_API_REAL_NAME(otThreadSetVendorModel)(otInstance * aInstance,const char * aVendorModel);
extern otError OT_API_REAL_NAME(otThreadSetVendorName)(otInstance * aInstance,const char * aVendorName);
extern otError OT_API_REAL_NAME(otThreadSetVendorSwVersion)(otInstance * aInstance,const char * aVendorSwVersion);

const char *OT_API_WRAPPER_NAME(otThreadGetVendorAppUrl)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetVendorAppUrl)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadGetVendorModel)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetVendorModel)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadGetVendorName)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetVendorName)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otThreadGetVendorSwVersion)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otThreadGetVendorSwVersion)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadGetNextDiagnosticTlv)(const otMessage * aMessage,otNetworkDiagIterator * aIterator,otNetworkDiagTlv * aNetworkDiagTlv)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadGetNextDiagnosticTlv)(aMessage, aIterator, aNetworkDiagTlv);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSendDiagnosticGet)(otInstance * aInstance,const otIp6Address * aDestination,const uint8_t aTlvTypes[],uint8_t aCount,otReceiveDiagnosticGetCallback aCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSendDiagnosticGet)(aInstance, aDestination, aTlvTypes, aCount, aCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSendDiagnosticReset)(otInstance * aInstance,const otIp6Address * aDestination,const uint8_t aTlvTypes[],uint8_t aCount)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSendDiagnosticReset)(aInstance, aDestination, aTlvTypes, aCount);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetVendorAppUrl)(otInstance * aInstance,const char * aVendorAppUrl)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetVendorAppUrl)(aInstance, aVendorAppUrl);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetVendorModel)(otInstance * aInstance,const char * aVendorModel)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetVendorModel)(aInstance, aVendorModel);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetVendorName)(otInstance * aInstance,const char * aVendorName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetVendorName)(aInstance, aVendorName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otThreadSetVendorSwVersion)(otInstance * aInstance,const char * aVendorSwVersion)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otThreadSetVendorSwVersion)(aInstance, aVendorSwVersion);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

