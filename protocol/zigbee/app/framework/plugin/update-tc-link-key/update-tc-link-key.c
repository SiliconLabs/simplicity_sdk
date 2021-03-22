/***************************************************************************//**
 * @file
 * @brief Definitions for the Update TC Link Key plugin, which provides a way
 *        for joining devices to request a new link key after joining a Zigbee
 *        3.0 network.
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
#include "update-tc-link-key.h"

#include "update-tc-link-key-config.h"

#include "app/framework/plugin/network-steering/network-steering-internal.h"

static bool inRequest = false;

static sl_zigbee_af_event_t beginTcLinkKeyUpdateEvents[SL_ZIGBEE_SUPPORTED_NETWORKS];

// Setting the default timer to a day.
static uint32_t LinkKeyUpdateTimerMilliseconds = MILLISECOND_TICKS_PER_DAY;

// -----------------------------------------------------------------------------
// Public API

sl_status_t sl_zigbee_af_update_tc_link_key_start(void)
{
  sl_status_t status;

  status = sl_zigbee_update_tc_link_key(SL_ZIGBEE_AF_PLUGIN_UPDATE_TC_LINK_KEY_MAX_ATTEMPTS);
  if (status == SL_STATUS_OK) {
    inRequest = true;
  }

  return status;
}

bool sl_zigbee_af_update_tc_link_key_stop(void)
{
  bool wasInRequest = inRequest;
  inRequest = false;
  return wasInRequest;
}

void sl_zigbee_af_update_tc_link_key_set_delay(uint32_t delayMs)
{
  sl_zigbee_af_event_set_delay_ms(beginTcLinkKeyUpdateEvents, delayMs);
}

void sl_zigbee_af_update_tc_link_key_set_inactive(void)
{
  sl_zigbee_af_event_set_inactive(beginTcLinkKeyUpdateEvents);
}

void sl_zigbee_af_update_tc_link_key_zigbee_key_establishment_cb(sl_802154_long_addr_t partner,
                                                                 sl_zigbee_key_status_t status)
{
  if (inRequest) {
    sl_zigbee_af_core_print("%p:", SL_ZIGBEE_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME);

    if ((status == SL_ZIGBEE_TRUST_CENTER_LINK_KEY_ESTABLISHED)
        || (status == SL_ZIGBEE_VERIFY_LINK_KEY_SUCCESS)) {
      sl_zigbee_af_core_print(" New key established:");
    } else if (status != SL_ZIGBEE_APP_LINK_KEY_ESTABLISHED) {
      sl_zigbee_af_core_print(" Error:");
    }
    sl_zigbee_af_core_println(" 0x%X", status);
    sl_zigbee_af_core_print("Partner: ");
    sl_zigbee_af_core_print_buffer(partner, EUI64_SIZE, true); // withSpace?
    sl_zigbee_af_core_println("");

    // Anything greater than SL_ZIGBEE_TRUST_CENTER_LINK_KEY_ESTABLISHED is the
    // final state for a joining device
    if (status > SL_ZIGBEE_TRUST_CENTER_LINK_KEY_ESTABLISHED) {
      inRequest = false;
    }

    sl_zigbee_af_update_tc_link_key_status_cb(status);
  }
}

// =============================================================================
// Begin Update of TC Link Key

// The TC link key may be updated on a timely basis, if desired. For instance,
// if a device is joining a network where the Trust Center is pre-R21 and the
// device knows that the TC will be upgraded to post-R21 at some point, the
// joining device may choose to update its Trust Center link key on a timely
// basis such that, eventually, the update completes successfully.
// Other applications may choose to regularly update their TC link key for
// security reasons.

static void beginTcLinkKeyUpdateEventHandler(sl_zigbee_af_event_t * event)
{
  if (!inRequest) {
    sl_zigbee_af_event_set_inactive(beginTcLinkKeyUpdateEvents);

    sl_status_t status = sl_zigbee_af_update_tc_link_key_start();
    sl_zigbee_af_core_println("%p: %p: 0x%X",
                              SL_ZIGBEE_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME,
                              "Starting update trust center link key process",
                              status);
    if (status != SL_STATUS_OK) {
      // If Update TCLK failed and network-steering is in progress, leave network
      // Else, this might be one of the periodic link key updates that does not
      // require that the node leave the network. This will be retried in the
      // next attempt to update TCLK
      if ( sli_zigbee_af_network_steering_state_update_tclk() ) {
        sl_zigbee_leave_network(SL_ZIGBEE_LEAVE_NWK_WITH_NO_OPTION);
        sli_zigbee_af_network_steering_cleanup(status);
      }
    }
  }
  sl_zigbee_af_update_tc_link_key_set_delay(LinkKeyUpdateTimerMilliseconds);
}

void sl_zigbee_af_set_t_c_link_key_update_timer_milli_seconds(uint32_t timeInMilliseconds)
{
  LinkKeyUpdateTimerMilliseconds = timeInMilliseconds;
}

void sli_zigbee_af_update_tc_link_key_begin_tc_link_key_update_init(uint8_t init_level)
{
  (void)init_level;

  sl_zigbee_af_network_event_init(beginTcLinkKeyUpdateEvents,
                                  beginTcLinkKeyUpdateEventHandler);
}
