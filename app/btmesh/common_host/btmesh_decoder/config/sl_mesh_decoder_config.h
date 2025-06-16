/***************************************************************************//**
 * @file sl_mesh_decoder_config.h
 * @brief Bluetooth Mesh message decoder library configuration
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

#ifndef SL_MESH_DECODER_CONFIG_H
#define SL_MESH_DECODER_CONFIG_H

/**
 * Whether to handle friendship messaging or not
 */
#define DECODE_FRIENDSHIP (1)

/**
 * Whether to handle directed forwarding or not
 */
#define DECODE_DIRECTED_FORWARDING (1)

/**
 * Maximum number of cryptographic keys (network keys, application
 * keys, device keys, and friendship key material) that can be in use
 * at any given time. The more keys are configured, the higher the
 * runtime memory requirements.
 *
 * Note that the PSA cryptographic library can limit the number of
 * keys that exist concurrently, and there are multiple key
 * derivations made for every network key and ongoing friendship.
 * Moreover, there are multiple key derivations made for ongoing
 * friendships.
 */
#define MAX_KEYS (16)

/**
 * Maximum number of segmented message reassemblies that can be
 * tracked in parallel. The more ressemblers are configured, the
 * higher the runtime memory requirements.
 */
#define MAX_REASSEMBLER_COUNT (16)

#if DECODE_FRIENDSHIP

/**
 * Maximum number of pending friendship establishments that can be
 * tracked in parallel. The more pending friendships are configured,
 * the higher the runtime memory requirements.
 */
#define MAX_INCOMPLETE_FRIENDSHIP_COUNT (16)

#endif

/**
 * Maximum number of Label UUIDs for virtual addresses. The more
 * lavels are configured, the higher the runtime memory requirements.
 */
#define MAX_VIRTUAL_LABELS (16)

/**
 * Maximum number of sensor data entries that can be stored during decoding.
 */
#define SL_MESH_SENSOR_ARRAY_SIZE (10)

#endif // SL_MESH_DECODER_CONFIG_H
