/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Steering plugin.
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
#include "app/util/ezsp/ezsp-enum.h"

/** @brief EZSP Error Handler
 *
 * This callback is fired when the host process receives an error from the EZSP
 * link when talking to the NCP. The return bool gives the user application
 * the option to reboot the NCP. If this function returns true, the NCP will be
 * rebooted and the connection between the host and NCP will drop. If not, the
 * NCP will continue operating.
 *
 * @param status The sl_zigbee_ezsp_status_t error code received.
 *
 * @return bool True to reset NCP, false not to.
 *
 * @note This callback is only fired on the host application. It has no use for
 * SoC or NCP applications.
 */
WEAK(bool sl_zigbee_af_zcl_framework_core_ezsp_error_cb(sl_zigbee_ezsp_status_t status))
{
#if defined EZSP_HOST
  if (status == SL_ZIGBEE_EZSP_ERROR_OVERFLOW) {
    sl_zigbee_af_core_println("WARNING: the NCP has run out of buffers, causing "
                              "general malfunction. Remediate network congestion, if "
                              "present.");
    sl_zigbee_af_core_flush();
  }

  // Do not reset if this is a decryption failure, as we ignored the packet
  // Do not reset for a callback overflow or error queue, as we don't want the device to reboot
  // under stress;
  // Resetting under these conditions does not solve the problem as the
  // problem is external to the NCP. Throttling the additional traffic and staggering things
  // might make it better instead.
  // For all other errors, we reset the NCP
  if ((status != SL_ZIGBEE_EZSP_ERROR_SECURITY_PARAMETERS_INVALID)
      && (status != SL_ZIGBEE_EZSP_ERROR_OVERFLOW) && (status != SL_ZIGBEE_EZSP_ERROR_QUEUE_FULL)) {
    return true;
  }
#endif // EZSP_HOST
  return false;
}
