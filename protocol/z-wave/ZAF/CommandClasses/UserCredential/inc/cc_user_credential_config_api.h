/**
 * @file
 * User Credential Command Class configuration API.
 *
 * @copyright 2024 Silicon Laboratories Inc.
 */

#ifndef CC_USER_CREDENTIAL_CONFIG_API_H
#define CC_USER_CREDENTIAL_CONFIG_API_H

#include "CC_UserCredential.h"

bool cc_user_credential_is_user_type_supported(u3c_user_type user_type);

uint16_t cc_user_credential_get_max_user_unique_idenfitiers(void);

uint8_t cc_user_credential_get_max_length_of_user_name(void);

bool cc_user_credential_is_credential_type_supported(u3c_credential_type credential_type);

bool cc_user_credential_is_credential_learn_supported(u3c_credential_type credential_type);

bool cc_user_credential_is_credential_rule_supported(u3c_credential_rule credential_rule);

uint8_t cc_user_credential_get_number_of_supported_credential_types(void);

uint16_t cc_user_credential_get_max_credential_slots(u3c_credential_type credential_type);

uint16_t cc_user_credential_get_min_length_of_data(u3c_credential_type credential_type);

uint16_t cc_user_credential_get_max_length_of_data(u3c_credential_type credential_type);

uint8_t cc_user_credential_get_cl_recommended_timeout(u3c_credential_type credential_type);

uint8_t cc_user_credential_get_cl_number_of_steps(u3c_credential_type credential_type);

bool cc_user_credential_is_all_users_checksum_supported(void);

bool cc_user_credential_is_user_checksum_supported(void);

bool cc_user_credential_is_credential_checksum_supported(void);

bool cc_user_credential_is_all_users_checksum_supported(void);

bool cc_user_credential_is_user_checksum_supported(void);

bool cc_user_credential_is_credential_checksum_supported(void);

#endif /* CC_USER_CREDENTIAL_CONFIG_API_H */
