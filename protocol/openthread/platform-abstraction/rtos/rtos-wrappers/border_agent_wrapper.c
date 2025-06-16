/*******************************************************************************
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Border Agent APIs
 *   allowing access to the thread stack in a multi-threaded environment.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_ot_rtos_adaptation.h"
#include <openthread/border_agent.h>

#if defined(__GNUC__)
#define REAL_NAME(function) __real_##function
#define WRAPPER_NAME(function) __wrap_##function
#define OT_API_REAL_NAME(function) REAL_NAME(function)
#define OT_API_WRAPPER_NAME(function) WRAPPER_NAME(function)
// #elif defined(__IAR_SYSTEMS_ICC__)
//     #define REAL_NAME(function)             $Super$$##function
//     #define WRAPPER_NAME(function)          $Sub$$##function
//     #define OT_API_REAL_NAME(function)      $Super$$__iar_dl##function
//     #define OT_API_WRAPPER_NAME(function)   $Sub$$__iar_dl##function
#else
#error Unsupported compiler
#endif

extern bool        OT_API_REAL_NAME(otBorderAgentIsActive)(otInstance *aInstance);
extern bool        OT_API_REAL_NAME(otBorderAgentIsEnabled)(otInstance *aInstance);
extern const char *OT_API_REAL_NAME(otBorderAgentEphemeralKeyStateToString)(otBorderAgentEphemeralKeyState aState);
extern const otBorderAgentCounters   *OT_API_REAL_NAME(otBorderAgentGetCounters)(otInstance *aInstance);
extern otBorderAgentEphemeralKeyState OT_API_REAL_NAME(otBorderAgentEphemeralKeyGetState)(otInstance *aInstance);
extern otError                        OT_API_REAL_NAME(otBorderAgentEphemeralKeyStart)(otInstance *aInstance,
                                                                const char *aKeyString,
                                                                uint32_t    aTimeout,
                                                                uint16_t    aUdpPort);
extern otError                        OT_API_REAL_NAME(otBorderAgentGetId)(otInstance *aInstance, otBorderAgentId *aId);
extern otError                        OT_API_REAL_NAME(otBorderAgentGetMeshCoPServiceTxtData)(otInstance                         *aInstance,
                                                                       otBorderAgentMeshCoPServiceTxtData *aTxtData);
extern otError  OT_API_REAL_NAME(otBorderAgentGetNextSessionInfo)(otBorderAgentSessionIterator *aIterator,
                                                                 otBorderAgentSessionInfo     *aSessionInfo);
extern otError  OT_API_REAL_NAME(otBorderAgentSetId)(otInstance *aInstance, const otBorderAgentId *aId);
extern otError  OT_API_REAL_NAME(otBorderAgentSetMeshCoPServiceBaseName)(otInstance *aInstance, const char *aBaseName);
extern uint16_t OT_API_REAL_NAME(otBorderAgentEphemeralKeyGetUdpPort)(otInstance *aInstance);
extern uint16_t OT_API_REAL_NAME(otBorderAgentGetUdpPort)(otInstance *aInstance);
extern void     OT_API_REAL_NAME(otBorderAgentEphemeralKeySetCallback)(otInstance                       *aInstance,
                                                                   otBorderAgentEphemeralKeyCallback aCallback,
                                                                   void                             *aContext);
extern void     OT_API_REAL_NAME(otBorderAgentEphemeralKeySetEnabled)(otInstance *aInstance, bool aEnabled);
extern void     OT_API_REAL_NAME(otBorderAgentEphemeralKeyStop)(otInstance *aInstance);
extern void     OT_API_REAL_NAME(otBorderAgentInitSessionIterator)(otInstance                   *aInstance,
                                                               otBorderAgentSessionIterator *aIterator);
extern void     OT_API_REAL_NAME(otBorderAgentSetEnabled)(otInstance *aInstance, bool aEnabled);
extern void     OT_API_REAL_NAME(otBorderAgentSetMeshCoPServiceChangedCallback)(
    otInstance                                *aInstance,
    otBorderAgentMeshCoPServiceChangedCallback aCallback,
    void                                      *aContext);
extern void OT_API_REAL_NAME(otBorderAgentSetVendorTxtData)(otInstance    *aInstance,
                                                            const uint8_t *aVendorData,
                                                            uint16_t       aVendorDataLength);

bool OT_API_WRAPPER_NAME(otBorderAgentIsActive)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBorderAgentIsActive)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

bool OT_API_WRAPPER_NAME(otBorderAgentIsEnabled)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    bool ret = OT_API_REAL_NAME(otBorderAgentIsEnabled)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeyStateToString)(otBorderAgentEphemeralKeyState aState)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otBorderAgentEphemeralKeyStateToString)(aState);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const otBorderAgentCounters *OT_API_WRAPPER_NAME(otBorderAgentGetCounters)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const otBorderAgentCounters *ret = OT_API_REAL_NAME(otBorderAgentGetCounters)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otBorderAgentEphemeralKeyState OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeyGetState)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otBorderAgentEphemeralKeyState ret = OT_API_REAL_NAME(otBorderAgentEphemeralKeyGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeyStart)(otInstance *aInstance,
                                                            const char *aKeyString,
                                                            uint32_t    aTimeout,
                                                            uint16_t    aUdpPort)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentEphemeralKeyStart)(aInstance, aKeyString, aTimeout, aUdpPort);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentGetId)(otInstance *aInstance, otBorderAgentId *aId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentGetId)(aInstance, aId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentGetMeshCoPServiceTxtData)(otInstance                         *aInstance,
                                                                   otBorderAgentMeshCoPServiceTxtData *aTxtData)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentGetMeshCoPServiceTxtData)(aInstance, aTxtData);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentGetNextSessionInfo)(otBorderAgentSessionIterator *aIterator,
                                                             otBorderAgentSessionInfo     *aSessionInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentGetNextSessionInfo)(aIterator, aSessionInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentSetId)(otInstance *aInstance, const otBorderAgentId *aId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentSetId)(aInstance, aId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otBorderAgentSetMeshCoPServiceBaseName)(otInstance *aInstance, const char *aBaseName)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otBorderAgentSetMeshCoPServiceBaseName)(aInstance, aBaseName);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeyGetUdpPort)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otBorderAgentEphemeralKeyGetUdpPort)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otBorderAgentGetUdpPort)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otBorderAgentGetUdpPort)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeySetCallback)(otInstance                       *aInstance,
                                                               otBorderAgentEphemeralKeyCallback aCallback,
                                                               void                             *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentEphemeralKeySetCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeySetEnabled)(otInstance *aInstance, bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentEphemeralKeySetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentEphemeralKeyStop)(otInstance *aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentEphemeralKeyStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentInitSessionIterator)(otInstance                   *aInstance,
                                                           otBorderAgentSessionIterator *aIterator)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentInitSessionIterator)(aInstance, aIterator);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentSetEnabled)(otInstance *aInstance, bool aEnabled)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentSetEnabled)(aInstance, aEnabled);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentSetMeshCoPServiceChangedCallback)(
    otInstance                                *aInstance,
    otBorderAgentMeshCoPServiceChangedCallback aCallback,
    void                                      *aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentSetMeshCoPServiceChangedCallback)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otBorderAgentSetVendorTxtData)(otInstance    *aInstance,
                                                        const uint8_t *aVendorData,
                                                        uint16_t       aVendorDataLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otBorderAgentSetVendorTxtData)(aInstance, aVendorData, aVendorDataLength);
    sl_ot_rtos_release_stack_mutex();
}
