/**
 * @file
 * @brief Handler for Command Class User Credential.
 * @copyright 2023 Silicon Laboratories Inc.
 */

#include "CC_UserCredential.h"
#include "cc_user_credential_config_api.h"
#include "cc_user_credential_io_config.h"
#include "cc_user_credential_io.h"
#include "cc_user_credential_operations.h"
#include "cc_user_credential_validation.h"
#include "CC_Notification.h"
#include "CC_Supervision.h"
#include "zaf_transport_tx.h"
#include "zaf_event_distributor_soc.h"
#include "ZW_TransportSecProtocol.h"
#include <string.h>
#include "assert.h"
#include "AppTimer.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "ZAF_Common_interface.h"
#include "CRC.h" // CC:0083.01.15.11.000 & CC:0083.01.17.11.000 & CC:0083.01.19.11.001
#include "zpal_power_manager.h"
#include "ZAF_TSE.h"
#include "association_plus_base.h"

/// Longest possible default user name: "User-65535", times 2 for UTF-16
#define DEFAULT_NAME_MAX_SIZE (sizeof("User-65535") - 1) * 2

// Size of a Capabilities Report Variant Group for a single Credential Type
#define CREDENTIAL_CAPABILITIES_REPORT_VG_SIZE ( \
  sizeof(uint8_t)   /**< Credential Type */\
  + sizeof(uint8_t)  /**< Properties1 */\
  + sizeof(uint16_t) /**< Credential Slots */\
  + sizeof(uint8_t)  /**< Minimum Length of Data */\
  + sizeof(uint8_t)  /**< Maximum Length of Data */\
  + sizeof(uint8_t)  /**< Credential Learn Recommended Timeout */\
  + sizeof(uint8_t)  /**< Credential Learn Number of Steps */\
  + sizeof(uint8_t)  /**< Maximum Credential Hash Length */\
)
#define S_TO_MS 1000 // Seconds to milliseconds, used for timers

/**
 * Maximum size for Credential Usage Data Notification Event/State parameters
 * 
 * UUID (16 bits) | Credential Count (8 bits)
 * 3 * (Credential Type (8 bits) + Credential Slot (16 bits))
 */
#define CREDENTIAL_USAGE_DATA_MAX_SIZE ( \
  sizeof(uuid) + sizeof(credential_count) \
  + (CREDENTIAL_RULE_TRIPLE * (1 + sizeof(p_credential_metadata->slot))) \
)

// Number of bytes of a credential set frame if credential length and credential data fields are empty.
#define CREDENTIAL_SET_FRAME_LENGTH_WITHOUT_CREDENTIAL_LENGTH (offsetof(ZW_CREDENTIAL_SET_1BYTE_FRAME, credentialLength))
// Number of bytes of a credential set frame if credential data field is empty.
#define CREDENTIAL_SET_FRAME_LENGTH_WITHOUT_CREDENTIAL_DATA (offsetof(ZW_CREDENTIAL_SET_1BYTE_FRAME, credentialData1))

// State and timer for the current Credential Learn operation
typedef struct credential_learn_status_ {
  bool in_progress;
  u3c_credential_learn_event_data data;
  RECEIVE_OPTIONS_TYPE_EX rx_options;
  uint8_t total_steps;
  uint8_t remaining_steps;
} credential_learn_status;

/****************************************************************************/
/*                           FORWARD DECLARATIONS                           */
/****************************************************************************/

static void send_report(
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options,
  const bool notify_lifeline
);

/****************************************************************************/
/*                             STATIC VARIABLES                             */
/****************************************************************************/

/// Static Credential Learn data
static credential_learn_status cl_state;
static SSwTimer cl_timeout_timer;

/// Power lock handle for the Credential Learn operation
static zpal_pm_handle_t credential_learn_power_lock = NULL;

/// Persistent data for reports
static uint8_t report_out_frame[TX_DATA_MAX_DATA_SIZE] = { 0 };
static uint8_t report_out_size = 0;

/**
 * @return true if a Credential Learn operation is in currently progress
 */
bool is_credential_learn_in_progress() {
  return cl_state.in_progress;
}

/**
 * @brief Cancels the learning process for user credentials.
 *
 * This function is responsible for canceling the learning process for user credentials.
 * It is called when the learning process needs to be stopped or aborted.
 */
static void credential_learn_reset();

static void credential_learn_timer_timeout_callback(__attribute__((unused)) SSwTimer *pTimer);

static void init(void)
{
  CC_UserCredential_init_database();
  AppTimerRegister(&cl_timeout_timer, false, credential_learn_timer_timeout_callback);
  if (!credential_learn_power_lock) {
    credential_learn_power_lock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);
  }
  credential_learn_reset();
}

static void reset(void)
{
  CC_UserCredential_factory_reset();
}

static uint8_t u3c_user_type_numeric_value[] = {
  0x00, // General User
  0x03, // Programming User
  0x04, // Non-Access User
  0x05, // Duress User
  0x06, // Disposable User
  0x07, // Expiring User
  0x09, // Remote Only User
};


/**
 * Generates a default User Name value with either ASCII or UTF-16 encoding
 *
 * @param[out] pName Pointer to the User Name
 * @param[in] pUser Pointer to the User object
 */
void set_default_name(uint8_t * pName, u3c_user * pUser)
{
  const char prefix[] = { 'U', 's', 'e', 'r', '-' };
  uint8_t id_text[DEFAULT_NAME_MAX_SIZE - sizeof(prefix)] = { 0 };
  bool is_16_bit_encoding =
    pUser->name_encoding == USER_NAME_ENCODING_UNICODE_UTF_16;

  uint8_t digits = 0;
  uint16_t ten_exponent = 1;
  for (uint8_t i = 0; i < sizeof(id_text); ++i) {
    uint16_t digit = (pUser->unique_identifier / ten_exponent) % 10;
    id_text[sizeof(id_text) - 1 - i] = '0' + (uint8_t)digit;
    if (digit > 0) {
      digits = i + 1;
    }
    ten_exponent *= 10;
  }
  uint8_t character_count = sizeof(prefix) + digits;

  for (uint8_t i = 0; i < character_count; ++i) {
    uint8_t current_character;
    if (i < sizeof(prefix)) {
      current_character = (uint8_t)prefix[i];
    } else {
      current_character = id_text[i - sizeof(prefix) + sizeof(id_text) - digits];
    }
    if (is_16_bit_encoding) {
      pName[2 * i] = '\0';
      pName[(2 * i) + 1] = current_character;
    } else {
      pName[i] = current_character;
    }
  }

  pUser->name_length = character_count * (is_16_bit_encoding ? 2 : 1);
}

static void calculate_credentials_checksum_for_uuid(
  const uint16_t uuid, uint16_t * checksum)
{
  u3c_credential_type type = CREDENTIAL_TYPE_NONE;
  uint16_t slot = 0;
  u3c_credential_metadata existing_metadata = { 0 };

  /**
   * When more than one Credential of the given uuid exists,
   * we must concatenate the data of each Credential and calculate the CRC-16
   * CC:0083.01.17.11.007
   * If there is no Credential data set for a User Unique Identifier, the checksum MUST NOT be modified
   * other than for the User Unique Identifier data.
   * CC:0083.01.17.11.005
   */
  while (
    CC_UserCredential_get_next_credential(
      uuid, type, slot, &type, &slot)
    ) {
    uint8_t e_data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };
    uint8_t slot_msb = 0;
    uint8_t slot_lsb = 0;

    CC_UserCredential_get_credential(
      uuid, type, slot, &existing_metadata, e_data);

    slot_msb = (slot >> 8);
    slot_lsb = slot & 0xFF;
    /**
     * Credential Type (8 bits) | Credential Slot (16 bits) |
     * Credential Length (8 bits) | Credential Data (Credential Length bytes)
     * CC:0083.01.17.11.002
     */
    *checksum = CRC_CheckCrc16(*checksum, (uint8_t *)&type, 1);
    *checksum = CRC_CheckCrc16(*checksum, &slot_msb, 1);
    *checksum = CRC_CheckCrc16(*checksum, &slot_lsb, 1);
    *checksum = CRC_CheckCrc16(*checksum, &existing_metadata.length, 1);
    *checksum = CRC_CheckCrc16(*checksum, e_data, existing_metadata.length);
  }
}

JOB_STATUS CC_User_Credential_UsageNotification_tx(
  const uint8_t notification_event,
  const uint16_t uuid,
  const uint8_t credential_count,
  const u3c_credential_metadata * const p_credential_metadata
)
{
  uint8_t notification_event_parameters[CREDENTIAL_USAGE_DATA_MAX_SIZE] = {
    uuid >> 8,
    uuid & 0xFF,
    credential_count
  };

  uint8_t * p_event_parameters = notification_event_parameters + 3;
  for (uint8_t i = 0; i < credential_count; ++i) {
    *p_event_parameters++ = p_credential_metadata[i].type;
    *p_event_parameters++ = p_credential_metadata[i].slot >> 8;
    *p_event_parameters++ = p_credential_metadata[i].slot & 0xFF;
  }

  return CC_Notification_TriggerAndTransmit(
    0, notification_event, notification_event_parameters,
    (p_event_parameters - notification_event_parameters), NULL, false);
}

/**
 * Called upon receiving a User Capabilities Get frame.
 *
 * @param[out] pReport Pointer to the data that will be included in the
 *                     Capabilities Report frame.
 */
static void
CC_UserCredential_UserCapabilitiesGet_handler(cc_handler_output_t * output)
{
  ZW_USER_CAPABILITIES_REPORT_2BYTE_FRAME * pReport = &output->frame->ZW_UserCapabilitiesReport2byteFrame;
  uint16_t max_user_unique_identifers =
    cc_user_credential_get_max_user_unique_idenfitiers();
  uint16_t supported_user_types_bit_mask = 0;
  for (uint8_t i = 0; i < sizeof(u3c_user_type_numeric_value); ++i) {
    uint8_t type = u3c_user_type_numeric_value[i];
    supported_user_types_bit_mask |= (cc_user_credential_is_user_type_supported(type) << type);
  }

  pReport->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  pReport->cmd = USER_CAPABILITIES_REPORT;

  pReport->numberOfSupportedUserUniqueIdentifiers1 = max_user_unique_identifers >> 8;
  pReport->numberOfSupportedUserUniqueIdentifiers2 = max_user_unique_identifers & 0xFF;

  pReport->supportedCredentialRulesBitMask =
    cc_user_credential_is_credential_rule_supported(CREDENTIAL_RULE_SINGLE) << CREDENTIAL_RULE_SINGLE
      | cc_user_credential_is_credential_rule_supported(CREDENTIAL_RULE_DUAL) << CREDENTIAL_RULE_DUAL
      | cc_user_credential_is_credential_rule_supported(CREDENTIAL_RULE_TRIPLE) << CREDENTIAL_RULE_TRIPLE;

  pReport->maxLengthOfUserName = cc_user_credential_get_max_length_of_user_name();

  // #TODO: Implement support for User Schedule

  pReport->properties1 =
    ((0 << 7) & USER_CAPABILITIES_REPORT_PROPERTIES1_USER_SCHEDULE_SUPPORT_BIT_MASK)
    | ((cc_user_credential_is_all_users_checksum_supported() << 6) & USER_CAPABILITIES_REPORT_PROPERTIES1_ALL_USERS_CHECKSUM_SUPPORT_BIT_MASK)
    | ((cc_user_credential_is_user_checksum_supported() << 5) & USER_CAPABILITIES_REPORT_PROPERTIES1_USER_CHECKSUM_SUPPORT_BIT_MASK);

  pReport->supportedUserTypesBitMaskLength = 2;
  pReport->variantgroup1.supportedUserTypesBitMask = supported_user_types_bit_mask & 0xFF;
  pReport->variantgroup2.supportedUserTypesBitMask = supported_user_types_bit_mask >> 8 & 0xFF;
}

/**
 * Called upon receiving a Credential Capabilities Get frame.
 *
 * @param[out] pReport Pointer to the data that will be included in the
 *                     Capabilities Report frame.
 * @param[out] pLength Pointer to the length of the frame.
 */
static void
CC_UserCredential_CredentialCapabilitiesGet_handler(
  cc_handler_output_t * output)
{
  ZW_CREDENTIAL_CAPABILITIES_REPORT_4BYTE_FRAME * pReport = &output->frame->ZW_CredentialCapabilitiesReport4byteFrame;
  uint8_t n_credential_types = cc_user_credential_get_number_of_supported_credential_types();

  pReport->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  pReport->cmd = CREDENTIAL_CAPABILITIES_REPORT;
  pReport->properties1 = 0; // Unset all bits.
  pReport->properties1 = (cc_user_credential_is_credential_checksum_supported() << 7); // Bit 7: Credential Checksum Support
  /**
   * Populate admin code support info in report.
   * Bit 6 - Admin Code Supported
   * Bit 5 - Admin Code Deactivation Supported
   */
  pReport->properties1 |= (cc_user_credential_get_admin_code_supported() << 6);
  pReport->properties1 |= (cc_user_credential_get_admin_code_deactivate_supported() << 5);

  pReport->numberOfSupportedCredentialTypes = n_credential_types;

  uint8_t * pArray = (uint8_t *)(&pReport->variantgroup1.credentialType);
  u3c_credential_type i_type = CREDENTIAL_TYPE_NONE;

  /**
   * The frame's variant group will contain arrays with as many elements as
   * there are supported credential types
   */
  for (uint8_t i_array = 0; i_array < n_credential_types; ++i_array) {
    // Find next supported Credential Type
    do {
      ++i_type;
    } while (!cc_user_credential_is_credential_type_supported(i_type));

    // Assemble element of variant group
    bool cl_support = cc_user_credential_is_credential_learn_supported(i_type);
    uint16_t max_credential_slots = cc_user_credential_get_max_credential_slots(i_type);
    uint8_t min_length_of_credential_data = cc_user_credential_get_min_length_of_data(i_type);
    uint8_t max_length_of_credential_data = cc_user_credential_get_max_length_of_data(i_type);
    uint8_t cl_recommended_timeout = cc_user_credential_get_cl_recommended_timeout(i_type);
    uint8_t cl_number_of_steps = cc_user_credential_get_cl_number_of_steps(i_type);
    uint8_t max_hash_length = cc_user_credential_get_max_hash_length(i_type);

    // Credential Type
    *(pArray + i_array) = i_type;

    // Properties1 (bit 7 is CL Support)
    *(pArray + 1 * n_credential_types + i_array) = (cl_support << 7) & 0x80;

    // Number of Supported Credential Slots (MSB, LSB)
    *(pArray + 2 * n_credential_types + 2 * i_array + 0) = max_credential_slots >> 8;
    *(pArray + 2 * n_credential_types + 2 * i_array + 1) = max_credential_slots & 0xFF;

    // Min Length of Credential Data
    *(pArray + 4 * n_credential_types + i_array) = min_length_of_credential_data;

    // Max Length of Credential Data
    *(pArray + 5 * n_credential_types + i_array) = max_length_of_credential_data;

    // Credential Learn Recommended Timeout
    *(pArray + 6 * n_credential_types + i_array) = cl_recommended_timeout;

    // Credential Learn Number of Steps
    *(pArray + 7 * n_credential_types + i_array) = cl_number_of_steps;

    // Maximum Credential Hash Length 
    *(pArray + 8 * n_credential_types + i_array) = max_hash_length;
  }

  output->length = 4 + CREDENTIAL_CAPABILITIES_REPORT_VG_SIZE * n_credential_types;
}

void CC_UserCredential_UserReport_tx(
  const u3c_user_report_type_t report_type,
  const u3c_user * const p_user,
  const uint8_t * const p_name,
  const uint16_t next_uuid,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  ZW_USER_REPORT_1BYTE_FRAME * pReport = (ZW_USER_REPORT_1BYTE_FRAME *)&report_out_frame;

  pReport->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  pReport->cmd = USER_REPORT;
  pReport->userReportType = report_type;
  pReport->nextUserUniqueIdentifier1 = next_uuid >> 8;
  pReport->nextUserUniqueIdentifier2 = next_uuid & 0xFF;
  pReport->userUniqueIdentifier1 = p_user->unique_identifier >> 8;
  pReport->userUniqueIdentifier2 = p_user->unique_identifier & 0xFF;
  pReport->userModifierType = p_user->modifier_type;
  pReport->userModifierNodeId1 = p_user->modifier_node_id >> 8;
  pReport->userModifierNodeId2 = p_user->modifier_node_id & 0xFF;
  pReport->userType = p_user->type;
  pReport->properties1 = p_user->active & USER_REPORT_PROPERTIES1_USER_ACTIVE_STATE_BIT_MASK;
  pReport->credentialRule = p_user->credential_rule;
  pReport->expiringTimeoutMinutes1 = p_user->expiring_timeout_minutes >> 8;
  pReport->expiringTimeoutMinutes2 = p_user->expiring_timeout_minutes & 0xFF;
  pReport->properties2 = p_user->name_encoding & USER_REPORT_PROPERTIES2_USER_NAME_ENCODING_MASK;
  pReport->userNameLength = p_user->name_length;
  // Copy User Name and increase length of outgoing frame by it
  memcpy(&pReport->userName1, p_name, p_user->name_length);
  report_out_size = sizeof(ZW_USER_REPORT_1BYTE_FRAME) - 1 + p_user->name_length;

  /**
   * Determine whether nodes in the the Lifeline association group must be
   * notified.
   * This only applies to successful database modifications.
   */
  bool notify_lifeline =
    (report_type == USER_REP_TYPE_ADDED)
    || (report_type == USER_REP_TYPE_MODIFIED)
    || (report_type == USER_REP_TYPE_DELETED);

  send_report(p_rx_options, notify_lifeline);
}

/**
 * Called upon receiving a User Set frame.
 *
 * @param[in] input Details of the incoming frame
 */
static received_frame_status_t CC_UserCredential_UserSet_handler(
  cc_handler_input_t * input)
{
  ZW_USER_SET_1BYTE_FRAME * pFrameIn = &input->frame->ZW_UserSet1byteFrame;
  RECEIVE_OPTIONS_TYPE_EX * rxOptions = input->rx_options;
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;
  u3c_db_operation_result operation_result = U3C_DB_OPERATION_RESULT_SUCCESS;

  // Extract User data from frame
  uint8_t name[U3C_BUFFER_SIZE_USER_NAME];
  u3c_user user = {
    .unique_identifier = (pFrameIn->userUniqueIdentifier1 << 8 | pFrameIn->userUniqueIdentifier2),
    .type = pFrameIn->userType,
    .active = pFrameIn->properties2 & USER_SET_PROPERTIES2_USER_ACTIVE_STATE_BIT_MASK,
    .credential_rule = pFrameIn->credentialRule,
    .expiring_timeout_minutes = (pFrameIn->expiringTimeoutMinutes1 << 8 | pFrameIn->expiringTimeoutMinutes2),
    .name_encoding = pFrameIn->properties3 & USER_SET_PROPERTIES3_USER_NAME_ENCODING_MASK,
    .name_length = pFrameIn->userNameLength,
    .modifier_type = MODIFIER_TYPE_Z_WAVE,
    .modifier_node_id = rxOptions->sourceNode.nodeId
  };
  uint8_t * pName = &pFrameIn->userName1;
  uint8_t operation = pFrameIn->properties1 & USER_SET_PROPERTIES1_OPERATION_TYPE_MASK;
  bool is_name_truncated = false;

  // CC:0083.01.05.11.014: Ignore frames where the User Unique Identifier is greater than allowed
  if (user.unique_identifier > cc_user_credential_get_max_user_unique_idenfitiers()) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if (operation != USER_SET_OPERATION_TYPE_DELETE) {
    // Ignore frames with unsupported values
    if (
      // CC:0083.01.05.11.015
      user.unique_identifier == 0
      // CC:0083.01.05.11.030
      || !cc_user_credential_is_user_type_supported(user.type)
      // CC:0083.01.05.11.039
      || !cc_user_credential_is_credential_rule_supported(user.credential_rule)
      // CC:0083.01.05.11.045
      || (user.name_encoding > USER_NAME_ENCODING_UNICODE_UTF_16)
      ) {
      return RECEIVED_FRAME_STATUS_FAIL;
    }

    // CC:0083.01.05.11.042: An expiring user must have a non-zero timeout
    if (user.type == USER_TYPE_EXPIRING && user.expiring_timeout_minutes == 0) {
      // Send back data of existing user
      bool user_found = CC_UserCredential_get_user(
        user.unique_identifier, &user, name)
        == U3C_DB_OPERATION_RESULT_SUCCESS;
      if (!user_found) {
        // Or send a blank user if not found
        memset(&user, 0, sizeof(u3c_user));
        user.credential_rule = CREDENTIAL_RULE_SINGLE;
      }
      CC_UserCredential_UserReport_tx(
        USER_REP_TYPE_Z_EXP_MIN_INVALID, &user, name,
        CC_UserCredential_get_next_user(user.unique_identifier), rxOptions);
      return RECEIVED_FRAME_STATUS_FAIL;
    }

    // CC:0083.01.05.11.053: Set non-Expiring User's Expiring Timeout to 0
    if (user.type != USER_TYPE_EXPIRING && user.expiring_timeout_minutes != 0) {
      user.expiring_timeout_minutes = 0;
    }

    // Truncate User Name if it is longer than allowed
    uint8_t user_name_max_length = cc_user_credential_get_max_length_of_user_name();
    if (user.name_length > user_name_max_length) {
      user.name_length = user_name_max_length;
      is_name_truncated = true;
    }

    // If no name is provided, set it to the default value
    if (user.name_length == 0) {
      set_default_name(name, &user);
      pName = name;
    }

    // Do not assign a schedule to a Programming User
  }

  switch (operation) {
    case USER_SET_OPERATION_TYPE_ADD: {
      operation_result = CC_UserCredential_add_user_and_report(&user, pName, rxOptions);
      status = (((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS)
                || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_IDENTICAL))
                && (is_name_truncated == false))
               ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      break;
    }

    case USER_SET_OPERATION_TYPE_MODIFY: {
      operation_result = CC_UserCredential_modify_user_and_report(&user, pName, rxOptions);
      status = (((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS)
                || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_IDENTICAL))
                && (is_name_truncated == false))
               ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      break;
    }

    case USER_SET_OPERATION_TYPE_DELETE: {
      if (user.unique_identifier == 0) {
        // Delete all Users
        uint16_t user_uid = CC_UserCredential_get_next_user(0);

        while (user_uid) {
          /**
           * Fetch the next User's details before deleting the current one
           * (otherwise CC_UserCredential_get_next_user will not find the next
           * one)
           */
          uint16_t next_user_uid = CC_UserCredential_get_next_user(user_uid);
          CC_UserCredential_delete_all_credentials_of_type(user_uid, CREDENTIAL_TYPE_NONE);
          CC_UserCredential_delete_user(user_uid);
          user_uid = next_user_uid;
        }

        // Mirror request in report, resetting non-identifying parameters
        user.modifier_type = MODIFIER_TYPE_DNE;
        user.modifier_node_id = 0;
        user.active = false;
        user.expiring_timeout_minutes = 0;
        CC_UserCredential_UserReport_tx(
          USER_REPORT_DELETED, &user, pName, 0, rxOptions);
        status = RECEIVED_FRAME_STATUS_SUCCESS;
      } else {
        // Delete a single User and report
        operation_result = CC_UserCredential_delete_user_and_report(user.unique_identifier, rxOptions);
        status = ((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS)
                  || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_DNE))
                 ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      }
      break;
    }

    default: {
      status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
    }
  }

  return status;
}

static received_frame_status_t CC_UserCredential_UserGet_handler(
  cc_handler_input_t * input)
{
  ZW_USER_GET_FRAME * pFrameIn = &input->frame->ZW_UserGetFrame;
  uint8_t name[U3C_BUFFER_SIZE_USER_NAME] = { 0 };
  u3c_user user = { 0 };

  uint16_t uid = pFrameIn->userUniqueIdentifier1 << 8 | pFrameIn->userUniqueIdentifier2;
  if (uid == 0) {
    // Return the first available User's data
    uid = CC_UserCredential_get_next_user(0);
  }
  uint16_t next_user_uid = CC_UserCredential_get_next_user(uid);

  u3c_db_operation_result result = CC_UserCredential_get_user(uid, &user, name);

  if (result != U3C_DB_OPERATION_RESULT_SUCCESS) {
    // The User doesn't exist, return blank data, but make sure all fields are valid
    memset(&user, 0, sizeof(user));
    user.unique_identifier = uid;
    user.credential_rule = CREDENTIAL_RULE_SINGLE;
  }

  CC_UserCredential_UserReport_tx(
    USER_REP_TYPE_RESPONSE_TO_GET, &user, name, next_user_uid, input->rx_options);

  return (result == U3C_DB_OPERATION_RESULT_SUCCESS)
    ? RECEIVED_FRAME_STATUS_SUCCESS
    : RECEIVED_FRAME_STATUS_FAIL;
}

// TODO: Move this function outside of the CC.
static received_frame_status_t CC_UserCredential_CredentialSet_handler(
  u3c_operation_type_t operation_type,
  u3c_credential * p_credential,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;
  u3c_db_operation_result operation_result = U3C_DB_OPERATION_RESULT_ERROR;

  if (operation_type != U3C_OPERATION_TYPE_DELETE) {
    if (!validate_new_credential_metadata(&p_credential->metadata)
        // CC:0083.01.0A.11.015,
        || (p_credential->metadata.length
            < cc_user_credential_get_min_length_of_data(p_credential->metadata.type))
        // CC:0083.01.0A.11.014
        || (p_credential->metadata.length
            > cc_user_credential_get_max_length_of_data(p_credential->metadata.type))
        || !validate_new_credential_data(p_credential, p_rx_options))
    {
      return RECEIVED_FRAME_STATUS_FAIL;
    }
  } else {
    // CC:0083.01.0A.11.002: an unsupported Credential Type must be ignored
    if ((p_credential->metadata.type != CREDENTIAL_TYPE_NONE)
        && !cc_user_credential_is_credential_type_supported(p_credential->metadata.type))
    {
      return RECEIVED_FRAME_STATUS_FAIL;
    }
  }

  switch (operation_type) {
    case U3C_OPERATION_TYPE_ADD: {
      operation_result = CC_UserCredential_add_credential_and_report(p_credential, p_rx_options);
      status = ((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS )
               || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_IDENTICAL))
               ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      break;
    }

    case U3C_OPERATION_TYPE_MODIFY: {
      operation_result = CC_UserCredential_modify_credential_and_report(p_credential, p_rx_options);
      status = ((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS)
                || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_IDENTICAL))
               ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      break;
    }

    case U3C_OPERATION_TYPE_DELETE: {
      if (p_credential->metadata.slot == 0) {
        // Bulk delete credentials

        // Remember whether any credentials were deleted
        bool deleted_any_credentials = false;

        uint16_t user_uid = p_credential->metadata.uuid;
        if (user_uid == 0) {
          user_uid = CC_UserCredential_get_next_user(0);
        }

        // Iterate through each user
        while (user_uid) {
          // Delete a user's every credential (of a certain type, if specified)
          deleted_any_credentials |= CC_UserCredential_delete_all_credentials_of_type(user_uid, p_credential->metadata.type);
          user_uid = CC_UserCredential_get_next_user(user_uid);

          /**
           * If the UUID was specified in the request, don't process any
           * additional users.
           */
          if (p_credential->metadata.uuid != 0) {
            break;
          }
        }

        p_credential->metadata.modifier_type = MODIFIER_TYPE_DNE;
        p_credential->metadata.modifier_node_id = 0;

        u3c_credential_report_type_t report_type =
          deleted_any_credentials
          ? CREDENTIAL_REP_TYPE_DELETED
          : CREDENTIAL_REP_TYPE_UNCHANGED;

        // Echo data from request back to sender
        CC_UserCredential_CredentialReport_tx(
          report_type, p_credential, CREDENTIAL_TYPE_NONE, 0,
          p_rx_options);

        status = RECEIVED_FRAME_STATUS_SUCCESS;
      } else if (p_credential->metadata.type != 0) {
        // Delete a single Credential and report
        operation_result = CC_UserCredential_delete_credential_and_report(
          p_credential->metadata.uuid, p_credential->metadata.type,
          p_credential->metadata.slot, p_rx_options);
        status = ((operation_result == U3C_DB_OPERATION_RESULT_SUCCESS)
                  || (operation_result == U3C_DB_OPERATION_RESULT_FAIL_DNE))
                 ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
      }
      // Type == 0 && Slot != 0 is not a valid request
    }
    break;

    default: {
      status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
    }
  }

  return status;
}

static received_frame_status_t CC_UserCredential_CredentialSet_parser(
  cc_handler_input_t * input)
{
  uint16_t uuid = (input->frame->ZW_CredentialSet1byteFrame.userUniqueIdentifier1 << 8)
                  | input->frame->ZW_CredentialSet1byteFrame.userUniqueIdentifier2;
  u3c_credential_type credential_type;
  credential_type = (u3c_credential_type)input->frame->ZW_CredentialSet1byteFrame.credentialType;

  // Validate operation type
  u3c_operation_type_t operation_type;
  operation_type = (u3c_operation_type_t)input->frame->ZW_CredentialSet1byteFrame.properties1
                   & CREDENTIAL_SET_PROPERTIES1_OPERATION_TYPE_MASK;
  if (operation_type > U3C_OPERATION_TYPE_DELETE) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Validate length
  uint8_t credential_length = 0;
  if (input->length < CREDENTIAL_SET_FRAME_LENGTH_WITHOUT_CREDENTIAL_LENGTH) {
    return RECEIVED_FRAME_STATUS_FAIL;
  } else if (input->length > CREDENTIAL_SET_FRAME_LENGTH_WITHOUT_CREDENTIAL_LENGTH) {
    credential_length = input->frame->ZW_CredentialSet1byteFrame.credentialLength;
    if (input->length != (CREDENTIAL_SET_FRAME_LENGTH_WITHOUT_CREDENTIAL_DATA + credential_length)) {
      return RECEIVED_FRAME_STATUS_FAIL;
    }
  }

  if (
    operation_type != U3C_OPERATION_TYPE_DELETE
    && (credential_length < cc_user_credential_get_min_length_of_data(credential_type)
        || credential_length > cc_user_credential_get_max_length_of_data(credential_type))
    ) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  uint16_t credential_slot = (input->frame->ZW_CredentialSet1byteFrame.credentialSlot1 << 8)
                             | input->frame->ZW_CredentialSet1byteFrame.credentialSlot2;
  u3c_credential credential = {
    .metadata = {
      .uuid = uuid,
      .length = credential_length,
      .modifier_node_id = input->rx_options->sourceNode.nodeId,
      .modifier_type = MODIFIER_TYPE_Z_WAVE,
      .slot = credential_slot,
      .type = credential_type
    },
    .data = &input->frame->ZW_CredentialSet1byteFrame.credentialData1
  };

  received_frame_status_t status;
  status = CC_UserCredential_CredentialSet_handler(
    operation_type, &credential, input->rx_options
  );
  return status;
}

typedef void (*u3c_credential_report_tx_callback_t)(
  const u3c_credential_report_type_t report_type,
  const u3c_credential * const p_credential,
  const u3c_credential_type next_credential_type,
  const uint16_t next_credential_slot,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
);

/**
 * Callback function for ZAF TSE to send User or Credential Reports to multiple
 * destinations
 */
static void send_report_tse(
  zaf_tx_options_t * p_tx_options,
  __attribute__((unused)) void * p_data
  )
{
  zaf_transport_tx(
    report_out_frame,
    report_out_size,
    ZAF_TSE_TXCallback,
    p_tx_options
  );
}

/**
 * Sends a User or Credential report.
 * 
 * @param[in] p_rx_options       Pointer to the properties of the incoming frame
 * @param[in] notify_lifeline    true if the nodes in the  Lifeline association
 *                               group should receive the report
 * @param[in] supervision_status What status the encapsulating Supervision frame
 *                               should have (if using Supervision)
 */
static void send_report(
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options,
  const bool notify_lifeline
  )
{
  node_id_t initiator_node_id = p_rx_options->sourceNode.nodeId;
  uint8_t initiator_endpoint = p_rx_options->sourceNode.endpoint;
  uint8_t n_nodes_in_lifeline = 0;
  bool initiator_is_in_lifeline = false;

  if (notify_lifeline) {
    MULTICHAN_NODE_ID * p_nodes = NULL;
    if (
      handleAssociationGetnodeList(LIFELINE_GROUP_ID, 0, &p_nodes, &n_nodes_in_lifeline)
      == NODE_LIST_STATUS_SUCCESS
    ) {
      for (uint8_t i = 0; i < n_nodes_in_lifeline; ++i) {
        if (p_nodes[i].node.nodeId == initiator_node_id
            && p_nodes[i].node.endpoint == initiator_endpoint) {
          initiator_is_in_lifeline = true;
        }
      }
    }
  }

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);

  /**
   * Send single report to requesting node only if
   * - the report is not addressed to the Lifeline group or
   * - the initiating node is not in the Lifeline
   * - the only node in the Lifeline is the initiator (TSE wouldn't send
   *   anything in this case)
   */
  if (!notify_lifeline
      || (!initiator_is_in_lifeline || (n_nodes_in_lifeline == 1))
  ) {
    zaf_transport_tx(
      report_out_frame, report_out_size, NULL, &tx_options);
  }

  if (notify_lifeline) {
    ZAF_TSE_Trigger(send_report_tse, p_rx_options, false);
  }
}

void CC_UserCredential_CredentialReport_tx(
  const u3c_credential_report_type_t report_type,
  const u3c_credential * const p_credential,
  const u3c_credential_type next_credential_type,
  const uint16_t next_credential_slot,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  uint8_t * p_frame_start = report_out_frame;
  uint8_t * p_frame = p_frame_start;

  /**
   * CC:0083.01.0C.11.021: If the credential can be read back (Maximum
   * Credential Hash Length 0 in the Credential Capabilities Report Command),
   * the CRB flag MUST be set to 1.
   */
  bool crb =
    (p_credential->metadata.type != CREDENTIAL_TYPE_NONE)
    && (cc_user_credential_get_max_hash_length(p_credential->metadata.type) == 0);

  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = CREDENTIAL_REPORT;
  *p_frame++ = (uint8_t)report_type;
  *p_frame++ = (uint8_t)(p_credential->metadata.uuid >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.uuid; // LSB
  *p_frame++ = (uint8_t)p_credential->metadata.type;
  *p_frame++ = (uint8_t)(p_credential->metadata.slot >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.slot; // LSB
  *p_frame++ = crb ? CREDENTIAL_REPORT_PROPERTIES1_CRB_BIT_MASK : 0; // CRB
  *p_frame++ = p_credential->metadata.length;
  memcpy(p_frame, p_credential->data, p_credential->metadata.length);
  p_frame += p_credential->metadata.length;
  *p_frame++ = (uint8_t)p_credential->metadata.modifier_type;
  *p_frame++ = (uint8_t)(p_credential->metadata.modifier_node_id >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.modifier_node_id; // LSB
  *p_frame++ = (uint8_t)next_credential_type;
  *p_frame++ = (uint8_t)(next_credential_slot >> 8); // MSB
  *p_frame++ = (uint8_t)next_credential_slot; // LSB
  report_out_size = p_frame - p_frame_start;

  /**
   * Determine whether nodes in the the Lifeline association group must be
   * notified.
   * This only applies to successful database modifications.
   */
  bool notify_lifeline =
    (report_type == CREDENTIAL_REP_TYPE_ADDED)
    || (report_type == CREDENTIAL_REP_TYPE_MODIFIED)
    || (report_type == CREDENTIAL_REP_TYPE_DELETED);

  send_report(p_rx_options, notify_lifeline);
}

static void CC_UserCredential_AllUsersChecksumReport_tx(
  uint16_t checksum,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  ZW_APPLICATION_TX_BUFFER buffer;
  uint8_t * p_frame_start = (uint8_t *)&buffer;
  uint8_t * p_frame = p_frame_start;
  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = ALL_USERS_CHECKSUM_REPORT;
  *p_frame++ = (uint8_t)(checksum >> 8);
  *p_frame++ = (uint8_t)checksum;

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx(p_frame_start, p_frame - p_frame_start, NULL, &tx_options);
  return;
}

static void CC_UserCredential_UserChecksumReport_tx(
  uint16_t uuid,
  uint16_t checksum,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  ZW_APPLICATION_TX_BUFFER buffer;
  uint8_t * p_frame_start = (uint8_t *)&buffer;
  uint8_t * p_frame = p_frame_start;
  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = USER_CHECKSUM_REPORT;
  *p_frame++ = (uint8_t)(uuid >> 8);
  *p_frame++ = (uint8_t)uuid;
  *p_frame++ = (uint8_t)(checksum >> 8);
  *p_frame++ = (uint8_t)checksum;

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx(p_frame_start, p_frame - p_frame_start, NULL, &tx_options);
  return;
}

static void CC_UserCredential_CredentialCheckusumReport_tx(
  u3c_credential_type type,
  uint16_t checksum,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options)
{
  ZW_APPLICATION_TX_BUFFER buffer;
  uint8_t * p_frame_start = (uint8_t *)&buffer;
  uint8_t * p_frame = p_frame_start;
  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = CREDENTIAL_CHECKSUM_REPORT;
  *p_frame++ = (uint8_t)type;
  *p_frame++ = (uint8_t)(checksum >> 8);
  *p_frame++ = (uint8_t)checksum;

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx(p_frame_start, p_frame - p_frame_start, NULL, &tx_options);
  return;
}

/**
 * @brief Translate admin code data to Report command and send 
 * to the original source per the RX options. 
 * 
 * @param data pointer to Admin Code metadata retrieved from the database. 
 * @param rx_options Information about the source of the original command.
 */
static void CC_UserCredential_AdminCodeReport_tx(
  u3c_admin_code_metadata_t *data, 
  RECEIVE_OPTIONS_TYPE_EX *rx_options)
{
  ZW_ADMIN_PIN_CODE_REPORT_1BYTE_FRAME * tx_frame =
    (ZW_ADMIN_PIN_CODE_REPORT_1BYTE_FRAME *)&report_out_frame;

  // Populate frame with data from metadata structure
  tx_frame->cmdClass = COMMAND_CLASS_USER_CREDENTIAL; 
  tx_frame->cmd = ADMIN_PIN_CODE_REPORT;
  tx_frame->properties1 = (uint8_t)(((data->result) << 4) & 0xF0) | (data->code_length & 0x0F); // Code length has to be less than 10 anyway 
  if (data->code_data && data->code_length) {
    memcpy((void*)&tx_frame->adminCode1, data->code_data, data->code_length); 
  } else if (!data->code_length) {
    memset((void*)&tx_frame->adminCode1, 0x00, AC_MAX_LENGTH); 
  }
  report_out_size =
    sizeof(ZW_ADMIN_PIN_CODE_REPORT_1BYTE_FRAME) - 1 + data->code_length;

  // CC:0083.01.1A.11.001: Notify the Lifeline on a succesful modification
  bool notify_lifeline = (data->result == ADMIN_CODE_OPERATION_RESULT_MODIFIED);
  send_report(rx_options, notify_lifeline);
}

// TODO: Move this function outside of the CC.
bool CC_UserCredential_CredentialGet_handler(
  cc_handler_input_t * input,
  u3c_credential_metadata * p_metadata,
  u3c_credential_report_tx_callback_t callback
  )
{
  /**
   * CC:0083.01.0B.11.006 Other combinations of zero and non-zero User Unique Identifier, 
   * Credential Type, and Credential Slot are considered invalid and MUST be ignored.
   * CC:0083.01.0C.11.002 - Table
   */
  if (p_metadata->type == CREDENTIAL_TYPE_NONE && p_metadata->slot != 0) {
    return false;
  }

  uint8_t data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };
  u3c_credential credential = {
    .metadata = {
      .length = 0,
      .modifier_node_id = 0,
      .modifier_type = MODIFIER_TYPE_DNE,
      .slot = p_metadata->slot,
      .type = p_metadata->type,
      .uuid = p_metadata->uuid
    },
    .data = data
  };
  u3c_credential_type next_credential_type = CREDENTIAL_TYPE_NONE;
  uint16_t next_credential_slot = 0;
  bool credential_found = false;

  if (p_metadata->slot == 0) {
    // Fetch the first available Credential's ID
    CC_UserCredential_get_next_credential(
      p_metadata->uuid, p_metadata->type, 0,
      &p_metadata->type, &p_metadata->slot);
  }

  u3c_db_operation_result get_result = CC_UserCredential_get_credential(
    p_metadata->uuid, p_metadata->type, p_metadata->slot,
    &credential.metadata, credential.data);

  switch (get_result) {
    case U3C_DB_OPERATION_RESULT_SUCCESS: {
      credential_found = true;

      __attribute__ ((fallthrough));
    }
    default: {
      /**
       * CC:0083.01.0C.11.016, CC:0083.01.0C.11.017
       * Query the next credential based on the incoming request if the
       * requested credential does not exist.
       * Otherwise, prefer the ID returned by the database. This will be
       * different from the request if that was for the first available
       * credential (slot number == 0).
       */
      CC_UserCredential_get_next_credential(
        p_metadata->uuid,
        credential_found ? credential.metadata.type : p_metadata->type,
        credential_found ? credential.metadata.slot : p_metadata->slot,
        &next_credential_type, &next_credential_slot);

      callback(
        CREDENTIAL_REP_TYPE_RESPONSE_TO_GET, &credential, next_credential_type,
        next_credential_slot, input->rx_options);
      break;
    }
  }
  return credential_found;
}

static received_frame_status_t CC_UserCredential_CredentialGet_parser(
  cc_handler_input_t * input)
{
  u3c_credential_metadata metadata = {
    .uuid = (input->frame->ZW_CredentialGetFrame.userUniqueIdentifier1 << 8)
            | input->frame->ZW_CredentialGetFrame.userUniqueIdentifier2,
    .length = 0,
    .modifier_type = MODIFIER_TYPE_UNKNOWN,
    .modifier_node_id = 0,
    .type = input->frame->ZW_CredentialGetFrame.credentialType,
    .slot = (input->frame->ZW_CredentialGetFrame.credentialSlot1 << 8)
            | input->frame->ZW_CredentialGetFrame.credentialSlot2
  };

  bool result = CC_UserCredential_CredentialGet_handler(input, &metadata, CC_UserCredential_CredentialReport_tx);

  return (true == result) ? RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
}

static void CC_UserCredential_CredentialLearnStatusReport_tx(
  const u3c_credential_learn_status status,
  const u3c_credential_identifier * const p_target,
  const uint8_t steps_remaining,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options)
{
  const bool is_status_valid = status <= CL_STATUS_STEP_RETRY
                               || (status >= CL_STATUS_INVALID_ADD_OPERATION_TYPE);
  if (!is_status_valid || !p_target || !p_rx_options) {
    assert(false);
    return;
  }

  if (is_rx_frame_initiated_locally(p_rx_options)) {
    fill_rx_frame_with_local(p_rx_options);
  }

  ZW_APPLICATION_TX_BUFFER tx_buffer;
  ZW_CREDENTIAL_LEARN_REPORT_FRAME * p_frame = &tx_buffer.ZW_CredentialLearnReportFrame;

  p_frame->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  p_frame->cmd = CREDENTIAL_LEARN_REPORT;
  p_frame->credentialLearnStatus = status;
  p_frame->userUniqueIdentifier1 = p_target->uuid >> 8;
  p_frame->userUniqueIdentifier2 = p_target->uuid & 0xFF;
  p_frame->credentialType = p_target->type;
  p_frame->credentialSlot1 = p_target->slot >> 8;
  p_frame->credentialSlot2 = p_target->slot & 0xFF;
  p_frame->credentialLearnStepsRemaining = steps_remaining;

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx((uint8_t *)&tx_buffer, sizeof(ZW_CREDENTIAL_LEARN_REPORT_FRAME),
                   NULL, &tx_options);
}

static void credential_learn_reset()
{
  TimerStop(&cl_timeout_timer);
  if (credential_learn_power_lock) {
    zpal_pm_cancel(credential_learn_power_lock);
  }
  memset(&cl_state, 0, sizeof(cl_state));
}

static received_frame_status_t CC_UserCredential_CredentialLearnStart_handler(
  const u3c_operation_type_t operation_type,
  const u3c_credential_identifier * const p_learn_target,
  const uint8_t timeout_seconds,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options)
{
  if (!p_learn_target || !p_rx_options || operation_type == U3C_OPERATION_TYPE_DELETE) {
    assert(false);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if (is_credential_learn_in_progress()) {
    CC_UserCredential_CredentialLearnStatusReport_tx(
      CL_STATUS_ALREADY_IN_PROGRESS, p_learn_target, cl_state.remaining_steps, p_rx_options
    );
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Validate that the metadata is correct
  u3c_credential_metadata metadata = {
    .uuid = p_learn_target->uuid,
    .type = p_learn_target->type,
    .slot = p_learn_target->slot
  };

  if (
    !validate_associated_uuid(p_learn_target->uuid)
    || !validate_new_credential_metadata(&metadata)
    // CC:0083.01.0F.11.004
    || !cc_user_credential_is_credential_learn_supported(metadata.type)
  ) {
    zaf_event_distributor_enqueue_cc_event(
      COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_FAILED, NULL);
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  /**
   * Check whether the requested Add or Modify operation is consistent with the
   * current state of the database
   */
  u3c_db_operation_result result = CC_UserCredential_get_credential(
    metadata.uuid, metadata.type, metadata.slot,
    &metadata, NULL);
  bool is_consistent_with_database = true;
  u3c_credential_learn_status status = CL_STATUS_STARTED;
  switch (operation_type) {
    case U3C_OPERATION_TYPE_ADD:
      if (result != U3C_DB_OPERATION_RESULT_FAIL_DNE) {
        status = CL_STATUS_INVALID_ADD_OPERATION_TYPE;
        is_consistent_with_database = false;
      }
      break;
    case U3C_OPERATION_TYPE_MODIFY:
      if (
        result != U3C_DB_OPERATION_RESULT_SUCCESS
        /**
         * The UUID must not be changed via this command.
         * User Credential Association Set should be used instead.
         */
        || metadata.uuid != p_learn_target->uuid
      ) {
        status = CL_STATUS_INVALID_MODIFY_OPERATION_TYPE;
        is_consistent_with_database = false;
      }
      break;
    default:
      assert(false); // Should not be possible due to earlier checks
      return RECEIVED_FRAME_STATUS_FAIL;
  }
  if (!is_consistent_with_database) {
    zaf_event_distributor_enqueue_cc_event(
      COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_FAILED, NULL);
    CC_UserCredential_CredentialLearnStatusReport_tx(
      status, p_learn_target, 0, p_rx_options
    );
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Store details of current Credential Learn operation
  memcpy(&cl_state.data.target, p_learn_target, sizeof(cl_state.data.target));
  memcpy(&cl_state.rx_options, p_rx_options, sizeof(RECEIVE_OPTIONS_TYPE_EX));
  cl_state.data.operation_type = operation_type;
  // If no Credential Learn Timeout value is provided, use the recommended value
  cl_state.data.timeout_seconds = timeout_seconds
                             ? timeout_seconds
                             : cc_user_credential_get_cl_recommended_timeout(p_learn_target->type);
  cl_state.data.source_id = (uint16_t)p_rx_options->sourceNode.nodeId;
  cl_state.in_progress = true;

  // Signal start of process to application
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_START, &cl_state.data
  );

  return RECEIVED_FRAME_STATUS_SUCCESS;
};

static received_frame_status_t CC_UserCredential_CredentialLearnStart_parser(
  cc_handler_input_t * input)
{
  ZW_CREDENTIAL_LEARN_START_FRAME * pFrameIn = &input->frame->ZW_CredentialLearnStartFrame;

  // Extract Credential data from incoming frame
  u3c_operation_type_t operation_type = pFrameIn->properties1 & CREDENTIAL_LEARN_START_PROPERTIES1_OPERATION_TYPE_MASK;
  uint8_t timeout_seconds = pFrameIn->credentialLearnTimeout;
  u3c_credential_identifier learn_target = {
    .uuid = (pFrameIn->userUniqueIdentifier1 << 8 | pFrameIn->userUniqueIdentifier2),
    .type = pFrameIn->credentialType,
    .slot = (pFrameIn->credentialSlot1 << 8 | pFrameIn->credentialSlot2),
  };

  if (
    learn_target.uuid == 0    // CC:0083.01.0F.11.005
    || learn_target.slot == 0 // CC:0083.01.0F.11.006
  ) {
    // Ignore the frame
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  return CC_UserCredential_CredentialLearnStart_handler(
    operation_type, &learn_target, timeout_seconds, input->rx_options
  );
}

static received_frame_status_t CC_UserCredential_CredentialLearnCancel_handler(
  cc_handler_input_t * input)
{
  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_FAIL;
  u3c_credential_learn_status cl_status = CL_STATUS_ALREADY_IN_PROGRESS;

  if (is_credential_learn_in_progress()) {
    // Only allow the initiating node to cancel the operation
    if (input->rx_options->sourceNode.nodeId == cl_state.rx_options.sourceNode.nodeId) {
      frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
      cl_status = CL_STATUS_ENDED_NOT_DUE_TO_TIMEOUT;
      zaf_event_distributor_enqueue_cc_event(
        COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_REMOTE, NULL
      );
      CC_UserCredential_CredentialLearnStatusReport_tx(
        cl_status, &cl_state.data.target, 0, &cl_state.rx_options
      );
      credential_learn_reset();
    }
  }

  return frame_status;
}

static received_frame_status_t CC_UserCredential_UserCredentialAssociationSet_handler(
  RECEIVE_OPTIONS_TYPE_EX * const p_rx_options,
  const u3c_credential_metadata * const source_metadata,
  const u3c_credential_metadata * const destination_metadata
)
{
  if (!p_rx_options || !source_metadata || !destination_metadata) {
    assert(false);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  u3c_db_operation_result operation_result =
      CC_UserCredential_move_credential_and_report(source_metadata->type,
      source_metadata->slot, destination_metadata->uuid, destination_metadata->slot, p_rx_options);

  return operation_result == U3C_DB_OPERATION_RESULT_SUCCESS ?
      RECEIVED_FRAME_STATUS_SUCCESS : RECEIVED_FRAME_STATUS_FAIL;
}

static received_frame_status_t CC_UserCredential_UserCredentialAssociationSet_parser(
  cc_handler_input_t const * const input)
{
  uint16_t source_slot = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.sourceCredentialSlot1 << 8) |
                                    input->frame->ZW_UserCredentialAssociationSetFrame.sourceCredentialSlot2;

  uint16_t destination_uuid = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.destinationUserUniqueIdentifier1 << 8) |
                                         input->frame->ZW_UserCredentialAssociationSetFrame.destinationUserUniqueIdentifier2;

  uint16_t destination_slot = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.destinationCredentialSlot1 << 8) |
                                         input->frame->ZW_UserCredentialAssociationSetFrame.destinationCredentialSlot2;

  u3c_credential_metadata source_metadata = {0};
  source_metadata.type = input->frame->ZW_UserCredentialAssociationSetFrame.credentialType;
  source_metadata.slot = source_slot;

  u3c_credential_metadata destination_metadata = {0};
  destination_metadata.uuid = destination_uuid;
  destination_metadata.type = source_metadata.type;
  destination_metadata.slot = destination_slot;

  bool parsing_success = false;
  u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_SUCCESS;

  // Configuration-related checks according to Table 2.546

  // Source Credential Type must be supported
  if (!cc_user_credential_is_credential_type_supported(source_metadata.type)) {
    status = U3C_UCAR_STATUS_CREDENTIAL_TYPE_INVALID;
  }
  // Source Credential Slot must be in supported range
  else if (source_slot == 0 || source_slot > cc_user_credential_get_max_credential_slots(source_metadata.type)) {
    status = U3C_UCAR_STATUS_SOURCE_CREDENTIAL_SLOT_INVALID;
  }
  // Destination User Unique Identifier must be in supported range
  else if (0 == destination_uuid || destination_uuid > cc_user_credential_get_max_user_unique_idenfitiers()) {
    status = U3C_UCAR_STATUS_DESTINATION_USER_UNIQUE_IDENTIFIER_INVALID;
  }
  // Check if the Destination Credential Slot must be in supported range
  else if (destination_slot == 0 || destination_slot > cc_user_credential_get_max_credential_slots(destination_metadata.type)) {
    status = U3C_UCAR_STATUS_DESTINATION_CREDENTIAL_SLOT_INVALID;
  } else {
    parsing_success = true;
  }

  if (!parsing_success) {
    CC_UserCredential_send_association_report(&source_metadata, &destination_metadata, status, input->rx_options);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  return CC_UserCredential_UserCredentialAssociationSet_handler(
    input->rx_options, &source_metadata, &destination_metadata
  );
}

static received_frame_status_t CC_UserCredential_AllUsersChecksumGet_handler(const cc_handler_input_t * input)
{
  /**
   * This command MUST be ignored by a node advertising no support for the All Users Checksum
   * functionality in the User Capabilities Report Command.
   * CC:0083.01.14.11.000
   */
  if (!cc_user_credential_is_all_users_checksum_supported()) {
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  u3c_user user = { 0 };
  uint8_t name[UINT8_MAX] = { 0 };
  uint8_t uuid_msb = 0;
  uint8_t uuid_lsb = 0;
  bool user_is_available = false;

  uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.15.11.000
  uint16_t user_uid = CC_UserCredential_get_next_user(0);

  while (user_uid) {
    user_is_available = true;
    if (CC_UserCredential_get_user(user_uid, &user, name) != U3C_DB_OPERATION_RESULT_SUCCESS) {
      // Driver error or database corruption
      return RECEIVED_FRAME_STATUS_FAIL;
    } else {
      /**
       * User Unique Identifier (16 bits) | User Type (8 bits) | User Active State (8 bits) |
       * Credential Rule (8 bits) | User Name Encoding (8 bits) | User Name Length (8 bits) |
       * User Name (User Name Length bytes)
       * CC:0083.01.15.11.001
       * CC:0083.01.15.11.002
       */
      uuid_msb = (user_uid >> 8);
      uuid_lsb = user_uid & 0xFF;
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&uuid_msb, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&uuid_lsb, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.type, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.active, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.credential_rule, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.name_encoding, 1);
      checksum = CRC_CheckCrc16(checksum, &user.name_length, 1);
      checksum = CRC_CheckCrc16(checksum, name, user.name_length);

      calculate_credentials_checksum_for_uuid(user_uid, &checksum);
    }
    user_uid = CC_UserCredential_get_next_user(user_uid);
  }

  /**
   * If there is no Users data (and thus no Credentials data) set at the node at all,
   * the checksum MUST be set to 0x0000.
   * CC:0083.01.15.11.006
   */
  checksum = user_is_available ? checksum : 0;

  /**
   * All Users Checksum Report command must be returned if this functionality is supported.
   * CC:0083.01.14.11.001
   */
  CC_UserCredential_AllUsersChecksumReport_tx(checksum, input->rx_options);

  return RECEIVED_FRAME_STATUS_SUCCESS;
}

static received_frame_status_t CC_UserCredential_UserChecksumGet_handler(const cc_handler_input_t * input)
{
  /**
   * This command MUST be ignored by a node advertising no support for the User Checksum functionality
   * in the User Capabilities Report Command.
   * CC:0083.01.16.11.000
   */
  if (!cc_user_credential_is_user_checksum_supported()) {
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  u3c_user user = { 0 };
  uint8_t name[UINT8_MAX] = { 0 };

  uint16_t uuid = input->frame->ZW_UserChecksumGetFrame.userUniqueIdentifier1 << 8
                  | input->frame->ZW_UserChecksumGetFrame.userUniqueIdentifier2;

  uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.17.11.000

  u3c_db_operation_result result = CC_UserCredential_get_user(uuid, &user, name);

  if (result == U3C_DB_OPERATION_RESULT_SUCCESS) {
    /**
     * User Type (8 bits) | User Active State (8 bits) | Credential Rule (8 bits) | User Name
     * Encoding (8 bits) | User Name Length (8 bits) | User Name (User Name Length bytes)
     * CC:0083.01.17.11.001
     * CC:0083.01.17.11.002
     */
    checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.type, 1);
    checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.active, 1);
    checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.credential_rule, 1);
    checksum = CRC_CheckCrc16(checksum, (uint8_t*)&user.name_encoding, 1);
    checksum = CRC_CheckCrc16(checksum, &user.name_length, 1); // CC:0083.01.17.11.004
    checksum = CRC_CheckCrc16(checksum, name, user.name_length);

    calculate_credentials_checksum_for_uuid(uuid, &checksum);
  } else if (result == U3C_DB_OPERATION_RESULT_FAIL_DNE) {
    /**
     * If there is no User data (and thus no Credentials data) set at the node for a User Unique Identifier,
     * the checksum MUST be set to 0x0000.
     * CC:0083.01.17.11.006
     */
    checksum = 0;
  } else {
    // Driver error or database corruption
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  /**
   * User Checksum Report command must be returned if this functionality is supported.
   * CC:0083.01.16.11.001
   */
  CC_UserCredential_UserChecksumReport_tx(uuid, checksum, input->rx_options);

  return RECEIVED_FRAME_STATUS_SUCCESS;
}

static received_frame_status_t CC_UserCredential_CredentialChecksumGet_handler(const cc_handler_input_t * input)
{
  /**
   * This command MUST be ignored by a node advertising no support for the Credential Checksum
   * functionality in the Credential Capabilities Report Command.
   * CC:0083.01.18.11.000
   */
  if (!cc_user_credential_is_credential_checksum_supported()) {
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }

  u3c_credential_type searched_type = input->frame->ZW_CredentialChecksumGetFrame.credentialType;
  uint16_t next_slot = 0;
  uint8_t next_slot_msb = 0;
  uint8_t next_slot_lsb = 0;
  u3c_credential_type next_type = CREDENTIAL_TYPE_NONE;
  uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.19.11.001
  bool credential_is_available = false;

  u3c_credential_metadata metadata = { 0 };
  uint8_t e_data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };

  while (CC_UserCredential_get_next_credential(0, searched_type, next_slot, &next_type, &next_slot)) {
    /**
     * Credential Slot (16 bits) | Credential Length (8 bits) |
     * Credential Data (Credential Length bytes)
     * CC:0083.01.19.11.002
     * CC:0083.01.19.11.003
     */
    credential_is_available = true;
    if (next_type != searched_type) {
      break;
    }

    if (CC_UserCredential_get_credential(0, searched_type, next_slot, &metadata, e_data) == U3C_DB_OPERATION_RESULT_SUCCESS) {
      next_slot_msb = next_slot >> 8;
      next_slot_lsb = next_slot & 0xFF;

      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&next_slot_msb, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&next_slot_lsb, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)&metadata.length, 1);
      checksum = CRC_CheckCrc16(checksum, (uint8_t*)e_data, metadata.length);
    } else {
      // Driver error or database corruption
      return RECEIVED_FRAME_STATUS_FAIL;
    }
  }

  /**
   * If there is no Credentials data set at the node for a Credential Type, the checksum MUST be
   * set to 0x0000.
   * CC:0083.01.19.11.006
   */
  checksum = credential_is_available ? checksum : 0;

  /**
   * Credential Checksum Report command must be returned if this functionality is supported.
   * CC:0083.01.18.11.001
   */
  CC_UserCredential_CredentialCheckusumReport_tx(searched_type, checksum, input->rx_options);

  return RECEIVED_FRAME_STATUS_SUCCESS;
}

static received_frame_status_t CC_UserCredential_AdminCodeSet_handler(u3c_admin_code_metadata_t *data, RECEIVE_OPTIONS_TYPE_EX *rx_options)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;  
  if (data->code_length >= AC_MIN_LENGTH && !validate_admin_pin_code(data)) {
    // Something went wrong - find out what. This will not run an IO operation and instead
    // will transmit a report.
    switch (data->result) {
      // Error cases
      case ADMIN_CODE_OPERATION_RESULT_INTERNAL_DUPLICATE_AC:
        data->result = ADMIN_CODE_OPERATION_RESULT_UNMODIFIED;
        break; 
      case ADMIN_CODE_OPERATION_RESULT_FAIL_DUPLICATE_CRED:
      case ADMIN_CODE_OPERATION_RESULT_FAIL_MANUF_RULE:
        break;  
      default: 
        return status; 
    }
  /**
   * If nothing goes wrong, run the IO operation.
   */
  } else {
    u3c_db_operation_result result = CC_UserCredential_set_admin_code(data);
    if (result != U3C_DB_OPERATION_RESULT_SUCCESS) {
      data->result = ADMIN_CODE_OPERATION_RESULT_ERROR_NODE; 
    } else {
      status = RECEIVED_FRAME_STATUS_SUCCESS; 
    }
  }
  /**
   * Result of the operation(s) trickles down to here. This contains 
   * the result within the metadata struct.
   */
  CC_UserCredential_AdminCodeReport_tx(data, rx_options); 
  return status; 
}

static received_frame_status_t CC_UserCredential_AdminCodeSet_parser(cc_handler_input_t *input)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL; 
  if (!cc_user_credential_get_admin_code_supported()) {
    /**
     * CC:0083.01.1A.13.002 - Ignore frame if AC not supported.
     */
    return RECEIVED_FRAME_STATUS_NO_SUPPORT;
  }
  /**
   * First, checks that the following requirements are met: 
   * CC:0083.01.1A.11.004 - Code is of appropriate length. 
   * CC:0083.01.1A.13.003 - If code is of length 0, ensures ACD is supported.
   * CC:0083.01.1A.11.008 - Code is ASCII encoded using characters '0' - '9'.   
   */ 
  u3c_admin_code_metadata_t data = {0};
  ZW_ADMIN_PIN_CODE_SET_1BYTE_FRAME *frame_in = &input->frame->ZW_AdminPinCodeSet1byteFrame;
  data.code_length = (frame_in->properties1 & 0x0F);
  data.code_data = (data.code_length == 0) ? NULL : &frame_in->adminCode1;  
  data.result = ADMIN_CODE_OPERATION_RESULT_INTERNAL_NONE; 

  if (data.code_length == 0 && !cc_user_credential_get_admin_code_deactivate_supported()) {
    /**
     * CC:0083.01.1A.13.003 - We MAY return a frame if ACD is not supported.
     * Length of 0 could be a mistake, worth reporting back to controller.
     */
    data.result = ADMIN_CODE_OPERATION_RESULT_ERROR_ACD_NOT_SUPPORTED; 
    status = RECEIVED_FRAME_STATUS_NO_SUPPORT; 
  } else if (data.code_length) {
    // Check valid length
    if (data.code_length < AC_MIN_LENGTH || data.code_length > AC_MAX_LENGTH) {
      /**
      *  CC:0083.01.1C.11.004 - length must be between 4 and 10 characters if length is not 0.
      */ 
      return status; 
    }
    /**
     * CC:0083.01.1A.11.010 - Check and ensure that if an Admin Code is provided, that the characters 
     * are valid. If they are not, ignore the command.
     * CC:0063.02.0E.11.006 - Each byte in this field MUST be encoded with ASCII representation.
     */
    for (int i = 0; i < data.code_length; i++) {
      if (data.code_data[i] > ASCII_AC_MAX) {
        return status;
      }
    }
  }

  if (data.result != ADMIN_CODE_OPERATION_RESULT_INTERNAL_NONE) {
    CC_UserCredential_AdminCodeReport_tx(&data, input->rx_options);
    return status;
  }
  return CC_UserCredential_AdminCodeSet_handler(&data, input->rx_options); 
}

static received_frame_status_t CC_UserCredential_AdminCodeGet_handler(cc_handler_input_t *input)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;
  /**
   * CC:0083.01.1B.13.001 - if AC is unsupported, command may be ignored.
   */
  if (cc_user_credential_get_admin_code_supported()) {
    /**
     * Result code will come from application layer handler with the exception 
     * of an unspecified node error. 
     */
    u3c_admin_code_metadata_t data = { 0 };
    uint8_t temp_code[AC_MAX_LENGTH] = { 0 }; 
    data.code_data = (uint8_t*)temp_code; 
    
    if (CC_UserCredential_get_admin_code_info(&data) != U3C_DB_OPERATION_RESULT_SUCCESS) {
      data.result = ADMIN_CODE_OPERATION_RESULT_ERROR_NODE; 
    }
    CC_UserCredential_AdminCodeReport_tx(&data, input->rx_options);  
    status = RECEIVED_FRAME_STATUS_SUCCESS; 
  } else {
    status = RECEIVED_FRAME_STATUS_NO_SUPPORT; 
  }

  return status; 
}

static received_frame_status_t CC_UserCredential_handler(
  cc_handler_input_t * input,
  cc_handler_output_t * output)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;
  switch (input->frame->ZW_Common.cmd) {
    case USER_CAPABILITIES_GET:
      CC_UserCredential_UserCapabilitiesGet_handler(output);
      output->length = sizeof(ZW_USER_CAPABILITIES_REPORT_2BYTE_FRAME);
      status = RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case CREDENTIAL_CAPABILITIES_GET:
      CC_UserCredential_CredentialCapabilitiesGet_handler(output);
      status = RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case USER_SET:
      status = CC_UserCredential_UserSet_handler(input);
      break;

    case USER_GET:
      status = CC_UserCredential_UserGet_handler(input);
      break;

    case CREDENTIAL_SET:
      status = CC_UserCredential_CredentialSet_parser(input);
      break;

    case CREDENTIAL_GET:
      status = CC_UserCredential_CredentialGet_parser(input);
      break;

    case CREDENTIAL_LEARN_START:
      status = CC_UserCredential_CredentialLearnStart_parser(input);
      break;

    case CREDENTIAL_LEARN_CANCEL:
      status = CC_UserCredential_CredentialLearnCancel_handler(input);
      break;

    case USER_CREDENTIAL_ASSOCIATION_SET:
      status = CC_UserCredential_UserCredentialAssociationSet_parser(input);
      break;

    case ALL_USERS_CHECKSUM_GET:
      status = CC_UserCredential_AllUsersChecksumGet_handler(input);
      break;

    case USER_CHECKSUM_GET:
      status = CC_UserCredential_UserChecksumGet_handler(input);
      break;
  
    case CREDENTIAL_CHECKSUM_GET:
      status = CC_UserCredential_CredentialChecksumGet_handler(input);
      break;

    case ADMIN_PIN_CODE_GET:
      status = CC_UserCredential_AdminCodeGet_handler(input); 
      break; 
    
    case ADMIN_PIN_CODE_SET: 
      status = CC_UserCredential_AdminCodeSet_parser(input);
      break; 

    default:
      // Unknown command
      status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
      break;
  }
  return status;
}

static uint8_t lifeline_reporting(ccc_pair_t * p_ccc_pair)
{
  p_ccc_pair->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  p_ccc_pair->cmd      = USER_REPORT;
  ++p_ccc_pair;
  p_ccc_pair->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  p_ccc_pair->cmd      = CREDENTIAL_REPORT;
  ++p_ccc_pair;
  p_ccc_pair->cmdClass = COMMAND_CLASS_USER_CREDENTIAL; 
  p_ccc_pair->cmd      = ADMIN_PIN_CODE_REPORT; 
  return 3;
}

static void credential_learn_timer_timeout_callback(__attribute__((unused)) SSwTimer *pTimer)
{
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_TIMEOUT, NULL
  );
  // CC:0083.01.0F.11.013
  CC_UserCredential_CredentialLearnStatusReport_tx(
    CL_STATUS_TIMEOUT, &cl_state.data.target, 0, &cl_state.rx_options
  );
  // CC:0083.01.0F.11.013
  credential_learn_reset();
}

REGISTER_CC_V5(COMMAND_CLASS_USER_CREDENTIAL, USER_CREDENTIAL_VERSION,
               CC_UserCredential_handler, NULL, NULL, lifeline_reporting, 0,
               init, reset);

static void
user_credential_event_handler(const uint8_t event, const void * p_data)
{
  switch (event) {
    case CC_USER_CREDENTIAL_EVENT_VALIDATE: {
      const u3c_event_data_validate * const p_data_validate = (u3c_event_data_validate *) p_data;
      uint8_t notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_NO_EVENT;
      uint8_t event_out = CC_USER_CREDENTIAL_EVENT_VALIDATE_INVALID;
      u3c_credential_metadata stored_credential = { 0 }; // Metadata of the existing credential in the database

      if (!find_existing_credential(p_data_validate->credential, &stored_credential)) {
        // The Credential does not exist in the database
        notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_INVALID_CREDENTIAL_USED_TO_ACCESS_THE_NODE;
        CC_Notification_TriggerAndTransmit(
          0, notification_event, NULL, 0, NULL, false);
      } else {
        // The Credential exists in the database, perform additional checks
        u3c_user current_user = { 0 };
        CC_UserCredential_get_user(stored_credential.uuid, &current_user, NULL);

        if (current_user.type == USER_TYPE_NON_ACCESS) {
          // Non-access Users may not operate the lock, but a report should be generated
          notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_NON_ACCESS_USER_ENTERED;
        } else {
          if (!current_user.active) {
            // Users with the "Occupied Disabled" state may not be granted access
            notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_ACCESS_DENIED_OCCUPIED_DISABLED;
          } else {
            // Grant access
            event_out = CC_USER_CREDENTIAL_EVENT_VALIDATE_VALID;
            notification_event = p_data_validate->is_unlocked
                                 ? NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_LOCK_CLOSE_OPERATION
                                 : NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_UNLOCK_OPEN_OPERATION;
          }
        }

        CC_User_Credential_UsageNotification_tx(
          notification_event, stored_credential.uuid, 1, &stored_credential
        );
      }

      zaf_event_distributor_enqueue_cc_event(
        COMMAND_CLASS_USER_CREDENTIAL, event_out, NULL);

      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_START_REQUEST: {
      /**
       * The application requests to start the Credential Learn process.
       * Proceed only if the node is already included.
       */
      if (ZAF_GetInclusionState() != EINCLUSIONSTATE_EXCLUDED) {
        const u3c_credential_learn_event_data * const p_data_learn_start = (const u3c_credential_learn_event_data *) p_data;
        RECEIVE_OPTIONS_TYPE_EX rx_options = { 0 };
        CC_UserCredential_CredentialLearnStart_handler(
          p_data_learn_start->operation_type, &p_data_learn_start->target,
          p_data_learn_start->timeout_seconds, &rx_options);
      }
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_STEP_START: {
      uint8_t * remaining_steps = (uint8_t *) p_data;
      cl_state.remaining_steps = *remaining_steps;
      // Start or restart timeout timer for this step
      TimerStart(&cl_timeout_timer, cl_state.data.timeout_seconds * S_TO_MS);
      /**
       * Keep device awake for the duration of this step.
       * This is necessary to ensure that the final reports will be sent out correctly.
       */
      if (credential_learn_power_lock) {
        zpal_pm_stay_awake(credential_learn_power_lock, cl_state.data.timeout_seconds * S_TO_MS);
      }
      CC_UserCredential_CredentialLearnStatusReport_tx(
        CL_STATUS_STARTED, &cl_state.data.target, *remaining_steps, &cl_state.rx_options
      );
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_READ_DONE: {
      u3c_event_data_learn_read_done * p_event_data = (u3c_event_data_learn_read_done *) p_data;
      uint8_t data_length = p_event_data->data_length;
      uint8_t * data = p_event_data->data;

      TimerStop(&cl_timeout_timer);
      if (credential_learn_power_lock) {
        zpal_pm_cancel(credential_learn_power_lock);
      }

      /**
       * Silently fail if the payload is missing or the process was not already
       * initiated
       */
      if (!data || !data_length || !is_credential_learn_in_progress()) {
        credential_learn_reset();
        break;
      }

      u3c_credential credential = {
        .metadata = {
          .uuid = cl_state.data.target.uuid,
          .type = cl_state.data.target.type,
          .slot = cl_state.data.target.slot,
          .length = data_length,
          .modifier_node_id = 0,
          .modifier_type = MODIFIER_TYPE_LOCALLY,
        },
        .data = data
      };
      u3c_db_operation_result db_op_result = U3C_DB_OPERATION_RESULT_ERROR;

      /**
       * CC:0083.01.0F.11.002: validate_new_credential_data() will send the
       * required Credential Set Error Report to the initiator upon failure
       */
      if (validate_new_credential_data(&credential, &cl_state.rx_options)) {
        // Write Credential into database
        switch (cl_state.data.operation_type) {
          case U3C_OPERATION_TYPE_ADD:
            db_op_result = CC_UserCredential_add_credential(&credential);
            break;
          case U3C_OPERATION_TYPE_MODIFY:
            db_op_result = CC_UserCredential_modify_credential(&credential);
            break;
          default:
            break;
        }
      }

      uint8_t event_out = CC_USER_CREDENTIAL_EVENT_LEARN_FAILED;
      u3c_credential_learn_status status_out = CL_STATUS_ENDED_NOT_DUE_TO_TIMEOUT;
      switch (db_op_result) {
        case U3C_DB_OPERATION_RESULT_SUCCESS:
          event_out = CC_USER_CREDENTIAL_EVENT_LEARN_SUCCESS;
          status_out = CL_STATUS_SUCCESS;
          break;
        default:
          DPRINTF("Error: Credential Learn failed due to database error (%d)\n", db_op_result);
          break;
      }

      // Notify application
      zaf_event_distributor_enqueue_cc_event(
        COMMAND_CLASS_USER_CREDENTIAL, event_out, NULL);

      // CC:0083:01.0F.11.000: Notify initiating node
      CC_UserCredential_CredentialLearnStatusReport_tx(
        status_out, &cl_state.data.target, 0, &cl_state.rx_options
      );

      // CC:0083.01.0F.11.001: Send Credential Report of new Credential
      if (db_op_result == U3C_DB_OPERATION_RESULT_SUCCESS) {
        u3c_credential_report_type_t report_type = CREDENTIAL_REP_TYPE_UNCHANGED;
        switch (cl_state.data.operation_type) {
          case U3C_OPERATION_TYPE_ADD:
            report_type = CREDENTIAL_REP_TYPE_ADDED;
            break;
          case U3C_OPERATION_TYPE_MODIFY:
            report_type = CREDENTIAL_REP_TYPE_MODIFIED;
            break;
          default:
            assert(false); // Invalid database operation type!
            break;
        }

        // Get next credential's details
        u3c_credential_type next_credential_type = CREDENTIAL_TYPE_NONE;
        uint16_t next_credential_slot = 0;
        CC_UserCredential_get_next_credential(
          0, credential.metadata.type, credential.metadata.slot,
          &next_credential_type, &next_credential_slot);

        CC_UserCredential_CredentialReport_tx(
          report_type, &credential, next_credential_type, next_credential_slot,
          &cl_state.rx_options
        );
      }

      credential_learn_reset();

      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_STEP_RETRY: {
      if (is_credential_learn_in_progress()) {
        uint8_t * remaining_steps = (uint8_t *) p_data;
        TimerRestart(&cl_timeout_timer);
        if (credential_learn_power_lock) {
          zpal_pm_stay_awake(credential_learn_power_lock, cl_state.data.timeout_seconds * S_TO_MS);
        }
        CC_UserCredential_CredentialLearnStatusReport_tx(
          CL_STATUS_STEP_RETRY, &cl_state.data.target, *remaining_steps, &cl_state.rx_options);
      }
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_LOCAL: {
      if (is_credential_learn_in_progress()) {
        CC_UserCredential_CredentialLearnStatusReport_tx(
          CL_STATUS_ENDED_NOT_DUE_TO_TIMEOUT, &cl_state.data.target, 0, &cl_state.rx_options);
        credential_learn_reset();
      }
      break;
    }
  }
}

ZAF_EVENT_DISTRIBUTOR_REGISTER_CC_EVENT_HANDLER(COMMAND_CLASS_USER_CREDENTIAL, user_credential_event_handler);
