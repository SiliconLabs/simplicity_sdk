/***************************************************************************//**
 * @file
 * @brief Functions used by the BGAPI protocol to output BGAPI trace over RTT
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "sl_core.h"
#include "sl_bgapi.h"
#include "sli_bgapi_trace.h"
#include "sl_bgapi_trace_config.h"
#include "sli_bgapi_debug_api.h"
// Sleeptimer is used for timestamps but only when metadata output is enabled
#if SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE
#include "sl_sleeptimer.h"
#endif // SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE
#include "SEGGER_RTT.h"
#include "sl_rtt_buffer_index.h"

/*******************************************************************************
 **************************  LOCAL DATA TYPES   ********************************
 ******************************************************************************/

/**
 * @brief Data structure for BGAPI header and the trace metadata event
 */
PACKSTRUCT(struct sli_bgapi_trace_metadata_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t header;

  /** Data field for trace message_metadata event*/
  sl_bgapi_debug_evt_trace_message_metadata_t  evt_trace_message_metadata;
});

/**
 * @brief Data structure for BGAPI header and the custom log message event
 */
PACKSTRUCT(struct sli_bgapi_trace_custom_message_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t header;

  /** Data field for trace custom_message event*/
  sl_bgapi_debug_evt_trace_custom_message_t  evt_trace_custom_message;
});

/**
 * @brief Data structure for BGAPI header and the sync event
 */
PACKSTRUCT(struct sli_bgapi_trace_sync_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t header;

  /** Data field for trace sync event*/
  sl_bgapi_debug_evt_trace_sync_t  evt_trace_sync;
});

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/// Set to true when we've successfully initialized BGAPI tracing
static bool bgapi_trace_initialized = false;

/// Set to true when BGAPI tracing has been started
static volatile bool bgapi_trace_started = false;

/// The RTT buffer used for BGAPI trace
static uint8_t bgapi_trace_rtt_buffer[SL_BGAPI_TRACE_RTT_BUFFER_SIZE];

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/**
 * @brief Get a microsecond timestamp
 *
 * @return A monotonically increasing number of microseconds since boot
 */
static uint64_t get_timestamp_us(void)
{
  // To achieve sub-millisecond resolution for the timestamp using the
  // conversion to milliseconds, we multiply the tick count by 1000. The 64-bit
  // value will not wrap around in realistic time.
  uint64_t ticks = sl_sleeptimer_get_tick_count64();
  uint64_t timestamp_us = 0;
  sl_status_t status = sl_sleeptimer_tick64_to_ms(ticks * 1000, &timestamp_us);
  if (status != SL_STATUS_OK) {
    // Use zero timestamp when failed. The BGAPI trace tool will detect zero as
    // a missing timestamp.
    return 0;
  }
  return timestamp_us;
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize BGAPI tracing.
 ******************************************************************************/
void sli_bgapi_trace_init(void)
{
  // Initialize only when we haven't already
  if (!bgapi_trace_initialized) {
    // Configure the specified buffer. Status >= 0 means success.
    int config_status = SEGGER_RTT_ConfigUpBuffer(SL_BGAPI_TRACE_RTT_BUFFER_INDEX,
                                                  "sl_bgapi_trace",
                                                  bgapi_trace_rtt_buffer,
                                                  sizeof(bgapi_trace_rtt_buffer),
                                                  SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    if (config_status >= 0) {
      bgapi_trace_initialized = true;

      // Unless auto-start is specifically disabled, BGAPI Trace is
      // automatically started at init time
#ifndef SLI_BGAPI_TRACE_DISABLE_AUTO_START
      bgapi_trace_started = true;
#endif
    }
  }
}

/***************************************************************************//**
 * Output a BGAPI message to the trace channel.
 ******************************************************************************/
void sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_t type,
                                    uint32_t header,
                                    const void *data)
{
  CORE_DECLARE_IRQ_STATE;

  // If BGAPI Trace is not started, exit immediately
  if (!bgapi_trace_started) {
    return;
  }

  // Metadata is output only when enabled by the configuration
#if SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE
  uint64_t timestamp_us = get_timestamp_us();
  size_t payload_size = sizeof(sl_bgapi_debug_evt_trace_message_metadata_t);
  struct sli_bgapi_trace_metadata_msg metadata_msg = {
    .header = SL_BGAPI_MSG_HEADER_FROM_ID_AND_LEN(sl_bgapi_debug_evt_trace_message_metadata_id, payload_size),
    .evt_trace_message_metadata = {
      .type = (uint8_t) type,
      .timestamp_us = timestamp_us
    }
  };
#endif // SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE

  // We need to make multiple writes to RTT and those need to be atomic
  CORE_ENTER_ATOMIC();

#if SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE
  // Write the metadata event to RTT
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, &metadata_msg, sizeof(metadata_msg));
#endif // SL_BGAPI_TRACE_MESSAGE_METADATA_ENABLE

  // Write both the header and the message to the RTT buffer
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, &header, sizeof(header));
  uint32_t data_len = SL_BGAPI_MSG_LEN(header);
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, data, data_len);

  CORE_EXIT_ATOMIC();
}

/***************************************************************************//**
 * Output a custom log message to the trace channel.
 ******************************************************************************/
size_t sli_bgapi_trace_log_custom_message(const void *buffer,
                                          size_t buffer_length)
{
  CORE_DECLARE_IRQ_STATE;

  // If BGAPI Trace is not started, exit immediately
  if (!bgapi_trace_started) {
    return 0;
  }

  // The maximum length of a custom message is limited by what we can fit in the
  // `uint8_t` length field. Truncate the supplied buffer to the maximum length
  // if we need to.
  if (buffer_length > UINT8_MAX) {
    buffer_length = UINT8_MAX;
  }

  // Construct the header
  uint64_t timestamp_us = get_timestamp_us();
  size_t payload_size = sizeof(sl_bgapi_debug_evt_trace_custom_message_t) + buffer_length;
  struct sli_bgapi_trace_custom_message_msg custom_msg = {
    .header = SL_BGAPI_MSG_HEADER_FROM_ID_AND_LEN(sl_bgapi_debug_evt_trace_custom_message_id, payload_size),
    .evt_trace_custom_message = {
      .timestamp_us = timestamp_us,
      .message = { .len = (uint8_t) buffer_length }
    }
  };

  // Make two atomic writes to RTT to write the header and the message itself
  CORE_ENTER_ATOMIC();
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, &custom_msg, sizeof(custom_msg));
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, buffer, buffer_length);
  CORE_EXIT_ATOMIC();

  return buffer_length;
}

/***************************************************************************//**
 * Start the BGAPI Trace.
 ******************************************************************************/
void sli_bgapi_trace_start(void)
{
  bgapi_trace_started = true;
}

/***************************************************************************//**
 * Stop the BGAPI Trace.
 ******************************************************************************/
void sli_bgapi_trace_stop(void)
{
  bgapi_trace_started = false;
}

/***************************************************************************//**
 * Synchronize BGAPI Trace with the host.
 ******************************************************************************/
void sli_bgapi_trace_sync(void)
{
  uint64_t timestamp_us = get_timestamp_us();
  size_t payload_size = sizeof(sl_bgapi_debug_evt_trace_sync_t);
  struct sli_bgapi_trace_sync_msg sync_msg = {
    .header = SL_BGAPI_MSG_HEADER_FROM_ID_AND_LEN(sl_bgapi_debug_evt_trace_sync_id, payload_size),
    .evt_trace_sync = {
      .timestamp_us = timestamp_us
    }
  };
  SEGGER_RTT_Write(SL_BGAPI_TRACE_RTT_BUFFER_INDEX, &sync_msg, sizeof(sync_msg));
  unsigned int bytes_in_buffer;
  do {
    bytes_in_buffer = SEGGER_RTT_GetBytesInBuffer(SL_BGAPI_TRACE_RTT_BUFFER_INDEX);
  } while (bytes_in_buffer > 0);
}
