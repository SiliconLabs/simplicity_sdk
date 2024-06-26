/***************************************************************************//**
 * @file
 * @brief APIs for the Comms Hub Function plugin.
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

#ifndef COMMS_HUB_FUNCTION_H_INCLUDED
#define COMMS_HUB_FUNCTION_H_INCLUDED

#include "comms-hub-function-config.h"

/**
 * @defgroup comms-hub-function Comms Hub Function
 * @ingroup component
 * @brief API and Callbacks for the Comms Hub Function Component
 *
 * Silicon Labs implementation of the Comms Hub Function (CHF).
 *
 */

/**
 * @addtogroup comms-hub-function
 * @{
 */

// Printing macros for plugin: Comms Hub Function
#define sl_zigbee_af_comms_hub_function_print(...)    sl_zigbee_af_app_print(__VA_ARGS__)
#define sl_zigbee_af_comms_hub_function_println(...)  sl_zigbee_af_app_println(__VA_ARGS__)
#define sl_zigbee_af_comms_hub_function_debug_exec(x)  sl_zigbee_af_app_debug_exec(x)
#define sl_zigbee_af_comms_hub_function_print_buffer(buffer, len, withSpace) sl_zigbee_af_app_print_buffer(buffer, len, withSpace)

typedef enum {
  SL_ZIGBEE_AF_CHF_STATUS_SUCCESS                 = 0x00,
  SL_ZIGBEE_AF_CHF_STATUS_TOO_MANY_PEND_MESSAGES  = 0xFA,
  SL_ZIGBEE_AF_CHF_STATUS_FNF_ATTR_FAILURE        = 0xFB,
  SL_ZIGBEE_AF_CHF_STATUS_NO_MIRROR               = 0xFC,
  SL_ZIGBEE_AF_CHF_STATUS_TUNNEL_FAILURE          = 0xFD,
  SL_ZIGBEE_AF_CHF_STATUS_NO_ACCESS               = 0xFE,
  SL_ZIGBEE_AF_CHF_STATUS_SEND_TIMEOUT            = 0xFF,
} sl_zigbee_af_plugin_comms_hub_function_status_t;

/**
 * @name API
 * @{
 */

/**
 * @brief Pass a message to be tunneled over the HAN using either
 * a sleepy buffer system (GSME) or directly to the device
 * by initiating a tunnel (ESME, HCALCS, PPMID, TYPE2).
 *
 * This function is used to transfer data to a device on the HAN.
 *
 * @param destinationDeviceId The EUI64 of the destination device.
 * @param length The length in octets of the data.
 * @param payload The buffer (memory location at WAN Message Handler) containing the
 *  raw octets of the message (GBCS Message)
 * @param messageCode The GBCS Message Code for the data that is being sent.
 * @return
 * ::SL_ZIGBEE_AF_CHF_STATUS_SUCCESS data was sent or has been queue to be sent.
 * ::SL_ZIGBEE_AF_CHF_STATUS_NO_ACCESS No entry in the GBCS Device Log for the specified device.
 * ::SL_ZIGBEE_AF_CHF_STATUS_NO_MIRROR Mirror endpoint for given device has not been configured.
 * ::SL_ZIGBEE_AF_CHF_STATUS_FNF_ATTR_FAILURE Unable to read or write the functional notification flags attribute.
 * ::SL_ZIGBEE_AF_CHF_STATUS_TOO_MANY_PEND_MESSAGES There are too many messages currently pending to be delivered.
 * ::SL_ZIGBEE_AF_CHF_STATUS_TUNNEL_FAILURE tunnel cannot be created to non sleepy devices.
 */
sl_zigbee_af_plugin_comms_hub_function_status_t sl_zigbee_af_comms_hub_function_send(sl_802154_long_addr_t destinationDeviceId,
                                                                                     uint16_t length,
                                                                                     uint8_t *payload,
                                                                                     uint16_t messageCode);

/**
 * @brief Set the default remote part message timeout.
 *
 * This function is used to set the default timeout for all messages
 * destined for a sleepy device. If the device does not retrieve the message
 * before this time, it will be discarded and a ::SL_ZIGBEE_AF_CHF_STATUS_SEND_TIMEOUT
 * error will be return in sl_zigbee_af_comms_hub_function_send_cb().
 *
 * @param timeout Timeout in seconds.
 */
void sli_zigbee_af_comms_hub_function_set_default_timeout(uint32_t timeout);

/**
 * @brief Update Functional Notification Flags for the given device.
 *
 * @param deviceId Identifier of the device for which the notification flags are
 *  to be updated
 * @param resetMask Each flag to be reset has the corresponding bit set to 0,
 *  all other flags have the corresponding bit set to 1
 * @param setMask Each flag to be set has the corresponding bit set to 1,
 *  all other flags have the corresponding bit set to 0
 */
sl_zigbee_af_plugin_comms_hub_function_status_t sli_zigbee_af_update_functional_notification_flags_by_eui64(sl_802154_long_addr_t deviceId,
                                                                                                            uint32_t resetMask,
                                                                                                            uint32_t setMask);

/**
 * @brief Update Functional Notification Flags on the given endpoint.
 *
 * @param endpoint An endpoint on which the notification flags are to be updated.
 * @param resetMask Each flag to be reset has the corresponding bit set to 0,
 *  all other flags have the corresponding bit set to 1.
 * @param setMask Each flag to be set has the corresponding bit set to 1,
 *  all other flags have the corresponding bit set to 0.
 */
sl_zigbee_af_plugin_comms_hub_function_status_t sli_zigbee_af_update_functional_notification_flags_by_endpoint(uint8_t endpoint,
                                                                                                               uint32_t resetMask,
                                                                                                               uint32_t setMask);

/** @} */ // end of name APIs

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup comms_hub_cb Comms Hub
 * @ingroup af_callback
 * @brief Callbacks for Comms Hub Component
 *
 */

/**
 * @addtogroup comms_hub_cb
 * @{
 */

/**
 * @brief Accept a tunnel.
 *
 * Called by the tunnel manager when a tunnel is requested.
 * Check the given device identifier should be checked against the Device Log
 * to verify whether tunnels from the device should be accepted or not.
 *
 * @param deviceId An identifier of the device from which a tunnel is requested
 * @return True if the tunnel should be allowed, false otherwise.
 */
bool sli_zigbee_af_comms_hub_function_tunnel_accept_callback(sl_802154_long_addr_t deviceId);

/**
 * @brief Notify that tunnel data was received.
 *
 * Called by the tunnel manager when data is received over a tunnel.
 * It is responsible for the implementation of the GET, GET_RESPONSE, PUT
 * protocol used when communicating with a sleepy device.
 *
 * @param senderDeviceId The identifier of the device from which the data was received.
 * @param length The length of the data received
 * @param payload Data received.
 */
void sli_zigbee_af_comms_hub_function_tunnel_data_received_callback(sl_802154_long_addr_t senderDeviceId,
                                                                    uint16_t length,
                                                                    uint8_t *payload);

/** @brief Send.
 *
 * This callback is called by the Comms Hub Function (CHF) plugin to report the
 * status of a message that was previously sent.
 *
 * @param status The status of the message that was sent Ver.: always
 * @param destinationDeviceId The EUI64 of the destination device to which the
 * data was sent Ver.: always
 * @param dataLen The length in octets of the data that was sent Ver.: always
 * @param data Buffer containing the raw octets of the data that was sent
 * Ver.: always
 */
void sl_zigbee_af_comms_hub_function_send_cb(uint8_t status,
                                             sl_802154_long_addr_t destinationDeviceId,
                                             uint16_t dataLen,
                                             uint8_t *data);

/** @brief Receive.
 *
 * This callback is called by the Comms Hub Function (CHF) plugin whenever a
 * message is received.
 *
 * @param senderDeviceId The EUI64 of the sending device Ver.: always
 * @param dataLen The length in octets of the data Ver.: always
 * @param data Buffer containing the raw octets of the data Ver.: always
 */
void sl_zigbee_af_comms_hub_function_received_cb(sl_802154_long_addr_t senderDeviceId,
                                                 uint16_t dataLen,
                                                 uint8_t *data);

/** @brief Alert WAN.
 *
 * Notify the application of an Alert that should be sent to the WAN. The
 * second argument is a pointer to the GBZ alert buffer. The application is
 * responsible for freeing this buffer.
 *
 * @param alertCode The 16 bit allert code as defined by GBCS Ver.: always
 * @param gbzAlert Buffer continaing the GBZ formatted Alert Ver.: always
 * @param gbzAlertLength The length in octets of the GBZ formatted Alert
 * Ver.: always
 */
void sl_zigbee_(uint16_t alertCode,
                uint8_t *gbzAlert,
                uint16_t gbzAlertLength);

/** @brief Tunnel opened.
 *
 * This function is called by the Comms Hub Function (CHF) plugin when a tunnel
 * is opened.
 *
 * @param remoteDeviceId The EUI64 of the remote device for which a tunnel is
 * opened Ver.: always
 */
void sl_zigbee_af_comms_hub_function_tunnel_opened_cb(sl_802154_long_addr_t remoteDeviceId);
/** @} */ // end of name comms_hub_cb
/** @} */ // end of name Callbacks
/** @} */ // end of comms-hub-function

#endif // COMMS_HUB_FUNCTION_SUB_GHZ_H_INCLUDED
