/***************************************************************************//**
 * @file
 * @brief this file adapts the form-and-join library to work on
 * an EZSP host processor.
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

#include PLATFORM_HEADER
#include "stack/include/sl_zigbee_types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/ezsp-host-configuration-defaults.h"
#include "form-and-join.h"
#include "form-and-join-adapter.h"

// Flat buffer for caching matched networks, channel energies, and random PAN
// ids.  The unused pan id code assumes a size of 32 bytes.  Each matching
// network consumes 16 to 20 bytes, depending on struct padding.
// The default size is set in ezsp-host-configuration-defaults.h and can
// be adjusted within the configuration header.
// The buffer is an uint16_t[] instead of an uint8_t[] in
// order to avoid alignment issues on the host.
static uint16_t data[EZSP_HOST_FORM_AND_JOIN_BUFFER_SIZE];

uint8_t formAndJoinStackProfile(void)
{
  uint16_t stackProfile = 2;  // Assume ZigBee Pro profile if the following call fails.
  sl_zigbee_ezsp_get_configuration_value(SL_ZIGBEE_EZSP_CONFIG_STACK_PROFILE, &stackProfile);
  return stackProfile;
}

sli_zigbee_network_info_t *formAndJoinGetNetworkPointer(uint8_t index)
{
  return ((sli_zigbee_network_info_t *) data) + index;
}

sl_status_t formAndJoinSetBufferLength(uint8_t entryCount)
{
  return (sizeof(data) < entryCount * sizeof(sli_zigbee_network_info_t)
          ? SL_STATUS_ALLOCATION_FAILED
          : SL_STATUS_OK);
}

void formAndJoinSetCleanupTimeout(void)
{
}

bool formAndJoinAllocateBuffer(void)
{
  return true;
}

void formAndJoinReleaseBuffer(void)
{
}
uint8_t*   getFormAndJoinBufferPointer(void)
{
  return (uint8_t *) data;
}
