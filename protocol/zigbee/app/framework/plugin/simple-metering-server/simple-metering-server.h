/***************************************************************************//**
 * @file
 * @brief Definitions for the Simple Metering Server plugin.
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

#ifndef SIMPLE_METERING_SERVER_H_
#define SIMPLE_METERING_SERVER_H_

#include "simple-metering-server-config.h"
#if (SL_ZIGBEE_AF_PLUGIN_SIMPLE_METERING_SERVER_TEST_METER_ERRORS == 1)
#define TEST_METER_ERRORS
#endif

/**
 * @defgroup simple-metering-server Simple Metering Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Simple Metering Cluster Server Component
 *
 * This is a test implementation of the simple metering server. This component
 * should only be used for testing but is helpful to test devices that require
 * inter-operation with a working meter. It supports the required current
 * summation delivered attribute (0x0000), but optionally the following
 * attributes as well if they are selected in the project: current tier 1
 * summation delivered (0x0100), battery life remaining (0x0201), hours
 * in operation (0x0202), and instantaneous demand (0x0400).
 *
 */

/**
 * @addtogroup simple-metering-server
 * @{
 */

/**
 * @name API
 * @{
 */

void sl_zigbee_af_simple_metering_cluster_read_attributes_response_cb(sl_zigbee_af_cluster_id_t clusterId,
                                                                      uint8_t *buffer,
                                                                      uint16_t bufLen);
uint16_t sl_zigbee_af_simple_metering_server_start_sampling(uint16_t requestedSampleId,
                                                            uint32_t issuerEventId,
                                                            uint32_t startSamplingTime,
                                                            uint8_t sampleType,
                                                            uint16_t sampleRequestInterval,
                                                            uint16_t maxNumberOfSamples,
                                                            uint8_t endpoint);
/** @brief Process notification flags.
 *
 * This function is called by the metering server plugin when any of the
 * Notification Attribute Set attributes are read or reported by the metering
 * client.
 *
 * @param attributeId   Ver.: always
 * @param attributeValue   Ver.: always
 */
void sl_zigbee_af_simple_metering_server_process_notification_flags_cb(uint16_t attributeId, uint32_t attributeValue);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/** @brief Process notification flags.
 *
 * This function is called by the metering server plugin when any of the
 * Notification Attribute Set attributes are read or reported by the metering
 * client.
 *
 * @param attributeId   Ver.: always
 * @param attributeValue   Ver.: always
 */
void sl_zigbee_af_simple_metering_server_process_notification_flags_cb(uint16_t attributeId,
                                                                       uint32_t attributeValue);
/** @} */ // end of Callbacks
/** @} */ // end of simple-metering-server

void sli_zigbee_af_toggle_fast_polling(uint8_t enableFastPolling);

#endif /* SIMPLE_METERING_SERVER_H_ */
