/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sl_component_catalog.h"
#include "app_init.h"
#include "app_process.h"
#include "simple_rail_assistance.h"
#include "sl_wmbus_support.h"
#include "sl_wmbus_packet_assembler.h"
#include "em_emu.h"

#if defined(SL_CATALOG_WMBUS_SENSOR_CORE_PRESENT)
#include "sl_wmbus_sensor_core.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

#include "rail_types.h"
#include "cmsis_compiler.h"
#include "sl_flex_rail_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Prepare and send the WMBus packet
 *
 * @param rail_handle: which rail handle to use for sending
 * @param length: length of the packet
 * @param send_at: absolute time when to send it
 * @return RAIL_Status_t: status of the transmission
 ******************************************************************************/
RAIL_Status_t send_packet_at(RAIL_Handle_t rail_handle, uint16_t length, uint64_t send_at);

/*******************************************************************************
 * @brief Handles the scheduling of a transmission.
 *
 * This function is responsible for managing the scheduling of a transmission
 * using the provided RAIL handle. It ensures that the transmission is properly
 * scheduled according to the application's requirements.
 *
 * @param rail_handle The RAIL handle used to manage the transmission.
 ******************************************************************************/
void handle_schedule_tx(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * @brief Handles the completion of a transmission.
 *
 * This function is called when a transmission is completed. It performs
 * necessary actions to process the completion of the transmission.
 ******************************************************************************/
void handle_tx_done(void);

/*******************************************************************************
 * @brief Handle the response delay .
 *
 * This function is responsible for managing the delay in responses for the
 * specified RAIL handle.
 *
 * @param rail_handle: The RAIL handle for which the response delay
 * is to be handled.
 ******************************************************************************/
void handle_response_delay(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * @brief Handle unlimited access.
 *
 * This function configures and schedules an RX window for unlimited access
 * using the RAIL library. It sets the start and end times for the RX window,
 * and enables sleep mode if the gap between RX and TX is sufficient.
 *
 * @param[in] rail_handle The RAIL handle.
 *****************************************************************************/
void handle_unlimited_access(RAIL_Handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Variables from app_init.c
extern uint16_t rx_channel;
extern uint8_t access_number;

/// Time for calculation for the proper sending timing
RAIL_Time_t last_tx_start_time = 0;
RAIL_Time_t last_tx_end_time = 0;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state
static volatile state_t state = S_SCHEDULE_TX;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0U;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = RAIL_STATUS_NO_ERROR;

/// Variable to allow to go to sleep
static volatile bool ok_to_sleep = true;

/// Last sent packet length, need for calculating the next one
static uint16_t last_tx_length = 0U;

/// Buffer to store in packets before sending
static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t tx_buffer[SL_FLEX_RAIL_TX_FIFO_SIZE];

/// WMBus specific parameters
static uint64_t wmbus_app_period_acc = 500e3;
static const uint32_t wmbus_app_period_nom = 10e6; //10s

/// Start the rx window earlier and close it later by this amount
static const uint32_t response_delay_safety_margin = 200U;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Set the next state for the process state machine
 * @param next_state: Which state will the sate machine enter next time
 ******************************************************************************/
void set_next_state(state_t next_state)
{
  state = next_state;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;
  switch (state) {
    case S_SCHEDULE_TX:
      handle_schedule_tx(rail_handle);
      break;
    case S_TX_DONE:
      handle_tx_done();
      break;
    case S_RESPONSE_DELAY:
      handle_response_delay(rail_handle);
      break;
    case S_UNLIMITED_ACCESS:
      handle_unlimited_access(rail_handle);
      break;
    //in idle states (like S_RX, we need the main oscillator, but waiting for an interrupt. EM1 is safe)
    case S_IDLE_OR_RX:
      // go to sleep or back to rx listening
      break;
    default:
      // unknown state
      break;
  }
}

/******************************************************************************
 * Manages the power_manager_sleep in main.c
 *****************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return ok_to_sleep;
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if ( events & RAIL_EVENT_TX_STARTED) {
    RAIL_GetTxTimePreambleStart(rail_handle, RAIL_TX_STARTED_BYTES, &last_tx_start_time);
  }

  if ( events & RAIL_EVENTS_TX_COMPLETION ) {
    if ( events & RAIL_EVENT_TX_PACKET_SENT ) {
      RAIL_GetTxPacketDetailsAlt(rail_handle, false, &last_tx_end_time);
      RAIL_GetTxTimeFrameEnd(rail_handle, last_tx_length, &last_tx_end_time);
    }
    state = S_TX_DONE;
  }

  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      toggle_receive_led();
    }
    switch (get_wmbus_accessibility()) {
      case WMBUS_ACCESSIBILITY_LIMITED_ACCESS:
        state = S_SCHEDULE_TX;
        break;
      case WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS:
        state = S_IDLE_OR_RX;
        break;
      default:
        state = S_IDLE_OR_RX;
    }
  }

  if ( events & RAIL_EVENT_RX_SCHEDULED_RX_END ) {
    state = S_SCHEDULE_TX;
  }

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * @brief Handles the scheduling of a transmission.
 *
 * This function is responsible for managing the scheduling of a transmission
 * using the provided RAIL handle. It ensures that the transmission is properly
 * scheduled according to the application's requirements.
 *
 * @param rail_handle The RAIL handle used to manage the transmission.
 ******************************************************************************/
void handle_schedule_tx(RAIL_Handle_t rail_handle)
{
  uint16_t length = 0;
  RAIL_Status_t rail_status;
  memset(tx_buffer, 0, SL_FLEX_RAIL_TX_FIFO_SIZE);
#if defined(SL_CATALOG_WMBUS_SENSOR_CORE_PRESENT)
  length = sl_wmbus_sensor_core_process(tx_buffer, &access_number);
#else
  uint32_t data = 1234;
  sl_rail_sdk_wmbus_sensor_data_t sensors_data = {
    .data = 1234,
    .vif = 0x17,
    .next = NULL
  };
  length = sl_wmbus_setup_frame(tx_buffer, *access_number, get_wmbus_accessibility(), WMBUS_DEVICE_WATER_METER, &sensors_data, true, true);
#endif
  // TX is scheduled, sleep can be enabled as RAIL deals with power manager
  rail_status = send_packet_at(rail_handle, length, last_tx_start_time + wmbus_app_period_acc);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    // TX failed, schedule the next TX immediately
    last_tx_start_time = RAIL_GetTime();
    state = S_SCHEDULE_TX;
  } else {
    // TX is scheduled, sleep can be enabled as RAIL deals with power manager
    ok_to_sleep = true;
    state = S_IDLE_OR_RX;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * @brief Handles the completion of a transmission.
 *
 * This function is called when a transmission is completed. It performs
 * necessary actions to process the completion of the transmission.
 ******************************************************************************/
void handle_tx_done(void)
{
  toggle_send_led();
  //let's setup the time for the next tx.
  /*
   * from EN13757-4:
   * tACC=1+(|nACC-128|-64)/2048*tNOM
   * a bit easier to calculate:
   * tACC=(2048+|nACC-128|-64)*tNOM/2048
   */
  access_number++;
  wmbus_app_period_acc = access_number > 128 ? access_number - 128 : 128 - access_number;
  wmbus_app_period_acc = (2048 + wmbus_app_period_acc - 64) * (wmbus_app_period_nom / 2048);
  switch (get_wmbus_accessibility()) {
    case WMBUS_ACCESSIBILITY_LIMITED_ACCESS:
      state = S_RESPONSE_DELAY;
      break;
    case WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS:
      state = S_UNLIMITED_ACCESS;
      break;
    // no RX between TXes, schedule the next TX immediately
    default:
      state = S_SCHEDULE_TX;
      break;
  }
  ok_to_sleep = false;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * @brief Handle the response delay .
 *
 * This function is responsible for managing the delay in responses for the specified RAIL handle.
 *
 * @param rail_handle: The RAIL handle for which the response delay is to be handled.
 ******************************************************************************/
void handle_response_delay(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;
  RAIL_ScheduleRxConfig_t schedule = {
    .start = last_tx_end_time + WMBUS_getMeterLimitedAccRxStart(false) - response_delay_safety_margin,
    .startMode = RAIL_TIME_ABSOLUTE,
    .end = last_tx_end_time + WMBUS_getMeterLimitedAccRxStop(false) + response_delay_safety_margin,
    .endMode = RAIL_TIME_ABSOLUTE,
    .hardWindowEnd = 0,
  };
  rail_status = RAIL_ScheduleRx(rail_handle, rx_channel, &schedule, NULL);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    // RX failed, schedule the next TX immediately
    state = S_SCHEDULE_TX;
  } else {
    // RX is scheduled, sleep can be enabled as RAIL deals with power manager
    ok_to_sleep = true;
    state = S_IDLE_OR_RX;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/**************************************************************************//**
 * @brief Handle unlimited access.
 *
 * This function configures and schedules an RX window for unlimited access
 * using the RAIL library. It sets the start and end times for the RX window,
 * and enables sleep mode if the gap between RX and TX is sufficient.
 *
 * @param[in] rail_handle The RAIL handle.
 *****************************************************************************/
void handle_unlimited_access(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t rail_status;
  RAIL_ScheduleRxConfig_t schedule = {
    .start =  last_tx_end_time + WMBUS_getMeterLimitedAccRxStart(false) - response_delay_safety_margin,
    .startMode = RAIL_TIME_ABSOLUTE,
    .end = (uint32_t)(last_tx_end_time + wmbus_app_period_acc - 2000),
    .endMode = RAIL_TIME_ABSOLUTE,
    .rxTransitionEndSchedule = 0,
    .hardWindowEnd = 0,
  };
  // RX is scheduled, sleep can be enabled as RAIL deals with power manager
  // although, if the gap between RX and TX is short enough, the device will
  // not have time to go to sleep
  rail_status = RAIL_ScheduleRx(rail_handle, rx_channel, &schedule, NULL);

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    // RX failed, schedule the next TX immediately
    state = S_SCHEDULE_TX;
  } else {
    // RX is scheduled, sleep can be enabled as RAIL deals with power manager
    ok_to_sleep = true;
    state = S_IDLE_OR_RX;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Prepare and send the WMBus packet
 *
 * @param rail_handle: which rail handle to use for sending
 * @param length: length of the packet
 * @param send_at: absolute time when to send it
 * @return RAIL_Status_t: status of the transmission
 ******************************************************************************/
RAIL_Status_t send_packet_at(RAIL_Handle_t rail_handle, uint16_t length, uint64_t send_at)
{
  last_tx_length = WMBUS_phy_software(tx_buffer, (uint8_t) length, SL_FLEX_RAIL_TX_FIFO_SIZE);
  RAIL_SetTxFifo(rail_handle, tx_buffer, last_tx_length, SL_FLEX_RAIL_TX_FIFO_SIZE);
  if ( last_tx_length != length ) {
    //Only for Series 1 Mode T M2O
    RAIL_SetFixedLength(rail_handle, last_tx_length);
  } else {
    RAIL_SetFixedLength(rail_handle, RAIL_SETFIXEDLENGTH_INVALID);
  }
  RAIL_ScheduleTxConfig_t schedule = {
    .when = send_at,
    .mode = RAIL_TIME_ABSOLUTE,
  };
  return RAIL_StartScheduledTx(rail_handle, DEFAULT_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, &schedule, NULL);
}
