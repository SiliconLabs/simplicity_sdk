/**************************************************************************//**
* @file
* @brief BT Mesh Provisioner component
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

// ----------------------------------------------------------------------------
// Includes

#include "app_assert.h"
#include "app_log.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"

#include "btmesh_conf.h"
#include "btmesh_db.h"
#include "btmesh_prov.h"

#include "sl_common.h"

// ----------------------------------------------------------------------------
// Macros

#define PB_ADV 0
#define PB_GATT 1
#define PB_REMOTE 2

/// Utility macro to convert two 8 bit values to 16 bit
#define TOUINT16(a, b) (((uint16_t)a << 8) | b)
/// Maximum number of already available networks to check on startup
#define MAX_NETWORKS_TO_CHECK 5
/// Current version of a key from a node
#define SL_BTMESH_NODE_CURRENT_KEY 1

// ----------------------------------------------------------------------------
// Type definitions

/// A helper struct to store network status info on provisioner init
typedef struct btmesh_prov_network_on_init_t {
  uint16_t netkey_index;  ///< Netkey index of the primary network
  uint16_t count;         ///< Number of networks on the provisioner
  uint16_t nodes;         ///< Number of nodes in the provisioner's DDB
} btmesh_prov_network_on_init_t;

// ----------------------------------------------------------------------------
// Static Function Declarations

/**************************************************************************//**
* Remove a node from the network
*
* @param[in] node The node to remove
* @param[in] on_job_notification Job status notification callback
* @return Status of starting the node removal process
* @retval SL_STATUS_OK If the node removal job started successfully.
*         Error code otherwise.
******************************************************************************/
static sl_status_t btmesh_prov_remove_node(btmesh_db_node_t *node,
                                           btmesh_conf_on_job_notification_t on_job_notification);

// ----------------------------------------------------------------------------
// Static Variables

/// Variable to store information about the primary network available on
/// provisioner init
static btmesh_prov_network_on_init_t startup_network;
/// The UUID of the provisioner
static uuid_128 provisioner_uuid;

// ----------------------------------------------------------------------------
// Function definitions

void btmesh_prov_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_prov_initialized_id: {
      sl_btmesh_evt_prov_initialized_t *prov_initialized_evt;
      prov_initialized_evt = (sl_btmesh_evt_prov_initialized_t *)&(evt->data);
      uint8_t networks = prov_initialized_evt->networks;
      if (0 < networks) {
        uint32_t num_of_keys;
        uint16_t mesh_key_id;
        uint16_t netkey_index;
        aes_key_128 key_data;
        // Get network key count to iterate through them
        sl_status_t sc = sl_btmesh_node_get_key_count(sl_btmesh_node_key_type_net, &num_of_keys);
        app_assert_status_f(sc, "Failed to get key count");
        for (size_t i = 0; i < num_of_keys; i++) {
          sl_status_t sc = sl_btmesh_node_get_key(sl_btmesh_node_key_type_net,
                                                  i,
                                                  SL_BTMESH_NODE_CURRENT_KEY,
                                                  &mesh_key_id,
                                                  &netkey_index,
                                                  &key_data);
          app_assert_status_f(sc, "Can't get key from node");
          sc = btmesh_db_create_network(mesh_key_id);
          if (SL_STATUS_OK != sc) {
            app_log_warning("Can't add network to database" APP_LOG_NEW_LINE);
          }
        }
        // List known nodes to build database
        btmesh_prov_list_ddb_entries(&startup_network.nodes);
        if (0 == startup_network.nodes) {
          // Inform app that there are no nodes present in DDB
          btmesh_prov_on_ddb_list_ready(startup_network.nodes);
        }
      }
      // Init provisioning records if CBP is present
      // Otherwise it's a weak function doing nothing
      sl_status_t sc = btmesh_prov_init_provisioning_records();
      app_assert_status_f(sc, "Failed to init provisioning records");
      break;
    }
    case sl_btmesh_evt_node_provisioning_started_id:
      app_log_info("Provisioning started" APP_LOG_NEW_LINE);
      break;
    case sl_btmesh_evt_node_provisioning_failed_id:
      app_log_info("Provisioning failed" APP_LOG_NEW_LINE);
      break;
    case sl_btmesh_evt_prov_unprov_beacon_id: {
      // Unprovisioned beacon seen
      sl_btmesh_evt_prov_unprov_beacon_t *unprov_beacon_evt;
      unprov_beacon_evt = (sl_btmesh_evt_prov_unprov_beacon_t *)&(evt->data);
      uint8_t bearer = unprov_beacon_evt->bearer;
      bd_addr address = unprov_beacon_evt->address;
      uuid_128 uuid = unprov_beacon_evt->uuid;
      uint16_t oob_capabilities = unprov_beacon_evt->oob_capabilities;
      // Create node in database only once
      // note: this will initialize as not available, i.e. unprovisoned
      if (NULL == btmesh_db_node_get_by_uuid(uuid) && PB_ADV == bearer) {
        btmesh_db_create_node(uuid, address, 0, 0, PB_ADV, oob_capabilities);
      }
      break;
    }
    case sl_btmesh_evt_prov_device_provisioned_id: {
      sl_status_t sc;
      sl_btmesh_evt_prov_device_provisioned_t *prov_evt;
      prov_evt = (sl_btmesh_evt_prov_device_provisioned_t *)&(evt->data);
      uint16_t address = prov_evt->address;
      uuid_128 uuid = prov_evt->uuid;
      btmesh_db_node_t *provisioned_node = btmesh_db_node_get_by_uuid(uuid);
      if (NULL != provisioned_node) {
        // Set node to available, i.e. provisioned
        sc = btmesh_db_node_set_availability(provisioned_node, true);
        app_log_status_warning_f(sc, "Unable to set node availability" APP_LOG_NEW_LINE);
        sc = btmesh_db_node_set_primary_address(provisioned_node, address);
        app_log_status_warning_f(sc, "Unable to set primary address" APP_LOG_NEW_LINE);
      }
      btmesh_prov_on_device_provisioned_evt(address, uuid);
      break;
    }
    case sl_btmesh_evt_prov_provisioning_failed_id: {
      sl_status_t sc;
      sl_btmesh_evt_prov_provisioning_failed_t *prov_failed_evt;
      prov_failed_evt = (sl_btmesh_evt_prov_provisioning_failed_t *)&(evt->data);
      uint8_t reason = prov_failed_evt->reason;
      uuid_128 uuid = prov_failed_evt->uuid;
      btmesh_db_node_t *provisioned_node = btmesh_db_node_get_by_uuid(uuid);
      if (NULL != provisioned_node) {
        // Remove node from network if present.
        // note: this should not happen as the node was and still is unprovisioned
        sc = btmesh_db_remove_node_from_network_by_uuid(provisioned_node->primary_subnet->netkey_index,
                                                        uuid);
        if (SL_STATUS_OK != sc) {
          app_log_status_warning_f(sc, "Failed to remove node form network" APP_LOG_NEW_LINE);
        }
      }
      btmesh_prov_on_provision_failed_evt(reason, uuid);
      app_log_error("Provisioning failed, reason: 0x%02x" APP_LOG_NEW_LINE, prov_failed_evt->reason);
      break;
    }
    case sl_btmesh_evt_prov_ddb_list_id: {
      sl_status_t sc;
      sl_btmesh_evt_prov_ddb_list_t *ddb_list_evt;
      ddb_list_evt = (sl_btmesh_evt_prov_ddb_list_t *)&(evt->data);
      uuid_128 uuid = ddb_list_evt->uuid;
      uint16_t address = ddb_list_evt->address;
      uint8_t elements = ddb_list_evt->elements;
      bd_addr mac = { 0 }; //unknown at this point
      sc = btmesh_db_create_node(uuid, mac, address, elements, PB_ADV, 0);
      if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
        app_log_warning("Failed to create node in database!" APP_LOG_NEW_LINE);
      }
      sc = btmesh_db_add_node_to_network_by_uuid(startup_network.netkey_index, uuid);
      if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
        app_log_warning("Failed to add node to network in database!" APP_LOG_NEW_LINE);
      }
      startup_network.count--;
      if (0 == startup_network.count) {
        btmesh_prov_on_ddb_list_ready(startup_network.nodes);
      }
      break;
    }
    case sl_btmesh_evt_prov_delete_ddb_entry_complete_id: {
      sl_status_t sc;
      sl_btmesh_evt_prov_delete_ddb_entry_complete_t *ddb_delete_evt;
      ddb_delete_evt = (sl_btmesh_evt_prov_delete_ddb_entry_complete_t *)&(evt->data);
      uint16_t result = ddb_delete_evt->result;
      uuid_128 uuid = ddb_delete_evt->uuid;
      if (0 == result) {
        sc = btmesh_db_remove_node_by_uuid(uuid);
        if (SL_STATUS_OK != sc) {
          app_log_error("Could not remove node from database!" APP_LOG_NEW_LINE);
        }
      } else {
        app_log_warning("Could not delete DDB entry!" APP_LOG_NEW_LINE);
      }
    }
    default:
      break;
  }
  // Let subcomponents handle other events
  btmesh_cbp_on_event(evt);
}

sl_status_t btmesh_prov_start_scanning(void)
{
  sl_status_t sc = sl_btmesh_prov_scan_unprov_beacons();
  return sc;
}

sl_status_t btmesh_prov_stop_scanning(void)
{
  sl_status_t sc = sl_btmesh_prov_stop_scan_unprov_beacons();
  return sc;
}

sl_status_t btmesh_prov_setup_provisioning(uint16_t netkey_index,
                                           uuid_128 uuid,
                                           uint8_t bearer_type,
                                           uint8_t attention_timer_sec)
{
  (void)bearer_type;
  sl_status_t sc;
  sc = sl_btmesh_prov_create_provisioning_session(netkey_index,
                                                  uuid,
                                                  attention_timer_sec);
  if (SL_STATUS_OK != sc) {
    app_log_warning("Failed to create provisioning session" APP_LOG_NEW_LINE);

    return sc;
  }

  // Setup CBP if enabled, otherwise do nothing
  sc = btmesh_prov_setup_cbp(uuid);
  return sc;
}

sl_status_t btmesh_prov_provision_adv_device(uint16_t netkey_index,
                                             uuid_128 uuid,
                                             bd_addr mac_address,
                                             uint8_t bearer_type,
                                             uint8_t attention_timer_sec)
{
  sl_status_t sc;
  sc = sl_btmesh_prov_provision_adv_device(uuid);
  if (SL_STATUS_OK != sc) {
    app_log_status_error_f(sc, "Failed to start provisioning" APP_LOG_NEW_LINE);
    return sc;
  }
  sc = btmesh_db_create_node(uuid, mac_address, netkey_index, 0, bearer_type, 0);
  if ((SL_STATUS_OK != sc) && (SL_STATUS_ALREADY_EXISTS != sc)) {
    app_log_status_warning_f(sc, "Failed to create node in database!" APP_LOG_NEW_LINE);
  }
  sc = btmesh_db_add_node_to_network_by_uuid(netkey_index, uuid);
  if (SL_STATUS_OK != sc) {
    app_log_status_warning_f(sc, "Failed to add node to network in database!" APP_LOG_NEW_LINE);
  }
  // Return OK even if DB is wrong because the network is actually working at this time
  return SL_STATUS_OK;
}

sl_status_t btmesh_prov_get_unprov_uuid_by_id(uint16_t id, uuid_128 *uuid)
{
  sl_status_t sc;
  uint16_t node_cnt = 0;
  sl_slist_node_t *node_list = btmesh_db_get_node_list();
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if ( false == node->node_available) {
      if (node_cnt == id) {
        memcpy(uuid, &(node->prov.uuid), sizeof(uuid_128));
        sc = SL_STATUS_OK;
        break;
      }
      node_cnt++;
    }
  }
  if ( id != node_cnt) {
    // ID overflow
    sc = SL_STATUS_BT_MESH_DOES_NOT_EXIST;
  }
  return sc;
}

sl_status_t btmesh_prov_get_prov_uuid_by_id(uint16_t id, uuid_128 *uuid)
{
  sl_status_t sc;
  uint16_t node_cnt = 0;
  sl_slist_node_t *node_list = btmesh_db_get_node_list();
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (true == node->node_available) {
      if (0 == memcmp(&provisioner_uuid, &node->prov.uuid, sizeof(uuid_128))) {
        // Skip the provisioner itself as self-configuration is not supported
        continue;
      }
      if (node_cnt == id) {
        memcpy(uuid, &(node->prov.uuid), sizeof(uuid_128));
        sc = SL_STATUS_OK;
        break;
      }
      node_cnt++;
    }
  }
  if (NULL == node) {
    app_log_error("Node not found!" APP_LOG_NEW_LINE);
    sc = SL_STATUS_NOT_FOUND;
  }
  return sc;
}

sl_status_t btmesh_prov_get_prov_uuid_by_address(uint16_t primary_address, uuid_128 *uuid)
{
  sl_status_t sc;
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(primary_address);
  if (NULL == node) {
    app_log_error("Node not found!" APP_LOG_NEW_LINE);
    sc = SL_STATUS_NOT_FOUND;
  } else {
    memcpy(uuid, &(node->prov.uuid), sizeof(uuid_128));
    sc = SL_STATUS_OK;
  }
  return sc;
}

sl_status_t btmesh_prov_remove_node(btmesh_db_node_t *node,
                                    btmesh_conf_on_job_notification_t on_job_notification)
{
  sl_status_t sc = btmesh_conf_reset_node(node->primary_subnet->netkey_index,
                                          node->prov.prim_address,
                                          on_job_notification);
  app_log_status_error_f(sc, "Reset node request failed" APP_LOG_NEW_LINE);
  return sc;
}

sl_status_t btmesh_prov_remove_node_by_uuid(uuid_128 uuid,
                                            btmesh_conf_on_job_notification_t on_job_notification)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_uuid(uuid);
  if (NULL == node) {
    app_log_error("Node not found!" APP_LOG_NEW_LINE);
    return SL_STATUS_NOT_FOUND;
  } else {
    sl_status_t sc = btmesh_prov_remove_node(node, on_job_notification);
    return sc;
  }
}

sl_status_t btmesh_prov_list_unprovisioned_nodes(void)
{
  static uint16_t id = 0;
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(btmesh_db_get_node_list(), node, btmesh_db_node_t, list_elem) {
    if (false == node->node_available) {
      btmesh_prov_on_unprovisioned_node_list_evt(id, node->prov.uuid, node->prov.oob_capabilities);
      id++;
    }
  }
  if (0 == id) {
    return SL_STATUS_EMPTY;
  }
  id = 0;
  return SL_STATUS_OK;
}

sl_status_t btmesh_prov_list_provisioned_nodes(void)
{
  static uint16_t id = 0;
  sl_status_t sc = sl_btmesh_node_get_uuid(&provisioner_uuid);
  if (SL_STATUS_OK != sc) {
    app_log_warning("Provisioner device UUID is unknown" APP_LOG_NEW_LINE);
  }
  btmesh_db_node_t *node;
  SL_SLIST_FOR_EACH_ENTRY(btmesh_db_get_node_list(), node, btmesh_db_node_t, list_elem) {
    if (true == node->node_available) {
      if (0 == memcmp(&provisioner_uuid, &node->prov.uuid, sizeof(uuid_128))) {
        // Skip the provisioner itself as self-configuration is not supported
        continue;
      }
      btmesh_prov_on_provisioned_node_list_evt(id, node->prov.uuid, node->prov.prim_address);
      id++;
    }
  }
  if (0 == id) {
    return SL_STATUS_EMPTY;
  }
  id = 0;
  return SL_STATUS_OK;
}

sl_status_t btmesh_prov_list_ddb_entries(uint16_t *count)
{
  sl_status_t sc = sl_btmesh_prov_list_ddb_entries(count);
  if (SL_STATUS_OK == sc) {
    startup_network.count = *count;
    startup_network.nodes = *count;
  }
  return sc;
}

sl_status_t btmesh_prov_delete_ddb_entry(uuid_128 uuid)
{
  sl_status_t sc = sl_btmesh_prov_delete_ddb_entry(uuid);
  return sc;
}

sl_status_t btmesh_prov_create_appkey(uint16_t netkey_index,
                                      uint16_t appkey_index,
                                      size_t key_len,
                                      const uint8_t *key,
                                      size_t max_application_key_size,
                                      size_t *application_key_len,
                                      uint8_t *application_key)
{
  sl_status_t sc = sl_btmesh_prov_create_appkey(netkey_index,
                                                appkey_index,
                                                key_len,
                                                key,
                                                max_application_key_size,
                                                application_key_len,
                                                application_key);
  return sc;
}

sl_status_t btmesh_prov_create_network(uint16_t netkey_index,
                                       size_t key_len,
                                       const uint8_t *key)
{
  sl_status_t sc = btmesh_db_create_network(netkey_index);
  if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
    app_log_error("Failed to add network to database" APP_LOG_NEW_LINE);
  }

  sc = sl_btmesh_prov_create_network(netkey_index,
                                     key_len,
                                     key);
  return sc;
}

// ----------------------------------------------------------------------------
// Weak implementations of callbacks

SL_WEAK void btmesh_prov_on_device_provisioned_evt(uint16_t address, uuid_128 uuid)
{
  (void)address;
  (void)uuid;
}

SL_WEAK void btmesh_prov_on_provision_failed_evt(uint8_t reason, uuid_128 uuid)
{
  (void)reason;
  (void)uuid;
}

SL_WEAK void btmesh_prov_on_unprovisioned_node_list_evt(uint16_t id,
                                                        uuid_128 uuid,
                                                        uint16_t oob_capabilities)
{
  (void)id;
  (void)uuid;
  (void)oob_capabilities;
}

SL_WEAK void btmesh_prov_on_provisioned_node_list_evt(uint16_t id,
                                                      uuid_128 uuid,
                                                      uint16_t primary_address)
{
  (void)id;
  (void)uuid;
  (void)primary_address;
}

SL_WEAK void btmesh_prov_free_remote_serverlist(void)
{
}

SL_WEAK void btmesh_prov_on_ddb_list_ready(uint16_t count)
{
  (void)count;
}

SL_WEAK sl_status_t btmesh_prov_init_provisioning_records(void)
{
  return SL_STATUS_OK;
}

SL_WEAK sl_status_t btmesh_prov_setup_cbp(uuid_128 uuid)
{
  (void)uuid;
  return SL_STATUS_OK;
}

SL_WEAK void btmesh_cbp_on_event(sl_btmesh_msg_t *evt)
{
  (void)evt;
}
