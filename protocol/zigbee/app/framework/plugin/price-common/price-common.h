/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Price Common plugin.
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

#ifndef SILABS_PRICE_COMMON_H
#define SILABS_PRICE_COMMON_H

/**
 * @defgroup price-common Price Common
 * @ingroup component
 * @brief API and Callbacks for the Price Common Component
 *
 * This component provides common utility functions used by both the
 * Price Server and Price Client components.
 *
 */

/**
 * @addtogroup price-common
 * @{
 */

#include "price-common-time.h"

#ifndef SL_ZIGBEE_ZCL_PRICE_CLUSTER_SERVER_ENDPOINT_COUNT
  #define SL_ZIGBEE_ZCL_PRICE_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#endif

#define EVENT_ID_UNSPECIFIED                          (0xFFFFFFFFUL)
#define TARIFF_TYPE_UNSPECIFIED                       (0xFFu)
#define ZCL_PRICE_CLUSTER_PRICE_ACKNOWLEDGEMENT_MASK  (0x01u)
#define ZCL_PRICE_CLUSTER_RESERVED_MASK               (0xFEu)
#define ZCL_PRICE_CLUSTER_DURATION16_UNTIL_CHANGED    (0xFFFFu)
#define ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED  (0xFFFFFFFFUL)
#define ZCL_PRICE_CLUSTER_END_TIME_NEVER              (0xFFFFFFFFUL)
#define ZCL_PRICE_CLUSTER_NUMBER_OF_EVENTS_ALL        (0x00u)
#define ZCL_PRICE_CLUSTER_START_TIME_NOW              (0x00000000UL)
#define ZCL_PRICE_CLUSTER_WILDCARD_ISSUER_ID          (0xFFFFFFFFUL)
#define ZCL_PRICE_INVALID_ENDPOINT_INDEX              (0xFFu)
#define ZCL_PRICE_INVALID_INDEX                       (0xFFu)
#define ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED      (0xFFFFu)

typedef struct {
  uint32_t startTime;

  /* Using uint32_t since int24u doesn't exist.  MEASURED IN SECONDS.
   * FOREVER or UNTIL_CHANGED should set this to ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED.
   * Some commands might not use this field.
   */
  uint32_t durationSec;

  uint32_t issuerEventId;
  bool valid;

  /* Flag showing if actions are required by the current entry, such
   * as updating attributes and etc, are still pending. Usage of this flag is
   * optional.
   */
  bool actionsPending;
} sl_zigbee_af_price_common_info_t;

typedef struct {
  uint8_t   rateLabel[ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH + 1];
  uint32_t  providerId;
  uint32_t  issuerEventID;
  uint32_t  startTime;
  uint32_t  price;
  uint32_t  generationPrice;
  uint32_t  alternateCostDelivered;
  uint16_t  currency;
  uint16_t  duration; // in minutes
  uint8_t   unitOfMeasure;
  uint8_t   priceTrailingDigitAndTier;
  uint8_t   numberOfPriceTiersAndTier; // added later in errata
  uint8_t   priceRatio;
  uint8_t   generationPriceRatio;
  uint8_t   alternateCostUnit;
  uint8_t   alternateCostTrailingDigit;
  uint8_t   numberOfBlockThresholds;
  uint8_t   priceControl;
} sl_zigbee_af_scheduled_price_t;

/**
 * @name API
 * @{
 */

/**
 * @brief Return the best matching or other index to use for inserting new data.
 *
 * @param commonInfos An array of sl_zigbee_af_price_common_info_t structures whose data is used to find
 * the best available index.
 * @param numberOfEntries The number of entries in the sl_zigbee_af_price_common_info_t array.
 * @param newIssuerEventId The issuerEventId of the new data. This is used to see if a match is present.
 * @param newStartTime The startTime of the new data.
 * @param expireTimedOut Treats any timed-out entries as invalid if set to true.
 * @return The best index - either a matching index, if found, or an invalid or timed out index.
 *
 **/
uint8_t sl_zigbee_af_price_common_get_common_matching_or_unused_index(sl_zigbee_af_price_common_info_t *commonInfos,
                                                                      uint8_t  numberOfEntries,
                                                                      uint32_t newIssuerEventId,
                                                                      uint32_t newStartTime,
                                                                      bool expireTimedOut);

/**
 * @brief Sort price-related data structures.
 *
 * This semi-generic sorting function can be used to sort all structures that
 * utilizes the sl_zigbee_af_price_common_info_t data type.
 *
 * @param commonInfos The destination address to which the command should be sent.
 * @param dataArray The source endpoint used in the transmission.
 * @param dataArrayBlockSizeInByte The source endpoint used in the transmission.
 * @param dataArraySize The source endpoint used in the transmission.
 *
 **/
void sl_zigbee_af_price_common_sort(sl_zigbee_af_price_common_info_t * commonInfos,
                                    uint8_t * dataArray,
                                    uint16_t dataArrayBlockSizeInByte,
                                    uint16_t dataArraySize);

/**
 * @brief Update durations to avoid overlapping the next event.
 *
 * @param commonInfos An array of sl_zigbee_af_price_common_info_t structures that will be evaluated.
 * @param numberOfEntries The number of entries in the sl_zigbee_af_price_common_info_t array.
 *
 **/
void sl_zigbee_af_price_common_update_duration_for_overlapping_events(sl_zigbee_af_price_common_info_t *commonInfos,
                                                                      uint8_t numberOfEntries);

/**
 * @brief Determine the time until the next index becomes active.
 *
 * This function assumes the commonInfos[] array is already sorted by startTime from earliest to latest.
 *
 * @param commonInfos An array of sl_zigbee_af_price_common_info_t structures that will be evaluated.
 * @param numberOfEntries The number of entries in the sl_zigbee_af_price_common_info_t array.
 *
 **/
uint32_t sl_zigbee_af_price_common_seconds_until_second_index_active(sl_zigbee_af_price_common_info_t *commonInfos,
                                                                     uint8_t numberOfEntries);

/**
 * @brief Find valid entries in the sl_zigbee_af_price_common_info_t structure array.
 *
 * @param validEntries An array of the same size as the sl_zigbee_af_price_common_info_t
 * array that will store the valid flag for each entry (true or false).
 * @param numberOfEntries The number of entries in the validEntries array and the commonInfos array.
 * @param commonInfos The sl_zigbee_af_price_common_info_t array that will be searched for valid entries.
 * @param earliestStartTime A minimum start time such that all valid entries have a start
 * time greater than or equal to this value.
 * @minIssuerEventId A minimum event ID such that all valid entries have an issuerEventId
 * greater than or equal to this.
 * @numberOfRequestedCommands The maximum number of valid entries to be returned.
 * @return The number of valid commands found in the commonInfos array.
 *
 **/
uint8_t sl_zigbee_af_price_common_find_valid_entries(uint8_t* validEntries,
                                                     uint8_t numberOfEntries,
                                                     sl_zigbee_af_price_common_info_t* commonInfos,
                                                     uint32_t earliestStartTime,
                                                     uint32_t minIssuerEventId,
                                                     uint8_t numberOfCommands);

/**
 * @brief Return the index of the active entry in the sl_zigbee_af_price_common_info_t array.
 *
 * Search through array for the most recent active entry. "Issuer Event Id" has higher
 * priority than "start time".
 *
 * @param commonInfos The sl_zigbee_af_price_common_info_t array that will be searched for an active entry.
 * @param numberOfEntries The number of entries in the commonInfo array.
 * @return The index of the active entry, or 0xFF if an active entry is not found.
 *
 **/
uint8_t sl_zigbee_af_price_common_server_get_active_index(sl_zigbee_af_price_common_info_t *commonInfos,
                                                          uint8_t numberOfEntries);

/**
 * @brief Return the index to the most recent entry that will become active in the future.
 *
 * @param commonInfos The sl_zigbee_af_price_common_info_t array that will be searched for the entry.
 * @numberOfEntries The number of entries in the commonInfo array.
 * @secUntilFutureEvent The output pointer to the number of seconds until the next
 *                      active entry.
 *
 * @return The index of the next-active entry, or 0xFF if an active entry is not found.
 *
 **/
uint8_t sl_zigbee_af_price_common_server_get_future_index(sl_zigbee_af_price_common_info_t *commonInfos,
                                                          uint8_t numberOfEntries,
                                                          uint32_t * secUntilFutureEvent);

/** @} */ // end of name API
/** @} */ // end of price-common

#endif  // #ifndef _PRICE_COMMON_H_
