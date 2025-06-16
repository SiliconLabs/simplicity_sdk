/***************************************************************************//**
 * @file sl_token_manager_api.h
 * @brief Common token manager APIs.
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

/***************************************************************************//**
 * @addtogroup token_manager Token Manager
 * @{
 ******************************************************************************/

#ifndef SL_TOKEN_MANAGER_API_H
#define SL_TOKEN_MANAGER_API_H

#include "sl_common.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initialize the Token Manager.
 *
 * @note This function must be called before any other token manager
 * functions are called. The sl_token_manager_init() call will be automatically
 * added to your initialization.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_init(void);

/***************************************************************************//**
 * Read token data.
 *
 * @param[in]  token       A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                         on how to create a token.
 * @param[out] data        A pointer to where the token data should be placed.
 * @param[in]  length      The size of the data to be read, in bytes.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_get_data(uint32_t token,
                                      void *data,
                                      uint32_t length);

/***************************************************************************//**
 * Write token data.
 *
 * @note Writing static tokens on Silicon Labs devices has different constraints
 * depending on the series:
 *
 * Silicon Labs Series 2 (EFR) devices: Only static token values that have not
 * been written since the last erase can be written. For areas of flash that
 * cannot be erased by user code, those static tokens are effectively write-once.
 *
 * Silicon Labs Series 3 (SixG3xx) devices: While static device tokens
 * support overwriting, the write operation is limited to a maximum of 100 writes.
 * The static secure tokens are effectively write-once.
 *
 * @param[in] token    A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                     on how to create a token.
 * @param[in] data     A pointer to the data being written.
 * @param[in] length   The size of the token data in number of bytes.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_set_data(uint32_t token,
                                      void *data,
                                      uint32_t length);

/***************************************************************************//**
 * This call supports reading both dynamic and static tokens partially.
 *
 * @param[in]  token       A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                         on how to create a token.
 * @param[out] data        A pointer to where the token data should be placed.
 * @param[in]  offset      Indicates the number of bytes to skip from the beginning of the
 *                         token data before starting to read.
 * @param[in]  length      The size of the data (full or partial) to be read, in bytes.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_get_partial_data(uint32_t token,
                                              void *data,
                                              uint32_t offset,
                                              uint32_t length);

/***************************************************************************//**
 * This call supports deleting a dynamic token.
 *
 * @note This call also supports deleting static tokens, if they have been
 * overridden with NVM3 override tokens. Only the override tokens are deleted.
 *
 * @param[in] token   A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                    on how to create a token.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_delete_dynamic_token(uint32_t token);

/***************************************************************************//**
 * Increments the value of a counter object by 1. Counter tokens are stored in NVM3.
 *
 * @param[in] token  A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                   on how to create a token.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_increment_counter(uint32_t token);

/***************************************************************************//**
 * This call reads the token size.
 *
 * @note This call can be used to know the token size and user can allocate buffer
 * for holding the token data at runtime.
 *
 * @param[in]  token     A 32-bit token identifier. Refer sl_token_manager_defines.h
 *                       on how to create a token.
 * @param[out] size_out  output parameter to know the size of token.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
sl_status_t sl_token_manager_get_size(uint32_t token,
                                      uint32_t *size_out);

/** @} end token_manager */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/***************************************************************************//**
 * Internal assert failed function.
 *
 * @note Minimal assert functionality to support token manager asserts.  In
 * the future a fully formed assert component will become available and make
 * this assert code obsolete.
 *
 * @param filename
 * @param linenumber
 ******************************************************************************/
void halInternalAssertFailed(const char * filename, int linenumber);

#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(SL_TOKEN_MANAGER_BACKWARD_COMPATIBILITY) || defined(SL_TOKEN_MANAGER_BACKEND_INT_FLASH)
// Note: Below header is for S2 backward compatibility.
// The legacy hal API support for silicon labs EFR series 2 devices
// are planned for deprecation. However the new API's listed above
// will be supported. Plan to migrate to new common token manager API's.
#include "sl_token_manager.h"
#endif
#ifdef __cplusplus
}
#endif

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup common_token_manager Common Token Manager
/// @{
/// @brief Routines for working with tokens
///
///   @details
///
///
/// @n @section common_token_manager Introduction
///
/// The Common Token Manager provides a means to read and write manufacturing (static device and static secure) and
/// dynamic tokens such as manufacturing ID, channel number, transmit power, and various pieces of information
/// that the application needs to be persistent between device power cycles.
/// The token system is designed to abstract implementation details and simplify interacting with differing non-volatile systems.
/// The majority of tokens are stored in NVM3 where they can be rewritten.
/// By default the Common Token Manager pulls in the NVM3 component for storage.
///
/// All the Static Device Tokens, Static Secure Tokens shall be stored as KLV object (KLV header + data) format in respective memory region.
/// The static device tokens are stored in SE MTP region, which is inherently secure and rollback protected.
/// The static secure tokens are stored in data region of flash and uses 128-bit AES-GCM mode to secure the data.
/// The static secure region has 4k reserved space at the end of flash and is configurable. Any change
/// to static secure region space will have impact on NVM3 start and end address.
///
/// @} end token_manager ******************************************************/

#endif /* SL_TOKEN_MANAGER_API_H */
