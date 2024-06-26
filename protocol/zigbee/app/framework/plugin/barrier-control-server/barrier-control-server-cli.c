/***************************************************************************//**
 * @file
 * @brief CLI for the Barrier Control Server plugin.
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

#include "app/util/serial/sl_zigbee_command_interpreter.h"

#include "app/framework/plugin/barrier-control-server/barrier-control-server.h"

static bool getBarrierPositionDescription(uint8_t endpoint,
                                          const char **descriptionLocation)
{
  bool descriptionWasSet = true;
  switch (sli_zigbee_af_barrier_control_server_get_barrier_position(endpoint)) {
    case SL_ZIGBEE_ZCL_BARRIER_CONTROL_BARRIER_POSITION_OPEN:
      *descriptionLocation = "open";
      break;
    case SL_ZIGBEE_ZCL_BARRIER_CONTROL_BARRIER_POSITION_CLOSED:
      *descriptionLocation = "closed";
      break;
    default:
      if (sli_zigbee_af_barrier_control_server_is_partial_barrier_supported(endpoint)) {
        descriptionWasSet = false;
      } else {
        *descriptionLocation = "unknown";
      }
  }
  return descriptionWasSet;
}

static void printSafetyStatus(uint16_t safetyStatus)
{
  sl_zigbee_af_app_print("SafetyStatus: 0x%2X (", safetyStatus);
  if (safetyStatus != 0) {
    if (READBITS(safetyStatus,
                 SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_REMOTE_LOCKOUT)) {
      sl_zigbee_af_app_print("lockout,");
    }
    if (READBITS(safetyStatus,
                 SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_TEMPER_DETECTED)) {
      sl_zigbee_af_app_print("tamper,");
    }
    if (READBITS(safetyStatus,
                 SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_FAILED_COMMUNICATION)) {
      sl_zigbee_af_app_print("communication,");
    }
    if (READBITS(safetyStatus,
                 SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_POSITION_FAILURE)) {
      sl_zigbee_af_app_print("position,");
    }
  } else {
    sl_zigbee_af_app_print("OK");
  }
  sl_zigbee_af_app_println(")");
}
// plugin barrier-control-server status <endpoint:1>
void sli_zigbee_af_barrier_control_server_status_command(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t barrierPosition
    = sli_zigbee_af_barrier_control_server_get_barrier_position(endpoint);
  const char *description;
  sl_zigbee_af_app_print("BarrierPosition: %d%%", barrierPosition);
  if (getBarrierPositionDescription(endpoint, &description)) {
    sl_zigbee_af_app_print(" (%s)", description);
  }
  sl_zigbee_af_app_println("");
  printSafetyStatus(sli_zigbee_af_barrier_control_server_get_safety_status(endpoint));
}

// plugin barrier-control-server open <endpoint:1>
// plugin barrier-control-server close <endpoint:1>
void sli_zigbee_af_barrier_control_server_open_or_close_command(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  bool open = memcmp(arguments->argv[arguments->arg_ofs - 1], "open", strlen("open")) == 0;
  uint8_t barrierPosition
    = (open
       ? SL_ZIGBEE_ZCL_BARRIER_CONTROL_BARRIER_POSITION_OPEN
       : SL_ZIGBEE_ZCL_BARRIER_CONTROL_BARRIER_POSITION_CLOSED);
  sli_zigbee_af_barrier_control_server_set_barrier_position(endpoint, barrierPosition);
  sli_zigbee_af_barrier_control_server_increment_events(endpoint, open, false);

  const char *description;
  assert(getBarrierPositionDescription(endpoint, &description));
  sl_zigbee_af_app_println("Barrier is now %s", description);
}

// plugin barrier-control-server remote-lockout <endpoint:1> <setOrClear:1>
// plugin barrier-control-server tamper-detected <endpoint:1> <setOrClear:1>
// plugin barrier-control-server failed-communication <endpoint:1> <setOrClear:1>
// plugin barrier-control-server position-failure <endpoint:1> <setOrClear:1>
void sli_zigbee_af_barrier_control_server_safety_status_command(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  bool doSet = sl_cli_get_argument_uint8(arguments, 1) == 1;
  uint8_t bit
    = (memcmp(arguments->argv[arguments->arg_ofs - 1], "remote-lockout", strlen("remote-lockout")) == 0
       ? SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_REMOTE_LOCKOUT
       : (memcmp(arguments->argv[arguments->arg_ofs - 1], "tamper-detected", strlen("tamper-detected")) == 0
          ? SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_TEMPER_DETECTED
          : (memcmp(arguments->argv[arguments->arg_ofs - 1], "failed-communication", strlen("failed-communication")) == 0
             ? SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_FAILED_COMMUNICATION
             : SL_ZIGBEE_AF_BARRIER_CONTROL_SAFETY_STATUS_POSITION_FAILURE)));

  uint16_t safetyStatus
    = sli_zigbee_af_barrier_control_server_get_safety_status(endpoint);
  if (doSet) {
    SETBITS(safetyStatus, bit);
  } else {
    CLEARBITS(safetyStatus, bit);
  }

  sl_zigbee_af_status_t status
    = sl_zigbee_af_write_server_attribute(endpoint,
                                          ZCL_BARRIER_CONTROL_CLUSTER_ID,
                                          ZCL_BARRIER_SAFETY_STATUS_ATTRIBUTE_ID,
                                          (uint8_t *)&safetyStatus,
                                          ZCL_BITMAP16_ATTRIBUTE_TYPE);
  assert(status == SL_ZIGBEE_ZCL_STATUS_SUCCESS);

  printSafetyStatus(safetyStatus);
}
