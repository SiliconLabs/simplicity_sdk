/*
 *  Copyright (c) 2023, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for radio communication.
 *
 */

#include <openthread-core-config.h>

#include "pa_conversions_efr32.h"
#include "platform-band.h"
#include "platform-efr32.h"
#include "radio_multi_channel.h"
#include "radio_power_manager.h"
#include "rail_config.h"
#include "rail_ieee802154.h"
#include "sl_multipan.h"

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
#include "sl_rail_mux_rename.h"
#endif //SL_CATALOG_RAIL_MULTIPLEXER_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_FAST_CHANNEL_SWITCHING_PRESENT
#include "sl_rail_util_ieee802154_fast_channel_switching_config.h"
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_FAST_CHANNEL_SWITCHING_PRESENT

#if !OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE

static int8_t sli_max_channel_power[RADIO_INTERFACE_COUNT][SL_MAX_CHANNELS_SUPPORTED];
static int8_t sli_default_tx_power[RADIO_INTERFACE_COUNT];

/**
 * This function gets the lowest value for the max_tx_power for a channel, from the max_tx_powerTable set
 * across all interfaces. It also gets the highest default_tx_power set across all interfaces.
 * 
 * @param[out]  default_tx_power    A pointer to update the derived default_tx_power across all IIDs.
 * @param[out]  tx_power_from_table A pointer to update the Tx Power derived from the MaxChannelPowerTable.
 * @param[in]   channel             Channel of interest 
 *
 */
static void sli_get_default_and_max_powers_across_iids(int8_t  *default_tx_power,
                                                       int8_t  *tx_power_from_table,
                                                       uint16_t channel)
{
    OT_ASSERT(tx_power_from_table != NULL);
    OT_ASSERT(default_tx_power != NULL);

    for (uint8_t iid = 0U; iid < RADIO_INTERFACE_COUNT; iid++)
    {
        // Obtain the minimum Tx power set by different iids, for `channel`
        // If there is an interface using lower Tx power than the one we have
        // in tx_power_from_table..
        // Update tx_power_from_table.
        *tx_power_from_table = SL_MIN(*tx_power_from_table, sli_max_channel_power[iid][channel - SL_CHANNEL_MIN]);

        // If the default Tx Power set is not invalid..
        if (sli_default_tx_power[iid] != SL_INVALID_TX_POWER)
        {
            // Obtain the Max value between local default_tx_power and sli_default_tx_power.
            // If selected default Tx Power is Invalid, initialise it to sli_default_tx_power.
            // We have already validated that sli_default_tx_power holds a valid value.
            *default_tx_power = (*default_tx_power == SL_INVALID_TX_POWER) ? sli_default_tx_power[iid]
                                                                       : SL_MAX(*default_tx_power, sli_default_tx_power[iid]);
        }
    }
}

/**
 * This function returns the tx power to be used based on the default and max tx power table, for a given channel.
 * 
 * @param[in]   channel   Channel of interest 
 * 
 * @returns The radio Tx Power for the given channel, in dBm.
 *
 */

static int8_t sli_get_max_tx_power_across_iids(uint16_t channel)
{
    int8_t   max_channel_tx_power = SL_INVALID_TX_POWER;
    int8_t   max_default_tx_power = SL_INVALID_TX_POWER;
    int8_t   selected_tx_power    = SL_INVALID_TX_POWER;

#if SL_RAIL_UTIL_IEEE802154_FAST_CHANNEL_SWITCHING_ENABLED && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

    if (sl_is_multi_channel_enabled())
    {
        RAIL_IEEE802154_RxChannelSwitchingCfg_t channel_switching_cfg;

        // Get switching config
        sl_get_channel_switching_cfg(&channel_switching_cfg);

        // Find the max_channel_tx_power, to be minimum of Max channel power for the
        // channels infast channel config, accross all iids. This is because, if a iid_1
        // sets the max tx power of the channel to be less than the max tx power set by
        // iid_2, we will need to work with the lower tx power to be compliant on both
        // interfaces.

        // Find the max_default_tx_power, to be maximum of the default Tx power accross all
        // the interfaces.

        for (uint8_t i = 0U; i < RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS; i++)
        {
            channel = channel_switching_cfg.channels[i];
            sli_get_default_and_max_powers_across_iids(&max_default_tx_power, &max_channel_tx_power, channel);
        }
    }
    else
#endif
    {
        sli_get_default_and_max_powers_across_iids(&max_default_tx_power, &max_channel_tx_power, channel);
    }

    // Return the minimum of max_channel_tx_power and max_default_tx_power.
    selected_tx_power = SL_MIN(max_channel_tx_power, max_default_tx_power);
    return (selected_tx_power == SL_INVALID_TX_POWER) ? OPENTHREAD_CONFIG_DEFAULT_TRANSMIT_POWER : selected_tx_power;
}

#endif //!OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE

void sli_set_tx_power_in_rail(int8_t power_in_dbm)
{
    RAIL_Status_t status;

    // RAIL_SetTxPowerDbm() takes power in units of deci-dBm (0.1dBm)
    // Multiply by 10 because power_in_dbm is supposed be in units dBm
    status = RAIL_SetTxPowerDbm(gRailHandle, ((RAIL_TxPower_t)power_in_dbm) * 10);
    
    OT_ASSERT(status == RAIL_STATUS_NO_ERROR);
}

void sli_init_power_manager(void)
{
#if !OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
    memset(sli_max_channel_power, SL_INVALID_TX_POWER, sizeof(sli_max_channel_power));
    memset(sli_default_tx_power, SL_INVALID_TX_POWER, sizeof(sli_default_tx_power));
#endif //!OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
}

void sli_update_tx_power_after_config_update(const RAIL_TxPowerConfig_t *tx_pwr_config, int8_t tx_power)
{
    RAIL_Status_t       status;
    RAIL_TxPowerLevel_t tx_power_lvl;
    RAIL_TxPower_t      tx_power_dbm = tx_power * 10;

    tx_power_lvl = RAIL_GetTxPower(gRailHandle);

    // Always need to call RAIL_SetTxPowerDbm after RAIL_ConfigTxPower
    // First need to get existing power setting and reassert value after config

    if (tx_power_lvl != RAIL_TX_POWER_LEVEL_INVALID)
    {
        tx_power_dbm = RAIL_GetTxPowerDbm(gRailHandle);
    }

    status = RAIL_ConfigTxPower(gRailHandle, tx_pwr_config);
    OT_ASSERT(status == RAIL_STATUS_NO_ERROR);

    status = RAIL_SetTxPowerDbm(gRailHandle, tx_power_dbm);
    OT_ASSERT(status == RAIL_STATUS_NO_ERROR);
}

otError sli_set_channel_max_tx_power(otInstance *instance, uint8_t channel, int8_t max_power)
{
    otError error  = OT_ERROR_NONE;

#if !OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
    int8_t tx_power;
    uint8_t iid = efr32GetIidFromInstance(instance);

    otEXPECT_ACTION(channel >= SL_CHANNEL_MIN && channel <= SL_CHANNEL_MAX, error = OT_ERROR_INVALID_ARGS);

    sli_max_channel_power[iid][channel - SL_CHANNEL_MIN] = max_power;
    tx_power = sl_get_tx_power_for_current_channel(instance);
    sli_set_tx_power_in_rail(tx_power);

exit:
#else
    OT_UNUSED_VARIABLE(instance);
    OT_UNUSED_VARIABLE(channel);
    OT_UNUSED_VARIABLE(max_power);
    error = OT_ERROR_NOT_IMPLEMENTED;
#endif
    return error;
}

otError sli_set_default_tx_power(otInstance *instance, int8_t tx_power)
{
    otError error   = OT_ERROR_NONE;

#if !OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
    int8_t          max_tx_power;
    uint8_t         iid         = efr32GetIidFromInstance(instance);

    sli_default_tx_power[iid] = tx_power;
    max_tx_power              = sl_get_tx_power_for_current_channel(instance);

    sli_set_tx_power_in_rail(max_tx_power);
#else
    OT_UNUSED_VARIABLE(instance);
    OT_UNUSED_VARIABLE(tx_power);
    error = OT_ERROR_NOT_IMPLEMENTED;
#endif

    return error;
}

int8_t sl_get_tx_power_for_current_channel(otInstance *instance)
{
    int8_t tx_power;
    uint16_t channel;

    RAIL_GetChannel(gRailHandle, &channel);

#if OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
    uint8_t     raw_power_calibration[SL_OPENTHREAD_RAW_POWER_CALIBRATION_LENGTH];
    uint8_t     fem_setting[SL_OPENTHREAD_FEM_SETTING_LENGTH];
    uint16_t    raw_calibration_length = SL_OPENTHREAD_RAW_POWER_CALIBRATION_LENGTH;
    uint16_t    fem_setting_length = SL_OPENTHREAD_FEM_SETTING_LENGTH;
    otError     error;

    error = otPlatRadioGetRawPowerSetting(  instance, channel,
                                            raw_power_calibration,
                                            &raw_calibration_length);

    error = sl_parse_raw_power_calibration_cb(raw_power_calibration, raw_calibration_length, &tx_power, fem_setting, &fem_setting_length);
    OT_ASSERT(error == OT_ERROR_NONE);

    sl_configure_fem_cb(fem_setting, fem_setting_length);

#else
    OT_UNUSED_VARIABLE(instance); 
    tx_power = sli_get_max_tx_power_across_iids(channel);  
#endif   

    return tx_power;
}

#if OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE
SL_WEAK otError sl_parse_raw_power_calibration_cb(  uint8_t   *raw_power_calibration, 
                                                    uint16_t  raw_setting_length, 
                                                    int8_t    *radio_power,
                                                    uint8_t   *fem_setting,
                                                    uint16_t  *fem_setting_length)
{
    OT_ASSERT(raw_power_calibration != NULL);
    OT_ASSERT(radio_power != NULL);
    OT_UNUSED_VARIABLE(raw_setting_length);
    OT_UNUSED_VARIABLE(fem_setting);
    OT_UNUSED_VARIABLE(fem_setting_length);

    *radio_power = raw_power_calibration[0];
    return OT_ERROR_NONE;
}

SL_WEAK void sl_configure_fem_cb(uint8_t *fem_setting, uint16_t fem_setting_length)
{
    OT_UNUSED_VARIABLE(fem_setting);
    OT_UNUSED_VARIABLE(fem_setting_length);
}
#endif // OPENTHREAD_CONFIG_POWER_CALIBRATION_ENABLE