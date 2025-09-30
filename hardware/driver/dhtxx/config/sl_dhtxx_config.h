/***************************************************************************/ /**
                                                                               * @file
                                                                               * @brief DHTXX Config
                                                                               *******************************************************************************
                                                                               * # License
                                                                               * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_DHTXX_CONFIG_H
#define SL_DHTXX_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> DHTXX Configuration

// <o SL_DHTXX_SENSOR_TYPE> Sensor Type
// <DHT11=> DHT11
// <DHT22=> DHT22
// <i> Select the DHT sensor type
// <i> Default: DHT22
#define SL_DHTXX_SENSOR_TYPE DHT22

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio> SL_DHTXX_DATA
// $[GPIO_SL_DHTXX_DATA]
#define SL_DHTXX_DATA_PORT SL_GPIO_PORT_C
#define SL_DHTXX_DATA_PIN 1
// [GPIO_SL_DHTXX_DATA]$
// <<< sl:end pin_tool >>>

// DHT sensor type definitions
#define DHT11 11
#define DHT22 22

#endif // SL_DHTXX_CONFIG_H
