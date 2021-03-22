/***************************************************************************//**
 * @file
 * @brief Plugin code for better DMP Tuning and Testing.
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
#include "dmp-tuning-profiles.h"
#include "sl_bluetooth.h"
#include "rail.h"

void* BTLE_LL_GetRadioHandle(void);

uint32_t railScheduledEventCntr = 0;
uint32_t railUnscheduledEventCntr = 0;

// Maintain a copy of the bluetooth link layer priorities
// This can then be selectively changed using the CLI provided in dmp-tuning-cli.c
sl_bt_bluetooth_ll_priorities ll_priorities = SL_BT_BLUETOOTH_PRIORITIES_DEFAULT;

void sl_zigbee_af_ble_get_config_cb(void)
{
  // Edit the local copy to match plugin settings
  ll_priorities.rail_mapping_offset = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_OFFSET;
  ll_priorities.rail_mapping_range = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_RANGE;

  ll_priorities.scan_min = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MIN;
  ll_priorities.scan_max = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MAX;

  ll_priorities.adv_min = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MIN;
  ll_priorities.adv_max = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MAX;

  ll_priorities.init_min = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MIN;
  ll_priorities.init_max = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MAX;

  ll_priorities.conn_min = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MIN;
  ll_priorities.conn_max = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MAX;

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("BLE scheduler priorities initialized");
  }
}
void sl_zigbee_af_dmp_tuning_init_cb(uint8_t init_level)
{
  if ( init_level == SL_ZIGBEE_INIT_LEVEL_DONE ) {
    sl_zigbee_af_ble_get_config_cb();

    // Configure the zigbee TX and RX priorities
    sl_zigbee_multiprotocol_priorities_t pri;
    pri.backgroundRx = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_ZB_BACKGROUND_RX_PRIORIY;
    pri.activeRx = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_RX_PRIORITY;
    pri.tx = SL_ZIGBEE_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_TX_PRIORITY;

    if (sl_802154_radio_set_scheduler_priorities(&pri) == SL_STATUS_OK) {
      sl_zigbee_af_core_println("Zigbee TX/RX priorities initialized.");
    } else {
      sl_zigbee_af_core_println("Warning: zigbee TX/RX priorities cannot be initialized.");
    }
  }
}

void sli_802154mac_radio_config_scheduled_callback(bool scheduled)
{
  if (scheduled) {
    railScheduledEventCntr++;
  } else {
    railUnscheduledEventCntr++;
  }
}

void sl_zigbee_af_dmp_stop_radio(void)
{
  RAIL_Idle(sl_zigbee_get_rail_handle(), RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
  RAIL_Idle(BTLE_LL_GetRadioHandle(), RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
}
