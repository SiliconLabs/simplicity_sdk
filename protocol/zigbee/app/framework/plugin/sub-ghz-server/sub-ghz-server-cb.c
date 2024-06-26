/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

/** @brief Duty cycle state change callback for the use with the Sub-GHz plugin
 *
 * The Sub-GHz plugin monitors the duty cycle and calls this callback every time
 * the duty cycle state changes.
 *
 * @param channel   the channel that registered a Duty Cycle state change
 * @param curMode   current Duty Cycle mode
 * @param oldState  the previous Duty Cycle state
 * @param newState  the current Duty Cycle state
 *
 * @return  true to handle in the application, false for default behaviour
 *
 * The default response is sending the 'Suspend ZCL Messages' command, either by
 * unicast to the most offending client of a broadcast to all clients, depending
 * on the old and new duty cycle state.
 */
WEAK(bool sl_zigbee_af_sub_ghz_duty_cycle_cb(uint8_t channelPage,
                                             uint8_t channel,
                                             sl_zigbee_duty_cycle_state_t oldState,
                                             sl_zigbee_duty_cycle_state_t newState))
{
  (void)channelPage;    // unreferenced parameter
  (void)channel;        // unreferenced parameter
  (void)oldState;       // unreferenced parameter
  (void)newState;       // unreferenced parameter

  return false;         // let the framework to its job
}

/** @brief Callback to let the application handle the incoming
 * Mgmt_NWK_Unsolicited_Enhanced_Update_notify command
 *
 * 'Mgmt_NWK_Unsolicited_Enhanced_Update_notify' is a client command. The client
 * sends it to notify the server about the poor link quality. The server should
 * determine whether a channel change is necessary and change the channel if so.
 * There is no default action performed by the application framework. If not
 * handled by the application, no action is taken.
 *
 * @param channelPage ........ current channel page
 * @param channel ............ current channel
 * @param macTxUcastTotal .... total number of Mac Tx transaction attempts
 * @param macTxUcastFailures . total number of Mac Tx transaction failures
 * @param macTxUcastRetries .. total number of Mac Tx transaction retries
 * @param period ............. time in minutes over which macTxUcastXxx were measured
 */
WEAK(void sl_zigbee_af_sub_ghz_unsolicited_enhanced_update_notify_cb(uint8_t channelPage,
                                                                     uint8_t channel,
                                                                     uint16_t macTxUcastTotal,
                                                                     uint16_t macTxUcastFailures,
                                                                     uint16_t macTxUcastRetries,
                                                                     uint8_t period))
{
  (void)channelPage;            // unreferenced parameter
  (void)channel;                // unreferenced parameter
  (void)macTxUcastTotal;        // unreferenced parameter
  (void)macTxUcastFailures;     // unreferenced parameter
  (void)macTxUcastRetries;      // unreferenced parameter
  (void)period;                 // unreferenced parameter
}

/** @brief Sub-GHz Cluster Get Suspend ZCL Messages Status
 *
 * 'Get Suspend ZCL Messages Status' is a client command. The client can send it
 * to the server to query the status instead of waiting for the 'Suspend ZCL Messages'
 * command.
 *
 * The server's default action is sending the 'Suspend ZCL Messages' command
 * with the parameter indicating the remaining suspend time in minutes.
 *
 * @return  true to indicate the callback has been handled by the application,
 *          false to keep the default behaviour
 */
WEAK(bool sl_zigbee_af_sub_ghz_get_suspend_zcl_messages_status_cb(void))
{
  return false; // let the framework do its job
}
