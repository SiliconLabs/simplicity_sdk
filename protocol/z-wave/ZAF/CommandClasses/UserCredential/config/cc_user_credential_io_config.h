/**
 * @file
 * @copyright 2024 Silicon Laboratories Inc.
 */

#ifndef _CC_USER_CREDENTIAL_IO_CONFIG_H_
#define _CC_USER_CREDENTIAL_IO_CONFIG_H_

/**
 * \defgroup configuration Configuration
 * Configuration
 *
 * \addtogroup configuration
 * @{
 */
/**
 * \defgroup command_class_user_credential_io_configuration Command Class User Credential I/O Configuration
 * Command Class User Credential I/O Configuration
 *
 * \addtogroup command_class_user_credential_io_configuration
 * @{
 */

/**
 * Size of buffer for holding User Names <1..255:1>
 *
 */
#if !defined(U3C_BUFFER_SIZE_USER_NAME)
#define U3C_BUFFER_SIZE_USER_NAME  255
#endif /* !defined(U3C_BUFFER_SIZE_USER_NAME) */

/**
 * Size of buffer for holding Credential Data <1..255:1>
 *
 */
#if !defined(U3C_BUFFER_SIZE_CREDENTIAL_DATA)
#define U3C_BUFFER_SIZE_CREDENTIAL_DATA  100
#endif /* !defined(U3C_BUFFER_SIZE_CREDENTIAL_DATA) */

/**
 * [SoC NVM driver] User Descriptor table buffer size <1..65535:1>
 *
 * Size of the buffer used for holding the User Descriptor table when it is read from NVM
 */
#if !defined(U3C_BUFFER_SIZE_USER_DESCRIPTORS)
#define U3C_BUFFER_SIZE_USER_DESCRIPTORS  20
#endif /* !defined(U3C_BUFFER_SIZE_USER_DESCRIPTORS) */

/**
 * [SoC NVM driver] Credential Descriptor table buffer size <1..65535:1>
 *
 * Size of the buffer used for holding the Credential Descriptor table when it is read from NVM
 */
#if !defined(U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS)
#define U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS  20
#endif /* !defined(U3C_BUFFER_SIZE_CREDENTIAL_DESCRIPTORS) */

/**@}*/ /* \addtogroup command_class_user_credential_io_configuration */

/**@}*/ /* \addtogroup configuration */
#endif /* _CC_USER_CREDENTIAL_IO_CONFIG_H_ */
