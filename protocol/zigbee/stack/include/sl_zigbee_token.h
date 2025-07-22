/***************************************************************************//**
 * @brief ZigBee token management code.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_ZIGBEE_TOKEN_H
#define SL_ZIGBEE_TOKEN_H

#include "stack/include/sl_zigbee_types.h"

/**
 * @brief Get the total count of the tokens configured.
 * @return The token count.
 *
 * Use this to get the number of tokens configured in a node.
 */
uint32_t sl_zigbee_get_token_count(void);

#ifndef ZNET_HEADER_SCRIPT
/**
 * @brief Get the default value of the given token.
 * @param token The NVM3 key of the token, the base if an index token.
 * @param default_token_value Pointer to memory to copy value into.
 * @return status.
 *
 * Use this to get the number of tokens configured in a node.
 */
sl_status_t sl_zigbee_get_token_default(uint32_t token,
                                        uint8_t *default_token_value);

/**
 * @brief Initialize a basic, non-counter, non-index token
 * (Common Token Manager Component only).
 * @param token The KLV of the token.
 * @param default_token_value Pointer to the token's default value.
 * @param token_size Size of the token being initialized.
 * @return status.
 *
 * Use this to get the number of tokens configured in a node.
 */
sl_status_t sl_zigbee_initialize_basic_token(uint32_t token,
                                             void *default_token_value,
                                             uint32_t token_size);

/**
 * @brief Initialize a counter token.
 * (Common Token Manager Component only).
 * @param token The KLV of the token.
 * @param default_token_value Pointer to the token's default value.
 * @param token_size Size of the token being initialized.
 * @return status.
 *
 * Use this to get the number of tokens configured in a node.
 */
sl_status_t sl_zigbee_initialize_counter_token(uint32_t token,
                                               void *default_token_value,
                                               uint32_t token_size);

/**
 * @brief Initialize an index token.
 * (Common Token Manager Component only).
 * @param token The KLV of the token.
 * @param default_token_value Pointer to the token's default value.
 * @param token_size Size of the token being initialized.
 * @param token_index_size Size of the array of tokens.
 * @return status.
 *
 * Use this to get the number of tokens configured in a node.
 */
sl_status_t sl_zigbee_initialize_index_token(uint32_t token_base,
                                             void *default_token_value,
                                             uint32_t token_size,
                                             uint8_t token_index_size);

#endif // ZNET_HEADER_SCRIPT

#endif // SL_ZIGBEE_TOKEN_H
