/***************************************************************************//**
 * @brief Return-code definitions for Ember Connect stack API functions.
 *
 * See @ref status_codes for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/**
 * Many Ember Connect API functions return an ::EmberStatus value to indicate
 * the success or failure of the call.
 * Return codes are one byte long.
 * This page documents the possible status codes and their meanings.
 *
 * See error-def.h for source code.
 *
 * See also error.h for information on how the values for the return codes are
 * built up from these definitions.
 * The file error-def.h is separated from error.h because utilities will use
 * this file to parse the return codes.
 *
 * @note Do not include error-def.h directly. It is included by
 * error.h inside an enum typedef, which is in turn included by sl_zigbee.h.
 */

#ifndef SILABS_ERROR_DEF_H
#define SILABS_ERROR_DEF_H

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberStatus
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
/**
 * @name Generic Messages
 * These messages are system wide.
 */
//@{

/**
 * @brief The generic "no error" message.
 */
  SL_ZIGBEE_SUCCESS = 0x00,

/**
 * @brief The generic "fatal error" message.
 */
  SL_ZIGBEE_ERR_FATAL = 0x01,

/**
 * @brief An invalid value was passed as an argument to a function.
 */
  SL_ZIGBEE_BAD_ARGUMENT = 0x02,

/**
 * @brief The manufacturing and stack token format in non-volatile memory
 * is different than what the stack expects (returned at initialization).
 */
  SL_ZIGBEE_EEPROM_MFG_STACK_VERSION_MISMATCH = 0x04,

/**
 * @brief The API call is not allowed given the current state of the
 * stack.
 */
  SL_ZIGBEE_INVALID_CALL = 0x70,

/**
 * @brief The manufacturing token format in non-volatile memory is
 * different than what the stack expects (returned at initialization).
 */
  SL_ZIGBEE_EEPROM_MFG_VERSION_MISMATCH = 0x06,

/**
 * @brief The stack token format in non-volatile memory is different
 * than what the stack expects (returned at initialization).
 */
  SL_ZIGBEE_EEPROM_STACK_VERSION_MISMATCH = 0x07,

//@} // END Generic Messages

/**
 * @name Packet Buffer Module Errors
 */
//@{

/**
 * @brief There are no more buffers.
 */
  SL_ZIGBEE_NO_BUFFERS = 0x18,

//@} / END Packet Buffer Module Errors

/**
 * @name Serial Manager Errors
 */
//@{

/**
 * @brief Specified an invalid baud rate.
 */
  SL_LEGACY_SERIAL_INVALID_BAUD_RATE = 0x20,

/**
 * @brief Specified an invalid serial port.
 */
  SL_LEGACY_SERIAL_INVALID_PORT = 0x21,

/**
 * @brief Tried to send too much data.
 */
  SL_LEGACY_SERIAL_TX_OVERFLOW = 0x22,

/**
 * @brief There was not enough space to store a received character
 * and the character was dropped.
 */
  SL_LEGACY_SERIAL_RX_OVERFLOW = 0x23,

/**
 * @brief Detected a UART framing error.
 */
  SL_LEGACY_SERIAL_RX_FRAME_ERROR = 0x24,

/**
 * @brief Detected a UART parity error.
 */
  SL_LEGACY_SERIAL_RX_PARITY_ERROR = 0x25,

/**
 * @brief There is no received data to process.
 */
  SL_LEGACY_SERIAL_RX_EMPTY = 0x26,

/**
 * @brief The receive interrupt was not handled in time, and a
 * character was dropped.
 */
  SL_LEGACY_SERIAL_RX_OVERRUN_ERROR = 0x27,

//@}

/**
 * @name MAC Errors
 */
//@{

/**
 * @brief No pending data exists for device doing a data poll.
 */
  SL_802154_NO_DATA = 0x31,

/**
 * @brief The frequency hopping client failed the frequency hopping
 * synchronization procedure, it timed out trying to reach the frequency hopping
 * server.
 */
  SL_802154_SYNC_TIMEOUT = 0x33,

/**
 * @brief The frequency hopping client failed the frequency hopping
 * synchronization procedure, the server is currently using a different seed.
 */
  SL_802154_SYNC_WRONG_SEED = 0x34,

/**
 * @brief MAC security operation failed.
 */
  SL_802154_SECURITY_FAILED = 0x35,

/**
 * @brief Transmission failed: the destination node does not appear in the
 * neighbor or child tables.
 */
  SL_802154_UNKNOWN_DESTINATION = 0x37,

/**
 * @brief Transmission failed: the local node does not support security or a
 * secured transmission has been requested to a child that does not support
 * security.
 */
  SL_802154_SECURITY_NOT_SUPPORTED = 0x38,

/**
 * @brief The MAC transmit queue is full.
 */
  SL_802154_TRANSMIT_QUEUE_FULL = 0x39,
// Internal

/**
 * @brief MAC ACK header received.
 */
  SL_802154_ACK_HEADER_TYPE = 0x3B,

/**
 * @brief The MAC can't complete this task because it is scanning.
 */
  SL_802154_SCANNING = 0x3D,

/**
 * @brief The requested operation cannot be completed because the MAC is
 * currently busy performing some high priority task.
 */
  SL_802154_BUSY = 0x3E,

/**
 * @brief We expected to receive an ACK following the transmission, but
 * the MAC level ACK was never received.
 */
  SL_802154_NO_ACK_RECEIVED = 0x40,

/**
 * @brief Indirect data message timed out before polled.
 */
  SL_802154_INDIRECT_TIMEOUT = 0x41,

/**
 * @brief Transmission failed: the indirect message was purged because the
 * destination child has been removed or updated.
 */
  SL_802154_INDIRECT_MESSAGE_PURGED = 0x42,

//@}

/**
 * @name  Flash Errors
 */
//@{

/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash.  The target memory attempting to be programmed is already programmed.
 * The flash write routines were asked to flip a bit from a 0 to 1, which is
 * physically impossible and the write was therefore inhibited.  The data in
 * the flash cannot be trusted after this error.
 */
  SL_ZIGBEE_ERR_FLASH_WRITE_INHIBITED = 0x46,

/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash and the write verification has failed.  The data in the flash
 * cannot be trusted after this error, and it is possible this error is the
 * result of exceeding the life cycles of the flash.
 */
  SL_ZIGBEE_ERR_FLASH_VERIFY_FAILED = 0x47,

/**
 * @brief A fatal error has occurred while trying to write data to the
 * flash, possibly due to write protection or an invalid address.  The data in
 * the flash cannot be trusted after this error, and it is possible this error
 * is the result of exceeding the life cycles of the flash.
 */
  SL_ZIGBEE_ERR_FLASH_PROG_FAIL = 0x4B,

/**
 * @brief A fatal error has occurred while trying to erase flash, possibly
 * due to write protection.  The data in the flash cannot be trusted after
 * this error, and it is possible this error is the result of exceeding the
 * life cycles of the flash.
 */
  SL_ZIGBEE_ERR_FLASH_ERASE_FAIL = 0x4C,

//@}

/**
 * @name  Bootloader Errors
 */
//@{

/**
 * @brief The bootloader received an invalid message (failed attempt
 * to go into bootloader).
 */
  SL_ZIGBEE_ERR_BOOTLOADER_TRAP_TABLE_BAD = 0x58,

/**
 * @brief Bootloader received an invalid message (failed attempt to go
 * into bootloader).
 */
  SL_ZIGBEE_ERR_BOOTLOADER_TRAP_UNKNOWN = 0x59,

/**
 * @brief The bootloader cannot complete the bootload operation because
 * either an image was not found or the image exceeded memory bounds.
 */
  SL_ZIGBEE_ERR_BOOTLOADER_NO_IMAGE = 0x05A,

//@}

/**
 * @name  Transport Errors
 */
//@{

/**
 * @brief The message to be transmitted is too big to fit into a
 * single over-the-air packet.
 */
  SL_ZIGBEE_MESSAGE_TOO_LONG = 0x74,

//@}

/**
 * @name  HAL Module Errors
 */
//@{

/**
 * @brief Conversion is complete.
 */
  SL_ZIGBEE_ADC_CONVERSION_DONE = 0x80,

/**
 * @brief Conversion cannot be done because a request is being
 * processed.
 */
  SL_ZIGBEE_ADC_CONVERSION_BUSY = 0x81,

/**
 * @brief Conversion is deferred until the current request has been
 * processed.
 */
  SL_ZIGBEE_ADC_CONVERSION_DEFERRED = 0x82,

/**
 * @brief No results are pending.
 */
  SL_ZIGBEE_ADC_NO_CONVERSION_PENDING = 0x84,

/**
 * @brief Sleeping (for a duration) has been abnormally interrupted
 * and exited prematurely.
 */
  SL_ZIGBEE_SLEEP_INTERRUPTED = 0x85,

//@}

/**
 * @name  PHY Errors
 */
//@{

/**
 * @brief The transmit hardware buffer underflowed.
 */
  SL_ZIGBEE_PHY_TX_UNDERFLOW = 0x88,

/**
 * @brief The transmit hardware did not finish transmitting a packet.
 */
  SL_ZIGBEE_PHY_TX_INCOMPLETE = 0x89,

/**
 * @brief An unsupported channel setting was specified.
 */
  SL_ZIGBEE_PHY_INVALID_CHANNEL = 0x8A,

/**
 * @brief An unsupported power setting was specified.
 */
  SL_ZIGBEE_PHY_INVALID_POWER = 0x8B,

/**
 * @brief The requested operation cannot be completed because the radio
 * is currently busy, either transmitting a packet or performing calibration.
 */
  SL_ZIGBEE_PHY_TX_BUSY = 0x8C,

/**
 * @brief The transmit attempt failed because all CCA attempts indicated that
 * the channel was busy.
 */
  SL_ZIGBEE_PHY_TX_CCA_FAIL = 0x8D,

/**
 * @brief The expected ACK was received after the last transmission.
 */
  SL_ZIGBEE_PHY_ACK_RECEIVED = 0x8F,

//@}

/**
 * @name  Return Codes Passed to sli_zigbee_stack_stack_status_handler()
 * See also ::sli_zigbee_stack_stack_status_handler().
 */
//@{

/**
 * @brief The stack software has completed initialization and is ready
 * to send and receive packets over the air.
 */
  SL_ZIGBEE_NETWORK_UP = 0x90,

/**
 * @brief The network is not operating.
 */
  SL_ZIGBEE_NETWORK_DOWN = 0x91,

/**
 * @brief The node failed to initiate the scanning process during the joining
 * process.
 */
  SL_ZIGBEE_JOIN_SCAN_FAILED = 0x92,

/**
 * @brief An attempt to join a network failed.
 */
  SL_ZIGBEE_JOIN_FAILED = 0x94,

/**
 * @brief An attempt to join a network was rejected.
 */
  SL_ZIGBEE_JOIN_DENIED = 0x95,

/**
 * @brief The node timed out waiting for a response during the joining process.
 */
  SL_ZIGBEE_JOIN_TIMEOUT = 0x96,

/** @brief An attempt to join or rejoin the network failed because
 *  no valid beacons could be heard by the joining node.
 */
  SL_ZIGBEE_NO_VALID_BEACONS = 0xAB,

//@}

/** @brief The security data provided was not valid, or an
 *    integrity check failed.
 */
  SL_ZIGBEE_SECURITY_DATA_INVALID = 0xBD,

//@}

/**
 * @name  Miscellaneous Network Errors
 */
//@{

/**
 * @brief The node has not joined a network.
 */
  SL_ZIGBEE_NOT_JOINED = 0x93,

//@}

/**
 * @name  Miscellaneous Utility Errors
 */
//@{

/**
 * @brief An index was passed into the function that was larger
 * than the valid range.
 */
  SL_ZIGBEE_INDEX_OUT_OF_RANGE = 0xB1,

/**
 * @brief There are no empty entries left in the table.
 */
  SL_ZIGBEE_TABLE_FULL = 0xB4,

/**
 * @brief The requested function cannot be executed because
 *   the library that contains the necessary functionality is not present.
 */
  SL_ZIGBEE_LIBRARY_NOT_PRESENT = 0xB5,

/**
 * @brief The stack accepted the command and is currently processing
 * the request.  The results will be returned via an appropriate handler.
 */
  SL_ZIGBEE_OPERATION_IN_PROGRESS = 0xBA,

//@}

/**
 * @name  NVM3 Token Errors
 */
//@{

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as no valid NVM3 page was found.
 */
  SL_ZIGBEE_NVM3_TOKEN_NO_VALID_PAGES = 0xC0,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance was already opened with other parameters.
 */
  SL_ZIGBEE_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS = 0xC1,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance is not aligned properly in memory.
 */
  SL_ZIGBEE_NVM3_ERR_ALIGNMENT_INVALID = 0xC2,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the size of the NVM3 instance is too small.
 */
  SL_ZIGBEE_NVM3_ERR_SIZE_TOO_SMALL = 0xC3,

/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *    as the NVM3 page size is not supported.
 */
  SL_ZIGBEE_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED = 0xC4,

/**
 * @brief NVM3 is telling the application that there was an error initializing
 * some of the tokens.
 */
  SL_ZIGBEE_NVM3_ERR_TOKEN_INIT = 0xC5,

/**
 * @brief NVM3 is telling the application that there has been an unknown
 * error.
 */
  SL_ZIGBEE_NVM3_ERR_UNKNOWN = 0xC7,

//@}

/**
 * @name  Application Errors
 * These error codes are available for application use.
 */
//@{

/**
 * @brief This error is reserved for customer application use.
 *  This will never be returned from any portion of the network stack or HAL.
 */
  SL_ZIGBEE_APPLICATION_ERROR_0 = 0xF0,
  SL_ZIGBEE_APPLICATION_ERROR_1 = 0xF1,
  SL_ZIGBEE_APPLICATION_ERROR_2 = 0xF2,
  SL_ZIGBEE_APPLICATION_ERROR_3 = 0xF3,
  SL_ZIGBEE_APPLICATION_ERROR_4 = 0xF4,
  SL_ZIGBEE_APPLICATION_ERROR_5 = 0xF5,
  SL_ZIGBEE_APPLICATION_ERROR_6 = 0xF6,
  SL_ZIGBEE_APPLICATION_ERROR_7 = 0xF7,
  SL_ZIGBEE_APPLICATION_ERROR_8 = 0xF8,
  SL_ZIGBEE_APPLICATION_ERROR_9 = 0xF9,
  SL_ZIGBEE_APPLICATION_ERROR_10 = 0xFA,
  SL_ZIGBEE_APPLICATION_ERROR_11 = 0xFB,
  SL_ZIGBEE_APPLICATION_ERROR_12 = 0xFC,
  SL_ZIGBEE_APPLICATION_ERROR_13 = 0xFD,
  SL_ZIGBEE_APPLICATION_ERROR_14 = 0xFE,
  SL_ZIGBEE_APPLICATION_ERROR_15 = 0xFF,
};
//@} // END name group

#endif // SILABS_ERROR_DEF_H
