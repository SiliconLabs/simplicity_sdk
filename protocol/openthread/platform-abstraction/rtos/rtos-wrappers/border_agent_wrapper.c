/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Border Agent APIs
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

#include <openthread/border_agent.h>
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

extern bool OT_API_REAL_NAME(otBorderAgentIsEphemeralKeyActive)(otInstance * aInstance);
extern otBorderAgentState OT_API_REAL_NAME(otBorderAgentGetState)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBorderAgentGetId)(otInstance * aInstance,otBorderAgentId * aId);
extern otError OT_API_REAL_NAME(otBorderAgentSetEphemeralKey)(otInstance * aInstance,const char * aKeyString,uint32_t aTimeout,uint16_t aUdpPort);
extern otError OT_API_REAL_NAME(otBorderAgentSetId)(otInstance * aInstance,const otBorderAgentId * aId);
extern uint16_t OT_API_REAL_NAME(otBorderAgentGetUdpPort)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otBorderAgentClearEphemeralKey)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otBorderAgentSetEphemeralKeyCallback)(otInstance * aInstance,otBorderAgentEphemeralKeyCallback aCallback,void * aContext);

bool OT_API_WRAPPER_NAME(otBorderAgentIsEphemeralKeyActive)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBorderAgentIsEphemeralKeyActive)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otBorderAgentState OT_API_WRAPPER_NAME(otBorderAgentGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBorderAgentState ret = OT_API_REAL_NAME(otBorderAgentGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentGetId)(otInstance * aInstance,otBorderAgentId * aId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentGetId)(aInstance, aId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentSetEphemeralKey)(otInstance * aInstance,const char * aKeyString,uint32_t aTimeout,uint16_t aUdpPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentSetEphemeralKey)(aInstance, aKeyString, aTimeout, aUdpPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentSetId)(otInstance * aInstance,const otBorderAgentId * aId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentSetId)(aInstance, aId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otBorderAgentGetUdpPort)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otBorderAgentGetUdpPort)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otBorderAgentClearEphemeralKey)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentClearEphemeralKey)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentSetEphemeralKeyCallback)(otInstance * aInstance,otBorderAgentEphemeralKeyCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentSetEphemeralKeyCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

