/***************************************************************************//**
 * @file
 * @brief CLI for the Network Creator Security plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "network-creator-security.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "network-creator-security-config.h"
#include "stack/include/zigbee-security-manager.h"

// plugin network_creator_security [open|close]
void sli_zigbee_af_network_creator_security_open_or_close_network_command(sl_cli_command_arg_t *arguments)
{
  sl_status_t status;
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  bool open = memcmp(sl_cli_get_command_string(arguments, position), "open", strlen("open")) == 0;

  status = (open
            ? sl_zigbee_af_network_creator_security_open_network()
            : sl_zigbee_af_network_creator_security_close_network());

  sl_zigbee_core_debug_println("%s: %s network: 0x%02X",
                               SL_ZIGBEE_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                               (open ? "Open" : "Close"),
                               status);
}

extern sl_zigbee_key_data_t distributedKey;

void sli_zigbee_af_network_creator_security_set_joining_link_key_command(sl_cli_command_arg_t *arguments)
{
  sl_802154_long_addr_t eui64;
  sl_zigbee_sec_man_key_t keyData;
  sl_status_t status;
  size_t len = 16;
  sl_zigbee_copy_eui64_arg(arguments, 0, eui64, true);
  //emberCopyKeyArgument(1, &keyData);
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 1, &len);
  memset(keyData.key, 0, SL_ZIGBEE_ENCRYPTION_KEY_SIZE);
  memmove(keyData.key, ptr_string, SL_ZIGBEE_ENCRYPTION_KEY_SIZE); // Is the padding correct?

  status = sl_zigbee_sec_man_import_transient_key(eui64,
                                                  &keyData);

  sl_zigbee_af_core_println("%s: %s: 0x%X",
                            SL_ZIGBEE_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                            "Set joining link key",
                            status);
}

void sli_zigbee_af_network_creator_security_clear_joining_link_key_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_clear_transient_link_keys();

  sl_zigbee_af_core_println("%s: %s: 0x%X",
                            SL_ZIGBEE_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                            "Clear joining link keys",
                            SL_STATUS_OK);
}

void sli_zigbee_af_network_creator_security_open_network_with_key_command(sl_cli_command_arg_t *arguments)
{
  sl_802154_long_addr_t eui64;
  sl_zigbee_key_data_t keyData;
  sl_status_t status;
  size_t len = 16;
  sl_zigbee_copy_eui64_arg(arguments, 0, eui64, true);
  //emberCopyKeyArgument(1, &keyData);
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 1, &len);
  memset(keyData.contents, 0, SL_ZIGBEE_ENCRYPTION_KEY_SIZE);
  memmove(keyData.contents, ptr_string, SL_ZIGBEE_ENCRYPTION_KEY_SIZE);  // Is the padding correct?

  status = sl_zigbee_af_network_creator_security_open_network_with_key_pair(eui64, keyData);

  sl_zigbee_af_core_println("%s: Open network: 0x%X",
                            SL_ZIGBEE_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                            status);
}

void sli_zigbee_af_network_creator_security_configure_distributed_key(sl_cli_command_arg_t *arguments)
{
  size_t len = 16;
  uint8_t *ptr_string = sl_cli_get_argument_hex(arguments, 0, &len);
  memset(distributedKey.contents, 0, SL_ZIGBEE_ENCRYPTION_KEY_SIZE);
  memmove(distributedKey.contents, ptr_string, SL_ZIGBEE_ENCRYPTION_KEY_SIZE); // Is the padding correct?
}
