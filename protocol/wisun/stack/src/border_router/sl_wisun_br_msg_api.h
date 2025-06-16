/***************************************************************************//**
 * @file sl_wisun_br_msg_api.h
 * @brief Wi-SUN Border Router message API
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_BR_MSG_API_H
#define SL_WISUN_BR_MSG_API_H

#include "sl_wisun_types.h"
#include "border_router/sl_wisun_br_connection_params_api.h"
#include "border_router/sl_wisun_br_lfn_params_api.h"

/**************************************************************************//**
 * @addtogroup SL_WISUN_BR_MSG_API Wi-SUN Message API
 * @{
 *****************************************************************************/

/// Wi-SUN BR Message API request IDs
typedef enum {
  SL_WISUN_BR_MSG_START_REQ_ID                          = 0x7F,
  SL_WISUN_BR_MSG_STOP_REQ_ID                           = 0x7E,
  SL_WISUN_BR_MSG_SET_NETWORK_SIZE_REQ_ID               = 0x7D,
  SL_WISUN_BR_MSG_SET_GTK_REQ_ID                        = 0x7C,
  SL_WISUN_BR_MSG_GET_IP_ADDR_REQ_ID                    = 0x7B,
  SL_WISUN_BR_MSG_GET_GAK_REQ_ID                        = 0x7A,
  SL_WISUN_BR_MSG_GET_STATE_REQ_ID                      = 0x79,
  SL_WISUN_BR_MSG_SET_DEVICE_CERTIFICATE_REQ_ID         = 0x78,
  SL_WISUN_BR_MSG_SET_BROADCAST_SETTINGS_REQ_ID         = 0x77,
  SL_WISUN_BR_MSG_SET_CONNECTION_PARAMS_REQ_ID          = 0x76,
  SL_WISUN_BR_MSG_SET_LFN_PARAMS_REQ_ID                 = 0x75,
  SL_WISUN_BR_MSG_SET_LFN_SUPPORT_REQ_ID                = 0x74,
  UNUSED_0x73_REQ_ID                                    = 0x73,
  SL_WISUN_BR_MSG_PAN_DEFECT_ADVERTISE_REQ_ID           = 0x72,
  SL_WISUN_BR_MSG_SET_IPV6_PREFIX_REQ_ID                = 0x71,
  SL_WISUN_BR_MSG_TRIGGER_GLOBAL_REPAIR_REQ_ID          = 0x70,
  SL_WISUN_BR_MSG_GET_ROUTING_TABLE_ENTRY_COUNT_REQ_ID  = 0x6F,
  SL_WISUN_BR_MSG_GET_ROUTING_TABLE_REQ_ID              = 0x6E,
} sl_wisun_br_msg_req_id_t;

/// Wi-SUN BR Message API confirmation IDs
typedef enum {
  SL_WISUN_BR_MSG_START_CNF_ID                          = 0x7F,
  SL_WISUN_BR_MSG_STOP_CNF_ID                           = 0x7E,
  SL_WISUN_BR_MSG_SET_NETWORK_SIZE_CNF_ID               = 0x7D,
  SL_WISUN_BR_MSG_SET_GTK_CNF_ID                        = 0x7C,
  SL_WISUN_BR_MSG_GET_IP_ADDR_CNF_ID                    = 0x7B,
  SL_WISUN_BR_MSG_GET_GAK_CNF_ID                        = 0x7A,
  SL_WISUN_BR_MSG_GET_STATE_CNF_ID                      = 0x79,
  SL_WISUN_BR_MSG_SET_DEVICE_CERTIFICATE_CNF_ID         = 0x78,
  SL_WISUN_BR_MSG_SET_BROADCAST_SETTINGS_CNF_ID         = 0x77,
  SL_WISUN_BR_MSG_SET_CONNECTION_PARAMS_CNF_ID          = 0x76,
  SL_WISUN_BR_MSG_SET_LFN_PARAMS_CNF_ID                 = 0x75,
  SL_WISUN_BR_MSG_SET_LFN_SUPPORT_CNF_ID                = 0x74,
  UNUSED_0x73_CNF_ID                                    = 0x73,
  SL_WISUN_BR_MSG_PAN_DEFECT_ADVERTISE_CNF_ID           = 0x72,
  SL_WISUN_BR_MSG_SET_IPV6_PREFIX_CNF_ID                = 0x71,
  SL_WISUN_BR_MSG_TRIGGER_GLOBAL_REPAIR_CNF_ID          = 0x70,
  SL_WISUN_BR_MSG_GET_ROUTING_TABLE_ENTRY_COUNT_CNF_ID  = 0x6F,
  SL_WISUN_BR_MSG_GET_ROUTING_TABLE_CNF_ID              = 0x6E,
} sl_wisun_br_msg_cnf_id_t;

/// Wi-SUN BR Message API common header
SL_PACK_START(1)
typedef struct {
  /// Total length of the message in bytes, this field included
  uint16_t length;
  /// ID (request, confirmation, indication) of the message
  uint8_t id;
  /// Processing metadata for the message
  uint8_t info;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_header_t;
SL_PACK_END()

/******************************************************************************
 * @defgroup SL_WISUN_MSG_GENERIC sl_wisun_br_msg_generic
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_generic_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_generic_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_generic_cnf_t;
SL_PACK_END()

/**************************************************************************//**
 * @defgroup SL_WISUN_BR_MSG_START sl_wisun_br_msg_start
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Network name as a NULL terminated string
  uint8_t name[SL_WISUN_NETWORK_NAME_SIZE];
  /// PHY configuration
  sl_wisun_phy_config_t phy_config;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_start_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_start_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_start_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_start_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_start_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_start_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_START) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_STOP sl_wisun_br_msg_stop
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// 1 for non-graceful stop
  uint32_t non_graceful;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_stop_req_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_stop_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_stop_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_stop_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_stop_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_stop_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_STOP) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_GTK sl_wisun_br_msg_set_get
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// GTK
  uint8_t gtk[SL_WISUN_GTK_LEN];
  /// Index
  uint8_t index;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_gtk_req_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_gtk_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_gtk_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_gtk_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_gtk_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_gtk_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_GTK) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_NETWORK_SIZE sl_wisun_br_msg_set_network_size
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// Network size
  uint8_t size;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_network_size_req_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_network_size_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_network_size_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_network_size_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_network_size_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_network_size_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_NETWORK_SIZE) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_GET_IP_ADDRESS sl_wisun_br_msg_get_ip_address
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_ip_addr_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
  /// Link-local address
  in6_addr_t link_local_address;
  /// Global Unicast address
  in6_addr_t global_address;
  /// DODAGID
  in6_addr_t dodagid;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_ip_addr_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_ip_addr_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_ip_addr_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_GET_IP_ADDRESS) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_GET_GAK sl_wisun_br_msg_get_gak
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// Index
  uint8_t index;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_gak_req_body_t;
SL_PACK_END()


SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_get_gak_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_gak_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
  /// GAK
  uint8_t gak[SL_WISUN_GAK_LEN];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_gak_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_get_gak_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_gak_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_GET_GAK) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_GET_STATE sl_wisun_br_msg_get_state
 * @{
 ******************************************************************************/

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_state_req_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
  /// Border router state
  uint32_t state;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_state_cnf_body_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_get_state_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_state_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_GET_STATE) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_DEVICE_CERTIFICATE sl_wisun_br_msg_set_device_certificate
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Options for the certificate
  uint16_t certificate_options;
  /// Size of the certificate data
  uint16_t certificate_length;
  /// Certificate data
  uint8_t certificate[];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_device_certificate_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_device_certificate_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_device_certificate_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_device_certificate_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_device_certificate_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_device_certificate_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_DEVICE_CERTIFICATE) */

/**************************************************************************//**
 * @defgroup SL_WISUN_BR_MSG_SET_BROADCAST_SETTINGS sl_wisun_br_msg_set_broadcast_settings
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Broadcast interval
  uint32_t interval_ms;
  /// Broadcast dwell interval
  uint8_t dwell_interval_ms;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_broadcast_settings_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_broadcast_settings_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_broadcast_settings_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_broadcast_settings_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_broadcast_settings_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_broadcast_settings_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_BROADCAST_SETTINGS) */

/**************************************************************************//**
 * @defgroup SL_WISUN_BR_MSG_SET_CONNECTION_PARAMS sl_wisun_br_msg_set_connection_params
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Connection parameters
  sl_wisun_br_connection_params_t parameters;
} SL_ATTRIBUTE_PACKED  sl_wisun_br_msg_set_connection_params_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_connection_params_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_connection_params_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_connection_params_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_connection_params_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_connection_params_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_CONNECTION_PARAMS) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_LFN_PARAMS sl_wisun_br_msg_set_lfn_params
 * @{
 *****************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// LFN parenting parameters
  sl_wisun_br_lfn_params_t parameters;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_params_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_lfn_params_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_params_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_params_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_lfn_params_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_params_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_LFN_PARAMS) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_LFN_SUPPORT sl_wisun_br_msg_set_lfn_support
 * @{
 *****************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Number of LFN supported [0, 10]
  uint8_t lfn_limit;
  /// Enable or disable LFN support PAN-wide
  uint8_t lfn_support_pan;
  /// Reserved, set to zero
  uint8_t reserved[2];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_support_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_lfn_support_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_support_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_support_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_lfn_support_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_lfn_support_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_LFN_SUPPORT) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_DHCP_VENDOR_DATA sl_wisun_br_msg_set_dhcp_vendor_data
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Vendor Specific Enterprise Number
  uint32_t enterprise_number;
  /// Length of Vendor specific data
  uint16_t data_length;
  /// Vendor specific data
  uint8_t data[];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_dhcp_vendor_data_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_dhcp_vendor_data_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_dhcp_vendor_data_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_dhcp_vendor_data_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_dhcp_vendor_data_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_dhcp_vendor_data_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_DHCP_VENDOR_DATA) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_PAN_DEFECT_ADVERTISE sl_wisun_br_msg_pan_defect_advertise
 * @{
 *****************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Minimum duration after which devices are allowed to
  /// switch their PAN if an alternative one is available.
  uint32_t min_scan_duration_s;
  /// Duration after which devices should systemically switch
  /// to the alternative PAN if one is available.
  uint32_t max_scan_duration_s;
  /// True to start PAN Defect procedure, false to stop it
  uint8_t defect_advertise;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_pan_defect_advertise_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_pan_defect_advertise_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_pan_defect_advertise_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_pan_defect_advertise_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_pan_defect_advertise_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_pan_defect_advertise_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_PAN_DEFECT_ADVERTISE) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_SET_IPV6_PREFIX sl_wisun_br_msg_set_ipv6_prefix
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// New IPv6 prefix
  uint8_t prefix[IPV6_ADDR_SIZE];
  /// IPv6 prefix length
  uint8_t prefix_length;
  /// Reserved, set to zero
  uint8_t reserved[3];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_ipv6_prefix_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_set_ipv6_prefix_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_ipv6_prefix_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_ipv6_prefix_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_set_ipv6_prefix_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_set_ipv6_prefix_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_SET_IPV6_PREFIX) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_TRIGGER_GLOBAL_REPAIR sl_wisun_br_trigger_global_repair
 * @{
 ******************************************************************************/

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_trigger_global_repair_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_trigger_global_repair_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_trigger_global_repair_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_trigger_global_repair_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_TRIGGER_GLOBAL_REPAIR) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_GET_ROUTING_TABLE_ENTRY_COUNT sl_wisun_br_msg_get_routing_table_entry_count
 * @{
 ******************************************************************************/

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_entry_count_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
  /// Size of the routing table
  uint16_t entry_count;
  /// Reserved, set to zero
  uint8_t reserved[2];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_entry_count_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_get_routing_table_entry_count_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_entry_count_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_GET_ROUTING_TABLE_ENTRY_COUNT) */

/******************************************************************************
 * @defgroup SL_WISUN_BR_MSG_GET_ROUTING_TABLE sl_wisun_br_msg_get_routing_table
 * @{
 ******************************************************************************/

/// Request message body
SL_PACK_START(1)
typedef struct {
  /// Maximum number of entries to be retrieved
  uint16_t entry_count;
  /// Reserved, set to zero
  uint8_t reserved[2];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_req_body_t;
SL_PACK_END()

/// Request message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Request message body
  sl_wisun_br_msg_get_routing_table_req_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_req_t;
SL_PACK_END()

/// Confirmation message body
SL_PACK_START(1)
typedef struct {
  /// Status of the request
  uint32_t status;
  /// Number of entries in the routing table
  uint16_t entry_count;
  /// Reserved, set to zero
  uint8_t reserved[2];
  /// Routing table entries read
  sl_wisun_br_routing_table_entry_t entries[];
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_cnf_body_t;
SL_PACK_END()

/// Confirmation message
SL_PACK_START(1)
typedef struct {
  /// Common message header
  sl_wisun_br_msg_header_t header;
  /// Confirmation message body
  sl_wisun_br_msg_get_routing_table_cnf_body_t body;
} SL_ATTRIBUTE_PACKED sl_wisun_br_msg_get_routing_table_cnf_t;
SL_PACK_END()

/** @} (end SL_WISUN_BR_MSG_GET_ROUTING_TABLE) */

/** @} (end addtogroup SL_WISUN_BR_MSG_API) */

#endif  // SL_WISUN_BR_MSG_API_H
