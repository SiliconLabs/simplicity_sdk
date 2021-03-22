/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Server APIs
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

#include <openthread/server.h>
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

extern otError OT_API_REAL_NAME(otServerAddService)(otInstance * aInstance,const otServiceConfig * aConfig);
extern otError OT_API_REAL_NAME(otServerGetNetDataLocal)(otInstance * aInstance,bool aStable,uint8_t * aData,uint8_t * aDataLength);
extern otError OT_API_REAL_NAME(otServerGetNextService)(otInstance * aInstance,otNetworkDataIterator * aIterator,otServiceConfig * aConfig);
extern otError OT_API_REAL_NAME(otServerRegister)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otServerRemoveService)(otInstance * aInstance,uint32_t aEnterpriseNumber,const uint8_t * aServiceData,uint8_t aServiceDataLength);

otError OT_API_WRAPPER_NAME(otServerAddService)(otInstance * aInstance,const otServiceConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otServerAddService)(aInstance, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otServerGetNetDataLocal)(otInstance * aInstance,bool aStable,uint8_t * aData,uint8_t * aDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otServerGetNetDataLocal)(aInstance, aStable, aData, aDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otServerGetNextService)(otInstance * aInstance,otNetworkDataIterator * aIterator,otServiceConfig * aConfig)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otServerGetNextService)(aInstance, aIterator, aConfig);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otServerRegister)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otServerRegister)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otServerRemoveService)(otInstance * aInstance,uint32_t aEnterpriseNumber,const uint8_t * aServiceData,uint8_t aServiceDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otServerRemoveService)(aInstance, aEnterpriseNumber, aServiceData, aServiceDataLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

