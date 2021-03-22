/***************************************************************************//**
 * @file
 * @brief  OpenThread wrapper functions for OpenThread Cli APIs
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

#include <openthread/cli.h>
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

extern otError OT_API_REAL_NAME(otCliSetUserCommands)(const otCliCommand * aUserCommands,uint8_t aLength,void * aContext);
extern void OT_API_REAL_NAME(otCliInit)(otInstance * aInstance,otCliOutputCallback aCallback,void * aContext);
extern void OT_API_REAL_NAME(otCliInputLine)(char * aBuf);
extern int OT_API_REAL_NAME(CliUartOutput)(void *aContext, const char *aFormat, va_list aArguments);

otError OT_API_WRAPPER_NAME(otCliSetUserCommands)(const otCliCommand * aUserCommands,uint8_t aLength,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    otError ret = OT_API_REAL_NAME(otCliSetUserCommands)(aUserCommands, aLength, aContext);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}

void OT_API_WRAPPER_NAME(otCliInit)(otInstance * aInstance,otCliOutputCallback aCallback,void * aContext)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCliInit)(aInstance, aCallback, aContext);
    sl_ot_rtos_release_stack_mutex();
}

void OT_API_WRAPPER_NAME(otCliInputLine)(char * aBuf)
{
    sl_ot_rtos_acquire_stack_mutex();
    OT_API_REAL_NAME(otCliInputLine)(aBuf);
    sl_ot_rtos_release_stack_mutex();
}

int OT_API_WRAPPER_NAME(CliUartOutput)(void *aContext, const char *aFormat, va_list aArguments)
{
    sl_ot_rtos_acquire_stack_mutex();
    int ret = OT_API_REAL_NAME(CliUartOutput)(aContext, aFormat, aArguments);
    sl_ot_rtos_release_stack_mutex();
    return ret;
}