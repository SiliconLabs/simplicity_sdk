/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_BUTTON_PRESS_CONFIG_H
#define APP_BUTTON_PRESS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Threshold values

// <o SHORT_BUTTON_PRESS_DURATION> Duration of Short button presses
// <i> Default: 250
// <i> Any button press shorter than this value will be considered SHORT_BUTTON_PRESS.
#define SHORT_BUTTON_PRESS_DURATION   (250)

// <o MEDIUM_BUTTON_PRESS_DURATION> Duration of MEDIUM button presses
// <i> Default: 1000
// <i> Any button press shorter than this value and longer SHORT_BUTTON_PRESS than will
// be considered MEDIUM_BUTTON_PRESS.
#define MEDIUM_BUTTON_PRESS_DURATION   (1000)

// <o LONG_BUTTON_PRESS_DURATION> Duration of LONG button presses
// <i> Default: 5000
// <i> Any button press shorter than this value and longer MEDIUM_BUTTON_PRESS than will
// be consideredLONG_BUTTON_PRESS. Any button press longer than this value will be
// considered VERY_LONG_BUTTON_PRESS
#define LONG_BUTTON_PRESS_DURATION   (5000)

// </h>

// <h> Runtime Adaptation

// <o APP_BUTTON_PRESS_CONFIG_PRIORITY> Runtime context priority
// <APP_RTA_PRIORITY_LOW=> Low
// <APP_RTA_PRIORITY_BELOW_NORMAL=> Below normal
// <APP_RTA_PRIORITY_NORMAL=> Normal
// <APP_RTA_PRIORITY_ABOVE_NORMAL=> Above normal
// <APP_RTA_PRIORITY_HIGH=> High
// <i> Default: Normal
// <i> Only takes effect for RTOS.
#define APP_BUTTON_PRESS_CONFIG_PRIORITY           APP_RTA_PRIORITY_NORMAL

// <o APP_BUTTON_PRESS_CONFIG_STACK_SIZE> Stack size (in bytes)
// <i> Default: 1024
// <i> Only takes effect for RTOS.
#define APP_BUTTON_PRESS_CONFIG_STACK_SIZE         (1024)

// <o APP_BUTTON_PRESS_CONFIG_WAIT_FOR_GUARD> Timeout for guard (in ticks)
// <i> Default: 5
// <i> Only takes effect for RTOS.
#define APP_BUTTON_PRESS_CONFIG_WAIT_FOR_GUARD     (5)

// <o APP_BUTTON_PRESS_CONFIG_QUEUE_SIZE> Queue size
// <i> Default: 2
// <i> Queue size for all (bare-metal and RTOS) adaptations.
#define APP_BUTTON_PRESS_CONFIG_QUEUE_SIZE         (2)

// </h>

// <<< end of configuration section >>>

#endif // APP_BUTTON_PRESS_CONFIG_H
