/***************************************************************************//**
 * @file high_datarate_phy_stack_interface_baremetal_wrapper.c
 * @brief internal implementations for 'high_datarate_phy_stack_interface' as a thin-wrapper
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
// automatically generated from high_datarate_phy_stack_interface.h.  Do not manually edit
#include "stack/include/high_datarate_phy_stack_interface.h"
#include "stack/internal/inc/high_datarate_phy_stack_interface_internal_def.h"

void sl_mac_lower_mac_set_high_datarate_csma_params(RAIL_CsmaConfig_t *csma_params)
{
  sli_mac_stack_lower_mac_set_high_datarate_csma_params(csma_params);
}

void sl_mac_lower_mac_set_high_datarate_phy_radio_priorities(sl_zigbee_multiprotocol_priorities_t *priorities)
{
  sli_mac_stack_lower_mac_set_high_datarate_phy_radio_priorities(priorities);
}

sl_status_t sl_mac_send_raw_high_datarate_phy_message(uint8_t nwk_index,
                                                      uint8_t *payload)
{
  return sli_mac_stack_send_raw_high_datarate_phy_message(nwk_index,
                                                          payload);
}

RAIL_Status_t sl_mac_set_mode_switch_sync_detect(bool enable_f)
{
  return sli_mac_stack_set_mode_switch_sync_detect(enable_f);
}
