/***************************************************************************//**
 * @file
 * @brief Routines for handling endpoints the Comms Hub Function plugin.
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
#include "app/framework/util/af-main.h"
#include "app/framework/util/common.h"
#include "comms-hub-tunnel-endpoints.h"
#include "comms-hub-function-config.h"
#include "gbcs-device-log-config.h"

#define MAX_NUMBER_DEVICES            SL_ZIGBEE_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE_LOG_SIZE
#define MAX_DEVICE_TUNNEL_ENDPOINTS   3

typedef struct {
  uint16_t nodeId;
  uint8_t endpoint;
} sli_zigbee_af_tunnel_endpoint_map;

static sli_zigbee_af_tunnel_endpoint_map endpointMap[MAX_NUMBER_DEVICES];

#define DEFAULT_ADDRESS  0xFFFE

// Templated to local_data_init context.
void sl_zigbee_af_tunneling_endpoint_init(uint8_t init_level)
{
  (void)init_level;

  uint8_t i;
  for ( i = 0; i < MAX_NUMBER_DEVICES; i++ ) {
    endpointMap[i].nodeId = DEFAULT_ADDRESS;
    endpointMap[i].endpoint = INVALID_TUNNELING_ENDPOINT;
  }
}

// NOTE:  This function and file assume only 1 tunneling endpoint exists on each address.
// If devices will support more than 1 tunneling endpoint, an array of endpoints should
// be stored.
void sl_zigbee_af_add_tunneling_endpoint(uint16_t nodeId, uint8_t *endpointList, uint8_t numEndpoints)
{
  uint8_t i;
  uint8_t firstUnused = MAX_NUMBER_DEVICES;

  if ( numEndpoints > 0 ) {
    for ( i = 0; i < MAX_NUMBER_DEVICES; i++ ) {
      if ( endpointMap[i].nodeId == nodeId ) {
        return;
      } else if ( (endpointMap[i].nodeId == DEFAULT_ADDRESS) && (firstUnused >= MAX_NUMBER_DEVICES) ) {
        firstUnused = i;
      }
    }
    if ( firstUnused < MAX_NUMBER_DEVICES ) {
      i = firstUnused;
      endpointMap[i].nodeId = nodeId;
      endpointMap[i].endpoint = endpointList[0];
    }
  }
}

uint8_t sl_zigbee_af_get_device_tunneling_endpoint(uint16_t nodeId)
{
  uint8_t i;
  uint8_t endpoint = INVALID_TUNNELING_ENDPOINT;
  for ( i = 0; i < MAX_NUMBER_DEVICES; i++ ) {
    if ( endpointMap[i].nodeId == nodeId ) {
      endpoint = endpointMap[i].endpoint;
      break;
    }
  }
  return endpoint;
}
