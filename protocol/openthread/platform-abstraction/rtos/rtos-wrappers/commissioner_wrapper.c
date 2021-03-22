/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Commissioner APIs
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

#include <openthread/commissioner.h>
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

extern const char *OT_API_REAL_NAME(otCommissionerGetId)(otInstance * aInstance);
extern const char *OT_API_REAL_NAME(otCommissionerGetProvisioningUrl)(otInstance * aInstance);
extern otCommissionerState OT_API_REAL_NAME(otCommissionerGetState)(otInstance * aInstance);
extern otError OT_API_REAL_NAME(otCommissionerAddJoiner)(otInstance * aInstance,const otExtAddress * aEui64,const char * aPskd,uint32_t aTimeout);
extern otError OT_API_REAL_NAME(otCommissionerAddJoinerWithDiscerner)(otInstance * aInstance,const otJoinerDiscerner * aDiscerner,const char * aPskd,uint32_t aTimeout);
extern otError OT_API_REAL_NAME(otCommissionerAnnounceBegin)(otInstance * aInstance,uint32_t aChannelMask,uint8_t aCount,uint16_t aPeriod,const otIp6Address * aAddress);
extern otError OT_API_REAL_NAME(otCommissionerEnergyScan)(otInstance * aInstance,uint32_t aChannelMask,uint8_t aCount,uint16_t aPeriod,uint16_t aScanDuration,const otIp6Address * aAddress,otCommissionerEnergyReportCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otCommissionerGetNextJoinerInfo)(otInstance * aInstance,uint16_t * aIterator,otJoinerInfo * aJoiner);
extern otError OT_API_REAL_NAME(otCommissionerPanIdQuery)(otInstance * aInstance,uint16_t aPanId,uint32_t aChannelMask,const otIp6Address * aAddress,otCommissionerPanIdConflictCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otCommissionerRemoveJoiner)(otInstance * aInstance,const otExtAddress * aEui64);
extern otError OT_API_REAL_NAME(otCommissionerRemoveJoinerWithDiscerner)(otInstance * aInstance,const otJoinerDiscerner * aDiscerner);
extern otError OT_API_REAL_NAME(otCommissionerSendMgmtGet)(otInstance * aInstance,const uint8_t * aTlvs,uint8_t aLength);
extern otError OT_API_REAL_NAME(otCommissionerSendMgmtSet)(otInstance * aInstance,const otCommissioningDataset * aDataset,const uint8_t * aTlvs,uint8_t aLength);
extern otError OT_API_REAL_NAME(otCommissionerSetId)(otInstance * aInstance,const char * aId);
extern otError OT_API_REAL_NAME(otCommissionerSetProvisioningUrl)(otInstance * aInstance,const char * aProvisioningUrl);
extern otError OT_API_REAL_NAME(otCommissionerStart)(otInstance * aInstance,otCommissionerStateCallback aStateCallback,otCommissionerJoinerCallback aJoinerCallback,void * aCallbackContext);
extern otError OT_API_REAL_NAME(otCommissionerStop)(otInstance * aInstance);
extern uint16_t OT_API_REAL_NAME(otCommissionerGetSessionId)(otInstance * aInstance);

const char *OT_API_WRAPPER_NAME(otCommissionerGetId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otCommissionerGetId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char *OT_API_WRAPPER_NAME(otCommissionerGetProvisioningUrl)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char *ret = OT_API_REAL_NAME(otCommissionerGetProvisioningUrl)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otCommissionerState OT_API_WRAPPER_NAME(otCommissionerGetState)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otCommissionerState ret = OT_API_REAL_NAME(otCommissionerGetState)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerAddJoiner)(otInstance * aInstance,const otExtAddress * aEui64,const char * aPskd,uint32_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerAddJoiner)(aInstance, aEui64, aPskd, aTimeout);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerAddJoinerWithDiscerner)(otInstance * aInstance,const otJoinerDiscerner * aDiscerner,const char * aPskd,uint32_t aTimeout)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerAddJoinerWithDiscerner)(aInstance, aDiscerner, aPskd, aTimeout);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerAnnounceBegin)(otInstance * aInstance,uint32_t aChannelMask,uint8_t aCount,uint16_t aPeriod,const otIp6Address * aAddress)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerAnnounceBegin)(aInstance, aChannelMask, aCount, aPeriod, aAddress);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerEnergyScan)(otInstance * aInstance,uint32_t aChannelMask,uint8_t aCount,uint16_t aPeriod,uint16_t aScanDuration,const otIp6Address * aAddress,otCommissionerEnergyReportCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerEnergyScan)(aInstance, aChannelMask, aCount, aPeriod, aScanDuration, aAddress, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerGetNextJoinerInfo)(otInstance * aInstance,uint16_t * aIterator,otJoinerInfo * aJoiner)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerGetNextJoinerInfo)(aInstance, aIterator, aJoiner);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerPanIdQuery)(otInstance * aInstance,uint16_t aPanId,uint32_t aChannelMask,const otIp6Address * aAddress,otCommissionerPanIdConflictCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerPanIdQuery)(aInstance, aPanId, aChannelMask, aAddress, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerRemoveJoiner)(otInstance * aInstance,const otExtAddress * aEui64)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerRemoveJoiner)(aInstance, aEui64);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerRemoveJoinerWithDiscerner)(otInstance * aInstance,const otJoinerDiscerner * aDiscerner)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerRemoveJoinerWithDiscerner)(aInstance, aDiscerner);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerSendMgmtGet)(otInstance * aInstance,const uint8_t * aTlvs,uint8_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerSendMgmtGet)(aInstance, aTlvs, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerSendMgmtSet)(otInstance * aInstance,const otCommissioningDataset * aDataset,const uint8_t * aTlvs,uint8_t aLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerSendMgmtSet)(aInstance, aDataset, aTlvs, aLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerSetId)(otInstance * aInstance,const char * aId)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerSetId)(aInstance, aId);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerSetProvisioningUrl)(otInstance * aInstance,const char * aProvisioningUrl)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerSetProvisioningUrl)(aInstance, aProvisioningUrl);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerStart)(otInstance * aInstance,otCommissionerStateCallback aStateCallback,otCommissionerJoinerCallback aJoinerCallback,void * aCallbackContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerStart)(aInstance, aStateCallback, aJoinerCallback, aCallbackContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otCommissionerStop)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCommissionerStop)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint16_t OT_API_WRAPPER_NAME(otCommissionerGetSessionId)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint16_t ret = OT_API_REAL_NAME(otCommissionerGetSessionId)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

