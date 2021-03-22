/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Joiner APIs
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

#include <openthread/joiner.h>
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

extern const char *OT_API_REAL_NAME(otJoinerStateToString)(otJoinerState aState);
extern const otExtAddress *OT_API_REAL_NAME(otJoinerGetId)(otInstance * aInstance);
extern const otJoinerDiscerner *OT_API_REAL_NAME(otJoinerGetDiscerner)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otJoinerSetDiscerner)(otInstance * aInstance,otJoinerDiscerner * aDiscerner);
extern otError OT_API_REAL_NAME(otJoinerStart)(otInstance * aInstance,const char * aPskd,const char * aProvisioningUrl,const char * aVendorName,const char * aVendorModel,const char * aVendorSwVersion,const char * aVendorData,otJoinerCallback aCallback,void * aContext);
extern otJoinerState OT_API_REAL_NAME(otJoinerGetState)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otJoinerStop)(otInstance * aInstance);

const char *OT_API_WRAPPER_NAME(otJoinerStateToString)(otJoinerState aState)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otJoinerStateToString)(aState);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otExtAddress *OT_API_WRAPPER_NAME(otJoinerGetId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otExtAddress *ret = OT_API_REAL_NAME(otJoinerGetId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otJoinerDiscerner *OT_API_WRAPPER_NAME(otJoinerGetDiscerner)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otJoinerDiscerner *ret = OT_API_REAL_NAME(otJoinerGetDiscerner)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJoinerSetDiscerner)(otInstance * aInstance,otJoinerDiscerner * aDiscerner)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJoinerSetDiscerner)(aInstance, aDiscerner);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJoinerStart)(otInstance * aInstance,const char * aPskd,const char * aProvisioningUrl,const char * aVendorName,const char * aVendorModel,const char * aVendorSwVersion,const char * aVendorData,otJoinerCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJoinerStart)(aInstance, aPskd, aProvisioningUrl, aVendorName, aVendorModel, aVendorSwVersion, aVendorData, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otJoinerState OT_API_WRAPPER_NAME(otJoinerGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otJoinerState ret = OT_API_REAL_NAME(otJoinerGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otJoinerStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otJoinerStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

