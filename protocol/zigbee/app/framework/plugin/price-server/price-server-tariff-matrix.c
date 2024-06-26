/***************************************************************************//**
 * @file
 * @brief Matrix routines for the Price Server plugin.
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
#include "app/framework/util/common.h"
#include "price-server.h"
#include "price-server-tick.h"

#include "zap-cluster-command-parser.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif

//=============================================================================
// Functions
void sl_zigbee_af_price_clear_tariff_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    memset(&priceServerInfo.scheduledTariffTable.commonInfos[ep][i], 0x00, sizeof(sl_zigbee_af_price_common_info_t));
    memset(&priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][i], 0x00, sizeof(sl_zigbee_af_scheduled_tariff_t));
  }
}

void sl_zigbee_af_price_clear_price_matrix_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    memset(&priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep][i], 0x00, sizeof(sl_zigbee_af_price_common_info_t));
    memset(&priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep][i], 0x00, sizeof(sl_zigbee_af_scheduled_price_matrix_t));
  }
}

void sl_zigbee_af_price_clear_block_thresholds_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    memset(&priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep][i], 0x00, sizeof(sl_zigbee_af_scheduled_block_thresholds_t));
    memset(&priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep][i], 0x00, sizeof(sl_zigbee_af_price_common_info_t));
  }
}

// Retrieves the tariff at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_get_tariff_table_entry(uint8_t endpoint,
                                               uint8_t index,
                                               sl_zigbee_af_price_common_info_t *info,
                                               sl_zigbee_af_scheduled_tariff_t *tariff)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX || index == 0xFF) {
    return false;
  }

  if (index < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    sl_zigbee_af_debug_println("sl_zigbee_af_price_get_tariff_table_entry: retrieving tariff at ep(%d), index(%d)", ep, index);
    memcpy(info, &priceServerInfo.scheduledTariffTable.commonInfos[ep][index], sizeof(sl_zigbee_af_price_common_info_t));
    memcpy(tariff, &priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index], sizeof(sl_zigbee_af_scheduled_tariff_t));
    return true;
  }

  return false;
}

// Retrieves the price matrix at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_get_price_matrix(uint8_t endpoint,
                                         uint8_t index,
                                         sl_zigbee_af_price_common_info_t *info,
                                         sl_zigbee_af_scheduled_price_matrix_t *pm)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX || index == 0xFF) {
    return false;
  }

  if (index < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    sl_zigbee_af_debug_println("sl_zigbee_af_price_get_tariff_table_entry: retrieving tariff at ep(%d), index(%d)", ep, index);
    memcpy(info, &priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep][index], sizeof(sl_zigbee_af_price_common_info_t));
    memcpy(pm, &priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep][index], sizeof(sl_zigbee_af_scheduled_price_matrix_t));
    return true;
  }

  return false;
}

// Retrieves the block thresholds at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_get_block_thresholds_table_entry(uint8_t endpoint,
                                                         uint8_t index,
                                                         sl_zigbee_af_scheduled_block_thresholds_t *bt)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX || index == 0xFF) {
    return false;
  }

  if (index < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    sl_zigbee_af_debug_println("sl_zigbee_af_price_get_block_thresholds_table_entry: retrieving tariff at index %d, %d", ep, index);
    memcpy(bt,
           &priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep][index],
           sizeof(sl_zigbee_af_scheduled_block_thresholds_t));
    return true;
  }

  return false;
}

// Retrieves the tariff with the corresponding issuer tariff ID. Returns false
// if the tariff is not found or is invalid.
bool sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(uint8_t endpoint,
                                                       uint32_t issuerTariffId,
                                                       sl_zigbee_af_price_common_info_t *info,
                                                       sl_zigbee_af_scheduled_tariff_t *tariff)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return false;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_common_info_t *curInfo = &priceServerInfo.scheduledTariffTable.commonInfos[ep][i];
    sl_zigbee_af_scheduled_tariff_t *lookup = &priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][i];

    if (lookup->status != 0
        && lookup->issuerTariffId == issuerTariffId) {
      memcpy(info, curInfo, sizeof(sl_zigbee_af_price_common_info_t));
      memcpy(tariff, lookup, sizeof(sl_zigbee_af_scheduled_tariff_t));
      return true;
    }
  }

  return false;
}

// Retrieves the price matrix with the corresponding issuer tariff ID. Returns false
// if the price matrix is not found or is invalid.
bool sl_zigbee_af_price_get_price_matrix_by_issuer_tariff_id(uint8_t endpoint,
                                                             uint32_t issuerTariffId,
                                                             sl_zigbee_af_price_common_info_t *info,
                                                             sl_zigbee_af_scheduled_price_matrix_t *pm)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return false;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_common_info_t * lookup_info = &priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep][i];
    sl_zigbee_af_scheduled_price_matrix_t *lookup_pm = &priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep][i];
    if (lookup_info->valid
        && (lookup_pm->status != 0)
        && lookup_pm->issuerTariffId == issuerTariffId) {
      memcpy(info, lookup_info, sizeof(sl_zigbee_af_price_common_info_t));
      memcpy(pm, lookup_pm, sizeof(sl_zigbee_af_scheduled_price_matrix_t));
      return true;
    }
  }

  return false;
}

// Retrieves the block thresholds with the corresponding issuer tariff ID. Returns false
// if the block thresholds is not found or is invalid.
bool sl_zigbee_af_price_get_block_thresholds_by_issuer_tariff_id(uint8_t endpoint,
                                                                 uint32_t issuerTariffId,
                                                                 sl_zigbee_af_price_common_info_t *info,
                                                                 sl_zigbee_af_scheduled_block_thresholds_t *bt)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return false;
  }

  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_common_info_t *lookup_info = &priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep][i];
    sl_zigbee_af_scheduled_block_thresholds_t *lookup_bt = &priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep][i];
    if ((lookup_info->valid)
        && (lookup_bt->status != 0)
        && lookup_bt->issuerTariffId == issuerTariffId) {
      if (info != NULL) {
        memcpy(info, lookup_info, sizeof(sl_zigbee_af_price_common_info_t));
      }
      if (bt != NULL) {
        memcpy(bt, lookup_bt, sizeof(sl_zigbee_af_scheduled_block_thresholds_t));
      }
      return true;
    }
  }

  return false;
}

// Query the tariff table for tariffs matching the selection requirements dictated
// by the GetTariffInformation command. Returns the number of matching tariffs found.
/*
   static uint8_t findTariffs(uint8_t endpoint,
                         uint32_t earliestStartTime,
                         uint32_t minIssuerId,
                         uint8_t numTariffs,
                         sl_zigbee_af_tariff_type_t tariffType,
                         sl_zigbee_af_price_common_info_t *dstInfos,
                         sl_zigbee_af_scheduled_tariff_t *dstTariffs)
   {
   uint8_t ep;
   uint8_t i, tariffsFound = 0;
   sl_zigbee_af_scheduled_tariff_t *curTariff;
   sl_zigbee_af_price_common_info_t *curInfo;

   sl_zigbee_af_debug_println("findTariffs: selection criteria");
   sl_zigbee_af_debug_println("             earliestStartTime: %4x", earliestStartTime);
   sl_zigbee_af_debug_println("             minIssuerId: %4x", minIssuerId);
   sl_zigbee_af_debug_println("             numberOfTariffsRequested: %d", numTariffs);
   sl_zigbee_af_debug_println("             tariffType: %x", tariffType);

   ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
   if (ep == 0xFF) {
    goto kickout;
   }

   while (numTariffs == 0 || tariffsFound < numTariffs) {
    uint32_t referenceStartTime = MAX_INT32U_VALUE;
    uint8_t indexToSend = 0xFF;

    // Find active or scheduled tariffs matching the filter fields in the
    // request that have not been sent out yet.  Of those, find the one that
    // starts the earliest.
    for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
      curTariff = &info.scheduledTariffTable.scheduledTariffs[ep][i];
      curInfo = &info.scheduledTariffTable.commonInfos[ep][i];

      if (!tariffIsPublished(curTariff)
          && (curInfo->startTime < referenceStartTime)
          && (minIssuerId == ZCL_PRICE_CLUSTER_WILDCARD_ISSUER_ID
 || curInfo->issuerEventId >= minIssuerId)
          && (curTariff->tariffTypeChargingScheme & TARIFF_TYPE_MASK) == (tariffType & TARIFF_TYPE_MASK)
          && (tariffIsCurrent(curTariff) || tariffIsFuture(tariff))) {
        referenceStartTime =*curTariff->startTime;
        indexToSend = i;
      }
    }

    // If no active or scheduled tariffs were found, it either means there are
    // no active or scheduled tariffs at the specified time or we've already
    // found all of them in previous iterations.  If we did find one, we send
    // it, mark it as sent, and move on.
    if (indexToSend == 0xFF) {
      break;
    } else {
      sl_zigbee_af_debug_println("findTariffs: found matching tariff at index %d", indexToSend);
      memcpy(&dstInfos[tariffsFound],
              &info.scheduledTariffTable.commonInfos[ep][indexToSend],
              sizeof(sl_zigbee_af_price_common_info_t));
      memcpy(&dstTariffs[tariffsFound],
              &info.scheduledTariffTable.scheduledTariffs[ep][indexToSend],
              sizeof(sl_zigbee_af_scheduled_tariff_t));
      tariffsFound++;
      sl_zigbee_af_debug_println("findTariffs: %d tariffs found so far", tariffsFound);
      info.scheduledTariffTable.scheduledTariffs[ep][indexToSend].status |= PUBLISHED;
    }
   }

   // Roll through all the tariffs and clear the published bit. We only use it in
   // in this function to keep track of which tariffs have already been added to
   // the return tariffs array.
   if (tariffsFound > 0) {
    for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
      if (tariffIsPublished(&info.scheduledTariffTable.scheduledTariffs[ep][i])) {
        info.scheduledTariffTable.scheduledTariffs[ep][i].status &= ~PUBLISHED;
      }
    }
   }
   kickout:
   return tariffsFound;
   }
 */
bool sl_zigbee_af_price_add_tariff_table_entry(uint8_t endpoint,
                                               sl_zigbee_af_price_common_info_t *info,
                                               const sl_zigbee_af_scheduled_tariff_t *curTariff)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  uint8_t tableIndex = ZCL_PRICE_INVALID_INDEX;

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to find endpoint (%d)!", endpoint);
    return false;
  }

  // init
  info->valid = true;
  info->actionsPending = true;

  // get table index for new entry
  tableIndex = sl_zigbee_af_price_common_get_common_matching_or_unused_index(priceServerInfo.scheduledTariffTable.commonInfos[ep],
                                                                             SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE,
                                                                             info->issuerEventId,
                                                                             info->startTime,
                                                                             false);

  if (tableIndex == ZCL_PRICE_INVALID_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to add new tariff info entry(%d)!", endpoint);
    return false;
  }

  return sl_zigbee_af_price_set_tariff_table_entry(endpoint,
                                                   tableIndex,
                                                   info,
                                                   curTariff);
}

// Sets the tariff at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_set_tariff_table_entry(uint8_t endpoint,
                                               uint8_t index,
                                               sl_zigbee_af_price_common_info_t *info,
                                               const sl_zigbee_af_scheduled_tariff_t *newTariff)
{
  bool update = false;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to find endpoint (%d)!", endpoint);
    update = false;
    goto kickout;
  }

  if (index >= SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    sl_zigbee_af_price_cluster_println("ERR: Index out of bound! (%d)!", index);
    update = false;
    goto kickout;
  }

  if ((newTariff == NULL)
      || (info == NULL)) {
    priceServerInfo.scheduledTariffTable.commonInfos[ep][index].valid = 0;
    priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index].status = 0;
    update = true;
    goto kickout;
  }

  // this command doesn't use a corresponding duration time, so we set the duration as never ending.
  info->durationSec = ZCL_PRICE_CLUSTER_END_TIME_NEVER;

  memcpy(&priceServerInfo.scheduledTariffTable.commonInfos[ep][index], info, sizeof(sl_zigbee_af_price_common_info_t));
  memcpy(&priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index], newTariff, sizeof(sl_zigbee_af_scheduled_tariff_t));

  sl_zigbee_af_price_cluster_println("Info: Updated TariffInfo(index=%d) with tariff infos below.", index);
  sl_zigbee_af_price_print_tariff(&priceServerInfo.scheduledTariffTable.commonInfos[ep][index],
                                  &priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index]);

  if (priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index].status == 0) {
    priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index].status |= FUTURE;
  }
  update = true;

  kickout:
  if (update) {
    sl_zigbee_af_price_common_sort((sl_zigbee_af_price_common_info_t *)&priceServerInfo.scheduledTariffTable.commonInfos[ep],
                                   (uint8_t *)&priceServerInfo.scheduledTariffTable.scheduledTariffs[ep],
                                   sizeof(sl_zigbee_af_scheduled_tariff_t),
                                   SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE);
    sl_zigbee_af_price_cluster_schedule_tick_cb(endpoint,
                                                SL_ZIGBEE_AF_PRICE_SERVER_CHANGE_TARIFF_INFORMATION_EVENT_MASK);
  }
  return update;
}

bool sl_zigbee_af_price_add_price_matrix(uint8_t endpoint,
                                         sl_zigbee_af_price_common_info_t *info,
                                         sl_zigbee_af_scheduled_price_matrix_t * pm)
{
  uint8_t ep;
  uint8_t tableIndex;
  ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to find endpoint (%d)!", endpoint);
    return false;
  }

  if (info->startTime == ZCL_PRICE_CLUSTER_START_TIME_NOW) {
    info->startTime = sl_zigbee_af_get_current_time();
  }

  tableIndex = sl_zigbee_af_price_common_get_common_matching_or_unused_index(priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep],
                                                                             SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE,
                                                                             info->issuerEventId,
                                                                             info->startTime,
                                                                             false);
  if (tableIndex == ZCL_PRICE_INVALID_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to add block threshold entry!");
    return false;
  }

  info->valid = true;
  info->durationSec = ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED;

  return sl_zigbee_af_price_set_price_matrix(endpoint,
                                             tableIndex,
                                             info,
                                             pm);
}

bool sl_zigbee_af_price_add_price_matrix_raw(uint8_t endpoint,
                                             uint32_t providerId,
                                             uint32_t issuerEventId,
                                             uint32_t startTime,
                                             uint32_t issuerTariffId,
                                             uint8_t commandIndex,
                                             uint8_t numberOfCommands,
                                             uint8_t subPayloadControl,
                                             uint8_t* payload)
{
  sl_zigbee_af_scheduled_price_matrix_t pm;
  sl_zigbee_af_price_common_info_t pmInfo;
  uint16_t payloadIndex = 0;
  uint16_t payloadLength = fieldLength(payload);

  pmInfo.valid = true;
  pmInfo.startTime = startTime;
  pmInfo.issuerEventId = issuerEventId;
  pmInfo.durationSec = ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED;
  pm.issuerTariffId = issuerTariffId;
  pm.providerId = providerId;

  while (payloadIndex + ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUBPAYLOAD_BLOCK_SIZE <= payloadLength) {
    uint32_t price;
    if ((subPayloadControl & 0x01) == 0x01) {
      uint8_t tier = payload[payloadIndex];
      memcpy(&price, &payload[payloadIndex + 1], 4);
      pm.matrix.tier[tier] = price;
      sl_zigbee_af_price_cluster_println("Info: Updating PriceMatrix tier[%d] = 0x%4X",
                                         tier,
                                         price);
    } else if ((subPayloadControl & 0x01) == 0x00) {
      uint8_t blockNumber = payload[payloadIndex] & 0x0F;
      uint8_t tier = (payload[payloadIndex] & 0xF0) >> 4;
      memcpy(&price, &payload[payloadIndex + 1], 4);
      pm.matrix.blockAndTier[tier][blockNumber] = price;
      sl_zigbee_af_price_cluster_println("Info: Updating PriceMatrix blockAndTier[%d][%d] = 0x%4X",
                                         tier,
                                         blockNumber,
                                         price);
    }

    payloadIndex += 5;
  }

  return sl_zigbee_af_price_add_price_matrix(endpoint, &pmInfo, &pm);
}

// Sets the price matrix at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_set_price_matrix(uint8_t endpoint,
                                         uint8_t index,
                                         sl_zigbee_af_price_common_info_t *info,
                                         const sl_zigbee_af_scheduled_price_matrix_t *pm)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX ) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to find endpoint (%d)!", endpoint);
    return false;
  }

  if (index < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    sl_zigbee_af_price_common_info_t * curInfo = &priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep][index];
    sl_zigbee_af_scheduled_price_matrix_t * curPM = &priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep][index];

    if (pm == NULL || info == NULL) {
      curInfo->valid = false;
      curPM->status = 0;
      return true;
    }

    info->actionsPending = true;

    memcpy(curInfo, info, sizeof(sl_zigbee_af_price_common_info_t));
    memcpy(curPM, pm, sizeof(sl_zigbee_af_scheduled_price_matrix_t));

    if (curPM->status == 0) {
      curPM->status |= FUTURE;
    }
    sl_zigbee_af_price_common_sort((sl_zigbee_af_price_common_info_t *)&priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep],
                                   (uint8_t *)&priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep],
                                   sizeof(sl_zigbee_af_scheduled_price_matrix_t),
                                   SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE);
    sl_zigbee_af_price_cluster_schedule_tick_cb(endpoint,
                                                SL_ZIGBEE_AF_PRICE_SERVER_ACTIVATED_PRICE_MATRIX_EVENT_MASK);

    return true;
  }

  return false;
}

bool sl_zigbee_af_price_add_block_thresholds_table_entry(uint8_t endpoint,
                                                         uint32_t providerId,
                                                         uint32_t issuerEventId,
                                                         uint32_t startTime,
                                                         uint32_t issuerTariffId,
                                                         uint8_t commandIndex,
                                                         uint8_t numberOfCommands,
                                                         uint8_t subpayloadControl,
                                                         uint8_t* payload)
{
  sl_zigbee_af_price_common_info_t inf;
  sl_zigbee_af_scheduled_block_thresholds_t bt;
  uint16_t payloadIndex = 0;
  bool status = true;
  uint8_t tableIndex;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  uint16_t payloadLength = fieldLength(payload);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return false;
  }

  if (startTime == ZCL_PRICE_CLUSTER_START_TIME_NOW) {
    startTime = sl_zigbee_af_get_current_time();
  }

  tableIndex = sl_zigbee_af_price_common_get_common_matching_or_unused_index(priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep],
                                                                             SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE,
                                                                             issuerEventId,
                                                                             startTime,
                                                                             false);
  if (tableIndex == ZCL_PRICE_INVALID_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to add block threshold entry!");
    return false;
  }

  inf.startTime = startTime;
  inf.issuerEventId = issuerEventId;
  bt.providerId = providerId;
  bt.issuerTariffId = issuerTariffId;

  if ((subpayloadControl & 0x01) == 0x01) {
    // tier/numberofblockthresholds
    // least sig nibble - number of thresholds
    // most  sig nibble - not used
    // number of thresholds will apply to all tier
    // we assume payload will contain 1 8bit bit/numberOfBlockThresholds
    // followed by block thresholds
    uint8_t i;
    uint8_t numberOfBt = (payload[payloadIndex] & 0x0F);

    payloadIndex++;
    if (payloadIndex + numberOfBt * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE
        > payloadLength) {
      sl_zigbee_af_price_cluster_println("ERR: Invalid number of thresholds(%d) within BlockThresholds payload!",
                                         numberOfBt);
      status = false;
      goto kickout;
    }

    if (numberOfBt > ZCL_PRICE_CLUSTER_MAX_TOU_BLOCKS - 1) {
      sl_zigbee_af_price_cluster_println("ERR: The number(%d) of block threshold is larger than the max (%d)!",
                                         numberOfBt,
                                         ZCL_PRICE_CLUSTER_MAX_TOU_BLOCKS - 1);
      status = false;
      goto kickout;
    }

    for (i = 0; i < numberOfBt; i++) {
      memcpy(&bt.thresholds.block[i],
             &payload[payloadIndex + i * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE],
             ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE);
      sl_zigbee_af_price_cluster_print("Info: Updating block[%d] = 0x",
                                       i);
      sl_zigbee_af_price_cluster_print_buffer(&payload[payloadIndex + i * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE],
                                              ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE,
                                              false);
      sl_zigbee_af_price_cluster_println("");
    }
  } else if ((subpayloadControl & 0x01) == 0x00) {
    // tier/numberofblockthresholds
    // least sig nibble - number of thresholds
    // most  sig nibble - tier
    while (payloadIndex < payloadLength) {
      uint8_t tier = (payload[payloadIndex] & 0xF0) >> 4;
      uint8_t numberOfBt = (payload[payloadIndex] & 0x0F);
      uint8_t i;
      payloadIndex++;

      // do the block thresholds data exist in the payloads?
      if (payloadIndex
          + numberOfBt * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE
          > payloadLength) {
        sl_zigbee_af_price_cluster_println("ERR: Invalid number of thresholds(%d) within BlockThresholds payload!",
                                           numberOfBt);
        status = false;
        goto kickout;
      }

      if (tier > ZCL_PRICE_CLUSTER_MAX_TOU_BLOCK_TIERS) {
        sl_zigbee_af_price_cluster_println("ERR: Invalid tier id within BlockThresholds payload!");
        status = false;
        goto kickout;
      }

      if (numberOfBt > ZCL_PRICE_CLUSTER_MAX_TOU_BLOCKS - 1) {
        sl_zigbee_af_price_cluster_println("ERR: The number(%d) of block threshold is larger than the max (%d)!",
                                           numberOfBt,
                                           ZCL_PRICE_CLUSTER_MAX_TOU_BLOCKS - 1);
        status = false;
        goto kickout;
      }

      for (i = 0; i < numberOfBt; i++) {
        memcpy(&bt.thresholds.blockAndTier[tier][i],
               &payload[payloadIndex + i * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE],
               ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE);
        sl_zigbee_af_price_cluster_print("Info: Updating blockAndTier[%d][%d] = 0x",
                                         tier,
                                         i);
        sl_zigbee_af_price_cluster_print_buffer(&payload[payloadIndex + i * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE],
                                                ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE,
                                                false);
        sl_zigbee_af_price_cluster_println("");
      }
      payloadIndex += numberOfBt * ZCL_PRICE_CLUSTER_BLOCK_THRESHOLDS_PAYLOAD_SIZE;
    }
  }

  kickout:
  if (status) {
    return sl_zigbee_af_price_set_block_thresholds_table_entry(endpoint,
                                                               tableIndex,
                                                               &inf,
                                                               &bt);
  } else {
    return false;
  }
}

// Sets the block thresholds at the index. Returns false if the index is invalid.
bool sl_zigbee_af_price_set_block_thresholds_table_entry(uint8_t endpoint,
                                                         uint8_t index,
                                                         const sl_zigbee_af_price_common_info_t *info,
                                                         const sl_zigbee_af_scheduled_block_thresholds_t *bt)
{
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);
  sl_zigbee_af_price_common_info_t * inf;
  sl_zigbee_af_scheduled_block_thresholds_t * blockThresholds;
  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return false;
  }

  if (index < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE) {
    inf = &priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep][index];
    blockThresholds = &priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep][index];

    if (bt == NULL || info == NULL) {
      inf->valid = false;
      blockThresholds->status = 0;
      return true;
    }

    memcpy(inf,
           info,
           sizeof(sl_zigbee_af_price_common_info_t));
    memcpy(blockThresholds,
           bt,
           sizeof(sl_zigbee_af_scheduled_block_thresholds_t));

    inf->valid = true;
    inf->actionsPending = true;
    inf->durationSec = ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED;
    if (inf->startTime == ZCL_PRICE_CLUSTER_START_TIME_NOW) {
      inf->startTime = sl_zigbee_af_get_current_time();
    }

    if (blockThresholds->status == 0) {
      blockThresholds->status |= FUTURE;
    }

    sl_zigbee_af_price_common_sort((sl_zigbee_af_price_common_info_t *)&priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep],
                                   (uint8_t *)&priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep],
                                   sizeof(sl_zigbee_af_scheduled_block_thresholds_t),
                                   SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE);
    sl_zigbee_af_price_cluster_schedule_tick_cb(endpoint,
                                                SL_ZIGBEE_AF_PRICE_SERVER_ACTIVATED_BLOCK_THRESHOLD_EVENT_MASK);

    return true;
  }
  return false;
}

void sl_zigbee_af_price_print_tariff(const sl_zigbee_af_price_common_info_t *info,
                                     const sl_zigbee_af_scheduled_tariff_t *tariff)
{
  sl_zigbee_af_price_cluster_println(" valid: %d", info->valid);
  sl_zigbee_af_price_cluster_print(" label: ");
  sl_zigbee_af_price_cluster_print_string(tariff->tariffLabel);
  if (sl_zigbee_af_string_length(tariff->tariffLabel) > 0) {
    sl_zigbee_af_price_cluster_print("(0x%X)", sl_zigbee_af_string_length(tariff->tariffLabel));
  }
  sl_zigbee_af_price_cluster_print("\n uom/cur: 0x%X/0x%2X"
                                   "\r\n pid/eid/etid: 0x%4X/0x%4X/0x%4X"
                                   "\r\n st/tt: 0x%4X/0x%X",
                                   tariff->unitOfMeasure,
                                   tariff->currency,
                                   tariff->providerId,
                                   info->issuerEventId,
                                   tariff->issuerTariffId,
                                   info->startTime,
                                   tariff->tariffTypeChargingScheme);
  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_print("\r\n ptu/btu: 0x%X/0x%X"
                                   "\r\n ptd/sc/tbm: 0x%X/0x%4X/0x%X"
                                   "\r\n btm/btd: 0x%4X/0x%4X",
                                   tariff->numberOfPriceTiersInUse,
                                   tariff->numberOfBlockThresholdsInUse,
                                   tariff->priceTrailingDigit,
                                   tariff->standingCharge,
                                   tariff->tierBlockMode,
                                   tariff->blockThresholdMultiplier,
                                   tariff->blockThresholdDivisor);
  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_print("\n tariff is ");
  if (tariffIsCurrent(tariff)) {
    sl_zigbee_af_price_cluster_println("current");
  } else if (tariffIsFuture(tariff)) {
    sl_zigbee_af_price_cluster_println("future");
  } else {
    sl_zigbee_af_price_cluster_println("invalid");
  }
  sl_zigbee_af_price_cluster_flush();
}

void sl_zigbee_af_price_print_price_matrix(uint8_t endpoint,
                                           const sl_zigbee_af_price_common_info_t *info,
                                           const sl_zigbee_af_scheduled_price_matrix_t *pm)
{
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t tariffInfo;
  bool found;
  uint8_t i, j, chargingScheme;

  found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint,
                                                            pm->issuerTariffId,
                                                            &tariffInfo,
                                                            &t);

  if (!found) {
    sl_zigbee_af_price_cluster_print("  No corresponding valid tariff found; price matrix cannot be printed.\n");
    sl_zigbee_af_price_cluster_print("  (NOTE: If printing from command line, be sure the tariff has been pushed to the tariff table.)\n");
    return;
  }

  chargingScheme = t.tariffTypeChargingScheme;

  sl_zigbee_af_price_cluster_print("  provider id: %4x\r\n", pm->providerId);
  sl_zigbee_af_price_cluster_print("  issuer event id: %4x\r\n", info->issuerEventId);
  sl_zigbee_af_price_cluster_print("  issuer tariff id: %4x\r\n", pm->issuerTariffId);
  sl_zigbee_af_price_cluster_print("  start time: %4x\r\n", info->startTime);

  sl_zigbee_af_price_cluster_flush();

  sl_zigbee_af_price_cluster_print("  == matrix contents == \r\n");
  switch (chargingScheme >> 4) {
    case 0: // TOU only
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        sl_zigbee_af_price_cluster_print("  tier %d: %4x\r\n", i, pm->matrix.tier[i]);
      }
      break;
    case 1: // Block only
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        sl_zigbee_af_price_cluster_print("  tier %d (block 1): %4x\r\n", i, pm->matrix.blockAndTier[i][0]);
      }
      break;
    case 2: // TOU and Block
    case 3:
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        for (j = 0; j < t.numberOfBlockThresholdsInUse + 1; j++) {
          sl_zigbee_af_price_cluster_print("  tier %d block %d: %4x\r\n", i, j + 1, pm->matrix.blockAndTier[i][j]);
        }
      }
      break;
    default:
      sl_zigbee_af_price_cluster_print("  Invalid pricing scheme; no contents. \r\n");
      break;
  }

  sl_zigbee_af_price_cluster_print("  == end matrix contents == \r\n");
  sl_zigbee_af_price_cluster_flush();
}

void sl_zigbee_af_price_print_block_thresholds(uint8_t endpoint,
                                               const sl_zigbee_af_price_common_info_t *info,
                                               const sl_zigbee_af_scheduled_block_thresholds_t *bt)
{
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t newInfo;
  bool found;
  uint8_t i, j, tierBlockMode;

  found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint,
                                                            bt->issuerTariffId,
                                                            &newInfo,
                                                            &t);

  if (!found) {
    sl_zigbee_af_price_cluster_print("  No corresponding valid tariff found; block thresholds cannot be printed.\n");
    sl_zigbee_af_price_cluster_print("  (NOTE: If printing from command line, be sure the tariff has been pushed to the tariff table.)\n");
    return;
  }

  tierBlockMode = t.tierBlockMode;

  sl_zigbee_af_price_cluster_print("  provider id: %4x\r\n", bt->providerId);
  sl_zigbee_af_price_cluster_print("  issuer event id: %4x\r\n", info->issuerEventId);
  sl_zigbee_af_price_cluster_print("  issuer tariff id: %4x\r\n", bt->issuerTariffId);
  sl_zigbee_af_price_cluster_print("  start time: %4x\r\n", info->startTime);

  sl_zigbee_af_price_cluster_flush();

  sl_zigbee_af_price_cluster_print("  == thresholds contents == \r\n");
  switch (tierBlockMode) {
    case 0: // ActiveBlock
    case 1: // ActiveBlockPriceTier
      for (j = 0; j < t.numberOfBlockThresholdsInUse; j++) {
        sl_zigbee_af_price_cluster_print("  block threshold %d: 0x%x%x%x%x%x%x\r\n", j,
                                         bt->thresholds.block[j][0],
                                         bt->thresholds.block[j][1],
                                         bt->thresholds.block[j][2],
                                         bt->thresholds.block[j][3],
                                         bt->thresholds.block[j][4],
                                         bt->thresholds.block[j][5]);
      }
      break;
    case 2: // ActiveBlockPriceTierThreshold
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        for (j = 0; j < t.numberOfBlockThresholdsInUse; j++) {
          sl_zigbee_af_price_cluster_print("  tier %d block threshold %d: 0x%x%x%x%x%x%x\r\n", i, j,
                                           bt->thresholds.blockAndTier[i][j][0],
                                           bt->thresholds.blockAndTier[i][j][1],
                                           bt->thresholds.blockAndTier[i][j][2],
                                           bt->thresholds.blockAndTier[i][j][3],
                                           bt->thresholds.blockAndTier[i][j][4],
                                           bt->thresholds.blockAndTier[i][j][5]);
        }
      }
      break;
    case 0xFF://Not used: in case of TOU tariff or Block tariff charging scheme only
      if ((t.tariffTypeChargingScheme & CHARGING_SCHEME_MASK) == 0x10) {//block tariff only
        for (j = 0; j < t.numberOfBlockThresholdsInUse; j++) {
          sl_zigbee_af_price_cluster_print("  block threshold %d: 0x%x%x%x%x%x%x\r\n", j,
                                           bt->thresholds.block[j][0],
                                           bt->thresholds.block[j][1],
                                           bt->thresholds.block[j][2],
                                           bt->thresholds.block[j][3],
                                           bt->thresholds.block[j][4],
                                           bt->thresholds.block[j][5]);
        }
      } else if ((t.tariffTypeChargingScheme & CHARGING_SCHEME_MASK) == 0x00) {//TOU tariff only
        for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
          for (j = 0; j < t.numberOfBlockThresholdsInUse; j++) {
            sl_zigbee_af_price_cluster_print("  tier %d block threshold %d: 0x%x%x%x%x%x%x\r\n", i, j,
                                             bt->thresholds.blockAndTier[i][j][0],
                                             bt->thresholds.blockAndTier[i][j][1],
                                             bt->thresholds.blockAndTier[i][j][2],
                                             bt->thresholds.blockAndTier[i][j][3],
                                             bt->thresholds.blockAndTier[i][j][4],
                                             bt->thresholds.blockAndTier[i][j][5]);
          }
        }
      } else {
        sl_zigbee_af_price_cluster_print("invalid tariff charging scheme 0x%x for tierblockmode 0xFF", t.tariffTypeChargingScheme);
      }
      break;
    default:
      sl_zigbee_af_price_cluster_print("  Invalid tier block mode; no contents. \r\n");
      break;
  }

  sl_zigbee_af_price_cluster_print("  == end thresholds contents == \r\n");
  sl_zigbee_af_price_cluster_flush();
}

void sl_zigbee_af_price_print_tariff_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("Tariff Table Contents: ");
  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("  Note: ALL values given in HEX\r\n");
  sl_zigbee_af_price_cluster_flush();
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_cluster_println("= TARIFF %d =",
                                       i);
    sl_zigbee_af_price_print_tariff(&priceServerInfo.scheduledTariffTable.commonInfos[ep][i],
                                    &priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][i]);
  }
}

void sl_zigbee_af_price_print_price_matrix_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("Price Matrix Table Contents: ");
  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("  Note: ALL values given in HEX (except indices)\r\n");
  sl_zigbee_af_price_cluster_flush();
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_cluster_println("=PRICE MATRIX %x =",
                                       i);
    sl_zigbee_af_price_print_price_matrix(endpoint,
                                          &priceServerInfo.scheduledPriceMatrixTable.commonInfos[ep][i],
                                          &priceServerInfo.scheduledPriceMatrixTable.scheduledPriceMatrix[ep][i]);
  }
}

void sl_zigbee_af_price_print_block_thresholds_table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    return;
  }

  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("Block Thresholds Table Contents: ");
  sl_zigbee_af_price_cluster_flush();
  sl_zigbee_af_price_cluster_println("  Note: ALL values given in HEX (except indices)\r\n");
  sl_zigbee_af_price_cluster_flush();
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE; i++) {
    sl_zigbee_af_price_cluster_println("=BLOCK THRESHOLDS %x =",
                                       i);
    sl_zigbee_af_price_print_block_thresholds(endpoint,
                                              &priceServerInfo.scheduledBlockThresholdsTable.commonInfos[ep][i],
                                              &priceServerInfo.scheduledBlockThresholdsTable.scheduledBlockThresholds[ep][i]);
  }
}

static void sl_zigbee_af_put_price_block_threshold_in_resp(sli_zigbee_af_price_block_threshold *threshold)
{
  uint16_t length = sizeof(sli_zigbee_af_price_block_threshold);

#if BIGENDIAN_CPU
  int8_t loc  = length - 1;
  int8_t end  = -1;
  int8_t incr = -1;
#else
  int8_t loc  = 0;
  int8_t end  = length;
  int8_t incr = 1;
#endif

  while ( loc != end ) {
    (void) sl_zigbee_af_put_int8u_in_resp(((uint8_t *)threshold)[loc]);
    loc += incr;
  }
}

//-----------------------
// ZCL Commands Callbacks

bool sl_zigbee_af_price_cluster_get_tariff_information_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_price_cluster_get_tariff_information_command_t cmd_data;
  uint8_t validCmds[SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE];
  uint8_t ep = sl_zigbee_af_find_cluster_server_endpoint_index(sl_zigbee_af_current_endpoint(), ZCL_PRICE_CLUSTER_ID);
  uint8_t entriesCount;
  uint8_t validEntriesCount;
  UNUSED_VAR(validEntriesCount);

  if (zcl_decode_price_cluster_get_tariff_information_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  sl_zigbee_af_debug_println("RX: GetTariffInformation, 0x%4X, 0x%4X, 0x%X, 0x%X",
                             cmd_data.earliestStartTime,
                             cmd_data.minIssuerEventId,
                             cmd_data.numberOfCommands,
                             cmd_data.tariffType);

  if (ep == ZCL_PRICE_INVALID_ENDPOINT_INDEX) {
    sl_zigbee_af_price_cluster_println("ERR: Unable to find endpoint (%d)!", ep);
    sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_INVALID_VALUE);
    return true;
  }

  entriesCount = sl_zigbee_af_price_common_find_valid_entries(validCmds,
                                                              SL_ZIGBEE_AF_PLUGIN_PRICE_SERVER_TARIFF_TABLE_SIZE,
                                                              (sl_zigbee_af_price_common_info_t *)priceServerInfo.scheduledTariffTable.commonInfos[ep],
                                                              cmd_data.earliestStartTime,
                                                              cmd_data.minIssuerEventId,
                                                              cmd_data.numberOfCommands);
  validEntriesCount = entriesCount;

  // eliminate commands with mismatching tariffType
  // upper nibble is reserved. we'll ignore them.
  {
    uint8_t i;
    for (i = 0; i < entriesCount; i++) {
      uint8_t index = validCmds[i];
      if ((priceServerInfo.scheduledTariffTable.scheduledTariffs[ep][index].tariffTypeChargingScheme & TARIFF_TYPE_MASK) != (cmd_data.tariffType & TARIFF_TYPE_MASK)) {
        validCmds[i] = ZCL_PRICE_INVALID_INDEX;
        validEntriesCount--;
      }
    }
  }

  sl_zigbee_af_debug_println("Tariffs found: %d", validEntriesCount);
  sendValidCmdEntries(ZCL_PUBLISH_TARIFF_INFORMATION_COMMAND_ID,
                      ep,
                      validCmds,
                      entriesCount);

  return true;
}

bool sl_zigbee_af_price_cluster_get_price_matrix_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_price_cluster_get_price_matrix_command_t cmd_data;
  sl_zigbee_af_scheduled_tariff_t tariff;
  sl_zigbee_af_price_common_info_t tariffInfo;
  sl_zigbee_af_scheduled_price_matrix_t pm;
  sl_zigbee_af_price_common_info_t pmInfo;
  bool found;

  if (zcl_decode_price_cluster_get_price_matrix_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  uint8_t endpoint = sl_zigbee_af_current_endpoint(), i, j, payloadControl;
  uint16_t size = 0;
  // Allocate for the largest possible size, unfortunately
  uint8_t subPayload[ZCL_PRICE_CLUSTER_MAX_TOU_BLOCKS
                     * ZCL_PRICE_CLUSTER_MAX_TOU_BLOCK_TIERS
                     * ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE];

  // A price matrix must have an associated tariff, otherwise it is meaningless
  found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint,
                                                            cmd_data.issuerTariffId,
                                                            &tariffInfo,
                                                            &tariff);

  if (!found) {
    sl_zigbee_af_debug_println("GetPriceMatrix: no corresponding tariff for id 0x%4x found",
                               cmd_data.issuerTariffId);
    sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // Grab the actual price matrix
  found = sl_zigbee_af_price_get_price_matrix_by_issuer_tariff_id(endpoint,
                                                                  cmd_data.issuerTariffId,
                                                                  &pmInfo,
                                                                  &pm);

  if (!found) {
    sl_zigbee_af_debug_println("GetPriceMatrix: no corresponding price matrix for id 0x%4x found",
                               cmd_data.issuerTariffId);
    sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // The structure of the price matrix will vary depending on the type of the tariff
  switch (tariff.tariffTypeChargingScheme >> 4) {
    case 0: // TOU only
      payloadControl = 1;
      for (i = 0; i < tariff.numberOfPriceTiersInUse; i++) {
        subPayload[i * ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE] = i + 1;
        sl_zigbee_af_copy_int32u(subPayload,
                                 i * ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE + 1,
                                 pm.matrix.tier[i]);
        size += ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE;
      }
      break;
    case 1: // Block only
      payloadControl = 0;
      for (i = 0; i < tariff.numberOfPriceTiersInUse; i++) {
        subPayload[i * ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE] = i << 4;
        sl_zigbee_af_copy_int32u(subPayload,
                                 i * ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE + 1,
                                 pm.matrix.blockAndTier[i][0]);
        size += ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE;
      }
      break;
    case 2:
    case 3: // TOU / Block combined
      payloadControl = 0;
      for (i = 0; i < tariff.numberOfPriceTiersInUse; i++) {
        for (j = 0; j < tariff.numberOfBlockThresholdsInUse + 1; j++) {
          subPayload[size] = (i << 4) | j;
          sl_zigbee_af_copy_int32u(subPayload,
                                   size + 1,
                                   pm.matrix.blockAndTier[i][j]);
          size += ZCL_PRICE_CLUSTER_PRICE_MATRIX_SUB_PAYLOAD_ENTRY_SIZE;
        }
      }
      break;
    default:
      sl_zigbee_af_debug_println("GetPriceMatrix: invalid tariff type / charging scheme");
      sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_INVALID_VALUE);
      return true;
  }

  // Populate and send the PublishPriceMatrix command
  sl_zigbee_af_debug_println("GetPriceMatrix: subpayload size 0x%2x", size);

#ifdef SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
  // GBCS explicitly lists some commands that need to be sent with "disable
  // default response" flag set. This is one of them.
  // We make it conditional on GBCS so it does not affect standard SE apps.
  sl_zigbee_af_set_disable_default_response(SL_ZIGBEE_AF_DISABLE_DEFAULT_RESPONSE_ONE_SHOT);
#endif

  sl_zigbee_af_fill_command_price_cluster_publish_price_matrix(pm.providerId,
                                                               pmInfo.issuerEventId,
                                                               pmInfo.startTime,
                                                               pm.issuerTariffId,
                                                               0,
                                                               1,
                                                               payloadControl,
                                                               subPayload,
                                                               size);
  sl_zigbee_af_send_response();

  return true;
}

bool sl_zigbee_af_price_cluster_get_block_thresholds_cb(sl_zigbee_af_cluster_command_t *cmd)
{
  sl_zcl_price_cluster_get_block_thresholds_command_t cmd_data;
  sl_zigbee_af_price_common_info_t tariffInfo;
  sl_zigbee_af_scheduled_tariff_t tariff;
  sl_zigbee_af_price_common_info_t btInfo;
  sl_zigbee_af_scheduled_block_thresholds_t bt;
  bool found;
  uint8_t endpoint = sl_zigbee_af_current_endpoint(), i, j;
  uint16_t size = 0;
  UNUSED_VAR(size);

  if (zcl_decode_price_cluster_get_block_thresholds_command(cmd, &cmd_data)
      != SL_ZIGBEE_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // Block thresholds must have an associated tariff, otherwise it is meaningless
  found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint,
                                                            cmd_data.issuerTariffId,
                                                            &tariffInfo,
                                                            &tariff);

  if (!found) {
    sl_zigbee_af_debug_println("GetBlockThresholds: no corresponding tariff for id 0x%4x found",
                               cmd_data.issuerTariffId);
    sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // Grab the actual block thresholds
  found = sl_zigbee_af_price_get_block_thresholds_by_issuer_tariff_id(endpoint,
                                                                      cmd_data.issuerTariffId,
                                                                      &btInfo,
                                                                      &bt);

  if (!found) {
    sl_zigbee_af_debug_println("GetBlockThresholds: no corresponding block thresholds for id 0x%4x found",
                               cmd_data.issuerTariffId);
    sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_NOT_FOUND);
    return true;
  }

  // Populate and send the PublishBlockThresholds command
  (void) sl_zigbee_af_fill_external_buffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                            | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                           ZCL_PRICE_CLUSTER_ID,
                                           ZCL_PUBLISH_BLOCK_THRESHOLDS_COMMAND_ID,
                                           "wwwwuu",
                                           bt.providerId,
                                           btInfo.issuerEventId,
                                           btInfo.startTime,
                                           bt.issuerTariffId,
                                           0,
                                           1);

  // The structure of the block thresholds subpayload will vary depending on the tier block mode
  switch (tariff.tierBlockMode) {
    case 0: // ActiveBlock
    case 1: // ActiveBlockPriceTier
      (void) sl_zigbee_af_put_int8u_in_resp(1); // payload control
      (void) sl_zigbee_af_put_int8u_in_resp(tariff.numberOfBlockThresholdsInUse);
      size += 1;
      for (j = 0; j < tariff.numberOfBlockThresholdsInUse; j++) {
        sl_zigbee_af_put_price_block_threshold_in_resp(&bt.thresholds.block[j]);
        size += 6;
      }
      break;
    case 2: // ActiveBlockPriceTierThreshold
      (void) sl_zigbee_af_put_int8u_in_resp(0); // payload control
      for (i = 0; i < tariff.numberOfPriceTiersInUse; i++) {
        (void) sl_zigbee_af_put_int8u_in_resp((i << 4) | tariff.numberOfBlockThresholdsInUse);
        size += 1;
        for (j = 0; j < tariff.numberOfBlockThresholdsInUse; j++) {
          sl_zigbee_af_put_price_block_threshold_in_resp(&bt.thresholds.blockAndTier[i][j]);
          size += 6;
        }
      }
      break;
    case 0xFF://Not used: in case of TOU tariff or Block tariff charging scheme only
      if ((tariff.tariffTypeChargingScheme & CHARGING_SCHEME_MASK) == 0x10) {//block tariff only
        (void) sl_zigbee_af_put_int8u_in_resp(1); // payload control
        (void) sl_zigbee_af_put_int8u_in_resp(tariff.numberOfBlockThresholdsInUse);
        size += 1;
        for (j = 0; j < tariff.numberOfBlockThresholdsInUse; j++) {
          sl_zigbee_af_put_price_block_threshold_in_resp(&bt.thresholds.block[j]);
          size += 6;
        }
      } else if ((tariff.tariffTypeChargingScheme & CHARGING_SCHEME_MASK) == 0x00) {//TOU tariff only
        (void) sl_zigbee_af_put_int8u_in_resp(0); // payload control
        for (i = 0; i < tariff.numberOfPriceTiersInUse; i++) {
          (void) sl_zigbee_af_put_int8u_in_resp((i << 4) | tariff.numberOfBlockThresholdsInUse);
          size += 1;
          for (j = 0; j < tariff.numberOfBlockThresholdsInUse; j++) {
            sl_zigbee_af_put_price_block_threshold_in_resp(&bt.thresholds.blockAndTier[i][j]);
            size += 6;
          }
        }
      } else {
        sl_zigbee_af_debug_println("GetBlockThresholds: invalid tariff charging scheme 0x%x for tierblockmode 0xFF",
                                   tariff.tariffTypeChargingScheme);
        sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_INVALID_VALUE);
        return true;
      }
      break;
    default:
      sl_zigbee_af_debug_println("GetBlockThresholds: invalid tier block mode");
      sl_zigbee_af_send_immediate_default_response(SL_ZIGBEE_ZCL_STATUS_INVALID_VALUE);
      return true;
  }

  sl_zigbee_af_debug_println("GetBlockThresholds: subpayload size 0x%2x", size);
  sl_zigbee_af_send_response();

  return true;
}
