/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Message APIs
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

#include <openthread/message.h>
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

extern bool OT_API_REAL_NAME(otMessageIsLinkSecurityEnabled)(const otMessage * aMessage);
extern bool OT_API_REAL_NAME(otMessageIsLoopbackToHostAllowed)(const otMessage * aMessage);
extern bool OT_API_REAL_NAME(otMessageIsMulticastLoopEnabled)(otMessage * aMessage);
extern int OT_API_REAL_NAME(otMessageWrite)(otMessage * aMessage,uint16_t aOffset,const void * aBuf,uint16_t aLength);
extern int8_t OT_API_REAL_NAME(otMessageGetRss)(const otMessage * aMessage);
extern otError OT_API_REAL_NAME(otMessageAppend)(otMessage * aMessage,const void * aBuf,uint16_t aLength);
extern otError OT_API_REAL_NAME(otMessageGetThreadLinkInfo)(const otMessage * aMessage,otThreadLinkInfo * aLinkInfo);
extern otError OT_API_REAL_NAME(otMessageSetLength)(otMessage * aMessage,uint16_t aLength);
extern otMessage *OT_API_REAL_NAME(otMessageQueueGetHead)(otMessageQueue * aQueue);
extern otMessage *OT_API_REAL_NAME(otMessageQueueGetNext)(otMessageQueue * aQueue,const otMessage * aMessage);
extern otMessageOrigin OT_API_REAL_NAME(otMessageGetOrigin)(const otMessage * aMessage);
extern uint16_t OT_API_REAL_NAME(otMessageGetLength)(const otMessage * aMessage);
extern uint16_t OT_API_REAL_NAME(otMessageGetOffset)(const otMessage * aMessage);
extern uint16_t OT_API_REAL_NAME(otMessageRead)(const otMessage * aMessage,uint16_t aOffset,void * aBuf,uint16_t aLength);
extern void OT_API_REAL_NAME(otMessageFree)(otMessage * aMessage);
extern void OT_API_REAL_NAME(otMessageGetBufferInfo)(otInstance * aInstance,otBufferInfo * aBufferInfo);
extern void OT_API_REAL_NAME(otMessageQueueDequeue)(otMessageQueue * aQueue,otMessage * aMessage);
extern void OT_API_REAL_NAME(otMessageQueueEnqueue)(otMessageQueue * aQueue,otMessage * aMessage);
extern void OT_API_REAL_NAME(otMessageQueueEnqueueAtHead)(otMessageQueue * aQueue,otMessage * aMessage);
extern void OT_API_REAL_NAME(otMessageQueueInit)(otMessageQueue * aQueue);
extern void OT_API_REAL_NAME(otMessageResetBufferInfo)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otMessageSetDirectTransmission)(otMessage * aMessage,bool aEnabled);
extern void OT_API_REAL_NAME(otMessageSetLoopbackToHostAllowed)(otMessage * aMessage,bool aAllowLoopbackToHost);
extern void OT_API_REAL_NAME(otMessageSetMulticastLoopEnabled)(otMessage * aMessage,bool aEnabled);
extern void OT_API_REAL_NAME(otMessageSetOffset)(otMessage * aMessage,uint16_t aOffset);
extern void OT_API_REAL_NAME(otMessageSetOrigin)(otMessage * aMessage,otMessageOrigin aOrigin);

bool OT_API_WRAPPER_NAME(otMessageIsLinkSecurityEnabled)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otMessageIsLinkSecurityEnabled)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otMessageIsLoopbackToHostAllowed)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otMessageIsLoopbackToHostAllowed)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otMessageIsMulticastLoopEnabled)(otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otMessageIsMulticastLoopEnabled)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int OT_API_WRAPPER_NAME(otMessageWrite)(otMessage * aMessage,uint16_t aOffset,const void * aBuf,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    int ret = OT_API_REAL_NAME(otMessageWrite)(aMessage, aOffset, aBuf, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

int8_t OT_API_WRAPPER_NAME(otMessageGetRss)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    int8_t ret = OT_API_REAL_NAME(otMessageGetRss)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMessageAppend)(otMessage * aMessage,const void * aBuf,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMessageAppend)(aMessage, aBuf, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMessageGetThreadLinkInfo)(const otMessage * aMessage,otThreadLinkInfo * aLinkInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMessageGetThreadLinkInfo)(aMessage, aLinkInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMessageSetLength)(otMessage * aMessage,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMessageSetLength)(aMessage, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otMessageQueueGetHead)(otMessageQueue * aQueue)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otMessageQueueGetHead)(aQueue);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otMessageQueueGetNext)(otMessageQueue * aQueue,const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otMessageQueueGetNext)(aQueue, aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessageOrigin OT_API_WRAPPER_NAME(otMessageGetOrigin)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessageOrigin ret = OT_API_REAL_NAME(otMessageGetOrigin)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otMessageGetLength)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otMessageGetLength)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otMessageGetOffset)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otMessageGetOffset)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otMessageRead)(const otMessage * aMessage,uint16_t aOffset,void * aBuf,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otMessageRead)(aMessage, aOffset, aBuf, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otMessageFree)(otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageFree)(aMessage);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageGetBufferInfo)(otInstance * aInstance,otBufferInfo * aBufferInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageGetBufferInfo)(aInstance, aBufferInfo);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageQueueDequeue)(otMessageQueue * aQueue,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageQueueDequeue)(aQueue, aMessage);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageQueueEnqueue)(otMessageQueue * aQueue,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageQueueEnqueue)(aQueue, aMessage);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageQueueEnqueueAtHead)(otMessageQueue * aQueue,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageQueueEnqueueAtHead)(aQueue, aMessage);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageQueueInit)(otMessageQueue * aQueue)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageQueueInit)(aQueue);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageResetBufferInfo)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageResetBufferInfo)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageSetDirectTransmission)(otMessage * aMessage,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageSetDirectTransmission)(aMessage, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageSetLoopbackToHostAllowed)(otMessage * aMessage,bool aAllowLoopbackToHost)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageSetLoopbackToHostAllowed)(aMessage, aAllowLoopbackToHost);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageSetMulticastLoopEnabled)(otMessage * aMessage,bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageSetMulticastLoopEnabled)(aMessage, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageSetOffset)(otMessage * aMessage,uint16_t aOffset)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageSetOffset)(aMessage, aOffset);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otMessageSetOrigin)(otMessage * aMessage,otMessageOrigin aOrigin)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMessageSetOrigin)(aMessage, aOrigin);
    sl_ot_rtos_release_stack_mutex();
}

