/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Udp APIs
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

#include <openthread/udp.h>
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

extern bool OT_API_REAL_NAME(otUdpIsOpen)(otInstance * aInstance,const otUdpSocket * aSocket);
extern bool OT_API_REAL_NAME(otUdpIsPortInUse)(otInstance * aInstance,uint16_t port);
extern otError OT_API_REAL_NAME(otUdpAddReceiver)(otInstance * aInstance,otUdpReceiver * aUdpReceiver);
extern otError OT_API_REAL_NAME(otUdpBind)(otInstance * aInstance,otUdpSocket * aSocket,const otSockAddr * aSockName,otNetifIdentifier aNetif);
extern otError OT_API_REAL_NAME(otUdpClose)(otInstance * aInstance,otUdpSocket * aSocket);
extern otError OT_API_REAL_NAME(otUdpConnect)(otInstance * aInstance,otUdpSocket * aSocket,const otSockAddr * aSockName);
extern otError OT_API_REAL_NAME(otUdpOpen)(otInstance * aInstance,otUdpSocket * aSocket,otUdpReceive aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otUdpRemoveReceiver)(otInstance * aInstance,otUdpReceiver * aUdpReceiver);
extern otError OT_API_REAL_NAME(otUdpSend)(otInstance * aInstance,otUdpSocket * aSocket,otMessage * aMessage,const otMessageInfo * aMessageInfo);
extern otError OT_API_REAL_NAME(otUdpSendDatagram)(otInstance * aInstance,otMessage * aMessage,otMessageInfo * aMessageInfo);
extern otMessage *OT_API_REAL_NAME(otUdpNewMessage)(otInstance * aInstance,const otMessageSettings * aSettings);
extern otUdpSocket *OT_API_REAL_NAME(otUdpGetSockets)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otUdpForwardReceive)(otInstance * aInstance,otMessage * aMessage,uint16_t aPeerPort,const otIp6Address * aPeerAddr,uint16_t aSockPort);
extern void OT_API_REAL_NAME(otUdpForwardSetForwarder)(otInstance * aInstance,otUdpForwarder aForwarder,void * aContext);

bool OT_API_WRAPPER_NAME(otUdpIsOpen)(otInstance * aInstance,const otUdpSocket * aSocket)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otUdpIsOpen)(aInstance, aSocket);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otUdpIsPortInUse)(otInstance * aInstance,uint16_t port)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otUdpIsPortInUse)(aInstance, port);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpAddReceiver)(otInstance * aInstance,otUdpReceiver * aUdpReceiver)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpAddReceiver)(aInstance, aUdpReceiver);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpBind)(otInstance * aInstance,otUdpSocket * aSocket,const otSockAddr * aSockName,otNetifIdentifier aNetif)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpBind)(aInstance, aSocket, aSockName, aNetif);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpClose)(otInstance * aInstance,otUdpSocket * aSocket)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpClose)(aInstance, aSocket);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpConnect)(otInstance * aInstance,otUdpSocket * aSocket,const otSockAddr * aSockName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpConnect)(aInstance, aSocket, aSockName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpOpen)(otInstance * aInstance,otUdpSocket * aSocket,otUdpReceive aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpOpen)(aInstance, aSocket, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpRemoveReceiver)(otInstance * aInstance,otUdpReceiver * aUdpReceiver)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpRemoveReceiver)(aInstance, aUdpReceiver);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpSend)(otInstance * aInstance,otUdpSocket * aSocket,otMessage * aMessage,const otMessageInfo * aMessageInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpSend)(aInstance, aSocket, aMessage, aMessageInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otUdpSendDatagram)(otInstance * aInstance,otMessage * aMessage,otMessageInfo * aMessageInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otUdpSendDatagram)(aInstance, aMessage, aMessageInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otUdpNewMessage)(otInstance * aInstance,const otMessageSettings * aSettings)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otUdpNewMessage)(aInstance, aSettings);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otUdpSocket *OT_API_WRAPPER_NAME(otUdpGetSockets)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otUdpSocket *ret = OT_API_REAL_NAME(otUdpGetSockets)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otUdpForwardReceive)(otInstance * aInstance,otMessage * aMessage,uint16_t aPeerPort,const otIp6Address * aPeerAddr,uint16_t aSockPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otUdpForwardReceive)(aInstance, aMessage, aPeerPort, aPeerAddr, aSockPort);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otUdpForwardSetForwarder)(otInstance * aInstance,otUdpForwarder aForwarder,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otUdpForwardSetForwarder)(aInstance, aForwarder, aContext);
    sl_ot_rtos_release_stack_mutex();
}

