/***************************************************************************//**
 * @file
 * @brief Display configuration header.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_DISPLAY_CONFIG_H
#define SL_DISPLAY_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Display configuration

// <o SL_DISPLAY_STACK_SIZE_WORD> Display render thread stack size in word
// <i> Default: 128
#define SL_DISPLAY_STACK_SIZE_WORD              128U

// <o SL_DISPLAY_MESSAGE_QUEUE_SIZE> Display render thread message size
// <i> Default: 16
#define SL_DISPLAY_MESSAGE_QUEUE_SIZE           16U

// </h>

// <<< end of configuration section >>>

#endif