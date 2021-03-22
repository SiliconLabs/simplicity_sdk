/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Coap APIs
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

#include <openthread/coap.h>
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

extern const char *OT_API_REAL_NAME(otCoapMessageCodeToString)(const otMessage * aMessage);
extern const otCoapOption *OT_API_REAL_NAME(otCoapOptionIteratorGetFirstOption)(otCoapOptionIterator * aIterator);
extern const otCoapOption *OT_API_REAL_NAME(otCoapOptionIteratorGetFirstOptionMatching)(otCoapOptionIterator * aIterator,uint16_t aOption);
extern const otCoapOption *OT_API_REAL_NAME(otCoapOptionIteratorGetNextOption)(otCoapOptionIterator * aIterator);
extern const otCoapOption *OT_API_REAL_NAME(otCoapOptionIteratorGetNextOptionMatching)(otCoapOptionIterator * aIterator,uint16_t aOption);
extern const uint8_t *OT_API_REAL_NAME(otCoapMessageGetToken)(const otMessage * aMessage);
extern otCoapCode OT_API_REAL_NAME(otCoapMessageGetCode)(const otMessage * aMessage);
extern otCoapType OT_API_REAL_NAME(otCoapMessageGetType)(const otMessage * aMessage);
extern otError OT_API_REAL_NAME(otCoapMessageAppendBlock1Option)(otMessage * aMessage,uint32_t aNum,bool aMore,otCoapBlockSzx aSize);
extern otError OT_API_REAL_NAME(otCoapMessageAppendBlock2Option)(otMessage * aMessage,uint32_t aNum,bool aMore,otCoapBlockSzx aSize);
extern otError OT_API_REAL_NAME(otCoapMessageAppendContentFormatOption)(otMessage * aMessage,otCoapOptionContentFormat aContentFormat);
extern otError OT_API_REAL_NAME(otCoapMessageAppendMaxAgeOption)(otMessage * aMessage,uint32_t aMaxAge);
extern otError OT_API_REAL_NAME(otCoapMessageAppendObserveOption)(otMessage * aMessage,uint32_t aObserve);
extern otError OT_API_REAL_NAME(otCoapMessageAppendOption)(otMessage * aMessage,uint16_t aNumber,uint16_t aLength,const void * aValue);
extern otError OT_API_REAL_NAME(otCoapMessageAppendProxyUriOption)(otMessage * aMessage,const char * aUriPath);
extern otError OT_API_REAL_NAME(otCoapMessageAppendUintOption)(otMessage * aMessage,uint16_t aNumber,uint32_t aValue);
extern otError OT_API_REAL_NAME(otCoapMessageAppendUriPathOptions)(otMessage * aMessage,const char * aUriPath);
extern otError OT_API_REAL_NAME(otCoapMessageAppendUriQueryOption)(otMessage * aMessage,const char * aUriQuery);
extern otError OT_API_REAL_NAME(otCoapMessageInitResponse)(otMessage * aResponse,const otMessage * aRequest,otCoapType aType,otCoapCode aCode);
extern otError OT_API_REAL_NAME(otCoapMessageSetPayloadMarker)(otMessage * aMessage);
extern otError OT_API_REAL_NAME(otCoapMessageSetToken)(otMessage * aMessage,const uint8_t * aToken,uint8_t aTokenLength);
extern otError OT_API_REAL_NAME(otCoapOptionIteratorGetOptionUintValue)(otCoapOptionIterator * aIterator,uint64_t * aValue);
extern otError OT_API_REAL_NAME(otCoapOptionIteratorGetOptionValue)(otCoapOptionIterator * aIterator,void * aValue);
extern otError OT_API_REAL_NAME(otCoapOptionIteratorInit)(otCoapOptionIterator * aIterator,const otMessage * aMessage);
extern otError OT_API_REAL_NAME(otCoapSendRequestBlockWiseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,otCoapResponseHandler aHandler,void * aContext,const otCoapTxParameters * aTxParameters,otCoapBlockwiseTransmitHook aTransmitHook,otCoapBlockwiseReceiveHook aReceiveHook);
extern otError OT_API_REAL_NAME(otCoapSendRequestWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,otCoapResponseHandler aHandler,void * aContext,const otCoapTxParameters * aTxParameters);
extern otError OT_API_REAL_NAME(otCoapSendResponseBlockWiseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,const otCoapTxParameters * aTxParameters,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook);
extern otError OT_API_REAL_NAME(otCoapSendResponseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,const otCoapTxParameters * aTxParameters);
extern otError OT_API_REAL_NAME(otCoapStart)(otInstance * aInstance,uint16_t aPort);
extern otError OT_API_REAL_NAME(otCoapStop)(otInstance * aInstance);
extern otMessage *OT_API_REAL_NAME(otCoapNewMessage)(otInstance * aInstance,const otMessageSettings * aSettings);
extern uint16_t OT_API_REAL_NAME(otCoapBlockSizeFromExponent)(otCoapBlockSzx aSize);
extern uint16_t OT_API_REAL_NAME(otCoapMessageGetMessageId)(const otMessage * aMessage);
extern uint8_t OT_API_REAL_NAME(otCoapMessageGetTokenLength)(const otMessage * aMessage);
extern void OT_API_REAL_NAME(otCoapAddBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource);
extern void OT_API_REAL_NAME(otCoapAddResource)(otInstance * aInstance,otCoapResource * aResource);
extern void OT_API_REAL_NAME(otCoapMessageGenerateToken)(otMessage * aMessage,uint8_t aTokenLength);
extern void OT_API_REAL_NAME(otCoapMessageInit)(otMessage * aMessage,otCoapType aType,otCoapCode aCode);
extern void OT_API_REAL_NAME(otCoapMessageSetCode)(otMessage * aMessage,otCoapCode aCode);
extern void OT_API_REAL_NAME(otCoapRemoveBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource);
extern void OT_API_REAL_NAME(otCoapRemoveResource)(otInstance * aInstance,otCoapResource * aResource);
extern void OT_API_REAL_NAME(otCoapSetDefaultHandler)(otInstance * aInstance,otCoapRequestHandler aHandler,void * aContext);

const char *OT_API_WRAPPER_NAME(otCoapMessageCodeToString)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otCoapMessageCodeToString)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otCoapOption *OT_API_WRAPPER_NAME(otCoapOptionIteratorGetFirstOption)(otCoapOptionIterator * aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otCoapOption *ret = OT_API_REAL_NAME(otCoapOptionIteratorGetFirstOption)(aIterator);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otCoapOption *OT_API_WRAPPER_NAME(otCoapOptionIteratorGetFirstOptionMatching)(otCoapOptionIterator * aIterator,uint16_t aOption)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otCoapOption *ret = OT_API_REAL_NAME(otCoapOptionIteratorGetFirstOptionMatching)(aIterator, aOption);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otCoapOption *OT_API_WRAPPER_NAME(otCoapOptionIteratorGetNextOption)(otCoapOptionIterator * aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otCoapOption *ret = OT_API_REAL_NAME(otCoapOptionIteratorGetNextOption)(aIterator);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otCoapOption *OT_API_WRAPPER_NAME(otCoapOptionIteratorGetNextOptionMatching)(otCoapOptionIterator * aIterator,uint16_t aOption)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otCoapOption *ret = OT_API_REAL_NAME(otCoapOptionIteratorGetNextOptionMatching)(aIterator, aOption);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const uint8_t *OT_API_WRAPPER_NAME(otCoapMessageGetToken)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    const uint8_t *ret = OT_API_REAL_NAME(otCoapMessageGetToken)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otCoapCode OT_API_WRAPPER_NAME(otCoapMessageGetCode)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otCoapCode ret = OT_API_REAL_NAME(otCoapMessageGetCode)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otCoapType OT_API_WRAPPER_NAME(otCoapMessageGetType)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otCoapType ret = OT_API_REAL_NAME(otCoapMessageGetType)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendBlock1Option)(otMessage * aMessage,uint32_t aNum,bool aMore,otCoapBlockSzx aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendBlock1Option)(aMessage, aNum, aMore, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendBlock2Option)(otMessage * aMessage,uint32_t aNum,bool aMore,otCoapBlockSzx aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendBlock2Option)(aMessage, aNum, aMore, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendContentFormatOption)(otMessage * aMessage,otCoapOptionContentFormat aContentFormat)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendContentFormatOption)(aMessage, aContentFormat);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendMaxAgeOption)(otMessage * aMessage,uint32_t aMaxAge)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendMaxAgeOption)(aMessage, aMaxAge);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendObserveOption)(otMessage * aMessage,uint32_t aObserve)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendObserveOption)(aMessage, aObserve);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendOption)(otMessage * aMessage,uint16_t aNumber,uint16_t aLength,const void * aValue)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendOption)(aMessage, aNumber, aLength, aValue);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendProxyUriOption)(otMessage * aMessage,const char * aUriPath)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendProxyUriOption)(aMessage, aUriPath);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendUintOption)(otMessage * aMessage,uint16_t aNumber,uint32_t aValue)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendUintOption)(aMessage, aNumber, aValue);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendUriPathOptions)(otMessage * aMessage,const char * aUriPath)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendUriPathOptions)(aMessage, aUriPath);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageAppendUriQueryOption)(otMessage * aMessage,const char * aUriQuery)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageAppendUriQueryOption)(aMessage, aUriQuery);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageInitResponse)(otMessage * aResponse,const otMessage * aRequest,otCoapType aType,otCoapCode aCode)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageInitResponse)(aResponse, aRequest, aType, aCode);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageSetPayloadMarker)(otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageSetPayloadMarker)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapMessageSetToken)(otMessage * aMessage,const uint8_t * aToken,uint8_t aTokenLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapMessageSetToken)(aMessage, aToken, aTokenLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapOptionIteratorGetOptionUintValue)(otCoapOptionIterator * aIterator,uint64_t * aValue)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapOptionIteratorGetOptionUintValue)(aIterator, aValue);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapOptionIteratorGetOptionValue)(otCoapOptionIterator * aIterator,void * aValue)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapOptionIteratorGetOptionValue)(aIterator, aValue);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapOptionIteratorInit)(otCoapOptionIterator * aIterator,const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapOptionIteratorInit)(aIterator, aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSendRequestBlockWiseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,otCoapResponseHandler aHandler,void * aContext,const otCoapTxParameters * aTxParameters,otCoapBlockwiseTransmitHook aTransmitHook,otCoapBlockwiseReceiveHook aReceiveHook)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSendRequestBlockWiseWithParameters)(aInstance, aMessage, aMessageInfo, aHandler, aContext, aTxParameters, aTransmitHook, aReceiveHook);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSendRequestWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,otCoapResponseHandler aHandler,void * aContext,const otCoapTxParameters * aTxParameters)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSendRequestWithParameters)(aInstance, aMessage, aMessageInfo, aHandler, aContext, aTxParameters);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSendResponseBlockWiseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,const otCoapTxParameters * aTxParameters,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSendResponseBlockWiseWithParameters)(aInstance, aMessage, aMessageInfo, aTxParameters, aContext, aTransmitHook);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSendResponseWithParameters)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,const otCoapTxParameters * aTxParameters)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSendResponseWithParameters)(aInstance, aMessage, aMessageInfo, aTxParameters);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapStart)(otInstance * aInstance,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapStart)(aInstance, aPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otMessage *OT_API_WRAPPER_NAME(otCoapNewMessage)(otInstance * aInstance,const otMessageSettings * aSettings)
{
    sl_ot_rtos_acquire_stack_mutex();
    otMessage *ret = OT_API_REAL_NAME(otCoapNewMessage)(aInstance, aSettings);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otCoapBlockSizeFromExponent)(otCoapBlockSzx aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otCoapBlockSizeFromExponent)(aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otCoapMessageGetMessageId)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otCoapMessageGetMessageId)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t OT_API_WRAPPER_NAME(otCoapMessageGetTokenLength)(const otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t ret = OT_API_REAL_NAME(otCoapMessageGetTokenLength)(aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otCoapAddBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapAddBlockWiseResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapAddResource)(otInstance * aInstance,otCoapResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapAddResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapMessageGenerateToken)(otMessage * aMessage,uint8_t aTokenLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapMessageGenerateToken)(aMessage, aTokenLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapMessageInit)(otMessage * aMessage,otCoapType aType,otCoapCode aCode)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapMessageInit)(aMessage, aType, aCode);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapMessageSetCode)(otMessage * aMessage,otCoapCode aCode)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapMessageSetCode)(aMessage, aCode);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapRemoveBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapRemoveBlockWiseResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapRemoveResource)(otInstance * aInstance,otCoapResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapRemoveResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSetDefaultHandler)(otInstance * aInstance,otCoapRequestHandler aHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSetDefaultHandler)(aInstance, aHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
}

