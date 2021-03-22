/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Instance APIs
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

#include <openthread/instance.h>
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

extern bool OT_API_REAL_NAME(otInstanceIsInitialized)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otGetRadioVersionString)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otGetVersionString)(void);
extern otError OT_API_REAL_NAME(otInstanceErasePersistentInfo)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otInstanceResetToBootloader)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otSetStateChangedCallback)(otInstance * aInstance,otStateChangedCallback aCallback,void * aContext);
extern otInstance *OT_API_REAL_NAME(otInstanceInit)(void * aInstanceBuffer,size_t * aInstanceBufferSize);
extern otInstance *OT_API_REAL_NAME(otInstanceInitMultiple)(uint8_t aIdx);
extern otInstance *OT_API_REAL_NAME(otInstanceInitSingle)(void);
extern uint32_t OT_API_REAL_NAME(otInstanceGetId)(otInstance * aInstance);
extern uint64_t OT_API_REAL_NAME(otInstanceGetUptime)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otInstanceFactoryReset)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otInstanceFinalize)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otInstanceGetUptimeAsString)(otInstance * aInstance,char * aBuffer,uint16_t aSize);
extern void OT_API_REAL_NAME(otInstanceReset)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otInstanceResetRadioStack)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otRemoveStateChangeCallback)(otInstance * aInstance,otStateChangedCallback aCallback,void * aContext);

bool OT_API_WRAPPER_NAME(otInstanceIsInitialized)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otInstanceIsInitialized)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otGetRadioVersionString)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otGetRadioVersionString)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otGetVersionString)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otGetVersionString)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otInstanceErasePersistentInfo)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otInstanceErasePersistentInfo)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otInstanceResetToBootloader)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otInstanceResetToBootloader)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otSetStateChangedCallback)(otInstance * aInstance,otStateChangedCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otSetStateChangedCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otInstance *OT_API_WRAPPER_NAME(otInstanceInit)(void * aInstanceBuffer,size_t * aInstanceBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otInstance *ret = OT_API_REAL_NAME(otInstanceInit)(aInstanceBuffer, aInstanceBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otInstance *OT_API_WRAPPER_NAME(otInstanceInitMultiple)(uint8_t aIdx)
{
    sl_ot_rtos_acquire_stack_mutex();
    otInstance *ret = OT_API_REAL_NAME(otInstanceInitMultiple)(aIdx);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otInstance *OT_API_WRAPPER_NAME(otInstanceInitSingle)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    otInstance *ret = OT_API_REAL_NAME(otInstanceInitSingle)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otInstanceGetId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otInstanceGetId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint64_t OT_API_WRAPPER_NAME(otInstanceGetUptime)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint64_t ret = OT_API_REAL_NAME(otInstanceGetUptime)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otInstanceFactoryReset)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otInstanceFactoryReset)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otInstanceFinalize)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otInstanceFinalize)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otInstanceGetUptimeAsString)(otInstance * aInstance,char * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otInstanceGetUptimeAsString)(aInstance, aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otInstanceReset)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otInstanceReset)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otInstanceResetRadioStack)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otInstanceResetRadioStack)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otRemoveStateChangeCallback)(otInstance * aInstance,otStateChangedCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otRemoveStateChangeCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

