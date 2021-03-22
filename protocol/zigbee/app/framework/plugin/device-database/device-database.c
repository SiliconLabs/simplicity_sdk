/***************************************************************************//**
 * @file
 * @brief A list of all devices known in the network and their services.
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

#include "device-database-config.h"

//============================================================================
// Globals

static sl_zigbee_af_device_info_t deviceDatabase[SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES];

#define INVALID_INDEX 0xFFFF

#define PLUGIN_NAME "Device-DB"

//============================================================================
// Forward Declarations

void sl_zigbee_af_device_database_discovery_complete_cb(const sl_zigbee_af_device_info_t* device);

//============================================================================

void sl_zigbee_af_device_database_init_cb(uint8_t init_level)
{
  (void)init_level;

  memset(deviceDatabase, 0xFF, sizeof(sl_zigbee_af_device_info_t) * SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES);
}

const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_get_device_by_index(uint16_t index)
{
  if (index > SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES) {
    return NULL;
  }

  if (sl_zigbee_af_memory_byte_compare(deviceDatabase[index].eui64, EUI64_SIZE, 0xFF)) {
    return NULL;
  }

  return &(deviceDatabase[index]);
}

const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_find_device_by_status(sl_zigbee_af_device_discovery_status_t status)
{
  uint16_t i;
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES; i++) {
    if (!sl_zigbee_af_memory_byte_compare(deviceDatabase[i].eui64, EUI64_SIZE, 0xFF)
        && (deviceDatabase[i].status & status)) {
      return &(deviceDatabase[i]);
    }
  }
  return NULL;
}

static sl_zigbee_af_device_info_t* findDeviceByEui64(const sl_802154_long_addr_t eui64)
{
  uint16_t i;
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES; i++) {
    if (0 == memcmp(eui64, deviceDatabase[i].eui64, EUI64_SIZE)) {
      return &(deviceDatabase[i]);
    }
  }
  return NULL;
}

const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_find_device_by_eui64(sl_802154_long_addr_t eui64)
{
  return findDeviceByEui64(eui64);
}

const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_add_device_with_all_info(const sl_zigbee_af_device_info_t* newDevice)
{
  if (NULL != findDeviceByEui64(newDevice->eui64)) {
    sl_zigbee_af_core_print("Error: %p cannot add device that already exists: ", PLUGIN_NAME);
    sl_zigbee_af_print_big_endian_eui64(newDevice->eui64);
    sl_zigbee_af_core_println("");
    return NULL;
  }
  sl_802154_long_addr_t nullEui64;
  memset(nullEui64, 0xFF, EUI64_SIZE);
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(nullEui64);
  if (device != NULL) {
    memmove(device, newDevice, sizeof(sl_zigbee_af_device_info_t));
    sl_zigbee_af_device_database_discovery_complete_cb(device);
  }
  return device;
}

const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_add(sl_802154_long_addr_t eui64, uint8_t zigbeeCapabalities)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  if (device == NULL) {
    sl_802154_long_addr_t nullEui64;
    memset(nullEui64, 0xFF, EUI64_SIZE);
    device = findDeviceByEui64(nullEui64);
    if (device != NULL) {
      memmove(device->eui64, eui64, EUI64_SIZE);
      device->status = SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_NEW;
      device->discoveryFailures = 0;
      device->capabilities = zigbeeCapabalities;
      device->endpointCount = 0;
    }
  }
  return device;
}

bool sl_zigbee_af_device_database_erase_device(sl_802154_long_addr_t eui64)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  if (device != NULL) {
    memset(device, 0xFF, sizeof(sl_zigbee_af_device_info_t));
    return true;
  }
  return false;
}

bool sl_zigbee_af_device_database_set_endpoints(const sl_802154_long_addr_t eui64,
                                                const uint8_t* endpointList,
                                                uint8_t endpointCount)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  if (device == NULL) {
    sl_zigbee_af_core_print("Error: %p cannot add endpoints.  No such device in database: ");
    sl_zigbee_af_print_big_endian_eui64(eui64);
    sl_zigbee_af_core_println("");
    return false;
  }

  // Clear all existing endpoints so there is no leftover clusters or endpoints.
  memset(device->endpoints,
         0xFF,
         sizeof(sl_zigbee_af_endpoint_info_struct_t) * SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE);

  device->endpointCount = (endpointCount < SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE
                           ? endpointCount
                           : SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE);

  uint8_t i;
  for (i = 0; i < device->endpointCount; i++) {
    device->endpoints[i].endpoint = *endpointList;
    device->endpoints[i].clusterCount = 0;
    endpointList++;
  }
  return true;
}

static uint8_t findEndpoint(sl_zigbee_af_device_info_t* device, uint8_t endpoint)
{
  uint8_t i;
  for (i = 0; i < SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE; i++) {
    if (endpoint == device->endpoints[i].endpoint) {
      return i;
    }
  }
  return 0xFF;
}

uint8_t sl_zigbee_af_device_database_get_device_endpoint_from_index(const sl_802154_long_addr_t eui64,
                                                                    uint8_t index)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  if (device != NULL
      && index < SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE) {
    return device->endpoints[index].endpoint;
  }
  return 0xFF;
}

uint8_t sl_zigbee_af_device_database_get_index_from_endpoint(uint8_t endpoint,
                                                             const sl_802154_long_addr_t eui64)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  uint8_t index = (device != NULL
                   ? findEndpoint(device, endpoint)
                   : 0xFF);
  return index;
}

bool sl_zigbee_af_device_database_set_clusters_for_endpoint(const sl_802154_long_addr_t eui64,
                                                            const sl_zigbee_af_cluster_list_t* clusterList)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(eui64);
  uint8_t index =  sl_zigbee_af_device_database_get_index_from_endpoint(clusterList->endpoint, eui64);
  if (index == 0xFF) {
    sl_zigbee_af_core_println("Error: %p endpoint %d does not exist for device.", PLUGIN_NAME, clusterList->endpoint);
    return false;
  }

  uint8_t doServer;
  device->endpoints[index].profileId = clusterList->profileId;
  device->endpoints[index].deviceId = clusterList->deviceId;
  device->endpoints[index].clusterCount = clusterList->inClusterCount + clusterList->outClusterCount;
  if (device->endpoints[index].clusterCount > SL_ZIGBEE_AF_MAX_CLUSTERS_PER_ENDPOINT) {
    sl_zigbee_af_core_println("%p too many clusters (%d) for endpoint.  Limiting to %d",
                              PLUGIN_NAME,
                              device->endpoints[index].clusterCount,
                              SL_ZIGBEE_AF_MAX_CLUSTERS_PER_ENDPOINT);
    device->endpoints[index].clusterCount = SL_ZIGBEE_AF_MAX_CLUSTERS_PER_ENDPOINT;
  }
  uint8_t deviceClusterIndex = 0;
  for (doServer = 0; doServer < 2; doServer++) {
    uint8_t clusterPointerIndex;
    uint8_t count = (doServer ? clusterList->inClusterCount : clusterList->outClusterCount);
    const uint16_t* clusterPointer = (doServer ? clusterList->inClusterList : clusterList->outClusterList);

    for (clusterPointerIndex = 0;
         (clusterPointerIndex < count)
         && (deviceClusterIndex < device->endpoints[index].clusterCount);
         clusterPointerIndex++) {
      device->endpoints[index].clusters[deviceClusterIndex].clusterId = clusterPointer[clusterPointerIndex];
      device->endpoints[index].clusters[deviceClusterIndex].server = (doServer ? true : false);
      deviceClusterIndex++;
    }
  }
  return true;
}

bool sl_zigbee_af_device_database_clear_all_failed_discovery_status(uint8_t maxFailureCount)
{
  bool atLeastOneCleared = false;
  uint16_t i;
  for (i = 0; i < SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES; i++) {
    if (sl_zigbee_af_memory_byte_compare(deviceDatabase[i].eui64, EUI64_SIZE, 0xFF)) {
      continue;
    }
    if (SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FAILED == deviceDatabase[i].status
        && deviceDatabase[i].discoveryFailures < maxFailureCount) {
      deviceDatabase[i].status = SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FIND_ENDPOINTS;
      atLeastOneCleared = true;
    }
  }
  return atLeastOneCleared;
}

bool sl_zigbee_af_device_database_set_status(const sl_802154_long_addr_t deviceEui64, sl_zigbee_af_device_discovery_status_t newStatus)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(deviceEui64);
  if (device != NULL) {
    device->status = newStatus;
    if (device->status == SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_FAILED) {
      device->discoveryFailures++;
    } else if (device->status == SL_ZIGBEE_AF_DEVICE_DISCOVERY_STATUS_DONE) {
      sl_zigbee_af_device_database_discovery_complete_cb(device);
    }
    return true;
  }
  return false;
}

static void doesDeviceHaveCluster(const sl_zigbee_af_device_info_t* device,
                                  sl_zigbee_af_cluster_id_t clusterToFind,
                                  bool server,
                                  uint8_t* returnEndpoint)
{
  uint8_t i;
  *returnEndpoint = SL_ZIGBEE_AF_INVALID_ENDPOINT;
  for (i = 0; i < SL_ZIGBEE_AF_MAX_ENDPOINTS_PER_DEVICE; i++) {
    uint8_t j;
    for (j = 0; j < SL_ZIGBEE_AF_MAX_CLUSTERS_PER_ENDPOINT; j++) {
      if (device->endpoints[i].clusters[j].clusterId == clusterToFind
          && device->endpoints[i].clusters[j].server == server) {
        *returnEndpoint = device->endpoints[i].endpoint;
        return;
      }
    }
  }
}

sl_status_t sl_zigbee_af_device_database_does_device_have_cluster(sl_802154_long_addr_t deviceEui64,
                                                                  sl_zigbee_af_cluster_id_t clusterToFind,
                                                                  bool server,
                                                                  uint8_t* returnEndpoint)
{
  sl_zigbee_af_device_info_t* device = findDeviceByEui64(deviceEui64);
  if (device == NULL) {
    return SL_STATUS_INVALID_STATE;
  }
  doesDeviceHaveCluster(device, clusterToFind, server, returnEndpoint);
  return SL_STATUS_OK;
}

void sl_zigbee_af_device_database_create_new_search(sl_zigbee_af_device_database_iterator_t* iterator)
{
  iterator->deviceIndex = 0;
}

sl_status_t sl_zigbee_af_device_database_find_device_supporting_cluster(sl_zigbee_af_device_database_iterator_t* iterator,
                                                                        sl_zigbee_af_cluster_id_t clusterToFind,
                                                                        bool server,
                                                                        uint8_t* returnEndpoint)
{
  if (iterator->deviceIndex >= SL_ZIGBEE_AF_PLUGIN_DEVICE_DATABASE_MAX_DEVICES) {
    // This was the most appropriate error code I could come up with to say, "Search Complete".
    return SL_STATUS_INVALID_INDEX;
  }

  doesDeviceHaveCluster(&(deviceDatabase[iterator->deviceIndex]), clusterToFind, server, returnEndpoint);
  iterator->deviceIndex++;
  return SL_STATUS_OK;
}

void sli_zigbee_af_device_database_update_node_stack_revision(sl_802154_long_addr_t eui64,
                                                              uint8_t stackRevision)
{
  sl_zigbee_af_device_info_t *device = findDeviceByEui64(eui64);
  if (device != NULL) {
    device->stackRevision = stackRevision;
  }
}
