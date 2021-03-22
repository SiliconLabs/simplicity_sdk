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

#ifndef SLI_BGAPI_TRACE_H
#define SLI_BGAPI_TRACE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup sli_bgapi_trace BGAPI Trace
 * @{
 *
 * @brief BGAPI trace utility for tracing BGAPI commands and events
 *
 * The functions provided by this component are automatically called by the
 * BGAPI protocol implementation when a command is processed or an event is
 * delivered to the application. The application does not need to and must not
 * call these functions directly.
 *
 ******************************************************************************/

/**
 * @brief Specifies the full type of a BGAPI message.
 */
typedef enum {
  sli_bgapi_trace_message_type_command  = 0x0, /**< (0x0) The message is a BGAPI command */
  sli_bgapi_trace_message_type_response = 0x1, /**< (0x1) The message is a BGAPI response */
  sli_bgapi_trace_message_type_event    = 0x2  /**< (0x2) The message is a BGAPI event */
} sli_bgapi_trace_message_type_t;

/***************************************************************************//**
 * Initialize BGAPI tracing.
 ******************************************************************************/
void sli_bgapi_trace_init(void);

/***************************************************************************//**
 * Output a BGAPI message to the trace channel.
 *
 * @param[in]  type    The BGAPI message type
 * @param[in]  header  The BGAPI message header
 * @param[in]  data    The BGAPI message payload data
 ******************************************************************************/
void sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_t type,
                                    uint32_t header,
                                    const void *data);

/***************************************************************************//**
 * Start the BGAPI Trace.
 *
 * This function is not used by normal applications. In the default configuration
 * the BGAPI Trace will start automatically at application initialization time so
 * that all BGAPI messages get traced. To avoid the RTT writes from blocking the
 * application execution, something on the host computer side needs to read the
 * RTT buffer. This may be inconvenient in situations where the user would like
 * to use one application image for both a normal run where RTT is not read, and
 * for taking debug traces using BGAPI Trace.
 *
 * To faciliate using one image for both normal runs and for tracing, special
 * applications (typically for example a test application used in automated
 * testing) may define `SLI_BGAPI_TRACE_DISABLE_AUTO_START` as a global `#define`
 * macro when building the application. When that macro is defined, the BGAPI
 * Trace is not started automatically and the application can run normally even
 * without anything reading RTT. When the application wants to start tracing in
 * this configuration, it can do so by calling this function
 * sli_bgapi_trace_start().
 *
 * Use @ref sli_bgapi_trace_stop() to stop the BGAPI Trace if needed.
 ******************************************************************************/
void sli_bgapi_trace_start(void);

/***************************************************************************//**
 * Stop the BGAPI Trace.
 *
 * This function is not used by normal applications. See @ref
 * sli_bgapi_trace_start() for a description of special situations where this
 * function might be useful.
 ******************************************************************************/
void sli_bgapi_trace_stop(void);

/** @} end sli_bgapi_trace */

#ifdef __cplusplus
}
#endif

#endif // SLI_BGAPI_TRACE_H
