/***************************************************************************//**
 * @file
 * @brief ZigBee 3.0 ota server test harness functionality
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

#include "app/framework/include/af.h"

#include "test-harness-z3-core.h"

// -----------------------------------------------------------------------------
extern void sl_zigbee_af_set_no_reply_for_next_message(bool set);

#define INVALID_MANUFACTURER_ID  0xFFFF
#define INVALID_DEVICE_ID        0xFFFF
#define INVALID_FIRMWARE_VERSION 0xFFFFFFFFUL
#define INVALID_EUI64 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

#define HARDWARE_VERSIONS_PRESENT_MASK                 0x0004

#ifndef EZSP_HOST
static sl_zigbee_af_ota_image_id_t loadedOtaImageId = {
  INVALID_MANUFACTURER_ID,
  INVALID_DEVICE_ID,
  INVALID_FIRMWARE_VERSION,
  INVALID_EUI64,
};
static sl_zigbee_af_ota_header_t loadedOTAFileHeader = {
  // Magic Number omitted since it is always the same.
  .headerVersion = 0x0100,
  .headerLength  = 0x0038,
  .fieldControl  = 0x0000,
  .manufacturerId = 0xffff,
  .imageTypeId    = 0xffff,           // a.k.a. Device ID
  .firmwareVersion = 0xffffffff,
  .zigbeeStackVersion = 0x0002,

  /**
   * @brief The spec. does NOT require that the string be NULL terminated in the
   *   header stored on disk.  Therefore we make sure we can support a
   *   32-character string without a NULL terminator by adding +1 in the data
   *   structure.
   */
  //.headerString[SL_ZIGBEE_AF_OTA_MAX_HEADER_STRING_LENGTH + 1] = {0},

  /**
   * @brief When reading the header this will be the complete length of
   *  the file. When writing the header, this must be set to
   *  the length of the MFG image data portion including all tags.
   */
  .imageSize = 0,

  /**
   * @brief The remaining four fields are optional. The field control should be checked
   *   to determine if their values are valid.
   */
  .securityCredentials = 0,
  .upgradeFileDestination = { 0xff },
  .minimumHardwareVersion = 0,
  .maximumHardwareVersion = 0
};
#endif

WEAK(sl_zigbee_af_ota_image_id_t sl_zigbee_af_ota_storage_search_cb(int16u manufacturerId,
                                                                    int16u imageTypeId,
                                                                    const int16u* hardwareVersion))
{
  sl_zigbee_af_ota_image_id_t InvalidId = {
    INVALID_MANUFACTURER_ID,
    INVALID_DEVICE_ID,
    INVALID_FIRMWARE_VERSION,
    INVALID_EUI64,
  };

  if (loadedOTAFileHeader.manufacturerId == manufacturerId
      && loadedOTAFileHeader.imageTypeId == imageTypeId) {
    return loadedOtaImageId;
  }
  (void)hardwareVersion;
  return InvalidId;
}
WEAK(void emAfOtaLoadFileCommand(void))
{
  // TODO: dummy file for the test harness
}

WEAK(sl_zigbee_af_ota_storage_status_t sl_zigbee_af_ota_storage_get_full_header_cb(const sl_zigbee_af_ota_image_id_t* id,
                                                                                   sl_zigbee_af_ota_header_t* returnData))
{
  if (id->manufacturerId == loadedOtaImageId.manufacturerId
      && id->imageTypeId == loadedOtaImageId.imageTypeId) {
    *returnData = loadedOTAFileHeader;
    return SL_ZIGBEE_AF_OTA_STORAGE_SUCCESS;
  }

  // If the requested image cannot be found, then an error should be returned.
  return SL_ZIGBEE_AF_OTA_STORAGE_ERROR;
}

/** @brief Ota Storage Read Image Data
 *
 * This callback reads data from the specified OTA file and returns that data to
 * the caller.
 *
 * @param id This is a pointer to the image id for the OTA file to retrieve data
 * from.  Ver.: always
 * @param offset This is the offset relative to the start of the image where the
 * data should be read from.  Ver.: always
 * @param length This is the length of data that will be read.  Ver.: always
 * @param returnData This is a pointer to where the data read out of the file
 * will be written to  Ver.: always
 * @param returnedLength This is a pointer to a variable where the actual length
 * of data read will be written to.  A short read may occur if the end of file
 * was reached.  Ver.: always
 */
WEAK(sl_zigbee_af_ota_storage_status_t sl_zigbee_af_ota_storage_read_image_data_cb(const sl_zigbee_af_ota_image_id_t* id,
                                                                                   uint32_t offset,
                                                                                   uint32_t length,
                                                                                   uint8_t* returnData,
                                                                                   uint32_t* returnedLength))
{
  // If the requested image cannot be found, then an error should be returned.
  if (id != NULL) {
    if (id->manufacturerId == loadedOtaImageId.manufacturerId
        && id->imageTypeId == loadedOtaImageId.imageTypeId) {
      sl_zigbee_af_set_no_reply_for_next_message(true);
      return SL_ZIGBEE_AF_OTA_STORAGE_SUCCESS;
    }
  }
  return SL_ZIGBEE_AF_OTA_STORAGE_ERROR;
}
/** @brief Ota Storage Get Total Image Size
 *
 * This function returns the total size of the ZigBee Over-the-air file with the
 * passed parameters.  If no file is found with those parameters, 0 is returned.
 *
 * @param id A pointer to the image identifier for the OTA file to retrieve
 * information for.  Ver.: always
 */
WEAK(uint32_t sl_zigbee_af_ota_storage_get_total_image_size_cb(const sl_zigbee_af_ota_image_id_t* id))
{
  if (id != NULL) {
    if (id->manufacturerId == loadedOtaImageId.manufacturerId
        && id->imageTypeId == loadedOtaImageId.imageTypeId) {
      return loadedOTAFileHeader.imageSize;
    }
  }
  // On failure this should return an image size of zero.
  return 0;
}

WEAK(bool sl_zigbee_af_is_ota_image_id_valid(const sl_zigbee_af_ota_image_id_t* idToCompare))
{
  sl_zigbee_af_ota_image_id_t InvalidId = {
    INVALID_MANUFACTURER_ID,
    INVALID_DEVICE_ID,
    INVALID_FIRMWARE_VERSION,
    INVALID_EUI64,
  };

  if (idToCompare != NULL) {
    // Check if the loaded image is a valid?
    if (idToCompare->manufacturerId != InvalidId.manufacturerId
        && idToCompare->imageTypeId != InvalidId.imageTypeId
        /*&& idToCompare->firmwareVersion != InvalidId.firmwareVersion*/) {
      // Then check if the requested image does match the loaded?
      if (idToCompare->manufacturerId == loadedOtaImageId.manufacturerId
          && idToCompare->imageTypeId == loadedOtaImageId.imageTypeId) {
        return true;
      }
    }
  }
  return false;
}

// OTA Server commands
// plugin test-harness z3 ota-server activate <shortAddress:2> <endpoint:1> <manufacturereCode:2> <imageType:2> <fileVersion:4> <fileSize:4>
//static sl_zigbee_af_ota_image_id_t otaImageId;
void sli_zigbee_af_test_harness_z3_ota_server_activate_command(SL_CLI_COMMAND_ARG)
{
  sl_status_t status = SL_STATUS_INVALID_STATE;
#ifndef EZSP_HOST
  sl_802154_short_addr_t shortAddress = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 1);
  loadedOTAFileHeader.manufacturerId = sl_cli_get_argument_uint16(arguments, 2);
  loadedOTAFileHeader.imageTypeId = sl_cli_get_argument_uint16(arguments, 3);
  loadedOTAFileHeader.firmwareVersion = sl_cli_get_argument_uint32(arguments, 4);
  loadedOTAFileHeader.imageSize = sl_cli_get_argument_uint32(arguments, 5);

  loadedOtaImageId.manufacturerId = loadedOTAFileHeader.manufacturerId;
  loadedOtaImageId.imageTypeId = loadedOTAFileHeader.imageTypeId;
  loadedOtaImageId.firmwareVersion = loadedOTAFileHeader.firmwareVersion;

  status = sl_zigbee_af_ota_server_send_image_notify_cb(shortAddress,
                                                        endpoint,
                                                        0x03,//payloadType,
                                                        60,//jitter,
                                                        &loadedOtaImageId);

#endif /* EZSP_HOST */
  sl_zigbee_af_core_println("%s: %s: 0x%X",
                            TEST_HARNESS_Z3_PRINT_NAME,
                            "Ota Server activate",
                            status);
}

// plugin test-harness z3 ota-server deactivate
void sli_zigbee_af_test_harness_z3_ota_server_de_activate_command(SL_CLI_COMMAND_ARG)
{
  sl_status_t status = SL_STATUS_INVALID_STATE;
#ifndef EZSP_HOST
  sl_zigbee_af_ota_image_id_t InvalidId = {
    INVALID_MANUFACTURER_ID,
    INVALID_DEVICE_ID,
    INVALID_FIRMWARE_VERSION,
    INVALID_EUI64,
  };
  loadedOtaImageId = InvalidId;
  status = SL_STATUS_OK;
#endif /* EZSP_HOST */

  sl_zigbee_af_core_println("%s: %s: 0x%X",
                            TEST_HARNESS_Z3_PRINT_NAME,
                            "Ota Server Deactivate",
                            status);
}

WEAK(uint8_t sl_zigbee_af_ota_server_query_cb(const sl_zigbee_af_ota_image_id_t* currentImageId,
                                              uint16_t* hardwareVersion,
                                              sl_zigbee_af_ota_image_id_t* nextUpgradeImageId))
{
  // This function is called by the OTA cluster server to determine what
  // the 'next' version of software is for a particular device requesting
  // a new download image.  The server returns a status code indicating
  // SL_ZIGBEE_ZCL_STATUS_NO_IMAGE_AVAILABLE, or SL_ZIGBEE_ZCL_STATUS_SUCCESS
  // (new image is available).  It then also fills in the 'nextUpgradeImageId'
  // structure with the appropriate version.

  // The server can use whatever criteria it wants to dictate what
  // the 'next' version is and if it is currently available.
  // This sample does this based on a global policy value.

  uint8_t status = SL_ZIGBEE_ZCL_STATUS_NO_IMAGE_AVAILABLE;
  bool hardwareVersionMismatch = false;
  *nextUpgradeImageId
    = sl_zigbee_af_ota_storage_search_cb(currentImageId->manufacturerId,
                                         currentImageId->imageTypeId,
                                         hardwareVersion);

  if (sl_zigbee_af_is_ota_image_id_valid(nextUpgradeImageId)) {
    // We only perform a check if both the query and the
    // file have hardware version(s).  If one or the other doesn't
    // have them, we assume a match is still possible.
    if (hardwareVersion) {
      sl_zigbee_af_ota_header_t header;
      sl_zigbee_af_ota_storage_get_full_header_cb(nextUpgradeImageId,
                                                  &header);
      if (header.fieldControl & HARDWARE_VERSIONS_PRESENT_MASK) {
        if (*hardwareVersion < header.minimumHardwareVersion
            || header.maximumHardwareVersion < *hardwareVersion) {
          hardwareVersionMismatch = true;
        }
      }
    }
    // "!hardwareVersionMismatch" does not mean the hardware
    // versions match.  It just means we don't *disqualify* the image
    // as a potential upgrade candidate because the hardware is out
    // of range.
    if (!hardwareVersionMismatch) {
      status = SL_ZIGBEE_ZCL_STATUS_SUCCESS;
    }
  }
  return status;
}

WEAK(uint8_t sl_zigbee_af_ota_server_image_block_request_cb(sl_zigbee_af_image_block_request_callback_struct_t* data))
{
  // The Block response is generated from the Zigbee 3.0 Test Tool, hence just
  // return success
  return SL_ZIGBEE_ZCL_STATUS_SUCCESS;
}

WEAK(bool sl_zigbee_af_ota_server_upgrade_end_request_cb(sl_802154_short_addr_t source,
                                                         uint8_t status,
                                                         uint32_t* returnValue,
                                                         const sl_zigbee_af_ota_image_id_t* imageId))
{
  // Response is generated by the Zigbee 3.0 Test Tool
  sl_zigbee_af_set_no_reply_for_next_message(true);
  return true;
}
