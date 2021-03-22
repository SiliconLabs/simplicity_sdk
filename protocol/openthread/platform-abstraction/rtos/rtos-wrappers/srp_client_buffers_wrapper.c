/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Srp Client Buffers APIs
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

#include <openthread/srp_client_buffers.h>
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

extern char *OT_API_REAL_NAME(otSrpClientBuffersGetHostNameString)(otInstance * aInstance,uint16_t * aSize);
extern char *OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryInstanceNameString)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize);
extern char *OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryServiceNameString)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize);
extern const char **OT_API_REAL_NAME(otSrpClientBuffersGetSubTypeLabelsArray)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aArrayLength);
extern otIp6Address *OT_API_REAL_NAME(otSrpClientBuffersGetHostAddressesArray)(otInstance * aInstance,uint8_t * aArrayLength);
extern otSrpClientBuffersServiceEntry *OT_API_REAL_NAME(otSrpClientBuffersAllocateService)(otInstance * aInstance);
extern uint8_t *OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryTxtBuffer)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize);
extern void OT_API_REAL_NAME(otSrpClientBuffersFreeAllServices)(otInstance * aInstance);
extern void OT_API_REAL_NAME(otSrpClientBuffersFreeService)(otInstance * aInstance,otSrpClientBuffersServiceEntry * aService);

char *OT_API_WRAPPER_NAME(otSrpClientBuffersGetHostNameString)(otInstance * aInstance,uint16_t * aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    char *ret = OT_API_REAL_NAME(otSrpClientBuffersGetHostNameString)(aInstance, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

char *OT_API_WRAPPER_NAME(otSrpClientBuffersGetServiceEntryInstanceNameString)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    char *ret = OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryInstanceNameString)(aEntry, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

char *OT_API_WRAPPER_NAME(otSrpClientBuffersGetServiceEntryServiceNameString)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    char *ret = OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryServiceNameString)(aEntry, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

const char **OT_API_WRAPPER_NAME(otSrpClientBuffersGetSubTypeLabelsArray)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aArrayLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    const char **ret = OT_API_REAL_NAME(otSrpClientBuffersGetSubTypeLabelsArray)(aEntry, aArrayLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otIp6Address *OT_API_WRAPPER_NAME(otSrpClientBuffersGetHostAddressesArray)(otInstance * aInstance,uint8_t * aArrayLength)
{
    sl_ot_rtos_acquire_stack_mutex();
    otIp6Address *ret = OT_API_REAL_NAME(otSrpClientBuffersGetHostAddressesArray)(aInstance, aArrayLength);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otSrpClientBuffersServiceEntry *OT_API_WRAPPER_NAME(otSrpClientBuffersAllocateService)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    otSrpClientBuffersServiceEntry *ret = OT_API_REAL_NAME(otSrpClientBuffersAllocateService)(aInstance);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

uint8_t *OT_API_WRAPPER_NAME(otSrpClientBuffersGetServiceEntryTxtBuffer)(otSrpClientBuffersServiceEntry * aEntry,uint16_t * aSize)
{
    sl_ot_rtos_acquire_stack_mutex();
    uint8_t *ret = OT_API_REAL_NAME(otSrpClientBuffersGetServiceEntryTxtBuffer)(aEntry, aSize);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otSrpClientBuffersFreeAllServices)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientBuffersFreeAllServices)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otSrpClientBuffersFreeService)(otInstance * aInstance,otSrpClientBuffersServiceEntry * aService)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otSrpClientBuffersFreeService)(aInstance, aService);
    sl_ot_rtos_release_stack_mutex();
}

