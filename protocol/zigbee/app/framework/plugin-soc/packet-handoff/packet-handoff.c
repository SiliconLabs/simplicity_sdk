/***************************************************************************//**
 * @file
 * @brief Implementation of the packet-handoff mechanism.  Definitions for the
 * sl_zigbee_packet_handoff stack hooks, intercepting packets from the stack,
 * copying the data into a flat buffer, then passing the flat packet
 * to other plugins for further processing.
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

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "buffer_manager/legacy-packet-buffer.h"

#ifdef PRO_COMPLIANCE
 #include "app/test/pro-compliance.h"
 #include "app/test/pro-compliance-printing.h"
#endif

#ifndef PACKET_HANDOFF_BUFFER_SIZE
  #define PACKET_HANDOFF_BUFFER_SIZE 256
#endif

#include "packet-handoff-config.h"
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ALL_PACKETS == 1)
#define ALLOW_ALL_PACKETS
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_RAW_MAC == 1)
#define ALLOW_RAW_MAC
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_MAC_COMMAND == 1)
#define ALLOW_MAC_COMMAND
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA == 1)
#define ALLOW_NETWORK_DATA
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND == 1)
#define ALLOW_NETWORK_COMMAND
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_DATA == 1)
#define ALLOW_APS_DATA
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_COMMAND == 1)
#define ALLOW_APS_COMMAND
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZDO == 1)
#define ALLOW_ZDO
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZCL == 1)
#define ALLOW_ZCL
#endif
#if (SL_ZIGBEE_AF_PLUGIN_PACKET_HANDOFF_ALLOW_BEACON == 1)
#define ALLOW_BEACON
#endif
#define sl_zigbee_internal_packet_handoff_incoming_handler sli_zigbee_af_packet_handoff_incoming_callback
#define sl_zigbee_internal_packet_handoff_outgoing_handler sli_zigbee_af_packet_handoff_outgoing_callback

// Callbacks
sl_zigbee_packet_action_t sl_zigbee_af_incoming_packet_filter_cb(sl_zigbee_zigbee_packet_type_t packetType,
                                                                 uint8_t* packetData,
                                                                 uint8_t* size_p,
                                                                 void* data);

sl_zigbee_packet_action_t sl_zigbee_af_outgoing_packet_filter_cb(sl_zigbee_zigbee_packet_type_t packetType,
                                                                 uint8_t* packetData,
                                                                 uint8_t* size_p,
                                                                 void* data);

// ----------------------------------------------------------------
// Intermediate Packet Handoff Mechanism
// Converts ::sli_buffer_manager_buffer_t into a flat array then produces the callback
// to sl_zigbee_af_incoming_packet_filter_cb and sl_zigbee_af_outgoing_packet_filter_cb
// ----------------------------------------------------------------

sl_zigbee_packet_action_t sl_zigbee_internal_packet_handoff_incoming_handler(sl_zigbee_zigbee_packet_type_t packetType,
                                                                             sli_buffer_manager_buffer_t packetBuffer,
                                                                             uint8_t index,
                                                                             void *data)
{
  uint8_t flatPacket[PACKET_HANDOFF_BUFFER_SIZE];

  switch (packetType) {
    #ifndef ALLOW_ALL_PACKETS

      #ifndef ALLOW_RAW_MAC
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_RAW_MAC:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // RAW_MAC

      #ifndef ALLOW_MAC_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_MAC_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // MAC_COMMAND

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA_PRE_DECRYPTION
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_DATA_PRE_DECRYPTION:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND_PRE_DECRYPTION
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_COMMAND_PRE_DECRYPTION:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_NETWORK_DATA
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_DATA:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef ALLOW_NETWORK_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_APS_DATA
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_APS_DATA:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // APS_DATA

      #ifndef ALLOW_APS_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_APS_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // APS_COMMAND

      #ifndef ALLOW_ZDO
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_ZDO:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // ZDO

      #ifndef ALLOW_ZCL
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_ZCL:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // ZCL

      #ifndef ALLOW_BEACON
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_BEACON:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // BEACON

       #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_INTERPAN
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_INTERPAN:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // INTERPAN

    #endif // !ALLOW_ALL_PACKETS
    default:
    {
      // Proceed only if the index is with in the length of the packet.
      // This check is important to calculate the length before a buffer copy function.
      // is called.
      if (sl_legacy_buffer_manager_message_buffer_length(packetBuffer) < index) {
        // Return deafault action
        return SL_ZIGBEE_ACCEPT_PACKET;
      }
      uint8_t packetLength = sl_legacy_buffer_manager_message_buffer_length(packetBuffer) - index;
      sl_zigbee_packet_action_t act;
      sl_legacy_buffer_manager_copy_from_linked_buffers(packetBuffer,
                                                        index,
                                                        flatPacket,
                                                        packetLength);
#ifdef SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT
      extern sli_buffer_manager_buffer_t emTempHandoffHeader;
      emTempHandoffHeader = packetBuffer;
#endif
      act = sl_zigbee_af_incoming_packet_filter_cb(packetType, flatPacket, &packetLength, data);
#ifdef SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT
      emTempHandoffHeader = NULL_BUFFER;
#endif
      if (act == SL_ZIGBEE_MANGLE_PACKET) {
        sl_status_t status = sl_legacy_buffer_manager_set_linked_buffers_length(packetBuffer,
                                                                                packetLength + index);
        // If changing the length of the buffer is unsuccessful (ran out of free block)
        // tell the stack to drop the packet because something has probably gone wrong
        if (status != SL_STATUS_OK) {
          return SL_ZIGBEE_DROP_PACKET;
        } else {
          sl_legacy_buffer_manager_copy_to_linked_buffers(flatPacket,
                                                          packetBuffer,
                                                          index,
                                                          packetLength);
        }
      }
      return act;
    }
  }
}

sl_zigbee_packet_action_t sl_zigbee_internal_packet_handoff_outgoing_handler(sl_zigbee_zigbee_packet_type_t packetType,
                                                                             sli_buffer_manager_buffer_t packetBuffer,
                                                                             uint8_t index,
                                                                             void *data)
{
  uint8_t flatPacket[PACKET_HANDOFF_BUFFER_SIZE];

  switch (packetType) {
    #ifndef ALLOW_ALL_PACKETS

      #ifndef ALLOW_RAW_MAC
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_RAW_MAC:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // RAW_MAC

      #ifndef ALLOW_MAC_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_MAC_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // MAC_COMMAND

      #ifndef ALLOW_NETWORK_DATA
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_DATA:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_DATA

      #ifndef ALLOW_NETWORK_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_NWK_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // NETWORK_COMMAND

      #ifndef ALLOW_APS_DATA
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_APS_DATA:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // APS_DATA

      #ifndef ALLOW_APS_COMMAND
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_APS_COMMAND:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // APS_COMMAND

      #ifndef ALLOW_ZDO
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_ZDO:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // ZDO

      #ifndef ALLOW_ZCL
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_ZCL:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // ZCL

      #ifndef ALLOW_BEACON
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_BEACON:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // BEACON

      #ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_INTERPAN
    case SL_ZIGBEE_ZIGBEE_PACKET_TYPE_INTERPAN:
      return SL_ZIGBEE_ACCEPT_PACKET;
      #endif // ZLL

    #endif // !ALLOW_ALL_PACKETS
    default:
    {
      uint8_t packetLength = sl_legacy_buffer_manager_message_buffer_length(packetBuffer) - index;
      sl_zigbee_packet_action_t act;
      sl_legacy_buffer_manager_copy_from_linked_buffers(packetBuffer,
                                                        index,
                                                        flatPacket,
                                                        packetLength);
      if (sli_legacy_packet_buffer_packet_header_payload(packetBuffer) != SL_ZIGBEE_NULL_MESSAGE_BUFFER) {
        sli_buffer_manager_buffer_t payload = sli_legacy_packet_buffer_packet_header_payload(packetBuffer);
        uint8_t payloadLength = sl_legacy_buffer_manager_message_buffer_length(payload);
        sl_legacy_buffer_manager_copy_from_linked_buffers(payload,
                                                          0,
                                                          flatPacket + packetLength,
                                                          payloadLength);
        packetLength += payloadLength;
      }
      act = sl_zigbee_af_outgoing_packet_filter_cb(packetType, flatPacket, &packetLength, data);
      if (act == SL_ZIGBEE_MANGLE_PACKET) {
        sl_status_t status = sl_legacy_buffer_manager_set_linked_buffers_length(packetBuffer,
                                                                                packetLength + index);
        // If changing the length of the buffer is unsuccessful (ran out of free block)
        // tell the stack to drop the packet because something has probably gone wrong
        if (status != SL_STATUS_OK) {
          return SL_ZIGBEE_DROP_PACKET;
        } else {
          sl_legacy_buffer_manager_copy_to_linked_buffers(flatPacket,
                                                          packetBuffer,
                                                          index,
                                                          packetLength);
          if (sli_legacy_packet_buffer_packet_header_payload(packetBuffer) != SL_ZIGBEE_NULL_MESSAGE_BUFFER) {
            // The payload is now appended to the end of the packet
            sli_legacy_packet_buffer_set_packet_header_payload(packetBuffer, SL_ZIGBEE_NULL_MESSAGE_BUFFER);
          }
        }
      }
      return act;
    }
  }
}
