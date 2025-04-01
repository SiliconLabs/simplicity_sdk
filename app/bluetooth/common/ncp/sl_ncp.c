/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include "sl_common.h"
#include "sl_core.h"
#include "sl_bluetooth.h"
#include "sl_status.h"
#include "sl_bt_ncp_transport.h"
#include "sl_ncp.h"
#include "sl_ncp_evt_filter.h"
#include "sl_component_catalog.h"
#include "app_timer.h"
#include "app_rta.h"
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
#include "sl_wake_lock.h"
#endif // SL_CATALOG_WAKE_LOCK_PRESENT
#if defined(SL_CATALOG_NCP_SEC_PRESENT)
#include "sl_ncp_sec.h"
#endif // SL_CATALOG_NCP_SEC_PRESENT
#if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
#include "btl_interface.h"
#endif // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT
#ifdef SL_CATALOG_APP_ASSERT_PRESENT
#include "app_assert.h"
#endif // SL_CATALOG_APP_ASSERT_PRESENT
#ifdef SL_CATALOG_BGAPI_TRACE_PRESENT
#include "sli_bgapi_trace.h"
#endif // SL_CATALOG_BGAPI_TRACE_PRESENT

void sli_bt_ncp_transport_usart_cancel_receive(void);

// Command buffer
typedef struct {
  uint16_t len;
  bool available;
  union {
    uint32_t header;
    uint8_t  buf[SL_NCP_CMD_BUF_SIZE];
  };
} cmd_t;

// Event buffer
typedef struct {
  uint16_t len;
  bool available;
  union {
    uint32_t header;
    uint8_t  buf[SL_NCP_EVT_BUF_SIZE];
  };
} evt_t;

// Timer states
typedef enum {
  CMD_IDLE = 0,
  CMD_RECEIVING,
  CMD_INCOMPLETE,
  CMD_RECEIVED
} cmd_state_t;

// Application Runtime Adaptor context
static app_rta_context_t ctx = APP_RTA_INVALID_CONTEXT;
static SL_ALIGN(4) cmd_t cmd SL_ATTRIBUTE_ALIGN(4) =
{
  0
};
static SL_ALIGN(4) evt_t evt SL_ATTRIBUTE_ALIGN(4) =
{
  0
};
static bool busy = false;

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
#define SEMAPHORE_MAX        63
// Sleep context to disable sleep prematurely
static union {
  uint8_t   data;               // Common data (interpret as a byte)
  struct {
    uint8_t wake_signal    : 1; // Wake signal flag
    uint8_t waiting_for_rx : 1; // Waiting for RX flag
    uint8_t semaphore      : 6; // Semaphore field
  }         fields;             // Field interpretation
} sleep_context;
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

#define MSG_GET_LEN(x) ((uint16_t)(SL_BT_MSG_LEN((x)->header) \
                                   + SL_BT_MSG_HEADER_LEN))

static bool is_user_command(uint32_t hdr);
static void handle_user_command(uint32_t hdr, void *data);

// Command and event buffer helper functions
static void cmd_enqueue(uint16_t len, uint8_t *data);
static void cmd_dequeue(void);
static void evt_enqueue(uint16_t len, uint8_t *data);
static void evt_dequeue(uint16_t len);

// Command and event helper functions
static inline bool cmd_is_available(void);
static inline void cmd_set_available(void);
static inline void cmd_clr_available(void);
static inline bool evt_is_available(void);
static inline void evt_set_available(void);
static inline void evt_clr_available(void);

// Timer handle and callback for command timeout.
static app_timer_t cmd_timer;
static void cmd_timer_cb1(app_timer_t *timer, void *data);
static void cmd_timer_cb2(const app_timer_t *timer, const void *data);

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
static inline void sleep_signal_all_set(void);
static inline void sleep_signal_flag_clr(void);
static inline void sleep_signal_wait_clr(void);
#endif // SL_CATALOG_WAKE_LOCK_PRESENT
// Runtime error handler
static void on_runtime_error(app_rta_error_t error,
                             sl_status_t     result);
// Step function
static void ncp_step(void);

// -----------------------------------------------------------------------------
// Public functions (API implementation)

/**************************************************************************//**
 * Ncp Initialization function.
 *****************************************************************************/
void sl_ncp_init(void)
{
  sl_status_t sc;

  // Clear all buffers
  cmd_dequeue();
  CORE_ATOMIC_SECTION(evt_dequeue(evt.len); ) // Ensures (evt.len == len)

  busy = false;
  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  sleep_context.data = 0;
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT

  app_rta_config_t config = {
    .requirement.runtime = true,
    .requirement.guard   = true,
    .requirement.signal  = false,
    .requirement.queue   = false,
    .step                = ncp_step,
    .priority            = SL_NCP_TASK_PRIO,
    .stack_size          = SL_NCP_TASK_STACK,
    .error               = on_runtime_error,
    .wait_for_guard      = SL_NCP_WAIT_FOR_GUARD
  };
  sc = app_rta_create_context(&config, &ctx);
  if (sc != SL_STATUS_OK) {
    on_runtime_error(APP_RTA_ERROR_RUNTIME_INIT_FAILED, sc);
  }
}

void sl_ncp_rta_ready(void)
{
  // Start reception
  sl_bt_ncp_transport_receive();
}

/**************************************************************************//**
 * Local event processor.
 *
 * Use this function to process Bluetooth stack events locally on NCP.
 * Set the return value to true, if the event should be forwarded to the
 * NCP-host. Otherwise, the event is discarded locally.
 * Implement your own function to override this default weak implementation.
 *
 * @param[in] evt The event.
 *
 * @return true, if we shall send the event to the host. This is the default.
 *
 * @note Weak implementation.
 *****************************************************************************/
SL_WEAK bool sl_ncp_local_evt_process(sl_bt_msg_t *evt)
{
  (void)evt;
  return true;
}

SL_WEAK void sl_ncp_on_error(sl_ncp_error_t error, sl_status_t status)
{
  #ifdef SL_CATALOG_APP_ASSERT_PRESENT
  app_assert_status_f(status, "NCP: Error %u occurred.", error);
  #endif // SL_CATALOG_APP_ASSERT_PRESENT
  (void)error;
  (void)status;
}

#if defined(SL_CATALOG_BTMESH_PRESENT)

/**************************************************************************//**
 * Local event processor.
 *
 * Use this function to process Bluetooth Mesh stack events locally on NCP.
 * Set the return value to true, if the event should be forwarded to the
 * NCP-host. Otherwise, the event is discarded locally.
 * Implement your own function to override this default weak implementation.
 *
 * @param[in] evt The event.
 *
 * @return true, if we shall send the event to the host. This is the default.
 *
 * @note Weak implementation.
 *****************************************************************************/
SL_WEAK bool sl_ncp_local_btmesh_evt_process(sl_btmesh_msg_t *evt)
{
  (void)evt;
  return true;
}

#endif

/**************************************************************************//**
 * User command (message_to_target) handler callback.
 *
 * @note Weak implementation.
 *****************************************************************************/
SL_WEAK void sl_ncp_user_cmd_message_to_target_cb(void *data)
{
  (void)data;
}

/**************************************************************************//**
 * User CS command (message_to_target) handler callback.
 *
 * @note Weak implementation.
 *****************************************************************************/
SL_WEAK void sl_ncp_user_cs_cmd_message_to_target_cb(const void *data)
{
  (void)data;
}

/**************************************************************************//**
 * Send user command (message_to_target) response.
 *****************************************************************************/
void sl_ncp_user_cmd_message_to_target_rsp(sl_status_t result,
                                           uint8_t len,
                                           uint8_t *data)
{
  sl_bt_send_rsp_user_message_to_target(result, len, data);
}

/**************************************************************************//**
 * Send user event (message_to_host).
 *****************************************************************************/
void sl_ncp_user_evt_message_to_host(uint8_t len, uint8_t *data)
{
  sl_bt_send_evt_user_message_to_host(len, data);
}

/******************************************************************************
 * Check for user commands.
 *****************************************************************************/
static bool is_user_command(uint32_t hdr)
{
  switch (SL_BT_MSG_ID(hdr)) {
    // The command IDs listed here have to be in sync with the command IDs in
    // the handle_user_command function.
    case sl_bt_cmd_user_message_to_target_id:
    case sl_bt_cmd_user_manage_event_filter_id:
    case sl_bt_cmd_user_reset_to_dfu_id:
    case sl_bt_cmd_user_cs_service_message_to_target_id:
      return true;
    default:
      return false;
  }
}

/**************************************************************************//**
 * Handle a user API command in binary format.
 *
 * @param[in] hdr the command header.
 * @param[in] data the command payload in a byte array
 *****************************************************************************/
static void handle_user_command(uint32_t hdr, void *data)
{
#ifdef SL_CATALOG_BGAPI_TRACE_PRESENT
  sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_command, hdr, data);
#endif // SL_CATALOG_BGAPI_TRACE_PRESENT
  switch (SL_BT_MSG_ID(hdr)) {
    // -------------------------------
    case sl_bt_cmd_user_message_to_target_id:
      if (((uint8array *)data)->len > 0) {
        sl_ncp_user_cmd_message_to_target_cb(data);
      } else {
        sl_bt_send_rsp_user_message_to_target(SL_STATUS_FAIL, 0, NULL);
      }
      break;
    // -------------------------------
    case sl_bt_cmd_user_manage_event_filter_id:
      if (((uint8array *)data)->len > 0) {
        sl_ncp_evt_filter_handler((user_cmd_manage_event_filter_t *)data);
      } else {
        sl_bt_send_rsp_user_manage_event_filter(SL_STATUS_FAIL);
      }
      break;
    // -------------------------------
    case sl_bt_cmd_user_reset_to_dfu_id:
      #if defined(SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT)
      bootloader_rebootAndInstall();
      #endif // SL_CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT
      break;
    // -------------------------------
    case sl_bt_cmd_user_cs_service_message_to_target_id:
      if (((uint8array *)data)->len > 0) {
        sl_ncp_user_cs_cmd_message_to_target_cb(data);
      } else {
        sl_bt_send_rsp_user_cs_service_message_to_target(SL_STATUS_FAIL, 0, NULL);
      }
      break;
    // -------------------------------
    default:
      break;
  }
#ifdef SL_CATALOG_BGAPI_TRACE_PRESENT
  sl_bt_msg_t *response = sl_bt_get_command_response();
  sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_response, response->header, response->data.payload);
#endif // SL_CATALOG_BGAPI_TRACE_PRESENT
}

// -----------------------------------------------------------------------------
// Functions used for communication between stack and Uart

/**************************************************************************//**
 * Bluetooth stack event handler.
 *
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  // Acquire guard
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc == SL_STATUS_OK) {
    if (!sl_ncp_evt_filter_is_filtered(SL_BT_MSG_ID(evt->header))
        && sl_ncp_local_common_evt_process(evt)) {
      // Enqueue event
      evt_enqueue(MSG_GET_LEN(evt),
                  (uint8_t *)evt);
      (void)app_rta_proceed(ctx);
    }
    // Release guard
    (void)app_rta_release(ctx);
  } else {
    on_runtime_error(APP_RTA_ERROR_ACQUIRE_FAILED, sc);
  }
}

#if defined(SL_CATALOG_BTMESH_PRESENT)

/**************************************************************************//**
 * Bluetooth mesh stack event handler.
 *
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth Mesh stack.
 *****************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  // Acquire guard
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc == SL_STATUS_OK) {
    if (!sl_ncp_evt_filter_is_filtered((uint32_t)SL_BT_MSG_ID(evt->header))) {
      if (sl_ncp_local_common_btmesh_evt_process(evt)) {
        // Enqueue event
        evt_enqueue(MSG_GET_LEN(evt),
                    (uint8_t *)evt);
        (void)app_rta_proceed(ctx);
      }
    }
    // Release guard
    (void)app_rta_release(ctx);
  } else {
    on_runtime_error(APP_RTA_ERROR_ACQUIRE_FAILED, sc);
  }
}
#endif

/**************************************************************************//**
 * Signal if ncp application can process a new received event.
 *
 * Called before putting event into event buffer.
 *
 * @param[in] len Message length
 *
 * @return True if message fits into buffer, otherwise false.
 *****************************************************************************/
static inline bool sl_ncp_can_process_event(uint32_t len)
{
  bool ret = false;
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc != SL_STATUS_OK) {
    return ret;
  }
  // event fits into event buffer; otherwise don't pop it from queue
  if ((len <= (uint32_t)(sizeof(evt.buf) - evt.len)) && !evt_is_available()
      && !cmd_is_available()) {
    ret = true;
  }
  // Release guard
  (void)app_rta_release(ctx);
  return ret;
}

/**************************************************************************//**
 * Signal if bluetooth stack can process a new received event.
 *
 * @param[in] len Message length
 *
 * @return True if message fits into buffer, otherwise false.
 *****************************************************************************/
bool sl_bt_can_process_event(uint32_t len)
{
  return sl_ncp_can_process_event(len);
}

#if defined(SL_CATALOG_BTMESH_PRESENT)
/**************************************************************************//**
 * Signal if bluetooth mesh stack can process a new received event.
 *
 * @param[in] len Message length
 *
 * @return True if message fits into buffer, otherwise false.
 *****************************************************************************/
bool sl_btmesh_can_process_event(uint32_t len)
{
  return sl_ncp_can_process_event(len);
}
#endif

/**************************************************************************//**
 * Transmit completed callback
 *
 * Called after transmission is finished.
 *
 * @param[in] status Status of the transmission
 *****************************************************************************/
void sl_bt_ncp_transport_on_transmit(sl_status_t status)
{
  (void)status;
  // Acquire guard
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc == SL_STATUS_OK) {
    #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
    // Signal other controller that it can go to sleep
    sl_wake_lock_clear_remote_req();

    // Command response completed, go to sleep if possible
    if (sleep_context.fields.semaphore > 0) {
      // Only decrease semaphore if a command was received before
      // i.e. not an event was sent out
      sleep_context.fields.semaphore--;
      if (sleep_context.data == 0) {
        // Go to sleep if signal is down and command answered
        sl_wake_lock_set_local();
      }
    }
    #endif // SL_CATALOG_WAKE_LOCK_PRESENT
    busy = false;
    (void)app_rta_proceed(ctx);
    // Release guard
    (void)app_rta_release(ctx);
  } else {
    on_runtime_error(APP_RTA_ERROR_ACQUIRE_FAILED, sc);
  }
}

/**************************************************************************//**
 * Receive completed callback
 *
 * Called after reception is finished. Called from ISR context.
 *
 * @param[in] status Status of the reception
 * @param[in] len Received message length
 * @param[in] data Data received
 *****************************************************************************/
void sl_bt_ncp_transport_on_receive(sl_status_t status, uint32_t len, uint8_t *data)
{
  (void)status;
  // Stop on communication error
  if (status != SL_STATUS_OK) {
    sl_ncp_on_error(SL_NCP_ERROR_RECEIVE, status);
  }
  // Acquire guard
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc == SL_STATUS_OK) {
    // Copy command into buffer
    cmd_enqueue((uint16_t)len, data);
    (void)app_rta_proceed(ctx);
    // Release guard
    (void)app_rta_release(ctx);
  } else {
    on_runtime_error(APP_RTA_ERROR_ACQUIRE_FAILED, sc);
  }
}

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
/**************************************************************************//**
 * Wake-up signal arrived from host.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void sl_wake_lock_set_req_rx_cb(void)
{
  // Other end set wake-up pin, so set signaling flag. Also signal that at least
  // one command needs to be received and processed before sleep.
  sleep_signal_all_set();
  sl_bt_send_system_awake();
}

/**************************************************************************//**
 * Go to sleep signal arrived from host.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void sl_wake_lock_clear_req_rx_cb(void)
{
  // Other end released pin, remove signaling flag
  sleep_signal_flag_clr();

  if (sleep_context.data == 0) {
    // Go to sleep if signal is down and command answered
    sl_wake_lock_set_local();
  }
}
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// -----------------------------------------------------------------------------
// Step function

static void ncp_step(void)
{
  // Acquire guard
  sl_status_t sc = app_rta_acquire(ctx);
  if (sc != SL_STATUS_OK) {
    on_runtime_error(APP_RTA_ERROR_ACQUIRE_FAILED, sc);
    (void)app_rta_proceed(ctx);
    return;
  }
  // -------------------------------
  // Command available and NCP not busy
  if (cmd_is_available() && !busy) {
    sl_bt_msg_t *command = (sl_bt_msg_t *)cmd.buf;
    sl_bt_msg_t *response;

    #if defined(SL_CATALOG_NCP_SEC_PRESENT)
    uint32_t result = 0;
    // store if cmd was encrypted during reception for further processing
    bool cmd_is_encrypted = sl_ncp_sec_is_encrypted(cmd.buf);
    result = sl_ncp_sec_command_handler(cmd.buf, sizeof(cmd.buf));
    if ((result & SL_NCP_SEC_CMD_PROCESS) == SL_NCP_SEC_CMD_PROCESS)
    #endif // SL_CATALOG_NCP_SEC_PRESENT
    {
      cmd_clr_available();
      sc = sl_bgapi_lock();
      if (sc != SL_STATUS_OK) {
        // Signal fatal error to the app, skip command processing afterwards
        sl_ncp_on_error(SL_NCP_ERROR_BGAPI_LOCK, SL_STATUS_FAIL);
        // Drop command from the buffer as locking error is already fatal
        cmd_dequeue();
        // Exit processing current command
        return;
      }
      // Check for user command
      if (is_user_command(command->header)) {
        handle_user_command(command->header, command->data.payload);
      } else {
        // Call Bluetooth API binary command handler
        sl_bt_handle_command(command->header, command->data.payload);
      }
    }
    #if defined(SL_CATALOG_NCP_SEC_PRESENT)
    if ((result & SL_NCP_SEC_EVT_PROCESS) == SL_NCP_SEC_EVT_PROCESS) {
      // Clear command buffer
      cmd_dequeue();
    } else if ((result & SL_NCP_SEC_RSP_PROCESS)
               == SL_NCP_SEC_RSP_PROCESS) {
      response = sl_ncp_sec_process_response(
        sl_bt_get_command_response(), cmd_is_encrypted);
    #else
    {
      response = sl_bt_get_command_response();
    #endif // SL_CATALOG_NCP_SEC_PRESENT
      busy = true;
      // Clear command buffer
      cmd_dequeue();
    #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
      // Wake up other controller
      sl_wake_lock_set_remote_req();
    #endif // SL_CATALOG_WAKE_LOCK_PRESENT
      // Transmit command response
      sl_bt_ncp_transport_transmit((uint32_t)(MSG_GET_LEN(response)),
                                   (uint8_t *)response);
      // Finally unlock the BGAPI to allow other commands to proceed
      sl_bgapi_unlock();
    }
  }

  // -------------------------------
  // Event available and NCP not busy
  if (evt_is_available() && !busy) {
    #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
    // Wake up other controller
    sl_wake_lock_set_remote_req();
    #endif // SL_CATALOG_WAKE_LOCK_PRESENT

    uint8_t *data_ptr = evt.buf;
    uint16_t msg_len = MSG_GET_LEN((sl_bt_msg_t*)data_ptr);

    if (msg_len != 0) {
      uint32_t tx_len = msg_len;
      #if defined(SL_CATALOG_NCP_SEC_PRESENT)
      // encrypt the outgoing event
      data_ptr = (uint8_t*)sl_ncp_sec_process_event((sl_bt_msg_t*)evt.buf);
      if (data_ptr == NULL) {
        sl_ncp_on_error(SL_NCP_ERROR_ENCRYPT, SL_STATUS_FAIL);
      }
      // refresh length as encrypted messages are always longer than original ones
      tx_len = MSG_GET_LEN((sl_bt_msg_t*)data_ptr);
      #endif // SL_CATALOG_NCP_SEC_PRESENT

      busy = true;
      // Transmit events
      sl_bt_ncp_transport_transmit(tx_len, data_ptr); // makes a copy of the msg!
    }
    // Clear event buffer
    evt_dequeue(msg_len);
  }

  (void)app_rta_release(ctx);
}

// -----------------------------------------------------------------------------
// Command and event buffer handling functions

/**************************************************************************//**
 * Put command to command buffer
 *
 * @param[in] len Command length
 * @param[in] data Command data
 *****************************************************************************/
static void cmd_enqueue(uint16_t len, uint8_t *data)
{
  sl_status_t sc;
  cmd_state_t state = CMD_IDLE;

  if (len == 0) {
    // Nothing to do if the receive has been cancelled by the transport layer RX timeout!
    return;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();

  if (cmd.len >= SL_BT_MSG_HEADER_LEN && len > (sizeof(cmd.buf) - cmd.len)) {
    len = ((uint16_t)SL_BT_MSG_LEN(cmd.header) + SL_BT_MSG_HEADER_LEN) - cmd.len;
  }

  // Command fits into command buffer; otherwise discard it.
  if (len <= (sizeof(cmd.buf) - cmd.len)) {
    memcpy((void *)&cmd.buf[cmd.len], (void *)data, (size_t)len);
    if (cmd.len == 0) {
      state = CMD_RECEIVING;
    }
    cmd.len += len;
    CORE_EXIT_ATOMIC();
    // Part of the command received, start timer to not get stuck.
    // Command header has been received.
    if (cmd.len >= SL_BT_MSG_HEADER_LEN) {
      // 4-byte header + len bytes payload
      uint16_t bt_msg_len = (uint16_t)SL_BT_MSG_LEN(cmd.header) + SL_BT_MSG_HEADER_LEN;
      if (cmd.len >= bt_msg_len) {
        // Command has been received entirely.
        cmd_set_available();
        state = CMD_RECEIVED;
  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
        // Command received fully, increase semaphore.
        if (sleep_context.fields.semaphore < SEMAPHORE_MAX) {
          sleep_context.fields.semaphore++;
        }
        // Also signal that a command was received, system can go to sleep after
        sleep_signal_wait_clr();
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT
      }
    } else if (state != CMD_RECEIVING) {
      state = CMD_INCOMPLETE;
    }
  } else {
    CORE_EXIT_ATOMIC();
    sl_ncp_on_error(SL_NCP_ERROR_RECEIVE, SL_STATUS_HAS_OVERFLOWED);
  }

  if (state == CMD_RECEIVED) {
    // Stop timer as the whole command was received. No problem if timer was
    // not started before, app_timer is prepared for that.
    sc = app_timer_stop(&cmd_timer);

    if (sc != SL_STATUS_OK) {
      sl_ncp_on_error(SL_NCP_ERROR_TIMER, sc);
    }
  } else if (state == CMD_RECEIVING) {
    // Start timer used for max waiting time of fragmented packets.
    sc = app_timer_start(&cmd_timer,
                         SL_NCP_CMD_TIMEOUT_MS,
                         cmd_timer_cb1, // first timeout will try finish the command from buffer
                         (void *)cmd_timer_cb2, // pass the emergency cb as parameter that will respond SL_STATUS_INCOMPLETE, otherwise
                         false);
    if (sc != SL_STATUS_OK) {
      sl_ncp_on_error(SL_NCP_ERROR_TIMER, sc);
    }
  }
}

/**************************************************************************//**
 * Clear command buffer
 *****************************************************************************/
static void cmd_dequeue(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  cmd.len = 0;
  cmd_clr_available();
  CORE_EXIT_ATOMIC();
}

/**************************************************************************//**
 * Put event to event buffer
 *
 * @param[in] len Event length
 * @param[in] data Event data
 *****************************************************************************/
static void evt_enqueue(uint16_t len, uint8_t *data)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  // event fits into event buffer; otherwise discard it
  if (len <= (sizeof(evt.buf) - evt.len)) {
    memcpy((void *)&evt.buf[evt.len], (void *)data, len);
    evt.len += len;
    evt_set_available();
    CORE_EXIT_ATOMIC();
  } else {
    CORE_EXIT_ATOMIC();
    // We could not fit an incoming event into the event queue
    // Increasing SL_NCP_EVT_BUF_SIZE may help if this ever happens

    switch (SL_BT_MSG_ID(((sl_bt_msg_t *)data)->header)) {
      case sl_bt_evt_scanner_legacy_advertisement_report_id:
      case sl_bt_evt_scanner_extended_advertisement_report_id:
        // The missed event is a scan response event.
        // Since these type of events are not mandatory, and tend to be
        // generated most densely, it can be ignored.
        break;

      default:
        // Handle error otherwise
        sl_ncp_on_error(SL_NCP_ERROR_EVT_ENQUE, SL_STATUS_WOULD_OVERFLOW);
        break;
    }
  }
}

/**************************************************************************//**
 * Clear event buffer
 *****************************************************************************/
static void evt_dequeue(uint16_t len)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (evt.len < len) {
    sl_ncp_on_error(SL_NCP_ERROR_EVT_DEQUE, SL_STATUS_INVALID_COUNT);
  } else if (evt.len == len) {
    evt.len = 0;
    evt_clr_available();
  } else {
    uint16_t remaining = evt.len - len;
    memmove(evt.buf, (void *)&evt.buf[len], remaining);
    evt.len = remaining;
  }
  CORE_EXIT_ATOMIC();
}

// -----------------------------------------------------------------------------
// Timer callback function

static void cmd_timer_cb1(app_timer_t *timer, void *data)
{
  // Try cancel receive first...
  sli_bt_ncp_transport_usart_cancel_receive();
  // Restart timer once more to wait for the packet to be finished from buffer
  (void)app_timer_start(timer,
                        SL_NCP_CMD_TIMEOUT_MS,
                        (app_timer_callback_t)data,
                        NULL,
                        false);
}

static void cmd_timer_cb2(const app_timer_t *timer, const void *data)
{
  (void)data;
  (void)timer;
  uint8_t rsp_buf[SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MSG_ERROR_PAYLOAD_LEN] = { 0 };
  sl_bt_msg_t *response = (sl_bt_msg_t *)rsp_buf;
  uint32_t cmd_hdr;

  // Clear missing bytes of header if it's also incomplete
  while (cmd.len < sizeof(cmd_hdr)) {
    cmd.buf[cmd.len++] = 0;
  }
  // prepare all known parts of the received header
  memcpy(&cmd_hdr, cmd.buf, sizeof(cmd_hdr));
  // Prepare the response for the other end that the command was received
  // only partially then send the response. The timer is already stopped.
  sl_bgapi_set_error_response(cmd_hdr,
                              (uint16_t) SL_STATUS_COMMAND_INCOMPLETE,
                              &rsp_buf,
                              sizeof(rsp_buf));

  cmd_dequeue();
#if defined(SL_CATALOG_NCP_SEC_PRESENT)
  response = sl_ncp_sec_process_response(response, SL_BT_MSG_ENCRYPTED(cmd_hdr));
#endif
  busy = true;
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  // Wake up other controller
  sl_wake_lock_set_remote_req();
#endif // SL_CATALOG_WAKE_LOCK_PRESENT
  // Transmit command error response
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
  sl_bt_ncp_transport_transmit((uint32_t)(MSG_GET_LEN(response)), rsp_buf);
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if SL_NCP_EMIT_SYSTEM_ERROR_EVT
  // Sending system error event as well to help app devs to give async response
  sl_bt_send_system_error(SL_STATUS_COMMAND_INCOMPLETE, sizeof(cmd_hdr), rsp_buf);
#endif // SL_NCP_EMIT_SYSTEM_ERROR_EVT
}

// -----------------------------------------------------------------------------
// Helper functions

/**************************************************************************//**
 * Get command availability in command buffer
 *****************************************************************************/
static inline bool cmd_is_available(void)
{
  return cmd.available;
}

/**************************************************************************//**
 * Set command available in command buffer
 *****************************************************************************/
static inline void cmd_set_available(void)
{
  cmd.available = true;
}

/**************************************************************************//**
 * Clear command available in command buffer
 *****************************************************************************/
static inline void cmd_clr_available(void)
{
  cmd.available = false;
}

/**************************************************************************//**
 * Get event availability in command buffer
 *****************************************************************************/
static inline bool evt_is_available(void)
{
  return evt.available;
}

/**************************************************************************//**
 * Set event available in command buffer
 *****************************************************************************/
static inline void evt_set_available(void)
{
  evt.available = true;
}

/**************************************************************************//**
 * Clear event available in command buffer
 *****************************************************************************/
static inline void evt_clr_available(void)
{
  evt.available = false;
}

// Error callback
static void on_runtime_error(app_rta_error_t error,
                             sl_status_t     result)
{
  (void)error;
  sl_ncp_on_error(SL_NCP_ERROR_RUNTIME, result);
}

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)

/**************************************************************************//**
 * Set wake signal flag and wait for rx cmd flag state
 *****************************************************************************/
static inline void sleep_signal_all_set(void)
{
  sleep_context.fields.wake_signal    = true;
  sleep_context.fields.waiting_for_rx = true;
}

/**************************************************************************//**
 * Clear wake signal flag state
 *****************************************************************************/
static inline void sleep_signal_flag_clr(void)
{
  sleep_context.fields.wake_signal = false;
}

/**************************************************************************//**
 * Clear wait for rx cmd flag state
 *****************************************************************************/
static inline void sleep_signal_wait_clr(void)
{
  sleep_context.fields.waiting_for_rx = false;
}
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

/*****************************************************************************
 * USART specific function, not needed for other transport layers.
 ****************************************************************************/
SL_WEAK void sli_bt_ncp_transport_usart_cancel_receive(void)
{
  // Empty weak implementation for non-USART transport layers.
}
