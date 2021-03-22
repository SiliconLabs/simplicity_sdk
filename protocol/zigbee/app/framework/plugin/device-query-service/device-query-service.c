/***************************************************************************//**
 * @file
 * @brief Device Query Service
 *
 * 1. After each device announce, add the device to a list of knonwn devices.
 * 2. Query the device for all Active Endpoints (Active Endpoint Request)
 * 3. Query each endpoint for its list of clusters and device information.
 *
 * Also periodically map the network to find all devices.
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
#include "app/framework/plugin/device-database/device-database.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

//============================================================================
// Globals
#include "device-query-service-config.h"
static sl_zigbee_af_event_t myEvent;
static void myEventHandler(sl_zigbee_af_event_t * event);
#if (SL_ZIGBEE_AF_PLUGIN_DEVICE_QUERY_SERVICE_AUTO_START == 1)
#define AUTO_START
#endif
static sl_802154_long_addr_t currentEui64;
static sl_802154_short_addr_t currentNodeId = SL_ZIGBEE_NULL_NODE_ID;
static uint8_t currentEndpointIndex;

#define DISCOVERY_DELAY_QS (5 << 2)

#define PLUGIN_NAME "Device-Query-Service"

#ifdef AUTO_START
  #define AUTO_START_BOOLEAN true
#else
  #define AUTO_START_BOOLEAN false
#endif
static bool enabled = AUTO_START_BOOLEAN;

//#define DEBUG_ON
#if defined(DEBUG_ON)
  #define debugPrintln(...) sl_zigbee_af_core_println(__VA_ARGS__)
  #define debugPrint(...) sl_zigbee_af_core_print(__VA_ARGS__)
  #define debugPrintEui64(eui64ToPrint) sl_zigbee_af_print_big_endian_eui64(eui64ToPrint)
#else
  #define debugPrintln(...)
  #define debugPrint(...)
  #define debugPrintEui64(eui64ToPrint)
#endif

// With device announce there is only the ZDO sequence number, there is no status code.
#define DEVICE_ANNOUNCE_NODE_ID_OFFSET 1
#define DEVICE_ANNOUNCE_EUI64_OFFSET   (DEVICE_ANNOUNCE_NODE_ID_OFFSET + 2)
#define DEVICE_ANNOUNCE_CAPABILITIES_OFFSET (DEVICE_ANNOUNCE_EUI64_OFFSET + EUI64_SIZE)

// Nice names for scheduleEvent()
#define RIGHT_NOW  false
#define WITH_DELAY true

#define RECEIVER_ON_WHEN_IDLE 0x08

// Bit mask in Node Descriptor response signifying which bits convey the
// compliance revision (bits 9-15)
#define SERVER_MASK_STACK_COMPLIANCE_REVISION_MASK          0xFE00
#define SERVER_MASK_STACK_COMPLIANCE_REVISION_BIT_POSITION  9

// We'll try sending up to 3 Node Descriptor requests in hopes of getting a resp
#define NODE_DESCRIPTOR_ATTEMPTS_MAX  3

static uint8_t gNodeDescriptorAttempts;

//============================================================================
// Forward declarations

static void sendActiveEndpointRequest(const sl_zigbee_af_device_info_t* device);
static void sendSimpleDescriptorRequest(const sl_zigbee_af_device_info_t* device);
static void sendNodeDescriptorRequest(const sl_zigbee_af_device_info_t* device);

//============================================================================

void sl_zigbee_af_device_query_service_get_current_discovery_target_eui64(sl_802154_long_addr_t returnEui64)
{
  memmove(returnEui64, currentEui64, EUI64_SIZE);
}

bool sl_zigbee_af_device_query_service_get_enabled_state(void)
{
  return enabled;
}

void sl_zigbee_af_device_query_service_enable_disable(bool enable)
{
  enabled = enable;
  sl_zigbee_af_event_set_inactive(&myEvent);
}

static void clearCurrentDevice(void)
{
  memset(currentEui64, 0xFF, EUI64_SIZE);
  currentNodeId = SL_ZIGBEE_NULL_NODE_ID;
}

void sl_zigbee_af_device_query_service_init_cb(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      sl_zigbee_af_network_event_init(&myEvent,
                                      myEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA:
    {
      debugPrintln("%p init called.", PLUGIN_NAME);
      clearCurrentDevice();
      break;
    }
  }
}

static void scheduleEvent(bool withDelay)
{
  if (withDelay) {
    debugPrintln("%p scheduled event for %d qs", PLUGIN_NAME, DISCOVERY_DELAY_QS);

    sl_zigbee_af_event_set_delay_qs(&myEvent, DISCOVERY_DELAY_QS);
  } else {
    sl_zigbee_af_event_set_active(&myEvent);
  }
}

bool sli_zigbee_af_device_query_pre_zdo_message_received(sl_802154_short_addr_t sender,
                                                         sl_zigbee_aps_frame_t* apsFrame,
                                                         uint8_t* message,
                                                         uint16_t length)
{
  sl_802154_long_addr_t tempEui64;
  const sl_zigbee_af_device_info_t *device;
  uint16_t serverMask;
  uint8_t stackRevision;

  if (!enabled) {
    return false;
  }

  if (apsFrame->clusterId == END_DEVICE_ANNOUNCE) {
    memmove(tempEui64, &(message[DEVICE_ANNOUNCE_EUI64_OFFSET]), EUI64_SIZE);
    // If the device already exists, this call won't overwrite it and will
    // leave its status alone.  Maybe it rejoined and we already know about,
    // in which case we won't bother re-interrogating it.
    device = sl_zigbee_af_device_database_add(tempEui64,
                                              message[DEVICE_ANNOUNCE_CAPABILITIES_OFFSET]);
    if (device == NULL) {
      sl_zigbee_af_core_print("Error: %p failed to add device to database: ",
                              PLUGIN_NAME);
      sl_zigbee_af_print_big_endian_eui64(tempEui64);
      sl_zigbee_af_core_println("");
    } else {
      if (device->status == SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_NEW) {
        sl_zigbee_af_core_print("%p added device to database: ", PLUGIN_NAME);
        sl_zigbee_af_print_big_endian_eui64(tempEui64);
        sl_zigbee_af_core_println(", capabilities: 0x%X", device->capabilities);
        scheduleEvent(WITH_DELAY);
      }
    }

    // returning true here will break the ias-zone-client.
    return false;
  } else if (apsFrame->clusterId == NODE_DESCRIPTOR_RESPONSE) {
    device = sl_zigbee_af_device_database_find_device_by_eui64(currentEui64);
    // This is state machine-driven so we must only process the Node Descriptor
    // response if we were expecting one
    if (device
        && (currentNodeId == sender)
        && (SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_STACK_REVISION == device->status)) {
      serverMask = message[12] | (message[13] << 8);
      serverMask &= SERVER_MASK_STACK_COMPLIANCE_REVISION_MASK;
      stackRevision = (serverMask
                       >> SERVER_MASK_STACK_COMPLIANCE_REVISION_BIT_POSITION);

      debugPrintln("%p got Node Descriptor response from 0x%2X",
                   PLUGIN_NAME,
                   currentNodeId);

      sli_zigbee_af_device_database_update_node_stack_revision(currentEui64,
                                                               stackRevision);

      // This is the last state - we're done
      sl_zigbee_af_device_database_set_status(currentEui64,
                                              SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_DONE);
      scheduleEvent(WITH_DELAY);
      clearCurrentDevice();
    }
  }
  return false;
}

static void noteFailedDiscovery(const sl_zigbee_af_device_info_t* device)
{
  sl_zigbee_af_device_database_set_status(device->eui64, SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FAILED);
  SL_ZIGBEE_TEST_ASSERT(0 == memcmp(device->eui64, currentEui64, EUI64_SIZE));
  clearCurrentDevice();
}

static void serviceDiscoveryCallback(const sl_zigbee_af_service_discovery_result_t* result)
{
  debugPrintln("%p serviceDiscoveryCallback()", PLUGIN_NAME);
  sl_zigbee_af_event_set_inactive(&myEvent);

  if (!enabled) {
    return;
  }

  const sl_zigbee_af_device_info_t* device = sl_zigbee_af_device_database_find_device_by_eui64(currentEui64);
  if (device == NULL) {
    sl_zigbee_af_core_print("Error:  %p could not find device in database with EUI64: ", PLUGIN_NAME);
    sl_zigbee_af_print_big_endian_eui64(currentEui64);
    sl_zigbee_af_core_println("");
    clearCurrentDevice();
    scheduleEvent(WITH_DELAY);
    return;
  }

  if (!sl_zigbee_af_have_discovery_response_status(result->status)) {
    sl_zigbee_af_core_println("Error: %p service discovery returned no results.", PLUGIN_NAME);
    noteFailedDiscovery(device);
    scheduleEvent(WITH_DELAY);
    return;
  } else if (result->zdoRequestClusterId == NETWORK_ADDRESS_REQUEST) {
    currentNodeId = result->matchAddress;
    sl_zigbee_af_device_database_set_status(device->eui64, SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS);
    scheduleEvent(RIGHT_NOW);
  } else if (result->zdoRequestClusterId == ACTIVE_ENDPOINTS_REQUEST) {
    const sl_zigbee_af_endpoint_list_t* listStruct = (const sl_zigbee_af_endpoint_list_t*)(result->responseData);
    debugPrintln("%p found %d active endpoints.", PLUGIN_NAME, listStruct->count);
    if (!sl_zigbee_af_device_database_set_endpoints(device->eui64,
                                                    listStruct->list,
                                                    listStruct->count)) {
      sl_zigbee_af_core_print("Error: %p failed to set endpoints in device database for:", PLUGIN_NAME);
      sl_zigbee_af_print_big_endian_eui64(device->eui64);
      sl_zigbee_af_core_println("");
      noteFailedDiscovery(device);
      scheduleEvent(WITH_DELAY);
    } else {
      currentEndpointIndex = 0;
      sl_zigbee_af_device_database_set_status(device->eui64, SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_CLUSTERS);
      scheduleEvent(RIGHT_NOW);
    }
  } else if (result->zdoRequestClusterId == SIMPLE_DESCRIPTOR_REQUEST) {
    const sl_zigbee_af_cluster_list_t* clusterInfo = (const sl_zigbee_af_cluster_list_t*)result->responseData;
    debugPrintln("%p found %d server clusters and %d client clusters on EP %d",
                 PLUGIN_NAME,
                 clusterInfo->inClusterCount,
                 clusterInfo->outClusterCount,
                 clusterInfo->endpoint);
    if (!sl_zigbee_af_device_database_set_clusters_for_endpoint(device->eui64,
                                                                clusterInfo)) {
      sl_zigbee_af_core_println("Error: %p failed to set clusters for device.", PLUGIN_NAME);
      noteFailedDiscovery(device);
      scheduleEvent(WITH_DELAY);
    } else {
      currentEndpointIndex++;
      scheduleEvent(RIGHT_NOW);
    }
  }
}

static void sendActiveEndpointRequest(const sl_zigbee_af_device_info_t* device)
{
  debugPrintln("%p initiating active endpoint request for: 0x%2X", PLUGIN_NAME, currentNodeId);
  sl_status_t status = sl_zigbee_af_find_active_endpoints(currentNodeId, serviceDiscoveryCallback);
  if (status != SL_STATUS_OK) {
    sl_zigbee_af_core_println("Error: %p failed to initiate Active EP request (0x%X)", PLUGIN_NAME, status);
    noteFailedDiscovery(device);
    scheduleEvent(WITH_DELAY);
  } else {
    // Don't schedule event, since service discovery returns the results via callback.
    sl_zigbee_af_device_database_set_status(device->eui64, SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS);
  }
}

static void sendSimpleDescriptorRequest(const sl_zigbee_af_device_info_t* device)
{
  uint8_t endpoint = sl_zigbee_af_device_database_get_device_endpoint_from_index(device->eui64, currentEndpointIndex);
  if (endpoint == 0xFF) {
    sl_zigbee_af_core_println("%p All endpoints discovered for 0x%2X", PLUGIN_NAME, currentNodeId);
    sl_zigbee_af_device_database_set_status(device->eui64,
                                            SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_STACK_REVISION);
    gNodeDescriptorAttempts = 0;
    scheduleEvent(RIGHT_NOW);
    return;
  }
  debugPrintln("%p initiating simple descriptor request for: 0x%2X EP %d", PLUGIN_NAME, currentNodeId, endpoint);
  sl_status_t status = sl_zigbee_af_find_clusters_by_device_and_endpoint(currentNodeId, endpoint, serviceDiscoveryCallback);
  if (status != SL_STATUS_OK) {
    noteFailedDiscovery(device);
    scheduleEvent(WITH_DELAY);
    return;
  }
  // Don't schedule event, since service discovery returns the results via callback.
}

static void sendNodeDescriptorRequest(const sl_zigbee_af_device_info_t* device)
{
  sl_status_t status;

  if (!device) {
    return;
  }

  // We'll try up to three times to send a Node Descriptor in hopes to receive a
  // response
  if (gNodeDescriptorAttempts >= NODE_DESCRIPTOR_ATTEMPTS_MAX) {
    noteFailedDiscovery(device);
    scheduleEvent(WITH_DELAY);
    return;
  }

  gNodeDescriptorAttempts++;

  debugPrintln("%p initiating node descriptor request for: 0x%2X",
               PLUGIN_NAME,
               currentNodeId);

  status = sl_zigbee_node_descriptor_request(currentNodeId,
                                             SL_ZIGBEE_AF_DEFAULT_APS_OPTIONS);

  if (status != SL_STATUS_OK) {
    noteFailedDiscovery(device);
    scheduleEvent(WITH_DELAY);
    return;
  }

  scheduleEvent(WITH_DELAY);
}

static void myEventHandler(sl_zigbee_af_event_t * event)
{
  sl_zigbee_af_event_set_inactive(&myEvent);

  debugPrintln("%p event", PLUGIN_NAME);
  const sl_zigbee_af_device_info_t* device;
  if (sl_zigbee_af_memory_byte_compare(currentEui64, EUI64_SIZE, 0xFF)) {
    debugPrintln("%p no current device being queried, looking in database for one.", PLUGIN_NAME);
    device = sl_zigbee_af_device_database_find_device_by_status(SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_NEW
                                                                | SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS
                                                                | SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_CLUSTERS
                                                                | SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_STACK_REVISION);
    if (device != NULL) {
      debugPrintln("%p found device with status (0x%X): %p",
                   PLUGIN_NAME,
                   device->status,
                   sl_zigbee_af_device_database_get_status_string(device->status));

      if ((device->capabilities & RECEIVER_ON_WHEN_IDLE) == 0) {
        sl_zigbee_af_device_database_set_status(device->eui64, SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_DONE);
        debugPrintln("%p ignoring sleepy device.", PLUGIN_NAME);
        scheduleEvent(RIGHT_NOW);
        return;
      } else {
        memmove(currentEui64, device->eui64, EUI64_SIZE);
      }
    }
  } else {
    device = sl_zigbee_af_device_database_find_device_by_eui64(currentEui64);
    if (device == NULL) {
      sl_zigbee_af_core_print("Error: %p the current EUI64 does not correspond to any known device: ", PLUGIN_NAME);
      sl_zigbee_af_print_big_endian_eui64(currentEui64);
      sl_zigbee_af_core_println("");
      clearCurrentDevice();
      scheduleEvent(WITH_DELAY);
      return;
    }
  }

  if (device == NULL) {
    clearCurrentDevice();

    if (sl_zigbee_af_device_database_clear_all_failed_discovery_status(SL_ZIGBEE_AF_PLUGIN_DEVICE_QUERY_SERVICE_MAX_FAILURES)) {
      sl_zigbee_af_core_println("%p Retrying failed discoveries.", PLUGIN_NAME);
      scheduleEvent(WITH_DELAY);
    } else {
      sl_zigbee_af_core_println("%p All known devices discovered.", PLUGIN_NAME);
    }
    return;
  }

  // Although we could consult our local tables for addresses, we perform a broadcast
  // lookup here to insure that we have a current source route back to the destination.
  // The target of the discovery will unicast the result, along with a route record.
  if (currentNodeId == SL_ZIGBEE_NULL_NODE_ID) {
    debugPrint("%p initiating node ID discovery for: ", PLUGIN_NAME);
    debugPrintEui64(currentEui64);
    debugPrintln("");
    sl_status_t status = sl_zigbee_af_find_node_id(currentEui64, serviceDiscoveryCallback);
    if (status != SL_STATUS_OK) {
      sl_zigbee_af_core_println("%p failed to initiate node ID discovery.", PLUGIN_NAME);
      noteFailedDiscovery(device);
      scheduleEvent(WITH_DELAY);
    }
    // Else
    //   Don't schedule event, since service discovery callback returns the results.
    return;
  }

  switch (device->status) {
    case SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_NEW:
    case SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS:
      sendActiveEndpointRequest(device);
      break;

    case SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_CLUSTERS:
      sendSimpleDescriptorRequest(device);
      break;

    case SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_STACK_REVISION:
      sendNodeDescriptorRequest(device);
      break;

    default:
      break;
  }
}
