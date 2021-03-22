/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Ble Secure APIs
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

#include <openthread/ble_secure.h>
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

extern bool OT_API_REAL_NAME(otBleSecureIsCommandClassAuthorized)(otInstance * aInstance,otTcatCommandClass aCommandClass);
extern bool OT_API_REAL_NAME(otBleSecureIsConnected)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otBleSecureIsConnectionActive)(otInstance * aInstance);
extern bool OT_API_REAL_NAME(otBleSecureIsTcatEnabled)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBleSecureConnect)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBleSecureFlush)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otBleSecureGetPeerCertificateBase64)(otInstance * aInstance,unsigned char * aPeerCert,size_t * aCertLength);
extern otError OT_API_REAL_NAME(otBleSecureGetPeerSubjectAttributeByOid)(otInstance * aInstance,const char * aOid,size_t aOidLength,uint8_t * aAttributeBuffer,size_t * aAttributeLength,int * aAsn1Type);
extern otError OT_API_REAL_NAME(otBleSecureGetThreadAttributeFromOwnCertificate)(otInstance * aInstance,int aThreadOidDescriptor,uint8_t * aAttributeBuffer,size_t * aAttributeLength);
extern otError OT_API_REAL_NAME(otBleSecureGetThreadAttributeFromPeerCertificate)(otInstance * aInstance,int aThreadOidDescriptor,uint8_t * aAttributeBuffer,size_t * aAttributeLength);
extern otError OT_API_REAL_NAME(otBleSecureSend)(otInstance * aInstance,uint8_t * aBuf,uint16_t aLength);
extern otError OT_API_REAL_NAME(otBleSecureSendApplicationTlv)(otInstance * aInstance,uint8_t * aBuf,uint16_t aLength);
extern otError OT_API_REAL_NAME(otBleSecureSendMessage)(otInstance * aInstance,otMessage * aMessage);
extern otError OT_API_REAL_NAME(otBleSecureStart)(otInstance * aInstance,otHandleBleSecureConnect aConnectHandler,otHandleBleSecureReceive aReceiveHandler,bool aTlvMode,void * aContext);
extern otError OT_API_REAL_NAME(otBleSecureTcatStart)(otInstance * aInstance,const otTcatVendorInfo * aVendorInfo,otHandleTcatJoin aHandler);
extern void OT_API_REAL_NAME(otBleSecureDisconnect)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otBleSecureSetCaCertificateChain)(otInstance * aInstance,const uint8_t * aX509CaCertificateChain,uint32_t aX509CaCertChainLength);
extern void OT_API_REAL_NAME(otBleSecureSetCertificate)(otInstance * aInstance,const uint8_t * aX509Cert,uint32_t aX509Length,const uint8_t * aPrivateKey,uint32_t aPrivateKeyLength);
extern void OT_API_REAL_NAME(otBleSecureSetPsk)(otInstance * aInstance,const uint8_t * aPsk,uint16_t aPskLength,const uint8_t * aPskIdentity,uint16_t aPskIdLength);
extern void OT_API_REAL_NAME(otBleSecureSetSslAuthMode)(otInstance * aInstance,bool aVerifyPeerCertificate);
extern void OT_API_REAL_NAME(otBleSecureStop)(otInstance * aInstance);

bool OT_API_WRAPPER_NAME(otBleSecureIsCommandClassAuthorized)(otInstance * aInstance,otTcatCommandClass aCommandClass)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBleSecureIsCommandClassAuthorized)(aInstance, aCommandClass);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otBleSecureIsConnected)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBleSecureIsConnected)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otBleSecureIsConnectionActive)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBleSecureIsConnectionActive)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otBleSecureIsTcatEnabled)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBleSecureIsTcatEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureConnect)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureConnect)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureFlush)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureFlush)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureGetPeerCertificateBase64)(otInstance * aInstance,unsigned char * aPeerCert,size_t * aCertLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureGetPeerCertificateBase64)(aInstance, aPeerCert, aCertLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureGetPeerSubjectAttributeByOid)(otInstance * aInstance,const char * aOid,size_t aOidLength,uint8_t * aAttributeBuffer,size_t * aAttributeLength,int * aAsn1Type)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureGetPeerSubjectAttributeByOid)(aInstance, aOid, aOidLength, aAttributeBuffer, aAttributeLength, aAsn1Type);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureGetThreadAttributeFromOwnCertificate)(otInstance * aInstance,int aThreadOidDescriptor,uint8_t * aAttributeBuffer,size_t * aAttributeLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureGetThreadAttributeFromOwnCertificate)(aInstance, aThreadOidDescriptor, aAttributeBuffer, aAttributeLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureGetThreadAttributeFromPeerCertificate)(otInstance * aInstance,int aThreadOidDescriptor,uint8_t * aAttributeBuffer,size_t * aAttributeLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureGetThreadAttributeFromPeerCertificate)(aInstance, aThreadOidDescriptor, aAttributeBuffer, aAttributeLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureSend)(otInstance * aInstance,uint8_t * aBuf,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureSend)(aInstance, aBuf, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureSendApplicationTlv)(otInstance * aInstance,uint8_t * aBuf,uint16_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureSendApplicationTlv)(aInstance, aBuf, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureSendMessage)(otInstance * aInstance,otMessage * aMessage)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureSendMessage)(aInstance, aMessage);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureStart)(otInstance * aInstance,otHandleBleSecureConnect aConnectHandler,otHandleBleSecureReceive aReceiveHandler,bool aTlvMode,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureStart)(aInstance, aConnectHandler, aReceiveHandler, aTlvMode, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBleSecureTcatStart)(otInstance * aInstance,const otTcatVendorInfo * aVendorInfo,otHandleTcatJoin aHandler)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBleSecureTcatStart)(aInstance, aVendorInfo, aHandler);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otBleSecureDisconnect)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureDisconnect)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBleSecureSetCaCertificateChain)(otInstance * aInstance,const uint8_t * aX509CaCertificateChain,uint32_t aX509CaCertChainLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureSetCaCertificateChain)(aInstance, aX509CaCertificateChain, aX509CaCertChainLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBleSecureSetCertificate)(otInstance * aInstance,const uint8_t * aX509Cert,uint32_t aX509Length,const uint8_t * aPrivateKey,uint32_t aPrivateKeyLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureSetCertificate)(aInstance, aX509Cert, aX509Length, aPrivateKey, aPrivateKeyLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBleSecureSetPsk)(otInstance * aInstance,const uint8_t * aPsk,uint16_t aPskLength,const uint8_t * aPskIdentity,uint16_t aPskIdLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureSetPsk)(aInstance, aPsk, aPskLength, aPskIdentity, aPskIdLength);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBleSecureSetSslAuthMode)(otInstance * aInstance,bool aVerifyPeerCertificate)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureSetSslAuthMode)(aInstance, aVerifyPeerCertificate);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBleSecureStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBleSecureStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

