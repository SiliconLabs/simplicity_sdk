/**
 * @file
 * @copyright 2024 Silicon Laboratories Inc.
 */

#ifndef _CC_USER_CREDENTIAL_CONFIG_H_
#define _CC_USER_CREDENTIAL_CONFIG_H_

/**
 * \defgroup configuration Configuration
 * Configuration
 *
 * \addtogroup configuration
 * @{
 */
/**
 * \defgroup command_class_user_credential_configuration Command Class User Credential Configuration
 * Command Class User Credential Configuration
 *
 * \addtogroup command_class_user_credential_configuration
 * @{
 */

/**
 * Number of supported User Unique Identifiers <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_USER_UNIQUE_IDENTIFIERS)
#define CC_USER_CREDENTIAL_MAX_USER_UNIQUE_IDENTIFIERS  20
#endif /* !defined(CC_USER_CREDENTIAL_MAX_USER_UNIQUE_IDENTIFIERS) */

/**
 * Maximum length of User Name <1..255:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_LENGTH_USER_NAME)
#define CC_USER_CREDENTIAL_MAX_LENGTH_USER_NAME  15
#endif /* !defined(CC_USER_CREDENTIAL_MAX_LENGTH_USER_NAME) */

/**
 * Number of Supported Credential Slots for PIN Code data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PIN_CODE)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PIN_CODE  5
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PIN_CODE) */

/**
 * Number of Supported Credential Slots for Password data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PASSWORD)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PASSWORD  5
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_PASSWORD) */

/**
 * Number of Supported Credential Slots for RFID Code data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_RFID_CODE)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_RFID_CODE  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_RFID_CODE) */

/**
 * Number of Supported Credential Slots for BLE data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_BLE)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_BLE  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_BLE) */

/**
 * Number of Supported Credential Slots for NFC data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_NFC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_NFC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_NFC) */

/**
 * Number of Supported Credential Slots for UWB data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UWB)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UWB  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UWB) */

/**
 * Number of Supported Credential Slots for Eye Biometric data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_EYE_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_EYE_BIOMETRIC) */

/**
 * Number of Supported Credential Slots for Face Biometric data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FACE_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FACE_BIOMETRIC) */

/**
 * Number of Supported Credential Slots for Finger Biometric data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FINGER_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_FINGER_BIOMETRIC) */

/**
 * Number of Supported Credential Slots for Hand Biometric data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_HAND_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_HAND_BIOMETRIC) */

/**
 * Number of Supported Credential Slots for Unspecified Biometric data <1..65535:1>
 *
 * Also defines whether this Credential Type is supported or not. 0 Slots imply no support, any other number implies support.
 */
#if !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UNSPECIFIED_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_MAX_CREDENTIAL_SLOTS_UNSPECIFIED_BIOMETRIC) */

/**
 * Minimum length of PIN Code data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PIN_CODE)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PIN_CODE  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PIN_CODE) */

/**
 * Minimum length of Password data <1..65535:2>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PASSWORD)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PASSWORD  2
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_PASSWORD) */

/**
 * Minimum length of RFID Code data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_RFID_CODE)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_RFID_CODE  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_RFID_CODE) */

/**
 * Minimum length of BLE data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_BLE)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_BLE  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_BLE) */

/**
 * Minimum length of NFC data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_NFC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_NFC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_NFC) */

/**
 * Minimum length of UWB data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UWB)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UWB  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UWB) */

/**
 * Minimum length of Eye Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_EYE_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_EYE_BIOMETRIC) */

/**
 * Minimum length of Face Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FACE_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FACE_BIOMETRIC) */

/**
 * Minimum length of Finger Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FINGER_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_FINGER_BIOMETRIC) */

/**
 * Minimum length of Hand Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_HAND_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_HAND_BIOMETRIC) */

/**
 * Minimum length of Unspecified Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UNSPECIFIED_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_MIN_DATA_LENGTH_UNSPECIFIED_BIOMETRIC) */

/**
 * Maximum length of PIN Code data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PIN_CODE)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PIN_CODE  10
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PIN_CODE) */

/**
 * Maximum length of Password data <1..65535:2>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PASSWORD)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PASSWORD  20
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_PASSWORD) */

/**
 * Maximum length of RFID Code data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_RFID_CODE)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_RFID_CODE  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_RFID_CODE) */

/**
 * Maximum length of BLE data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_BLE)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_BLE  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_BLE) */

/**
 * Maximum length of NFC data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_NFC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_NFC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_NFC) */

/**
 * Maximum length of UWB data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UWB)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UWB  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UWB) */

/**
 * Maximum length of Eye Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_EYE_BIOMETRIC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_EYE_BIOMETRIC) */

/**
 * Maximum length of Face Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FACE_BIOMETRIC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FACE_BIOMETRIC) */

/**
 * Maximum length of Finger Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FINGER_BIOMETRIC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_FINGER_BIOMETRIC) */

/**
 * Maximum length of Hand Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_HAND_BIOMETRIC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_HAND_BIOMETRIC) */

/**
 * Maximum length of Unspecified Biometric data <1..65535:1>
 *
 */
#if !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UNSPECIFIED_BIOMETRIC  32
#endif /* !defined(CC_USER_CREDENTIAL_MAX_DATA_LENGTH_UNSPECIFIED_BIOMETRIC) */

/**
 * Support User Type 'General'
 *
 * Defines whether User Type 'General' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_GENERAL)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_GENERAL  1
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_GENERAL) */

/**
 * Support User Type 'Programming'
 *
 * Defines whether User Type 'Programming' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_PROGRAMMING)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_PROGRAMMING  1
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_PROGRAMMING) */

/**
 * Support User Type 'Non-Access'
 *
 * Defines whether User Type 'Non-Access' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_NON_ACCESS)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_NON_ACCESS  1
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_NON_ACCESS) */

/**
 * Support User Type 'Duress'
 *
 * Defines whether User Type 'Duress' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DURESS)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DURESS  0
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DURESS) */

/**
 * Support User Type 'Disposable'
 *
 * Defines whether User Type 'Disposable' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DISPOSABLE)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DISPOSABLE  0
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_DISPOSABLE) */

/**
 * Support User Type 'Expiring'
 *
 * Defines whether User Type 'Expiring' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_EXPIRING)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_EXPIRING  0
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_EXPIRING) */

/**
 * Support User Type 'Remote Only'
 *
 * Defines whether User Type 'Remote Only' is supported or not
 */
#if !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_REMOTE_ONLY)
#define CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_REMOTE_ONLY  0
#endif /* !defined(CC_USER_CREDENTIAL_USER_TYPE_SUPPORTED_REMOTE_ONLY) */

/**
 * Support Credential Learn functionality for Credential Type 'PIN Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PIN_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PIN_CODE  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PIN_CODE) */

/**
 * Support Credential Learn functionality for Credential Type 'Password'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PASSWORD)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PASSWORD  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_PASSWORD) */

/**
 * Support Credential Learn functionality for Credential Type 'RFID Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_RFID_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_RFID_CODE  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_RFID_CODE) */

/**
 * Support Credential Learn functionality for Credential Type 'BLE'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_BLE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_BLE  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_BLE) */

/**
 * Support Credential Learn functionality for Credential Type 'NFC'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_NFC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_NFC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_NFC) */

/**
 * Support Credential Learn functionality for Credential Type 'UWB'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UWB)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UWB  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UWB) */

/**
 * Support Credential Learn functionality for Credential Type 'Eye Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_EYE_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_EYE_BIOMETRIC) */

/**
 * Support Credential Learn functionality for Credential Type 'Face Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FACE_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FACE_BIOMETRIC) */

/**
 * Support Credential Learn functionality for Credential Type 'Finger Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FINGER_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_FINGER_BIOMETRIC) */

/**
 * Support Credential Learn functionality for Credential Type 'Hand Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_HAND_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_HAND_BIOMETRIC) */

/**
 * Support Credential Learn functionality for Credential Type 'Unspecified Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UNSPECIFIED_BIOMETRIC  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_SUPPORTED_UNSPECIFIED_BIOMETRIC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'PIN Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PIN_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PIN_CODE  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PIN_CODE) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Password'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PASSWORD)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PASSWORD  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_PASSWORD) */

/**
 * Recommended Credential Learn timeout for Credential Type 'RFID Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_RFID_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_RFID_CODE  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_RFID_CODE) */

/**
 * Recommended Credential Learn timeout for Credential Type 'BLE'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_BLE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_BLE  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_BLE) */

/**
 * Recommended Credential Learn timeout for Credential Type 'NFC'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_NFC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_NFC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_NFC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'UWB'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UWB)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UWB  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UWB) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Eye Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_EYE_BIOMETRIC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_EYE_BIOMETRIC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Face Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FACE_BIOMETRIC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FACE_BIOMETRIC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Finger Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FINGER_BIOMETRIC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_FINGER_BIOMETRIC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Hand Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_HAND_BIOMETRIC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_HAND_BIOMETRIC) */

/**
 * Recommended Credential Learn timeout for Credential Type 'Unspecified Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UNSPECIFIED_BIOMETRIC  20
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_RECOMMENDED_TIMEOUT_UNSPECIFIED_BIOMETRIC) */

/**
 * Credential Learn Number of Steps for Credential Type 'PIN Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PIN_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PIN_CODE  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PIN_CODE) */

/**
 * Credential Learn Number of Steps for Credential Type 'Password'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PASSWORD)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PASSWORD  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_PASSWORD) */

/**
 * Credential Learn Number of Steps for Credential Type 'RFID Code'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_RFID_CODE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_RFID_CODE  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_RFID_CODE) */

/**
 * Credential Learn Number of Steps for Credential Type 'BLE'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_BLE)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_BLE  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_BLE) */

/**
 * Credential Learn Number of Steps for Credential Type 'NFC'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_NFC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_NFC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_NFC) */

/**
 * Credential Learn Number of Steps for Credential Type 'UWB'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UWB)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UWB  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UWB) */

/**
 * Credential Learn Number of Steps for Credential Type 'Eye Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_EYE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_EYE_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_EYE_BIOMETRIC) */

/**
 * Credential Learn Number of Steps for Credential Type 'Face Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FACE_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FACE_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FACE_BIOMETRIC) */

/**
 * Credential Learn Number of Steps for Credential Type 'Finger Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FINGER_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FINGER_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_FINGER_BIOMETRIC) */

/**
 * Credential Learn Number of Steps for Credential Type 'Hand Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_HAND_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_HAND_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_HAND_BIOMETRIC) */

/**
 * Credential Learn Number of Steps for Credential Type 'Unspecified Biometric'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UNSPECIFIED_BIOMETRIC)
#define CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UNSPECIFIED_BIOMETRIC  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_LEARN_NUMBER_OF_STEPS_UNSPECIFIED_BIOMETRIC) */

/**
 * Support Credential Rule 'Single'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_SINGLE)
#define CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_SINGLE  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_SINGLE) */

/**
 * Support Credential Rule 'Dual'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_DUAL)
#define CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_DUAL  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_DUAL) */

/**
 * Support Credential Rule 'Triple'
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_TRIPLE)
#define CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_TRIPLE  0
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_RULE_SUPPORTED_TRIPLE) */

/**
 * Support All Users Checksum
 *
 */
#if !defined(CC_USER_CREDENTIAL_ALL_USERS_CHECKSUM_SUPPORTED)
#define CC_USER_CREDENTIAL_ALL_USERS_CHECKSUM_SUPPORTED  1
#endif /* !defined(CC_USER_CREDENTIAL_ALL_USERS_CHECKSUM_SUPPORTED) */

/**
 * Support User Checksum
 *
 */
#if !defined(CC_USER_CREDENTIAL_USER_CHECKSUM_SUPPORTED)
#define CC_USER_CREDENTIAL_USER_CHECKSUM_SUPPORTED  1
#endif /* !defined(CC_USER_CREDENTIAL_USER_CHECKSUM_SUPPORTED) */

/**
 * Support Credential Checksum
 *
 */
#if !defined(CC_USER_CREDENTIAL_CREDENTIAL_CHECKSUM_SUPPORTED)
#define CC_USER_CREDENTIAL_CREDENTIAL_CHECKSUM_SUPPORTED  1
#endif /* !defined(CC_USER_CREDENTIAL_CREDENTIAL_CHECKSUM_SUPPORTED) */

/**@}*/ /* \addtogroup command_class_user_credential_configuration */

/**@}*/ /* \addtogroup configuration */
#endif /* _CC_USER_CREDENTIAL_CONFIG_H_ */
