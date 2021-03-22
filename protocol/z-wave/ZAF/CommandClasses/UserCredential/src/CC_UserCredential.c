/**
 * @file
 * @brief Handler for Command Class User Credential.
 * @copyright 2023 Silicon Laboratories Inc.
 */

#include "CC_UserCredential.h"
#include "cc_user_credential_config_api.h"
#include "cc_user_credential_io_config.h"
#include "cc_user_credential_io.h"
#include "CC_Notification.h"
#include "zaf_transport_tx.h"
#include "zaf_event_distributor_soc.h"
#include <string.h>
#include "assert.h"
#include "AppTimer.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "ZAF_Common_interface.h"
#include "CRC.h" // CC:0083.01.15.11.000 & CC:0083.01.17.11.000 & CC:0083.01.19.11.001

/// Longest possible default user name: "User-65535", times 2 for UTF-16
#define DEFAULT_NAME_MAX_SIZE (sizeof("User-65535") - 1) * 2

#define CREDENTIAL_NOTIFICATION_REPORT_MAX_SIZE \
  TX_DATA_MAX_DATA_SIZE - sizeof(ZW_CREDENTIAL_REPORT_1BYTE_FRAME) + 1
#define CREDENTIAL_SET_ERROR_REPORT_MAX_SIZE \
  TX_DATA_MAX_DATA_SIZE - sizeof(ZW_CREDENTIAL_SET_ERROR_REPORT_1BYTE_FRAME) + 1

// Size of a Capabilities Report Variant Group for a single Credential Type
#define CREDENTIAL_CAPABILITIES_REPORT_VG_SIZE ( \
  sizeof(uint8_t)   /**< Credential Type */\
  + sizeof(uint8_t)  /**< Properties1 */\
  + sizeof(uint16_t) /**< Credential Slots */\
  + sizeof(uint16_t) /**< Minimum Length of Data */\
  + sizeof(uint16_t) /**< Maximum Length of Data */\
  + sizeof(uint8_t)  /**< Credential Learn Recommended Timeout */\
  + sizeof(uint8_t)  /**< Credential Learn Number of Steps */\
)
#define S_TO_MS 1000 // Seconds to milliseconds, used for timers

/**
 * Length of the Event/State Parameters data in the User Notification Report
 * frame without the Command Class and Command fields
 */
#define USER_NOTIFICATION_EV_PAR_LENGTH \
  sizeof(ZW_USER_NOTIFICATION_REPORT_FRAME) - \
  offsetof(ZW_USER_NOTIFICATION_REPORT_FRAME, userModifierType)

// State and timer for the current Credential Learn operation
typedef struct credential_learn_state_ {
  bool in_progress;
  u3c_operation_type_t operation_type;
  RECEIVE_OPTIONS_TYPE_EX rx_options;
  uint8_t timeout_seconds;
  uint8_t total_steps;
  uint8_t remaining_steps;
} credential_learn_state;
static credential_learn_state cl_state;
u3c_credential_identifier cl_target;
static SSwTimer cl_timeout_timer;

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
 * Fills the Event parameters for a Notification Report frame based on an
 * incoming User Set frame.
 *
 * @param[in] pFrameIn Pointer to the incoming frame
 * @param[in] rxOptions Properties of the incoming frame
 * @param[out] pEventParameters Pointer to the Event parameters
 */
static void user_event_parameters_from_frame(
  const ZW_USER_SET_1BYTE_FRAME * pFrameIn,
  const RECEIVE_OPTIONS_TYPE_EX * rxOptions,
  ZW_USER_NOTIFICATION_REPORT_FRAME * pEventParameters)
{
  if (!pFrameIn || !rxOptions || !pEventParameters) {
    assert(false);
    return;
  }

  pEventParameters->userModifierType = MODIFIER_TYPE_Z_WAVE;
  pEventParameters->userModifierNodeId1 = rxOptions->sourceNode.nodeId >> 8;
  pEventParameters->userModifierNodeId2 = rxOptions->sourceNode.nodeId & 0xFF;
  pEventParameters->userUniqueIdentifier1 = pFrameIn->userUniqueIdentifier1;
  pEventParameters->userUniqueIdentifier2 = pFrameIn->userUniqueIdentifier2;
  pEventParameters->userType = pFrameIn->userType;
  pEventParameters->properties1 = pFrameIn->properties2; // Bit 0: User Active State
  pEventParameters->credentialRule = pFrameIn->credentialRule;
  pEventParameters->expiringTimeoutMinutes1 = pFrameIn->expiringTimeoutMinutes1;
  pEventParameters->expiringTimeoutMinutes2 = pFrameIn->expiringTimeoutMinutes2;
}

/**
 * Fills the Event parameters for a Notification Report frame based on an
 * existing User
 *
 * @param[in] pUser Pointer to the User
 * @param[out] pEventParameters Pointer to the Event parameters
 */
static void user_event_parameters_from_user(
  const u3c_user * pUser, ZW_USER_NOTIFICATION_REPORT_FRAME * pEventParameters)
{
  if (!pUser || !pEventParameters) {
    assert(false);
    return;
  }

  pEventParameters->userModifierType = pUser->modifier_type;
  pEventParameters->userModifierNodeId1 = pUser->modifier_node_id >> 8;
  pEventParameters->userModifierNodeId2 = pUser->modifier_node_id & 0xFF;
  pEventParameters->userUniqueIdentifier1 = pUser->unique_identifier >> 8;
  pEventParameters->userUniqueIdentifier2 = pUser->unique_identifier & 0xFF;
  pEventParameters->userType = pUser->type;
  pEventParameters->properties1 = pUser->active & USER_NOTIFICATION_REPORT_PROPERTIES1_USER_ACTIVE_STATE_BIT_MASK; // Bit 0: User Active State
  pEventParameters->credentialRule = pUser->credential_rule;
  pEventParameters->expiringTimeoutMinutes1 = pUser->expiring_timeout_minutes >> 8;
  pEventParameters->expiringTimeoutMinutes2 = pUser->expiring_timeout_minutes & 0xFF;
}

/**
 * Determines whether the Credential Read Back feature should be used for a
 * given Credential Type
 *
 * @param[in] type Credential Type
 * @return Whether CRB is to be used
 */
static bool should_use_crb(u3c_credential_type type)
{
  // CC:0083.01.0C.12.002: Non-Biometric Credential Types SHOULD be reported by the node.
  bool is_not_biometric = (type > CREDENTIAL_TYPE_NONE)
                          && (type < CREDENTIAL_TYPE_EYE_BIOMETRIC);
  return is_not_biometric;
}

/**
 * Fills the Event parameters for a Notification Report frame based on a
 * Credential metadata struct.
 *
 * @param[in] pCredential Pointer to the Credential metadata
 * @param[out] p_frame Pointer to the Event parameters
 */
static void CC_UserCredential_CredentialNotificationReport_tx(
  const uint8_t notificationEvent, const u3c_credential * const pCredential)
{
  if (!pCredential) {
    assert(false);
    return;
  }

  uint8_t event_parameters[CREDENTIAL_NOTIFICATION_REPORT_MAX_SIZE] = { 0 };
  uint8_t * p_frame = event_parameters;
  const bool multiple_deleted =
    notificationEvent == NOTIFICATION_EVENT_ACCESS_CONTROL_MULTIPLE_CREDENTIALS_DELETED;
  uint8_t data_length = 0;
  // Set the length of the Credential Data if it fits in the frame
  if ((sizeof(ZW_CREDENTIAL_NOTIFICATION_REPORT_1BYTE_FRAME) - 1 + pCredential->metadata.length)
      <= CREDENTIAL_NOTIFICATION_REPORT_MAX_SIZE) {
    data_length = pCredential->metadata.length;
  }
  const bool credential_read_back =
    data_length > 0
    && !multiple_deleted
    && should_use_crb(pCredential->metadata.type);

  *p_frame++ = pCredential->metadata.uuid >> 8; // User Unique Identifer (MSB)
  *p_frame++ = pCredential->metadata.uuid & 0xFF; // User Unique Identifier (LSB)
  *p_frame++ = pCredential->metadata.type; // Credential Type
  *p_frame++ = pCredential->metadata.slot >> 8; // Credential Slot (MSB)
  *p_frame++ = pCredential->metadata.slot & 0xFF; // Credential Slot (LSB)
  *p_frame++ = (credential_read_back << 7) & CREDENTIAL_REPORT_PROPERTIES1_CRB_BIT_MASK; // CRB
  *p_frame++ = credential_read_back ? data_length : 0; // Credential Length
  if (credential_read_back) {
    memcpy(p_frame, pCredential->data, data_length); // Credential Data
    p_frame += data_length;
  }
  if (multiple_deleted) {
    *p_frame++ = MODIFIER_TYPE_DNE; // Credential Modifier Type
    *p_frame++ = 0; // Credential Modifier Node ID (MSB)
    *p_frame++ = 0; // Credential Modifier Node ID (LSB)
  } else {
    *p_frame++ = pCredential->metadata.modifier_type; // Credential Modifier Type
    *p_frame++ = pCredential->metadata.modifier_node_id >> 8; // Credential Modifier Node ID (MSB)
    *p_frame++ = pCredential->metadata.modifier_node_id & 0xFF; // Credential Modifier Node ID (LSB)
  }

  CC_Notification_TriggerAndTransmit(0, notificationEvent, event_parameters,
                                     p_frame - event_parameters, NULL, false);
}

void CC_UserCredential_CredentialSetErrorReport_tx(
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options, const uint8_t errorType,
  const u3c_credential * const pCredential)
{
  if (!p_rx_options || !pCredential) {
    assert(false);
    return;
  }

  ZW_APPLICATION_TX_BUFFER buffer;
  uint8_t * p_frame_start = (uint8_t *)&buffer;
  uint8_t * p_frame = p_frame_start;
  uint8_t data_length = 0;
  // Set the length of the Credential Data if it fits in the frame
  if ((sizeof(ZW_CREDENTIAL_NOTIFICATION_REPORT_1BYTE_FRAME) - 1 + pCredential->metadata.length)
      <= CREDENTIAL_NOTIFICATION_REPORT_MAX_SIZE) {
    data_length = pCredential->metadata.length;
  }
  const bool credential_read_back =
    data_length > 0 && should_use_crb(pCredential->metadata.type);

  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = CREDENTIAL_SET_ERROR_REPORT;
  *p_frame++ = errorType;
  *p_frame++ = pCredential->metadata.uuid >> 8;
  *p_frame++ = pCredential->metadata.uuid & 0xFF;
  *p_frame++ = pCredential->metadata.type;
  *p_frame++ = pCredential->metadata.slot >> 8;
  *p_frame++ = pCredential->metadata.slot & 0xFF;
  *p_frame++ = credential_read_back << 7 & CREDENTIAL_REPORT_PROPERTIES1_CRB_BIT_MASK;
  *p_frame++ = credential_read_back ? data_length : 0;
  if (credential_read_back) {
    memcpy(p_frame, pCredential->data, data_length);
    p_frame += data_length;
  }
  *p_frame++ = pCredential->metadata.modifier_type;
  *p_frame++ = pCredential->metadata.modifier_node_id >> 8;
  *p_frame++ = pCredential->metadata.modifier_node_id & 0xFF;

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx(p_frame_start, p_frame - p_frame_start, NULL, &tx_options);
}

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

/**
 * Deletes all Credentials of a given type for a User.
 *
 * @param[in] user_uid    User Unique Identifier
 * @param[in] filter_type The type of Credentials to delete, set this to
 *                        CREDENTIAL_TYPE_NONE to delete every Credential
 */
void delete_all_credentials_of_type(
  uint16_t uuid, u3c_credential_type filter_type)
{
  u3c_credential_type next_type = CREDENTIAL_TYPE_NONE;
  uint16_t next_slot = 0;
  while (
    CC_UserCredential_get_next_credential(
      uuid, next_type, next_slot, &next_type, &next_slot)
    ) {
    if (filter_type != CREDENTIAL_TYPE_NONE && next_type == filter_type) {
      CC_UserCredential_delete_credential(uuid, next_type, next_slot);
    }
  }
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

/**
 * Checks whether a Credential is identical to an existing Credential in the
 * database.
 *
 * @param[in]  p_credential        Pointer to the incoming Credential
 * @param[out] p_existing_metadata Pointer to the existing Credential metadata
 *                                 (valid only if true was returned)
 *
 * @return true if the Credential is a duplicate
 */
bool find_existing_credential(
  const u3c_credential * const p_credential,
  u3c_credential_metadata * p_existing_metadata)
{
  // Iterate through each User
  uint16_t uuid = CC_UserCredential_get_next_user(0);
  while (uuid) {
    // Iterate through each Credential
    u3c_credential_type type = CREDENTIAL_TYPE_NONE;
    uint16_t slot = 0;

    while (
      CC_UserCredential_get_next_credential(
        uuid, type, slot, &type, &slot)
      ) {
      if (p_credential->metadata.type == type) {
        // Read existing credential
        uint8_t e_data[U3C_BUFFER_SIZE_CREDENTIAL_DATA] = { 0 };

        CC_UserCredential_get_credential(
          uuid, type, slot, p_existing_metadata, e_data);

        // Check whether the incoming and existing data is identical
        if (p_existing_metadata->length == p_credential->metadata.length
            && (memcmp(e_data, p_credential->data, p_existing_metadata->length)
                == 0)
            ) {
          return true;
        }
      }
    }
    uuid = CC_UserCredential_get_next_user(uuid);
  }
  return false;
}

/**
 * Validates that a new Credential conforms to the active configuration of the
 * User Credential Command Class.
 *
 * @param[in] p_metadata Pointer to the incoming Credential data
 * 
 * @return true if the Credential passes all checks
 */
bool validate_new_credential_metadata(
  const u3c_credential_metadata * const p_metadata)
{
  if (!p_metadata) {
    assert(false);
    return false;
  }

  return (
    // CC:0083.01.0A.11.002
    cc_user_credential_is_credential_type_supported(p_metadata->type)

    // CC:0083.01.05.11.015
    && (p_metadata->uuid != 0)
    // CC:0083.01.05.11.014
    && (p_metadata->uuid <= cc_user_credential_get_max_user_unique_idenfitiers())

    // CC:0083.01.0A.11.004
    && (p_metadata->slot
        <= cc_user_credential_get_max_credential_slots(p_metadata->type))
    // CC:0083.01.0A.11.005
    && (p_metadata->slot != 0)

    // CC:0083.01.05.11.016: Associated UUID must reference existing User
    && (CC_UserCredential_get_user(p_metadata->uuid, NULL, NULL)
      == U3C_DB_OPERATION_RESULT_SUCCESS)
  );
}

/**
 * Validates that a new Credential is not a duplicate and that it conforms to
 * the manufacturer's security rules and sends the required reports if these
 * checks fail.
 *
 * @param[in] p_credential Pointer to the incoming Credential data
 * @param[in] p_rx_options Pointer to the properties of the incoming frame
 *                         (set to NULL if a response should not be sent)
 * 
 * @return true if the Credential passes all checks
 */
bool validate_new_credential_data(
  u3c_credential * p_credential, RECEIVE_OPTIONS_TYPE_EX * p_rx_options) {
  // CC:0083.01.0A.11.018: no duplicate credentials within a Credential Type
  u3c_credential existing_credential = {
    .metadata = { 0 },
    .data = p_credential->data
  };
  if (find_existing_credential(p_credential, &existing_credential.metadata)) {
    bool is_identifier_identical = (
      p_credential->metadata.uuid == existing_credential.metadata.uuid
      && p_credential->metadata.type == existing_credential.metadata.type
      && p_credential->metadata.slot == existing_credential.metadata.slot
    );
    if (!is_identifier_identical) {
      if (p_rx_options) {
        CC_UserCredential_CredentialSetErrorReport_tx(
          p_rx_options,
          CREDENTIAL_SET_ERROR_REPORT_DUPLICATECREDENTIAL,
          &existing_credential);
      }
      return false;
    }
  }

  // CC:0083.01.0D.11.001: Credential must follow manufacturer security rules
  if (!CC_UserCredential_manufacturer_validate_credential(p_credential)) {
    CC_UserCredential_CredentialSetErrorReport_tx(
      p_rx_options,
      CREDENTIAL_SET_ERROR_REPORT_MANUFACTURERSECURITYRULES,
      p_credential);
    return false;
  }

  return true;
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
  pReport->properties1 = (cc_user_credential_is_credential_checksum_supported() << 7); // Bit 7: Credential Checksum Support
  pReport->numberOfSupportedCredentialTypes = n_credential_types;

  uint8_t * pArray = (uint8_t *)&(pReport->credentialType1);
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
    uint16_t min_length_of_credential_data = cc_user_credential_get_min_length_of_data(i_type);
    uint16_t max_length_of_credential_data = cc_user_credential_get_max_length_of_data(i_type);
    uint8_t cl_recommended_timeout = cc_user_credential_get_cl_recommended_timeout(i_type);
    uint8_t cl_number_of_steps = cc_user_credential_get_cl_number_of_steps(i_type);

    // Credential Type
    *(pArray + i_array) = i_type;

    // Properties1 (bit 7 is CL Support)
    *(pArray + 1 * n_credential_types + i_array) = (cl_support << 7) & 0x80;

    // Number of Supported Credential Slots (MSB, LSB)
    *(pArray + 2 * n_credential_types + 2 * i_array + 0) = max_credential_slots >> 8;
    *(pArray + 2 * n_credential_types + 2 * i_array + 1) = max_credential_slots & 0xFF;

    // Min Length of Credential Data (MSB, LSB)
    *(pArray + 4 * n_credential_types + 2 * i_array + 0) = min_length_of_credential_data >> 8;
    *(pArray + 4 * n_credential_types + 2 * i_array + 1) = min_length_of_credential_data & 0xFF;

    // Max Length of Credential Data (MSB, LSB)
    *(pArray + 6 * n_credential_types + 2 * i_array + 0) = max_length_of_credential_data >> 8;
    *(pArray + 6 * n_credential_types + 2 * i_array + 1) = max_length_of_credential_data & 0xFF;

    // Credential Learn Recommended Timeout
    *(pArray + 8 * n_credential_types + i_array) = cl_recommended_timeout;

    // Credential Learn Number of Steps
    *(pArray + 9 * n_credential_types + i_array) = cl_number_of_steps;
  }

  output->length = 4 + CREDENTIAL_CAPABILITIES_REPORT_VG_SIZE * n_credential_types;
}

/**
 * Called upon receiving a User Set frame.
 *
 * @param[in] pFrameIn Pointer to the incoming frame
 * @param[in] rxOptions Properties of the received frame
 */
static received_frame_status_t CC_UserCredential_UserSet_handler(
  cc_handler_input_t * input, cc_handler_output_t * output)
{
  ZW_USER_SET_1BYTE_FRAME * pFrameIn = &input->frame->ZW_UserSet1byteFrame;
  RECEIVE_OPTIONS_TYPE_EX * rxOptions = input->rx_options;
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;
  ZW_USER_NOTIFICATION_REPORT_FRAME notification_parameters;

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

  // Ignore frames where the User Unique Identifier is greater than allowed
  if (user.unique_identifier > cc_user_credential_get_max_user_unique_idenfitiers()) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  if (operation != USER_SET_OPERATION_TYPE_DELETE) {
    // Ignore frames with unsupported values
    if (
      user.unique_identifier == 0
      || user.type > USER_TYPE_REMOTE_ONLY
      || user.credential_rule > CREDENTIAL_RULE_TRIPLE
      || user.name_encoding > USER_NAME_ENCODING_UNICODE_UTF_16
      ) {
      return RECEIVED_FRAME_STATUS_FAIL;
    }

    // Set non-Expiring User's Expiring Timeout to 0
    if (user.type != USER_TYPE_EXPIRING && user.expiring_timeout_minutes != 0) {
      user.expiring_timeout_minutes = 0;
    }

    // Truncate User Name if it is longer than allowed
    uint8_t user_name_max_length = cc_user_credential_get_max_length_of_user_name();
    if (user.name_length > user_name_max_length) {
      user.name_length = user_name_max_length;
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
      switch (CC_UserCredential_add_user(&user, pName)) {
        case U3C_DB_OPERATION_RESULT_SUCCESS: {
          // Send Notification Report
          user_event_parameters_from_frame(pFrameIn, rxOptions, &notification_parameters);
          CC_Notification_TriggerAndTransmit(
            0, NOTIFICATION_EVENT_ACCESS_CONTROL_USER_ADDED,
            (&notification_parameters.userModifierType),
            USER_NOTIFICATION_EV_PAR_LENGTH, NULL, false);
          status = RECEIVED_FRAME_STATUS_SUCCESS;
          break;
        }

        case U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED: {
          // Send User Set Error Report with existing data
          ZW_USER_SET_ERROR_REPORT_1BYTE_FRAME * pFrameOut = &output->frame->ZW_UserSetErrorReport1byteFrame;
          CC_UserCredential_get_user(user.unique_identifier, &user, name);
          output->length = sizeof(ZW_USER_SET_ERROR_REPORT_1BYTE_FRAME) - 1 + user.name_length;

          pFrameOut->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
          pFrameOut->cmd = USER_SET_ERROR_REPORT;
          pFrameOut->userSetErrorType = USER_SET_ERROR_REPORT_USERADDREJECTEDLOCATIONOCCUPIED;
          pFrameOut->userModifierType = user.modifier_type;
          pFrameOut->userModifierNodeId1 = user.modifier_node_id >> 8;
          pFrameOut->userModifierNodeId2 = user.modifier_node_id & 0xFF;
          pFrameOut->userUniqueIdentifier1 = pFrameIn->userUniqueIdentifier1;
          pFrameOut->userUniqueIdentifier2 = pFrameIn->userUniqueIdentifier2;
          pFrameOut->userType = user.type;
          pFrameOut->properties1 |= user.active & USER_SET_PROPERTIES2_USER_ACTIVE_STATE_BIT_MASK;
          pFrameOut->credentialRule = user.credential_rule;
          pFrameOut->expiringTimeoutMinutes1 = user.expiring_timeout_minutes >> 8;
          pFrameOut->expiringTimeoutMinutes2 = user.expiring_timeout_minutes & 0xFF;
          pFrameOut->properties2 |= user.name_encoding & USER_SET_PROPERTIES3_USER_NAME_ENCODING_MASK;
          pFrameOut->userNameLength = user.name_length;
          memcpy(&pFrameOut->userName1, name, user.name_length);

          __attribute__ ((fallthrough));
        }

        case U3C_DB_OPERATION_RESULT_FAIL_FULL: {
          // Send Notification Report
          user_event_parameters_from_user(&user, &notification_parameters);
          CC_Notification_TriggerAndTransmit(
            0, NOTIFICATION_EVENT_ACCESS_CONTROL_USER_UNCHANGED,
            (&notification_parameters.userModifierType),
            USER_NOTIFICATION_EV_PAR_LENGTH, NULL, false);
          status = RECEIVED_FRAME_STATUS_FAIL;
          break;
        }

        default:
          break;
      }
      break;
    }

    case USER_SET_OPERATION_TYPE_MODIFY: {
      switch (CC_UserCredential_modify_user(&user, pName)) {
        case U3C_DB_OPERATION_RESULT_SUCCESS: {
          // Send Notification Report
          user_event_parameters_from_frame(pFrameIn, rxOptions, &notification_parameters);
          CC_Notification_TriggerAndTransmit(
            0, NOTIFICATION_EVENT_ACCESS_CONTROL_USER_MODIFIED,
            (&notification_parameters.userModifierType),
            USER_NOTIFICATION_EV_PAR_LENGTH, NULL, false);
          status = RECEIVED_FRAME_STATUS_SUCCESS;
          break;
        }

        case U3C_DB_OPERATION_RESULT_FAIL_DNE: {
          // Send User Set Error Report with the rejected data
          ZW_USER_SET_ERROR_REPORT_1BYTE_FRAME * pFrameOut = &output->frame->ZW_UserSetErrorReport1byteFrame;
          output->length = sizeof(ZW_USER_SET_ERROR_REPORT_1BYTE_FRAME) - 1 + pFrameIn->userNameLength;

          pFrameOut->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
          pFrameOut->cmd = USER_SET_ERROR_REPORT;
          pFrameOut->userSetErrorType = USER_SET_ERROR_REPORT_USERMODIFYREJECTEDLOCATIONEMPTY;
          pFrameOut->userModifierType = MODIFIER_TYPE_DNE;
          pFrameOut->userModifierNodeId1 = 0;
          pFrameOut->userModifierNodeId2 = 0;
          pFrameOut->userType = pFrameIn->userType;
          pFrameOut->properties1 |= pFrameIn->properties2 & USER_SET_PROPERTIES2_USER_ACTIVE_STATE_BIT_MASK;
          pFrameOut->credentialRule = pFrameIn->credentialRule;
          pFrameOut->expiringTimeoutMinutes1 = pFrameIn->expiringTimeoutMinutes1;
          pFrameOut->expiringTimeoutMinutes2 = pFrameIn->expiringTimeoutMinutes2;
          pFrameOut->properties2 |= pFrameIn->properties3 & USER_SET_PROPERTIES3_USER_NAME_ENCODING_MASK;
          pFrameOut->userNameLength = pFrameIn->userNameLength;
          strncpy((char *)&pFrameOut->userName1, (char *)&pFrameIn->userName1, pFrameIn->userNameLength);

          status = RECEIVED_FRAME_STATUS_FAIL;
          break;
        }

        default:
          break;
      }
      break;
    }

    case USER_SET_OPERATION_TYPE_DELETE: {
      if (user.unique_identifier == 0) {
        // Delete all Users
        uint16_t user_uid = CC_UserCredential_get_next_user(0);
        uint16_t previous_user_uid = user_uid;

        while (user_uid) {
          previous_user_uid = user_uid;
          delete_all_credentials_of_type(user_uid, CREDENTIAL_TYPE_NONE);
          user_uid = CC_UserCredential_get_next_user(user_uid);
          CC_UserCredential_delete_user(previous_user_uid);
        }

        // Send Notification Report
        user_event_parameters_from_frame(pFrameIn, rxOptions, &notification_parameters);
        CC_Notification_TriggerAndTransmit(
          0, NOTIFICATION_EVENT_ACCESS_CONTROL_ALL_USERS_DELETED,
          (&notification_parameters.userModifierType),
          USER_NOTIFICATION_EV_PAR_LENGTH, NULL, false);
        status = RECEIVED_FRAME_STATUS_SUCCESS;
      } else {
        // Delete a single User
        delete_all_credentials_of_type(user.unique_identifier, CREDENTIAL_TYPE_NONE);
        u3c_db_operation_result result = CC_UserCredential_delete_user(user.unique_identifier);

        switch (result) {
          case U3C_DB_OPERATION_RESULT_SUCCESS: {
            // Send Notification Report
            user_event_parameters_from_frame(pFrameIn, rxOptions, &notification_parameters);
            CC_Notification_TriggerAndTransmit(
              0, NOTIFICATION_EVENT_ACCESS_CONTROL_USER_DELETED,
              (&notification_parameters.userModifierType),
              USER_NOTIFICATION_EV_PAR_LENGTH, NULL, false);
            __attribute__ ((fallthrough));
          }

          case U3C_DB_OPERATION_RESULT_FAIL_DNE: {
            status = RECEIVED_FRAME_STATUS_FAIL;
            break;
          }

          default:
            break;
        }
      }
      break;
    }

    default: {
      status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
    }
  }

  return status;
}

static void CC_UserCredential_UserGet_handler(
  cc_handler_input_t * input, cc_handler_output_t * output)
{
  ZW_USER_GET_FRAME * pFrameIn = &input->frame->ZW_UserGetFrame;
  ZW_USER_REPORT_1BYTE_FRAME * pReport = &output->frame->ZW_UserReport1byteFrame;
  uint8_t name[U3C_BUFFER_SIZE_USER_NAME] = { 0 };
  u3c_user user = { 0 };

  uint16_t uid = pFrameIn->userUniqueIdentifier1 << 8 | pFrameIn->userUniqueIdentifier2;
  if (uid == 0) {
    // Return the first available User's data
    uid = CC_UserCredential_get_next_user(0);
  }
  uint16_t next_user_uid = CC_UserCredential_get_next_user(uid);

  u3c_db_operation_result result = CC_UserCredential_get_user(uid, &user, name);

  pReport->cmdClass = COMMAND_CLASS_USER_CREDENTIAL;
  pReport->cmd = USER_REPORT;
  output->length = sizeof(ZW_USER_REPORT_1BYTE_FRAME) - 1;

  pReport->nextUserUniqueIdentifier1 = next_user_uid >> 8;
  pReport->nextUserUniqueIdentifier2 = next_user_uid & 0xFF;
  pReport->userUniqueIdentifier1 = uid >> 8;
  pReport->userUniqueIdentifier2 = uid & 0xFF;

  if (result == U3C_DB_OPERATION_RESULT_SUCCESS) {
    // Return User's data
    pReport->userModifierType = user.modifier_type;
    pReport->userModifierNodeId1 = user.modifier_node_id >> 8;
    pReport->userModifierNodeId2 = user.modifier_node_id & 0xFF;
    pReport->userType = user.type;
    pReport->properties1 = user.active & USER_REPORT_PROPERTIES1_USER_ACTIVE_STATE_BIT_MASK;
    pReport->credentialRule = user.credential_rule;
    pReport->expiringTimeoutMinutes1 = user.expiring_timeout_minutes >> 8;
    pReport->expiringTimeoutMinutes2 = user.expiring_timeout_minutes & 0xFF;
    pReport->properties2 = user.name_encoding & USER_REPORT_PROPERTIES2_USER_NAME_ENCODING_MASK;
    pReport->userNameLength = user.name_length;

    // Copy User Name and increase length of outgoing frame by it
    memcpy(&pReport->userName1, name, user.name_length);
    output->length += user.name_length;
  } else {
    // The User doesn't exist, return blank data, but make sure all fields are valid
    pReport->credentialRule = CREDENTIAL_RULE_SINGLE;
  }
}

// TODO: Move this function outside of the CC.
static received_frame_status_t CC_UserCredential_CredentialSet_handler(
  u3c_operation_type_t operation_type,
  u3c_credential * p_credential,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_FAIL;

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
    if (!cc_user_credential_is_credential_type_supported(p_credential->metadata.type))
    {
      return RECEIVED_FRAME_STATUS_FAIL;
    }
  }

  switch (operation_type) {
    case U3C_OPERATION_TYPE_ADD: {
      switch (CC_UserCredential_add_credential(p_credential)) {
        case U3C_DB_OPERATION_RESULT_SUCCESS: {
          CC_UserCredential_CredentialNotificationReport_tx(
            NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_ADDED,
            p_credential);

          status = RECEIVED_FRAME_STATUS_SUCCESS;
          break;
        }

        case U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED: {
          CC_UserCredential_CredentialSetErrorReport_tx(
            p_rx_options,
            CREDENTIAL_SET_ERROR_REPORT_CREDENTIALADDREJECTEDLOCATIONOCCUPIED,
            p_credential);

          __attribute__ ((fallthrough));
        }

        case U3C_DB_OPERATION_RESULT_FAIL_FULL: {
          CC_UserCredential_CredentialNotificationReport_tx(
            NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_UNCHANGED,
            p_credential);

          status = RECEIVED_FRAME_STATUS_FAIL;
        }

        default:
          break;
      }
      break;
    }

    case U3C_OPERATION_TYPE_MODIFY: {
      switch (CC_UserCredential_modify_credential(p_credential)) {
        case U3C_DB_OPERATION_RESULT_SUCCESS: {
          CC_UserCredential_CredentialNotificationReport_tx(
            NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_MODIFIED,
            p_credential);

          status = RECEIVED_FRAME_STATUS_SUCCESS;
          break;
        }

        case U3C_DB_OPERATION_RESULT_FAIL_DNE: {
          CC_UserCredential_CredentialSetErrorReport_tx(
            p_rx_options,
            CREDENTIAL_SET_ERROR_REPORT_CREDENTIALMODIFYREJECTEDLOCATIONEMPTY,
            p_credential);

          CC_UserCredential_CredentialNotificationReport_tx(
            NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_UNCHANGED,
            p_credential);

          status = RECEIVED_FRAME_STATUS_FAIL;
          break;
        }

        default:
          break;
      }
      break;
    }

    case U3C_OPERATION_TYPE_DELETE: {
      if (p_credential->metadata.uuid == 0) {
        // Delete all User Credentials

        // Iterate through each User
        uint16_t user_uid = CC_UserCredential_get_next_user(0);
        while (user_uid) {
          // Delete an User's every Credential (of a certain type, if specified)
          delete_all_credentials_of_type(user_uid, p_credential->metadata.type);
          user_uid = CC_UserCredential_get_next_user(user_uid);
        }

        CC_UserCredential_CredentialNotificationReport_tx(
          NOTIFICATION_EVENT_ACCESS_CONTROL_MULTIPLE_CREDENTIALS_DELETED,
          p_credential);

        status = RECEIVED_FRAME_STATUS_SUCCESS;
      } else {
        // Delete Credential(s) of a specific User

        if (p_credential->metadata.type == 0 && p_credential->metadata.slot == 0) {
          // Delete an User's every Credential (of a certain type, if specified)
          delete_all_credentials_of_type(
            p_credential->metadata.uuid, p_credential->metadata.type);

          CC_UserCredential_CredentialNotificationReport_tx(
            NOTIFICATION_EVENT_ACCESS_CONTROL_MULTIPLE_CREDENTIALS_DELETED,
            p_credential);

          status = RECEIVED_FRAME_STATUS_SUCCESS;
        } else {
          // Delete a single Credential

          // Read existing data pending deletion for use in Notification Report
          bool credential_exists = CC_UserCredential_get_credential(
            p_credential->metadata.uuid,
            p_credential->metadata.type,
            p_credential->metadata.slot,
            &p_credential->metadata, p_credential->data
            )
                                   == U3C_DB_OPERATION_RESULT_SUCCESS;

          if (credential_exists) {
            if (CC_UserCredential_delete_credential(
                  p_credential->metadata.uuid,
                  p_credential->metadata.type,
                  p_credential->metadata.slot
                  )
                == U3C_DB_OPERATION_RESULT_SUCCESS
                ) {
              // Send report of deleted Credential
              CC_UserCredential_CredentialNotificationReport_tx(
                NOTIFICATION_EVENT_ACCESS_CONTROL_CREDENTIAL_DELETED,
                p_credential);

              status = RECEIVED_FRAME_STATUS_SUCCESS;
            }
          }
        }
      }
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
  uint8_t credential_length = input->frame->ZW_CredentialSet1byteFrame.credentialLength;
  const uint8_t FRAME_LENGTH_WITHOUT_CREDENTIAL_DATA = 9;
  if (input->length != (FRAME_LENGTH_WITHOUT_CREDENTIAL_DATA + credential_length)) {
    return RECEIVED_FRAME_STATUS_FAIL;
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
  u3c_credential * p_credential,
  u3c_credential_type next_credential_type,
  uint16_t next_credential_slot,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  );

void CC_UserCredential_CredentialReport_tx(
  u3c_credential * p_credential,
  u3c_credential_type next_credential_type,
  uint16_t next_credential_slot,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options
  )
{
  ZW_APPLICATION_TX_BUFFER buffer;
  uint8_t * p_frame_start = (uint8_t *)&buffer;
  uint8_t * p_frame = p_frame_start;
  *p_frame++ = COMMAND_CLASS_USER_CREDENTIAL;
  *p_frame++ = CREDENTIAL_REPORT;
  *p_frame++ = (uint8_t)(p_credential->metadata.uuid >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.uuid; // LSB
  *p_frame++ = (uint8_t)p_credential->metadata.type;
  *p_frame++ = (uint8_t)(p_credential->metadata.slot >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.slot; // LSB
  *p_frame++ = (p_credential->metadata.length > 0) ? 0x80 : 0; // CRB
  *p_frame++ = p_credential->metadata.length;
  memcpy(p_frame, p_credential->data, p_credential->metadata.length);
  p_frame += p_credential->metadata.length;
  *p_frame++ = (uint8_t)p_credential->metadata.modifier_type;
  *p_frame++ = (uint8_t)(p_credential->metadata.modifier_node_id >> 8); // MSB
  *p_frame++ = (uint8_t)p_credential->metadata.modifier_node_id; // LSB
  *p_frame++ = (uint8_t)next_credential_type;
  *p_frame++ = (uint8_t)(next_credential_slot >> 8); // MSB
  *p_frame++ = (uint8_t)next_credential_slot; // LSB

  zaf_tx_options_t tx_options;
  zaf_transport_rx_to_tx_options(p_rx_options, &tx_options);
  zaf_transport_tx(p_frame_start, p_frame - p_frame_start, NULL, &tx_options);
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

// TODO: Move this function outside of the CC.
bool CC_UserCredential_CredentialGet_handler(
  cc_handler_input_t * input,
  u3c_credential_metadata * p_metadata,
  u3c_credential_report_tx_callback_t callback
  )
{
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
  bool result = false;

  if (p_metadata->type == 0 && p_metadata->slot == 0) {
    // Both Type and Slot are zero, return the User's first Credential
    CC_UserCredential_get_next_credential(
      p_metadata->uuid, 0, 0, &p_metadata->type, &p_metadata->slot);
  }

  switch (CC_UserCredential_get_credential(
            p_metadata->uuid, p_metadata->type, p_metadata->slot,
            &credential.metadata, credential.data)) {
    case U3C_DB_OPERATION_RESULT_SUCCESS: {
      CC_UserCredential_get_next_credential(
        credential.metadata.uuid, credential.metadata.type, credential.metadata.slot,
        &next_credential_type, &next_credential_slot
        );
      result = true;

      __attribute__ ((fallthrough));
    }
    default: {
      callback(
        &credential,
        next_credential_type, next_credential_slot,
        input->rx_options);
      break;
    }
  }
  return result;
}

static received_frame_status_t CC_UserCredential_CredentialGet_parser(
  cc_handler_input_t * input)
{
  // Validate UUID
  uint16_t uuid = (input->frame->ZW_CredentialGetFrame.userUniqueIdentifier1 << 8)
                  | input->frame->ZW_CredentialGetFrame.userUniqueIdentifier2;
  if (0 == uuid) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  u3c_credential_metadata metadata = {
    .uuid = uuid,
    .length = 0,
    .modifier_type = MODIFIER_TYPE_UNKNOWN,
    .modifier_node_id = 0,
    .type = input->frame->ZW_CredentialGetFrame.credentialType,
    .slot = (input->frame->ZW_CredentialGetFrame.credentialSlot1 << 8)
            | input->frame->ZW_CredentialGetFrame.credentialSlot2
  };

  /**
   * The Credential Type and Credential Metadata must be either:
   * - both non-0 (requesting a specific Credential), or
   * - both 0 (requesting the first Credential of a User)
   */
  if ((metadata.type == 0) ^ (metadata.slot == 0)) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

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

  if (p_rx_options->destNode.nodeId == 0 || p_rx_options->sourceNode.nodeId) {
    /**
     * Since the Learn process was initiated locally, there is no incoming
     * frame to parse the receive options from.
     * Use the Controller's ID as the source and the node's own ID as the
     * destination.
     */
    p_rx_options->sourceNode.nodeId = ZAF_GetSucNodeId();
    p_rx_options->destNode.nodeId = ZAF_GetNodeID();
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
  memset(&cl_state, 0, sizeof(cl_state));
  memset(&cl_target, 0, sizeof(cl_target));
}

static received_frame_status_t CC_UserCredential_CredentialLearnStart_handler(
  const u3c_operation_type_t operation_type,
  const u3c_credential_identifier * const p_learn_target,
  const uint8_t timeout_seconds,
  RECEIVE_OPTIONS_TYPE_EX * p_rx_options)
{
  if (!p_learn_target || !p_rx_options) {
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
    !validate_new_credential_metadata(&metadata)
    // CC:0083.01.0F.11.004
    || !cc_user_credential_is_credential_learn_supported(metadata.type)
  ) {
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  /**
   * Check whether the requested Add or Modify operation is consistent with the
   * current state of the database
   */
  u3c_db_operation_result result = CC_UserCredential_get_credential(
    metadata.uuid, metadata.type, metadata.slot,
    NULL, NULL);
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
      if (result != U3C_DB_OPERATION_RESULT_SUCCESS) {
        status = CL_STATUS_INVALID_MODIFY_OPERATION_TYPE;
        is_consistent_with_database = false;
      }
      break;
    default:
      assert(false); // Should not be possible due to earlier checks
      return RECEIVED_FRAME_STATUS_FAIL;
  }
  if (!is_consistent_with_database) {
    CC_UserCredential_CredentialLearnStatusReport_tx(
      status, p_learn_target, 0, p_rx_options
    );
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Store details of current Credential Learn operation
  memcpy(&cl_target, p_learn_target, sizeof(cl_target));
  memcpy(&cl_state.rx_options, p_rx_options, sizeof(RECEIVE_OPTIONS_TYPE_EX));
  cl_state.operation_type = operation_type;
  // If no Credential Learn Timeout value is provided, use the recommended value
  cl_state.timeout_seconds = timeout_seconds
                             ? timeout_seconds
                             : cc_user_credential_get_cl_recommended_timeout(p_learn_target->type);
  cl_state.in_progress = true;

  // Signal start of process to application
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_START, &cl_target.type
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
      credential_learn_reset();
      zaf_event_distributor_enqueue_cc_event(
        COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_REMOTE, NULL
      );
    }
    CC_UserCredential_CredentialLearnStatusReport_tx(
      cl_status, &cl_target, 0, &cl_state.rx_options
    );
  }

  return frame_status;
}

typedef enum
{
  U3C_UCAR_STATUS_SUCCESS                                         = USER_CREDENTIAL_ASSOCIATION_REPORT_SUCCESS,
  U3C_UCAR_STATUS_SOURCE_USER_UNIQUE_IDENTIFIER_INVALID           = USER_CREDENTIAL_ASSOCIATION_REPORT_SOURCE_USER_UNIQUE_IDENTIFIER_INVALID,
  U3C_UCAR_STATUS_SOURCE_USER_UNIQUE_IDENTIFIER_NONEXISTENT       = USER_CREDENTIAL_ASSOCIATION_REPORT_SOURCE_USER_UNIQUE_IDENTIFIER_NONEXISTENT,
  U3C_UCAR_STATUS_SOURCE_CREDENTIAL_TYPE_INVALID                  = USER_CREDENTIAL_ASSOCIATION_REPORT_SOURCE_CREDENTIAL_TYPE_INVALID,
  U3C_UCAR_STATUS_SOURCE_CREDENTIAL_SLOT_INVALID                  = USER_CREDENTIAL_ASSOCIATION_REPORT_SOURCE_CREDENTIAL_SLOT_INVALID,
  U3C_UCAR_STATUS_SOURCE_CREDENTIAL_SLOT_EMPTY                    = USER_CREDENTIAL_ASSOCIATION_REPORT_SOURCE_CREDENTIAL_SLOT_EMPTY,
  U3C_UCAR_STATUS_DESTINATION_USER_UNIQUE_IDENTIFIER_INVALID      = USER_CREDENTIAL_ASSOCIATION_REPORT_DESTINATION_USER_UNIQUE_IDENTIFIER_INVALID,
  U3C_UCAR_STATUS_DESTINATION_USER_UNIQUE_IDENTIFIER_NONEXISTENT  = USER_CREDENTIAL_ASSOCIATION_REPORT_DESTINATION_USER_UNIQUE_IDENTIFIER_NONEXISTENT,
  U3C_UCAR_STATUS_DESTINATION_CREDENTIAL_SLOT_INVALID             = USER_CREDENTIAL_ASSOCIATION_REPORT_DESTINATION_CREDENTIAL_SLOT_INVALID,
  U3C_UCAR_STATUS_DESTINATION_CREDENTIAL_SLOT_OCCUPIED            = USER_CREDENTIAL_ASSOCIATION_REPORT_DESTINATION_CREDENTIAL_SLOT_OCCUPIED
}
u3c_user_credential_association_report_status_t;

/**
 * @brief Fills a given array with USER_CREDENTIAL_ASSOCIATION_REPORT.
 * 
 * @param[out] p_frame Address of frame memory to fill.
 * @param[in,out] p_length Frame buffer length. Will be overwritten with the filled frame length.
 * @param[in] p_source_metadata Source credential metadata.
 * @param[in] p_destination_metadata Destination credential metadata.
 * @param[in] status Status of association set action.
 */
static void fill_user_credential_association_report(
  uint8_t * const p_frame,
  uint8_t * const p_length,
  u3c_credential_metadata const * const p_source_metadata,
  u3c_credential_metadata const * const p_destination_metadata,
  u3c_user_credential_association_report_status_t const status
  )
{
  if (NULL == p_length) {
    assert(false);
    return;
  }
  if ((NULL == p_frame) || 
      (sizeof(ZW_USER_CREDENTIAL_ASSOCIATION_REPORT_FRAME) < *p_length) ||
      (NULL == p_source_metadata) ||
      (NULL == p_destination_metadata)) {
    assert(false);
    *p_length = 0;
    return;
  }
  ZW_USER_CREDENTIAL_ASSOCIATION_REPORT_FRAME * p_cmd;
  p_cmd = (ZW_USER_CREDENTIAL_ASSOCIATION_REPORT_FRAME *)p_frame;

  p_cmd->cmdClass                         = COMMAND_CLASS_USER_CREDENTIAL;
  p_cmd->cmd                              = USER_CREDENTIAL_ASSOCIATION_REPORT;
  p_cmd->sourceUserUniqueIdentifier1      = (uint8_t)(p_source_metadata->uuid >> 8); // MSB
  p_cmd->sourceUserUniqueIdentifier2      = (uint8_t)p_source_metadata->uuid; // LSB
  p_cmd->sourceCredentialType             = (uint8_t)p_source_metadata->type;
  p_cmd->sourceCredentialSlot1            = (uint8_t)(p_source_metadata->slot >> 8); // MSB
  p_cmd->sourceCredentialSlot2            = (uint8_t)p_source_metadata->slot; // LSB
  p_cmd->destinationUserUniqueIdentifier1 = (uint8_t)(p_destination_metadata->uuid >> 8); // MSB
  p_cmd->destinationUserUniqueIdentifier2 = (uint8_t)p_destination_metadata->uuid; // LSB
  p_cmd->destinationCredentialSlot1       = (uint8_t)(p_destination_metadata->slot >> 8); // MSB
  p_cmd->destinationCredentialSlot2       = (uint8_t)p_destination_metadata->slot; // LSB
  p_cmd->userCredentialAssociationStatus  = (uint8_t)status;

  *p_length = sizeof(ZW_USER_CREDENTIAL_ASSOCIATION_REPORT_FRAME);
}

static received_frame_status_t CC_UserCredential_UserCredentialAssociationSet_handler(
  cc_handler_output_t * const output,
  const u3c_credential_metadata * const source_metadata,
  const u3c_credential_metadata * const destination_metadata
)
{
  if (!output || !source_metadata || !destination_metadata) {
    assert(false);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Database-related checks according to Table 2.546
  // Source User Unique Identifier must reference an existing User
  if (U3C_DB_OPERATION_RESULT_SUCCESS != CC_UserCredential_get_user(source_metadata->uuid, NULL, NULL)) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_SOURCE_USER_UNIQUE_IDENTIFIER_NONEXISTENT;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, source_metadata, destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Destination User Unique Identifier must reference an existing User
  if (U3C_DB_OPERATION_RESULT_SUCCESS != CC_UserCredential_get_user(destination_metadata->uuid, NULL, NULL)) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_DESTINATION_USER_UNIQUE_IDENTIFIER_NONEXISTENT;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, source_metadata, destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  /**
   * Attempt to execute the move operation. If the source Credential does not
   * exist or the destination slot is occupied, send an error report.
   */
  u3c_db_operation_result move_credential_result = CC_UserCredential_move_credential(
    source_metadata->uuid, source_metadata->type, source_metadata->slot,
    destination_metadata->uuid, destination_metadata->slot);
  u3c_user_credential_association_report_status_t status;
  received_frame_status_t frame_status = RECEIVED_FRAME_STATUS_FAIL;
  switch (move_credential_result) {
    case U3C_DB_OPERATION_RESULT_SUCCESS:
      status = U3C_UCAR_STATUS_SUCCESS;
      frame_status = RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case U3C_DB_OPERATION_RESULT_FAIL_DNE:
      status = U3C_UCAR_STATUS_SOURCE_CREDENTIAL_SLOT_EMPTY;
      break;
    case U3C_DB_OPERATION_RESULT_FAIL_OCCUPIED:
      status = U3C_UCAR_STATUS_DESTINATION_CREDENTIAL_SLOT_OCCUPIED;
      break;
    default:
      // Database error
      return RECEIVED_FRAME_STATUS_FAIL;
  }
  fill_user_credential_association_report((uint8_t *)output->frame, &output->length, source_metadata, destination_metadata, status);
  return frame_status;
}

static received_frame_status_t CC_UserCredential_UserCredentialAssociationSet_parser(
  cc_handler_input_t const * const input,
  cc_handler_output_t * const output)
{
  uint16_t source_uuid = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.sourceUserUniqueIdentifier1 << 8) |
                                    input->frame->ZW_UserCredentialAssociationSetFrame.sourceUserUniqueIdentifier2;

  uint16_t source_slot = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.sourceCredentialSlot1 << 8) |
                                    input->frame->ZW_UserCredentialAssociationSetFrame.sourceCredentialSlot2;

  uint16_t destination_uuid = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.destinationUserUniqueIdentifier1 << 8) |
                                         input->frame->ZW_UserCredentialAssociationSetFrame.destinationUserUniqueIdentifier2;

  uint16_t destination_slot = (uint16_t)(input->frame->ZW_UserCredentialAssociationSetFrame.destinationCredentialSlot1 << 8) |
                                         input->frame->ZW_UserCredentialAssociationSetFrame.destinationCredentialSlot2;

  u3c_credential_metadata source_metadata = {0};
  source_metadata.uuid = source_uuid;
  source_metadata.type = input->frame->ZW_UserCredentialAssociationSetFrame.sourceCredentialType;
  source_metadata.slot = source_slot;

  u3c_credential_metadata destination_metadata = {0};
  destination_metadata.uuid = destination_uuid;
  destination_metadata.type = source_metadata.type;
  destination_metadata.slot = destination_slot;

  // Configuration-related checks according to Table 2.546
  // Source User Unique Identifier must be in supported range
  if (0 == source_uuid || source_uuid > cc_user_credential_get_max_user_unique_idenfitiers()) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_SOURCE_USER_UNIQUE_IDENTIFIER_INVALID;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, &source_metadata, &destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Source Credential Type must be supported
  if (!cc_user_credential_is_credential_type_supported(source_metadata.type)) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_SOURCE_CREDENTIAL_TYPE_INVALID;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, &source_metadata, &destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Source Credential Slot must be in supported range
  if (source_slot == 0 || source_slot > cc_user_credential_get_max_credential_slots(source_metadata.type)) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_SOURCE_CREDENTIAL_SLOT_INVALID;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, &source_metadata, &destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Destination User Unique Identifier must be in supported range
  if (0 == destination_uuid || destination_uuid > cc_user_credential_get_max_user_unique_idenfitiers()) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_DESTINATION_USER_UNIQUE_IDENTIFIER_INVALID;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, &source_metadata, &destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  // Check if the Destination Credential Slot must be in supported range
  if (destination_slot == 0 || destination_slot > cc_user_credential_get_max_credential_slots(destination_metadata.type)) {
    u3c_user_credential_association_report_status_t status = U3C_UCAR_STATUS_DESTINATION_CREDENTIAL_SLOT_INVALID;
    fill_user_credential_association_report((uint8_t *)output->frame, &output->length, &source_metadata, &destination_metadata, status);
    return RECEIVED_FRAME_STATUS_FAIL;
  }

  return CC_UserCredential_UserCredentialAssociationSet_handler(
    output, &source_metadata, &destination_metadata
  );
}

static received_frame_status_t CC_UserCredential_AllUsersChecksumGet_handler(const cc_handler_input_t * input)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
  /**
   * This command MUST be ignored by a node advertising no support for the All Users Checksum
   * functionality in the User Capabilities Report Command.
   * CC:0083.01.14.11.000
   */
  if (cc_user_credential_is_all_users_checksum_supported() == true) {
    u3c_user user = { 0 };
    uint8_t name[UINT8_MAX] = { 0 };
    uint8_t uuid_msb = 0;
    uint8_t uuid_lsb = 0;
    bool user_is_available = false;

    uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.15.11.000
    uint16_t user_uid = CC_UserCredential_get_next_user(0);

    status = RECEIVED_FRAME_STATUS_SUCCESS;
    while (user_uid) {
      user_is_available = true;
      if (CC_UserCredential_get_user(user_uid, &user, name) == U3C_DB_OPERATION_RESULT_SUCCESS) {
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
      } else {
        // Driver error or database corruption
        return RECEIVED_FRAME_STATUS_FAIL;
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
  }
  return status;
}

static received_frame_status_t CC_UserCredential_UserChecksumGet_handler(const cc_handler_input_t * input)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
  /**
   * This command MUST be ignored by a node advertising no support for the User Checksum functionality
   * in the User Capabilities Report Command.
   * CC:0083.01.16.11.000
   */
  if (cc_user_credential_is_user_checksum_supported() == true) {
    u3c_user user = { 0 };
    uint8_t name[UINT8_MAX] = { 0 };

    uint16_t uuid = input->frame->ZW_UserChecksumGetFrame.userUniqueIdentifier1 << 8
                    | input->frame->ZW_UserChecksumGetFrame.userUniqueIdentifier2;

    uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.17.11.000

    status = RECEIVED_FRAME_STATUS_SUCCESS;
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
  }
  return status;
}

static received_frame_status_t CC_UserCredential_CredentialChecksumGet_handler(const cc_handler_input_t * input)
{
  received_frame_status_t status = RECEIVED_FRAME_STATUS_NO_SUPPORT;
  /**
   * This command MUST be ignored by a node advertising no support for the Credential Checksum
   * functionality in the Credential Capabilities Report Command.
   * CC:0083.01.18.11.000
   */
  if (cc_user_credential_is_credential_checksum_supported() == true) {
    u3c_credential_type searched_type = input->frame->ZW_CredentialChecksumGetFrame.credentialType;
    uint16_t next_slot = 0;
    uint8_t next_slot_msb = 0;
    uint8_t next_slot_lsb = 0;
    u3c_credential_type next_type = CREDENTIAL_TYPE_NONE;
    uint16_t checksum = CRC_INITAL_VALUE; // CC:0083.01.19.11.001
    bool credential_is_available = false;

    status = RECEIVED_FRAME_STATUS_SUCCESS;

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
      if (next_type == searched_type) {
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
      status = CC_UserCredential_UserSet_handler(input, output);
      break;

    case USER_GET:
      CC_UserCredential_UserGet_handler(input, output);
      status = RECEIVED_FRAME_STATUS_SUCCESS;
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
      status = CC_UserCredential_UserCredentialAssociationSet_parser(input, output);
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
  p_ccc_pair->cmd      = USER_CAPABILITIES_REPORT;
  return 1;
}

static void credential_learn_timer_timeout_callback(__attribute__((unused)) SSwTimer *pTimer)
{
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_TIMEOUT, NULL
  );
  // CC:0083.01.0F.11.013
  CC_UserCredential_CredentialLearnStatusReport_tx(
    CL_STATUS_TIMEOUT, &cl_target, 0, &cl_state.rx_options
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
      const u3c_credential * p_credential = p_data_validate->credential;
      uint8_t notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_NO_EVENT;
      uint8_t event_out = CC_USER_CREDENTIAL_EVENT_VALIDATE_INVALID;
      u3c_credential_metadata tmp_metadata = { 0 };

      if (!find_existing_credential(p_credential, &tmp_metadata)) {
        // The Credential does not exist in the database
        notification_event = NOTIFICATION_EVENT_ACCESS_CONTROL_INVALID_CREDENTIAL_USED_TO_ACCESS_THE_NODE;
        CC_Notification_TriggerAndTransmit(
          0, notification_event, NULL, 0, NULL, false);
      } else {
        // The Credential exists in the database, perform additional checks
        uint8_t pNotificationEventParam[5];
        u3c_user current_user = { 0 };
        CC_UserCredential_get_user(p_credential->metadata.uuid, &current_user, NULL);

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

        pNotificationEventParam[0] = p_credential->metadata.uuid >> 8;
        pNotificationEventParam[1] = p_credential->metadata.uuid & 0xFF;
        pNotificationEventParam[2] = p_credential->metadata.type;
        pNotificationEventParam[3] = p_credential->metadata.slot >> 8;
        pNotificationEventParam[4] = p_credential->metadata.slot & 0xFF;

        CC_Notification_TriggerAndTransmit(
          0, notification_event, pNotificationEventParam,
          sizeof(pNotificationEventParam), NULL, false);
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
        const u3c_event_data_learn_start * const p_data_learn_start = (const u3c_event_data_learn_start *) p_data;
        RECEIVE_OPTIONS_TYPE_EX rx_options = { 0 };
        CC_UserCredential_CredentialLearnStart_handler(
          p_data_learn_start->operation_type, &p_data_learn_start->learn_target,
          p_data_learn_start->timeout_seconds, &rx_options);
      }
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_STEP_START: {
      uint8_t * remaining_steps = (uint8_t *) p_data;
      cl_state.remaining_steps = *remaining_steps;
      // Start or restart timeout timer for this step
      TimerStart(&cl_timeout_timer, cl_state.timeout_seconds * S_TO_MS);
      CC_UserCredential_CredentialLearnStatusReport_tx(
        CL_STATUS_STARTED, &cl_target, *remaining_steps, &cl_state.rx_options
      );
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_READ_DONE: {
      u3c_event_data_learn_read_done * p_event_data = (u3c_event_data_learn_read_done *) p_data;
      uint8_t data_length = p_event_data->data_length;
      uint8_t * data = p_event_data->data;

      TimerStop(&cl_timeout_timer);

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
          .uuid = cl_target.uuid,
          .type = cl_target.type,
          .slot = cl_target.slot,
          .length = data_length,
          .modifier_node_id = cl_state.rx_options.sourceNode.nodeId,
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
        switch (cl_state.operation_type) {
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
        status_out, &cl_target, 0, &cl_state.rx_options
      );

      // CC:0083.01.0F.11.001: Send Credential Report of new Credential
      if (db_op_result == U3C_DB_OPERATION_RESULT_SUCCESS) {
        u3c_credential_type next_credential_type = CREDENTIAL_TYPE_NONE;
        uint16_t next_credential_slot = 0;
        CC_UserCredential_get_next_credential(
          credential.metadata.uuid, credential.metadata.type,
          credential.metadata.slot,
          &next_credential_type, &next_credential_slot);
        CC_UserCredential_CredentialReport_tx(
          &credential, next_credential_type, next_credential_slot,
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
        CC_UserCredential_CredentialLearnStatusReport_tx(
          CL_STATUS_STEP_RETRY, &cl_target, *remaining_steps, &cl_state.rx_options);
      }
      break;
    }
    case CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_LOCAL: {
      if (is_credential_learn_in_progress()) {
        CC_UserCredential_CredentialLearnStatusReport_tx(
          CL_STATUS_ENDED_NOT_DUE_TO_TIMEOUT, &cl_target, 0, &cl_state.rx_options);
        credential_learn_reset();
      }
      break;
    }
  }
}

ZAF_EVENT_DISTRIBUTOR_REGISTER_CC_EVENT_HANDLER(COMMAND_CLASS_USER_CREDENTIAL, user_credential_event_handler);
