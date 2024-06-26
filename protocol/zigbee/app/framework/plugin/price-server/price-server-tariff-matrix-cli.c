/***************************************************************************//**
 * @file
 * @brief Matrix CLI for the Price Server plugin.
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
#include "app/framework/plugin/price-server/price-server.h"
#include "sl_cli.h"

//=============================================================================
// Globals

static sl_zigbee_af_price_common_info_t          scheduledTariffInfo;
static sl_zigbee_af_scheduled_tariff_t          scheduledTariff;
static sl_zigbee_af_price_common_info_t          btInfo;
static sl_zigbee_af_scheduled_block_thresholds_t bt;
static sl_zigbee_af_price_common_info_t          pmInfo;
static sl_zigbee_af_scheduled_price_matrix_t     pm;

//=============================================================================
// Functions

// plugin price-server tclear <endpoint:1>
void sli_zigbee_af_price_server_cli_t_clear(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_price_clear_tariff_table(sl_cli_get_argument_uint8(arguments, 0));
}

// plugin price-server pmclear <endpoint:1>
void sli_zigbee_af_price_server_cli_pm_clear(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_price_clear_price_matrix_table(sl_cli_get_argument_uint8(arguments, 0));
}

// plugin price-server btclear <endpoint:1>
void sli_zigbee_af_price_server_cli_bt_clear(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_price_clear_block_thresholds_table(sl_cli_get_argument_uint8(arguments, 0));
}

// plugin price-server twho <provId:4> <label:1-13> <eventId:4> <tariffId:4>
void sli_zigbee_af_price_server_cli_t_who(sl_cli_command_arg_t *arguments)
{
  uint8_t length;
  scheduledTariff.providerId = sl_cli_get_argument_uint32(arguments, 0);
  length = sl_zigbee_copy_string_arg(arguments, 1,
                                     scheduledTariff.tariffLabel + 1,
                                     ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH,
                                     false);
  scheduledTariff.tariffLabel[0] = length;
  scheduledTariffInfo.issuerEventId = sl_cli_get_argument_uint32(arguments, 2);
  scheduledTariff.issuerTariffId = sl_cli_get_argument_uint32(arguments, 3);
}

// plugin price-server twhat <type:1> <unitOfMeas:1> <curr:2> <ptd:1> <prt:1> <btu:1> <blockMode:1>
void sli_zigbee_af_price_server_cli_t_what(sl_cli_command_arg_t *arguments)
{
  scheduledTariff.tariffTypeChargingScheme = sl_cli_get_argument_uint8(arguments, 0);
  scheduledTariff.unitOfMeasure = sl_cli_get_argument_uint8(arguments, 1);
  scheduledTariff.currency = sl_cli_get_argument_uint16(arguments, 2);
  scheduledTariff.priceTrailingDigit = sl_cli_get_argument_uint8(arguments, 3);
  scheduledTariff.numberOfPriceTiersInUse = sl_cli_get_argument_uint8(arguments, 4);
  scheduledTariff.numberOfBlockThresholdsInUse = sl_cli_get_argument_uint8(arguments, 5);
  scheduledTariff.tierBlockMode = sl_cli_get_argument_uint8(arguments, 6);
}

// plugin price-server twhen <startTime:4>
void sli_zigbee_af_price_server_cli_t_when(sl_cli_command_arg_t *arguments)
{
  scheduledTariffInfo.startTime = sl_cli_get_argument_uint32(arguments, 0);
  scheduledTariffInfo.durationSec = ZCL_PRICE_CLUSTER_DURATION_SEC_UNTIL_CHANGED;
}

// plugin price-server tariff <standingCharge:4> <btm:4> <btd:4>
void sli_zigbee_af_price_server_cli_tariff(sl_cli_command_arg_t *arguments)
{
  scheduledTariff.standingCharge = sl_cli_get_argument_uint32(arguments, 0);
  scheduledTariff.blockThresholdMultiplier = sl_cli_get_argument_uint32(arguments, 1);
  scheduledTariff.blockThresholdDivisor = sl_cli_get_argument_uint32(arguments, 2);
}

// plugin price-server tadd <endpoint:1> <status:1>
void sli_zigbee_af_price_server_cli_t_add(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t status = sl_cli_get_argument_uint8(arguments, 1);

  if (status == 0) {
    scheduledTariff.status |= CURRENT;
  } else {
    scheduledTariff.status |= FUTURE;
  }

  if (!sl_zigbee_af_price_add_tariff_table_entry(endpoint,
                                                 &scheduledTariffInfo,
                                                 &scheduledTariff)) {
    sl_zigbee_af_price_cluster_println("ERR:Failed to set table entry!");
  }
}

// plugin price-server tget <endpoint:1> <index:1>
void sli_zigbee_af_price_server_cli_t_get(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);
  if (!sl_zigbee_af_price_get_tariff_table_entry(endpoint,
                                                 index,
                                                 &scheduledTariffInfo,
                                                 &scheduledTariff)) {
    sl_zigbee_af_price_cluster_println("tariff entry %d not present", index);
  }
}

// plugin price-server tset <endpoint:1> <index:1> <status:1>
void sli_zigbee_af_price_server_cli_t_set(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t status = sl_cli_get_argument_uint8(arguments, 2);

  scheduledTariffInfo.valid = true;
  scheduledTariff.status = status;

  if (!sl_zigbee_af_price_set_tariff_table_entry(endpoint,
                                                 index,
                                                 &scheduledTariffInfo,
                                                 &scheduledTariff)) {
    sl_zigbee_af_price_cluster_println("tariff entry %d not present", index);
  }
}

// plugin price-server tprint <endpoint:1>
void sli_zigbee_af_price_server_cli_t_print(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  sl_zigbee_af_price_print_tariff_table(endpoint);
}

// plugin price-server tsprint
void sli_zigbee_af_price_server_cli_ts_print(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_price_print_tariff(&scheduledTariffInfo, &scheduledTariff);
}

// plugin price-server pmprint <endpoint:1>
void sli_zigbee_af_price_server_cli_pm_print(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  sl_zigbee_af_price_print_price_matrix_table(endpoint);
}

// plugin price-server pm set-metadata <endpoint:1> <providerId:4> <issuerEventId:4>
//                                     <issuerTariffId:4> <startTime:4> <status:1>
void sli_zigbee_af_price_server_cli_pm_set_metadata(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint32_t tariffId = sl_cli_get_argument_uint32(arguments, 3);
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;

  if (!(sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, tariffId, &i, &t)
        || scheduledTariff.issuerTariffId == tariffId)) {
    sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID; no corresponding tariff found.\n");
    return;
  }

  pm.providerId = sl_cli_get_argument_uint32(arguments, 1);
  pmInfo.issuerEventId = sl_cli_get_argument_uint32(arguments, 2);
  pm.issuerTariffId = tariffId;
  pmInfo.startTime = sl_cli_get_argument_uint32(arguments, 4);
  pm.status = sl_cli_get_argument_uint8(arguments, 5);
}

// plugin price-server pm set-provider <providerId:4>
void sli_zigbee_af_price_server_cli_pm_set_provider(sl_cli_command_arg_t *arguments)
{
  pm.providerId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server pm set-event <issuerEventId:4>
void sli_zigbee_af_price_server_cli_pm_set_event(sl_cli_command_arg_t *arguments)
{
  pmInfo.issuerEventId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server pm set-tariff <issuerTariffId:4>
void sli_zigbee_af_price_server_cli_pm_set_tariff(sl_cli_command_arg_t *arguments)
{
  pm.issuerTariffId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server pm set-time <startTime:4>
void sli_zigbee_af_price_server_cli_pm_set_time(sl_cli_command_arg_t *arguments)
{
  pmInfo.startTime = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server pm set-status <status:1>
void sli_zigbee_af_price_server_cli_pm_set_status(sl_cli_command_arg_t *arguments)
{
  pm.status = sl_cli_get_argument_uint8(arguments, 0);
}

// plugin price-server pm get <endpoint:1> <index:1>
void sli_zigbee_af_price_server_cli_pm_get(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);
  sl_zigbee_af_price_get_price_matrix(endpoint, index, &pmInfo, &pm);
}

// plugin price-server pm add <endpoint:1>
void sli_zigbee_af_price_server_cli_pm_add(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;

  if (!(sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, pm.issuerTariffId, &i, &t)
        || scheduledTariff.issuerTariffId == pm.issuerTariffId)) {
    sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID; no corresponding tariff found.\n");
    return;
  }

  sl_zigbee_af_price_add_price_matrix(endpoint, &pmInfo, &pm);
}

// plugin price-server pm put <endpoint:1> <tier:1> <block:1> <price:4>
void sli_zigbee_af_price_server_cli_pm_put(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t tier = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t block = sl_cli_get_argument_uint8(arguments, 2);
  uint32_t price = sl_cli_get_argument_uint32(arguments, 3);
  uint8_t chargingScheme;
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;

  if (pm.issuerTariffId == scheduledTariff.issuerTariffId) {
    t = scheduledTariff;
    chargingScheme = scheduledTariff.tariffTypeChargingScheme;
  } else {
    bool found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, pm.issuerTariffId, &i, &t);
    if (!found) {
      sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID in price matrix; no corresponding tariff found.\n");
      return;
    } else {
      chargingScheme = t.tariffTypeChargingScheme;
    }
  }

  if (tier >= t.numberOfPriceTiersInUse
      || block > t.numberOfBlockThresholdsInUse) {
    sl_zigbee_af_price_cluster_print("Invalid index into price matrix. Value not set.\n");
    return;
  }

  switch (chargingScheme >> 4) {
    case 0: // TOU only
      pm.matrix.tier[tier] = price;
      break;
    case 1: // Block only
      pm.matrix.blockAndTier[tier][0] = price;
      break;
    case 2:
    case 3: // TOU and Block
      pm.matrix.blockAndTier[tier][block] = price;
      break;
    default:
      sl_zigbee_af_debug_println("Invalid tariff type / charging scheme.");
      break;
  }
}

// plugin price-server pm fill-tier <endpoint:1> <tier:1> <price:4>
void sli_zigbee_af_price_server_cli_pm_fill_tier(sl_cli_command_arg_t *arguments)
{
  uint8_t chargingScheme, i;
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t inf;
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t tier = sl_cli_get_argument_uint8(arguments, 1);
  uint32_t price = sl_cli_get_argument_uint32(arguments, 2);

  if (pm.issuerTariffId == scheduledTariff.issuerTariffId) {
    t = scheduledTariff;
    chargingScheme = scheduledTariff.tariffTypeChargingScheme;
  } else {
    bool found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, pm.issuerTariffId, &inf, &t);
    if (!found) {
      sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID in price matrix; no corresponding tariff found.\n");
      return;
    } else {
      chargingScheme = t.tariffTypeChargingScheme;
    }
  }

  if (tier >= t.numberOfPriceTiersInUse) {
    sl_zigbee_af_price_cluster_print("Tier exceeds number of price tiers in use for this tariff. Values not set.\n");
    return;
  }

  switch (chargingScheme >> 4) {
    case 0: // TOU only
      pm.matrix.tier[tier] = price;
      break;
    case 1: // Block only
      pm.matrix.blockAndTier[tier][0] = price;
      break;
    case 2:
    case 3: // TOU and Block
      for (i = 0; i < t.numberOfBlockThresholdsInUse + 1; i++) {
        pm.matrix.blockAndTier[tier][i] = price;
      }
      break;
    default:
      sl_zigbee_af_debug_println("Invalid tariff type / charging scheme.");
      break;
  }
}

// plugin price-server pm fill-tier <endpoint:1> <block:1> <price:4>
void sli_zigbee_af_price_server_cli_pm_fill_block(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t block = sl_cli_get_argument_uint8(arguments, 1);
  uint32_t price = sl_cli_get_argument_uint32(arguments, 2);
  uint8_t chargingScheme, i;
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t inf;

  if (pm.issuerTariffId == scheduledTariff.issuerTariffId) {
    t = scheduledTariff;
    chargingScheme = scheduledTariff.tariffTypeChargingScheme;
  } else {
    bool found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, pm.issuerTariffId, &inf, &t);
    if (!found) {
      sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID in price matrix; no corresponding tariff found.\n");
      return;
    } else {
      chargingScheme = t.tariffTypeChargingScheme;
    }
  }

  if ( block > t.numberOfBlockThresholdsInUse) {
    sl_zigbee_af_price_cluster_print("Block exceeds number of block thresholds in use for this tariff. Values not set.\n");
    return;
  }

  switch (chargingScheme >> 4) {
    case 0: // TOU only
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        pm.matrix.tier[i] = price;
      }
      break;
    case 1: // Block only
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        pm.matrix.blockAndTier[i][0] = price;
      }
      break;
    case 2:
    case 3: // TOU and Block
      for (i = 0; i < t.numberOfPriceTiersInUse; i++) {
        pm.matrix.blockAndTier[i][block] = price;
      }
      break;
    default:
      sl_zigbee_af_debug_println("Invalid tariff type / charging scheme.");
      break;
  }
}

// plugin price-server btprint <endpoint:1>
void sli_zigbee_af_price_server_cli_bt_print(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  sl_zigbee_af_price_print_block_thresholds_table(endpoint);
}

// plugin price-server bt set-metadata <endpoint:1> <providerId:4> <issuerEventId:4>
//                                     <issuerTariffId:4> <startTime:4> <status:1>
void sli_zigbee_af_price_server_cli_bt_set_metadata(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint32_t tariffId = sl_cli_get_argument_uint32(arguments, 3);
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;

  if (!(sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, tariffId, &i, &t)
        || scheduledTariff.issuerTariffId == tariffId)) {
    sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID; no corresponding tariff found.\n");
    return;
  }

  btInfo.issuerEventId = sl_cli_get_argument_uint32(arguments, 2);
  btInfo.startTime = sl_cli_get_argument_uint32(arguments, 4);
  bt.issuerTariffId = tariffId;
  bt.providerId = sl_cli_get_argument_uint32(arguments, 1);
  bt.issuerTariffId = tariffId;
  bt.status = sl_cli_get_argument_uint8(arguments, 5);
}

// plugin price-server bt set-provider <providerId:4>
void sli_zigbee_af_price_server_cli_bt_set_provider(sl_cli_command_arg_t *arguments)
{
  bt.providerId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server bt set-event <issuerEventId:4>
void sli_zigbee_af_price_server_cli_bt_set_event(sl_cli_command_arg_t *arguments)
{
  btInfo.issuerEventId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server bt set-tariff <issuerTariffId:4>
void sli_zigbee_af_price_server_cli_bt_set_tariff(sl_cli_command_arg_t *arguments)
{
  bt.issuerTariffId = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server bt set-time <startTime:4>
void sli_zigbee_af_price_server_cli_bt_set_time(sl_cli_command_arg_t *arguments)
{
  btInfo.startTime = sl_cli_get_argument_uint32(arguments, 0);
}

// plugin price-server bt set-status <status:1>
void sli_zigbee_af_price_server_cli_bt_set_status(sl_cli_command_arg_t *arguments)
{
  bt.status = sl_cli_get_argument_uint8(arguments, 0);
}

// plugin price-server bt get <endpoint:1> <index:1>
void sli_zigbee_af_price_server_cli_bt_get(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);

  sl_zigbee_af_price_get_block_thresholds_table_entry(endpoint, index, &bt);
}

// plugin price-server bt set <endpoint:1> <index:1>
void sli_zigbee_af_price_server_cli_bt_set(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t index = sl_cli_get_argument_uint8(arguments, 1);
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;

  if (!sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, bt.issuerTariffId, &i, &t)) {
    sl_zigbee_af_price_cluster_print("Invalid issuer tariff ID; no corresponding tariff found.\n");
    return;
  }

  sl_zigbee_af_price_set_block_thresholds_table_entry(endpoint, index, &btInfo, &bt);
  memset(&btInfo, 0x00, sizeof(sl_zigbee_af_price_common_info_t));
  memset(&bt, 0x00, sizeof(sl_zigbee_af_scheduled_block_thresholds_t));
}

// plugin price-server bt put <endpoint:1> <tier:1> <block:1> <threshold:6>
void sli_zigbee_af_price_server_cli_bt_put(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t tier = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t block = sl_cli_get_argument_uint8(arguments, 2);
  sli_zigbee_af_price_block_threshold threshold;
  uint8_t tierBlockMode;
  sl_zigbee_af_scheduled_tariff_t t;
  sl_zigbee_af_price_common_info_t i;
  uint8_t *dst = NULL;
  sl_zigbee_copy_hex_arg(arguments, 3, (uint8_t *)&threshold, sizeof(sli_zigbee_af_price_block_threshold), true);

  if (bt.issuerTariffId == scheduledTariff.issuerTariffId) {
    t = scheduledTariff;
    tierBlockMode = scheduledTariff.tierBlockMode;
  } else {
    bool found = sl_zigbee_af_price_get_tariff_by_issuer_tariff_id(endpoint, bt.issuerTariffId, &i, &t);
    if (!found) {
      sl_zigbee_af_price_cluster_println("Invalid issuer tariff ID in block thresholds; no corresponding tariff found.");
      return;
    } else {
      tierBlockMode = t.tierBlockMode;
    }
  }

  if (tier >= t.numberOfPriceTiersInUse
      || block > t.numberOfBlockThresholdsInUse) {
    sl_zigbee_af_price_cluster_println("Invalid index into block thresholds. Value not set.");
    return;
  }

  switch (tierBlockMode) {
    case 0: // ActiveBlock
    case 1: // ActiveBlockPriceTier
      dst = (uint8_t *)&bt.thresholds.block[block];
      break;
    case 2: // ActiveBlockPriceTierThreshold
      dst = (uint8_t *)&bt.thresholds.blockAndTier[tier][block];
      break;
    default:
      sl_zigbee_af_debug_println("Invalid tier block mode.");
      break;
  }

  if (dst != NULL) {
#if BIGENDIAN_CPU
    memcpy(dst, &threshold, sizeof(sli_zigbee_af_price_block_threshold));
#else
    uint8_t i;
    for (i = 0; i < sizeof(sli_zigbee_af_price_block_threshold); i++) {
      dst[i] = threshold[sizeof(sli_zigbee_af_price_block_threshold) - 1 - i];
    }
#endif
  }
}
