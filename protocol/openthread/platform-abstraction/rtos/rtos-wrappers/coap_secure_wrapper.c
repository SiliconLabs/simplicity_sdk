/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Coap Secure APIs
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

#include <openthread/coap_secure.h>
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

extern bool OT_API_REAL_NAME(otCoapSecureIsClosed)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otCoapSecureIsConnected)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otCoapSecureIsConnectionActive)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otCoapSecureConnect)(otInstance * aInstance,const otSockAddr * aSockAddr,otHandleCoapSecureClientConnect aHandler,void * aContext);
extern otError OT_API_REAL_NAME(otCoapSecureGetPeerCertificateBase64)(otInstance * aInstance,unsigned char * aPeerCert,size_t * aCertLength,size_t aCertBufferSize);
extern otError OT_API_REAL_NAME(otCoapSecureSendRequest)(otInstance * aInstance,otMessage * aMessage,otCoapResponseHandler aHandler,void * aContext);
extern otError OT_API_REAL_NAME(otCoapSecureSendRequestBlockWise)(otInstance * aInstance,otMessage * aMessage,otCoapResponseHandler aHandler,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook,otCoapBlockwiseReceiveHook aReceiveHook);
extern otError OT_API_REAL_NAME(otCoapSecureSendResponse)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo);
extern otError OT_API_REAL_NAME(otCoapSecureSendResponseBlockWise)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook);
extern otError OT_API_REAL_NAME(otCoapSecureStart)(otInstance * aInstance,uint16_t aPort);
extern otError OT_API_REAL_NAME(otCoapSecureStartWithMaxConnAttempts)(otInstance * aInstance,uint16_t aPort,uint16_t aMaxAttempts,otCoapSecureAutoStopCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otCoapSecureAddBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource);
extern void OT_API_REAL_NAME(otCoapSecureAddResource)(otInstance * aInstance,otCoapResource * aResource);
extern void OT_API_REAL_NAME(otCoapSecureDisconnect)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otCoapSecureRemoveBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource);
extern void OT_API_REAL_NAME(otCoapSecureRemoveResource)(otInstance * aInstance,otCoapResource * aResource);
extern void OT_API_REAL_NAME(otCoapSecureSetCaCertificateChain)(otInstance * aInstance,const uint8_t * aX509CaCertificateChain,uint32_t aX509CaCertChainLength);
extern void OT_API_REAL_NAME(otCoapSecureSetCertificate)(otInstance * aInstance,const uint8_t * aX509Cert,uint32_t aX509Length,const uint8_t * aPrivateKey,uint32_t aPrivateKeyLength);
extern void OT_API_REAL_NAME(otCoapSecureSetClientConnectedCallback)(otInstance * aInstance,otHandleCoapSecureClientConnect aHandler,void * aContext);
extern void OT_API_REAL_NAME(otCoapSecureSetDefaultHandler)(otInstance * aInstance,otCoapRequestHandler aHandler,void * aContext);
extern void OT_API_REAL_NAME(otCoapSecureSetPsk)(otInstance * aInstance,const uint8_t * aPsk,uint16_t aPskLength,const uint8_t * aPskIdentity,uint16_t aPskIdLength);
extern void OT_API_REAL_NAME(otCoapSecureSetSslAuthMode)(otInstance * aInstance,bool aVerifyPeerCertificate);
extern void OT_API_REAL_NAME(otCoapSecureStop)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otCoapSecureIsClosed)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otCoapSecureIsClosed)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otCoapSecureIsConnected)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otCoapSecureIsConnected)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otCoapSecureIsConnectionActive)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otCoapSecureIsConnectionActive)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureConnect)(otInstance * aInstance,const otSockAddr * aSockAddr,otHandleCoapSecureClientConnect aHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureConnect)(aInstance, aSockAddr, aHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureGetPeerCertificateBase64)(otInstance * aInstance,unsigned char * aPeerCert,size_t * aCertLength,size_t aCertBufferSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureGetPeerCertificateBase64)(aInstance, aPeerCert, aCertLength, aCertBufferSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureSendRequest)(otInstance * aInstance,otMessage * aMessage,otCoapResponseHandler aHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureSendRequest)(aInstance, aMessage, aHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureSendRequestBlockWise)(otInstance * aInstance,otMessage * aMessage,otCoapResponseHandler aHandler,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook,otCoapBlockwiseReceiveHook aReceiveHook)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureSendRequestBlockWise)(aInstance, aMessage, aHandler, aContext, aTransmitHook, aReceiveHook);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureSendResponse)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureSendResponse)(aInstance, aMessage, aMessageInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureSendResponseBlockWise)(otInstance * aInstance,otMessage * aMessage,const otMessageInfo * aMessageInfo,void * aContext,otCoapBlockwiseTransmitHook aTransmitHook)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureSendResponseBlockWise)(aInstance, aMessage, aMessageInfo, aContext, aTransmitHook);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureStart)(otInstance * aInstance,uint16_t aPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureStart)(aInstance, aPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCoapSecureStartWithMaxConnAttempts)(otInstance * aInstance,uint16_t aPort,uint16_t aMaxAttempts,otCoapSecureAutoStopCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCoapSecureStartWithMaxConnAttempts)(aInstance, aPort, aMaxAttempts, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otCoapSecureAddBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureAddBlockWiseResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureAddResource)(otInstance * aInstance,otCoapResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureAddResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureDisconnect)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureDisconnect)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureRemoveBlockWiseResource)(otInstance * aInstance,otCoapBlockwiseResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureRemoveBlockWiseResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureRemoveResource)(otInstance * aInstance,otCoapResource * aResource)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureRemoveResource)(aInstance, aResource);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetCaCertificateChain)(otInstance * aInstance,const uint8_t * aX509CaCertificateChain,uint32_t aX509CaCertChainLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetCaCertificateChain)(aInstance, aX509CaCertificateChain, aX509CaCertChainLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetCertificate)(otInstance * aInstance,const uint8_t * aX509Cert,uint32_t aX509Length,const uint8_t * aPrivateKey,uint32_t aPrivateKeyLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetCertificate)(aInstance, aX509Cert, aX509Length, aPrivateKey, aPrivateKeyLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetClientConnectedCallback)(otInstance * aInstance,otHandleCoapSecureClientConnect aHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetClientConnectedCallback)(aInstance, aHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetDefaultHandler)(otInstance * aInstance,otCoapRequestHandler aHandler,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetDefaultHandler)(aInstance, aHandler, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetPsk)(otInstance * aInstance,const uint8_t * aPsk,uint16_t aPskLength,const uint8_t * aPskIdentity,uint16_t aPskIdLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetPsk)(aInstance, aPsk, aPskLength, aPskIdentity, aPskIdLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureSetSslAuthMode)(otInstance * aInstance,bool aVerifyPeerCertificate)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureSetSslAuthMode)(aInstance, aVerifyPeerCertificate);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCoapSecureStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCoapSecureStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

