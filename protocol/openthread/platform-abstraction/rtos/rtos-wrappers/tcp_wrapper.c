/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Tcp APIs
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

#include <openthread/tcp.h>
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

extern const otSockAddr *OT_API_REAL_NAME(otTcpGetLocalAddress)(const otTcpEndpoint * aEndpoint);
extern const otSockAddr *OT_API_REAL_NAME(otTcpGetPeerAddress)(const otTcpEndpoint * aEndpoint);
extern otError OT_API_REAL_NAME(otTcpAbort)(otTcpEndpoint * aEndpoint);
extern otError OT_API_REAL_NAME(otTcpBind)(otTcpEndpoint * aEndpoint,const otSockAddr * aSockName);
extern otError OT_API_REAL_NAME(otTcpCommitReceive)(otTcpEndpoint * aEndpoint,size_t aNumBytes,uint32_t aFlags);
extern otError OT_API_REAL_NAME(otTcpConnect)(otTcpEndpoint * aEndpoint,const otSockAddr * aSockName,uint32_t aFlags);
extern otError OT_API_REAL_NAME(otTcpEndpointDeinitialize)(otTcpEndpoint * aEndpoint);
extern otError OT_API_REAL_NAME(otTcpEndpointInitialize)(otInstance * aInstance,otTcpEndpoint * aEndpoint,const otTcpEndpointInitializeArgs * aArgs);
extern otError OT_API_REAL_NAME(otTcpListen)(otTcpListener * aListener,const otSockAddr * aSockName);
extern otError OT_API_REAL_NAME(otTcpListenerDeinitialize)(otTcpListener * aListener);
extern otError OT_API_REAL_NAME(otTcpListenerInitialize)(otInstance * aInstance,otTcpListener * aListener,const otTcpListenerInitializeArgs * aArgs);
extern otError OT_API_REAL_NAME(otTcpReceiveByReference)(otTcpEndpoint * aEndpoint,const otLinkedBuffer ** aBuffer);
extern otError OT_API_REAL_NAME(otTcpReceiveContiguify)(otTcpEndpoint * aEndpoint);
extern otError OT_API_REAL_NAME(otTcpSendByExtension)(otTcpEndpoint * aEndpoint,size_t aNumBytes,uint32_t aFlags);
extern otError OT_API_REAL_NAME(otTcpSendByReference)(otTcpEndpoint * aEndpoint,otLinkedBuffer * aBuffer,uint32_t aFlags);
extern otError OT_API_REAL_NAME(otTcpSendEndOfStream)(otTcpEndpoint * aEndpoint);
extern otError OT_API_REAL_NAME(otTcpStopListening)(otTcpListener * aListener);
extern otInstance *OT_API_REAL_NAME(otTcpEndpointGetInstance)(otTcpEndpoint * aEndpoint);
extern otInstance *OT_API_REAL_NAME(otTcpListenerGetInstance)(otTcpListener * aListener);
extern void *OT_API_REAL_NAME(otTcpEndpointGetContext)(otTcpEndpoint * aEndpoint);
extern void *OT_API_REAL_NAME(otTcpListenerGetContext)(otTcpListener * aListener);

const otSockAddr *OT_API_WRAPPER_NAME(otTcpGetLocalAddress)(const otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSockAddr *ret = OT_API_REAL_NAME(otTcpGetLocalAddress)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otSockAddr *OT_API_WRAPPER_NAME(otTcpGetPeerAddress)(const otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otSockAddr *ret = OT_API_REAL_NAME(otTcpGetPeerAddress)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpAbort)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpAbort)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpBind)(otTcpEndpoint * aEndpoint,const otSockAddr * aSockName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpBind)(aEndpoint, aSockName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpCommitReceive)(otTcpEndpoint * aEndpoint,size_t aNumBytes,uint32_t aFlags)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpCommitReceive)(aEndpoint, aNumBytes, aFlags);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpConnect)(otTcpEndpoint * aEndpoint,const otSockAddr * aSockName,uint32_t aFlags)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpConnect)(aEndpoint, aSockName, aFlags);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpEndpointDeinitialize)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpEndpointDeinitialize)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpEndpointInitialize)(otInstance * aInstance,otTcpEndpoint * aEndpoint,const otTcpEndpointInitializeArgs * aArgs)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpEndpointInitialize)(aInstance, aEndpoint, aArgs);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpListen)(otTcpListener * aListener,const otSockAddr * aSockName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpListen)(aListener, aSockName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpListenerDeinitialize)(otTcpListener * aListener)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpListenerDeinitialize)(aListener);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpListenerInitialize)(otInstance * aInstance,otTcpListener * aListener,const otTcpListenerInitializeArgs * aArgs)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpListenerInitialize)(aInstance, aListener, aArgs);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpReceiveByReference)(otTcpEndpoint * aEndpoint,const otLinkedBuffer ** aBuffer)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpReceiveByReference)(aEndpoint, aBuffer);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpReceiveContiguify)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpReceiveContiguify)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpSendByExtension)(otTcpEndpoint * aEndpoint,size_t aNumBytes,uint32_t aFlags)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpSendByExtension)(aEndpoint, aNumBytes, aFlags);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpSendByReference)(otTcpEndpoint * aEndpoint,otLinkedBuffer * aBuffer,uint32_t aFlags)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpSendByReference)(aEndpoint, aBuffer, aFlags);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpSendEndOfStream)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpSendEndOfStream)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otTcpStopListening)(otTcpListener * aListener)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otTcpStopListening)(aListener);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otInstance *OT_API_WRAPPER_NAME(otTcpEndpointGetInstance)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    otInstance *ret = OT_API_REAL_NAME(otTcpEndpointGetInstance)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otInstance *OT_API_WRAPPER_NAME(otTcpListenerGetInstance)(otTcpListener * aListener)
{
    sl_ot_rtos_acquire_stack_mutex();
    otInstance *ret = OT_API_REAL_NAME(otTcpListenerGetInstance)(aListener);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void *OT_API_WRAPPER_NAME(otTcpEndpointGetContext)(otTcpEndpoint * aEndpoint)
{
    sl_ot_rtos_acquire_stack_mutex();
    void *ret = OT_API_REAL_NAME(otTcpEndpointGetContext)(aEndpoint);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void *OT_API_WRAPPER_NAME(otTcpListenerGetContext)(otTcpListener * aListener)
{
    sl_ot_rtos_acquire_stack_mutex();
    void *ret = OT_API_REAL_NAME(otTcpListenerGetContext)(aListener);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

