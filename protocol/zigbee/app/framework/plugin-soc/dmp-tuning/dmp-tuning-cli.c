/***************************************************************************//**
 * @file
 * @brief CLI for the DMO Tuning and Testing plugin.
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
#include "app/framework/util/af-main.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "sl_bt_ll_config.h"
#include "lower-mac.h"
#include "dmp-tuning.h"
#include "dmp-tuning-profiles.h"
#include "sl_bluetooth.h"

void sli_802154mac_radio_enable_config_scheduled_callback(bool enable);
bool sli_802154mac_radio_config_scheduled_callback_enabled(void);
void sli_802154mac_radio_config_scheduled_callback(bool scheduled);
void sl_zigbee_af_dmp_stop_radio(void);

// Zigbee related CLI commands
// Zigbee scheduler priorities
void sl_zigbee_af_dmp_tuning_set_zigbee_scheduler_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_multiprotocol_priorities_t pri;
  pri.backgroundRx = sl_cli_get_argument_uint8(arguments, 0);
  pri.activeRx = sl_cli_get_argument_uint8(arguments, 1);
  pri.tx = sl_cli_get_argument_uint8(arguments, 2);
  if (sl_802154_radio_set_scheduler_priorities(&pri) == SL_STATUS_OK) {
    sli_mac_lower_mac_radio_sleep();
    sli_mac_lower_mac_radio_wakeup();
    sl_zigbee_af_core_println("DmpTuning - SetZigbeeSchedulerPrioritiesCommand: success");
  } else {
    sl_zigbee_af_core_println("invalid call");
  }
}

void sl_zigbee_af_dmp_tuning_get_zigbee_scheduler_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_multiprotocol_priorities_t pri;
  sl_802154_radio_get_scheduler_priorities(&pri);
  sl_zigbee_af_core_println("DmpTuning - GetZigbeeSchedulerPrioritiesCommand: backgroundRx:%d activeRx:%d tx:%d",
                            pri.backgroundRx,
                            pri.activeRx,
                            pri.tx);
}

// Zigbee Slip Time
void sl_zigbee_af_dmp_tuning_set_zigbee_slip_time_command(sl_cli_command_arg_t *arguments)
{
  sl_status_t status = sl_802154_radio_set_scheduler_sliptime((uint32_t)sl_zigbee_unsigned_command_argument(0));
  if (status == SL_STATUS_OK) {
    sl_zigbee_af_core_println("DmpTuning - SetZigbeeSlipTime: success");
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetZigbeeSlipTime should only be used in multiprotocol");
  }
}

void sl_zigbee_af_dmp_tuning_get_zigbee_slip_time_command(sl_cli_command_arg_t *arguments)
{
  uint32_t slipTime;
  sl_status_t status = sl_802154_radio_get_scheduler_sliptime(&slipTime);
  if (status == SL_STATUS_OK) {
    sl_zigbee_af_core_println("DmpTuning - GetZigbeeSlipTime: slip_time:%d", slipTime);
  } else {
    sl_zigbee_af_core_println("DmpTuning - GetZigbeeSlipTime should only be used in multiprotocol");
  }
}

// BLE related CLI commands
// BLE Scan Priorities
void sl_zigbee_af_dmp_tuning_set_ble_scan_priorities_command(sl_cli_command_arg_t *arguments)
{
  ll_priorities.scan_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.scan_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetBleScanPriorities: success");
  }
}

void sl_zigbee_af_dmp_tuning_get_ble_scan_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("DmpTuning - GetBleScanPriorities: scan_min:%d scan_max:%d",
                            ll_priorities.scan_min,
                            ll_priorities.scan_max);
}

// BLE Advertisement Priorities
void sl_zigbee_af_dmp_tuning_set_ble_advertisement_priorities_command(sl_cli_command_arg_t *arguments)
{
  ll_priorities.adv_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.adv_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetBleAdvertisementPriorities: success");
  }
}

void sl_zigbee_af_dmp_tuning_get_ble_advertisement_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("DmpTuning - GetBleAdvertisementPriorities: adv_min:%d adv_max:%d",
                            ll_priorities.adv_min,
                            ll_priorities.adv_max);
}

// BLE Connection Init Priorities
void sl_zigbee_af_dmp_tuning_set_ble_connection_init_priorities_command(sl_cli_command_arg_t *arguments)
{
  ll_priorities.init_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.init_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetBleConnectionInitPriorities: success");
  }
}

void sl_zigbee_af_dmp_tuning_get_ble_connection_init_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("DmpTuning - GetBleConnectionInitPriorities: init_min:%d init_max:%d",
                            ll_priorities.init_min,
                            ll_priorities.init_max);
}

// BLE Connection Priorities
void sl_zigbee_af_dmp_tuning_set_ble_connection_priorities_command(sl_cli_command_arg_t *arguments)
{
  ll_priorities.conn_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.conn_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetBleConnectionPriorities: success");
  }
}

void sl_zigbee_af_dmp_tuning_get_ble_connection_priorities_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("DmpTuning - GetBleConnectionPriorities: conn_min:%d conn_max:%d",
                            ll_priorities.conn_min,
                            ll_priorities.conn_max);
}

// BLE RAIL Mapping
void sl_zigbee_af_dmp_tuning_set_ble_rail_mapping_command(sl_cli_command_arg_t *arguments)
{
  ll_priorities.rail_mapping_offset = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.rail_mapping_range = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    sl_zigbee_af_core_println("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    sl_zigbee_af_core_println("DmpTuning - SetBleRailMapping: success");
  }
}

void sl_zigbee_af_dmp_tuning_get_ble_rail_mapping_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("DmpTuning - GetBleRailMapping: rail_mapping_offset:%d rail_mapping_range:%d",
                            ll_priorities.rail_mapping_offset,
                            ll_priorities.rail_mapping_range);
}

void sl_zigbee_af_dmp_tuning_stop_radio_command(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_dmp_stop_radio();
  sl_zigbee_af_core_println("Radio has been stopped");
}

// CLI for protocol switch related RAIL events.
void sl_zigbee_af_dmp_tuning_get_rail_scheduled_event_counters(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("Scheduled event counter:%d Unscheduled event counter:%d",
                            railScheduledEventCntr,
                            railUnscheduledEventCntr);
}

void sl_zigbee_af_dmp_tuning_enable_rail_config_scheduler_events(sl_cli_command_arg_t *arguments)
{
  sli_802154mac_radio_enable_config_scheduled_callback(true);
  sl_zigbee_af_core_println("DmpTuning - EnableRailConfigSchedulerEvents: success");
}

void sl_zigbee_af_dmp_tuning_disable_rail_config_scheduler_events(sl_cli_command_arg_t *arguments)
{
  sli_802154mac_radio_enable_config_scheduled_callback(false);
  sl_zigbee_af_core_println("DmpTuning - DisableRailConfigSchedulerEvents: success");
}

void sl_zigbee_af_dmp_tuning_get_rail_config_scheduler_events_enabled(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_af_core_println("RAIL config scheduler event enabled: %d",
                            sli_802154mac_radio_config_scheduled_callback_enabled());
}
