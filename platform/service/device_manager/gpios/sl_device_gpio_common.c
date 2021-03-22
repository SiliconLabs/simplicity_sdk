/**************************************************************************//**
 * @file
 * @brief Device Manager Clock Definition
 ******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#include "em_device.h"
#include "sl_device_gpio.h"

/***************************************************************************//**
 * @addtogroup device_gpio Device Manager GPIO
 * @{
 ******************************************************************************/

const uint32_t gpio_port_a_pin_mask = GPIO_PA_MASK;
const uint32_t gpio_port_b_pin_mask = GPIO_PB_MASK;
const uint32_t gpio_port_c_pin_mask = GPIO_PC_MASK;
const uint32_t gpio_port_d_pin_mask = GPIO_PD_MASK;

const uint32_t gpio_port_a_pin_count = GPIO_PA_COUNT;
const uint32_t gpio_port_b_pin_count = GPIO_PB_COUNT;
const uint32_t gpio_port_c_pin_count = GPIO_PC_COUNT;
const uint32_t gpio_port_d_pin_count = GPIO_PD_COUNT;

/** @} (end addtogroup device_gpio) */
