/**
 * @file
 * @brief Non-volatile memory implementation for Command Class User Credential
 *        I/O
 *
 * @copyright 2023 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "cc_user_credential_io.h"
#include "cc_user_credential_io_config.h"
#include "ZAF_file_ids.h"
#include "ZAF_nvm.h"
#include "cc_user_credential_config_api.h"
#include "assert.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/****************************************************************************/
/*                          CONSTANTS and TYPEDEFS                          */
/****************************************************************************/

// Maximum number of User and User Name objects that can be stored in the NVM
#define MAX_USER_OBJECTS                   \
  ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_LAST \
  - ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_BASE
/**
 * Maximum number of Credential and Credential Data objects that can be stored
 * in the NVM
 */
#define MAX_CREDENTIAL_OBJECTS                   \
  ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_LAST \
  - ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_BASE

/**
 * Credential metadata object for storage in NVM.
 */
typedef struct credential_metadata_nvm_ {
  uint16_t modifier_node_id;
  uint8_t length;
  u3c_modifier_type modifier_type;
} credential_metadata_nvm;

/**
 * The User descriptor table is an array of associations between User Unique IDs
 * and file IDs of User objects.
 * Only the entries from ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_BASE to
 * ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_BASE + n_users - 1 are to be
 * considered valid.
 */
typedef struct u3c_user_descriptor_ {
  uint16_t unique_identifier;
  uint16_t object_offset;
} u3c_user_descriptor;

/**
 * The Credential descriptor table is an array of associations between unique
 * Credentials and file IDs of Credential metadata objects. A Credential is
 * identified by its owner's User Unique ID and the Credential's type and slot.
 * Only the entries from ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_BASE to
 * ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_BASE + n_credentials - 1 are to be
 * considered valid.
 */
typedef struct u3c_credential_descriptor_ {
  uint16_t user_unique_identifier;
  uint16_t credential_slot;
  uint16_t object_offset;
  u3c_credential_type credential_type;
} u3c_credential_descriptor;

typedef enum u3c_nvm_operation_ {
  U3C_READ,
  U3C_WRITE
} u3c_nvm_operation;

typedef enum u3c_nvm_area_ {
  AREA_NUMBER_OF_USERS,
  AREA_NUMBER_OF_CREDENTIALS,
  AREA_USER_DESCRIPTORS,
  AREA_USERS,
  AREA_USER_NAMES,
  AREA_CREDENTIAL_DESCRIPTORS,
  AREA_CREDENTIAL_METADATA,
  AREA_CREDENTIAL_DATA,
} u3c_nvm_area;

/****************************************************************************/
/*                             STATIC VARIABLES                             */
/****************************************************************************/

/**
 * The following variables must be loaded with the appropriate values by
 * invoking @ref CC_UserCredential_init_database after an application reset.
 */

// Maximum number of entries
static uint16_t max_users = 0;
static uint16_t max_credentials = 0;

// Current number of entries
static uint16_t n_users = UINT16_MAX;
static uint16_t n_credentials = UINT16_MAX;

/**
 * @brief Index of the circular buffer's head
 *
 * The range of file IDs between
 * ZAF_FILE_ID_CC_USER_CREDENTIAL_<OBJECT_TYPE>_BASE and
 * ZAF_FILE_ID_CC_USER_CREDENTIAL_<OBJECT_TYPE>_LAST is treated as a circular
 * buffer.
 * When adding a new entry, the pool of available file IDs is scanned until a
 * free ID is found, which is then populated with the corresponding object's
 * data.
 */
static uint16_t users_buffer_head = 0;
static uint16_t credentials_buffer_head = 0;

/****************************************************************************/
/*                             PRIVATE FUNCTIONS                            */
/****************************************************************************/

/**
 * Execute an NVM read or write operation for object types handled by the User
 * Credential Command Class.
 *
 * @return true if the operation has been executed succesfully and more than 0
 *         bytes were transferred
 */
bool nvm(
  u3c_nvm_operation operation, u3c_nvm_area area, uint16_t offset, void* pData,
  uint16_t size)
{
  // Set parameters depending on the NVM area
  zpal_nvm_object_key_t file_base;
  switch (area) {
    /**********************/
    /* Known size objects */
    /**********************/
    case AREA_NUMBER_OF_USERS:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_NUMBER_OF_USERS;
      size = sizeof(uint16_t);
      offset = 0;
      break;

    case AREA_NUMBER_OF_CREDENTIALS:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_NUMBER_OF_CREDENTIALS;
      size = sizeof(uint16_t);
      offset = 0;
      break;

    case AREA_USER_DESCRIPTORS:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_DESCRIPTOR_TABLE;
      size = sizeof(u3c_user_descriptor) * n_users;
      offset = 0;
      break;

    case AREA_CREDENTIAL_DESCRIPTORS:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_DESCRIPTOR_TABLE;
      size = sizeof(u3c_credential_descriptor) * n_credentials;
      offset = 0;
      break;

    case AREA_USERS:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_BASE;
      size = sizeof(u3c_user);
      break;

    case AREA_CREDENTIAL_METADATA:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_BASE;
      size = sizeof(credential_metadata_nvm);
      break;

    /************************/
    /* Dynamic size objects */
    /************************/
    case AREA_CREDENTIAL_DATA:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_CREDENTIAL_DATA_BASE;
      break;

    case AREA_USER_NAMES:
      file_base = ZAF_FILE_ID_CC_USER_CREDENTIAL_USER_NAME_BASE;
      break;

    default:
      return false;
  }

  if (size == 0) {
    return true;
  }

  zpal_status_t nvm_result = ZPAL_STATUS_FAIL;
  switch (operation) {
    case U3C_READ:
      nvm_result = ZAF_nvm_read(file_base + offset, pData, (size_t)size);
      break;
    case U3C_WRITE:
      nvm_result = ZAF_nvm_write(file_base + offset, pData, (size_t)size);
      break;
    default:
      break;
  }
  return nvm_result == ZPAL_STATUS_OK;
}

/**
 * Convert a u3c_credential_metadata struct to the format stored in the NVM.
 *
 * @param[out] p_metadata_nvm Pointer to the Credential metadata struct to be
 *                            stored in the NVM
 * @param[in] p_metadata_in   Pointer to the incoming standard Credential
 *                            metadata struct
 */
void convert_credential_metadata_to_nvm(
  credential_metadata_nvm * p_metadata_nvm,
  u3c_credential_metadata * p_metadata_in)
{
  p_metadata_nvm->length = p_metadata_in->length;
  p_metadata_nvm->modifier_node_id = p_metadata_in->modifier_node_id;
  p_metadata_nvm->modifier_type = p_metadata_in->modifier_type;
}

/**
 * Convert Credential metadata stored in the NVM to a u3c_credential_metadata
 * struct.
 *
 * @param[out] p_metadata_out Pointer to the outgoing standard Credential
 *                            struct
 * @param[in] p_metadata_nvm  Pointer to the Credential metadata struct stored
 *                            in the NVM
 * @param[in] type            The type of the Credential
 * @param[in] slot            The slot of the Credential
 * @return True if the conversion has succeeded
 */
bool convert_credential_metadata_from_nvm(
  u3c_credential_metadata * p_metadata_out,
  credential_metadata_nvm * p_metadata_nvm,
  u3c_credential_type type,
  uint16_t slot)
{
  p_metadata_out->length = p_metadata_nvm->length;
  p_metadata_out->modifier_node_id = p_metadata_nvm->modifier_node_id;
  p_metadata_out->modifier_type = p_metadata_nvm->modifier_type;
  p_metadata_out->slot = slot;
  p_metadata_out->type = type;
  return true;
}

static void ordered_insert_user_descriptor(u3c_user_descriptor * users, u3c_user * user, uint16_t offset)
{
  uint16_t insert_index = n_users;

  // Find the correct position to insert the new user
  for (uint16_t i = 0; i < n_users; ++i) {
    if (users[i].unique_identifier > user->unique_identifier) {
      insert_index = i;
      break;
    }
  }

  // Shift the elements to make room for the new user
  memmove(&users[insert_index + 1], &users[insert_index], (n_users - insert_index) * sizeof(u3c_user_descriptor));

  // Insert the new user at the correct position
  users[insert_index].object_offset = offset;
  users[insert_index].unique_identifier = user->unique_identifier;

  // Increment the number of users
  ++n_users;
}

static void ordered_insert_credential_descriptor(u3c_credential_descriptor * credentials, u3c_credential * p_credential, uint16_t object_offset)
{
  uint16_t insert_index_base = n_credentials;
  uint16_t insert_index = n_credentials;

  // Find the minimum Credential type, this is used as a base for the insert index
  for (uint16_t i = 0; i < n_credentials; ++i) {
    if (credentials[i].credential_type == p_credential->metadata.type) {
      insert_index_base = i;
      break;
    }
  }

  // Find the correct position to insert the new credential according to the slot
  for (uint16_t i = insert_index_base; i < n_credentials; ++i) {
    if (credentials[i].credential_slot > p_credential->metadata.slot 
        || credentials[i].credential_type != p_credential->metadata.type) {
      insert_index = i;
      break;
    }
  }

  // Shift the elements to make room for the new credential
  memmove(&credentials[insert_index + 1], &credentials[insert_index], (n_credentials - insert_index) * sizeof(u3c_credential_descriptor));

  // Insert the new credential at the correct position
  credentials[insert_index].object_offset = object_offset;
  credentials[insert_index].user_unique_identifier = p_credential->metadata.uuid;
  credentials[insert_index].credential_type = p_credential->metadata.type;
  credentials[insert_index].credential_slot = p_credential->metadata.slot;


  // Increment the number of credentials
  ++n_credentials;
}

void init_database_variables()
{
  users_buffer_head = 0;
  credentials_buffer_head = 0;
  max_users = cc_user_credential_get_max_user_unique_idenfitiers();
  max_credentials = MAX_CREDENTIAL_OBJECTS;

  /**
   * The upper limit of the descriptor table buffer sizes takes precedence over
   * the limits provided by the configuration API.
   */
  if (max_users > U3C_BUFFER_SIZE_USER_DESCRIPTORS) {
    max_users = U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS;
  }
  if (max_credentials > U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS) {
    max_credentials = U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS;
  }

  // Ensure that there are enough file IDs reserved for objects
  assert(max_users <= MAX_USER_OBJECTS);
  assert(max_credentials <= MAX_CREDENTIAL_OBJECTS);
}

/****************************************************************************/
/*                           GENERAL API FUNCTIONS                          */
/****************************************************************************/

void CC_UserCredential_factory_reset(void)
{
  n_users = 0;
  n_credentials = 0;
  // Create empty descriptor tables to initialize their NVM files
  u3c_user_descriptor user_desciptors[1] = { 0 };
  u3c_credential_descriptor credential_descriptors[1] = { 0 };
  nvm(U3C_WRITE, AREA_USER_DESCRIPTORS, 0, user_desciptors, 0);
  nvm(U3C_WRITE, AREA_CREDENTIAL_DESCRIPTORS, 0, credential_descriptors, 0);
  // Initialize static database variables
  nvm(U3C_WRITE, AREA_NUMBER_OF_USERS, 0, &n_users, 0);
  nvm(U3C_WRITE, AREA_NUMBER_OF_CREDENTIALS, 0, &n_credentials, 0);
  init_database_variables();
}

void CC_UserCredential_init_database(void)
{
  // Read stored variables, or initialize them to factory defaults if unset
  bool is_nvm_initialized = true;
  is_nvm_initialized &= nvm(U3C_READ, AREA_NUMBER_OF_USERS, 0, &n_users, 0);
  is_nvm_initialized &= nvm(U3C_READ, AREA_NUMBER_OF_CREDENTIALS, 0, &n_credentials, 0);
  if (!is_nvm_initialized
      || n_users == UINT16_MAX || n_credentials == UINT16_MAX
      ) {
    CC_UserCredential_factory_reset();
  } else {
    init_database_variables();
  }
}

/****************************************************************************/
/*                        USER RELATED API FUNCTIONS                        */
/****************************************************************************/

u3c_db_operation_result CC_UserCredential_get_user(
  uint16_t unique_identifier, u3c_user * user, uint8_t * name)
{
  // Check if the database is empty
  if (n_users < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the User descriptor table from NVM
  u3c_user_descriptor users[U3C_BUFFER_SIZE_USER_DESCRIPTORS];
  memset(users, 0, sizeof(users));

  if (!nvm(U3C_READ, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  // Find User
  for (uint16_t i = 0; i < n_users; ++i) {
    if (users[i].unique_identifier == unique_identifier) {
      // User found

      // Copy User object from NVM if requested
      if (user) {
        if (!nvm(U3C_READ, AREA_USERS, users[i].object_offset, user, 0)) {
          return U3C_DB_OPERATION_RESULT_ERROR_IO;
        }
      }

      // Copy User name from NVM if requested
      if (name) {
        if (!nvm(U3C_READ, AREA_USER_NAMES, users[i].object_offset, name,
                 user->name_length)) {
          return U3C_DB_OPERATION_RESULT_ERROR_IO;
        }
      }

      return U3C_DB_OPERATION_RESULT_SUCCESS;
    }
  }

  // User not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

uint16_t CC_UserCredential_get_next_user(uint16_t unique_identifier)
{
  // Check if the database is empty
  if (n_users < 1) {
    return 0;
  }

  // Read the User descriptor table from NVM
  u3c_user_descriptor users[U3C_BUFFER_SIZE_USER_DESCRIPTORS];
  memset(users, 0, sizeof(users));

  if (!nvm(U3C_READ, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
    return 0;
  }

  uint16_t result = 0;
  if (unique_identifier == 0) {
    // Find the first User
    return users[0].unique_identifier;
  } else {
    // Find the next User
    for (uint16_t i = 0; i < n_users - 1; ++i) {
      if (users[i].unique_identifier == unique_identifier) {
        result = users[i + 1].unique_identifier;
        break;
      }
    }
  }

  return result;
}

u3c_db_operation_result CC_UserCredential_add_user(
  u3c_user * user, uint8_t * name)
{
  // Check if the database is full
  if (n_users >= max_users) {
    return U3C_DB_OPERATION_RESULT_FAIL_FULL;
  }

  // Read the User descriptor table from NVM if it is not empty
  u3c_user_descriptor users[U3C_BUFFER_SIZE_USER_DESCRIPTORS];
  memset(users, 0, sizeof(users));

  if (n_users > 0 && !nvm(U3C_READ, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  // Find next empty object
  bool available = false;
  uint16_t object_offset = 0;

  // Loop through all objects
  for (uint16_t attempts = 0; attempts < max_users; ++attempts) {
    object_offset = users_buffer_head;
    available = true;

    // Loop through the descriptor table
    for (uint16_t i = 0; i < n_users; ++i) {
      // Check if the user already exists
      if (users[i].unique_identifier == user->unique_identifier) {
        return U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED;
      }

      // Check if the object is not assigned to any User
      if (users[i].object_offset == object_offset) {
        users_buffer_head = (uint16_t)((users_buffer_head + 1) % max_users);
        available = false;
        break; // Try the next object
      }
    }

    if (available) {
      // Write User object and name in NVM
      if (!nvm(U3C_WRITE, AREA_USERS, object_offset, user, 0)
          || !nvm(U3C_WRITE, AREA_USER_NAMES, object_offset, name,
                  user->name_length)) {
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Update the descriptor table
      ordered_insert_user_descriptor(users, user, object_offset);

      // Update the descriptor table and number of Users in NVM
      if (
        nvm(U3C_WRITE, AREA_USER_DESCRIPTORS, 0, users, 0)
        && nvm(U3C_WRITE, AREA_NUMBER_OF_USERS, 0, &n_users, 0)
        ) {
        return U3C_DB_OPERATION_RESULT_SUCCESS;
      } else {
        --n_users;
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }
    }
  }

  // Impossible path! The database is not full, but no free object was found
  return U3C_DB_OPERATION_RESULT_ERROR;
}

u3c_db_operation_result CC_UserCredential_modify_user(
  u3c_user * user, uint8_t * name)
{
  // Check if the database is empty
  if (n_users < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the User descriptor table from NVM
  u3c_user_descriptor users[U3C_BUFFER_SIZE_USER_DESCRIPTORS];
  memset(users, 0, sizeof(users));

  if (!nvm(U3C_READ, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  // Find User
  for (uint16_t i = 0; i < n_users; ++i) {
    if (users[i].unique_identifier == user->unique_identifier) {
      return (
        // Overwrite User object in NVM
        nvm(U3C_WRITE, AREA_USERS, users[i].object_offset, user, 0)

        // Overwrite User name in NVM
        && nvm(U3C_WRITE, AREA_USER_NAMES, users[i].object_offset, name,
               user->name_length))

             ? U3C_DB_OPERATION_RESULT_SUCCESS
             : U3C_DB_OPERATION_RESULT_ERROR_IO;
    }
  }

  // User not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

u3c_db_operation_result CC_UserCredential_delete_user(
  uint16_t user_unique_identifier)
{
  // Check if the database is empty
  if (n_users < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the User descriptor table from NVM
  u3c_user_descriptor users[U3C_BUFFER_SIZE_USER_DESCRIPTORS];
  memset(users, 0, sizeof(users));

  if (!nvm(U3C_READ, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  // Find User
  for (uint16_t i = 0; i < n_users; ++i) {
    if (users[i].unique_identifier == user_unique_identifier) {
      --n_users;

      // If the deleted User was not the last in the list
      if (i != n_users) {
        // Shift the elements to fill the gap
        memmove(&users[i], &users[n_users], (n_users - i) * sizeof(u3c_user_descriptor));

      }
      // Otherwise, simply consider its entry 'popped' from the array.

      // Update the descriptor table in NVM
      if (!nvm(U3C_WRITE, AREA_USER_DESCRIPTORS, 0, &users, 0)) {
        ++n_users;
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Update the number of Users in NVM
      if (!nvm(U3C_WRITE, AREA_NUMBER_OF_USERS, 0, &n_users, 0)) {
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Make sure the buffer's head is pointing at a valid object
      if (users_buffer_head >= n_users) {
        users_buffer_head = 0;
      }

      return U3C_DB_OPERATION_RESULT_SUCCESS;
    }
  }

  // User not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

/****************************************************************************/
/*                     CREDENTIAL RELATED API FUNCTIONS                     */
/****************************************************************************/

u3c_db_operation_result CC_UserCredential_get_credential(
  uint16_t user_unique_identifier, u3c_credential_type credential_type,
  uint16_t credential_slot, u3c_credential_metadata * p_credential_metadata,
  uint8_t * p_credential_data)
{
  // Check if the database is empty
  if (n_credentials < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the Credential descriptor table from NVM
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS];
  if (!nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  bool match_any_user = (user_unique_identifier == 0);

  // Find Credential
  for (uint16_t i = 0; i < n_credentials; ++i) {
    if ((match_any_user
         || credentials[i].user_unique_identifier == user_unique_identifier)
        && credentials[i].credential_type == credential_type
        && credentials[i].credential_slot == credential_slot
        ) {
      credential_metadata_nvm metadata;
      memset(&metadata, 0, sizeof(credential_metadata_nvm));

      // Copy Credential metadata from NVM if requested
      if (p_credential_metadata) {
        credential_metadata_nvm metadata;

        if (!nvm(U3C_READ, AREA_CREDENTIAL_METADATA, credentials[i].object_offset,
                &metadata, 0)) {
          return U3C_DB_OPERATION_RESULT_ERROR_IO;
        }

        // Assemble outgoing Credential metadata struct
        p_credential_metadata->uuid = user_unique_identifier;
        p_credential_metadata->slot = credential_slot;
        p_credential_metadata->type = credential_type;
        p_credential_metadata->length = metadata.length;
        p_credential_metadata->modifier_node_id = metadata.modifier_node_id;
        p_credential_metadata->modifier_type = metadata.modifier_type;
      }

      // Copy Credential data from NVM if requested

      if (p_credential_data) {
        if (!nvm(U3C_READ, AREA_CREDENTIAL_DATA, credentials[i].object_offset,
                 p_credential_data, p_credential_metadata->length)) {
          return U3C_DB_OPERATION_RESULT_ERROR_IO;
        }
      }
      return U3C_DB_OPERATION_RESULT_SUCCESS;
    }
  }

  // Credential not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

bool CC_UserCredential_get_next_credential(
  uint16_t user_unique_identifier, u3c_credential_type credential_type,
  uint16_t credential_slot, u3c_credential_type * next_credential_type,
  uint16_t * next_credential_slot)
{
  // Check if the database is empty
  if (n_credentials < 1) {
    return false;
  }

  // Read the Credential descriptor table from NVM
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS];
  if (!nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)) {
    return false;
  }

  bool match_any_user = (user_unique_identifier == 0);

  if (credential_type == 0 && credential_slot == 0) {
    // Find the first Credential
    for (uint16_t i = 0; i < n_credentials; ++i) {
      if (match_any_user
          || credentials[i].user_unique_identifier == user_unique_identifier) {
        *next_credential_type = credentials[i].credential_type;
        *next_credential_slot = credentials[i].credential_slot;
        return true;
      }
    }
  } else {
    // Find the next Credential for this User
    for (uint16_t i = 0; i < n_credentials - 1; ++i) {
      if (
        /**
         * If the associated UUID is specified, then it must be the same for
         * both the current and the next Credential
         */
        (match_any_user || (
          credentials[i].user_unique_identifier == user_unique_identifier
          && credentials[i + 1].user_unique_identifier == user_unique_identifier
        ))
        && credentials[i].credential_type == credential_type
        && credentials[i].credential_slot == credential_slot
      ) {
        *next_credential_type = credentials[i + 1].credential_type;
        *next_credential_slot = credentials[i + 1].credential_slot;
        return true;
      }
    }
  }

  // No Credential satisfying the criteria was found
  return false;
}

u3c_db_operation_result CC_UserCredential_add_credential(
  u3c_credential * p_credential)
{
  // Check if the database is full
  if (n_credentials >= max_credentials) {
    return U3C_DB_OPERATION_RESULT_FAIL_FULL;
  }

  // Read Credential descriptor table if it is not empty
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS] = { 0 };
  if (n_credentials > 0
      && !nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)
      ) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  // Find next empty object
  bool available = false;
  uint16_t object_offset = 0;

  // Loop through all objects
  for (uint16_t attempts = 0; attempts < max_credentials; ++attempts) {
    object_offset = credentials_buffer_head;
    available = true;

    // Loop through the descriptor table
    for (uint16_t i = 0; i < n_credentials; ++i) {
      // Check if the Credential already exists
      if (credentials[i].credential_type == p_credential->metadata.type
          && credentials[i].credential_slot == p_credential->metadata.slot) {
        return U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED;
      }

      // Check if object is not assigned to any Credential
      if (credentials[i].object_offset == object_offset) {
        credentials_buffer_head = (uint16_t)((credentials_buffer_head + 1)
                                             % max_credentials);
        available = false;
        break; // Try the next object
      }
    }

    if (available) {
      credential_metadata_nvm metadata;
      convert_credential_metadata_to_nvm(&metadata, &p_credential->metadata);

      // Write Credential metadata and data in NVM
      if (!nvm(U3C_WRITE, AREA_CREDENTIAL_METADATA, object_offset, &metadata, 0)
          || !nvm(U3C_WRITE, AREA_CREDENTIAL_DATA, object_offset,
                  p_credential->data, metadata.length)) {
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Update descriptor table
      ordered_insert_credential_descriptor(credentials, p_credential, object_offset);

      // Update the descriptor table and number of Credentials in NVM
      if (
        nvm(U3C_WRITE, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)
        && nvm(U3C_WRITE, AREA_NUMBER_OF_CREDENTIALS, 0, &n_credentials, 0)) {
        return U3C_DB_OPERATION_RESULT_SUCCESS;
      } else {
        --n_credentials;
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }
    }
  }

  // Impossible path! The database is not full, but no free object was found
  return U3C_DB_OPERATION_RESULT_ERROR;
}

u3c_db_operation_result CC_UserCredential_modify_credential(
  u3c_credential * p_credential)
{
  // Check if the database is empty
  if (n_credentials < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the Credential descriptor table from NVM
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS];
  if (!nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  bool match_any_user = (p_credential->metadata.uuid == 0);

  // Find Credential
  for (uint16_t i = 0; i < n_credentials; ++i) {
    if ((match_any_user
         || credentials[i].user_unique_identifier == p_credential->metadata.uuid
         )
        && credentials[i].credential_type == p_credential->metadata.type
        && credentials[i].credential_slot == p_credential->metadata.slot
        ) {
      credential_metadata_nvm metadata;
      convert_credential_metadata_to_nvm(&metadata, &p_credential->metadata);

      return (
        // Overwrite Credential metadata in NVM
        nvm(U3C_WRITE, AREA_CREDENTIAL_METADATA, credentials[i].object_offset,
            &metadata, 0)

        // Overwrite Credential data in NVM
        && nvm(U3C_WRITE, AREA_CREDENTIAL_DATA, credentials[i].object_offset,
               p_credential->data, p_credential->metadata.length))
             ? U3C_DB_OPERATION_RESULT_SUCCESS
             : U3C_DB_OPERATION_RESULT_ERROR_IO;
    }
  }

  // Credential not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

u3c_db_operation_result CC_UserCredential_delete_credential(
  uint16_t user_unique_identifier, u3c_credential_type credential_type,
  uint16_t credential_slot)
{
  // Check if the database is empty
  if (n_credentials < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the Credential descriptor table from NVM
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS];
  if (!nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  bool match_any_user = (user_unique_identifier == 0);

  // Find Credential
  for (uint16_t i = 0; i < n_credentials; ++i) {
    if ((match_any_user
         || credentials[i].user_unique_identifier == user_unique_identifier)
        && credentials[i].credential_type == credential_type
        && credentials[i].credential_slot == credential_slot) {
      --n_credentials;

      // If the deleted Credential was not the last in the list
      if (i != n_credentials) {
        // Move the entry at the end of the table into the deleted entry's
        // position.
        memcpy(&credentials[i], &credentials[n_credentials],
               sizeof(u3c_credential_descriptor));
      }
      // Otherwise, simply consider its entry 'popped' from the array.

      // Update the descriptor table
      if (!nvm(U3C_WRITE, AREA_CREDENTIAL_DESCRIPTORS, 0, credentials, 0)) {
        ++n_credentials;
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Update the number of Credentials
      if (!nvm(U3C_WRITE, AREA_NUMBER_OF_CREDENTIALS, 0, &n_credentials, 0)) {
        return U3C_DB_OPERATION_RESULT_ERROR_IO;
      }

      // Make sure the buffer's head is pointing at a valid object
      if (credentials_buffer_head >= n_credentials) {
        credentials_buffer_head = 0;
      }

      return U3C_DB_OPERATION_RESULT_SUCCESS;
    }
  }

  // Credential not found
  return U3C_DB_OPERATION_RESULT_FAIL_DNE;
}

u3c_db_operation_result CC_UserCredential_move_credential(
  uint16_t source_user_uid, u3c_credential_type credential_type,
  uint16_t source_credential_slot, uint16_t destination_user_uid,
  uint16_t destination_credential_slot)
{
  // Check if the database is empty
  if (n_credentials < 1) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }

  // Read the Credential descriptor table from NVM
  u3c_credential_descriptor credentials[U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS];
  if (!nvm(U3C_READ, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0)) {
    return U3C_DB_OPERATION_RESULT_ERROR_IO;
  }

  bool source_exists = false;
  bool destination_exists = false;
  uint16_t i_source;

  // Find source and destination Credentials
  for (uint16_t i = 0;
       i < n_credentials && !source_exists && !destination_exists;
       ++i) {
    if (credentials[i].credential_type == credential_type) {
      // Source Credential
      if (credentials[i].user_unique_identifier == source_user_uid
          && credentials[i].credential_slot == source_credential_slot) {
        i_source = i;
        source_exists = true;
      }

      // Destination Credential
      if (credentials[i].user_unique_identifier == destination_user_uid
          && credentials[i].credential_slot == destination_credential_slot) {
        destination_exists = true;
      }
    }
  }

  // Check if the criteria for the move were met
  if (!source_exists) {
    return U3C_DB_OPERATION_RESULT_FAIL_DNE;
  }
  if (destination_exists) {
    return U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED;
  }

  // Credential data is not moved, only the metadata
  uint16_t object_offset = credentials[i_source].object_offset;
  
  u3c_credential credential = {
    .metadata = {
      .uuid = destination_user_uid,
      .slot = destination_credential_slot,
      .type = credential_type,
    }
  };

  // Remove the old element from the array
  memmove(&credentials[i_source], &credentials[i_source + 1], (n_credentials - i_source) * sizeof(u3c_credential_descriptor));
  n_credentials--;

  // Insert the new element into the array and with the old credentials object offset
  ordered_insert_credential_descriptor(credentials, &credential, object_offset);

  return (
    // Overwrite Credential descriptor table in NVM
    nvm(U3C_WRITE, AREA_CREDENTIAL_DESCRIPTORS, 0, &credentials, 0))
         ? U3C_DB_OPERATION_RESULT_SUCCESS
         : U3C_DB_OPERATION_RESULT_ERROR_IO;
}
