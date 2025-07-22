/***************************************************************************//**
 * @brief ZigBee token definition code.
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_zigbee_token.h"
#if !defined(SL_CATALOG_TOKEN_MANAGER_PRESENT)
#define DEFINETYPES
#endif
#include "stack/config/sl_zigbee_token_defines.h"
#include "sl_zigbee_types.h"
#include "multi-pan-token-config.h"

#if !defined(EZSP_HOST) && (defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || defined(SL_ZIGBEE_TEST)) && !defined(SL_ZIGBEE_AF_API_TOKEN) && !defined(ZIGBEE_PRO_COMPLIANCE_ON_HOST)
#include "sl_zigbee_green_power_config.h"
#endif // !defined(EZSP_HOST) && (defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || defined(SL_ZIGBEE_TEST)) && !SL_ZIGBEE_AF_API_TOKEN

extern sl_status_t sl_zigbee_initialize_app_tokens(void);

sl_status_t halStackInitTokens(void)
{
  sl_status_t status = SL_STATUS_OK;
  status = sl_zigbee_initialize_app_tokens();
  assert(status == SL_STATUS_OK);

  tokTypeStackNvdataVersion tokTypeStackNvdataVersionDefault = TOKEN_STACK_NVDATA_VERSION_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_NVDATA_VERSION, &tokTypeStackNvdataVersionDefault, sizeof(tokTypeStackNvdataVersion));
  assert(status == SL_STATUS_OK);
  tokTypeStackNonceCounter tokTypeStackAPSFrameCounterDefault = TOKEN_STACK_APS_FRAME_COUNTER_DEFAULT;
  status = sl_zigbee_initialize_counter_token(COMMON_TOKEN_STACK_APS_FRAME_COUNTER, &tokTypeStackAPSFrameCounterDefault, sizeof(tokTypeStackNonceCounter));
  assert(status == SL_STATUS_OK);
  tokTypeStackKeys tokTypeStackAlternateKeysDefault = TOKEN_STACK_ALTERNATE_KEY_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_ALTERNATE_KEY, &tokTypeStackAlternateKeysDefault, sizeof(tokTypeStackKeys));
  assert(status == SL_STATUS_OK);
  tokTypeStackBootCounter tokTypeStackBootCounterDefault = TOKEN_STACK_BOOT_COUNTER_DEFAULT;
  status = sl_zigbee_initialize_counter_token(COMMON_TOKEN_STACK_BOOT_COUNTER, &tokTypeStackBootCounterDefault, sizeof(tokTypeStackBootCounter));
  assert(status == SL_STATUS_OK);
  tokTypeStackNonceCounter tokTypeStackNonceCounterDefault = TOKEN_STACK_NONCE_COUNTER_DEFAULT;
  status = sl_zigbee_initialize_counter_token(COMMON_TOKEN_STACK_NONCE_COUNTER, &tokTypeStackNonceCounterDefault, sizeof(tokTypeStackNonceCounter));
  assert(status == SL_STATUS_OK);
  tokTypeStackAnalysisReboot tokTypeStackAnalysisRebootDefault = TOKEN_STACK_ANALYSIS_REBOOT_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_ANALYSIS_REBOOT, &tokTypeStackAnalysisRebootDefault, sizeof(tokTypeStackAnalysisReboot));
  assert(status == SL_STATUS_OK);
  tokTypeStackKeys tokTypeStackKeysDefault = TOKEN_STACK_KEYS_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_KEYS, &tokTypeStackKeysDefault, sizeof(tokTypeStackKeys));
  assert(status == SL_STATUS_OK);
  tokTypeStackNodeData tokTypeStackNodeDataDefault = TOKEN_STACK_NODE_DATA_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_NODE_DATA, &tokTypeStackNodeDataDefault, sizeof(tokTypeStackNodeData));
  assert(status == SL_STATUS_OK);
  tokTypeStackTrustCenter tokTypeStackTrustCenterDefault = TOKEN_STACK_TRUST_CENTER_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_TRUST_CENTER, &tokTypeStackTrustCenterDefault, sizeof(tokTypeStackTrustCenter));
  assert(status == SL_STATUS_OK);
  tokTypeStackNetworkManagement tokTypeStackNetworkManagementDefault = TOKEN_STACK_NETWORK_MANAGEMENT_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_NETWORK_MANAGEMENT, &tokTypeStackNetworkManagementDefault, sizeof(tokTypeStackNetworkManagement));
  assert(status == SL_STATUS_OK);
  tokTypeStackParentInfo tokTypeStackParentInfoDefault = TOKEN_STACK_PARENT_INFO_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_PARENT_INFO, &tokTypeStackParentInfoDefault, sizeof(tokTypeStackParentInfo));
  assert(status == SL_STATUS_OK);
  tokTypeStackParentAdditionalInfo tokTypeStackParentAdditionalInfoDefault = TOKEN_STACK_PARENT_ADDITIONAL_INFO_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_PARENT_ADDITIONAL_INFO, &tokTypeStackParentAdditionalInfoDefault, sizeof(tokTypeStackParentAdditionalInfo));
  assert(status == SL_STATUS_OK);
  tokTypeStackMultiPhyNwkInfo tokTypeStackMultiPhyNwkInfoDefault = TOKEN_STACK_MULTI_PHY_NWK_INFO_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_MULTI_PHY_NWK_INFO, &tokTypeStackMultiPhyNwkInfoDefault, sizeof(tokTypeStackMultiPhyNwkInfo));
  assert(status == SL_STATUS_OK);
  tokTypeRSSI tokTypeRSSIDefault = TOKEN_STACK_MIN_RECEIVED_RSSI_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_MIN_RECEIVED_RSSI, &tokTypeRSSIDefault, sizeof(tokTypeRSSI));
  assert(status == SL_STATUS_OK);
  tokTypeStackRestoredEui64 tokTypeStackRestoredEui64Default = TOKEN_STACK_RESTORED_EUI64_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_RESTORED_EUI64, &tokTypeStackRestoredEui64Default, sizeof(tokTypeStackRestoredEui64));
  assert(status == SL_STATUS_OK);

  // Multi-network stack tokens
#if !defined(SL_ZIGBEE_MULTI_NETWORK_STRIPPED)
  tokTypeStackKeys tokTypeMNStackKeysDefault = TOKEN_MULTI_NETWORK_STACK_KEYS_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_KEYS, &tokTypeMNStackKeysDefault, sizeof(tokTypeStackKeys), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
  tokTypeStackNodeData tokTypeMNStackNodeDataDefault = TOKEN_MULTI_NETWORK_STACK_NODE_DATA_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_NODE_DATA, &tokTypeMNStackNodeDataDefault, sizeof(tokTypeStackNodeData), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
  tokTypeStackKeys tokTypeMNAlternateStackKeysDefault = TOKEN_MULTI_NETWORK_STACK_ALTERNATE_KEY_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_ALTERNATE_KEY, &tokTypeMNAlternateStackKeysDefault, sizeof(tokTypeStackKeys), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
  tokTypeStackTrustCenter tokTypeMNStackTrustCenterDefault = TOKEN_MULTI_NETWORK_STACK_TRUST_CENTER_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_TRUST_CENTER, &tokTypeMNStackTrustCenterDefault, sizeof(tokTypeStackTrustCenter), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
  tokTypeStackNetworkManagement tokTypeMNStackNetworkManagementDefault = TOKEN_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_NETWORK_MANAGEMENT, &tokTypeMNStackNetworkManagementDefault, sizeof(tokTypeStackNetworkManagement), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
  tokTypeStackParentInfo tokTypeMNStackParentInfoDefault = TOKEN_MULTI_NETWORK_STACK_PARENT_INFO_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_PARENT_INFO, &tokTypeMNStackParentInfoDefault, sizeof(tokTypeStackParentInfo), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
#if !defined(SL_ZIGBEE_MULTI_NETWORK_STRIPPED)
  // MULTI_NETWORK_STACK_NONCE_COUNTER was always defined, even if MN was not present
  tokTypeStackNonceCounter tokTypeMNStackNonceCounterDefault = TOKEN_MULTI_NETWORK_STACK_NONCE_COUNTER_DEFAULT;
  status = sl_zigbee_initialize_counter_token(COMMON_TOKEN_MULTI_NETWORK_STACK_NONCE_COUNTER, &tokTypeMNStackNonceCounterDefault, sizeof(tokTypeStackNonceCounter));
  assert(status == SL_STATUS_OK);
#endif  // !SL_ZIGBEE_MULTI_NETWORK_STRIPPED
  tokTypeStackParentAdditionalInfo tokTypeMNStackParentAdditionalInfoDefault = TOKEN_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_NETWORK_STACK_PARENT_ADDITIONAL_INFO, &tokTypeMNStackParentAdditionalInfoDefault, sizeof(tokTypeStackParentAdditionalInfo), EXTRA_NETWORKS_NUMBER);
  assert(status == SL_STATUS_OK);
#endif  // !SL_ZIGBEE_MULTI_NETWORK_STRIPPED

  // App tokens
  tokTypeStackBindingTable tokTypeStackBindingTableDefault = TOKEN_STACK_BINDING_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_BINDING_TABLE, &tokTypeStackBindingTableDefault, sizeof(tokTypeStackBindingTable), SL_ZIGBEE_BINDING_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeStackChildTable tokTypeStackChildTableDefault = TOKEN_STACK_CHILD_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_CHILD_TABLE, &tokTypeStackChildTableDefault, sizeof(tokTypeStackChildTable), SL_ZIGBEE_CHILD_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
#if !defined(EZSP_HOST)
  tokTypeStackKeyTable tokTypeStackKeyTableDefault = TOKEN_STACK_KEY_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_KEY_TABLE, &tokTypeStackKeyTableDefault, sizeof(tokTypeStackKeyTable), SL_ZIGBEE_ORIGINAL_KEY_TABLE_MAX_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeStackKeyTable tokTypeStackKeyTableExtendedDefault = TOKEN_STACK_KEY_TABLE_EXTENDED_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_KEY_TABLE_EXTENDED, &tokTypeStackKeyTableExtendedDefault, sizeof(tokTypeStackKeyTable), SL_ZIGBEE_EXTENDED_KEY_TABLE_MAX_SIZE);
  assert(status == SL_STATUS_OK);
#endif // !defined(EZSP_HOST
  tokTypeStackCertificateTable tokTypeStackCertificateTableDefault = TOKEN_STACK_CERTIFICATE_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_CERTIFICATE_TABLE, &tokTypeStackCertificateTableDefault, sizeof(tokTypeStackCertificateTable), SL_ZIGBEE_CERTIFICATE_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeStackAdditionalChildData tokTypeStackAdditionalChildDataDefault = TOKEN_STACK_ADDITIONAL_CHILD_DATA_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_ADDITIONAL_CHILD_DATA, &tokTypeStackAdditionalChildDataDefault, sizeof(tokTypeStackAdditionalChildData), SL_ZIGBEE_CHILD_TABLE_SIZE);
  assert(status == SL_STATUS_OK);

  // ZLL tokens
  tokTypeStackZllData tokTypeStackZllDataDefault = TOKEN_STACK_ZLL_DATA_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_ZLL_DATA, &tokTypeStackZllDataDefault, sizeof(tokTypeStackZllData));
  assert(status == SL_STATUS_OK);
  tokTypeStackZllSecurity tokTypeStackZllSecurityDefault = TOKEN_STACK_ZLL_SECURITY_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_ZLL_SECURITY, &tokTypeStackZllSecurityDefault, sizeof(tokTypeStackZllSecurity));
  assert(status == SL_STATUS_OK);

#if !defined(EZSP_HOST) && (defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || defined(SL_ZIGBEE_TEST))
  tokTypeStackGpData tokTypeStackGpDataDefault = TOKEN_STACK_GP_DATA_DEFAULT;
  status = sl_zigbee_initialize_basic_token(COMMON_TOKEN_STACK_GP_DATA, &tokTypeStackGpDataDefault, sizeof(tokTypeStackGpData));
  assert(status == SL_STATUS_OK);
  tokTypeStackGpProxyTableEntry tokTypeStackGpProxyTableEntryDefault = TOKEN_STACK_GP_PROXY_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_GP_PROXY_TABLE, &tokTypeStackGpProxyTableEntryDefault, sizeof(tokTypeStackGpProxyTableEntry), SL_ZIGBEE_GP_PROXY_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeGPDIncomingFC tokTypeGPDIncomingFCDefault = TOKEN_STACK_GP_INCOMING_FC_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_GP_INCOMING_FC, &tokTypeGPDIncomingFCDefault, sizeof(tokTypeGPDIncomingFC), SL_ZIGBEE_GP_INCOMING_FC_TOKEN_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeStackGpSinkTableEntry tokTypeStackGpSinkTableEntryDefault = TOKEN_STACK_GP_SINK_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_GP_SINK_TABLE, &tokTypeStackGpSinkTableEntryDefault, sizeof(tokTypeStackGpSinkTableEntry), SL_ZIGBEE_GP_SINK_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeGPDIncomingFCInSink tokTypeGPDIncomingFCInSinkDefault = TOKEN_STACK_GP_INCOMING_FC_IN_SINK_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_STACK_GP_INCOMING_FC_IN_SINK, &tokTypeGPDIncomingFCInSinkDefault, sizeof(tokTypeGPDIncomingFCInSink), SL_ZIGBEE_GP_SINK_TABLE_SIZE);
  assert(status == SL_STATUS_OK);
#endif // !defined(EZSP_HOST) && (defined(SL_CATALOG_ZIGBEE_GREEN_POWER_PRESENT) || defined(SL_ZIGBEE_TEST))

  // Multi-pan tokens
  tokTypeStackChildTable tokTypeMPStackChildTableDefault = TOKEN_MULTI_PAN_STACK_CHILD_TABLE_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_PAN_STACK_CHILD_TABLE, &tokTypeMPStackChildTableDefault, sizeof(tokTypeStackChildTable), SL_ZIGBEE_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE);
  assert(status == SL_STATUS_OK);
  tokTypeStackAdditionalChildData tokTypeMPStackAdditionalChildDataDefault = TOKEN_MULTI_PAN_STACK_ADDITIONAL_CHILD_DATA_DEFAULT;
  status = sl_zigbee_initialize_index_token(COMMON_TOKEN_MULTI_PAN_STACK_ADDITIONAL_CHILD_DATA, &tokTypeMPStackAdditionalChildDataDefault, sizeof(tokTypeStackAdditionalChildData), SL_ZIGBEE_MULTI_PAN_CHILD_TABLE_TOKEN_SIZE);
  assert(status == SL_STATUS_OK);
  return status;
}
