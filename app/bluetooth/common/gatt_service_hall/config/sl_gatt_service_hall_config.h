/***************************************************************************//**
 * @file
 * @brief Hall Effect GATT Service Configuration
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

#ifndef SL_GATT_SERVICE_HALL_CONFIG_H
#define SL_GATT_SERVICE_HALL_CONFIG_H

/***********************************************************************************************//**
 * @addtogroup gatt_service_hall
 * @{
 **************************************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_GATT_SERVICE_HALL_FIELD_STRENGTH_INVALID> Dummy field strength measurement results for uninitialized sensors. <0-0x7FFFFFFF>
// <i> Default: 0x7FFFFFFF
#define SL_GATT_SERVICE_HALL_FIELD_STRENGTH_INVALID  0x7FFFFFFF

// <o SL_GATT_SERVICE_HALL_ALERT_INVALID> Dummy alert for uninitialized sensors.
// <true=> True
// <false=> False
#define SL_GATT_SERVICE_HALL_ALERT_INVALID  false

// <o SL_GATT_SERVICE_HALL_TAMPER_INVALID> Dummy tamper indication for uninitialized sensors.
// <true=> True
// <false=> False
#define SL_GATT_SERVICE_HALL_TAMPER_INVALID  true
// <<< end of configuration section >>>

/** @} (end addtogroup gatt_service_hall) */
#endif // SL_GATT_SERVICE_HALL_CONFIG_H
