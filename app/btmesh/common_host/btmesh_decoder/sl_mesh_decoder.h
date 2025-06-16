/***************************************************************************//**
 * @file sl_mesh_decoder.h
 * @brief Bluetooth Mesh message decoder component header file
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_MESH_DECODER_H
#define SL_MESH_DECODER_H

// -----------------------------------------------------------------------------
// Includes
#include <stdint.h>
#include <sl_status.h>
#include "sl_btmesh_sensor_model_capi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// -----------------------------------------------------------------------------
// Macros

/// Invalid cryptographic key
#define SL_MESH_KEY_HANDLE_INVALID  ~0
/// Maximum network PDU length
#define SL_MESH_MAX_NET_PDU_LEN     29
/// Maximum transport PDU length
#define SL_MESH_MAX_CONTROL_PDU_LEN 256
/// Maximum access message length
#define SL_MESH_MAX_ACCESS_PDU_LEN  384
/// Property ID length
#define SL_MESH_PROPERTY_ID_LEN     2

// -----------------------------------------------------------------------------
// Enums, structs, typedefs

/// Mesh beacon type enumeration
enum sl_mesh_beacon {
  sl_mesh_beacon_unprovisioned = 0x00, //< Unprovisioned device beacon
  sl_mesh_beacon_secure = 0x01,        //< Secure network beacon
  sl_mesh_beacon_private = 0x02,       //< Mesh private beacon
};

/// Transport control message opcode
enum sl_mesh_control_opcode {
  /** Segment acknowledgement */
  sl_mesh_control_opcode_segack = 0x00, // Strictly speaking a separate entity
  /** Friend Poll message */
  sl_mesh_control_opcode_friend_poll = 0x01,
  /** Friend Update message */
  sl_mesh_control_opcode_friend_update = 0x02,
  /** Friend Request message */
  sl_mesh_control_opcode_friend_request = 0x03,
  /** Friend Offer message */
  sl_mesh_control_opcode_friend_offer = 0x04,
  /** Friend Clear message */
  sl_mesh_control_opcode_friend_clear = 0x05,
  /** Friend Clear Confirm message */
  sl_mesh_control_opcode_friend_clear_confirm = 0x06,
  /** Friend Subscription List Add message */
  sl_mesh_control_opcode_friend_subscription_add = 0x07,
  /** Friend Subscription List Remove message */
  sl_mesh_control_opcode_friend_subscription_remove = 0x08,
  /** Friend Subscription List Confirm message */
  sl_mesh_control_opcode_friend_subscription_confirm = 0x09,
  /** Heartbeat message */
  sl_mesh_control_opcode_heartbeat = 0x0a,
  /** Path Request message */
  sl_mesh_control_opcode_path_request = 0x0b,
  /** Path Reply message */
  sl_mesh_control_opcode_path_reply = 0x0c,
  /** Path Confirmation message */
  sl_mesh_control_opcode_path_confirmation = 0x0d,
  /** Path Echo Request message */
  sl_mesh_control_opcode_path_echo_request = 0x0e,
  /** Path Echo Reply message */
  sl_mesh_control_opcode_path_echo_reply = 0x0f,
  /** Dependent Node Update message */
  sl_mesh_control_opcode_dependent_node_update = 0x10,
  /** Path Request Solicitation message */
  sl_mesh_control_opcode_path_request_solicitation = 0x11,
};

/// Decoder context
struct sl_mesh_decoder;

/// Mesh friendship key material context
struct sl_mesh_friendship {
  uint16_t lpn_addr;       //< LPN address
  uint16_t lpn_counter;    //< LPN counter
  uint16_t friend_addr;    //< Friend address
  uint16_t friend_counter; //< Friend counter
};

/// Mesh network PDU
struct sl_mesh_net_pdu {
  uint8_t pdu_len; //< PDU length
  uint8_t pdu_data[SL_MESH_MAX_NET_PDU_LEN]; //< PDU data
};

/// Access message
struct sl_mesh_access_pdu {
  uint8_t pdu_len; //< Access message data
  uint8_t pdu_data[SL_MESH_MAX_ACCESS_PDU_LEN]; //< Access message length
};

/// Cryptographic key abstraction
typedef void* sl_mesh_key_handle;

/// This structure contains information about a marshalled property,
/// including its format, data length, and associated sensor property ID.
typedef struct {
  uint8_t format;                   //< Marshalled property format
  uint8_t length;                   //< Data length
  sensor_property_id_t property_id; //< Sensor Property ID
} marshalled_property;

/// Represents the content of a Sensor Status message.
typedef struct {
  marshalled_property marshalled_property_id; //< Sensor Property ID
  uint16_t raw_value_length;                  //< Data length
  uint8_t *raw_value;                         //< Raw value
} sensor_status_msg_t;

/// Represents the content of a Sensor Column Get message.
typedef struct {
  sensor_property_id_t property_id; //< Sensor Property ID
  uint16_t raw_value_length;        //< Data length/
  uint8_t *raw_value;               //< Raw setting value
} sensor_get_column_msg_t;

/// Represents the content of a Sensor Column Status message.
typedef struct {
  sensor_property_id_t property_id; //< Sensor Property ID
  uint16_t raw_value_length;        //< Data length
  uint8_t *raw_value_x;             //< Raw value X
  uint8_t *column_width;            //< Column Width
  uint8_t *raw_value_y;             //< Raw value Y
} sensor_column_status_msg_t;

/// Represents the content of a Sensor Series Get message.
typedef struct {
  sensor_property_id_t property_id; //< Sensor Property ID
  uint16_t raw_value_length;        //< Data length
  uint8_t *raw_value_x1;            //< Raw value X1
  uint8_t *raw_value_x2;            //< Raw value X2
} sensor_get_series_msg_t;

/// Represents the content of a Sensor Series Status message.
typedef struct {
  sensor_property_id_t property_id; //< Sensor Property ID
  uint16_t raw_value_length;        //< Data length
  uint8_t *raw_value_x;             //< Raw value X
  uint8_t *column_width;            //< Column Width
  uint8_t *raw_value_y;             //< Raw value Y
} sensor_series_status_msg_t;

// -----------------------------------------------------------------------------
// Function declarations

/**
 * @mainpage Bluetooth Mesh Message Decoder Library
 *
 * @section intro Introduction
 *
 * This library provides utilities for decrypting, authenticating, and
 * decoding Bluetooth Mesh messages. It relies on the PSA Crypto API
 * for cryptographic functionality.
 *
 * The library supports the following Mesh stack features:
 *     - Decrypting and authenticating Mesh Network PDUs with
 *           - Flooding credentials
 *           - Directed forwarding credentials
 *           - Friendship credentials (provided that friendship
 *             establishment is observed)
 *     - Decoding decrypted Mesh Network PDU contents
 *     - Decoding Lower Transport PDU contents
 *     - Reassembling segmented Upper Transport messages
 *           - Provided that all segments are observed
 *     - Decoding Transport Control messages
 *     - Decrypting and authenticating Access messages
 *     - Authenticating Mesh beacons
 *     - Decoding Mesh beacons
 *
 * The library does *not* support the following Mesh stack features:
 *     - Network cache
 *           - It is up to the application to process or not process
 *             data that is decoded from receives messages when it
 *             can be retransmitted
 *     - Replay protection
 *           - It is up to the application to process or not process
 *             data that is decoded from receives messages when it
 *             can be replayed
 *     - Scanning for Mesh traffic
 *           - It is the application's responsibility to supply
 *             scanned data to the library
 *     - Decoding model messages
 *           - This is up to the application or a higher layer
 *             library
 *     - GATT traffic
 *           - Potentially to be added later
 *     - The provisioning protocol
 *           - Potentially to be added later
 *
 * @section quickstart Quickstart
 *
 * When building an application the library needs to be configured accordingly by
 * - Setting the optional feature support;
 * - Setting the number of encryption keys to support;
 * - Setting the number of label UUIDs to support;
 * - Setting the number of SAR reassemblies to support;
 * - Setting the number of friendships to track;
 *
 * These can be done by editing sl_mesh_decoder_config.h
 *
 * Furthermore, it needs to be ensured that PSA Crypto is compiled
 * with enough resources to manage all necessary encryption keys
 * including all derivations for the network key (seven if directed
 * forwarding credentials are supported) and friendship derivations
 * (two per friendship).
 *
 * An application using the library should do the following:
 *
 * - Initialize a decoder context: sl_mesh_decoder_create()
 * - Install an initial set of encryption keys:
 *     - sl_mesh_decoder_add_network_key() -- note that this
 *       will generate multiple derived keys internally
 *     - sl_mesh_decoder_add_application_key()
 *     - sl_mesh_decoder_add_device_key()
 * - Install a set of label UUIDs (if any) by sl_mesh_decoder_add_virtual_label_uuid()
 * - Know the current IV index for the network
 * - Feed incoming network PDU data to sl_mesh_decoder_decrypt_net_pdu()
 * - If decryption succeeds:
 *     - Examine the network PDU contents, starting with sl_mesh_net_pdu_is_valid()
 *     - Examine the lower transport PDU within network PDU payload, starting with sl_mesh_net_pdu_contains_valid_trans_pdu()
 *     - Make use of the SAR reassembler for segmented messages (sl_mesh_reassembler_add_segment())
 *     - Examine transport control message contents, starting with sl_mesh_pointer_contains_valid_control_message()
 *         - Make use of friendship tracking (sl_mesh_friendship_add_friend_request(), sl_mesh_friendship_add_friend_offer())
 *     - Feed access message data to sl_mesh_decoder_decrypt_access_pdu()
 *     - If decryption succeeds:
 *         - Examine access message contents, starting with sl_mesh_access_pdu_contains_valid_access_message()
 *
 */

// -------------------------------
// Library initialization / deinitialization

/**
 * @brief Create a decoder context
 *
 * @param context Pointer to an allocated context is returned here on success
 * @return SL_STATUS_OK if a context has been allocated and is ready to use
 */
sl_status_t sl_mesh_decoder_create(struct sl_mesh_decoder **context);

/**
 * @brief Delete a decoder context
 *
 * @param context Context to delete
 * @return SL_STATUS_OK if the given context was deleted
 */
sl_status_t sl_mesh_decoder_delete(struct sl_mesh_decoder *context);

// -------------------------------
// Crypto key management

/**
 * @brief Add a network key
 *
 * Add a known network key to a decoder. The decoder will internally
 * construct the various derivations Mesh Protocol uses, and use those
 * to decrypt and authenticate Mesh network PDUs.
 *
 * @param context Context to use
 * @param key_value Exactly 16 octets of AES key material
 * @param handle A reference to the key is returned here on success
 * @return SL_STATUS_OK if the key has been added
 */
sl_status_t sl_mesh_decoder_add_network_key(struct sl_mesh_decoder *context,
                                            const uint8_t *key_value,
                                            sl_mesh_key_handle *handle);

/**
 * @brief Add an application key
 *
 * Add a known application key to a decoder. The decoder will use the
 * key to decrypt and authenticate Mesh access messages.
 *
 * @param context Context to use
 * @param key_value Exactly 16 octets of AES key material
 * @param handle A reference to the key is returned here on success
 * @return SL_STATUS_OK if the key has been added
 */
sl_status_t sl_mesh_decoder_add_application_key(struct sl_mesh_decoder *context,
                                                const uint8_t *key_value,
                                                sl_mesh_key_handle *handle);

/**
 * @brief Add a device key
 *
 * Add a known device key to a decoder. The decoder will use the
 * key to decrypt and authenticate Mesh access messages.
 *
 * @param context Context to use
 * @param key_value Exactly 16 octets of AES key material
 * @param device_addr Primary address of the device; used to match keys to messages
 * @param handle A reference to the key is returned here on success
 * @return SL_STATUS_OK if the key has been added
 */
sl_status_t sl_mesh_decoder_add_device_key(struct sl_mesh_decoder *context,
                                           const uint8_t *key_value,
                                           uint16_t device_addr,
                                           sl_mesh_key_handle *handle);

/**
 * @brief Remove a key by reference
 *
 * @param context Context to use
 * @param handle Reference to the key to remove
 * @return SL_STATUS_OK if the key has been removed
 */
sl_status_t sl_mesh_decoder_remove_key(struct sl_mesh_decoder *context,
                                       sl_mesh_key_handle handle);

/**
 * @brief Get friendship context for a key
 *
 * If a network PDU is encrypted using friendship key material,
 * sl_mesh_decoder_decrypt_pdu() will return a reference to the
 * friendship key that decrypted the PDU successfully. The
 * friendship context can be extracted by this API call.
 *
 * @param context Context to use
 * @param handle Reference to the friendship key to query
 * @param friendship The friendship context will be returned here
 * @return SL_STATUS_OK if the key was a friendship key and the context was read;
 * SL_STATUS_NOT_SUPPORTED if library was compiled without friendship support.
 */
sl_status_t sl_mesh_decoder_get_friendship_key_context(struct sl_mesh_decoder *context,
                                                       sl_mesh_key_handle handle,
                                                       struct sl_mesh_friendship *friendship);

/**
 * @brief Get a friendship key by LPN friendship parameters
 *
 * @param context Context to use
 * @param lpn_addr LPN address for the friendship
 * @param lpn_counter LPN counter for the friendship
 * @param handle The friendship key handle is returned here
 * @return SL_STATUS_OK if the friendship key was returned;
 * SL_STATUS_NOT_SUPPORTED if library was compiled without friendship support.
 */
sl_status_t sl_mesh_decoder_get_friendship_key(struct sl_mesh_decoder *context,
                                               uint16_t lpn_addr,
                                               uint16_t lpn_counter,
                                               sl_mesh_key_handle *handle);

/**
 * @brief Remove a friendship key by LPN address and network key
 *
 * @param context Context to use
 * @param lpn_addr LPN address for the friendship
 * @param net_key_handle Network key from which friendship credentials were derived
 * @return SL_STATUS_OK if the friendship key was removed;
 * SL_STATUS_NOT_SUPPORTED if library was compiled without friendship support.
 */
sl_status_t sl_mesh_decoder_remove_friendship_key(struct sl_mesh_decoder *context,
                                                  uint16_t lpn_addr,
                                                  sl_mesh_key_handle net_key_handle);

// -------------------------------
// Network layer decoding

/**
 * @brief Decrypt a network PDU
 *
 * Decrypt a network PDU. The decoder will try to decrypt and
 * authenticate the given encrypted network PDU with the encryption
 * keys it has; and if successful, it will return the decrypted PDU
 * contents as well as a reference to the matching network key, and a
 * reference to the matching friendship key derivation in case the PDU
 * was encrypted using friendship key material that could be decoded.
 *
 * On successful decryption the pdu structure will contain the
 * plaintext IVI and NID values; the deobfuscated CTL, TTL, sequence
 * number, and source address values; the decrypted and authenticated
 * destination address and payload; and the plaintext NetMIC at the
 * end for reference.
 *
 * @param context Context to use
 * @param current_ivindex Network IV Index value
 * @param net_pdu_ptr Encrypted network PDU data
 * @param net_pdu_len Encrypted network PDU length
 * @param pdu The decrypted PDU is returned here
 * @param matching_net_key A reference to the matching network key is returned here
 * @param matching_friend_key A reference to the matching friendship is returned here,
 * if the PDU was encrypted with friendship key material; otherwise,
 * SL_MESH_KEY_HANDLE_INVALID is returned.
 * @return SL_STATUS_OK if the PDU is successfully decrypted and authenticated
 */
sl_status_t sl_mesh_decoder_decrypt_net_pdu(struct sl_mesh_decoder *context,
                                            uint32_t current_ivindex,
                                            const uint8_t *net_pdu_ptr,
                                            size_t net_pdu_len,
                                            struct sl_mesh_net_pdu *pdu,
                                            sl_mesh_key_handle *matching_net_key,
                                            sl_mesh_key_handle *matching_friend_key);

/**
 * @brief Verify NetPDU length
 *
 * Verify that a decrypted NetPDU is long enough to parse. Note that
 * this only verifies that there is enough data to read all network
 * PDU headers, that there is some payload, and that there is MIC
 * data; it does not check the header field values or the higher layer
 * PDU structure.
 *
 * This API must be called before calling any of the network
 * PDU parsing functions, if the PDU structure is not created
 * by sl_mesh_decoder_decrypt_net_pdu().
 *
 * @param pdu Decrypted Network PDU
 * @return true if the Network PDU contains enough data to be parsed
 */
bool sl_mesh_net_pdu_is_valid(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU IVI header
 *
 * @param pdu Decrypted Network PDU
 * @return Low bit of the IV index used to send the PDU
 */
uint32_t sl_mesh_net_pdu_ivi(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU NID header
 *
 * @param pdu Decrypted Network PDU
 * @return NID header value
 */
uint8_t sl_mesh_net_pdu_nid(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU CTL header
 *
 * Get the CTL header value of a networkPDU.  This indicates whether
 * the lower transport PDU contained in the network PDU is a control
 * message or an access message.
 *
 * @param pdu Decrypted Network PDU
 * @return CTL header value
 */
uint8_t sl_mesh_net_pdu_ctl(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU TTL header
 *
 * @param pdu Decrypted Network PDU
 * @return TTL header value
 */
uint8_t sl_mesh_net_pdu_ttl(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU sequence number
 *
 * @param pdu Decrypted Network PDU
 * @return Sequence number value
 */
uint32_t sl_mesh_net_pdu_seq(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU source address
 *
 * @param pdu Decrypted Network PDU
 * @return Source address value
 */
uint16_t sl_mesh_net_pdu_src(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU destination address
 *
 * @param pdu Decrypted Network PDU
 * @return Destination address value
 */
uint16_t sl_mesh_net_pdu_dst(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU TransportPDU data
 *
 * @param pdu Decrypted Network PDU
 * @return Pointer to the beginning of the TransportPDU
 */
const uint8_t *sl_mesh_net_pdu_data_ptr(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU TransportPDU length
 *
 * @param pdu Decrypted Network PDU
 * @return TransportPDU length
 */
size_t sl_mesh_net_pdu_data_len(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU NetMIC data
 *
 * @param pdu Decrypted Network PDU
 * @return Pointer to the beginning of NetMIC
 */
const uint8_t *sl_mesh_net_pdu_mic_ptr(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Network PDU NetMIC length
 *
 * @param pdu Decrypted Network PDU
 * @return NetMIC length
 */
size_t sl_mesh_net_pdu_mic_len(const struct sl_mesh_net_pdu *pdu);

// -------------------------------
// Lower transport layer decoding

/**
 * @brief Verify TransportPDU length
 *
 * Verify that the TransportPDU field in a decrypted PDU is long
 * enough to parse. Note that this only verifies that there is enough
 * data to read all lower transport layer headers and that there is
 * some payload; it does not check the header field values or the
 * higher layer PDU structure.
 *
 * This API must be called before calling any of the lower transport
 * PDU parsing functions.
 *
 * @param pdu Decrypted Network PDU
 * @return true if the TransportPDU field contains enough data to be parsed
 */
bool sl_mesh_net_pdu_contains_valid_trans_pdu(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU SEG header
 *
 * Get the SEG header value of a lower transport PDU.  This
 * indicates whether the lower transport PDU is segmented or not.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the SEG header
 */
uint8_t sl_mesh_trans_pdu_seg(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU SeqZero header
 *
 * Get the SeqZero header value of a segmented lower transport PDU.
 * This function should only be called for PDUs that are segmented.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the SeqZero header
 */
uint16_t sl_mesh_trans_pdu_seg_seq0(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU SegO header
 *
 * Get the SegO header value of a segmented lower transport PDU.
 * This function should only be called for PDUs that are segmented.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the SegO header
 */
uint8_t sl_mesh_trans_pdu_seg_o(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU SegN header
 *
 * Get the SegN header value of a segmented lower transport PDU.
 * This function should only be called for PDUs that are segmented.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the SegN header
 */
uint8_t sl_mesh_trans_pdu_seg_n(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU AKF header
 *
 * Get the AKF header value of a lower transport access message. This
 * function should only be called for PDUs that contain access
 * messages.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the AKF header
 */
uint8_t sl_mesh_trans_pdu_access_akf(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU AID header
 *
 * Get the AID header value of a lower transport access message. This
 * function should only be called for PDUs that contain access
 * messages.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the AID header
 */
uint8_t sl_mesh_trans_pdu_access_aid(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU SZMIC header
 *
 * Get the SZMIC header value of a lower transport access message. This
 * function should only be called for PDUs that contain access
 * messages. For non-segmented access messages this function will always
 * return 0.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the SZMIC header
 */
uint8_t sl_mesh_trans_pdu_access_szmic(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU Opcode header
 *
 * Get the Opcode header value of a lower transport control
 * message. This function should only be called for PDUs that contain
 * control messages.
 *
 * @param pdu Decrypted Network PDU
 * @return Value of the Opcode header
 */
uint8_t sl_mesh_trans_pdu_control_opcode(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU payload data
 *
 * Get a pointer to the beginning of lower transport layer PDU payload.
 * This returns a pointer to the beginning of upper transport access PDU
 * for unsegmented access messages; a pointer to the beginning of an
 * access message segment for segmented access messages; a pointer to
 * the beginning of control message parameters for unsegmented control
 * messages; and a pointer to the beginning of a control message segment
 * for segmented control messages.
 *
 * @param pdu Decrypted Network PDU
 * @return Pointer to lower transport PDU payload
 */
const uint8_t *sl_mesh_trans_pdu_data_ptr(const struct sl_mesh_net_pdu *pdu);

/**
 * @brief Get Lower Transport PDU payload length
 *
 * @param pdu Decrypted Network PDU
 * @return Lower transport PDU payload length
 */
size_t sl_mesh_trans_pdu_data_len(const struct sl_mesh_net_pdu *pdu);

// -------------------------------
// Transport layer reassembly

/**
 * @brief Add segment data to SAR reassembly
 *
 * Segmented access and control message data can be reassembled by the decoder
 * as long as all segment data is received at least once. This function should
 * be called when segmented data is received. The decoder will make the full
 * message available to the application when all segment data is at hand.
 *
 * The decoder will do a consistency check on incoming segments and will not
 * consider badly formatted segments or segments with conflicting parameters
 * for reassembly.
 *
 * The caller does not need to filter for retransmitted segments as the
 * decoder will take care of that.
 *
 * Reassembler structures are internally allocated and deallocated by
 * the decoder. The maximum number of parallel reassemblies that can
 * be attempted is defined by the #MAX_REASSEMBLER_COUNT macro.
 *
 * @param context Context to use
 * @param net_key Reference to the network key the data was encrypted with
 * @param ctl CTL header for the network PDU the data was received in
 * @param seq Sequence number for the network PDU the data was received in
 * @param src Source address for the network PDU the data was received in
 * @param dst Destination address for the network PDU the data was received in
 * @param seq_0 SeqZero for the lower transport PDU the data was received in
 * @param seg_o SegO for the lower transport PDU the data was received in
 * @param seg_n SegN for the lower transport PDU the data was received in
 * @param ctl_opcode Control opcode; ignored for access messages
 * @param akf AKF flag; ignored for control messages
 * @param aid AID value; ignored for control messages
 * @param szmic SZMIC flag; ignored for control messages
 * @param segment_ptr Pointer to segment data
 * @param segment_len Length of segment data
 * @param now_completed Set to true on first reception of a segment that
 * completes the reassembly; the caller can make a decision on when to
 * process the full message data based on this value, if it wants to process
 * the data only once.
 * @return SL_STATUS_OK if segment data has been successfully processed
 * for reassembly.
 */
sl_status_t sl_mesh_reassembler_add_segment(struct sl_mesh_decoder *context,
                                            sl_mesh_key_handle net_key,
                                            uint8_t ctl,
                                            uint32_t seq,
                                            uint16_t src,
                                            uint16_t dst,
                                            uint16_t seq_0,
                                            uint8_t seg_o,
                                            uint8_t seg_n,
                                            uint8_t ctl_opcode,
                                            uint8_t akf,
                                            uint8_t aid,
                                            uint8_t szmic,
                                            const uint8_t *segment_ptr,
                                            size_t segment_len,
                                            bool *now_completed);

/**
 * @brief Get reassembled data
 *
 * Get the reassembled access or control message data, if possible.
 *
 * @param context Context to use
 * @param src Source address for the network PDU the data was received in
 * @param dst Destination address for the network PDU the data was received in
 * @param seq_0 SeqZero for the lower transport PDU the data was received in
 * @param ptr Pointer to the beginning of reassembled data is returned here
 * @param len Length of the reassembled data is returned here
 * @param seq_auth The SeqAuth value for the reassembled message, used in
 * access message decryption and authentication.
 * @return SL_STATUS_OK if reassembled data is available and could be returned
 */
sl_status_t sl_mesh_reassembler_get_data(struct sl_mesh_decoder *context,
                                         uint16_t src,
                                         uint16_t dst,
                                         uint16_t seq_0,
                                         const uint8_t **ptr,
                                         size_t *len,
                                         uint32_t *seq_auth);

// -------------------------------
// Transport control

/**
 * @brief Verify upper transport control message length
 *
 * Verify that the upper transport control message is long enough to
 * parse. Note that this only verifies that there is enough data to
 * read all parameters for the given opcode; it does not check
 * parameter value validity.
 *
 * This API must be called before starting to interpret the data.
 *
 * @param opcode Transport control opcode
 * @param param_ptr Parameters array data
 * @param param_len Parameters array length
 * @return true if the parameters array contains enough data to be parsed
 */
bool sl_mesh_pointer_contains_valid_control_message(uint8_t opcode,
                                                    const uint8_t *param_ptr,
                                                    size_t param_len);

/**
 * @brief Verify upper transport access PDU length
 *
 * Verify that the upper transport access PDU is long enough to
 * parse. Note that this only verifies that there is enough data to
 * read all fields; it does not check field value validity.
 *
 * This API must be called before starting to interpret the data.
 *
 * @param szmic The SZMIC value for the PDU
 * @param pdu_ptr PDU data array
 * @param pdu_len PDU data length
 * @return true if PDU data array contains enough data to be parsed
 */
bool sl_mesh_pointer_contains_valid_access_pdu(uint8_t szmic,
                                               const uint8_t *pdu_ptr,
                                               size_t pdu_len);

// -------------------------------
// Friendship

/**
 * @brief Add a Friend Request for pending friendship tracking
 *
 * The decoder must be made aware of the friendships it should keep track of
 * as friendship negotiation is taking place. When a Friend Request PDU that
 * is sent by an LPN that is of interest is seen, the application should call
 * this API to add the pending friendship into the list of friendships to
 * follow.
 *
 * Once decoder is aware of both a Friend Request and the matching
 * Friend Offer -- see sl_mesh_friendship_add_friend_offer() -- it can
 * track a friendship that is established by being able to decode a
 * PDU (such as a Friend Poll) that is encrypted with friendship key
 * material derived from the parameters carried in these PDUs. It will
 * do this automatically, provided that it is able to store the
 * derived key material for use.
 *
 * The decoder will internally manage the resources for friendship tracking.
 * The macro #MAX_INCOMPLETE_FRIENDSHIP_COUNT defines how many pending
 * friendships can be tracked in parallel; and the macro #MAX_KEYS affects
 * how many friendship key derivations can be stored in addition to the
 * other cryptographic keys.
 *
 * The application should call sl_mesh_decoder_remove_key() to drop friendship
 * key material for any friendships that are no longer of interest, such as
 * terminated ones, so that it can derive keys for new friendships as needed.
 *
 * @param context The context to use
 * @param net_key Reference to the network key from which friendship key material is derived
 * @param lpn_addr Address of the LPN making the Friend Request
 * @param lpn_counter LPN's friendship counter
 * @return SL_STATUS_OK if the pending friendship is being monitored;
 * SL_STATUS_NOT_SUPPORTED if library was compiled without friendship support.
 */
sl_status_t sl_mesh_friendship_add_friend_request(struct sl_mesh_decoder *context,
                                                  sl_mesh_key_handle net_key,
                                                  uint16_t lpn_addr,
                                                  uint16_t lpn_counter);

/**
 * @brief Add a Friend Offer for pending friendship tracking
 *
 * This is the Friend Offer control message counterpart for
 * sl_mesh_friendship_add_friend_request().
 *
 * @param context The context to use
 * @param net_key Reference to the network key from which friendship key material is derived
 * @param lpn_addr Address of the LPN that made a Friend Request
 * @param friend_addr Address of the Friend that made a Friend Offer
 * @param friend_counter Friend's friendship counter
 * @return SL_STATUS_OK if the pending friendship is being monitored;
 * SL_STATUS_NOT_SUPPORTED if library was compiled without friendship support.
 */
sl_status_t sl_mesh_friendship_add_friend_offer(struct sl_mesh_decoder *context,
                                                sl_mesh_key_handle net_key,
                                                uint16_t lpn_addr,
                                                uint16_t friend_addr,
                                                uint16_t friend_counter);

// -------------------------------
// Access layer decoding

/**
 * @brief Add a virtual address label UUID
 *
 * Virtual address label UUIDs need to be known by the decoder in order
 * to be able to decrypt and authenticate access messages that are sent
 * to virtual address destinations.
 *
 * @param context The context to use
 * @param label_uuid The label UUID to add
 * @return SL_STATUS_OK if the label UUID was successfully added
 */
sl_status_t sl_mesh_decoder_add_virtual_label_uuid(struct sl_mesh_decoder *context,
                                                   const uint8_t *label_uuid);

/**
 * @brief Remove a virtual address label UUID
 *
 * @param context The context to use
 * @param label_uuid The label UUID to remove
 * @return SL_STATUS_OK if the label UUID was successfully removed
 */
sl_status_t sl_mesh_decoder_remove_virtual_label_uuid(struct sl_mesh_decoder *context,
                                                      const uint8_t *label_uuid);

/**
 * @brief Decrypt an access message
 *
 * Decrypt an access message. The decoder will try to decrypt and
 * authenticate the given encrypted access message with the encryption
 * keys it has; and if successful, it will return the decrypted PDU
 * contents as well as a reference to the matching application or
 * device key, and a reference to the matching label UUID if the
 * destination address is a virtual address or the NULL pointer.
 *
 * On successful decryption the message structure will contain the
 * decrypted and authenticated access message; and the plaintext
 * TransMIC at the end for reference.
 *
 * @param context Context to use
 * @param ivi Low bit of the IV index of the network PDUs carrying the access
 * message
 * @param akf The AKF header of the lower transport PDUs carrying the access
 * message
 * @param aid The AID header of the lower transport PDUs carrying the access
 * message
 * @param szmic The SZMIC header of the lower transport PDUs carrying the access
 * message
 * @param src The source address for the message
 * @param dst The destination address for the message
 * @param seq The sequence number of the network PDU carrying the access message,
 * for unsegmented messages; or the SeqAuth value for segmented messages
 * @param current_ivindex Network IV Index value
 * @param access_pdu_ptr Encrypted access message data
 * @param access_pdu_len Encrypted access message length
 * @param pdu The decrypted message is returned here
 * @param matching_app_key A reference to the matching application or device key is returned here
 * @param matching_label_uuid Pointer to the matching label UUID for virtual destination addresses,
 * or NULL for non-virtual destination addresses, is returned here
 * @return SL_STATUS_OK if the message is successfully decrypted and authenticated
 */
sl_status_t sl_mesh_decoder_decrypt_access_pdu(struct sl_mesh_decoder *context,
                                               uint8_t ivi,
                                               uint8_t akf,
                                               uint8_t aid,
                                               uint8_t szmic,
                                               uint32_t seq,
                                               uint16_t src,
                                               uint16_t dst,
                                               uint32_t current_ivindex,
                                               const uint8_t *access_pdu_ptr,
                                               size_t access_pdu_len,
                                               struct sl_mesh_access_pdu *pdu,
                                               sl_mesh_key_handle *matching_app_key,
                                               const uint8_t **matching_label_uuid);

/**
 * @brief Verify access message length
 *
 * Verify that the access message is long enough to parse. Note that
 * this only verifies that there is enough data to read the access
 * message opcode; it does not check parameter value validity for the
 * particular message.
 *
 * This API must be called before starting to interpret the data.
 *
 * @param pdu Decrypted access message
 * @return true if the access message contains enough data so that opcode can be parsed
 */
bool sl_mesh_access_pdu_contains_valid_access_message(const struct sl_mesh_access_pdu *pdu);

/**
 * @brief Get a pointer to the access message payload
 *
 * @param pdu Decrypted access message
 * @return a pointer to the payload
 */
const uint8_t *sl_mesh_access_pdu_opcode_ptr(const struct sl_mesh_access_pdu *pdu);

/**
 * @brief Get access message payload length
 *
 * @param pdu Decrypted access message
 * @return length of the payload
 */
size_t sl_mesh_access_pdu_opcode_len(const struct sl_mesh_access_pdu *pdu);

/**
 * @brief Get a pointer to the access message parameters
 *
 * @param pdu Decrypted access message
 * @return a pointer to the beginning of parameters
 */
const uint8_t *sl_mesh_access_pdu_data_ptr(const struct sl_mesh_access_pdu *pdu);

/**
 * @brief Get access message parameter length
 *
 * @param pdu Decrypted access message
 * @return length of the parameters
 */
size_t sl_mesh_access_pdu_data_len(const struct sl_mesh_access_pdu *pdu);

// -------------------------------
// Beacon decoding

/**
 * @brief Authenticate beacon and get beacon contents
 *
 * @param context The context to use
 * @param beacon_ptr Beacon data pointer
 * @param beacon_len Beacon data length
 * @param type Beacon type will be returned here
 * @param flags Beacon flags will be returned here
 * @param iv_index Beacon IV index value will be returned here
 * @param matching_net_key The network key with which the beacon authenticates will be returned here
 * @return SL_STATUS_OK if beacon was authenticated successfully
 */
sl_status_t sl_mesh_decoder_authenticate_network_beacon(struct sl_mesh_decoder *context,
                                                        const uint8_t *beacon_ptr,
                                                        size_t beacon_len,
                                                        enum sl_mesh_beacon *type,
                                                        uint8_t *flags,
                                                        uint32_t *iv_index,
                                                        sl_mesh_key_handle *matching_net_key);

// -------------------------------
// Sensor model decoding

/**
 * @brief Converts a time interval in milliseconds to seconds.
 *
 * This function takes a time interval in milliseconds and converts it to seconds.
 *
 * @param[in] interval The time interval in milliseconds.
 * @return The time interval in seconds as a float.
 */
float sl_mesh_decoder_time_to_sec(uint32_t interval);

/**
 * @brief Converts a tolerance value to an error percentage.
 *
 * This function takes a tolerance value and converts it to an error percentage.
 *
 * @param[in] tolerance The tolerance value.
 * @return The error percentage as a float.
 */
float sl_mesh_decoder_tolerance_to_error_percentage(uint16_t tolerance);

/**
 * @brief Parses sensor descriptors from a buffer.
 *
 * This function extracts sensor descriptor information from the provided buffer
 * and populates the given `sensor_descriptor_t` structure.
 *
 * @param[out] descriptor     Pointer to the structure where the parsed sensor descriptor
 *                            information will be stored.
 * @param[in] pdu_data        Pointer to the buffer containing the sensor descriptor data.
 * @param[in] pdu_data_len    Length of the buffer in bytes.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_descriptors_from_buf(sensor_descriptor_t *descriptor,
                                                const uint8_t *pdu_data,
                                                const size_t pdu_data_len);

/**
 * @brief Decodes sensor cadence state from a buffer.
 *
 * This function parses the provided buffer to extract the sensor cadence state
 * and populates the given `sensort_cadence_state` structure with the decoded data.
 *
 * @param[out] cadence       Pointer to the structure where the decoded sensor cadence state will be stored.
 * @param[in]  pdu_data      Pointer to the buffer containing the encoded sensor cadence data.
 * @param[in]  pdu_data_len  Length of the buffer containing the encoded data.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_cadence_from_buf(struct sensort_cadence_state *cadence,
                                            uint8_t *pdu_data,
                                            const size_t pdu_data_len);

/**
 * @brief Parses the sensor setting from a buffer.
 *
 * This function parses the provided buffer and populates the sensor setting
 * structure with the extracted data.
 *
 * @param[out] set_setting  Pointer to the structure where the parsed sensor setting
 *                          information will be stored.
 * @param[in] pdu_data      Pointer to the buffer containing the sensor setting data.
 * @param[in] pdu_data_len  Length of the buffer in bytes.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_setting_set_from_buf(sensor_set_setting_msg_t *set_setting,
                                                uint8_t *pdu_data,
                                                const size_t pdu_data_len);

/**
 * @brief Extracts sensor setting information from a buffer.
 *
 * This function parses the provided buffer to populate the
 * sensor_get_setting_msg_t structure with sensor setting data.
 *
 * @param[out] get_setting   Pointer to the structure where the sensor setting
 *                           information will be stored.
 * @param[in]  pdu_data      Pointer to the buffer containing the sensor setting data.
 * @param[in]  pdu_data_len  Length of the buffer in bytes.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_setting_get_from_buf(sensor_get_setting_msg_t *get_setting,
                                                uint8_t *pdu_data,
                                                const size_t pdu_data_len);

/**
 * @brief Parses a sensor setting status message from a buffer.
 *
 * This function extracts the sensor setting status information from the provided
 * data buffer and populates the given `sensor_setting_status`
 * structure with the parsed data.
 *
 * @param[out] setting_status Pointer to a `sensor_setting_status` structure where the
 *                            parsed data will be stored.
 * @param[in]  pdu_data       Pointer to the buffer containing the PDU data.
 * @param[in]  pdu_data_len   Length of the PDU data buffer.
 *
 * @return sl_status_t Returns SL_STATUS_OK if the parsing is successful, or an
 *                     appropriate error code otherwise.
 */
sl_status_t sl_mesh_sensor_setting_status_from_buf(struct sensor_setting_status *setting_status,
                                                   uint8_t *pdu_data,
                                                   const size_t pdu_data_len);

/**
 * @brief Decodes a sensor status message from a buffer.
 *
 * This function parses the provided buffer containing sensor status data
 * and populates the given `sensor_status_msg_t` structure with the decoded
 * information.
 *
 * @param[out] sensor_status Pointer to the structure where the decoded sensor
 *                           status message will be stored.
 * @param[in]  pdu_data      Pointer to the buffer containing the sensor status
 *                           message data.
 * @param[in]  pdu_data_len  Length of the buffer containing the sensor status
 *                           message data.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_status_from_buf(sensor_status_msg_t *sensor_status,
                                           uint8_t *pdu_data,
                                           const size_t pdu_data_len);

/**
 * @brief Extracts sensor settings status message from a buffer.
 *
 * This function parses the provided buffer to extract the sensor settings
 * status information and populates the given `sensor_settings_status_t` structure.
 *
 * @param[out] settings_status Pointer to the structure where the decoded sensor
 *                             settings status will be stored.
 * @param[in] pdu_data         Pointer to the buffer containing the encoded sensor
 *                             settings status message.
 * @param[in] pdu_data_len     Length of the buffer in bytes.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_settings_status_from_buf(sensor_settings_status_t *settings_status,
                                                    uint8_t *pdu_data,
                                                    const size_t pdu_data_len);

/**
 * @brief Extracts sensor column data from a buffer.
 *
 * This function parses the provided buffer to populate the
 * sensor_get_column_msg_t structure with sensor column data.
 *
 * @param[out] column_get     Pointer to the structure where the extracted
 *                            sensor column data will be stored.
 * @param[in]  pdu_data       Pointer to the buffer containing the sensor column data.
 * @param[in]  pdu_data_len   Length of the buffer in bytes.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_column_get_from_buf(sensor_get_column_msg_t *column_get,
                                               uint8_t *pdu_data,
                                               const size_t pdu_data_len);

/**
 * @brief Extracts sensor series data from a buffer.
 *
 * This function parses the provided PDU data buffer to extract sensor series
 * information and populates the given `sensor_get_series_msg_t` structure.
 *
 * @param[out] series_get Pointer to the structure where the extracted sensor
 *                        series data will be stored.
 * @param[in]  pdu_data   Pointer to the buffer containing the PDU data.
 * @param[in]  pdu_data_len Length of the PDU data buffer.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_series_get_from_buf(sensor_get_series_msg_t *series_get,
                                               uint8_t *pdu_data,
                                               const size_t pdu_data_len);

/**
 * @brief Decodes a sensor series status message from a buffer.
 *
 * This function parses the provided PDU data and populates the
 * sensor_series_status_msg_t structure with the decoded information.
 *
 * @param[out] series_status Pointer to the structure where the decoded
 *                           sensor series status will be stored.
 * @param[in]  pdu_data      Pointer to the buffer containing the PDU data.
 * @param[in]  pdu_data_len  Length of the PDU data buffer.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_series_status_from_buf(sensor_series_status_msg_t *series_status,
                                                  uint8_t *pdu_data,
                                                  const size_t pdu_data_len);
/**
 * @brief Decodes a sensor column status message from a buffer.
 *
 * This function extracts the sensor column status information from the provided
 * buffer and populates the given `sensor_column_status_msg_t` structure.
 *
 * @param[out] column_status Pointer to the structure where the decoded sensor column
 *                           status will be stored.
 * @param[in]  pdu_data      Pointer to the buffer containing the encoded sensor column
 *                           status message.
 * @param[in]  pdu_data_len  Length of the buffer containing the encoded message.
 *
 * @return sl_status_t Status code indicating the success or failure of the operation.
 *                     - SL_STATUS_OK: The operation was successful.
 *                     - Error code: An error occurred during parsing.
 */
sl_status_t sl_mesh_sensor_column_status_from_buf(sensor_column_status_msg_t *column_status,
                                                  uint8_t *pdu_data,
                                                  const size_t pdu_data_len);

#ifdef __cplusplus
}
#endif

#endif // SL_MESH_DECODER_H
