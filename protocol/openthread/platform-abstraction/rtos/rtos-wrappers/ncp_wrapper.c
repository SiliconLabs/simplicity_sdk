/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Ncp APIs
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

#include <openthread/ncp.h>
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

extern otError OT_API_REAL_NAME(otNcpStreamWrite)(int aStreamId,const uint8_t * aDataPtr,int aDataLen);
extern void OT_API_REAL_NAME(otNcpHdlcInit)(otInstance * aInstance,otNcpHdlcSendCallback aSendCallback);
extern void OT_API_REAL_NAME(otNcpHdlcInitMulti)(otInstance ** aInstance,uint8_t aCount,otNcpHdlcSendCallback aSendCallback);
extern void OT_API_REAL_NAME(otNcpHdlcReceive)(const uint8_t * aBuf,uint16_t aBufLength);
extern void OT_API_REAL_NAME(otNcpHdlcSendDone)(void);
extern void OT_API_REAL_NAME(otNcpPlatLogv)(otLogLevel aLogLevel,otLogRegion aLogRegion,const char * aFormat,va_list aArgs);
extern void OT_API_REAL_NAME(otNcpRegisterPeekPokeDelegates)(otNcpDelegateAllowPeekPoke aAllowPeekDelegate,otNcpDelegateAllowPeekPoke aAllowPokeDelegate);
extern void OT_API_REAL_NAME(otNcpSpiInit)(otInstance * aInstance);

otError OT_API_WRAPPER_NAME(otNcpStreamWrite)(int aStreamId,const uint8_t * aDataPtr,int aDataLen)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otNcpStreamWrite)(aStreamId, aDataPtr, aDataLen);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otNcpHdlcInit)(otInstance * aInstance,otNcpHdlcSendCallback aSendCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpHdlcInit)(aInstance, aSendCallback);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpHdlcInitMulti)(otInstance ** aInstance,uint8_t aCount,otNcpHdlcSendCallback aSendCallback)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpHdlcInitMulti)(aInstance, aCount, aSendCallback);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpHdlcReceive)(const uint8_t * aBuf,uint16_t aBufLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpHdlcReceive)(aBuf, aBufLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpHdlcSendDone)(void)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpHdlcSendDone)();
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpPlatLogv)(otLogLevel aLogLevel,otLogRegion aLogRegion,const char * aFormat,va_list aArgs)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpPlatLogv)(aLogLevel, aLogRegion, aFormat, aArgs);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpRegisterPeekPokeDelegates)(otNcpDelegateAllowPeekPoke aAllowPeekDelegate,otNcpDelegateAllowPeekPoke aAllowPokeDelegate)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpRegisterPeekPokeDelegates)(aAllowPeekDelegate, aAllowPokeDelegate);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otNcpSpiInit)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otNcpSpiInit)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

