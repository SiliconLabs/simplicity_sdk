/***************************************************************************//**
 * @brief ZigBee Application Framework token management code.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee.h"

//------------------------------------------------------------------------------
// API Stubs
// Weak implementation to return the library not available incase the token
// manager is not present.

WEAK(uint8_t sli_zigbee_stack_get_token_count(void))
{
  return 0;
}

WEAK(sl_status_t sli_zigbee_stack_get_token_info(uint8_t index,
                                                 sl_zigbee_token_info_t *tokenInfo))
{
  (void) index;
  (void) tokenInfo;
  return SL_STATUS_NOT_AVAILABLE;
}

WEAK(sl_status_t sli_zigbee_stack_get_token_data(uint32_t token,
                                                 uint32_t index,
                                                 sl_zigbee_token_data_t *tokenData))
{
  (void) token;
  (void) index;
  (void) tokenData;
  return SL_STATUS_NOT_AVAILABLE;
}

WEAK(sl_status_t sli_zigbee_stack_set_token_data(uint32_t token,
                                                 uint32_t index,
                                                 sl_zigbee_token_data_t *tokenData))
{
  (void) token;
  (void) index;
  (void) tokenData;
  return SL_STATUS_NOT_AVAILABLE;
}
WEAK(void sli_zigbee_stack_token_factory_reset(bool exclude_outgoing_fc, bool exclude_boot_counter))
{
  (void)exclude_outgoing_fc;
  (void)exclude_boot_counter;
}
