/***************************************************************************//**
 * @file
 * @brief This file defines various security parameters based on the security
 * profile setup via App. Builder.
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

#ifndef SILABS_SECURITY_CONFIG_H
#define SILABS_SECURITY_CONFIG_H

#if defined(SL_ZIGBEE_AF_HAS_SECURITY_PROFILE_SE_TEST) || defined(SL_ZIGBEE_AF_HAS_SECURITY_PROFILE_SE_FULL)
  #define SL_ZIGBEE_AF_HAS_SECURITY_PROFILE_SE
#endif

// This key is "ZigBeeAlliance09"
#define ZIGBEE_PROFILE_INTEROPERABILITY_LINK_KEY       \
  {                                                    \
    { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C,  \
      0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 } \
  }

#define SE_SECURITY_TEST_LINK_KEY                      \
  {                                                    \
    { 0x56, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,  \
      0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77 } \
  }

#define DUMMY_KEY                                      \
  {                                                    \
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  \
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } \
  }

// None.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_TC_SECURITY_BITMASK            0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_TC_EXTENDED_SECURITY_BITMASK   0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_NODE_SECURITY_BITMASK          0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_NODE_EXTENDED_SECURITY_BITMASK 0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_DENY_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_NONE_PRECONFIGURED_KEY              DUMMY_KEY

// HA.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_TC_SECURITY_BITMASK            (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY \
                                                                         | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY     \
                                                                         | SL_ZIGBEE_HAVE_NETWORK_KEY           \
                                                                         | SL_ZIGBEE_NO_FRAME_COUNTER_RESET     \
                                                                         | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_TC_EXTENDED_SECURITY_BITMASK   SL_ZIGBEE_JOINER_GLOBAL_LINK_KEY
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_NODE_SECURITY_BITMASK          (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY \
                                                                         | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY     \
                                                                         | SL_ZIGBEE_NO_FRAME_COUNTER_RESET     \
                                                                         | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_NODE_EXTENDED_SECURITY_BITMASK SL_ZIGBEE_JOINER_GLOBAL_LINK_KEY
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_ALLOW_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA_PRECONFIGURED_KEY              ZIGBEE_PROFILE_INTEROPERABILITY_LINK_KEY

// HA1.2.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_TC_SECURITY_BITMASK            (SL_ZIGBEE_HAVE_NETWORK_KEY \
                                                                           | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_TC_EXTENDED_SECURITY_BITMASK   0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_NODE_SECURITY_BITMASK          (SL_ZIGBEE_GET_PRECONFIGURED_KEY_FROM_INSTALL_CODE \
                                                                           | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_NODE_EXTENDED_SECURITY_BITMASK 0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_ALLOW_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_HA12_PRECONFIGURED_KEY              DUMMY_KEY

// SE Security Test.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_TC_SECURITY_BITMASK            (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY \
                                                                              | SL_ZIGBEE_HAVE_NETWORK_KEY           \
                                                                              | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY     \
                                                                              | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_TC_EXTENDED_SECURITY_BITMASK   SL_ZIGBEE_NWK_LEAVE_REQUEST_NOT_ALLOWED
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_NODE_SECURITY_BITMASK          (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY \
                                                                              | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY     \
                                                                              | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_NODE_EXTENDED_SECURITY_BITMASK SL_ZIGBEE_NWK_LEAVE_REQUEST_NOT_ALLOWED
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_ALLOW_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_TEST_PRECONFIGURED_KEY              SE_SECURITY_TEST_LINK_KEY

// SE Security Full.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_TC_SECURITY_BITMASK            (SL_ZIGBEE_HAVE_NETWORK_KEY \
                                                                              | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_TC_EXTENDED_SECURITY_BITMASK   SL_ZIGBEE_NWK_LEAVE_REQUEST_NOT_ALLOWED
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_NODE_SECURITY_BITMASK          (SL_ZIGBEE_GET_PRECONFIGURED_KEY_FROM_INSTALL_CODE \
                                                                              | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_NODE_EXTENDED_SECURITY_BITMASK SL_ZIGBEE_NWK_LEAVE_REQUEST_NOT_ALLOWED
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_ALLOW_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_SE_FULL_PRECONFIGURED_KEY              DUMMY_KEY

// Z3 security.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_TC_SECURITY_BITMASK            (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY        \
                                                                         | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY            \
                                                                         | SL_ZIGBEE_HAVE_NETWORK_KEY                  \
                                                                         | SL_ZIGBEE_NO_FRAME_COUNTER_RESET            \
                                                                         | SL_ZIGBEE_TRUST_CENTER_USES_HASHED_LINK_KEY \
                                                                         | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_TC_EXTENDED_SECURITY_BITMASK   SL_ZIGBEE_NWK_LEAVE_REQUEST_NOT_ALLOWED
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_NODE_SECURITY_BITMASK          (SL_ZIGBEE_TRUST_CENTER_GLOBAL_LINK_KEY \
                                                                         | SL_ZIGBEE_HAVE_PRECONFIGURED_KEY     \
                                                                         | SL_ZIGBEE_REQUIRE_ENCRYPTED_KEY      \
                                                                         | SL_ZIGBEE_NO_FRAME_COUNTER_RESET)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_NODE_EXTENDED_SECURITY_BITMASK (SL_ZIGBEE_JOINER_GLOBAL_LINK_KEY \
                                                                         | SL_ZIGBEE_EXT_NO_FRAME_COUNTER_RESET)
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_DENY_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_Z3_PRECONFIGURED_KEY              ZIGBEE_PROFILE_INTEROPERABILITY_LINK_KEY

// Custom security.
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_TC_SECURITY_BITMASK            0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_TC_EXTENDED_SECURITY_BITMASK   0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_NODE_SECURITY_BITMASK          0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_NODE_EXTENDED_SECURITY_BITMASK 0
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_TC_LINK_KEY_REQUEST_POLICY     SL_ZIGBEE_AF_DENY_TC_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_APP_LINK_KEY_REQUEST_POLICY    SL_ZIGBEE_AF_DENY_APP_KEY_REQUESTS
#define SL_ZIGBEE_AF_SECURITY_PROFILE_CUSTOM_PRECONFIGURED_KEY              DUMMY_KEY

#endif // SILABS_SECURITY_CONFIG_H
