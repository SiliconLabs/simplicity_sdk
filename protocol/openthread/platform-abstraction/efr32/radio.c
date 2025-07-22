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

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread-system.h>
#include <openthread/link.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
#include <openthread/platform/multipan.h>
#endif

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/logging.hpp"
#include "utils/code_utils.h"
#include "utils/link_metrics.h"
#include "utils/mac_frame.h"

#include "circular_queue.h"
#include "em_device.h"
#include "sl_core.h"
#if defined _SILICON_LABS_32B_SERIES_2
#include "em_system.h"
#else
#include "sl_hal_system.h"
#endif
#include "ieee802154mac.h"
#include "platform-band.h"
#include "platform-efr32.h"
#include "radio_coex.h"
#include "radio_multi_channel.h"
#include "radio_power_manager.h"
#include "rail_config.h"
#include "sl_memory_manager.h"
#include "sl_multipan.h"
#include "sl_packet_utils.h"
#include "sl_rail.h"
#include "sl_rail_ieee802154.h"
#include "soft_source_match_table.h"

#include "pa_conversions_efr32.h"
#include "sl_openthread_radio_config.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
#include "sl_rail_mux_rename.h"
#endif

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
#include "sl_rail_util_ant_div.h"
#include "sl_rail_util_ant_div_config.h"
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
#include "coexistence-802154.h"
#include "coexistence-ot.h"
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
#include "sl_rail_util_ieee802154_stack_event.h"
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
#include "sl_rail_util_ieee802154_phy_select.h"
#endif // #ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT

#include "sl_gp_interface.h"

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_FAST_CHANNEL_SWITCHING_PRESENT
#include "sl_rail_util_ieee802154_fast_channel_switching_config.h"
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_FAST_CHANNEL_SWITCHING_PRESENT

//------------------------------------------------------------------------------
// Enums, macros and static variables

#ifndef LOW_BYTE
#define LOW_BYTE(n) ((uint8_t)((n)&0xFF))
#endif // LOW_BTE

#ifndef HIGH_BYTE
#define HIGH_BYTE(n) ((uint8_t)(LOW_BYTE((n) >> 8)))
#endif // HIGH_BYTE

#ifndef BIT32
#define BIT32(x) (((uint32_t)1) << (x))
#endif // BIT32

// Intentionally maintaining separate groups for the different device series.
// This gives flexibility to add new elements to be read, like CCA Thresholds.
#if defined(_SILICON_LABS_32B_SERIES_2)
#define USERDATA_MFG_CUSTOM_EUI_64 (2)
#endif

#ifndef USERDATA_END
#define USERDATA_END (USERDATA_BASE + FLASH_PAGE_SIZE)
#endif

#if SL_OPENTHREAD_RADIO_RX_BUFFER_COUNT > CIRCULAR_QUEUE_LEN_MAX
#error "Rx buffer count cannot be greater than max circular queue length."
#endif

// Internal flags
#define FLAG_RADIO_INIT_DONE 0x00000001
#define FLAG_ONGOING_TX_DATA 0x00000002
#define FLAG_ONGOING_TX_ACK 0x00000004
#define FLAG_WAITING_FOR_ACK 0x00000008
#define FLAG_CURRENT_TX_USE_CSMA 0x00000010
#define FLAG_SCHEDULED_RX_PENDING 0x00000020
#define FLAG_SCHEDULED_TX_PENDING 0x00000040

// Radio Events
#define EVENT_TX_SUCCESS 0x00000100
#define EVENT_TX_CCA_FAILED 0x00000200
#define EVENT_TX_NO_ACK 0x00000400
#define EVENT_TX_SCHEDULER_ERROR 0x00000800
#define EVENT_TX_FAILED 0x00001000
#define EVENT_ACK_SENT_WITH_FP_SET 0x00002000
#define EVENT_SECURED_ACK_SENT 0x00004000
#define EVENT_SCHEDULED_RX_STARTED 0x00008000
#define EVENT_SCHEDULED_TX_STARTED 0x00010000

#define TX_WAITING_FOR_ACK 0x00
#define TX_NO_ACK 0x01

#define RADIO_TX_EVENTS \
    (EVENT_TX_SUCCESS | EVENT_TX_CCA_FAILED | EVENT_TX_NO_ACK | EVENT_TX_SCHEDULER_ERROR | EVENT_TX_FAILED)

#define QUARTER_DBM_IN_DBM 4
#define US_IN_MS 1000

/* FilterMask provided by RAIL is structured as follows:
 * |  Bit:7  |  Bit:6  |  Bit:5  |    Bit:4    |  Bit:3  |  Bit:2  |  Bit:1  |    Bit:0     |
 * |  Addr2  |  Addr1  |  Addr0  |  Bcast Addr | Pan Id2 | Pan Id1 | Pan Id0 | Bcast PanId  |
 * | Matched | Matched | Matched |   Matched   | Matched | Matched | Matched |   Matched    |
 */

#define RADIO_BCAST_IID (0)
#define RADIO_GET_FILTER_MASK(iid) (1 << (iid))
#define RADIO_PANID_FILTER_SHIFT (0)
#define RADIO_ADDR_FILTER_SHIFT (4)

#define RADIO_BCAST_PANID_FILTER_MASK RADIO_GET_FILTER_MASK(0)
#define RADIO_INDEX0_PANID_FILTER_MASK RADIO_GET_FILTER_MASK(1)
#define RADIO_INDEX1_PANID_FILTER_MASK RADIO_GET_FILTER_MASK(2)
#define RADIO_INDEX2_PANID_FILTER_MASK RADIO_GET_FILTER_MASK(3)

#define RADIO_GET_PANID_FILTER_MASK(filter)                                                             \
    (filter                                                                                             \
     & (RADIO_BCAST_PANID_FILTER_MASK | RADIO_INDEX0_PANID_FILTER_MASK | RADIO_INDEX1_PANID_FILTER_MASK \
        | RADIO_INDEX2_PANID_FILTER_MASK))

#define RADIO_BCAST_ADDR_FILTER_MASK (RADIO_GET_FILTER_MASK(0) << RADIO_ADDR_FILTER_SHIFT)
#define RADIO_INDEX0_ADDR_FILTER_MASK (RADIO_GET_FILTER_MASK(1) << RADIO_ADDR_FILTER_SHIFT)
#define RADIO_INDEX1_ADDR_FILTER_MASK (RADIO_GET_FILTER_MASK(2) << RADIO_ADDR_FILTER_SHIFT)
#define RADIO_INDEX2_ADDR_FILTER_MASK (RADIO_GET_FILTER_MASK(3) << RADIO_ADDR_FILTER_SHIFT)

#define RADIO_GET_ADDR_FILTER_MASK(filter)                                                           \
    (filter                                                                                          \
     & (RADIO_BCAST_ADDR_FILTER_MASK | RADIO_INDEX0_ADDR_FILTER_MASK | RADIO_INDEX1_ADDR_FILTER_MASK \
        | RADIO_INDEX2_ADDR_FILTER_MASK))

#define RADIO_BCAST_PANID (0xFFFF)

#if defined(_SILICON_LABS_32B_SERIES_2)
#define DEVICE_CAPABILITY_MCU_EN (DEVINFO->SWCAPA1 & _DEVINFO_SWCAPA1_RFMCUEN_MASK)
#else
#define DEVICE_CAPABILITY_MCU_EN (DEVINFO->SWCAPA & _DEVINFO_SWCAPA_RFMCUEN_MASK)
#endif

#define ENERGY_READS_MAX -128
#define SYMBOLS_PER_ENERGY_READING 8 // we take a reading every 8 symbols

static otRadioCaps sRadioCapabilities =
    (OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_ENERGY_SCAN | OT_RADIO_CAPS_SLEEP_TO_TX
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
     | OT_RADIO_CAPS_TRANSMIT_SEC
     // When scheduled tx is required, we support sl_rail_start_scheduled_cca_csma_tx
     // (delay is indicated in tx frame info set in MAC)
     | OT_RADIO_CAPS_TRANSMIT_TIMING
     // When scheduled rx is required, we support sl_rail_start_scheduled_rx in our
     // implementation of otPlatRadioReceiveAt
     | OT_RADIO_CAPS_RECEIVE_TIMING
#endif
    );
static uint32_t              sScanFrameCounter    = 0; // How many 'frames' left in the current scan.
static uint32_t              sScanFrameCounterMax = 0; // Max 'frames' when performing scan
static int8_t                sEnergyReadsMax      = ENERGY_READS_MAX;
static sl_rail_multi_timer_t rail_timer;

// Energy Scan
typedef enum
{
    ENERGY_SCAN_STATUS_IDLE,
    ENERGY_SCAN_STATUS_IN_PROGRESS,
    ENERGY_SCAN_STATUS_COMPLETED
} energyScanStatus;

typedef enum
{
    ENERGY_SCAN_MODE_SYNC,
    ENERGY_SCAN_MODE_ASYNC
} energyScanMode;

typedef struct
{
    uint8_t        length;
    uint8_t        channel;
    uint8_t        lqi;
    int8_t         rssi;
    uint8_t        iid;
    sl_rail_time_t timestamp;
} rxPacketDetails;

typedef struct
{
    rxPacketDetails packetInfo;
    uint8_t         psdu[IEEE802154_MAX_LENGTH];
} rxBuffer;

typedef uint8_t rxBufferIndex_t;

static volatile energyScanStatus sEnergyScanStatus;
static volatile int8_t           sEnergyScanResultDbm;
static energyScanMode            sEnergyScanMode;
// To track active interface the energy scan is being performed.
static uint8_t sEnergyScanActiveInterface = INVALID_INTERFACE_INDEX;

static bool sIsSrcMatchEnabled = false;

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
#define RADIO_EXT_ADDR_COUNT (RADIO_INTERFACE_COUNT - 1)
// To hold transmit/energy-scan requests from the hosts i.e. one per instance/host,
// if radio is busy.
#define RADIO_REQUEST_BUFFER_COUNT OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_NUM
#else
#define RADIO_EXT_ADDR_COUNT (RADIO_INTERFACE_COUNT)
#define RADIO_REQUEST_BUFFER_COUNT 1
#endif

typedef struct
{
    otRadioFrame frame;
    uint8_t      iid;
    uint8_t      currentRadioTxPriority;
} radioFrame;

// Receive
static Queue_t          sRxPacketQueue;
static sl_memory_pool_t sRxPacketMemPoolHandle = {0};
static uint8_t          sReceiveAckPsdu[IEEE802154_MAX_LENGTH];
static radioFrame       sReceive;
static radioFrame       sReceiveAck;
static otError          sReceiveError;

// Transmit
// One of the IID is reserved for broadcast hence we need RADIO_INTERFACE_COUNT - 1.
// IID zero is for broadcast, so request from host1 (i.e. iid = 1) will use tx buffer
// index 0 (i.e. iid - 1) and so on.
static radioFrame   sTransmitBuffer[RADIO_REQUEST_BUFFER_COUNT];
static uint8_t      sTransmitPsdu[RADIO_REQUEST_BUFFER_COUNT][IEEE802154_MAX_LENGTH];
static radioFrame  *sCurrentTxPacket = NULL;
static uint8_t      sLastLqi         = 0;
static int8_t       sLastRssi        = 0;
static otExtAddress sExtAddress[RADIO_EXT_ADDR_COUNT];

// CSMA config: Should be globally scoped
#define CSL_CSMA_BACKOFF_TIME_IN_US 150
sl_rail_csma_config_t csmaConfig    = SL_RAIL_CSMA_CONFIG_802_15_4_2003_2P4_GHZ_OQPSK_CSMA;
sl_rail_csma_config_t cslCsmaConfig = SL_RAIL_CSMA_CONFIG_SINGLE_CCA;

#define SCHEDULE_TX_DELAY_US 3000 // Chosen after internal certification testing

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
static otRadioIeInfo sTransmitIeInfo[RADIO_REQUEST_BUFFER_COUNT];
#endif

// Radio
#define CCA_THRESHOLD_UNINIT 127
#define CCA_THRESHOLD_DEFAULT -75 // dBm  - default for 2.4GHz 802.15.4

#define UNINITIALIZED_CHANNEL 0xFF

static bool              sPromiscuous = false;
static efr32CommonConfig sCommonConfig;
static efr32BandConfig   sBandConfig;
static efr32BandConfig  *sCurrentBandConfig = NULL;

static int8_t sCcaThresholdDbm = CCA_THRESHOLD_DEFAULT;

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
efr32RadioCounters railDebugCounters;

// Create an alias for readability of RX debug tracking
#define rxDebugStep (railDebugCounters.mRadioDebugData.m8[RX_DEBUG_COUNTER0])
#endif

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
// Type of commands that can be added to the pending command buffer.
typedef enum
{
    kPendingCommandTypeTransmit,
    kPendingCommandTypeEnergyScan,
} pendingCommandType;

typedef struct
{
    // Energy scan channel.
    uint8_t scanChannel;
    // Energy scan duration.
    uint16_t scanDuration;
} energyScanParams;

// The structure representing pending transmit and energy-scan command requests.
typedef struct
{
    // The union of transmit and energy-scan requests parameters.
    union
    {
        // A pointer to the transmit radio frame.
        otRadioFrame *txFrame;
        // The structure of energy-scan request parameters.
        energyScanParams energyScan;
    } request;
    // The pending command type.
    pendingCommandType cmdType : 2;
    // The interface iid of the pending command.
    uint8_t iid : 2;
} pendingCommandEntry;

static Queue_t sPendingCommandQueue;

extern otInstance *sInstances[OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_NUM];
static uint8_t     sRailFilterMask = RADIO_BCAST_PANID_FILTER_MASK;
static bool        isRadioTransmittingOrScanning(void);

#if FAST_CHANNEL_SWITCHING_SUPPORT
static sl_rail_ieee802154_rx_channel_switching_cfg_t sChannelSwitchingCfg;
static SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_BUF_ALIGNMENT_TYPE
    sChannelSwitchingBuffer[SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_BUF_BYTES
                            / SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_BUF_ALIGNMENT];

bool sl_is_multi_channel_enabled(void)
{
    uint8_t firstChannel = UNINITIALIZED_CHANNEL;
    for (uint8_t i = 0U; i < SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS; i++)
    {
        if (sChannelSwitchingCfg.channels[i] != UNINITIALIZED_CHANNEL)
        {
            if (firstChannel == UNINITIALIZED_CHANNEL)
            {
                firstChannel = sChannelSwitchingCfg.channels[i];
            }
            else if (firstChannel != sChannelSwitchingCfg.channels[i])
            {
                return true;
            }
        }
    }
    return false;
}

otError sl_get_channel_switching_cfg(sl_rail_ieee802154_rx_channel_switching_cfg_t *channelSwitchingCfg)
{
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(channelSwitchingCfg != NULL, error = OT_ERROR_INVALID_ARGS);

    memcpy(channelSwitchingCfg, &sChannelSwitchingCfg, sizeof(sChannelSwitchingCfg));

exit:
    return error;
}

static uint8_t fastChannelIndex(uint8_t aChannel)
{
    for (uint8_t i = 0U; i < SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS; i++)
    {
        if (sChannelSwitchingCfg.channels[i] == aChannel)
        {
            return i;
        }
    }
    return INVALID_INTERFACE_INDEX;
}

#endif // FAST_CHANNEL_SWITCHING_SUPPORT
#endif // OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

static void efr32ScanTimerHandler(struct sl_rail_multi_timer *tmr, sl_rail_time_t expectedTimeOfEvent, void *cbArg);
static sl_rail_status_t efr32ScanDwell(uint16_t symbols);

// RAIL

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
sl_rail_handle_t gRailHandle;
#if SL_OPENTHREAD_COEX_COUNTER_ENABLE
static void sl_ot_coex_counter_on_event(sl_rail_util_coex_event_t event);
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE
#else
sl_rail_handle_t emPhyRailHandle;
#endif // SL_CATALOG_RAIL_MULTIPLEXER_PRESENT

static const sl_rail_ieee802154_config_t sRailIeee802154Config = {
    .p_addresses = NULL,
    .ack_config =
        {
            .enable         = true,
            .ack_timeout_us = 672,
            .rx_transitions =
                {
                    .success = SL_RAIL_RF_STATE_RX,
                    .error   = SL_RAIL_RF_STATE_RX,
                },
            .tx_transitions =
                {
                    .success = SL_RAIL_RF_STATE_RX,
                    .error   = SL_RAIL_RF_STATE_RX,
                },
        },
    .timings =
        {
            .idle_to_rx = 100,
            .tx_to_rx   = 192 - 10,
            .idle_to_tx = 100,
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
            .rx_to_tx = 256, // accommodate enhanced ACKs
#else
            .rx_to_tx = 192,
#endif
            .rxsearch_timeout       = 0,
            .tx_to_rxsearch_timeout = 0,
            .tx_to_tx               = 0,
        },
    .frames_mask                            = SL_RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES,
    .promiscuous_mode                       = false,
    .is_pan_coordinator                     = false,
    .default_frame_pending_in_outgoing_acks = false,
};

#if RADIO_CONFIG_SUBGHZ_SUPPORT
#define PHY_HEADER_SIZE 2
// SHR: 4 bytes preamble, 2 bytes SFD
// 802.15.4 spec describes GFSK SHR to be the same format as SUN O-QPSK
// except preamble is 32 symbols (4 octets).
#define SHR_SIZE 6
#else
#define PHY_HEADER_SIZE 1
#define SHR_SIZE 5 // 4 bytes of preamble, 1 byte sync-word
#endif

#define SHR_DURATION_US 160 // Duration of SHR in us.

// Misc
static volatile uint32_t miscRadioState = 0;
static bool              emPendingData  = false;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
enum
{
    RHO_INACTIVE = 0,
    RHO_EXT_ACTIVE,
    RHO_INT_ACTIVE, // Not used
    RHO_BOTH_ACTIVE,
};

static uint8_t rhoActive = RHO_INACTIVE;
static bool    ptaGntEventReported;
static bool    sRadioCoexEnabled = true;

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE
uint32_t efr32RadioCoexCounters[SL_RAIL_UTIL_COEX_EVENT_COUNT] = {0};
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE

#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

// Note: This callback can be called from ISR context.
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool rxPacketQueueOverflowCallback(const Queue_t *queue, void *data)
{
    OT_UNUSED_VARIABLE(queue);
    OT_UNUSED_VARIABLE(data);

    // True to discard the queue item being considered for removal.
    // False for nothing to be discarded from the queue.
    // Do not discard the oldest entry from the queue, rather drop
    // the new received packet, hence, return false.
    return false;
}

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
static bool pendingCommandQueueOverflowCallback(const Queue_t *queue, void *data)
{
    OT_UNUSED_VARIABLE(queue);
    OT_UNUSED_VARIABLE(data);

    // We should never hit this callback because a host can only request
    // one command at a time. Hence, added a assert if it happens.
    OT_ASSERT(false);

    return false;
}
#endif

#if RADIO_CONFIG_ENABLE_CUSTOM_EUI_SUPPORT && defined(_SILICON_LABS_32B_SERIES_2)
/*
 * This API reads the UserData page on the given EFR device.
 */
static int readUserData(void *buffer, uint16_t index, int len, bool changeByteOrder)
{
    uint8_t *readLocation  = (uint8_t *)USERDATA_BASE + index;
    uint8_t *writeLocation = (uint8_t *)buffer;

    // Sanity check to verify if the ouput buffer is valid and the index and len are valid.
    // If invalid, change the len to -1 and return.
    otEXPECT_ACTION((writeLocation != NULL) && ((readLocation + len) <= (uint8_t *)USERDATA_END), len = -1);

    // Copy the contents of flash into output buffer.

    for (int idx = 0; idx < len; idx++)
    {
        if (changeByteOrder)
        {
            writeLocation[idx] = readLocation[(len - 1) - idx];
        }
        else
        {
            writeLocation[idx] = readLocation[idx];
        }
    }

exit:
    // Return len, len was changed to -1 to indicate failure.
    return len;
}
#endif

/*
 * This API converts the FilterMask to appropriate IID. If there are any errors, it will fallback on bcast IID.
 *
 */
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static inline uint8_t getIidFromFilterMask(uint8_t mask)
{
    uint8_t iid = INVALID_INTERFACE_INDEX;

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    // We need only the Pan Id masks here, as we are not matching the addresses.
    // Also mask all the unused indices.
    mask &= sRailFilterMask;

    // The only acceptable values for mask at this point are:
    // 1 - BCAST PANID   - IID(0)
    // 2 - INDEX 0       - IID(1)
    // 4 - INDEX 1       - IID(2)
    // 8 - INDEX 2       - IID(3)
    //
    // The packet should either be directed to one of the PANs or Bcast.
    // (mask & (mask -1) is a simplistic way of testing if the mask is a power of 2.
    otEXPECT_ACTION(((mask != 0) && (mask & (mask - 1)) == 0), iid = 0);

    while (mask)
    {
        iid++;
        mask >>= 1;
    }

exit:
#else
    (void)mask;
    iid = RADIO_BCAST_IID;
#endif
    return iid;
}

/*
 * This API validates the received FilterMask by checking if the destination address
 * in the received packet corresponds to destination PanID.
 */
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool isFilterMaskValid(uint8_t mask)
{
    bool valid = false;

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

    /* Packet will be considered as a valid packet in 3 cases:
     * Case 1: If the packet was directed towards bcast address or bcast panid
     *
     * Case 2: If the packet was directed to one of our valid address/PANID combos
     *         (Compare all non-bcast PANID filters against their corresponding
     *          address filters for same IID and see if any match)
     *
     * Case 3: We don't have either the destination addressing field or destination PanId
     *         in the received packet to determine if the dest address and dest pan match.
     */
    if (((mask & RADIO_BCAST_PANID_FILTER_MASK) || (mask & RADIO_BCAST_ADDR_FILTER_MASK))
        || // Case 1
           // Find any non-broadcast PAN ID match and get ready to compare it
        ((((mask & (RADIO_INDEX0_PANID_FILTER_MASK | RADIO_INDEX1_PANID_FILTER_MASK | RADIO_INDEX2_PANID_FILTER_MASK))
           >> RADIO_PANID_FILTER_SHIFT)
          &
          // ...To see if it coincides with any address matches for same IID
          (RADIO_GET_ADDR_FILTER_MASK(mask) >> RADIO_ADDR_FILTER_SHIFT))
         != 0)
        ||                                                                                        // Case 2
        (((RADIO_GET_PANID_FILTER_MASK(mask)) == 0) || ((RADIO_GET_ADDR_FILTER_MASK(mask)) == 0)) // Case 3
    )
    {
        valid = true;
    }
#else
    (void)mask;
    valid = true;
#endif

    return valid;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

enum
{
    MAC_KEY_PREV,
    MAC_KEY_CURRENT,
    MAC_KEY_NEXT,
    MAC_KEY_COUNT
};

typedef struct securityMaterial
{
    uint8_t          ackKeyId;
    uint8_t          keyId;
    uint32_t         macFrameCounter;
    uint32_t         ackFrameCounter;
    otMacKeyMaterial keys[MAC_KEY_COUNT];
} securityMaterial;

// Transmit Security
static securityMaterial sMacKeys[RADIO_INTERFACE_COUNT];

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

// CSL parameters
static uint32_t sCslPeriod;
static uint32_t sCslSampleTime;

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static uint16_t getCslPhase(uint32_t shrTxTime)
{
    uint32_t cslPeriodInUs = sCslPeriod * OT_US_PER_TEN_SYMBOLS;
    uint32_t diff;

    if (shrTxTime == 0U)
    {
        shrTxTime = otPlatAlarmMicroGetNow();
    }

    diff = ((sCslSampleTime % cslPeriodInUs) - (shrTxTime % cslPeriodInUs) + cslPeriodInUs) % cslPeriodInUs;

    return (uint16_t)(diff / OT_US_PER_TEN_SYMBOLS);
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

// Enhanced ACK IE data
static uint8_t sAckIeData[OT_ACK_IE_MAX_SIZE];
static uint8_t sAckIeDataLength = 0;

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static uint8_t generateAckIeData(uint8_t *aLinkMetricsIeData, uint8_t aLinkMetricsIeDataLen)
{
    OT_UNUSED_VARIABLE(aLinkMetricsIeData);
    OT_UNUSED_VARIABLE(aLinkMetricsIeDataLen);

    uint8_t offset = 0;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        offset += otMacFrameGenerateCslIeTemplate(sAckIeData);
    }
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    if (aLinkMetricsIeData != NULL && aLinkMetricsIeDataLen > 0)
    {
        offset += otMacFrameGenerateEnhAckProbingIe(sAckIeData, aLinkMetricsIeData, aLinkMetricsIeDataLen);
    }
#endif

    return offset;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static otError radioProcessTransmitSecurity(otRadioFrame *aFrame, uint8_t iid)
{
    otError error = OT_ERROR_NONE;
    uint8_t keyId;
    uint8_t keyToUse;
    uint8_t panIndex = efr32GetPanIndexFromIid(iid);

    otEXPECT(otMacFrameIsSecurityEnabled(aFrame) && otMacFrameIsKeyIdMode1(aFrame)
             && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed);

    OT_ASSERT(panIndex != INVALID_INTERFACE_INDEX);

    if (otMacFrameIsAck(aFrame))
    {
        keyId = otMacFrameGetKeyId(aFrame);

        otEXPECT_ACTION(keyId != 0, error = OT_ERROR_FAILED);

        if (keyId == sMacKeys[iid].keyId - 1)
        {
            keyToUse = MAC_KEY_PREV;
        }
        else if (keyId == sMacKeys[iid].keyId)
        {
            keyToUse = MAC_KEY_CURRENT;
        }
        else if (keyId == sMacKeys[iid].keyId + 1)
        {
            keyToUse = MAC_KEY_NEXT;
        }
        else
        {
            error = OT_ERROR_SECURITY;
            otEXPECT(false);
        }
    }
    else
    {
        keyId    = sMacKeys[iid].keyId;
        keyToUse = MAC_KEY_CURRENT;
    }

    aFrame->mInfo.mTxInfo.mAesKey = &sMacKeys[iid].keys[keyToUse];

    if (!aFrame->mInfo.mTxInfo.mIsHeaderUpdated)
    {
        if (otMacFrameIsAck(aFrame))
        {
            // Store ack frame counter and ack key ID for receive frame
            sMacKeys[iid].ackKeyId        = keyId;
            sMacKeys[iid].ackFrameCounter = sMacKeys[iid].macFrameCounter;
        }

        otMacFrameSetKeyId(aFrame, keyId);
        otMacFrameSetFrameCounter(aFrame, sMacKeys[iid].macFrameCounter++);
    }

    efr32PlatProcessTransmitAesCcm(aFrame, &sExtAddress[panIndex]);

exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static uint8_t readInitialPacketData(sl_rail_rx_packet_info_t *packetInfo,
                                     uint8_t                   expected_data_bytes_max,
                                     uint8_t                   expected_data_bytes_min,
                                     uint8_t                  *buffer,
                                     uint8_t                   buffer_len)
{
    uint8_t                  packetBytesRead = 0;
    sl_rail_rx_packet_info_t adjustedPacketInfo;

    // Check if we have enough buffer
    OT_ASSERT((buffer_len >= expected_data_bytes_max) || (packetInfo != NULL));

    // Read the packet info
    sl_rail_get_rx_incoming_packet_info(gRailHandle, packetInfo);

    // We are trying to get the packet info of a packet before it is completely received.
    // We do this to evaluate the FP bit in response and add IEs to ACK if needed.
    // Check to see if we have received atleast minimum number of bytes requested.
    otEXPECT_ACTION(packetInfo->packet_bytes >= expected_data_bytes_min, packetBytesRead = 0);

    adjustedPacketInfo = *packetInfo;

    // Only extract what we care about
    if (packetInfo->packet_bytes > expected_data_bytes_max)
    {
        adjustedPacketInfo.packet_bytes = expected_data_bytes_max;
        // Check if the initial portion of the packet received so far exceeds the max value requested.
        if (packetInfo->first_portion_bytes >= expected_data_bytes_max)
        {
            // If we have received more, make sure to copy only the required bytes into the buffer.
            adjustedPacketInfo.first_portion_bytes = expected_data_bytes_max;
            adjustedPacketInfo.p_last_portion_data = NULL;
        }
    }

    // Copy number of bytes as indicated in `packetInfo->first_portion_bytes` into the buffer.
    sl_rail_copy_rx_packet(gRailHandle, buffer, &adjustedPacketInfo);
    // Put it back to packetBytes.
    packetBytesRead = (uint8_t)adjustedPacketInfo.packet_bytes;

exit:
    return packetBytesRead;
}

//------------------------------------------------------------------------------
// Forward Declarations

static void RAILCb_Generic(sl_rail_handle_t aRailHandle, sl_rail_events_t aEvents);

static void efr32PhyStackInit(void);

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
static void updateIeInfoTxFrame(uint32_t shrTxTime);
#endif

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
static void efr32CoexInit(void);
// Try to transmit the current outgoing frame subject to MAC-level PTA
static void tryTxCurrentPacket(void);
#else
// Transmit the current outgoing frame.
void        txCurrentPacket(void);
#define tryTxCurrentPacket txCurrentPacket
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

static void txFailedCallback(bool isAck, uint32_t status);

static bool validatePacketDetails(sl_rail_rx_packet_handle_t   packetHandle,
                                  sl_rail_rx_packet_details_t *pPacketDetails,
                                  sl_rail_rx_packet_info_t    *pPacketInfo,
                                  uint16_t                    *packetLength);
static bool validatePacketTimestamp(sl_rail_rx_packet_details_t *pPacketDetails, uint16_t packetLength);

static void updateRxFrameTimestamp(bool aIsAckFrame, sl_rail_time_t aTimestamp);

static otError skipRxPacketLengthBytes(sl_rail_rx_packet_info_t *pPacketInfo);

//------------------------------------------------------------------------------
// Helper Functions

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool phyStackEventIsEnabled(void)
{
    bool result = false;

#if (defined(SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT) && SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT)
    result = true;
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (sRadioCoexEnabled)
    {
        result |= sl_rail_util_coex_is_enabled();
#ifdef SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
        result |= SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT;
#endif
    }
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return result;
}

static sl_rail_events_t currentEventConfig = SL_RAIL_EVENTS_NONE;
static void             updateEvents(sl_rail_events_t mask, sl_rail_events_t values)
{
    sl_rail_status_t status;
    sl_rail_events_t newEventConfig = (currentEventConfig & ~mask) | (values & mask);
    if (newEventConfig != currentEventConfig)
    {
        currentEventConfig = newEventConfig;
        status             = sl_rail_config_events(gRailHandle, mask, values);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static sl_rail_util_ieee802154_stack_event_t handlePhyStackEvent(sl_rail_util_ieee802154_stack_event_t stackEvent,
                                                                 uint32_t                              supplement)
{
    return (phyStackEventIsEnabled()
#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
                ? sl_rail_mux_ieee802154_on_event(gRailHandle, stackEvent, supplement)
#else
                ? sl_rail_util_ieee802154_on_event(stackEvent, supplement)
#endif
                : 0);
}
#else
static void updateEvents(sl_rail_events_t mask, sl_rail_events_t values)
{
    sl_rail_status_t status;
    status = sl_rail_config_events(gRailHandle, mask, values);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
}

#define handlePhyStackEvent(event, supplement) 0
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

// Set or clear the passed flag.
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static inline void setInternalFlag(uint32_t flag, bool val)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    miscRadioState = (val ? (miscRadioState | flag) : (miscRadioState & ~flag));
    CORE_EXIT_ATOMIC();
}
// Returns true if the passed flag is set, false otherwise.
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static inline bool getInternalFlag(uint32_t flag)
{
    bool isFlagSet;
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    isFlagSet = (miscRadioState & flag) ? true : false;
    CORE_EXIT_ATOMIC();

    return isFlagSet;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static inline bool txWaitingForAck(void)
{
    return (getInternalFlag(FLAG_ONGOING_TX_DATA)
            && ((sCurrentTxPacket->frame.mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED) != 0));
}

static inline bool isRadioTransmittingOrScanning(void)
{
    return ((sEnergyScanStatus != ENERGY_SCAN_STATUS_IDLE) || getInternalFlag(FLAG_ONGOING_TX_DATA)
            || getInternalFlag(RADIO_TX_EVENTS));
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool txIsDataRequest(void)
{
    uint16_t fcf = sCurrentTxPacket->frame.mPsdu[IEEE802154_FCF_OFFSET]
                   | (sCurrentTxPacket->frame.mPsdu[IEEE802154_FCF_OFFSET + 1] << 8);

    return (getInternalFlag(FLAG_ONGOING_TX_DATA)
            && (fcf & IEEE802154_FRAME_TYPE_MASK) == IEEE802154_FRAME_TYPE_COMMAND);
}

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
static inline bool isReceivingFrame(void)
{
    return (sl_rail_get_radio_state(gRailHandle) & SL_RAIL_RF_STATE_RX_ACTIVE) == SL_RAIL_RF_STATE_RX_ACTIVE;
}
#endif

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void radioSetIdle(void)
{
    if (sl_rail_get_radio_state(gRailHandle) != SL_RAIL_RF_STATE_IDLE)
    {
        sl_rail_idle(gRailHandle, SL_RAIL_IDLE, true);
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_IDLED, 0U);
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED, 0U);
    }
    sl_rail_yield_radio(gRailHandle);
}

static otError radioSetRx(uint8_t aChannel)
{
    otError          error = OT_ERROR_NONE;
    sl_rail_status_t status;

    sl_rail_scheduler_info_t bgRxSchedulerInfo = {
        .priority = SL_802154_RADIO_PRIO_BACKGROUND_RX_VALUE,
        // sliptime/transaction time is not used for bg rx
    };

#if FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    if (sl_is_multi_channel_enabled())
    {
        // Calling sl_rail_start_rx with a channel not listed in the channel
        // switching config is a bug.
        OT_ASSERT(fastChannelIndex(aChannel) != INVALID_INTERFACE_INDEX);

        radioSetIdle();
        status = sl_rail_ieee802154_config_rx_channel_switching(gRailHandle, &sChannelSwitchingCfg);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
        status = sl_rail_config_rx_options(gRailHandle,
                                           SL_RAIL_RX_OPTION_CHANNEL_SWITCHING,
                                           SL_RAIL_RX_OPTION_CHANNEL_SWITCHING);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    }
    else
    {
        status = sl_rail_config_rx_options(gRailHandle, SL_RAIL_RX_OPTION_CHANNEL_SWITCHING, SL_RAIL_RX_OPTIONS_NONE);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    }
#endif

    status = sl_rail_start_rx(gRailHandle, aChannel, &bgRxSchedulerInfo);
    otEXPECT_ACTION(status == SL_RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN, 0U);

    otLogInfoPlat("State=OT_RADIO_STATE_RECEIVE");
exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
static otError radioScheduleRx(uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    otError          error = OT_ERROR_NONE;
    sl_rail_status_t status;

#if FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    // UID 1327639: Schedule Rx and Concurrent listening when used together
    // will cause undefined behavior. Therefore it was decided to assert upon detecting
    // this condition
    assert(0);
#endif // FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

    sl_rail_scheduler_info_t bgRxSchedulerInfo = {
        .priority = SL_802154_RADIO_PRIO_BACKGROUND_RX_VALUE,
        // sliptime/transaction time is not used for bg rx
    };

    // Configure scheduled receive as requested
    sl_rail_scheduled_rx_config_t rxCfg = {
        .start                      = aStart,
        .start_mode                 = SL_RAIL_TIME_ABSOLUTE,
        .end                        = aDuration,
        .end_mode                   = SL_RAIL_TIME_DELAY,
        .rx_transition_end_schedule = 0, // To stay in schedule Rx state after packet receive.
        .hard_window_end            = 0};           // This lets us receive a packet near a window-end-event

    status = sl_rail_start_scheduled_rx(gRailHandle, aChannel, &rxCfg, &bgRxSchedulerInfo);
    otEXPECT_ACTION(status == SL_RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN, 0U);
exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

//------------------------------------------------------------------------------
// Radio Initialization
static sl_rail_handle_t efr32RailInit(efr32CommonConfig *aCommonConfig)
{
    sl_rail_status_t status;
    sl_rail_handle_t handle = SL_RAIL_EFR32_HANDLE;

#if !OPENTHREAD_RADIO
    OT_ASSERT(DEVICE_CAPABILITY_MCU_EN);
#endif

    aCommonConfig->mRailConfig.rx_packet_queue_entries = sl_rail_builtin_rx_packet_queue_entries;
    aCommonConfig->mRailConfig.p_rx_packet_queue       = sl_rail_builtin_rx_packet_queue_ptr;

    aCommonConfig->mRailConfig.rx_fifo_bytes    = sl_rail_builtin_rx_fifo_bytes;
    aCommonConfig->mRailConfig.p_rx_fifo_buffer = sl_rail_builtin_rx_fifo_ptr;

    aCommonConfig->mRailConfig.tx_fifo_init_bytes = 0U;
    aCommonConfig->mRailConfig.p_tx_fifo_buffer   = NULL;

    status = sl_rail_init(&handle, &aCommonConfig->mRailConfig, NULL);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    status = sl_rail_init_power_manager();
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

    status = sl_rail_config_cal(handle, SL_RAIL_CAL_ALL);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    status = sl_rail_set_pti_protocol(handle, SL_RAIL_PTI_PROTOCOL_THREAD);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    status = sl_rail_ieee802154_init(handle, &sRailIeee802154Config);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // Enhanced Frame Pending
    status = sl_rail_ieee802154_enable_early_frame_pending(handle, true);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    status = sl_rail_ieee802154_enable_data_frame_pending(handle, true);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    // Copies of MAC keys for encrypting at the radio layer
    memset(sMacKeys, 0, sizeof(sMacKeys));
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

    status = sl_rail_set_tx_fifo(handle,
                                 (sl_rail_fifo_buffer_align_t *)aCommonConfig->mRailTxFifo.fifo,
                                 sizeof(aCommonConfig->mRailTxFifo.fifo),
                                 0,
                                 0);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    // Enable RAIL multi-timer
    sl_rail_config_multi_timer(handle, true);

    return handle;
}

static void efr32RailConfigLoad(efr32BandConfig *aBandConfig, int8_t aTxPower)
{
    sl_rail_status_t           status;
    sl_rail_tx_power_config_t *txPowerConfig = NULL;

    if (aBandConfig->mChannelConfig != NULL)
    {
        status = sl_rail_ieee802154_set_phy_id(gRailHandle, RAIL_IEEE802154_PTI_RADIO_CONFIG_915MHZ_R23_NA_EXT);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
        status = sl_rail_config_channels(gRailHandle, aBandConfig->mChannelConfig, NULL);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
        uint16_t firstChannel = sl_rail_get_first_channel(gRailHandle, aBandConfig->mChannelConfig);
        OT_ASSERT(firstChannel == aBandConfig->mChannelMin);
        status = sl_rail_prepare_channel(gRailHandle, firstChannel);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
        status = sl_rail_ieee802154_config_g_options(gRailHandle,
                                                     SL_RAIL_IEEE802154_G_OPTION_GB868,
                                                     SL_RAIL_IEEE802154_G_OPTION_GB868);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

        txPowerConfig = (sl_rail_tx_power_config_t *)sl_rail_util_pa_get_tx_power_config_subghz();
    }
    else
    {
#if defined(SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT)
        status = sl_rail_util_ieee802154_config_radio(gRailHandle);
#else
        status = sl_rail_ieee802154_config_2p4_ghz_radio(gRailHandle);
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

        txPowerConfig = (sl_rail_tx_power_config_t *)sl_rail_util_pa_get_tx_power_config_2p4ghz();
    }

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // 802.15.4E support (only on platforms that support it, so error checking is disabled)
    // Note: This has to be called after sl_rail_ieee802154_config_2p4_ghz_radio due to a bug where this call
    // can overwrite options set below.
    sl_rail_ieee802154_config_e_options(gRailHandle,
                                        (SL_RAIL_IEEE802154_E_OPTION_GB868 | SL_RAIL_IEEE802154_E_OPTION_ENH_ACK),
                                        (SL_RAIL_IEEE802154_E_OPTION_GB868 | SL_RAIL_IEEE802154_E_OPTION_ENH_ACK));
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

    if (aTxPower != SL_INVALID_TX_POWER)
    {
        sli_update_tx_power_after_config_update(txPowerConfig, aTxPower);
    }
}

static efr32BandConfig *efr32RadioGetBandConfig(uint8_t aChannel)
{
    efr32BandConfig *config = NULL;

    if ((sBandConfig.mChannelMin <= aChannel) && (aChannel <= sBandConfig.mChannelMax))
    {
        config = &sBandConfig;
    }

    return config;
}

static void efr32ConfigInit(void (*aEventCallback)(sl_rail_handle_t railHandle, sl_rail_events_t events))
{
    sCommonConfig.mRailConfig.events_callback = aEventCallback;

#if RADIO_CONFIG_2P4GHZ_OQPSK_SUPPORT
    sBandConfig.mChannelConfig = NULL;
#else
    sBandConfig.mChannelConfig = channelConfigs[0];
#endif
    sBandConfig.mChannelMin = SL_CHANNEL_MIN;
    sBandConfig.mChannelMax = SL_CHANNEL_MAX;

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    memset(&railDebugCounters, 0x00, sizeof(efr32RadioCounters));
#endif

    sli_init_power_manager();

    gRailHandle = efr32RailInit(&sCommonConfig);
    OT_ASSERT(gRailHandle != NULL);

    updateEvents(SL_RAIL_EVENTS_ALL,
                 (0 | SL_RAIL_EVENT_RX_ACK_TIMEOUT | SL_RAIL_EVENT_RX_PACKET_RECEIVED
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
                  | SL_RAIL_EVENT_TX_SCHEDULED_TX_STARTED | SL_RAIL_EVENT_TX_SCHEDULED_TX_MISSED
                  | SL_RAIL_EVENT_RX_SCHEDULED_RX_STARTED | SL_RAIL_EVENT_RX_SCHEDULED_RX_END
                  | SL_RAIL_EVENT_RX_SCHEDULED_RX_MISSED
#endif
                  | SL_RAIL_EVENTS_TXACK_COMPLETION | SL_RAIL_EVENTS_TX_COMPLETION
                  | SL_RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT || RADIO_CONFIG_DMP_SUPPORT
                  | SL_RAIL_EVENT_CONFIG_SCHEDULED | SL_RAIL_EVENT_CONFIG_UNSCHEDULED | SL_RAIL_EVENT_SCHEDULER_STATUS
#endif
                  | SL_RAIL_EVENT_CAL_NEEDED));

    efr32RailConfigLoad(&(sBandConfig), OPENTHREAD_CONFIG_DEFAULT_TRANSMIT_POWER);
}

void efr32RadioInit(void)
{
    if (getInternalFlag(FLAG_RADIO_INIT_DONE))
    {
        return;
    }
    sl_rail_status_t            status;
    sl_status_t                 rxMemPoolStatus;
    bool                        queueStatus;
    sl_rail_timer_sync_config_t timer_sync_config = SL_RAIL_TIMER_SYNC_DEFAULT;

    // check if RAIL_TX_FIFO_SIZE is power of two..
    OT_ASSERT((RAIL_TX_FIFO_SIZE & (RAIL_TX_FIFO_SIZE - 1)) == 0);

    // check the limits of the RAIL_TX_FIFO_SIZE.
    OT_ASSERT((RAIL_TX_FIFO_SIZE >= SL_RAIL_MINIMUM_FIFO_BYTES) || (RAIL_TX_FIFO_SIZE <= SL_RAIL_MAXIMUM_FIFO_BYTES));

    efr32ConfigInit(RAILCb_Generic);
    setInternalFlag(FLAG_RADIO_INIT_DONE, true);
    status = sl_rail_config_sleep(gRailHandle, &timer_sync_config);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    sReceive.frame.mLength = 0;
    sReceive.frame.mPsdu   = NULL;

    sReceiveAck.frame.mLength = 0;
    sReceiveAck.frame.mPsdu   = sReceiveAckPsdu;

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    // Initialize the queue for received packets.
    queueStatus = queueInit(&sPendingCommandQueue, RADIO_REQUEST_BUFFER_COUNT);
    OT_ASSERT(queueStatus);

    // Specify a callback to be called upon queue overflow.
    queueStatus = queueOverflow(&sPendingCommandQueue, &pendingCommandQueueOverflowCallback);
    OT_ASSERT(queueStatus);
#endif

    for (uint8_t i = 0; i < RADIO_REQUEST_BUFFER_COUNT; i++)
    {
        // Initialize the tx buffer params.
        sTransmitBuffer[i].iid           = INVALID_INTERFACE_INDEX;
        sTransmitBuffer[i].frame.mLength = 0;
        sTransmitBuffer[i].frame.mPsdu   = sTransmitPsdu[i];

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
        sTransmitBuffer[i].frame.mInfo.mTxInfo.mIeInfo = &sTransmitIeInfo[i];
#endif
    }

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    otLinkMetricsInit(SL_OPENTHREAD_RECEIVE_SENSITIVITY);
#endif
    sCurrentBandConfig = efr32RadioGetBandConfig(OPENTHREAD_CONFIG_DEFAULT_CHANNEL);
    OT_ASSERT(sCurrentBandConfig != NULL);

    sl_rail_util_pa_init();
    sli_set_tx_power_in_rail(OPENTHREAD_CONFIG_DEFAULT_TRANSMIT_POWER);

    status = sl_rail_config_rx_options(gRailHandle,
                                       SL_RAIL_RX_OPTION_TRACK_ABORTED_FRAMES,
                                       SL_RAIL_RX_OPTION_TRACK_ABORTED_FRAMES);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    efr32PhyStackInit();
    efr32RadioSetCcaMode(SL_OPENTHREAD_RADIO_CCA_MODE);

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;

#if FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    sChannelSwitchingCfg.buffer_bytes = SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_BUF_BYTES;
    sChannelSwitchingCfg.p_buffer     = sChannelSwitchingBuffer;
    for (uint8_t i = 0U; i < SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS; i++)
    {
        sChannelSwitchingCfg.channels[i] = UNINITIALIZED_CHANNEL;
    }
#endif

    // Initialize the queue for received packets.
    queueStatus = queueInit(&sRxPacketQueue, SL_OPENTHREAD_RADIO_RX_BUFFER_COUNT);
    OT_ASSERT(queueStatus);

    // Specify a callback to be called upon queue overflow.
    queueStatus = queueOverflow(&sRxPacketQueue, &rxPacketQueueOverflowCallback);
    OT_ASSERT(queueStatus);

    // Initialize the memory pool for rx packets.
    rxMemPoolStatus =
        sl_memory_create_pool(sizeof(rxBuffer), SL_OPENTHREAD_RADIO_RX_BUFFER_COUNT, &sRxPacketMemPoolHandle);
    OT_ASSERT(rxMemPoolStatus == SL_STATUS_OK);

    otLogInfoPlat("Initialized");
}

void efr32RadioDeinit(void)
{
    sl_rail_status_t status;

    sl_rail_idle(gRailHandle, SL_RAIL_IDLE_ABORT, true);
    status = sl_rail_config_events(gRailHandle, SL_RAIL_EVENTS_ALL, 0);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

    sCurrentBandConfig = NULL;

    sl_memory_delete_pool(&sRxPacketMemPoolHandle);
}

//------------------------------------------------------------------------------
// Energy Scan support

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void energyScanComplete(int8_t scanResultDbm)
{
    sEnergyScanResultDbm = scanResultDbm;
    sEnergyScanStatus    = ENERGY_SCAN_STATUS_COMPLETED;
}

static uint16_t efr32GetSymbolDurationUs(void)
{
    uint32_t symbolRate = sl_rail_get_symbol_rate(gRailHandle);
    if (symbolRate)
    {
        symbolRate = (uint16_t)(1000000 / symbolRate);
    }
    return (symbolRate > 0) ? (uint16_t)symbolRate : 1;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void efr32ScanTimerHandler(struct sl_rail_multi_timer *tmr, sl_rail_time_t expectedTimeOfEvent, void *cbArg)
{
    OT_UNUSED_VARIABLE(tmr);
    OT_UNUSED_VARIABLE(expectedTimeOfEvent);
    OT_UNUSED_VARIABLE(cbArg);
    int16_t currentRSSIQuarterdBm =
        sl_rail_get_rssi(gRailHandle,
                         ((sScanFrameCounter == sScanFrameCounterMax) ? SL_RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT
                                                                      : SL_RAIL_GET_RSSI_NO_WAIT));

    if (currentRSSIQuarterdBm != SL_RAIL_RSSI_INVALID)
    {
        int8_t currentRSSI = (int8_t)(currentRSSIQuarterdBm / QUARTER_DBM_IN_DBM);
        if (sEnergyReadsMax < currentRSSI)
        {
            sEnergyReadsMax = currentRSSI;
        }
    }

    if (sScanFrameCounter != 0)
    {
        efr32ScanDwell(SYMBOLS_PER_ENERGY_READING);
        sScanFrameCounter--;
    }
    else
    {
        // done energy scan, reporting rssi value
        energyScanComplete(sEnergyReadsMax);
        OT_ASSERT(
            sl_rail_config_rx_options(gRailHandle, SL_RAIL_RX_OPTION_DISABLE_FRAME_DETECTION, SL_RAIL_RX_OPTIONS_NONE)
            == SL_RAIL_STATUS_NO_ERROR);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailPlatRadioEnergyScanDoneCbCount++;
#endif
    }
}

static sl_rail_status_t efr32ScanDwell(uint16_t symbols)
{
    sl_rail_cancel_multi_timer(gRailHandle, &rail_timer);
    return sl_rail_set_multi_timer(gRailHandle,
                                   &rail_timer,
                                   symbols * efr32GetSymbolDurationUs(),
                                   SL_RAIL_TIME_DELAY,
                                   efr32ScanTimerHandler,
                                   NULL);
}

static otError efr32StartEnergyScan(energyScanMode aMode, uint16_t aChannel, sl_rail_time_t aAveragingTimeUs)
{
    sl_rail_status_t status = SL_RAIL_STATUS_NO_ERROR;
    otError          error  = OT_ERROR_NONE;
    efr32BandConfig *config = NULL;

    otEXPECT_ACTION(sEnergyScanStatus == ENERGY_SCAN_STATUS_IDLE, error = OT_ERROR_BUSY);

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IN_PROGRESS;
    sEnergyScanMode   = aMode;

    sl_rail_idle(gRailHandle, SL_RAIL_IDLE, true);

    config = efr32RadioGetBandConfig(aChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);

    if (sCurrentBandConfig != config)
    {
        efr32RailConfigLoad(config, SL_INVALID_TX_POWER);
        sCurrentBandConfig = config;
    }
    if (sScanFrameCounter != 0)
    {
        return OT_ERROR_FAILED;
    }

    sEnergyReadsMax      = ENERGY_READS_MAX;
    sScanFrameCounterMax = aAveragingTimeUs / (efr32GetSymbolDurationUs() * SYMBOLS_PER_ENERGY_READING);
    sScanFrameCounter    = sScanFrameCounterMax;

    status = sl_rail_config_rx_options(gRailHandle,
                                       SL_RAIL_RX_OPTION_DISABLE_FRAME_DETECTION,
                                       SL_RAIL_RX_OPTION_DISABLE_FRAME_DETECTION);
    otEXPECT_ACTION(status == SL_RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    // set radio to Rx
    otEXPECT_ACTION(radioSetRx(aChannel) == OT_ERROR_NONE, error = OT_ERROR_FAILED);

    // start energy scan
    status = efr32ScanDwell(SYMBOLS_PER_ENERGY_READING);
    otEXPECT_ACTION(status == SL_RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

exit:
    if (status != SL_RAIL_STATUS_NO_ERROR)
    {
        energyScanComplete(OT_RADIO_RSSI_INVALID);
    }
    return error;
}

//------------------------------------------------------------------------------
// Stack support

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otPlatTimeGet();
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);

#if (RADIO_CONFIG_ENABLE_CUSTOM_EUI_SUPPORT) && defined(_SILICON_LABS_32B_SERIES_2)
    // Invalid EUI
    uint8_t nullEui[] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

    // Read the Custom EUI and compare it to nullEui
    if ((readUserData(aIeeeEui64, USERDATA_MFG_CUSTOM_EUI_64, OT_EXT_ADDRESS_SIZE, true) == -1)
        || (memcmp(aIeeeEui64, nullEui, OT_EXT_ADDRESS_SIZE) == 0))
#endif
    {
        uint64_t eui64;
        uint8_t *eui64Ptr = NULL;

#if defined(_SILICON_LABS_32B_SERIES_2)
        eui64 = SYSTEM_GetUnique();
#else
        eui64 = sl_hal_system_get_unique();
#endif
        eui64Ptr = (uint8_t *)&eui64;

        for (uint8_t i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
        {
            aIeeeEui64[i] = eui64Ptr[(OT_EXT_ADDRESS_SIZE - 1) - i];
        }
    }
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    sl_rail_status_t status;
    uint8_t          iid      = efr32GetIidFromInstance(aInstance);
    uint8_t          panIndex = efr32GetPanIndexFromIid(iid);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    OT_ASSERT(panIndex != INVALID_INTERFACE_INDEX);
    otLogInfoPlat("PANID=%X index=%u IID=%d", aPanId, panIndex, iid);
    utilsSoftSrcMatchSetPanId(iid, aPanId);

    status = sl_rail_ieee802154_set_pan_id(gRailHandle, aPanId, panIndex);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    // We already have IID 0 enabled in filtermask to track BCAST Packets, so
    // track only unique PanIds.
    if (aPanId != RADIO_BCAST_PANID)
    {
        sRailFilterMask |= RADIO_GET_FILTER_MASK(iid);
    }
#endif
exit:
    return;
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    sl_rail_status_t status;
    uint8_t          iid      = efr32GetIidFromInstance(aInstance);
    uint8_t          panIndex = efr32GetPanIndexFromIid(iid);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    OT_ASSERT(panIndex != INVALID_INTERFACE_INDEX);

    for (size_t i = 0; i < sizeof(*aAddress); i++)
    {
        sExtAddress[panIndex].m8[i] = aAddress->m8[sizeof(*aAddress) - 1 - i];
    }

    otLogInfoPlat("ExtAddr=%X%X%X%X%X%X%X%X index=%u",
                  aAddress->m8[7],
                  aAddress->m8[6],
                  aAddress->m8[5],
                  aAddress->m8[4],
                  aAddress->m8[3],
                  aAddress->m8[2],
                  aAddress->m8[1],
                  aAddress->m8[0],
                  panIndex);

    status = sl_rail_ieee802154_set_long_address(gRailHandle, (const uint8_t *)aAddress->m8, panIndex);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

exit:
    return;
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    sl_rail_status_t status;
    uint8_t          iid      = efr32GetIidFromInstance(aInstance);
    uint8_t          panIndex = efr32GetPanIndexFromIid(iid);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    OT_ASSERT(panIndex != INVALID_INTERFACE_INDEX);
    otLogInfoPlat("ShortAddr=%X index=%u", aAddress, panIndex);

    status = sl_rail_ieee802154_set_short_address(gRailHandle, aAddress, panIndex);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

exit:
    return;
}

otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    otRadioState radioState = OT_RADIO_STATE_INVALID;

    OT_UNUSED_VARIABLE(aInstance);

    switch (sl_rail_get_radio_state(gRailHandle))
    {
    case SL_RAIL_RF_STATE_RX:
    case SL_RAIL_RF_STATE_RX_ACTIVE:
        radioState = OT_RADIO_STATE_RECEIVE;
        break;

    case SL_RAIL_RF_STATE_TX:
    case SL_RAIL_RF_STATE_TX_ACTIVE:
        radioState = OT_RADIO_STATE_TRANSMIT;
        break;

    case SL_RAIL_RF_STATE_IDLE:
        radioState = OT_RADIO_STATE_SLEEP;
        break;

    case SL_RAIL_RF_STATE_INACTIVE:
        radioState = OT_RADIO_STATE_DISABLED;
        break;
    default:
        break;
    }

    return radioState;
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (getInternalFlag(FLAG_RADIO_INIT_DONE));
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    otError error = OT_ERROR_NONE;

    otEXPECT(!otPlatRadioIsEnabled(aInstance));

    otLogInfoPlat("State=OT_RADIO_STATE_SLEEP");

exit:
    return error;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);
    otEXPECT_ACTION(otPlatRadioIsEnabled(aInstance), error = OT_ERROR_INVALID_STATE);

    otLogInfoPlat("State=OT_RADIO_STATE_DISABLED");

exit:
    return error;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(!getInternalFlag(FLAG_ONGOING_TX_DATA), error = OT_ERROR_BUSY);

    otLogInfoPlat("State=OT_RADIO_STATE_SLEEP");
    setInternalFlag(FLAG_SCHEDULED_RX_PENDING, false);
    radioSetIdle();

exit:
    return error;
}

otError efr32RadioLoadChannelConfig(uint8_t aChannel, int8_t aTxPower)
{
    otError          error = OT_ERROR_NONE;
    efr32BandConfig *config;

    config = efr32RadioGetBandConfig(aChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);

    if (sCurrentBandConfig != config)
    {
        sl_rail_idle(gRailHandle, SL_RAIL_IDLE, true);
        efr32RailConfigLoad(config, aTxPower);
        sCurrentBandConfig = config;
    }
    else
    {
        sli_set_tx_power_in_rail(aTxPower);
    }

exit:
    return error;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    otError          error = OT_ERROR_NONE;
    sl_rail_status_t status;
    int8_t           txPower;
    uint8_t          iid = efr32GetIidFromInstance(aInstance);

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);
    otEXPECT_ACTION(!getInternalFlag(FLAG_ONGOING_TX_DATA) && sEnergyScanStatus != ENERGY_SCAN_STATUS_IN_PROGRESS,
                    error = OT_ERROR_INVALID_STATE);

    OT_UNUSED_VARIABLE(iid);
#if FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    uint8_t index = efr32GetPanIndexFromIid(iid);
    OT_ASSERT(index < SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS);
    sChannelSwitchingCfg.channels[index] = aChannel;
#endif

    txPower = sl_get_tx_power_for_current_channel(aInstance);
    error   = efr32RadioLoadChannelConfig(aChannel, txPower);
    otEXPECT(error == OT_ERROR_NONE);

    status = radioSetRx(aChannel);
    otEXPECT_ACTION(status == SL_RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);
    setInternalFlag(FLAG_SCHEDULED_RX_PENDING, false);

    sReceive.frame.mChannel    = aChannel;
    sReceiveAck.frame.mChannel = aChannel;

exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    otError error   = OT_ERROR_NONE;
    int8_t  txPower = sl_get_tx_power_for_current_channel(aInstance);

    // We can only have one schedule request i.e. either Rx or Tx as they use the
    // same RAIL resources.
    otEXPECT_ACTION(!getInternalFlag(FLAG_SCHEDULED_TX_PENDING | EVENT_SCHEDULED_TX_STARTED), error = OT_ERROR_FAILED);

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);
    OT_UNUSED_VARIABLE(aInstance);

    error = efr32RadioLoadChannelConfig(aChannel, txPower);
    otEXPECT(error == OT_ERROR_NONE);

    // Set the flag first and then schedule the Rx as the rail scheduler can trigger the events even before
    // sl_rail_start_scheduled_rx() API returns the status if start time is too close to the current time which could
    // otherwise cause the race condition.
    setInternalFlag(FLAG_SCHEDULED_RX_PENDING, true);
    error = radioScheduleRx(aChannel, aStart, aDuration);
    otEXPECT_ACTION(error == OT_ERROR_NONE, setInternalFlag(FLAG_SCHEDULED_RX_PENDING, false));

    sReceive.frame.mChannel    = aChannel;
    sReceiveAck.frame.mChannel = aChannel;

exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
inline static void pushPendingCommand(pendingCommandType aCmdType, uint8_t aIid, void *aCmdParams)
{
    pendingCommandEntry *pendingCommand = (pendingCommandEntry *)sl_malloc(sizeof(pendingCommandEntry));
    OT_ASSERT(pendingCommand != NULL);

    pendingCommand->cmdType = aCmdType;
    pendingCommand->iid     = aIid;

    if (aCmdType == kPendingCommandTypeTransmit)
    {
        otRadioFrame *txFrame           = (otRadioFrame *)aCmdParams;
        pendingCommand->request.txFrame = txFrame;
    }
    else if (aCmdType == kPendingCommandTypeEnergyScan)
    {
        energyScanParams *energyScanReq                 = (energyScanParams *)aCmdParams;
        pendingCommand->request.energyScan.scanChannel  = energyScanReq->scanChannel;
        pendingCommand->request.energyScan.scanDuration = energyScanReq->scanDuration;
    }

    if (!queueAdd(&sPendingCommandQueue, (void *)pendingCommand))
    {
        sl_free(pendingCommand);
    }
}
#endif

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError error   = OT_ERROR_NONE;
    int8_t  txPower = sl_get_tx_power_for_current_channel(aInstance);
    uint8_t iid     = efr32GetIidFromInstance(aInstance);

    // sTransmitBuffer's index 0 corresponds to host 1 i.e. iid 1 and reason is,
    // iid zero is reserved for broadcast frames in multipan case.
    uint8_t txBufIndex = iid ? (iid - 1) : 0;

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    // Accept GP packets even if radio is not in required state.
    if ((sl_gp_intf_get_state() != SL_GP_STATE_SEND_RESPONSE) && sl_gp_intf_should_buffer_pkt(aInstance, aFrame, false))
    {
        sl_gp_intf_buffer_pkt(aInstance);
    }
    else
#endif
    {
        OT_ASSERT(txBufIndex < RADIO_REQUEST_BUFFER_COUNT);
        OT_ASSERT(aFrame == &sTransmitBuffer[txBufIndex].frame);
        OT_ASSERT(aFrame->mPsdu == sTransmitPsdu[txBufIndex]);

        if (!aFrame->mInfo.mTxInfo.mIsARetx)
        {
            sTransmitBuffer[txBufIndex].currentRadioTxPriority = SL_802154_RADIO_PRIO_TX_MIN;
        }
        else if (sTransmitBuffer[txBufIndex].currentRadioTxPriority > SL_802154_RADIO_PRIO_TX_STEP)
        {
            sTransmitBuffer[txBufIndex].currentRadioTxPriority -= SL_802154_RADIO_PRIO_TX_STEP;
        }
        // TX priority is always bounded by the maximum priority configured
        if (sTransmitBuffer[txBufIndex].currentRadioTxPriority < SL_802154_RADIO_PRIO_TX_MAX)
        {
            sTransmitBuffer[txBufIndex].currentRadioTxPriority = SL_802154_RADIO_PRIO_TX_MAX;
        }

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
        OT_ASSERT((iid != RADIO_BCAST_IID) && (iid < RADIO_INTERFACE_COUNT));
        // Push pending transmit and exit if radio is busy.
        if (isRadioTransmittingOrScanning())
        {
            pushPendingCommand(kPendingCommandTypeTransmit, iid, aFrame);
            ExitNow(error = OT_ERROR_NONE);
        }
#endif
        error = efr32RadioLoadChannelConfig(aFrame->mChannel, txPower);
        otEXPECT(error == OT_ERROR_NONE);

        OT_ASSERT(!getInternalFlag(FLAG_ONGOING_TX_DATA));

        setInternalFlag(RADIO_TX_EVENTS, false);
        sTransmitBuffer[txBufIndex].iid = iid;
        sCurrentTxPacket                = &sTransmitBuffer[txBufIndex];

        setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, aFrame->mInfo.mTxInfo.mCsmaCaEnabled);

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        if (sCslPeriod > 0 && sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay == 0)
        {
            // Only called for CSL children (sCslPeriod > 0)
            // Note: Our SSEDs "schedule" transmissions to their parent in order to know
            // exactly when in the future the data packets go out so they can calculate
            // the accurate CSL phase to send to their parent.
            sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelayBaseTime = sl_rail_get_time(SL_RAIL_EFR32_HANDLE);
            sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay =
                SCHEDULE_TX_DELAY_US; // Chosen after internal certification testing
        }
#endif
        updateIeInfoTxFrame(sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelayBaseTime
                            + sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay + SHR_DURATION_US);
        // Note - we need to call this outside of txCurrentPacket as for Series 2,
        // this results in calling the SE interface from a critical section which is not permitted.
        radioProcessTransmitSecurity(&sCurrentTxPacket->frame, sCurrentTxPacket->iid);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

        CORE_DECLARE_IRQ_STATE;
        CORE_ENTER_ATOMIC();
        setInternalFlag(FLAG_ONGOING_TX_DATA, true);
        tryTxCurrentPacket();
        CORE_EXIT_ATOMIC();

        if (getInternalFlag(EVENT_TX_FAILED))
        {
            otPlatRadioTxStarted(aInstance, aFrame);
        }
    }
exit:
    return error;
}

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
void updateIeInfoTxFrame(uint32_t shrTxTime)
{
    OT_ASSERT(sCurrentTxPacket != NULL);

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT && OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // Seek the time sync offset and update the rendezvous time
    if (sCurrentTxPacket->frame.mInfo.mTxInfo.mIeInfo->mTimeIeOffset != 0)
    {
        uint8_t *timeIe = sCurrentTxPacket->frame.mPsdu + sCurrentTxPacket->frame.mInfo.mTxInfo.mIeInfo->mTimeIeOffset;
        uint64_t time   = otPlatTimeGet();

        // Use the same mechanism as CSL children to schedule tx for the future without CSMA
        // so that the value written to the time IE exactly matches the actual tx time of the packet
        sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelayBaseTime =
            (uint32_t)time; // Bottom 32 bits are sl_rail_get_time()
        sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay = SCHEDULE_TX_DELAY_US;
        time += sCurrentTxPacket->frame.mInfo.mTxInfo.mIeInfo->mNetworkTimeOffset
                + sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay + SHR_DURATION_US;
        *timeIe = sCurrentTxPacket->frame.mInfo.mTxInfo.mIeInfo->mTimeSyncSeq;

        *(++timeIe) = (uint8_t)(time & 0xff);
        for (uint8_t i = 1; i < sizeof(uint64_t); i++)
        {
            time        = time >> 8;
            *(++timeIe) = (uint8_t)(time & 0xff);
        }
    }
#endif // OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT && OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // Update IE data in the 802.15.4 header with the newest CSL period / phase
    if (sCslPeriod > 0 && !sCurrentTxPacket->frame.mInfo.mTxInfo.mIsHeaderUpdated)
    {
        otMacFrameSetCslIe(&sCurrentTxPacket->frame, (uint16_t)sCslPeriod, getCslPhase(shrTxTime));
    }
#else
    OT_UNUSED_VARIABLE(shrTxTime);
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

void txCurrentPacket(void)
{
    OT_ASSERT(getInternalFlag(FLAG_ONGOING_TX_DATA));
    OT_ASSERT(sCurrentTxPacket != NULL);

    sl_rail_tx_options_t txOptions = SL_RAIL_TX_OPTIONS_DEFAULT;
    sl_rail_status_t     status    = SL_RAIL_STATUS_INVALID_STATE;
    uint8_t              frameLength;
    bool                 ackRequested;

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    railDebugCounters.mRailPlatTxTriggered++;
#endif
    // signalling this event earlier, as this event can OT_ASSERT REQ (expecially for a
    // non-CSMA transmit) giving the Coex master a little more time to grant or deny.
    if (getInternalFlag(FLAG_CURRENT_TX_USE_CSMA))
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY, (uint32_t) true);
    }
    else
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY, (uint32_t) false);
    }

    frameLength = (uint8_t)sCurrentTxPacket->frame.mLength;

    if (PHY_HEADER_SIZE == 1)
    {
        sl_rail_write_tx_fifo(gRailHandle, &frameLength, sizeof frameLength, true);
    }
    else
    { // 2 byte PHR for Sub-GHz
        uint8_t PHRByte1 = (0x08U /*FCS=2byte*/ | 0x10U /*Whiten=enabled*/);
        uint8_t PHRByte2 = (uint8_t)(__RBIT(frameLength) >> 24);

        sl_rail_write_tx_fifo(gRailHandle, &PHRByte1, sizeof PHRByte1, true);
        sl_rail_write_tx_fifo(gRailHandle, &PHRByte2, sizeof PHRByte2, false);
    }
    sl_rail_write_tx_fifo(gRailHandle, sCurrentTxPacket->frame.mPsdu, frameLength - 2, false);

    sl_rail_scheduler_info_t txSchedulerInfo = {
        .priority         = sCurrentTxPacket->currentRadioTxPriority,
        .slip_time        = RADIO_SCHEDULER_CHANNEL_SLIP_TIME,
        .transaction_time = 0, // will be calculated later if DMP is used
    };

    ackRequested = (sCurrentTxPacket->frame.mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED);
    if (ackRequested)
    {
        txOptions |= SL_RAIL_TX_OPTION_WAIT_FOR_ACK;

#if RADIO_CONFIG_DMP_SUPPORT
        // time we wait for ACK
        if (sl_rail_get_symbol_rate(gRailHandle) > 0)
        {
            txSchedulerInfo.transaction_time += (sl_rail_time_t)(12 * 1e6 / sl_rail_get_symbol_rate(gRailHandle));
        }
        else
        {
            txSchedulerInfo.transaction_time += 12 * RADIO_TIMING_DEFAULT_SYMBOLTIME_US;
        }
#endif
    }

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // Update Tx options to use currently-selected antenna.
    // If antenna diverisity on Tx is disabled, leave both options 0
    // so Tx antenna tracks Rx antenna.
    if (sl_rail_util_ant_div_get_tx_antenna_mode() != SL_RAIL_UTIL_ANT_DIV_DISABLED)
    {
        txOptions |= ((sl_rail_util_ant_div_get_tx_antenna_selected() == SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1)
                          ? SL_RAIL_TX_OPTION_ANTENNA_0
                          : SL_RAIL_TX_OPTION_ANTENNA_1);
    }
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#if RADIO_CONFIG_DMP_SUPPORT
    // time needed for the frame itself
    // 4B preamble, 1B SFD, 1B PHR is not counted in frameLength
    if (sl_rail_get_bit_rate(gRailHandle) > 0)
    {
        txSchedulerInfo.transaction_time +=
            (sl_rail_time_t)((frameLength + 4 + 1 + 1) * 8 * 1e6 / sl_rail_get_bit_rate(gRailHandle));
    }
    else
    { // assume 250kbps
        txSchedulerInfo.transaction_time += (frameLength + 4 + 1 + 1) * RADIO_TIMING_DEFAULT_BYTETIME_US;
    }
#endif

    // Prioritize the Tx over schedule Rx to avoid missing data check-ins such as data polls.
    if (getInternalFlag(FLAG_SCHEDULED_RX_PENDING))
    {
        sl_rail_idle(gRailHandle, SL_RAIL_IDLE, true);
        setInternalFlag(FLAG_SCHEDULED_RX_PENDING | EVENT_SCHEDULED_RX_STARTED, false);
    }

    if (sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay == 0)
    {
        if (getInternalFlag(FLAG_CURRENT_TX_USE_CSMA))
        {
#if RADIO_CONFIG_DMP_SUPPORT
            // time needed for CSMA/CA
            txSchedulerInfo.transaction_time += RADIO_TIMING_CSMA_OVERHEAD_US;
#endif
            csmaConfig.csma_tries        = sCurrentTxPacket->frame.mInfo.mTxInfo.mMaxCsmaBackoffs;
            csmaConfig.cca_threshold_dbm = sCcaThresholdDbm;

            status = sl_rail_start_cca_csma_tx(gRailHandle,
                                               sCurrentTxPacket->frame.mChannel,
                                               txOptions,
                                               &csmaConfig,
                                               &txSchedulerInfo);
        }
        else
        {
            status = sl_rail_start_tx(gRailHandle, sCurrentTxPacket->frame.mChannel, txOptions, &txSchedulerInfo);
        }

        if (status == SL_RAIL_STATUS_NO_ERROR)
        {
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED, 0U);
        }
    }
    else
    {
        // For CSL transmitters (FTDs):
        // mTxDelayBaseTime = rx-timestamp (end of sync word) when we received CSL-sync with IEs
        // mTxDelay = Delay starting from mTxDelayBaseTime
        //
        // For CSL receivers (SSEDs):
        // mTxDelayBaseTime = timestamp when otPlatRadioTransmit is called
        // mTxDelay = Chosen value in the future where transmit is scheduled, so we know exactly
        // when to calculate the phase (we can't do this on-the-fly as the packet is going out
        // due to platform limitations.  see radioScheduleRx)
        //
        // Note that both use single CCA config, overriding any CCA/CSMA configs from the stack
        //
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        sl_rail_scheduled_tx_config_t scheduleTxOptions = {
            .when = sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelayBaseTime
                    + sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay - SHR_DURATION_US,
            .mode         = SL_RAIL_TIME_ABSOLUTE,
            .tx_during_rx = SL_RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX};

        // Set ccaBackoff to some constant value, so we have predictable radio warmup time for schedule tx.
        cslCsmaConfig.cca_backoff_us = CSL_CSMA_BACKOFF_TIME_IN_US;
        scheduleTxOptions.when -= cslCsmaConfig.cca_backoff_us;

        // CSL transmissions don't use CSMA but MAC accounts for single CCA time.
        // cslCsmaConfig is set to SL_RAIL_CSMA_CONFIG_SINGLE_CCA above.
        status = sl_rail_start_scheduled_cca_csma_tx(gRailHandle,
                                                     sCurrentTxPacket->frame.mChannel,
                                                     txOptions,
                                                     &scheduleTxOptions,
                                                     &cslCsmaConfig,
                                                     &txSchedulerInfo);

        if (status == SL_RAIL_STATUS_NO_ERROR)
        {
            setInternalFlag(FLAG_SCHEDULED_TX_PENDING, true);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            railDebugCounters.mRailEventsScheduledTxTriggeredCount++;
#endif
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED, 0U);
        }
#endif
    }

    if (status == SL_RAIL_STATUS_NO_ERROR)
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailTxStarted++;
#endif
    }
    else
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailTxStartFailed++;
#endif
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t)ackRequested);
        txFailedCallback(false, EVENT_TX_FAILED);

        otSysEventSignalPending();
    }
}

// This API gets called from init procedure so instance to IID mapping does not exist
// at that point. Also this api will get called sequentially so assign new transmit
// buffer if aInstance does not exist in sInstances.
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    uint8_t       index       = 0;
    otRadioFrame *aRadioFrame = NULL;

    otEXPECT(sl_ot_rtos_task_can_access_pal());

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    for (index = 0; index < OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_NUM; index++)
    {
        if (sInstances[index] == aInstance || sInstances[index] == NULL)
        {
            break;
        }
    }
#endif // OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

    aRadioFrame = &sTransmitBuffer[index].frame;

exit:
    return aRadioFrame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    otError  error;
    uint32_t start;
    int8_t   rssi     = OT_RADIO_RSSI_INVALID;
    uint8_t  aChannel = sReceive.frame.mChannel;
    uint8_t  iid      = efr32GetIidFromInstance(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    otEXPECT(!getInternalFlag(FLAG_ONGOING_TX_DATA));

    OT_UNUSED_VARIABLE(iid);
#if FAST_CHANNEL_SWITCHING_SUPPORT && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    uint8_t index = efr32GetPanIndexFromIid(iid);
    OT_ASSERT(index < SL_RAIL_IEEE802154_RX_CHANNEL_SWITCHING_NUM_CHANNELS);
    if (sChannelSwitchingCfg.channels[index] != UNINITIALIZED_CHANNEL)
    {
        aChannel = sChannelSwitchingCfg.channels[index];
    }
#endif

    error = efr32StartEnergyScan(ENERGY_SCAN_MODE_SYNC, aChannel, SL_OPENTHREAD_RSSI_AVERAGING_TIME);
    otEXPECT(error == OT_ERROR_NONE);

    start = sl_rail_get_time(SL_RAIL_EFR32_HANDLE);

    // waiting for the event SL_RAIL_EVENT_RSSI_AVERAGE_DONE
    while (sEnergyScanStatus == ENERGY_SCAN_STATUS_IN_PROGRESS
           && ((sl_rail_get_time(SL_RAIL_EFR32_HANDLE) - start) < SL_OPENTHREAD_RSSI_AVERAGING_TIMEOUT))
        ;

    if (sEnergyScanStatus == ENERGY_SCAN_STATUS_COMPLETED)
    {
        rssi = sEnergyScanResultDbm;
    }

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;
exit:
    return rssi;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sRadioCapabilities;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sPromiscuous;
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    sl_rail_status_t status;

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    sPromiscuous = aEnable;

    status = sl_rail_ieee802154_set_promiscuous_mode(gRailHandle, aEnable);
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);

exit:
    return;
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    // set Frame Pending bit for all outgoing ACKs if aEnable is false
    sIsSrcMatchEnabled = aEnable;

exit:
    return;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(aPower != NULL, error = OT_ERROR_INVALID_ARGS);
    // sl_rail_get_tx_power_dbm() returns power in deci-dBm (0.1dBm)
    // Divide by 10 because aPower is supposed be in units dBm
    *aPower = (int8_t)(sl_rail_get_tx_power_dbm(gRailHandle) / 10);

exit:
    return error;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    return sli_set_default_tx_power(aInstance, aPower);
}

// Required for RCP error recovery
// See src/lib/spinel/radio_spinel.cpp::RestoreProperties()
otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower)
{
    return sli_set_channel_max_tx_power(aInstance, aChannel, aMaxPower);
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    otEXPECT_ACTION(aThreshold != NULL, error = OT_ERROR_INVALID_ARGS);

    *aThreshold = sCcaThresholdDbm;

exit:
    return error;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);
    sCcaThresholdDbm = aThreshold;

exit:
    return error;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return SL_OPENTHREAD_RECEIVE_SENSITIVITY;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    otError error = OT_ERROR_NONE;
    uint8_t iid   = efr32GetIidFromInstance(aInstance);

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    OT_ASSERT((iid != RADIO_BCAST_IID) && (iid < RADIO_INTERFACE_COUNT));

    // Push pending energy-scan and exit if radio is busy.
    if (isRadioTransmittingOrScanning())
    {
        energyScanParams params = {aScanChannel, aScanDuration};
        pushPendingCommand(kPendingCommandTypeEnergyScan, iid, &params);
        ExitNow(error = OT_ERROR_NONE);
    }
#endif

    sEnergyScanActiveInterface = iid;
    error = efr32StartEnergyScan(ENERGY_SCAN_MODE_ASYNC, aScanChannel, (sl_rail_time_t)aScanDuration * US_IN_MS);

exit:
    return error;
}

//------------------------------------------------------------------------------
// Radio Config: Thread 1.2 transmit security support

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
void otPlatRadioSetMacKey(otInstance             *aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType)
{
    OT_UNUSED_VARIABLE(aKeyIdMode);
    OT_UNUSED_VARIABLE(aKeyType);

    uint8_t iid = efr32GetIidFromInstance(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    OT_ASSERT(aPrevKey != NULL && aCurrKey != NULL && aNextKey != NULL);

    // MAC frame counters are reset before updating keys. This order
    // safeguards against issues that can arise when the radio
    // platform handles TX security and counter assignment.  The
    // radio platform might prepare an enhanced ACK to a received
    // frame from an parallel (e.g., ISR) context, which consumes
    // a MAC frame counter value.
    //
    // If the MAC key is updated before the frame counter is cleared,
    // the radio could receive and send an enhanced ACK between these
    // two actions, possibly using the new MAC key with a larger
    // (current) frame counter value. This could then prevent the
    // receiver from accepting subsequent transmissions after the
    // frame counter reset for a long time.
    //
    // While resetting counters first might briefly cause an enhanced
    // ACK to be sent with the old key and a zero counter (which might
    // be rejected by the receiver), this is a transient issue that
    // quickly resolves itself.
    otPlatRadioSetMacFrameCounter(aInstance, 0);

    sMacKeys[iid].keyId = aKeyId;
    memcpy(&sMacKeys[iid].keys[MAC_KEY_PREV], aPrevKey, sizeof(otMacKeyMaterial));
    memcpy(&sMacKeys[iid].keys[MAC_KEY_CURRENT], aCurrKey, sizeof(otMacKeyMaterial));
    memcpy(&sMacKeys[iid].keys[MAC_KEY_NEXT], aNextKey, sizeof(otMacKeyMaterial));

#if (OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PSA)
    size_t  aKeyLen = 0;
    otError error   = OT_ERROR_NONE;

    error = otPlatCryptoExportKey(sMacKeys[iid].keys[MAC_KEY_PREV].mKeyMaterial.mKeyRef,
                                  sMacKeys[iid].keys[MAC_KEY_PREV].mKeyMaterial.mKey.m8,
                                  sizeof(sMacKeys[iid].keys[MAC_KEY_PREV]),
                                  &aKeyLen);
    OT_ASSERT(error == OT_ERROR_NONE);

    error = otPlatCryptoExportKey(sMacKeys[iid].keys[MAC_KEY_CURRENT].mKeyMaterial.mKeyRef,
                                  sMacKeys[iid].keys[MAC_KEY_CURRENT].mKeyMaterial.mKey.m8,
                                  sizeof(sMacKeys[iid].keys[MAC_KEY_CURRENT]),
                                  &aKeyLen);
    OT_ASSERT(error == OT_ERROR_NONE);

    error = otPlatCryptoExportKey(sMacKeys[iid].keys[MAC_KEY_NEXT].mKeyMaterial.mKeyRef,
                                  sMacKeys[iid].keys[MAC_KEY_NEXT].mKeyMaterial.mKey.m8,
                                  sizeof(sMacKeys[iid].keys[MAC_KEY_NEXT]),
                                  &aKeyLen);
    OT_ASSERT(error == OT_ERROR_NONE);
#endif

exit:
    return;
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    uint8_t iid = efr32GetIidFromInstance(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    sMacKeys[iid].macFrameCounter = aMacFrameCounter;

    CORE_EXIT_ATOMIC();

exit:
    return;
}

void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    uint8_t iid = efr32GetIidFromInstance(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    if (aMacFrameCounter > sMacKeys[iid].macFrameCounter)
    {
        sMacKeys[iid].macFrameCounter = aMacFrameCounter;
    }

    CORE_EXIT_ATOMIC();

exit:
    return;
}

//------------------------------------------------------------------------------
// Radio Config: Enhanced Acks, CSL

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
otError otPlatRadioEnableCsl(otInstance         *aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr)
{
    otError error = OT_ERROR_NONE;

    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddr);
    OT_UNUSED_VARIABLE(aExtAddr);

    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);

    sCslPeriod = aCslPeriod;

exit:
    return error;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    otEXPECT(sl_ot_rtos_task_can_access_pal());
    sCslSampleTime = aCslSampleTime;
exit:
    return;
}

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otPlatTimeGetXtalAccuracy();
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return SL_OPENTHREAD_CSL_TX_UNCERTAINTY;
}

//------------------------------------------------------------------------------
// Radio Config: Link Metrics

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance          *aInstance,
                                          otLinkMetrics        aLinkMetrics,
                                          const otShortAddress aShortAddress,
                                          const otExtAddress  *aExtAddress)
{
    otError error;

    OT_UNUSED_VARIABLE(aInstance);
    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);

    error = otLinkMetricsConfigureEnhAckProbing(aShortAddress, aExtAddress, aLinkMetrics);

exit:
    return error;
}
#endif
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled)
{
    otError error;

    OT_UNUSED_VARIABLE(aInstance);
    otEXPECT_ACTION(sl_ot_rtos_task_can_access_pal(), error = OT_ERROR_REJECTED);

    sl_status_t status = sl_rail_util_coex_set_enable(aEnabled);
    if (aEnabled && !sl_rail_util_coex_is_enabled())
    {
        otLogInfoPlat("Coexistence GPIO configurations not set");
        return OT_ERROR_FAILED;
    }
    sRadioCoexEnabled = aEnabled;

    error = (status != SL_STATUS_OK) ? OT_ERROR_FAILED : OT_ERROR_NONE;

exit:
    return error;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return (sRadioCoexEnabled && sl_rail_util_coex_is_enabled());
}

#endif // OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
//------------------------------------------------------------------------------
// Radio implementation: Enhanced ACKs, CSL

// Return false if we should generate an immediate ACK
// Return true otherwise
SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool writeIeee802154EnhancedAck(sl_rail_handle_t          aRailHandle,
                                       sl_rail_rx_packet_info_t *packetInfoForEnhAck,
                                       uint32_t                  rxTimestamp,
                                       uint8_t                  *initialPktReadBytes,
                                       uint8_t                  *receivedPsdu)
{
    // RAIL will generate an Immediate ACK for us.
    // For an Enhanced ACK, we need to generate the whole packet ourselves.

    // An 802.15.4 packet from RAIL should look like:
    // 1/2 |   1/2  | 0/1  |  0/2   | 0/2/8  |  0/2   | 0/2/8  |   14
    // PHR | MacFCF | Seq# | DstPan | DstAdr | SrcPan | SrcAdr | SecHdr

    // With sl_rail_ieee802154_enable_early_frame_pending(), SL_RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND
    // is triggered after receiving through the SrcAdr field of Version 0/1 packets,
    // and after receiving through the SecHdr for Version 2 packets.

    otRadioFrame receivedFrame, enhAckFrame;
    uint8_t      enhAckPsdu[IEEE802154_MAX_LENGTH];

#define EARLY_FRAME_PENDING_EXPECTED_BYTES (2U + 2U + 1U + 2U + 8U + 2U + 8U + 14U)
#define FINAL_PACKET_LENGTH_WITH_IE (EARLY_FRAME_PENDING_EXPECTED_BYTES + OT_ACK_IE_MAX_SIZE)

    otEXPECT((packetInfoForEnhAck != NULL) && (initialPktReadBytes != NULL) && (receivedPsdu != NULL));

    *initialPktReadBytes = readInitialPacketData(packetInfoForEnhAck,
                                                 EARLY_FRAME_PENDING_EXPECTED_BYTES,
                                                 (PHY_HEADER_SIZE + 2),
                                                 receivedPsdu,
                                                 FINAL_PACKET_LENGTH_WITH_IE);

    uint8_t iid = INVALID_INTERFACE_INDEX;

    if (*initialPktReadBytes == 0U)
    {
        return true; // Nothing to read, which means generating an immediate ACK is also pointless
    }

    receivedFrame.mPsdu = receivedPsdu + PHY_HEADER_SIZE;
    // This should be set to the expected length of the packet is being received.
    // We consider this while calculating the phase value below.
    receivedFrame.mLength = packetInfoForEnhAck->p_first_portion_data[0];
    enhAckFrame.mPsdu     = enhAckPsdu + PHY_HEADER_SIZE;

    if (!otMacFrameIsVersion2015(&receivedFrame))
    {
        return false;
    }

    otMacAddress aSrcAddress;
    uint8_t      linkMetricsDataLen = 0;
    uint8_t     *dataPtr            = NULL;
    bool         setFramePending    = false;

    iid = getIidFromFilterMask(packetInfoForEnhAck->filter_mask);

    otMacFrameGetSrcAddr(&receivedFrame, &aSrcAddress);

    if (sIsSrcMatchEnabled && (aSrcAddress.mType != OT_MAC_ADDRESS_TYPE_NONE))
    {
        setFramePending = (aSrcAddress.mType == OT_MAC_ADDRESS_TYPE_EXTENDED
                               ? (utilsSoftSrcMatchExtFindEntry(iid, &aSrcAddress.mAddress.mExtAddress) >= 0)
                               : (utilsSoftSrcMatchShortFindEntry(iid, aSrcAddress.mAddress.mShortAddress) >= 0));
    }

    // Generate our IE header.
    // Write IE data for enhanced ACK (link metrics + allocate bytes for CSL)

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    uint8_t linkMetricsData[OT_ENH_PROBING_IE_DATA_MAX_SIZE];

    linkMetricsDataLen = otLinkMetricsEnhAckGenData(&aSrcAddress, sLastLqi, sLastRssi, linkMetricsData);

    if (linkMetricsDataLen > 0)
    {
        dataPtr = linkMetricsData;
    }
#endif

    sAckIeDataLength = generateAckIeData(dataPtr, linkMetricsDataLen);

    otEXPECT(otMacFrameGenerateEnhAck(&receivedFrame, setFramePending, sAckIeData, sAckIeDataLength, &enhAckFrame)
             == OT_ERROR_NONE);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        // Calculate time in the future where the SHR is done being sent out
        uint32_t ackShrDoneTime = // Currently partially received packet's SHR time
            (rxTimestamp
             - (packetInfoForEnhAck->packet_bytes * OT_RADIO_SYMBOL_TIME * 2)
             // PHR of this packet
             + (PHY_HEADER_SIZE * OT_RADIO_SYMBOL_TIME * 2)
             // Received frame's expected time in the PHR
             + (receivedFrame.mLength * OT_RADIO_SYMBOL_TIME * 2)
             // rxToTx turnaround time
             + sRailIeee802154Config.timings.rx_to_tx
             // PHR time of the ACK
             + (PHY_HEADER_SIZE * OT_RADIO_SYMBOL_TIME * 2)
             // SHR time of the ACK
             + (SHR_SIZE * OT_RADIO_SYMBOL_TIME * 2));

        // Update IE data in the 802.15.4 header with the newest CSL period / phase
        otMacFrameSetCslIe(&enhAckFrame, (uint16_t)sCslPeriod, getCslPhase(ackShrDoneTime));
    }
#else
    OT_UNUSED_VARIABLE(rxTimestamp);
#endif

    if (otMacFrameIsSecurityEnabled(&enhAckFrame))
    {
        otEXPECT(radioProcessTransmitSecurity(&enhAckFrame, iid) == OT_ERROR_NONE);
    }

    // Before we're done, store some important info in reserved bits in the
    // MAC header (cleared later)
    // Check whether frame pending is set.
    // Check whether enhanced ACK is secured.
    otEXPECT((skipRxPacketLengthBytes(packetInfoForEnhAck)) == OT_ERROR_NONE);
    uint8_t *macFcfPointer =
        ((packetInfoForEnhAck->first_portion_bytes == 0) ? packetInfoForEnhAck->p_last_portion_data
                                                         : packetInfoForEnhAck->p_first_portion_data);

    if (otMacFrameIsSecurityEnabled(&enhAckFrame))
    {
        *macFcfPointer |= IEEE802154_SECURED_OUTGOING_ENHANCED_ACK;
    }

    if (setFramePending)
    {
        *macFcfPointer |= IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;
    }

    // Fill in PHR now that we know Enh-ACK's length
    if (PHY_HEADER_SIZE == 2U)
    { // Not true till SubGhz implementation is in place
        enhAckPsdu[0] = (0x08U /*FCS=2byte*/ | 0x10U /*Whiten=enabled*/);
        enhAckPsdu[1] = (uint8_t)(__RBIT(enhAckFrame.mLength) >> 24);
    }
    else
    {
        enhAckPsdu[0] = enhAckFrame.mLength;
    }

    sl_rail_status_t enhAckStatus = sl_rail_ieee802154_write_enh_ack(aRailHandle, enhAckPsdu, enhAckFrame.mLength);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    otEXPECT_ACTION(enhAckStatus == SL_RAIL_STATUS_NO_ERROR, railDebugCounters.mRailEventsEnhAckTxFailed++);
#else
    otEXPECT(enhAckStatus == SL_RAIL_STATUS_NO_ERROR);
#endif

exit:
    return true;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

//------------------------------------------------------------------------------
// RAIL callbacks

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void dataRequestCommandCallback(sl_rail_handle_t aRailHandle)
{
#define MAX_EXPECTED_BYTES (2U + 2U + 1U) // PHR + FCF + DSN

    uint8_t                  receivedPsdu[IEEE802154_MAX_LENGTH];
    uint8_t                  pktOffset = PHY_HEADER_SIZE;
    uint8_t                  initialPktReadBytes;
    sl_rail_rx_packet_info_t packetInfo;
    uint32_t                 rxCallbackTimestamp = otPlatAlarmMicroGetNow();

    // This callback occurs after the address fields of an incoming
    // ACK-requesting CMD or DATA frame have been received and we
    // can do a frame pending check.  We must also figure out what
    // kind of ACK is being requested -- Immediate or Enhanced.

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    if (writeIeee802154EnhancedAck(aRailHandle, &packetInfo, rxCallbackTimestamp, &initialPktReadBytes, receivedPsdu))
    {
        // We also return true above if there were failures in
        // generating an enhanced ACK.
        return;
    }
#else
    OT_UNUSED_VARIABLE(rxCallbackTimestamp);
    initialPktReadBytes =
        readInitialPacketData(&packetInfo, MAX_EXPECTED_BYTES, pktOffset + 2, receivedPsdu, MAX_EXPECTED_BYTES);
#endif

    // Calculate frame pending for immediate-ACK
    // If not, RAIL will send an immediate ACK, but we need to do FP lookup.
    sl_rail_status_t status = SL_RAIL_STATUS_NO_ERROR;

    // Check if we read the FCF, if not, set macFcf to 0
    uint16_t macFcf = (initialPktReadBytes <= pktOffset) ? 0U : receivedPsdu[pktOffset];

    bool framePendingSet = false;

    if (sIsSrcMatchEnabled)
    {
        sl_rail_ieee802154_address_t sourceAddress;

        status = sl_rail_ieee802154_get_address(aRailHandle, &sourceAddress);
        otEXPECT(status == SL_RAIL_STATUS_NO_ERROR);

        uint8_t iid = getIidFromFilterMask(packetInfo.filter_mask);
        if ((sourceAddress.address_length == SL_RAIL_IEEE802154_LONG_ADDRESS
             && utilsSoftSrcMatchExtFindEntry(iid, (otExtAddress *)sourceAddress.long_address) >= 0)
            || (sourceAddress.address_length == SL_RAIL_IEEE802154_SHORT_ADDRESS
                && utilsSoftSrcMatchShortFindEntry(iid, sourceAddress.short_address) >= 0))
        {
            status = sl_rail_ieee802154_toggle_frame_pending(aRailHandle);
            otEXPECT(status == SL_RAIL_STATUS_NO_ERROR);
            framePendingSet = true;
        }
    }
    else if ((macFcf & IEEE802154_FRAME_TYPE_MASK) != IEEE802154_FRAME_TYPE_DATA)
    {
        status = sl_rail_ieee802154_toggle_frame_pending(aRailHandle);
        otEXPECT(status == SL_RAIL_STATUS_NO_ERROR);
        framePendingSet = true;
    }

    if (framePendingSet)
    {
        // Store whether frame pending was set in the outgoing ACK in a reserved
        // bit of the MAC header (cleared later)

        otEXPECT((skipRxPacketLengthBytes(&packetInfo)) == OT_ERROR_NONE);
        uint8_t *macFcfPointer =
            ((packetInfo.first_portion_bytes == 0) ? packetInfo.p_last_portion_data : packetInfo.p_first_portion_data);
        *macFcfPointer |= IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;
    }

exit:
    if (status == SL_RAIL_STATUS_INVALID_STATE)
    {
        otLogWarnPlat("Too late to modify outgoing FP");
    }
    else
    {
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void packetReceivedCallback(void)
{
    sl_rail_rx_packet_info_t    packetInfo;
    sl_rail_rx_packet_details_t packetDetails;
    uint16_t                    length            = 0;
    bool                        framePendingInAck = false;
    bool                        dropPacket        = false;
    uint8_t                     iid               = 0;
    sl_status_t                 status;
    bool                        isRxPacketQueued;
    rxBuffer                   *rxPacketBuf = NULL;

    sl_rail_rx_packet_handle_t packetHandle =
        sl_rail_get_rx_packet_info(gRailHandle, SL_RAIL_RX_PACKET_HANDLE_NEWEST, &packetInfo);
    otEXPECT_ACTION((packetHandle != SL_RAIL_RX_PACKET_HANDLE_INVALID
                     && packetInfo.packet_status == SL_RAIL_RX_PACKET_READY_SUCCESS),
                    dropPacket = true);

    otEXPECT_ACTION(validatePacketDetails(packetHandle, &packetDetails, &packetInfo, &length), dropPacket = true);

    otEXPECT_ACTION((skipRxPacketLengthBytes(&packetInfo)) == OT_ERROR_NONE, dropPacket = true);

    uint8_t macFcf = ((packetInfo.first_portion_bytes == 0) ? packetInfo.p_last_portion_data[0]
                                                            : packetInfo.p_first_portion_data[0]);

    iid = getIidFromFilterMask(packetInfo.filter_mask);

    if (packetDetails.is_ack)
    {
        otEXPECT_ACTION(
            (length >= IEEE802154_MIN_LENGTH && (macFcf & IEEE802154_FRAME_TYPE_MASK) == IEEE802154_FRAME_TYPE_ACK),
            dropPacket = true);

        // read packet
        sl_rail_copy_rx_packet(gRailHandle, sReceiveAck.frame.mPsdu, &packetInfo);

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventAcksReceived++;
#endif
        sReceiveAck.frame.mLength = length;

        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED, (uint32_t)isReceivingFrame());

        if (txWaitingForAck()
            && (sReceiveAck.frame.mPsdu[IEEE802154_DSN_OFFSET] == sCurrentTxPacket->frame.mPsdu[IEEE802154_DSN_OFFSET]))
        {
            otEXPECT_ACTION(validatePacketTimestamp(&packetDetails, length), dropPacket = true);

            sReceiveAck.frame.mInfo.mRxInfo.mRssi = packetDetails.rssi_dbm;
            sReceiveAck.frame.mInfo.mRxInfo.mLqi  = packetDetails.lqi;
            sReceiveAck.iid                       = iid;
            updateRxFrameTimestamp(true, packetDetails.time_received.packet_time);

            // Processing the ACK frame in ISR context avoids the Tx state to be messed up,
            // in case the Rx FIFO queue gets wiped out in a DMP situation.
            setInternalFlag(EVENT_TX_SUCCESS, true);
            setInternalFlag(FLAG_WAITING_FOR_ACK | FLAG_ONGOING_TX_DATA | EVENT_SCHEDULED_TX_STARTED, false);

            framePendingInAck = ((macFcf & IEEE802154_FRAME_FLAG_FRAME_PENDING) != 0);
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_RECEIVED, (uint32_t)framePendingInAck);

            if (txIsDataRequest() && framePendingInAck)
            {
                emPendingData = true;
            }
        }
        // Yield the radio upon receiving an ACK as long as it is not related to
        // a data request.
        if (!txIsDataRequest())
        {
            sl_rail_yield_radio(gRailHandle);
        }
    }
    else
    {
        otEXPECT_ACTION(sPromiscuous || (length >= IEEE802154_MIN_DATA_LENGTH), dropPacket = true);

        otEXPECT_ACTION(validatePacketTimestamp(&packetDetails, length), dropPacket = true);

        // Drop the packet if queue is full.
        otEXPECT_ACTION(!queueIsFull(&sRxPacketQueue), dropPacket = true);
        // Allocate a block from memory pool for the received packet.
        status = sl_memory_pool_alloc(&sRxPacketMemPoolHandle, (void **)&rxPacketBuf);
        // Drop the packet if no more memory block present in the pool to store it.
        otEXPECT_ACTION(status == SL_STATUS_OK && rxPacketBuf != NULL, dropPacket = true);

        // read packet
        sl_rail_copy_rx_packet(gRailHandle, rxPacketBuf->psdu, &packetInfo);

        rxPacketBuf->packetInfo.length    = (uint8_t)length;
        rxPacketBuf->packetInfo.channel   = (uint8_t)packetDetails.channel;
        rxPacketBuf->packetInfo.rssi      = packetDetails.rssi_dbm;
        rxPacketBuf->packetInfo.lqi       = packetDetails.lqi;
        rxPacketBuf->packetInfo.timestamp = packetDetails.time_received.packet_time;
        rxPacketBuf->packetInfo.iid       = iid;

        // Queue the rx packet or drop it if queueing fails and free the memory block.
        isRxPacketQueued = queueAdd(&sRxPacketQueue, (void *)rxPacketBuf);
        otEXPECT_ACTION(isRxPacketQueued, dropPacket = true);

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailPlatRadioReceiveProcessedCount++;
#endif

        if (macFcf & IEEE802154_FRAME_FLAG_ACK_REQUIRED)
        {
            (void)handlePhyStackEvent((sl_rail_is_rx_auto_ack_paused(gRailHandle)
                                           ? SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED
                                           : SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACKING),
                                      (uint32_t)isReceivingFrame());
            setInternalFlag(FLAG_ONGOING_TX_ACK, true);
        }
        else
        {
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED, (uint32_t)isReceivingFrame());
            // We received a frame that does not require an ACK as result of a data
            // poll: we yield the radio here.
            if (emPendingData)
            {
                sl_rail_yield_radio(gRailHandle);
                emPendingData = false;
            }
        }
    }
exit:
    if (dropPacket)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED, (uint32_t)isReceivingFrame());

        IgnoreError(sl_memory_pool_free(&sRxPacketMemPoolHandle, rxPacketBuf));
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void packetSentCallback(bool isAck)
{
    if (isAck)
    {
        // We successfully sent out an ACK.
        setInternalFlag(FLAG_ONGOING_TX_ACK, false);
        // We acked the packet we received after a poll: we can yield now.
        if (emPendingData)
        {
            sl_rail_yield_radio(gRailHandle);
            emPendingData = false;
        }
    }
    else if (getInternalFlag(FLAG_ONGOING_TX_DATA))
    {
        setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, false);

        if (txWaitingForAck())
        {
            setInternalFlag(FLAG_WAITING_FOR_ACK, true);
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_WAITING, 0U);
        }
        else
        {
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ENDED, 0U);
            sl_rail_yield_radio(gRailHandle);
            setInternalFlag(EVENT_TX_SUCCESS, true);
            // Broadcast packet clear the ONGOING flag here.
            setInternalFlag(FLAG_ONGOING_TX_DATA | EVENT_SCHEDULED_TX_STARTED, false);
        }
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventPacketSent++;
#endif
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void txFailedCallback(bool isAck, uint32_t status)
{
    if (isAck)
    {
        setInternalFlag(FLAG_ONGOING_TX_ACK, false);
    }
    else if (getInternalFlag(FLAG_ONGOING_TX_DATA))
    {
        if (status == EVENT_TX_CCA_FAILED)
        {
            setInternalFlag(EVENT_TX_CCA_FAILED, true);
            setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            railDebugCounters.mRailEventChannelBusy++;
#endif
        }
        else
        {
            setInternalFlag(EVENT_TX_FAILED, true);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            railDebugCounters.mRailEventTxAbort++;
#endif
        }
        setInternalFlag((FLAG_ONGOING_TX_DATA | FLAG_WAITING_FOR_ACK | EVENT_SCHEDULED_TX_STARTED), false);
        sl_rail_yield_radio(gRailHandle);
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void ackTimeoutCallback(void)
{
    OT_ASSERT(txWaitingForAck());
    OT_ASSERT(getInternalFlag(FLAG_WAITING_FOR_ACK));

    setInternalFlag(EVENT_TX_NO_ACK, true);
    setInternalFlag(FLAG_ONGOING_TX_DATA | EVENT_SCHEDULED_TX_STARTED, false);

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    railDebugCounters.mRailEventNoAck++;
#endif

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // If antenna diversity is enabled toggle the selected antenna.
    sl_rail_util_ant_div_toggle_tx_antenna();
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // TO DO: Check if we have an OT function that
    // provides the number of mac retry attempts left
    (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_TIMEDOUT, 0);

    setInternalFlag(FLAG_WAITING_FOR_ACK, false);
    sl_rail_yield_radio(gRailHandle);
    emPendingData = false;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void schedulerEventCallback(sl_rail_handle_t aRailHandle)
{
    sl_rail_scheduler_status_t scheduler_status;
    sl_rail_status_t           rail_status;
    sl_rail_get_scheduler_status(aRailHandle, &scheduler_status, &rail_status);
    if (scheduler_status != SL_RAIL_SCHEDULER_STATUS_NO_ERROR)
    {
        setInternalFlag(FLAG_SCHEDULED_RX_PENDING | FLAG_SCHEDULED_TX_PENDING | EVENT_SCHEDULED_TX_STARTED, false);
        if (getInternalFlag(FLAG_ONGOING_TX_ACK))
        {
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED, (uint32_t)isReceivingFrame());
            txFailedCallback(true, EVENT_TX_FAILED);
        }
        // We were in the process of TXing a data frame, treat it as a CCA_FAIL.
        if (getInternalFlag(FLAG_ONGOING_TX_DATA))
        {
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t)txWaitingForAck());
            txFailedCallback(false, EVENT_TX_CCA_FAILED);
        }
        // We are waiting for an ACK: we will never get the ACK we were waiting for.
        // We want to call ackTimeoutCallback() only if the PACKET_SENT event
        // already fired (which would clear the FLAG_ONGOING_TX_DATA flag).
        if (getInternalFlag(FLAG_WAITING_FOR_ACK))
        {
            ackTimeoutCallback();
        }

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventSchedulerStatusError++;
#endif
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void RAILCb_Generic(sl_rail_handle_t aRailHandle, sl_rail_events_t aEvents)
{
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & (SL_RAIL_EVENT_RX_SYNC_0_DETECT | SL_RAIL_EVENT_RX_SYNC_1_DETECT))
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_STARTED, (uint32_t)isReceivingFrame());
    }
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (aEvents & SL_RAIL_EVENT_SIGNAL_DETECTED)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_SIGNAL_DETECTED, 0U);
    }
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    if ((aEvents & SL_RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND)
#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
        && !sl_rail_is_rx_auto_ack_paused(aRailHandle)
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    )
    {
        dataRequestCommandCallback(aRailHandle);
    }

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & SL_RAIL_EVENT_RX_FILTER_PASSED)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACCEPTED, (uint32_t)isReceivingFrame());
    }
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

    if (aEvents & SL_RAIL_EVENT_TX_PACKET_SENT)
    {
        packetSentCallback(false);
    }
    else if (aEvents & SL_RAIL_EVENT_TX_CHANNEL_BUSY)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t)txWaitingForAck());
        txFailedCallback(false, EVENT_TX_CCA_FAILED);
    }
    else if (aEvents & SL_RAIL_EVENT_TX_BLOCKED)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t)txWaitingForAck());
        txFailedCallback(false, EVENT_TX_FAILED);
    }
    else if (aEvents & (SL_RAIL_EVENT_TX_UNDERFLOW | SL_RAIL_EVENT_TX_ABORTED))
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ABORTED, (uint32_t)txWaitingForAck());
        txFailedCallback(false, EVENT_TX_FAILED);
    }
    else
    {
        // Pre-completion aEvents are processed in their logical order:
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
        if (aEvents & SL_RAIL_EVENT_TX_START_CCA)
        {
            // We are starting RXWARM for a CCA check
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_SOON, 0U);
        }
        if (aEvents & SL_RAIL_EVENT_TX_CCA_RETRY)
        {
            // We failed a CCA check and need to retry
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_BUSY, 0U);
        }
        if (aEvents & SL_RAIL_EVENT_TX_CHANNEL_CLEAR)
        {
            // We're going on-air
            (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED, 0U);
        }
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    }

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

    // Note: SL_RAIL_EVENT_TX_SCHEDULED_TX_STARTED and SL_RAIL_EVENT_RX_SCHEDULED_RX_STARTED have the
    // same numerical value because one cannot schedule both RX and TX simultaneously.
    // Have to take due care to check for these statuses depending on whether we're
    // scheduling a transmit or receive.  Therefore, we're using an internal flag
    // 'FLAG_SCHEDULED_RX_PENDING' to track our scheduled-receive state.

    if (getInternalFlag(FLAG_SCHEDULED_RX_PENDING))
    {
        if (aEvents & SL_RAIL_EVENT_RX_SCHEDULED_RX_STARTED)
        {
            setInternalFlag(EVENT_SCHEDULED_RX_STARTED, true);
        }

        // Once done with scheduled receive, clear internal scheduled-rx flag and idle.
        // If we miss a scheduled receive, let application schedule another.
        if (aEvents & SL_RAIL_EVENT_RX_SCHEDULED_RX_END || aEvents & SL_RAIL_EVENT_RX_SCHEDULED_RX_MISSED)
        {
            setInternalFlag(FLAG_SCHEDULED_RX_PENDING | EVENT_SCHEDULED_RX_STARTED, false);
            radioSetIdle();
        }
    }
    else
    {
        if (aEvents & SL_RAIL_EVENT_TX_SCHEDULED_TX_STARTED)
        {
            setInternalFlag(EVENT_SCHEDULED_TX_STARTED, true);
            setInternalFlag(FLAG_SCHEDULED_TX_PENDING, false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            railDebugCounters.mRailEventsScheduledTxStartedCount++;
#endif
        }
        else if (aEvents & SL_RAIL_EVENT_TX_SCHEDULED_TX_MISSED)
        {
            setInternalFlag(FLAG_SCHEDULED_TX_PENDING, false);
            txFailedCallback(false, EVENT_TX_SCHEDULER_ERROR);
        }
    }
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

    if (aEvents & SL_RAIL_EVENT_RX_PACKET_RECEIVED)
    {
        packetReceivedCallback();
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventPacketReceived++;
#endif
    }

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & SL_RAIL_EVENT_RX_FRAME_ERROR)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED, (uint32_t)isReceivingFrame());
    }
    // The following 3 events cause us to not receive a packet
    if (aEvents
        & (SL_RAIL_EVENT_RX_PACKET_ABORTED | SL_RAIL_EVENT_RX_ADDRESS_FILTERED | SL_RAIL_EVENT_RX_FIFO_OVERFLOW))
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_FILTERED, (uint32_t)isReceivingFrame());
    }
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

    if (aEvents & SL_RAIL_EVENT_TXACK_PACKET_SENT)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_SENT, (uint32_t)isReceivingFrame());
        packetSentCallback(true);
    }
    if (aEvents & (SL_RAIL_EVENT_TXACK_ABORTED | SL_RAIL_EVENT_TXACK_UNDERFLOW))
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED, (uint32_t)isReceivingFrame());
        txFailedCallback(true, 0xFF);
    }
    if (aEvents & SL_RAIL_EVENT_TXACK_BLOCKED)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED, (uint32_t)isReceivingFrame());
        txFailedCallback(true, 0xFF);
    }
    // Deal with ACK timeout after possible RX completion in case RAIL
    // notifies us of the ACK and the timeout simultaneously -- we want
    // the ACK to win over the timeout.
    if ((aEvents & SL_RAIL_EVENT_RX_ACK_TIMEOUT) && (getInternalFlag(FLAG_WAITING_FOR_ACK)))
    {
        ackTimeoutCallback();
    }

    if (aEvents & SL_RAIL_EVENT_CONFIG_UNSCHEDULED)
    {
        (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED, 0U);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventConfigUnScheduled++;
#endif
    }

    if (aEvents & SL_RAIL_EVENT_CONFIG_SCHEDULED)
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventConfigScheduled++;
#endif
    }

    if (aEvents & SL_RAIL_EVENT_SCHEDULER_STATUS)
    {
        schedulerEventCallback(aRailHandle);
    }

    if (aEvents & SL_RAIL_EVENT_CAL_NEEDED)
    {
        sl_rail_status_t status;

        status = sl_rail_calibrate(aRailHandle, NULL, SL_RAIL_CAL_ALL_PENDING);
        // TODO: Non-RTOS DMP case fails
#if (!defined(SL_CATALOG_BLUETOOTH_PRESENT) || defined(SL_CATALOG_KERNEL_PRESENT))
        // TEMPORARY - this asserts on Mux - OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
        OT_UNUSED_VARIABLE(status);
#else
        OT_UNUSED_VARIABLE(status);
#endif

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventCalNeeded++;
#endif
    }

    // scheduled and unscheduled config events happen very often,
    // especially in a DMP situation where there is an active BLE connection.
    // Waking up the OT RTOS task on every one of these occurrences causes
    // a lower priority Serial task to starve and makes it appear like a code lockup
    // There is no reason to wake the OT task for these events!
    if (!(aEvents & SL_RAIL_EVENT_CONFIG_SCHEDULED) && !(aEvents & SL_RAIL_EVENT_CONFIG_UNSCHEDULED))
    {
        otSysEventSignalPending();
    }
}

//------------------------------------------------------------------------------
// Main thread packet handling

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool validatePacketDetails(sl_rail_rx_packet_handle_t   packetHandle,
                                  sl_rail_rx_packet_details_t *pPacketDetails,
                                  sl_rail_rx_packet_info_t    *pPacketInfo,
                                  uint16_t                    *packetLength)
{
    bool             pktValid = true;
    sl_rail_status_t rStatus;
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    rxDebugStep = 0;
#endif

    rStatus = sl_rail_get_rx_packet_details(gRailHandle, packetHandle, pPacketDetails);
    otEXPECT_ACTION(rStatus == SL_RAIL_STATUS_NO_ERROR, pktValid = false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    rxDebugStep++;
#endif

    otEXPECT_ACTION(isFilterMaskValid(pPacketInfo->filter_mask), pktValid = false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    rxDebugStep++;
#endif

    // RAIL's packetBytes includes the (1 or 2 byte) PHY header but not the 2-byte CRC.
    // We want *packetLength to match the PHY header length so we add 2 for CRC
    // and subtract the PHY header size.
    *packetLength = pPacketInfo->packet_bytes + 2U - PHY_HEADER_SIZE;

    if (PHY_HEADER_SIZE == 1)
    {
        otEXPECT_ACTION(*packetLength == pPacketInfo->p_first_portion_data[0], pktValid = false);
    }
    else
    {
        uint8_t lengthByte = ((pPacketInfo->first_portion_bytes > 1) ? pPacketInfo->p_first_portion_data[1]
                                                                     : pPacketInfo->p_last_portion_data[0]);
        otEXPECT_ACTION(*packetLength == (uint16_t)(__RBIT(lengthByte) >> 24), pktValid = false);
    }
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    rxDebugStep++;
#endif

    // check the length validity of recv packet; RAIL should take care of this.
    otEXPECT_ACTION((*packetLength >= IEEE802154_MIN_LENGTH && *packetLength <= IEEE802154_MAX_LENGTH),
                    pktValid = false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    rxDebugStep++;
#endif

exit:
#if (OPENTHREAD_CONFIG_LOG_LEVEL == OT_LOG_LEVEL_DEBG)
    if (!pktValid)
    {
        otLogDebgPlat("RX Pkt Invalid: rStatus=0x%lX, filterMask=0x%2X, pktLen=%i",
                      rStatus,
                      pPacketInfo->filter_mask,
                      *packetLength);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        otLogDebgPlat("RX debug step=%i", rxDebugStep);
#endif
    }
#endif
    return pktValid;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static bool validatePacketTimestamp(sl_rail_rx_packet_details_t *pPacketDetails, uint16_t packetLength)
{
    bool rxTimestampValid = true;

    // Get the timestamp when the SFD was received
    otEXPECT_ACTION(pPacketDetails->time_received.time_position != SL_RAIL_PACKET_TIME_INVALID,
                    rxTimestampValid = false);

    // + PHY HEADER SIZE for PHY header
    // We would not need this if PHR is not included and we want the MHR
    pPacketDetails->time_received.total_packet_bytes = packetLength + PHY_HEADER_SIZE;

    otEXPECT_ACTION((sl_rail_get_rx_time_sync_word_end(gRailHandle, pPacketDetails) == SL_RAIL_STATUS_NO_ERROR),
                    rxTimestampValid = false);
exit:
    return rxTimestampValid;
}

otError otPlatMultipanGetActiveInstance(otInstance **aInstance)
{
    otError error = OT_ERROR_NOT_IMPLEMENTED;
    OT_UNUSED_VARIABLE(aInstance);

    return error;
}

otError otPlatMultipanSetActiveInstance(otInstance *aInstance, bool aCompletePending)
{
    otError error = OT_ERROR_NOT_IMPLEMENTED;

    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aCompletePending);

    return error;
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static void updateRxFrameTimestamp(bool aIsAckFrame, sl_rail_time_t aTimestamp)
{
    // Current time > sync-receive timestamp
    // Therefore lower 32 bits of current time should always be greater than lower 32 bits
    // of sync-rx timestamp unless there is a overflow.  In such cases, we do not want to
    // take overflow into consideration for sync-rx timestamp.
    uint64_t railUsTimeNow    = otPlatTimeGet();
    uint32_t railUsTimerWraps = railUsTimeNow >> 32;

    // Address multiple overflows, such as what would happen if the current time overflows
    // from 0x00000001FFFFFFFF to 0x0000000200000000 (leave the higher 32 bits as 0)
    if ((railUsTimeNow & 0xFFFFFFFF) <= aTimestamp)
    {
        railUsTimerWraps--;
    }

    if (aIsAckFrame)
    {
        sReceiveAck.frame.mInfo.mRxInfo.mTimestamp = aTimestamp + ((uint64_t)railUsTimerWraps << 32);
    }
    else
    {
        sReceive.frame.mInfo.mRxInfo.mTimestamp = aTimestamp + ((uint64_t)railUsTimerWraps << 32);
    }
}

SL_CODE_CLASSIFY(SL_CODE_COMPONENT_OT_PLATFORM_ABSTRACTION, SL_CODE_CLASS_TIME_CRITICAL)
static otError skipRxPacketLengthBytes(sl_rail_rx_packet_info_t *pPacketInfo)
{
    otError error = OT_ERROR_NONE;
    otEXPECT_ACTION(pPacketInfo->first_portion_bytes > 0, error = OT_ERROR_FAILED);

    pPacketInfo->p_first_portion_data += PHY_HEADER_SIZE;
    pPacketInfo->packet_bytes -= PHY_HEADER_SIZE;

    if (PHY_HEADER_SIZE == 1 || pPacketInfo->first_portion_bytes > 1)
    {
        pPacketInfo->first_portion_bytes -= PHY_HEADER_SIZE;
    }
    else
    {
        pPacketInfo->first_portion_bytes = 0U;
        // Increment lastPortionData to skip the second byte of the PHY header
        otEXPECT_ACTION(pPacketInfo->p_last_portion_data != NULL, error = OT_ERROR_FAILED);
        pPacketInfo->p_last_portion_data++;
    }

exit:
    return error;
}

// This function dequeues the rx-queue, move the content to sReceive buffer
// and return the memory block which was used to store the received packet.
// So that memory block can be freed after submitting the receiveDone Callback.
static rxBuffer *prepareNextRxPacketforCb(void)
{
    rxBuffer *rxPacketBuf = (rxBuffer *)queueRemove(&sRxPacketQueue);
    OT_ASSERT(rxPacketBuf != NULL);
    uint8_t *psdu = rxPacketBuf->psdu;

    // Check the reserved bits in the MAC header, then clear them.
    // If we sent an enhanced ACK, check if it was secured.
    // Set this flag only when the packet is really acknowledged with a secured enhanced ACK.
    sReceive.frame.mInfo.mRxInfo.mAckedWithSecEnhAck = ((*psdu & IEEE802154_SECURED_OUTGOING_ENHANCED_ACK) != 0);
    *psdu &= ~IEEE802154_SECURED_OUTGOING_ENHANCED_ACK;

    // Check whether frame pendinng bit was set in the outgoing ACK.
    // Set this flag only when the packet is really acknowledged with frame pending set.
    sReceive.frame.mInfo.mRxInfo.mAckedWithFramePending = ((*psdu & IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK) != 0);
    *psdu &= ~IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;

    sReceive.frame.mChannel = rxPacketBuf->packetInfo.channel;
    sReceive.frame.mLength  = rxPacketBuf->packetInfo.length;
    sReceive.frame.mPsdu    = rxPacketBuf->psdu;

    sReceive.frame.mInfo.mRxInfo.mRssi = rxPacketBuf->packetInfo.rssi;
    sLastRssi                          = rxPacketBuf->packetInfo.rssi;

    sReceive.frame.mInfo.mRxInfo.mLqi = rxPacketBuf->packetInfo.lqi;
    sLastLqi                          = rxPacketBuf->packetInfo.lqi;

    sReceive.iid = rxPacketBuf->packetInfo.iid;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    // Use stored values for these
    sReceive.frame.mInfo.mRxInfo.mAckKeyId        = sMacKeys[sReceive.iid].ackKeyId;
    sReceive.frame.mInfo.mRxInfo.mAckFrameCounter = sMacKeys[sReceive.iid].ackFrameCounter;
#endif

    updateRxFrameTimestamp(false, rxPacketBuf->packetInfo.timestamp);
    return rxPacketBuf;
}

static void processNextRxPacket(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    sReceiveError           = OT_ERROR_NONE;
    uint8_t     interfaceId = INVALID_INTERFACE_INDEX;
    otInstance *instance    = NULL;
    rxBuffer   *rxPacketBuf = NULL;

    rxPacketBuf = prepareNextRxPacketforCb();

    // sReceive buffer gets populated from prepareNextRxPacketforCb.
    interfaceId = sReceive.iid;

    // Submit broadcast packet to all initilized instances.
    do
    {
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
        instance = otPlatMultipanIidToInstance(interfaceId);
#else
        instance = aInstance;
#endif
        interfaceId++;
        if (instance == NULL)
        {
            continue;
        }
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
            otPlatDiagRadioReceiveDone(instance, &sReceive.frame, sReceiveError);
        }
        else
#endif // OPENTHREAD_CONFIG_DIAG_ENABLE
        {
            bool isGpPacket = sl_gp_intf_is_gp_pkt(&sReceive.frame);
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
            // For multipan RCP, continue normal processing.
            OT_UNUSED_VARIABLE(isGpPacket);
#else
            // Else, we should not receive GP packets.
            otEXPECT(!isGpPacket);
#endif

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
            (void)sl_gp_intf_should_buffer_pkt(instance, &sReceive.frame, true);
#endif
            otPlatRadioReceiveDone(instance, &sReceive.frame, sReceiveError);
        }
    } while (sReceive.iid == RADIO_BCAST_IID && interfaceId < RADIO_INTERFACE_COUNT);

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    railDebugCounters.mRailPlatRadioReceiveDoneCbCount++;
#endif

#if !OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
exit:
#endif
    IgnoreError(sl_memory_pool_free(&sRxPacketMemPoolHandle, rxPacketBuf));
    otSysEventSignalPending();
}

static void processRxPackets(otInstance *aInstance)
{
    while (!queueIsEmpty(&sRxPacketQueue))
    {
        processNextRxPacket(aInstance);
    }
}

static void processTxComplete(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    otError       txStatus;
    otRadioFrame *ackFrame = NULL;

    if (getInternalFlag(RADIO_TX_EVENTS))
    {
        if (getInternalFlag(EVENT_TX_SUCCESS))
        {
            txStatus = OT_ERROR_NONE;

            if (sCurrentTxPacket->frame.mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED)
            {
                ackFrame = &sReceiveAck.frame;
            }

            setInternalFlag(EVENT_TX_SUCCESS, false);
        }
        else if (getInternalFlag(EVENT_TX_CCA_FAILED))
        {
            txStatus = OT_ERROR_CHANNEL_ACCESS_FAILURE;
            setInternalFlag(EVENT_TX_CCA_FAILED, false);
        }
        else if (getInternalFlag(EVENT_TX_NO_ACK))
        {
            txStatus = OT_ERROR_NO_ACK;
            setInternalFlag(EVENT_TX_NO_ACK, false);
        }
        else
        {
            txStatus = OT_ERROR_ABORT;
            setInternalFlag(EVENT_TX_FAILED, false);
        }

        if (txStatus != OT_ERROR_NONE)
        {
            otLogDebgPlat("Transmit failed ErrorCode=%d", txStatus);
        }

#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
            otPlatDiagRadioTransmitDone(otPlatMultipanIidToInstance(sCurrentTxPacket->iid),
                                        &sCurrentTxPacket->frame,
                                        txStatus);
#else
            otPlatDiagRadioTransmitDone(aInstance, &sCurrentTxPacket->frame, txStatus);
#endif
        }
        else
#endif
        {
            // Clear any internally-set txDelays so future transmits are not affected.
            sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelayBaseTime = 0;
            sCurrentTxPacket->frame.mInfo.mTxInfo.mTxDelay         = 0;
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
            otPlatRadioTxDone(otPlatMultipanIidToInstance(sCurrentTxPacket->iid),
                              &sCurrentTxPacket->frame,
                              ackFrame,
                              txStatus);
#else
            otPlatRadioTxDone(aInstance, &sCurrentTxPacket->frame, ackFrame, txStatus);
#endif
        }

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailPlatRadioTxDoneCbCount++;
#endif
        otSysEventSignalPending();
    }
}

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
static inline void processPendingCommands(void)
{
    // Check and process pending transmit and energy scan commands if radio is not busy.
    if (!queueIsEmpty(&sPendingCommandQueue) && (!isRadioTransmittingOrScanning()))
    {
        // Dequeue the pending command
        pendingCommandEntry *pendingCommand = (pendingCommandEntry *)queueRemove(&sPendingCommandQueue);
        OT_ASSERT(pendingCommand != NULL);
        uint8_t iid = pendingCommand->iid;

        switch (pendingCommand->cmdType)
        {
        case kPendingCommandTypeTransmit:
            otPlatRadioTransmit(otPlatMultipanIidToInstance(iid), pendingCommand->request.txFrame);
            break;

        case kPendingCommandTypeEnergyScan:
            otPlatRadioEnergyScan(otPlatMultipanIidToInstance(iid),
                                  pendingCommand->request.energyScan.scanChannel,
                                  pendingCommand->request.energyScan.scanDuration);
            break;

        default:
            OT_ASSERT(false);
            break;
        }

        // Free the allocated memory.
        sl_free(pendingCommand);
    }
}
#endif // OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE

void efr32RadioProcess(otInstance *aInstance)
{
    (void)handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK, 0U);

    // We should process the received packet first. Adding it at the end of this function,
    // will delay the stack notification until the next call to efr32RadioProcess()
    processRxPackets(aInstance);
    processTxComplete(aInstance);

    if (sEnergyScanMode == ENERGY_SCAN_MODE_ASYNC && sEnergyScanStatus == ENERGY_SCAN_STATUS_COMPLETED)
    {
        sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;
        OT_ASSERT(sEnergyScanActiveInterface != INVALID_INTERFACE_INDEX);
#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
        otPlatRadioEnergyScanDone(otPlatMultipanIidToInstance(sEnergyScanActiveInterface), sEnergyScanResultDbm);
#else
        otPlatRadioEnergyScanDone(aInstance, sEnergyScanResultDbm);
#endif
        sEnergyScanActiveInterface = INVALID_INTERFACE_INDEX;
        otSysEventSignalPending();

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        railDebugCounters.mRailEventEnergyScanCompleted++;
#endif
    }

#if OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
    processPendingCommands();
#endif // OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE
}

//------------------------------------------------------------------------------
// Antenna Diversity, Wifi coexistence and Runtime PHY select support

sl_rail_status_t efr32RadioSetCcaMode(uint8_t aMode)
{
    return sl_rail_ieee802154_config_cca_mode(gRailHandle, aMode);
}

sl_rail_ieee802154_phy_t efr32GetPtiRadioConfig(void)
{
    return (sl_rail_ieee802154_get_phy_id(gRailHandle));
}

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT

otError setRadioState(otRadioState state)
{
    otError error = OT_ERROR_NONE;

    // Defer idling the radio if we have an ongoing TX task
    otEXPECT_ACTION(!getInternalFlag(FLAG_ONGOING_TX_DATA), error = OT_ERROR_FAILED);

    switch (state)
    {
    case OT_RADIO_STATE_RECEIVE:
        otEXPECT_ACTION(radioSetRx(sReceive.frame.mChannel) == OT_ERROR_NONE, error = OT_ERROR_FAILED);
        break;
    case OT_RADIO_STATE_SLEEP:
        radioSetIdle();
        break;
    default:
        error = OT_ERROR_FAILED;
    }
exit:
    return error;
}

void sl_ot_update_active_radio_config(void)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    // Proceed with PHY selection only if 2.4 GHz band is used
    otEXPECT(sBandConfig.mChannelConfig == NULL);

    otRadioState currentState = otPlatRadioGetState(NULL);
    otEXPECT(setRadioState(OT_RADIO_STATE_SLEEP) == OT_ERROR_NONE);
    sl_rail_util_ieee802154_config_radio(gRailHandle);
    otEXPECT(setRadioState(currentState) == OT_ERROR_NONE);

exit:
    CORE_EXIT_ATOMIC();
    return;
}
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
void efr32AntennaConfigInit(void)
{
    sl_rail_status_t status;
    sl_rail_util_ant_div_init();
    status = sl_rail_util_ant_div_update_antenna_config();
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
}
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

static void changeDynamicEvents(void)
{
    /* clang-format off */
  const sl_rail_events_t eventMask = SL_RAIL_EVENTS_NONE
                                     | SL_RAIL_EVENT_RX_SYNC_0_DETECT
                                     | SL_RAIL_EVENT_RX_SYNC_1_DETECT
                                     | SL_RAIL_EVENT_RX_FRAME_ERROR
                                     | SL_RAIL_EVENT_RX_FIFO_OVERFLOW
                                     | SL_RAIL_EVENT_RX_ADDRESS_FILTERED
                                     | SL_RAIL_EVENT_RX_PACKET_ABORTED
                                     | SL_RAIL_EVENT_RX_FILTER_PASSED
                                     | SL_RAIL_EVENT_TX_CHANNEL_CLEAR
                                     | SL_RAIL_EVENT_TX_CCA_RETRY
                                     | SL_RAIL_EVENT_TX_START_CCA
                                     | SL_RAIL_EVENT_SIGNAL_DETECTED;
    /* clang-format on */
    sl_rail_events_t eventValues = SL_RAIL_EVENTS_NONE;

    if (phyStackEventIsEnabled())
    {
        eventValues |= eventMask;
    }
    updateEvents(eventMask, eventValues);
}
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

static void efr32PhyStackInit(void)
{
#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    efr32AntennaConfigInit();
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    efr32CoexInit();
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    changeDynamicEvents();
#endif
}

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT

static void emRadioEnableAutoAck(void)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    if (getInternalFlag(FLAG_RADIO_INIT_DONE))
    {
        if ((rhoActive >= RHO_INT_ACTIVE) // Internal always holds ACKs
            || ((rhoActive > RHO_INACTIVE)
                && ((sl_rail_util_coex_get_options() & SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF)
                    != SL_RAIL_UTIL_COEX_OPT_DISABLED)))
        {
            sl_rail_pause_rx_auto_ack(gRailHandle, true);
        }
        else
        {
            sl_rail_pause_rx_auto_ack(gRailHandle, false);
        }
    }
    CORE_EXIT_ATOMIC();
}

static void emRadioEnablePta(bool enable)
{
    halInternalInitPta();

    // When PTA is enabled, we want to negate PTA_REQ as soon as an incoming
    // frame is aborted, e.g. due to filtering.  To do that we must turn off
    // the TRACKABFRAME feature that's normally on to benefit sniffing on PTI.
    sl_rail_status_t status =
        sl_rail_config_rx_options(gRailHandle,
                                  SL_RAIL_RX_OPTION_TRACK_ABORTED_FRAMES,
                                  (enable ? SL_RAIL_RX_OPTIONS_NONE : SL_RAIL_RX_OPTION_TRACK_ABORTED_FRAMES));
    OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
}

static void efr32CoexInit(void)
{
#if SL_OPENTHREAD_COEX_COUNTER_ENABLE && defined(SL_CATALOG_RAIL_MULTIPLEXER_PRESENT)
    sl_rail_mux_set_coex_counter_handler(gRailHandle, &sl_ot_coex_counter_on_event);
#else
    sli_radio_coex_reset();
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE && defined(SL_CATALOG_RAIL_MULTIPLEXER_PRESENT)

#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
#if defined(SL_RAIL_UTIL_COEX_REQ_GPIO) || defined(SL_RAIL_UTIL_COEX_REQ_PORT) || defined(SL_RAIL_UTIL_COEX_GNT_GPIO) \
    || defined(SL_RAIL_UTIL_COEX_GNT_PORT) || SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
    sl_rail_util_ot_enable_coex_state_event_filter();
#endif
#endif // OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE

    sl_rail_util_coex_options_t coexOptions = sl_rail_util_coex_get_options();

#if SL_OPENTHREAD_COEX_MAC_HOLDOFF_ENABLE
    coexOptions |= SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF;
#else
    if (sl_rail_util_coex_get_radio_holdoff())
    {
        coexOptions |= SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF;
    }
#endif // SL_OPENTHREAD_COEX_MAC_HOLDOFF_ENABLE

    sl_rail_util_coex_set_options(coexOptions);

    emRadioEnableAutoAck(); // Might suspend AutoACK if RHO already in effect
    emRadioEnablePta(sl_rail_util_coex_is_enabled());
}

// Managing radio transmission
static void onPtaGrantTx(sl_rail_util_coex_req_t ptaStatus)
{
    // Only pay attention to first PTA Grant callback, ignore any further ones
    if (ptaGntEventReported)
    {
        return;
    }
    ptaGntEventReported = true;

    OT_ASSERT(ptaStatus == SL_RAIL_UTIL_COEX_REQCB_GRANTED);
    // PTA is telling us we've gotten GRANT and should send ASAP *without* CSMA
    setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, false);
    txCurrentPacket();
}

static void tryTxCurrentPacket(void)
{
    OT_ASSERT(getInternalFlag(FLAG_ONGOING_TX_DATA));

    ptaGntEventReported = false;
    sl_rail_util_ieee802154_stack_event_t ptaStatus =
        handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_MAC, (uint32_t)&onPtaGrantTx);
    if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_SUCCESS)
    {
        // Normal case where PTA allows us to start the (CSMA) transmit below
        txCurrentPacket();
    }
    else if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_CB_PENDING)
    {
        // onPtaGrantTx() callback will take over (and might already have)
    }
    else if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_HOLDOFF)
    {
        txFailedCallback(false, EVENT_TX_FAILED);
    }
}

// Managing CCA Threshold
static void setCcaThreshold(void)
{
    if (sCcaThresholdDbm == CCA_THRESHOLD_UNINIT)
    {
        sCcaThresholdDbm = CCA_THRESHOLD_DEFAULT;
    }
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    int8_t thresholddBm = sCcaThresholdDbm;

    if (getInternalFlag(FLAG_RADIO_INIT_DONE))
    {
        if (rhoActive > RHO_INACTIVE)
        {
            thresholddBm = SL_RAIL_RSSI_INVALID_DBM;
        }
        sl_rail_status_t status = sl_rail_set_cca_threshold(gRailHandle, thresholddBm);
        OT_ASSERT(status == SL_RAIL_STATUS_NO_ERROR);
    }
    CORE_EXIT_ATOMIC();
}

static void emRadioHoldOffInternalIsr(uint8_t active)
{
    if (active != rhoActive)
    {
        rhoActive = active; // Update rhoActive early
        if (getInternalFlag(FLAG_RADIO_INIT_DONE))
        {
            setCcaThreshold();
            emRadioEnableAutoAck();
        }
    }
}

// External API used by Coex Component
SL_WEAK void emRadioHoldOffIsr(bool active)
{
    emRadioHoldOffInternalIsr((uint8_t)active | (rhoActive & ~RHO_EXT_ACTIVE));
}

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
static void sl_ot_coex_counter_on_event(sl_rail_util_coex_event_t event)
#else
void sl_rail_util_coex_counter_on_event(sl_rail_util_coex_event_t event)
#endif
{
    otEXPECT(event < SL_RAIL_UTIL_COEX_EVENT_COUNT);
    efr32RadioCoexCounters[event] += 1;
exit:
    return;
}

void efr32RadioClearCoexCounters(void)
{
    memset((void *)efr32RadioCoexCounters, 0, sizeof(efr32RadioCoexCounters));
}

#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE

#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
void efr32ClearRadioCounters(void)
{
    memset(&railDebugCounters, 0, sizeof(railDebugCounters));
}
#endif // RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
