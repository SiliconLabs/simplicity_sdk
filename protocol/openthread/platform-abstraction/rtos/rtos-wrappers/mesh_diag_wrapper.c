/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Mesh Diag APIs
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

#include <openthread/mesh_diag.h>
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

extern otError OT_API_REAL_NAME(otMeshDiagDiscoverTopology)(otInstance * aInstance,const otMeshDiagDiscoverConfig * aConfig,otMeshDiagDiscoverCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otMeshDiagGetNextChildInfo)(otMeshDiagChildIterator * aIterator,otMeshDiagChildInfo * aChildInfo);
extern otError OT_API_REAL_NAME(otMeshDiagGetNextIp6Address)(otMeshDiagIp6AddrIterator * aIterator,otIp6Address * aIp6Address);
extern otError OT_API_REAL_NAME(otMeshDiagQueryChildTable)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagQueryChildTableCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otMeshDiagQueryChildrenIp6Addrs)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagChildIp6AddrsCallback aCallback,void * aContext);
extern otError OT_API_REAL_NAME(otMeshDiagQueryRouterNeighborTable)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagQueryRouterNeighborTableCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otMeshDiagCancel)(otInstance * aInstance);

otError OT_API_WRAPPER_NAME(otMeshDiagDiscoverTopology)(otInstance * aInstance,const otMeshDiagDiscoverConfig * aConfig,otMeshDiagDiscoverCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagDiscoverTopology)(aInstance, aConfig, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMeshDiagGetNextChildInfo)(otMeshDiagChildIterator * aIterator,otMeshDiagChildInfo * aChildInfo)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagGetNextChildInfo)(aIterator, aChildInfo);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMeshDiagGetNextIp6Address)(otMeshDiagIp6AddrIterator * aIterator,otIp6Address * aIp6Address)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagGetNextIp6Address)(aIterator, aIp6Address);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMeshDiagQueryChildTable)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagQueryChildTableCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagQueryChildTable)(aInstance, aRloc16, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMeshDiagQueryChildrenIp6Addrs)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagChildIp6AddrsCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagQueryChildrenIp6Addrs)(aInstance, aRloc16, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

otError OT_API_WRAPPER_NAME(otMeshDiagQueryRouterNeighborTable)(otInstance * aInstance,uint16_t aRloc16,otMeshDiagQueryRouterNeighborTableCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otMeshDiagQueryRouterNeighborTable)(aInstance, aRloc16, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otMeshDiagCancel)(otInstance * aInstance)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otMeshDiagCancel)(aInstance);
    sl_ot_rtos_release_stack_mutex();
}

