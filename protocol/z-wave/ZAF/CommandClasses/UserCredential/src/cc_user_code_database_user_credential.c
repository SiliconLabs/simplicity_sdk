/**
 * @file
 * @brief Wrapper around the User Credential database for CC User Code
 * @details Replaces the database implementation of the User Code Command Class
 * with a wrapper accessing the PIN Codes stored by the User Credential Command
 * Class.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "CC_UserCode.h"
#include "cc_user_code_io.h"
#include "cc_user_credential_io.h"
#include "cc_user_credential_config_api.h"
#include "cc_user_credential_io_config.h"
#include <string.h>
#include "assert.h"

/****************************************************************************/
/*                           FORWARD DECLARATIONS                           */
/****************************************************************************/

extern void delete_all_credentials_of_type(
  uint16_t uuid, u3c_credential_type filter_type);

extern bool find_existing_credential(
  const u3c_credential * const p_credential,
  u3c_credential_metadata * p_existing_metadata);

extern void set_default_name(uint8_t * pName, u3c_user * pUser);

extern bool validate_new_credential_metadata(
  const u3c_credential_metadata * const p_metadata);

extern bool validate_new_credential_data(
  u3c_credential * p_credential, RECEIVE_OPTIONS_TYPE_EX * p_rx_options);

/****************************************************************************/
/*                                CONSTANTS                                 */
/****************************************************************************/

#define USER_CODE_REPORT_DATA_MAX_LENGTH \
  TX_DATA_MAX_DATA_SIZE - offsetof(ZW_USER_CODE_REPORT_1BYTE_FRAME, userCode1)

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static bool is_valid_user_identifier(uint8_t user_identifier)
{
  bool is_valid = user_identifier
                  <= cc_user_credential_get_max_credential_slots(
    CREDENTIAL_TYPE_PIN_CODE);
  return is_valid;
}

/**
 * Add or modify a User Code
 *
 * @return true if the operation was successful
 */
static bool set_user_code(
  uint8_t user_identifier,
  uint8_t * pUserCode,
  uint8_t len,
  uint16_t modifier_node_id,
  bool credential_found)
{
  // Ensure that the length of the data is correct
  if (len < cc_user_credential_get_min_length_of_data(
        CREDENTIAL_TYPE_PIN_CODE)
      || len > cc_user_credential_get_max_length_of_data(
        CREDENTIAL_TYPE_PIN_CODE)) {
    return false;
  }

  bool user_found = credential_found;
  u3c_user existing_user = { 0 };
  user_found = CC_UserCredential_get_user(user_identifier, &existing_user, NULL)
               == U3C_DB_OPERATION_RESULT_SUCCESS;
  if (!credential_found && !user_found) {
    // Add a new User with the same UUID as the User Identifier
    uint8_t name[U3C_BUFFER_SIZE_USER_NAME];
    u3c_user user = {
      .unique_identifier = user_identifier,
      .type = USER_TYPE_GENERAL,
      .modifier_type = MODIFIER_TYPE_Z_WAVE,
      .modifier_node_id = modifier_node_id,
      .credential_rule = CREDENTIAL_RULE_SINGLE,
      .active = true,
      .name_encoding = USER_NAME_ENCODING_STANDARD_ASCII,
      .name_length = 0
    };
    set_default_name(name, &user);
    if (CC_UserCredential_add_user(&user, name)
        != U3C_DB_OPERATION_RESULT_SUCCESS) {
      return false;
    }
  }

  u3c_credential credential = {
    .data = pUserCode,
    .metadata = {
      .length = len,
      .modifier_type = MODIFIER_TYPE_Z_WAVE,
      .modifier_node_id = modifier_node_id,
      .uuid = user_found ? existing_user.unique_identifier : user_identifier,
      .type = CREDENTIAL_TYPE_PIN_CODE,
      .slot = user_identifier
    }
  };

  if (!validate_new_credential_metadata(&credential.metadata)
      || !validate_new_credential_data(&credential, NULL)) {
    return false;
  }

  u3c_db_operation_result credential_set_result =
    credential_found
    ? CC_UserCredential_modify_credential(&credential)
    : CC_UserCredential_add_credential(&credential);

  return (credential_set_result == U3C_DB_OPERATION_RESULT_SUCCESS);
}

/****************************************************************************/
/*                             PUBLIC FUNCTIONS                             */
/****************************************************************************/

uint8_t CC_UserCode_UsersNumberReport_handler(
  __attribute__((unused)) uint8_t endpoint)
{
  uint16_t max_slots =
    cc_user_credential_get_max_credential_slots(CREDENTIAL_TYPE_PIN_CODE);
  return (uint8_t)(max_slots > UINT8_MAX ? UINT8_MAX : max_slots);
}

bool CC_UserCode_getId_handler(
  uint8_t user_identifier,
  USER_ID_STATUS* pUserIdStatus,
  __attribute__((unused)) uint8_t endpoint)
{
  if (!pUserIdStatus) {
    assert(false);
    return false;
  }

  if (!is_valid_user_identifier(user_identifier)) {
    return false;
  }

  *pUserIdStatus = USER_ID_NO_STATUS;
  switch (CC_UserCredential_get_credential(
            0, CREDENTIAL_TYPE_PIN_CODE, user_identifier, NULL, NULL)
          ) {
    case U3C_DB_OPERATION_RESULT_SUCCESS:
      *pUserIdStatus = USER_ID_OCCUPIED;
      break;
    case U3C_DB_OPERATION_RESULT_FAIL_DNE:
      *pUserIdStatus = USER_ID_AVAILABLE;
      break;
    default:
      return false;
  }

  return true;
}

bool CC_UserCode_Report_handler(
  uint8_t user_identifier,
  uint8_t* pUserCode,
  size_t * pLen,
  __attribute__((unused)) uint8_t endpoint)
{
  if (!pUserCode || !pLen) {
    assert(false);
    return false;
  }

  if (!is_valid_user_identifier(user_identifier)) {
    return false;
  }

  u3c_credential_metadata metadata = { 0 };
  uint8_t credential_data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };
  u3c_db_operation_result result_credential_get =
    CC_UserCredential_get_credential(
      0, CREDENTIAL_TYPE_PIN_CODE, user_identifier, &metadata, credential_data);
  if (!(result_credential_get == U3C_DB_OPERATION_RESULT_SUCCESS
        || result_credential_get == U3C_DB_OPERATION_RESULT_FAIL_DNE)) {
    return false;
  }

  /**
   * If the constant length fields of the report + the PIN Code data do not fit
   * in the outgoing frame abort.
   */
  if (metadata.length > USER_CODE_REPORT_DATA_MAX_LENGTH) {
    return false;
  }

  *pLen = metadata.length;
  memcpy(pUserCode, credential_data, metadata.length);
  return true;
}

e_cmd_handler_return_code_t CC_UserCode_Set_handler(
  uint8_t user_identifier,
  USER_ID_STATUS user_id_status,
  uint8_t* pUserCode,
  uint8_t len,
  uint16_t modifier_node_id)
{
  if (!pUserCode) {
    assert(false);
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }

  if (!is_valid_user_identifier(user_identifier)
      || user_id_status == USER_ID_RESERVED
      || user_id_status == USER_ID_NO_STATUS) {
    return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }

  e_cmd_handler_return_code_t status = E_CMD_HANDLER_RETURN_CODE_FAIL;

  // Bulk deletion
  if (user_identifier == 0) {
    if (user_id_status == USER_ID_AVAILABLE) {
      delete_all_credentials_of_type(0, CREDENTIAL_TYPE_PIN_CODE);
      status = E_CMD_HANDLER_RETURN_CODE_HANDLED;
    }
    return status;
  }

  // Single operation
  bool credential_found = false;
  switch (CC_UserCredential_get_credential(
            0, CREDENTIAL_TYPE_PIN_CODE, user_identifier, NULL, NULL)
          ) {
    case U3C_DB_OPERATION_RESULT_SUCCESS:
      credential_found = true;
      break;
    case U3C_DB_OPERATION_RESULT_FAIL_DNE:
      credential_found = false;
      break;
    default:
      // I/O error
      return E_CMD_HANDLER_RETURN_CODE_FAIL;
  }

  if (user_id_status == USER_ID_AVAILABLE) {
    // Delete User Code
    if (credential_found
        && (CC_UserCredential_delete_credential(
              0, CREDENTIAL_TYPE_PIN_CODE, user_identifier)
            == U3C_DB_OPERATION_RESULT_SUCCESS)) {
      status = E_CMD_HANDLER_RETURN_CODE_HANDLED;
    }
  } else {
    if (set_user_code(
          user_identifier, pUserCode, len, modifier_node_id, credential_found)
        ) {
      status = E_CMD_HANDLER_RETURN_CODE_HANDLED;
    }
  }
  return status;
}

void CC_UserCode_reset_data(void)
{
  // The database is handled by the User Credential Command Class.
}

bool CC_UserCode_Validate(uint8_t identifier, const uint8_t *pCode, uint8_t len)
{
  uint8_t data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };
  memcpy(&data, pCode, len);
  const u3c_credential credential = {
    .data = data,
    .metadata = {
      .length = len,
      .modifier_type = MODIFIER_TYPE_UNKNOWN,
      .modifier_node_id = 0,
      .uuid = 0,
      .type = CREDENTIAL_TYPE_PIN_CODE,
      .slot = identifier
    }
  };
  u3c_credential_metadata existing_metadata = { 0 };
  return find_existing_credential(&credential, &existing_metadata)
         && (existing_metadata.slot == identifier);
}
