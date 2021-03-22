/**
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: 2024 Silicon Laboratories Inc.
 */
#include "app_credentials.h"
#include "AppTimer.h"
#include "SwTimer.h"
#include "CC_UserCredential.h"
#include "cc_user_credential_io.h"
#include "cc_user_credential_config_api.h"
#include "events.h"
#include "CC_DoorLock.h"

static SSwTimer CredentialLearnTimer;

// PIN code for simulating manual User Credential Entry
uint8_t credential_learn_pin_code[] = { '1', '1', '6', '7' };
u3c_event_data_learn_read_done credential_learn_done_data = {
  .data = credential_learn_pin_code,
  .data_length = sizeof(credential_learn_pin_code)
};
uint8_t credential_learn_steps = 1;

static void CredentialLearnTimerCallback(SSwTimer *pTimer)
{
  TimerStop(pTimer);
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_READ_DONE,
    &credential_learn_done_data);
}

void credentials_init(void)
{
  /* Timer for simulating the User entering a Credential */
  AppTimerRegister(&CredentialLearnTimer, false, CredentialLearnTimerCallback);
}

void initialize_user_credential_database(void)
{
  if (!CC_UserCredential_get_next_user(0)) {
    uint16_t uuid = 1;
    unsigned char name[] = "Admin";
    u3c_user user = {
      .active = true,
      .unique_identifier = uuid,
      .modifier_node_id = 0,
      .expiring_timeout_minutes = 0,
      .name_length = sizeof(name) - 1,
      .type = USER_TYPE_PROGRAMMING,
      .modifier_type = MODIFIER_TYPE_LOCALLY,
      .credential_rule = CREDENTIAL_RULE_SINGLE,
      .name_encoding = USER_NAME_ENCODING_STANDARD_ASCII,
    };
    CC_UserCredential_add_user(&user, name);

    unsigned char pin_code[] = { 0x31, 0x32, 0x33, 0x34 };
    u3c_credential credential = {
      .metadata = {
        .uuid = uuid,
        .type = CREDENTIAL_TYPE_PIN_CODE,
        .slot = 1,
        .modifier_node_id = 0,
        .length = sizeof(pin_code),
        .modifier_type = MODIFIER_TYPE_LOCALLY
      },
      .data = pin_code
    };
    CC_UserCredential_add_credential(&credential);
  }
}

void request_credential_from_user(void)
{
  zaf_event_distributor_enqueue_cc_event(
    COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_STEP_START,
    &credential_learn_steps
    );
  // Simulate the User entering their Credential with a timer
  TimerStart(&CredentialLearnTimer, PIN_CODE_ENTRY_SECONDS * 1000);
}

void user_credential_app_event_handler(const uint8_t event, const void *data)
{
  switch (event) {
    case CC_USER_CREDENTIAL_EVENT_VALIDATE_VALID:
      zaf_event_distributor_enqueue_cc_event(COMMAND_CLASS_DOOR_LOCK, CC_DOOR_LOCK_CODE_EVENT_TOGGLE, NULL);
      break;
    case CC_USER_CREDENTIAL_EVENT_LEARN_START: {
      if (*(u3c_credential_type *)data == CREDENTIAL_TYPE_PIN_CODE) {
        // Request app to read PIN code
        zaf_event_distributor_enqueue_app_event(EVENT_APP_CREDENTIAL_LEARN_START);
      } else {
        // Other Credential types are not supported, cancel the operation
        zaf_event_distributor_enqueue_cc_event(COMMAND_CLASS_USER_CREDENTIAL, CC_USER_CREDENTIAL_EVENT_LEARN_CANCEL_LOCAL, NULL);
      }
      break;
    }
  }
}

ZAF_EVENT_DISTRIBUTOR_REGISTER_CC_EVENT_HANDLER(COMMAND_CLASS_USER_CREDENTIAL, user_credential_app_event_handler);
