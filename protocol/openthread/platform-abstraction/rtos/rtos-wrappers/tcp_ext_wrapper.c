/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Tcp Ext APIs
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

#include <openthread/tcp_ext.h>
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

extern int OT_API_REAL_NAME(otTcpMbedTlsSslRecvCallback)(void * aCtx,unsigned char * aBuf,size_t aLen);
extern int OT_API_REAL_NAME(otTcpMbedTlsSslSendCallback)(void * aCtx,const unsigned char * aBuf,size_t aLen);
extern otError OT_API_REAL_NAME(otTcpCircularSendBufferDeinitialize)(otTcpCircularSendBuffer * aSendBuffer);
extern otError OT_API_REAL_NAME(otTcpCircularSendBufferWrite)(otTcpEndpoint * aEndpoint,otTcpCircularSendBuffer * aSendBuffer,const void * aData,size_t aLength,size_t * aWritten,uint32_t aFlags);
extern size_t OT_API_REAL_NAME(otTcpCircularSendBufferGetFreeSpace)(const otTcpCircularSendBuffer * aSendBuffer);
extern void OT_API_REAL_NAME(otTcpCircularSendBufferForceDiscardAll)(otTcpCircularSendBuffer * aSendBuffer);
extern void OT_API_REAL_NAME(otTcpCircularSendBufferHandleForwardProgress)(otTcpCircularSendBuffer * aSendBuffer,size_t aInSendBuffer);
extern void OT_API_REAL_NAME(otTcpCircularSendBufferInitialize)(otTcpCircularSendBuffer * aSendBuffer,void * aDataBuffer,size_t aCapacity);

int OT_API_WRAPPER_NAME(otTcpMbedTlsSslRecvCallback)(void * aCtx,unsigned char * aBuf,size_t aLen)
{
    sl_ot_rtos_acquire_stack_mutex();
    int ret = OT_API_REAL_NAME(otTcpMbedTlsSslRecvCallback)(aCtx, aBuf, aLen);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int OT_API_WRAPPER_NAME(otTcpMbedTlsSslSendCallback)(void * aCtx,const unsigned char * aBuf,size_t aLen)
{
    sl_ot_rtos_acquire_stack_mutex();
    int ret = OT_API_REAL_NAME(otTcpMbedTlsSslSendCallback)(aCtx, aBuf, aLen);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpCircularSendBufferDeinitialize)(otTcpCircularSendBuffer * aSendBuffer)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpCircularSendBufferDeinitialize)(aSendBuffer);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpCircularSendBufferWrite)(otTcpEndpoint * aEndpoint,otTcpCircularSendBuffer * aSendBuffer,const void * aData,size_t aLength,size_t * aWritten,uint32_t aFlags)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpCircularSendBufferWrite)(aEndpoint, aSendBuffer, aData, aLength, aWritten, aFlags);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

size_t OT_API_WRAPPER_NAME(otTcpCircularSendBufferGetFreeSpace)(const otTcpCircularSendBuffer * aSendBuffer)
{
    sl_ot_rtos_acquire_stack_mutex();
    size_t ret = OT_API_REAL_NAME(otTcpCircularSendBufferGetFreeSpace)(aSendBuffer);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otTcpCircularSendBufferForceDiscardAll)(otTcpCircularSendBuffer * aSendBuffer)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otTcpCircularSendBufferForceDiscardAll)(aSendBuffer);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otTcpCircularSendBufferHandleForwardProgress)(otTcpCircularSendBuffer * aSendBuffer,size_t aInSendBuffer)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otTcpCircularSendBufferHandleForwardProgress)(aSendBuffer, aInSendBuffer);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otTcpCircularSendBufferInitialize)(otTcpCircularSendBuffer * aSendBuffer,void * aDataBuffer,size_t aCapacity)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otTcpCircularSendBufferInitialize)(aSendBuffer, aDataBuffer, aCapacity);
    sl_ot_rtos_release_stack_mutex();
}

