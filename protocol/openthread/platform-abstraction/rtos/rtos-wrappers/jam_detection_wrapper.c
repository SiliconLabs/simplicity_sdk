/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Jam Detection APIs
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

#include <openthread/jam_detection.h>
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

extern bool OT_API_REAL_NAME(otJamDetectionGetState)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otJamDetectionIsEnabled)(otInstance * aInstance);
extern int8_t OT_API_REAL_NAME(otJamDetectionGetRssiThreshold)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otJamDetectionSetBusyPeriod)(otInstance * aInstance,uint8_t aBusyPeriod);
extern otError OT_API_REAL_NAME(otJamDetectionSetRssiThreshold)(otInstance * aInstance,int8_t aRssiThreshold);
extern otError OT_API_REAL_NAME(otJamDetectionSetWindow)(otInstance * aInstance,uint8_t aWindow);
extern otError OT_API_REAL_NAME(otJamDetectionStart)(otInstance * aInstance,otJamDetectionCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otJamDetectionStop)(otInstance * aInstance);
extern uint64_t OT_API_REAL_NAME(otJamDetectionGetHistoryBitmap)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otJamDetectionGetBusyPeriod)(otInstance * aInstance);
extern uint8_t OT_API_REAL_NAME(otJamDetectionGetWindow)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otJamDetectionGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otJamDetectionGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otJamDetectionIsEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otJamDetectionIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int8_t OT_API_WRAPPER_NAME(otJamDetectionGetRssiThreshold)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    int8_t ret = OT_API_REAL_NAME(otJamDetectionGetRssiThreshold)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJamDetectionSetBusyPeriod)(otInstance * aInstance,uint8_t aBusyPeriod)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJamDetectionSetBusyPeriod)(aInstance, aBusyPeriod);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJamDetectionSetRssiThreshold)(otInstance * aInstance,int8_t aRssiThreshold)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJamDetectionSetRssiThreshold)(aInstance, aRssiThreshold);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJamDetectionSetWindow)(otInstance * aInstance,uint8_t aWindow)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJamDetectionSetWindow)(aInstance, aWindow);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJamDetectionStart)(otInstance * aInstance,otJamDetectionCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJamDetectionStart)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otJamDetectionStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otJamDetectionStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint64_t OT_API_WRAPPER_NAME(otJamDetectionGetHistoryBitmap)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint64_t ret = OT_API_REAL_NAME(otJamDetectionGetHistoryBitmap)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otJamDetectionGetBusyPeriod)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otJamDetectionGetBusyPeriod)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otJamDetectionGetWindow)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otJamDetectionGetWindow)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

