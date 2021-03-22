/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Device Database plugin.
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

/**
 * @defgroup device-database Device Database
 * @ingroup component
 * @brief API and Callbacks for the Device Database Component
 *
 * This component provides an API to add/remove a device
 * from a list of known devices, and to record their list of endpoints and clusters.
 *
 */

/**
 * @addtogroup device-database
 * @{
 */

/**
 * @name API
 * @{
 */
/** @brief  Get device by index.
 *
 * @param index Ver.: always
 *
 * @return sl_zigbee_af_device_info_t Device Information struct
 *
 */
const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_get_device_by_index(uint16_t index);

/** @brief Find device by status.
 *
 * @param status Ver.: always
 *
 * @return sl_zigbee_af_device_info_t Device Information struct
 *
 */
const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_find_device_by_status(sl_zigbee_af_device_discovery_status_t status);

/** @brief Find device by EUI64.
 *
 * @param eui64
 *
 * @return sl_zigbee_af_device_info_t Device Information struct
 *
 */
const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_find_device_by_eui64(sl_802154_long_addr_t eui64);

/** @brief Add to the device database.
 *
 * @param eui64 Ver.: always
 * @param zigbeeCapabilities Ver.: always
 *
 * @return sl_zigbee_device_info_t
 *
 */
const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_add(sl_802154_long_addr_t eui64, uint8_t zigbeeCapabilities);

/** @brief Erase device from device database.
 *
 * @param eui64 Ver.: always
 *
 * @return bool true is success
 *
 */
bool sl_zigbee_af_device_database_erase_device(sl_802154_long_addr_t eui64);

/** @brief Set device endpoints.
 *
 * @param eui64
 * @param endpointList
 * @param endpointCount
 *
 * @return bool true is success
 *
 */
bool sl_zigbee_af_device_database_set_endpoints(const sl_802154_long_addr_t eui64,
                                                const uint8_t* endpointList,
                                                uint8_t endpointCount);

/** @brief Get device endpoint from index.
 *
 * @param eui64
 * @param index
 *
 * @return uint8_t
 *
 */
uint8_t sl_zigbee_af_device_database_get_device_endpoint_from_index(const sl_802154_long_addr_t eui64,
                                                                    uint8_t index);

/** @brief Get index from endpoint.
 *
 * @param endpoint
 * @param eui64
 *
 * @note Explicitly made the eui64 the second argument to prevent confusion between
 * this function and the emberAfPluginDeviceDatabaseGetDeviceEndpointsFromIndex()
 *
 */
uint8_t sl_zigbee_af_device_database_get_index_from_endpoint(uint8_t endpoint,
                                                             const sl_802154_long_addr_t eui64);

/** @brief Set clusters from endpoint.
 *
 * @param eui64
 * @param clusterList
 *
 * @return bool true is success
 *
 */
bool sl_zigbee_af_device_database_set_clusters_for_endpoint(const sl_802154_long_addr_t eui64,
                                                            const sl_zigbee_af_cluster_list_t* clusterList);
/** @brief Clear the failed DB discovery count.
 *
 * @param maxFailureCount
 *
 * @return bool true is success
 */
bool sl_zigbee_af_device_database_clear_all_failed_discovery_status(uint8_t maxFailureCount);

/** @brief Get device database status string.
 *
 * @param status
 *
 * @return const char* status string
 *
 */
const char* sl_zigbee_af_device_database_get_status_string(sl_zigbee_af_device_discovery_status_t status);

/** @brief Set device database status.
 *
 * @param deviceEui64
 * @param newStatus
 *
 * @return bool true is success.
 *
 */
bool sl_zigbee_af_device_database_set_status(const sl_802154_long_addr_t deviceEui64, sl_zigbee_af_device_discovery_status_t newStatus);

/** @brief Add device to database with all information.
 *
 * @param newDevice Struct pointer to device infor
 *
 * @return sl_zigbee_device_info_t
 *
 */
const sl_zigbee_af_device_info_t* sl_zigbee_af_device_database_add_device_with_all_info(const sl_zigbee_af_device_info_t* newDevice);

/** @brief Check if a device has cluster.
 *
 * @param deviceEui64
 * @param clusterToFind
 * @param server
 * @param returnEndpoint
 *
 * @return sl_status_t status code
 *
 */
sl_status_t sl_zigbee_af_device_database_does_device_have_cluster(sl_802154_long_addr_t deviceEui64,
                                                                  sl_zigbee_af_cluster_id_t clusterToFind,
                                                                  bool server,
                                                                  uint8_t* returnEndpoint);

/** @brief Create a new search.
 *
 * @param iterator
 *
 */
void sl_zigbee_af_device_database_create_new_search(sl_zigbee_af_device_database_iterator_t* iterator);

/** @brief Find device supporting cluster.
 *
 * @param iterator
 * @param clusterToFind
 * @param server
 * @param returnEndpoint
 *
 * @return sl_status_t status code
 */
sl_status_t sl_zigbee_af_device_database_find_device_supporting_cluster(sl_zigbee_af_device_database_iterator_t* iterator,
                                                                        sl_zigbee_af_cluster_id_t clusterToFind,
                                                                        bool server,
                                                                        uint8_t* returnEndpoint);

/** @} */ // end of name API
/** @} */ // end of device-database

void sli_zigbee_af_device_database_update_node_stack_revision(sl_802154_long_addr_t eui64,
                                                              uint8_t stackRevision);
