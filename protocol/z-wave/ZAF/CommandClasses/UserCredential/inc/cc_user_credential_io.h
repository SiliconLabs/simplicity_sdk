/**
 * @file
 * User Credential Command Class I/O abstraction.
 *
 * @copyright 2023 Silicon Laboratories Inc.
 */

#ifndef CC_USER_CREDENTIAL_IO_H
#define CC_USER_CREDENTIAL_IO_H

#include "CC_UserCredential.h"

/**
 * @addtogroup CC
 * @{
 * @addtogroup UserCredential
 * @{
 */

/****************************************************************************/
/*                      EXPORTED TYPES and DEFINITIONS                      */
/****************************************************************************/

typedef enum u3c_db_operation_result_ {
  U3C_DB_OPERATION_RESULT_SUCCESS, // The operation completed succesfully
  U3C_DB_OPERATION_RESULT_ERROR, // An error occurred
  U3C_DB_OPERATION_RESULT_ERROR_IO, // An I/O error occurred
  U3C_DB_OPERATION_RESULT_FAIL_DNE, // The object does not exist
  U3C_DB_OPERATION_RESULT_FAIL_FULL, // There is no space left for the object
  U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED // The object already exists
} u3c_db_operation_result;

/****************************************************************************/
/*                           GENERAL API FUNCTIONS                          */
/****************************************************************************/

void CC_UserCredential_factory_reset(void);

void CC_UserCredential_init_database(void);

/****************************************************************************/
/*                         USER RELATED API FUNCTIONS                       */
/****************************************************************************/

/**
 * Copies the data for a given User from the database to an arbitrary location
 * in memory.
 *
 * @param[in] unique_identifier Unique Identifier of the requested User
 * @param[out] user The memory location where the User data will be copied,
 *                  or NULL if not requested
 * @param[out] name The memory location where the User name will be copied,
 *                  or NULL if not requested
 * @return the result of the operation
 */
u3c_db_operation_result CC_UserCredential_get_user(
  uint16_t unique_identifier,
  u3c_user * user,
  uint8_t * name
  );

/**
 * Finds the first or next available User Unique Identifier.
 *
 * @param[in] unique_identifier Unique Identifier of the current User, or
 * 0 if the first available User Unique Identifier is requested
 * @return The Unique Identifier of the next User if available, otherwise 0
 */
uint16_t CC_UserCredential_get_next_user(uint16_t unique_identifier);

u3c_db_operation_result CC_UserCredential_add_user(
  u3c_user * user,
  uint8_t * name
  );

u3c_db_operation_result CC_UserCredential_modify_user(
  u3c_user * user,
  uint8_t * name
  );

u3c_db_operation_result CC_UserCredential_delete_user(
  uint16_t user_unique_identifier
  );

/****************************************************************************/
/*                      CREDENTIAL RELATED API FUNCTIONS                    */
/****************************************************************************/

/**
 * Copies the data for a given Credential from the database to an arbitrary
 * location in memory.
 *
 * @param[in] user_unique_identifier Unique Identifier of the Credential's
 *                                   associated User (0 matches any UUID)
 * @param[in] credential_type Type of the requested Credential
 * @param[in] credential_slot Slot of the requested Credential
 * @param[out] credential_metadata The memory location where the Credential
 *                                 metadata will be copied, or NULL if not
 *                                 requested
 * @param[out] credential_data The memory location where the Credential data
 *                             will be copied, or NULL if not requested
 * @return true if the Credential object was copied succesfully
 */
u3c_db_operation_result CC_UserCredential_get_credential(
  uint16_t user_unique_identifier,
  u3c_credential_type credential_type,
  uint16_t credential_slot,
  u3c_credential_metadata * credential_metadata,
  uint8_t * credential_data
  );

/**
 * Finds the first or next available Credential entry for a given User.
 * Set both credential_type and credential_slot to 0 to get the first available
 * Credential for a User.
 *
 * @param[in] user_unique_identifier Unique Identifier of the Credential's
 *                                   associated User (0 matches any UUID)
 * @param[in] credential_type Type of the current Credential
 * @param[in] credential_slot Slot of the current Credential
 * @param[out] next_credential_type Type of the next available Credential, if
 *                                  available
 * @param[out] next_credential_slot Slot of the next available Credential, if
 *                                  available
 * @return true if a subsequent Credential matching the criteria is found
 */
bool CC_UserCredential_get_next_credential(
  uint16_t user_unique_identifier,
  u3c_credential_type credential_type,
  uint16_t credential_slot,
  u3c_credential_type * next_credential_type,
  uint16_t * next_credential_slot
  );

u3c_db_operation_result CC_UserCredential_add_credential(
  u3c_credential * credential
  );

u3c_db_operation_result CC_UserCredential_modify_credential(
  u3c_credential * credential
  );

/**
 * Deletes a Credential.
 *
 * @param[in] user_unique_identifier Unique Identifier of the Credential's
 *                                   associated User (0 matches any UUID)
 * @param[in] credential_type Type of the current Credential
 * @param[in] credential_slot Slot of the current Credential
 * @return the result of the operation
 */
u3c_db_operation_result CC_UserCredential_delete_credential(
  uint16_t user_unique_identifier,
  u3c_credential_type credential_type,
  uint16_t credential_slot
  );

u3c_db_operation_result CC_UserCredential_move_credential(
  uint16_t source_user_uid,
  u3c_credential_type credential_type,
  uint16_t source_credential_slot,
  uint16_t destination_user_uid,
  uint16_t destination_credential_slot
  );

/**
 * @}
 * @}
 */

#endif /* CC_USER_CREDENTIAL_IO_H */
