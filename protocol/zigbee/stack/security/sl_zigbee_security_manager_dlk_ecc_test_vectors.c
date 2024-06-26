/***************************************************************************//**
 * @file sl_zigbee_security_manager_dlk_ecc_test_vectors.c
 * @brief test vectors for dynamic link key elliptic curve cryptography
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "stack/include/sl_zigbee_security_manager_dlk_ecc_test_vectors.h"
#include "stack/internal/inc/sl_zigbee_security_manager_dlk_ecc_internal_def.h"
// TODO ECDHE test vectors?

// SPEKE test vectors

// TODO redefine test vector data in a more compact way?
struct sl_zigbee_dlk_ecc_test_vector_shared_data sl_dlk_ecc_speke_c25519_share_data[] =
{
  // share_1
  { .config     = { DLK_ECC_OPERATION_SPEKE, DLK_ECC_CURVE_25519, DLK_ECC_HASH_AES_MMO_128 },
    .GIVEN_psk  = "ZigBeeAlliance18",
    .EXPECT_G   =
    { 0x90, 0x2B, 0x44, 0x85, 0xC8, 0x4E, 0xC4, 0xA0,
      0x59, 0x44, 0xAB, 0x34, 0x42, 0x92, 0x68, 0x78,
      0x90, 0x2B, 0x44, 0x85, 0xC8, 0x4E, 0xC4, 0xA0,
      0x59, 0x44, 0xAB, 0x34, 0x42, 0x92, 0x68, 0x78, },
    .EXPECT_x_k =
    { 0xFB, 0x9C, 0x3C, 0x7A, 0x2E, 0x49, 0x03, 0xCD,
      0xD2, 0x36, 0xDA, 0x82, 0xCD, 0x0B, 0x71, 0x81,
      0xB1, 0x61, 0x7D, 0x99, 0x67, 0x4C, 0x4E, 0xA8,
      0xA3, 0xF5, 0xD4, 0x60, 0x31, 0xDD, 0xA7, 0x09, },
    .EXPECT_s   =
    { 0xAE, 0x46, 0x6B, 0x75, 0x30, 0x9D, 0x5C, 0x2D,
      0x71, 0x5F, 0x7E, 0x44, 0x31, 0xDF, 0x04, 0x7A, },
    .EXPECT_dk  =
    { 0x25, 0x47, 0xF3, 0xAF, 0x96, 0x39, 0x1E, 0x1E,
      0xBF, 0xF2, 0xA3, 0xB7, 0x6D, 0x6A, 0x29, 0x29, }, },
  // share_2
  { .config     = { DLK_ECC_OPERATION_SPEKE, DLK_ECC_CURVE_25519, DLK_ECC_HASH_AES_MMO_128 },
    .GIVEN_psk  = "ZigBeeAlliance20",
    .EXPECT_G   =
    { 0xDE, 0xE6, 0x39, 0xE5, 0xFF, 0xF9, 0x46, 0xD7,
      0xB1, 0x00, 0xCC, 0x5F, 0x3F, 0x9C, 0xE8, 0x9C,
      0xDE, 0xE6, 0x39, 0xE5, 0xFF, 0xF9, 0x46, 0xD7,
      0xB1, 0x00, 0xCC, 0x5F, 0x3F, 0x9C, 0xE8, 0x9C, },
    .EXPECT_x_k =
    { 0x78, 0x3F, 0x96, 0x76, 0xC9, 0xC7, 0x4A, 0x69,
      0xC1, 0x41, 0xC3, 0xC2, 0x7B, 0xB9, 0xB4, 0x64,
      0x55, 0x12, 0xE7, 0x1B, 0xC6, 0xE1, 0x76, 0x79,
      0xB4, 0xBC, 0x33, 0xE7, 0x48, 0x5B, 0xF5, 0x04, },
    .EXPECT_s   =
    { 0xD9, 0x7B, 0x98, 0x45, 0xC6, 0x06, 0xE1, 0x05,
      0x72, 0x8E, 0x84, 0xC1, 0x48, 0x6A, 0x9F, 0x68, },
    .EXPECT_dk  =
    { 0x63, 0xF2, 0x38, 0x9B, 0x06, 0x38, 0x72, 0x20,
      0x96, 0x48, 0x18, 0x65, 0x7B, 0x47, 0xB5, 0xDB, }, },
  // share_3
  { .config     = { DLK_ECC_OPERATION_SPEKE, DLK_ECC_CURVE_25519, DLK_ECC_HASH_SHA_256 },
    .GIVEN_psk  = "ZigBeeAlliance18",
    .EXPECT_G   =
    { 0x09, 0xE8, 0xB7, 0x90, 0x39, 0x6F, 0x5B, 0xC0,
      0x99, 0x4B, 0xE4, 0x4F, 0xA7, 0x3C, 0xAE, 0x1A,
      0x4B, 0xFE, 0xDC, 0x7A, 0x71, 0x2F, 0x7B, 0x32,
      0x86, 0x4B, 0x45, 0xBF, 0x86, 0xF9, 0xF1, 0x78, },
    .EXPECT_x_k =
    { 0x24, 0x14, 0x1C, 0x4C, 0x06, 0xA2, 0xE7, 0xD5,
      0x9F, 0x15, 0x36, 0x7F, 0xAC, 0xE3, 0x9E, 0xC2,
      0x0C, 0x17, 0x67, 0xBB, 0x97, 0x25, 0x79, 0x41,
      0x6F, 0x14, 0x10, 0xCC, 0x36, 0x22, 0x2A, 0x2F, },
    .EXPECT_s   =
    { 0x6F, 0x79, 0xF3, 0x60, 0xC9, 0x8A, 0xE7, 0xF2,
      0x4E, 0x6D, 0xDD, 0xAB, 0xB5, 0xA3, 0x6D, 0x6E,
      0xA8, 0x0C, 0x76, 0x5A, 0xC6, 0x91, 0x1B, 0xAE,
      0x17, 0x1F, 0x21, 0x20, 0x3E, 0x88, 0x90, 0xAD, },
    .EXPECT_dk  =
    { 0xEB, 0x52, 0xE5, 0xBF, 0x6B, 0x5A, 0xC7, 0xF0,
      0xA9, 0x44, 0x0C, 0xAD, 0x78, 0x0B, 0xB7, 0x0B, }, },
  // share_4
  { .config     = { DLK_ECC_OPERATION_SPEKE, DLK_ECC_CURVE_25519, DLK_ECC_HASH_SHA_256 },
    .GIVEN_psk  = "ZigBeeAlliance20",
    .EXPECT_G   =
    { 0x09, 0xD0, 0x7B, 0x39, 0x6D, 0x10, 0xC1, 0x48,
      0xE3, 0xDA, 0xC6, 0xDE, 0xA7, 0xDC, 0xB2, 0x82,
      0xBB, 0xDD, 0xEE, 0x80, 0x63, 0x63, 0xA6, 0x5C,
      0x2B, 0x9F, 0xC9, 0x72, 0x7F, 0xE2, 0xD4, 0xF0, },
    .EXPECT_x_k =
    { 0x35, 0x88, 0x71, 0xB8, 0xF6, 0x22, 0x24, 0x4D,
      0x9A, 0xCF, 0x82, 0x62, 0x47, 0x13, 0x7F, 0x88,
      0x9F, 0xAF, 0x39, 0x38, 0xA5, 0x38, 0xDC, 0x41,
      0x7D, 0xE2, 0xE4, 0x14, 0xBB, 0xE6, 0x0C, 0x2D, },
    .EXPECT_s   =
    { 0xB7, 0x7D, 0x9E, 0xA0, 0xB6, 0x4D, 0x6A, 0xD7,
      0x33, 0x9A, 0xB2, 0x7D, 0x60, 0x03, 0xE6, 0x8B,
      0x8A, 0x27, 0x26, 0x19, 0x7F, 0x76, 0xE2, 0x0F,
      0xB8, 0x24, 0xC4, 0x3B, 0x6B, 0xB4, 0x5A, 0x47, },
    .EXPECT_dk  =
    { 0x31, 0xED, 0x35, 0x83, 0xAE, 0x18, 0x89, 0xBE,
      0xB4, 0x24, 0x90, 0x1B, 0xCB, 0x18, 0x1A, 0x99, }, },
};

// TODO there is a way to save meemory by only defining the id-key pair once per profile
struct sl_zigbee_dlk_ecc_test_vector_profile_data sl_dlk_ecc_speke_c25519_alice_data =
{
  .GIVEN_eui64 = GIVEN_speke_alice_eui64,
  .GIVEN_privkey = GIVEN_speke_c25519_alice_privkey,
  .EXPECT_pubkeys = {
    // a_pubkey_1
    { 0xBA, 0xBB, 0xC4, 0xD7, 0x85, 0x6A, 0xBF, 0x56,
      0x1B, 0xB4, 0x37, 0x8F, 0xD9, 0xFD, 0x24, 0x92,
      0xC1, 0xEA, 0x16, 0x02, 0x1B, 0x90, 0xD6, 0x1F,
      0xCE, 0x3A, 0x96, 0x5B, 0x04, 0x1C, 0xA2, 0x59, },
    // a_pubkey_2
    { 0x04, 0x8E, 0x8D, 0x32, 0x31, 0x21, 0x96, 0x39,
      0x28, 0x21, 0x7B, 0x2C, 0xF3, 0xC7, 0xDB, 0x23,
      0xAA, 0x4E, 0x75, 0x66, 0xD9, 0x69, 0xBF, 0x0E,
      0xD5, 0xFC, 0xA9, 0xF1, 0xA2, 0x3E, 0x0F, 0x6E, },
    // a_pubkey_3
    { 0x02, 0xB0, 0x9E, 0xDD, 0x3B, 0x8D, 0xE0, 0x88,
      0x4F, 0xC4, 0xE1, 0x93, 0xA2, 0xAE, 0x17, 0x97,
      0x02, 0xF8, 0x73, 0x6C, 0x79, 0xF0, 0xE2, 0x34,
      0x2C, 0x09, 0x9C, 0x4B, 0xF5, 0xF9, 0xB2, 0x77, },
    // a_pubkey_4
    { 0x09, 0xD0, 0x7B, 0x39, 0x6D, 0x10, 0xC1, 0x48,
      0xE3, 0xDA, 0xC6, 0xDE, 0xA7, 0xDC, 0xB2, 0x82,
      0xBB, 0xDD, 0xEE, 0x80, 0x63, 0x63, 0xA6, 0x5C,
      0x2B, 0x9F, 0xC9, 0x72, 0x7F, 0xE2, 0xD4, 0xF0, },
  },
};

struct sl_zigbee_dlk_ecc_test_vector_profile_data sl_dlk_ecc_speke_c25519_bob_data =
{
  .GIVEN_eui64   = GIVEN_speke_bob_eui64,
  .GIVEN_privkey = GIVEN_speke_c25519_bob_privkey,
  .EXPECT_pubkeys = {
    // b_pubkey_1
    { 0x47, 0x7D, 0xC0, 0x5F, 0xF5, 0x42, 0xAC, 0x83,
      0xAD, 0xDF, 0x2B, 0x87, 0x87, 0x11, 0x92, 0xDC,
      0x6C, 0x59, 0x6A, 0xC5, 0x40, 0xC8, 0xD3, 0x5A,
      0xFB, 0x7E, 0xC7, 0x25, 0x9A, 0x71, 0x5B, 0x6C, },
    // b_pubkey_2
    { 0x02, 0x29, 0xD3, 0x63, 0xE4, 0xC5, 0xE6, 0x7F,
      0xC4, 0xB1, 0x0B, 0xE2, 0xCD, 0x98, 0xE4, 0x53,
      0xE8, 0x6D, 0x86, 0x33, 0x8E, 0x15, 0xA0, 0x3A,
      0x3A, 0x68, 0xA4, 0x83, 0x3F, 0xE7, 0x84, 0x0C, },
    // b_pubkey_3
    { 0x68, 0x8F, 0xA2, 0xA3, 0xF0, 0xD1, 0x92, 0xB4,
      0x64, 0x6F, 0xE6, 0x62, 0x99, 0x4D, 0x27, 0xCC,
      0x5B, 0x6A, 0x57, 0x71, 0xBA, 0x56, 0xF3, 0x3F,
      0x3E, 0x8D, 0xED, 0x5E, 0x71, 0x71, 0x09, 0x0F, },
    // b_pubkey_4
    { 0xAA, 0xF7, 0x27, 0xB1, 0xF7, 0x9D, 0x63, 0x4C,
      0x21, 0xDA, 0x31, 0xE4, 0xAF, 0x39, 0xFD, 0x52,
      0x62, 0x92, 0x37, 0xBF, 0x53, 0xC8, 0xB2, 0x03,
      0xA5, 0x6C, 0x4B, 0x17, 0xBB, 0x3F, 0xB5, 0x51, },
  },
};

static sl_zigbee_dlk_ecc_test_vector_profile_data_t * assumed_role = NULL;
sl_zigbee_dlk_ecc_test_vector_profile_data_t* sli_zigbee_stack_dlk_ecc_test_vector_get_profile(void)
{
  return assumed_role;
}
void sli_zigbee_stack_dlk_ecc_test_vector_set_profile(const sl_zigbee_dlk_ecc_test_vector_profile_data_t* profile)
{
  assumed_role = (sl_zigbee_dlk_ecc_test_vector_profile_data_t*)profile;
  // set local eui
}

sl_status_t sli_zigbee_stack_sec_man_ecc_init_with_test_vector(
  sl_zigbee_sec_man_dlk_ecc_context_t *dlk_ecc_ctx,
  sl_zb_dlk_ecc_config_t *config,
  uint8_t *psk,
  sl_zigbee_dlk_ecc_test_vector_bundle_t *vector_data)
{
  sl_status_t status = sli_zigbee_stack_sec_man_ecc_init(dlk_ecc_ctx, config, psk);
  if (status != SL_STATUS_OK) {
    return status;
  }
  dlk_ecc_ctx->test = (void *) vector_data;
  return SL_STATUS_OK;
}
