/**
 * @file
 * @brief Handler and API for Command Class User Credential.
 *
 * @details The User Credential Command Class is used to manage user Credentials
 * for gaining access to properties, typically through unlocking or opening
 * doors. This command class employs a user-centric model, allowing multiple
 * Credentials to be associated with one User Unique Identifier.
 *
 * @copyright 2023 Silicon Laboratories Inc.
 */

#ifndef _COMMANDCLASSUSERCREDENTIAL_H_
#define _COMMANDCLASSUSERCREDENTIAL_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <stdbool.h>
#include "CC_Common.h"
#include "ZW_classcmd.h"

/**
 * @addtogroup CC
 * @{
 * @addtogroup UserCredential
 * @{
 */

/****************************************************************************/
/*                      EXPORTED TYPES and DEFINITIONS                      */
/****************************************************************************/

typedef enum u3c_modifier_type_ {
  MODIFIER_TYPE_DNE = CREDENTIAL_REPORT_DNE,
  MODIFIER_TYPE_UNKNOWN,
  MODIFIER_TYPE_Z_WAVE,
  MODIFIER_TYPE_LOCALLY,
  MODIFIER_TYPE_MOBILE_APP_OR_OTHER_IOT_TECHNOLOGY
} u3c_modifier_type;

typedef enum u3c_credential_type_ {
  CREDENTIAL_TYPE_NONE,
  CREDENTIAL_TYPE_PIN_CODE = CREDENTIAL_REPORT_PIN_CODE,
  CREDENTIAL_TYPE_PASSWORD,
  CREDENTIAL_TYPE_RFID_CODE,
  CREDENTIAL_TYPE_BLE,
  CREDENTIAL_TYPE_NFC,
  CREDENTIAL_TYPE_UWB,
  CREDENTIAL_TYPE_EYE_BIOMETRIC,
  CREDENTIAL_TYPE_FACE_BIOMETRIC,
  CREDENTIAL_TYPE_FINGER_BIOMETRIC,
  CREDENTIAL_TYPE_HAND_BIOMETRIC,
  CREDENTIAL_TYPE_UNSPECIFIED_BIOMETRIC
} u3c_credential_type;

typedef enum u3c_credential_learn_status_ {
  CL_STATUS_STARTED =                       CREDENTIAL_LEARN_REPORT_STARTED,
  CL_STATUS_SUCCESS =                       CREDENTIAL_LEARN_REPORT_SUCCESS,
  CL_STATUS_ALREADY_IN_PROGRESS =           CREDENTIAL_LEARN_REPORT_ALREADY_IN_PROGRESS,
  CL_STATUS_ENDED_NOT_DUE_TO_TIMEOUT =      CREDENTIAL_LEARN_REPORT_ENDED_NOT_DUE_TO_TIMEOUT,
  CL_STATUS_TIMEOUT =                       CREDENTIAL_LEARN_REPORT_TIMEOUT,
  CL_STATUS_STEP_RETRY =                    CREDENTIAL_LEARN_REPORT_CREDENTIAL_LEARN_STEP_RETRY,
  CL_STATUS_INVALID_ADD_OPERATION_TYPE =    CREDENTIAL_LEARN_REPORT_INVALID_CREDENTIAL_LEARN_ADD_OPERATION_TYPE,
  CL_STATUS_INVALID_MODIFY_OPERATION_TYPE = CREDENTIAL_LEARN_REPORT_INVALID_CREDENTIAL_LEARN_MODIFY_OPERATION_TYPE
} u3c_credential_learn_status;

typedef enum u3c_user_type_ {
  USER_TYPE_GENERAL = 0x00,
  USER_TYPE_PROGRAMMING = 0x03,
  USER_TYPE_NON_ACCESS = 0x04,
  USER_TYPE_DURESS = 0x05,
  USER_TYPE_DISPOSABLE = 0x06,
  USER_TYPE_EXPIRING = 0x07,
  USER_TYPE_REMOTE_ONLY = 0x09
} u3c_user_type;

typedef enum u3c_credential_rule_ {
  CREDENTIAL_RULE_SINGLE = 0x01,
  CREDENTIAL_RULE_DUAL,
  CREDENTIAL_RULE_TRIPLE
} u3c_credential_rule;

typedef enum u3c_user_name_encoding_ {
  USER_NAME_ENCODING_STANDARD_ASCII,
  USER_NAME_ENCODING_STANDARD_AND_OEM_EXTENDED_ASCII,
  USER_NAME_ENCODING_UNICODE_UTF_16
} u3c_user_name_encoding;

typedef struct u3c_user_ {
  uint16_t unique_identifier;
  uint16_t modifier_node_id;
  uint16_t expiring_timeout_minutes;
  uint8_t name_length;
  u3c_user_type type;
  u3c_modifier_type modifier_type;
  u3c_credential_rule credential_rule;
  u3c_user_name_encoding name_encoding;
  bool active;
} u3c_user;

typedef struct u3c_credential_metadata_ {
  uint16_t uuid;
  uint16_t slot;
  uint16_t modifier_node_id;
  uint8_t length;
  u3c_modifier_type modifier_type;
  u3c_credential_type type;
} u3c_credential_metadata;

typedef struct u3c_credential_ {
  u3c_credential_metadata metadata;
  uint8_t * data;
} u3c_credential;

typedef enum {
  U3C_OPERATION_TYPE_ADD    = 0,
  U3C_OPERATION_TYPE_MODIFY = 1,
  U3C_OPERATION_TYPE_DELETE = 2
} u3c_operation_type_t;

typedef struct u3c_credential_identifier_ {
  uint16_t uuid;
  u3c_credential_type type;
  uint16_t slot;
} u3c_credential_identifier;

typedef struct u3c_event_data_validate_ {
  u3c_credential * credential;
  bool is_unlocked;
} u3c_event_data_validate;

typedef struct u3c_event_data_learn_start_ {
  u3c_operation_type_t operation_type;
  u3c_credential_identifier learn_target;
  uint8_t timeout_seconds;
} u3c_event_data_learn_start;

typedef struct u3c_event_data_learn_read_done_ {
  uint8_t * data;
  uint8_t data_length;
} u3c_event_data_learn_read_done;

typedef enum u3c_event_ {
  CC_USER_CREDENTIAL_EVENT_VALIDATE            = 1,  ///< Application is requesting a Credential to be validated
  CC_USER_CREDENTIAL_EVENT_VALIDATE_VALID      = 2,  ///< Credential found and User has sufficient rights
  CC_USER_CREDENTIAL_EVENT_VALIDATE_INVALID    = 3,  ///< Credential not found or User has insufficient rights
  CC_USER_CREDENTIAL_EVENT_LEARN_START_REQUEST = 4,  ///< Application is requesting to start Credential Learn
  CC_USER_CREDENTIAL_EVENT_LEARN_START         = 5,  ///< Credential Learn process started
  CC_USER_CREDENTIAL_EVENT_LEARN_STEP_START    = 6,  ///< Application progressed to new Credential Learn step
  CC_USER_CREDENTIAL_EVENT_LEARN_READ_DONE     = 7,  ///< Credential data is ready to be entered into the database
  CC_USER_CREDENTIAL_EVENT_LEARN_SUCCESS       = 8,  ///< Credential has been accepted into the database
  CC_USER_CREDENTIAL_EVENT_LEARN_FAILED        = 9,
  CC_USER_CREDENTIAL_EVENT_LEARN_STEP_RETRY    = 10, ///< Local Credential read failed, trying to read again
  CC_USER_CREDENTIAL_EVENT_LEARN_TIMEOUT       = 11, ///< Credential Learn step timed out
  CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_REMOTE = 12, ///< Remote node cancelled Credential Learn process
  CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_LOCAL  = 13  ///< Application cancelled Credential Learn process
} u3c_event_type;

/****************************************************************************/
/*                              API FUNCTIONS                               */
/****************************************************************************/

/**
 * Validates a Credential according to the manufacturer's security rules
 *
 * @return True if the Credential conforms to the security rules
 */
bool CC_UserCredential_manufacturer_validate_credential(
  u3c_credential * credential
  );

/**
 * @}
 * @}
 */

#endif /* _COMMANDCLASSUSERCREDENTIAL_H_ */
