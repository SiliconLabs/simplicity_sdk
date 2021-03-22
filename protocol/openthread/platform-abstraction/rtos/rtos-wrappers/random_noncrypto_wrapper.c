/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Random Noncrypto APIs
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

#include <openthread/random_noncrypto.h>
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

extern uint16_t OT_API_REAL_NAME(otRandomNonCryptoGetUint16)(void);
extern uint16_t OT_API_REAL_NAME(otRandomNonCryptoGetUint16InRange)(uint16_t aMin,uint16_t aMax);
extern uint32_t OT_API_REAL_NAME(otRandomNonCryptoAddJitter)(uint32_t aValue,uint16_t aJitter);
extern uint32_t OT_API_REAL_NAME(otRandomNonCryptoGetUint32)(void);
extern uint32_t OT_API_REAL_NAME(otRandomNonCryptoGetUint32InRange)(uint32_t aMin,uint32_t aMax);
extern uint8_t OT_API_REAL_NAME(otRandomNonCryptoGetUint8)(void);
extern uint8_t OT_API_REAL_NAME(otRandomNonCryptoGetUint8InRange)(uint8_t aMin,uint8_t aMax);
extern void OT_API_REAL_NAME(otRandomNonCryptoFillBuffer)(uint8_t * aBuffer,uint16_t aSize);

uint16_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint16)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint16)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint16InRange)(uint16_t aMin,uint16_t aMax)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint16InRange)(aMin, aMax);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otRandomNonCryptoAddJitter)(uint32_t aValue,uint16_t aJitter)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otRandomNonCryptoAddJitter)(aValue, aJitter);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint32)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint32)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint32_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint32InRange)(uint32_t aMin,uint32_t aMax)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint32_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint32InRange)(aMin, aMax);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint8)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint8)();
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otRandomNonCryptoGetUint8InRange)(uint8_t aMin,uint8_t aMax)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otRandomNonCryptoGetUint8InRange)(aMin, aMax);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otRandomNonCryptoFillBuffer)(uint8_t * aBuffer,uint16_t aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otRandomNonCryptoFillBuffer)(aBuffer, aSize);
    sl_ot_rtos_release_stack_mutex();
}

