/***************************************************************************//**
 * @file
 * @brief Code for manipulating the incoming and outgoing messages in a flat
 * memory buffer.
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
#include "app/framework/util/util.h"
#include "app/framework/util/config.h"

#include "app/framework/signature-decode/sl_signature_decode.h"

//------------------------------------------------------------------------------

// these variables are for storing responses that are created by zcl-utils
// in functions called from emberIncomingMsgHandler. The response is sent
// from sl_zigbee_af_tick (meant to be called immediately after sl_zigbee_tick).
// There is only space for one response as each call to sl_zigbee_tick will
// only result in a single call to emberIncomingMsgHandler. If the device
// receives multiple ZCL messages, the stack will queue these and hand
// these to the application via emberIncomingMsgHandler one at a time.
sl_zigbee_aps_frame_t      sl_zigbee_af_response_aps_frame;
sl_802154_short_addr_t        sl_zigbee_af_response_destination;
uint8_t  appResponseData[SL_ZIGBEE_AF_RESPONSE_BUFFER_LEN];
uint16_t  appResponseLength;

// Used for empty string
static uint16_t zeroLenByte = 0;
static uint8_t* zeroLenBytePtr = (uint8_t *)&zeroLenByte;

//------------------------------------------------------------------------------
// Utilities for adding bytes to the response buffer: appResponseData. These
// functions take care of incrementing appResponseLength.

void sl_zigbee_af_clear_response_data(void)
{
  sl_zigbee_af_response_type = ZCL_UTIL_RESP_NORMAL;
  // To prevent accidentally sending to someone else,
  // set the destination to ourselves.
  sl_zigbee_af_response_destination = sl_zigbee_af_get_node_id();
  memset(appResponseData, 0, SL_ZIGBEE_AF_RESPONSE_BUFFER_LEN);
  appResponseLength = 0;
  memset(&sl_zigbee_af_response_aps_frame, 0, sizeof(sl_zigbee_aps_frame_t));
}

uint8_t * sl_zigbee_af_put_int8u_in_resp(uint8_t value)
{
  //sl_zigbee_af_debug_print("try %x max %x\r\n", appResponseLength, SL_ZIGBEE_AF_RESPONSE_BUFFER_LEN);
  if (appResponseLength < SL_ZIGBEE_AF_RESPONSE_BUFFER_LEN) {
    //sl_zigbee_af_debug_print("put %x at spot %x\r\n", value, appResponseLength);
    appResponseData[appResponseLength] = value;
    appResponseLength++;
    return &appResponseData[appResponseLength - 1];
  }

  return NULL;
}

uint16_t * sl_zigbee_af_put_int16u_in_resp(uint16_t value)
{
  uint8_t * low = sl_zigbee_af_put_int8u_in_resp(LOW_BYTE(value));
  uint8_t * high = sl_zigbee_af_put_int8u_in_resp(HIGH_BYTE(value));

  if (low && high) {
    return (uint16_t *) low;
  } else {
    return NULL;
  }
}

uint32_t * sl_zigbee_af_put_int32u_in_resp(uint32_t value)
{
  uint8_t * a =  sl_zigbee_af_put_int8u_in_resp(BYTE_0(value));
  uint8_t * b =  sl_zigbee_af_put_int8u_in_resp(BYTE_1(value));
  uint8_t * c =  sl_zigbee_af_put_int8u_in_resp(BYTE_2(value));
  uint8_t * d =  sl_zigbee_af_put_int8u_in_resp(BYTE_3(value));

  if (a && b && c && d) {
    return (uint32_t *)a;
  } else {
    return NULL;
  }
}

uint32_t * sl_zigbee_af_put_int24u_in_resp(uint32_t value)
{
  uint8_t * a = sl_zigbee_af_put_int8u_in_resp(BYTE_0(value));
  uint8_t * b = sl_zigbee_af_put_int8u_in_resp(BYTE_1(value));
  uint8_t * c = sl_zigbee_af_put_int8u_in_resp(BYTE_2(value));

  if (a && b && c) {
    return (uint32_t *) a;
  } else {
    return NULL;
  }
}

uint8_t * sl_zigbee_af_put_block_in_resp(const uint8_t* data, uint16_t length)
{
  if ((appResponseLength + length) < SL_ZIGBEE_AF_RESPONSE_BUFFER_LEN) {
    memmove(appResponseData + appResponseLength, data, length);
    appResponseLength += length;
    return &appResponseData[appResponseLength - length];
  } else {
    return NULL;
  }
}

uint8_t * sl_zigbee_af_put_string_in_resp(const uint8_t *buffer)
{
  uint8_t length = sl_zigbee_af_string_length(buffer);
  return sl_zigbee_af_put_block_in_resp(buffer, length + 1);
}

uint8_t * sl_zigbee_af_put_date_in_resp(sl_zigbee_af_date_t *value)
{
  uint8_t * a = sl_zigbee_af_put_int8u_in_resp(value->year);
  uint8_t * b = sl_zigbee_af_put_int8u_in_resp(value->month);
  uint8_t * c = sl_zigbee_af_put_int8u_in_resp(value->dayOfMonth);
  uint8_t * d = sl_zigbee_af_put_int8u_in_resp(value->dayOfWeek);

  if (a && b && c && d) {
    return a;
  } else {
    return NULL;
  }
}

// ------------------------------------
// Utilities for reading from RAM buffers (reading from incoming message
// buffer)
// ------------------------------------

// retrieves an uint32_t which contains between 1 and 4 bytes of relevent data
// depending on number of bytes requested.
uint32_t sl_zigbee_af_get_int(const uint8_t* message,
                              uint16_t currentIndex,
                              uint16_t msgLen,
                              uint8_t bytes)
{
  // This printout needs to stay here, otherwise `./z run linkkey` test fails.
  // We should rework the test so we can remove this....
  if ((currentIndex + bytes) > msgLen) {
    sl_zigbee_af_debug_println("GetInt, %x bytes short", bytes);
    sl_zigbee_af_debug_flush();
    return 0;
  }
  return sl_signature_decode_int(message, currentIndex, msgLen, bytes);
}

uint32_t sl_zigbee_af_get_int32u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen)
{
  return sl_zigbee_af_get_int(message, currentIndex, msgLen, 4);
}

uint32_t sl_zigbee_af_get_int24u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen)
{
  return sl_zigbee_af_get_int(message, currentIndex, msgLen, 3);
}

uint16_t sl_zigbee_af_get_int16u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen)
{
  return (uint16_t)sl_zigbee_af_get_int(message, currentIndex, msgLen, 2);
}

uint8_t* sl_zigbee_af_get_string(uint8_t* message, uint16_t currentIndex, uint16_t msgLen)
{
  // Strings must contain at least one byte for the length.
  if (msgLen <= currentIndex) {
    sl_zigbee_af_debug_println("GetString: %p", "buffer too short");
    return zeroLenBytePtr;
  }

  // Starting from the current index, there must be enough bytes in the message
  // for the string and the length byte.
  if (msgLen < currentIndex + sl_zigbee_af_string_length(&message[currentIndex]) + 1) {
    sl_zigbee_af_debug_println("GetString: %p", "len byte wrong");
    return zeroLenBytePtr;
  }

  return &message[currentIndex];
}

uint8_t* sl_zigbee_af_get_long_string(uint8_t* message, uint16_t currentIndex, uint16_t msgLen)
{
  // Long strings must contain at least two bytes for the length.
  if (msgLen <= currentIndex + 1) {
    sl_zigbee_af_debug_println("GetLongString: %p", "buffer too short");
    return zeroLenBytePtr;
  }

  // Starting from the current index, there must be enough bytes in the message
  // for the string and the length bytes.
  if (msgLen
      < currentIndex + sl_zigbee_af_long_string_length(&message[currentIndex]) + 2) {
    sl_zigbee_af_debug_println("GetLongString: %p", "len bytes wrong");
    return zeroLenBytePtr;
  }

  return &message[currentIndex];
}

uint8_t sl_zigbee_af_string_length(const uint8_t *buffer)
{
  // The first byte specifies the length of the string.  A length of 0xFF means
  // the string is invalid and there is no character data.
  return (buffer[0] == 0xFF ? 0 : buffer[0]);
}

uint16_t sl_zigbee_af_long_string_length(const uint8_t *buffer)
{
  // The first two bytes specify the length of the long string.  A length of
  // 0xFFFF means the string is invalid and there is no character data.
  uint16_t length = sl_zigbee_af_get_int16u(buffer, 0, 2);
  return (length == 0xFFFF ? 0 : length);
}

uint8_t sl_zigbee_af_get_date(uint8_t* message,
                              uint16_t currentIndex,
                              uint16_t msgLen,
                              sl_zigbee_af_date_t *destination)
{
  if ((currentIndex + 4) > msgLen) {
    sl_zigbee_af_debug_println("GetDate, %x bytes short", 4);
    sl_zigbee_af_debug_flush();
    return 0;
  }
  destination->year       = message[(currentIndex + 0)];
  destination->month      = message[(currentIndex + 1)];
  destination->dayOfMonth = message[(currentIndex + 2)];
  destination->dayOfWeek  = message[(currentIndex + 3)];
  return 4;
}

uint8_t sli_decode_parse_one_byte(sl_zigbee_af_cluster_command_t *cmd, uint16_t payloadOffset)
{
  return (cmd->buffer)[payloadOffset];
}

uint16_t sli_decode_parse_two_bytes(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return sl_zigbee_af_get_int16u(cmd->buffer, payloadOffset, cmd->bufLen);
}

uint32_t sli_decode_parse_three_bytes(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return sl_zigbee_af_get_int24u(cmd->buffer, payloadOffset, cmd->bufLen);
}

uint32_t sli_decode_parse_four_bytes(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return sl_zigbee_af_get_int32u(cmd->buffer, payloadOffset, cmd->bufLen);
}

uint8_t *sli_decode_string(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return sl_zigbee_af_get_string(cmd->buffer, payloadOffset, cmd->bufLen);
}

uint8_t *sli_decode_long_string(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return sl_zigbee_af_get_long_string(cmd->buffer, payloadOffset, cmd->bufLen);
}

uint8_t *sli_decode_bytes(sl_zigbee_af_cluster_command_t * cmd, uint16_t payloadOffset)
{
  return cmd->buffer + payloadOffset;
}
