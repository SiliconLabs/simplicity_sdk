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

/** @brief Timeout
 *
 * This function is called by the Trust Center Keepalive plugin when the Trust
 * Center fails to acknowledge enough keepalive messages. Upon timing out,
 * the Trust Center Keepalive plugin code will initiate a search for a new
 * Trust Center, which falls in line with the Trust Center Swapout feature.
 * Applications may consume this callback and have it return true in order to
 * prevent the Trust Center Keepalive plugin code from issuing a Trust Center
 * (insecure) rejoin. Doing so will also give the application a chance to
 * implement its own rejoin algorithm or logic. If this callback returns false,
 * the Trust Center Keepalive plugin code will proceed with issuing a Trust
 * Center rejoin.
 */
WEAK(bool sl_zigbee_af_trust_center_keepalive_timeout_cb(void))
{
  return false;
}

/** @brief ConnectivityEstablished
 * This function is called by the Trust Center Keepalive plugin when a read
 * attribute response command from the trust center is received after trust
 * center connectivity had previously been lost. This function is also called
 * after a match descriptor response.
 */
WEAK(void sl_zigbee_af_trust_center_keepalive_connectivity_established_cb(void))
{
  return;
}

/** @brief Serverless Keep Alive Supported
 * This function is called by the Trust Center Keep Alive plugin when service
 * discovery receives a response indicating that the server does not support the
 * cluster.
 * Applications may consume this callback and have it return true in order to
 * have the Trust Center Keep Alive plugin code to still start the keep alive
 * process even if the server cluster is not discovered. If this callback returns
 * false, the Trust Center Keep Alive plugin code will only proceed to start the
 * process in case the service discovery was succesful.
 */
WEAK(bool sl_zigbee_af_trust_center_keepalive_serverless_is_supported_cb(void))
{
  return false;
}

/** @brief Overwrite Default Timing Parameters
 * This function is called by the Trust Center Keep Alive plugin when the Trust
 * Center Keep Alive process is started.
 * Applications may consume this callback and have it return true in order to
 * have the Trust Center Keep Alive plugin code to start the keep alive process
 * with timing parameters other than the default values. If this callback returns
 * false, the Trust Center Keep Alive plugin code will proceed to start with the
 * default timing parameters.
 *
 * @param baseTimeSeconds, base time for keep alive signalling to be set in seconds
 *
 * @param jitterTimeSeconds, jitter time for keep alive signalling to be set in seconds
 */
WEAK(bool sl_zigbee_af_trust_center_keepalive_overwrite_default_timing_cb(uint16_t *baseTimeSeconds, uint16_t *jitterTimeSeconds))
{
  return false;
}

/** @brief Serverless Keep Alive Enabled
 * This function is called by the Trust Center Keep Alive plugin when a service
 * discovery is done, a response has arrived and
 * sl_zigbee_af_trust_center_keepalive_serverless_is_supported_cb() returned true.
 * Applications may consume this callback and have it return true in order to
 * have the Trust Center Keep Alive plugin code to start the keep alive process
 * right away. If this callback returns false, the Trust Center Keep Alive plugin
 * code will return to the disabled state waiting to be enabled.
 */
WEAK(bool sl_zigbee_af_trust_center_keepalive_serverless_is_enabled_cb(void))
{
  return true;
}
