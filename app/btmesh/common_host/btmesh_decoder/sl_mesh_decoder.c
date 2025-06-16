/***************************************************************************//**
 * @file sl_mesh_decoder.c
 * @brief Bluetooth Mesh message decoder component source file
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
// -----------------------------------------------------------------------------
// Includes
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <psa/crypto.h>
#include <math.h>

#include "config/sl_mesh_decoder_config.h"
#include "sl_btmesh_sensor.h"
#include "sl_btmesh_sensor_model_capi_types.h"
#include "sl_btmesh_model_specification_defs.h"
#include "sl_mesh_decoder.h"
#include "sl_mesh_decoder_util.h"

// -----------------------------------------------------------------------------
// Macros

#define NET_HEADER_LEN (9)
#define MIC32_LEN (4)
#define MIC64_LEN (8)

#define MIN_PDU_LEN (NET_HEADER_LEN + 1 + MIC32_LEN)

#define MAX_TRANS_ACCESS_PDU_LEN (16)
#define MAX_TRANS_CONTROL_PDU_LEN (12)

// -----------------------------------------------------------------------------
// Enums, structs, typedefs

typedef psa_key_id_t mesh_key_handle;

enum mesh_key_type {
  mesh_key_null = 0,
  mesh_key_net,
#if (DECODE_FRIENDSHIP)
  mesh_key_net_friend,
#endif
  mesh_key_app,
  mesh_key_dev,
};

struct mesh_key {
  enum mesh_key_type type;
  union {
    struct {
      uint8_t net_key[16];
      psa_key_id_t identity_key;
      psa_key_id_t beacon_key;
      psa_key_id_t flooding_encryption_key;
      psa_key_id_t flooding_privacy_key;
#if DECODE_DIRECTED_FORWARDING
      psa_key_id_t directed_encryption_key;
      psa_key_id_t directed_privacy_key;
#endif
      psa_key_id_t private_beacon_key;
      uint8_t flooding_nid;
#if DECODE_DIRECTED_FORWARDING
      uint8_t directed_nid;
#endif
      uint8_t network_id[8];
    } net;
#if DECODE_FRIENDSHIP
    struct {
      psa_key_id_t encryption_key;
      psa_key_id_t privacy_key;
      uint8_t nid;
      size_t net_key_index;
      struct sl_mesh_friendship friendship;
    } net_friend;
#endif
    struct {
      psa_key_id_t key;
      uint8_t aid;
    } app;
    struct {
      psa_key_id_t key;
      uint16_t addr;
    } dev;
  };
};

struct reassembler {
  struct reassembler *prev;
  struct reassembler *next;
  sl_mesh_key_handle net_key;
  uint16_t src;
  uint16_t dst;
  uint32_t seq_0;
  uint8_t seg_n;
  uint8_t ctl;
  uint32_t bitmap;
  uint32_t seq_auth;
  uint8_t ctl_opcode;
  uint8_t akf;
  uint8_t aid;
  uint8_t szmic;
  size_t pdu_len;
  uint8_t pdu[];
};

#if DECODE_FRIENDSHIP

struct friend_request {
  sl_mesh_key_handle net_key;
  uint16_t lpn_addr;
  uint16_t lpn_counter;
};

struct friend_offer {
  sl_mesh_key_handle net_key;
  uint16_t lpn_addr;
  uint16_t friend_addr;
  uint16_t friend_counter;
};

#endif

struct virtual_label {
  uint8_t uuid[16];
  uint16_t hash;
};

struct sl_mesh_decoder {
  struct reassembler *reassemblers;
  size_t reassembler_count;
#if DECODE_FRIENDSHIP
  struct friend_request friend_request[MAX_INCOMPLETE_FRIENDSHIP_COUNT];
  size_t friend_request_count;
  struct friend_offer friend_offer[MAX_INCOMPLETE_FRIENDSHIP_COUNT];
  size_t friend_offer_count;
#endif
  struct virtual_label virtual_label[MAX_VIRTUAL_LABELS];
  size_t virtual_label_count;
  struct mesh_key key[MAX_KEYS];
};

// -----------------------------------------------------------------------------
// Private function definitions

#if DECODE_FRIENDSHIP
static sl_status_t add_friendship_key(struct sl_mesh_decoder *context,
                                      sl_mesh_key_handle net_key,
                                      psa_key_id_t enc_key,
                                      psa_key_id_t priv_key,
                                      uint8_t nid,
                                      uint16_t lpn_addr,
                                      uint16_t lpn_counter,
                                      uint16_t friend_addr,
                                      uint16_t friend_counter,
                                      sl_mesh_key_handle *handle);
#endif

static void delete_all_reassemblers(struct sl_mesh_decoder *context);

static void delete_all_reassemblers_for_key(struct sl_mesh_decoder *context,
                                            sl_mesh_key_handle net_key);

static bool valid_key(struct sl_mesh_decoder *context,
                      sl_mesh_key_handle n)
{
  uintptr_t k = (uintptr_t)n;
  if (k > MAX_KEYS || context->key[k].type == mesh_key_null) {
    return false;
  }
  return true;
}

static bool valid_net_key(struct sl_mesh_decoder *context,
                          sl_mesh_key_handle n)
{
  uintptr_t k = (uintptr_t)n;
  if (k > MAX_KEYS || context->key[k].type != mesh_key_net) {
    return false;
  }
  return true;
}

#if DECODE_FRIENDSHIP

static bool valid_friend_key(struct sl_mesh_decoder *context,
                             sl_mesh_key_handle n)
{
  uintptr_t k = (uintptr_t)n;
  if (k > MAX_KEYS || context->key[k].type != mesh_key_net_friend) {
    return false;
  }
  return true;
}

#endif

static void value2mem(uint64_t value,
                      uint8_t *ptr,
                      size_t len)
{
  size_t pos;
  for (pos = 0; pos < len; pos++) {
    ptr[len - pos - 1] = value & 0xff;
    value >>= 8;
  }
}

static void mem2value(const uint8_t *ptr,
                      size_t len,
                      uint64_t *value)
{
  uint64_t accum = 0;
  size_t pos;
  for (pos = 0; pos < len; pos++) {
    accum <<= 8;
    accum += ptr[pos];
  }
  *value = accum;
}

static sl_status_t build_symmetric_key(psa_algorithm_t alg,
                                       psa_key_usage_t usage,
                                       psa_key_type_t type,
                                       size_t bits,
                                       const uint8_t *data,
                                       size_t data_len,
                                       psa_key_id_t *ptr)
{
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

  psa_set_key_usage_flags(&attributes, usage);
  psa_set_key_algorithm(&attributes, alg);
  psa_set_key_type(&attributes, type);
  psa_set_key_bits(&attributes, bits);

  return psa_import_key(&attributes, data, data_len, ptr);
}

static sl_status_t build_aes_128_cmac_key(const uint8_t *data,
                                          psa_key_id_t *ptr)
{
  return build_symmetric_key(PSA_ALG_CMAC,
                             PSA_KEY_USAGE_SIGN_HASH,
                             PSA_KEY_TYPE_AES,
                             128,
                             data,
                             16,
                             ptr);
}

static sl_status_t build_aes_128_ecb_key(const uint8_t *data,
                                         psa_key_id_t *ptr)
{
  return build_symmetric_key(PSA_ALG_ECB_NO_PADDING,
                             PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_ENCRYPT,
                             PSA_KEY_TYPE_AES,
                             128,
                             data,
                             16,
                             ptr);
}

static sl_status_t build_aes_128_ccm_key(const uint8_t *data,
                                         size_t mac_len,
                                         psa_key_id_t *ptr)
{
  return build_symmetric_key(PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, mac_len),
                             PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_ENCRYPT,
                             PSA_KEY_TYPE_AES,
                             128,
                             data,
                             16,
                             ptr);
}

static sl_status_t hash(psa_key_id_t key,
                        psa_algorithm_t alg,
                        size_t hash_len,
                        const uint8_t *text_ptr,
                        size_t text_len,
                        uint8_t *result)
{
  psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
  size_t result_len = 0;
  sl_status_t sc = SL_STATUS_OK;

  sc = psa_mac_sign_setup(&operation, key, alg);
  if (sc) {
    goto out;
  }

  sc = psa_mac_update(&operation, text_ptr, text_len);
  if (sc) {
    goto out;
  }

  sc = psa_mac_sign_finish(&operation, result, hash_len, &result_len);
  if (sc) {
    goto out;
  }

  if (result_len != hash_len) {
    sc = SL_STATUS_FAIL;
    goto out;
  }

  out:
  psa_mac_abort(&operation);
  return sc;
}

static sl_status_t aes_128_cmac(psa_key_id_t k,
                                const uint8_t *text_ptr,
                                size_t text_len,
                                uint8_t *result)
{
  return hash(k, PSA_ALG_CMAC, 16, text_ptr, text_len, result);
}

static sl_status_t aes_128_ecb_encrypt(psa_key_id_t k,
                                       const uint8_t *text_ptr,
                                       uint8_t *result)
{
  psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
  size_t result_len = 0;
  int e = 0;

  e = psa_cipher_encrypt_setup(&operation, k, PSA_ALG_ECB_NO_PADDING);
  if (e) {
    goto out;
  }

  e = psa_cipher_update(&operation, text_ptr, 16, result, 16, &result_len);
  if (e) {
    goto out;
  }

  e = psa_cipher_finish(&operation, result + result_len, 16 - result_len, &result_len);
  if (e) {
    goto out;
  }

  out:
  psa_cipher_abort(&operation);
  return e;
}

static sl_status_t aes_128_ccm_decrypt(psa_key_id_t k,
                                       const uint8_t *nonce,
                                       const uint8_t *cipher_and_auth_ptr,
                                       size_t cipher_len,
                                       size_t auth_len,
                                       const uint8_t *additional_ptr,
                                       size_t additional_len,
                                       uint8_t *text_ptr,
                                       size_t text_len)
{
  psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, auth_len);
  sl_status_t sc = 0;

  size_t out_len = 0;
  sc = psa_aead_decrypt(k,
                        alg,
                        nonce,
                        13,
                        additional_ptr,
                        additional_len,
                        cipher_and_auth_ptr,
                        cipher_len + auth_len,
                        text_ptr,
                        text_len,
                        &out_len);
  if (sc != 0) {
    goto out;
  }

  out:
  return sc;
}

static sl_status_t cmac(const uint8_t *key_ptr,
                        const uint8_t *text_ptr,
                        size_t text_len,
                        uint8_t *result)
{
  psa_key_id_t k = MBEDTLS_SVC_KEY_ID_INIT;
  sl_status_t sc;

  sc = build_aes_128_cmac_key(key_ptr, &k);
  if (sc) {
    goto out;
  }

  sc = aes_128_cmac(k, text_ptr, text_len, result);

  out:
  psa_destroy_key(k);
  return sc;
}

static sl_status_t s1(const uint8_t *text_ptr,
                      size_t text_len,
                      uint8_t *result)
{
  return cmac((const uint8_t *)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", text_ptr, text_len, result);
}

static sl_status_t k1(const uint8_t *network_key,
                      const uint8_t *salt_ptr,
                      size_t salt_len,
                      const uint8_t *P_ptr,
                      size_t P_len,
                      uint8_t *retptr)
{
  uint8_t salt[16], T[16], result[16];
  sl_status_t sc;

  sc = s1(salt_ptr, salt_len, salt);
  if (sc) {
    goto out;
  }

  sc = cmac(salt, network_key, 16, T);
  if (sc) {
    goto out;
  }

  sc = cmac(T, P_ptr, P_len, result);
  if (sc) {
    goto out;
  }

  memcpy(retptr, result, 16);

  out:
  return sc;
}

static sl_status_t k2(const uint8_t *network_key,
                      const uint8_t *P_ptr,
                      size_t P_len,
                      uint8_t *nidptr,
                      uint8_t *encptr,
                      uint8_t *privptr)
{
  uint8_t salt[16], T[16], input[33], result[48];
  sl_status_t sc = SL_STATUS_OK;

  sc = s1((const uint8_t *)"smk2", 4, salt);
  if (sc) {
    goto out;
  }

  sc = cmac(salt, network_key, 16, T);
  if (sc) {
    goto out;
  }

  // T1
  memcpy(input, P_ptr, P_len);
  input[P_len] = 0x01;
  sc = cmac(T, input, P_len + 1, result + 0);
  if (sc) {
    goto out;
  }

  // T2
  memcpy(input, result, 16);
  memcpy(input + 16, P_ptr, P_len);
  input[16 + P_len] = 0x02;
  sc = cmac(T, input, 16 + P_len + 1, result + 16);
  if (sc) {
    goto out;
  }

  // T3
  memcpy(input, result + 16, 16);
  memcpy(input + 16, P_ptr, P_len);
  input[16 + P_len] = 0x03;
  sc = cmac(T, input, 16 + P_len + 1, result + 32);
  if (sc) {
    goto out;
  }

  *nidptr = result[15] & 0x7f;
  memcpy(encptr, result + 16, 16);
  memcpy(privptr, result + 32, 16);

  out:
  return sc;
}

static sl_status_t k3(const uint8_t *network_key,
                      uint8_t *retptr)
{
  uint8_t salt[16], T[16], result[16];
  sl_status_t sc;

  sc = s1((const uint8_t *)"smk3", 4, salt);
  if (sc) {
    goto out;
  }

  sc = cmac(salt, network_key, 16, T);
  if (sc) {
    goto out;
  }

  sc = cmac(T, (const uint8_t *)"id64\x01", 5, result);
  if (sc) {
    goto out;
  }

  memcpy(retptr, result + 8, 8);

  out:
  return sc;
}

static sl_status_t k4(const uint8_t *application_key,
                      uint8_t *retptr)
{
  uint8_t salt[16], T[16], result[16];
  sl_status_t sc;

  sc = s1((const uint8_t *)"smk4", 4, salt);
  if (sc) {
    goto out;
  }

  sc = cmac(salt, application_key, 16, T);
  if (sc) {
    goto out;
  }

  sc = cmac(T, (const uint8_t *)"id6\x01", 4, result);
  if (sc) {
    goto out;
  }

  *retptr = result[15] & 0x3f;

  out:
  return sc;
}

static void delete_key(struct mesh_key *k)
{
  switch (k->type) {
    case mesh_key_net:
      psa_destroy_key(k->net.identity_key);
      psa_destroy_key(k->net.beacon_key);
      psa_destroy_key(k->net.flooding_encryption_key);
      psa_destroy_key(k->net.flooding_privacy_key);
#if DECODE_DIRECTED_FORWARDING
      psa_destroy_key(k->net.directed_encryption_key);
      psa_destroy_key(k->net.directed_privacy_key);
#endif
      psa_destroy_key(k->net.private_beacon_key);
      break;
#if DECODE_FRIENDSHIP
    case mesh_key_net_friend:
      psa_destroy_key(k->net_friend.encryption_key);
      psa_destroy_key(k->net_friend.privacy_key);
      break;
#endif
    case mesh_key_app:
      psa_destroy_key(k->app.key);
      break;
    case mesh_key_dev:
      psa_destroy_key(k->dev.key);
      break;
    default:
      break;
  }
  memset(k, 0, sizeof(struct mesh_key));
}

static uintptr_t key_index(struct sl_mesh_decoder *context, struct mesh_key *k)
{
  return k - context->key;
}

static struct mesh_key *find_unused_key(struct sl_mesh_decoder *context)
{
  size_t k;
  for (k = 0; k < MAX_KEYS; k++) {
    if (context->key[k].type == mesh_key_null) {
      return &context->key[k];
    }
  }
  return NULL;
}

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_mesh_decoder_create(struct sl_mesh_decoder **context)
{
  if (!context) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  struct sl_mesh_decoder *d = calloc(1, sizeof(struct sl_mesh_decoder));
  if (!d) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  *context = d;
  return SL_STATUS_OK;
}

sl_status_t sl_mesh_decoder_delete(struct sl_mesh_decoder *context)
{
  size_t k;

  if (!context) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  delete_all_reassemblers(context);

  for (k = 0; k < MAX_KEYS; k++) {
    delete_key(&context->key[k]);
  }

  free(context);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_decoder_add_virtual_label_uuid(struct sl_mesh_decoder *context,
                                                   const uint8_t *label_uuid)
{
  sl_status_t sc;

  if (!context || !label_uuid) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (context->virtual_label_count == MAX_VIRTUAL_LABELS) {
    return SL_STATUS_FULL;
  }

  size_t v;
  for (v = 0; v < context->virtual_label_count; v++) {
    if (memcmp(context->virtual_label[v].uuid, label_uuid, 16) == 0) {
      return SL_STATUS_ALREADY_EXISTS;
    }
  }

  uint8_t salt[16], label_hash[16];

  sc = s1((const uint8_t *)"vtad", 4, salt);
  if (sc) {
    return sc;
  }

  sc = cmac(salt, label_uuid, 16, label_hash);
  if (sc) {
    return sc;
  }

  memcpy(context->virtual_label[v].uuid, label_uuid, 16);
  context->virtual_label[v].hash = 0x8000 | ((label_hash[14] & 0x3f) << 8) | label_hash[15];
  context->virtual_label_count++;

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_decoder_remove_virtual_label_uuid(struct sl_mesh_decoder *context,
                                                      const uint8_t *label_uuid)
{
  if (!context || !label_uuid) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  size_t v;
  for (v = 0; v < context->virtual_label_count; v++) {
    if (memcmp(context->virtual_label[v].uuid, label_uuid, 16) == 0) {
      memmove(&context->virtual_label[v],
              &context->virtual_label[v + 1],
              (context->virtual_label_count - v - 1) * sizeof(struct virtual_label));
      context->virtual_label_count--;
      return SL_STATUS_OK;
    }
  }

  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_mesh_decoder_add_network_key(struct sl_mesh_decoder *context,
                                            const uint8_t *key_value,
                                            sl_mesh_key_handle *handle)
{
  sl_status_t sc;
  uint8_t fnid;
#if DECODE_DIRECTED_FORWARDING
  uint8_t dnid;
#endif
  uint8_t id[16], beacon[16], fenc[16], fpriv[16], pbeacon[16], netid[8];
#if DECODE_DIRECTED_FORWARDING
  uint8_t denc[16], dpriv[16];
#endif
  struct mesh_key *k;

  if (!context || !key_value || !handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  k = find_unused_key(context);
  if (!k) {
    return SL_STATUS_FULL;
  }

  sc = k2(key_value, (const uint8_t *)"\x00", 1, &fnid, fenc, fpriv);
  if (sc) {
    return sc;
  }

#if DECODE_DIRECTED_FORWARDING
  sc = k2(key_value, (const uint8_t *)"\x02", 1, &dnid, denc, dpriv);
  if (sc) {
    return sc;
  }
#endif

  sc = k1(key_value, (const uint8_t *)"nkik", 4, (const uint8_t *)"id128\x01", 6, id);
  if (sc) {
    return sc;
  }

  sc = k1(key_value, (const uint8_t *)"nkbk", 4, (const uint8_t *)"id128\x01", 6, beacon);
  if (sc) {
    return sc;
  }

  sc = k1(key_value, (const uint8_t *)"nkpk", 4, (const uint8_t *)"id128\x01", 6, pbeacon);
  if (sc) {
    return sc;
  }

  sc = k3(key_value, netid);
  if (sc) {
    return sc;
  }

  k->type = mesh_key_net;

  sc = build_aes_128_ccm_key(fenc, 4, &k->net.flooding_encryption_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }

#if DECODE_DIRECTED_FORWARDING
  sc = build_aes_128_ccm_key(denc, 4, &k->net.directed_encryption_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }
#endif

  sc = build_aes_128_ecb_key(fpriv, &k->net.flooding_privacy_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }

#if DECODE_DIRECTED_FORWARDING
  sc = build_aes_128_ecb_key(dpriv, &k->net.directed_privacy_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }
#endif

  sc = build_aes_128_cmac_key(beacon, &k->net.beacon_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }

  sc = build_aes_128_ecb_key(pbeacon, &k->net.private_beacon_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }

  sc = build_aes_128_ecb_key(id, &k->net.identity_key);
  if (sc != SL_STATUS_OK) {
    goto out;
  }

  memcpy(k->net.net_key, key_value, 16);
  memcpy(k->net.network_id, netid, 8);
  k->net.flooding_nid = fnid;
#if DECODE_DIRECTED_FORWARDING
  k->net.directed_nid = dnid;
#endif

  *handle = (void *)key_index(context, k);

  out:
  if (sc != SL_STATUS_OK) {
    delete_key(k);
  }

  return sc;
}

sl_status_t sl_mesh_decoder_add_application_key(struct sl_mesh_decoder *context,
                                                const uint8_t *key_value,
                                                sl_mesh_key_handle *handle)
{
  sl_status_t sc;
  uint8_t aid;
  struct mesh_key *k;

  if (!context || !key_value || !handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  k = find_unused_key(context);
  if (!k) {
    return SL_STATUS_FULL;
  }

  sc = k4(key_value, &aid);
  if (sc) {
    return sc;
  }

  sc = build_aes_128_ccm_key(key_value, 4, &k->app.key);
  if (sc == SL_STATUS_OK) {
    k->type = mesh_key_app;
    k->app.aid = aid;
    *handle = (void *)key_index(context, k);
  }

  return sc;
}

sl_status_t sl_mesh_decoder_add_device_key(struct sl_mesh_decoder *context,
                                           const uint8_t *key_value,
                                           uint16_t device_addr,
                                           sl_mesh_key_handle *handle)
{
  sl_status_t sc;
  struct mesh_key *k;

  if (!context || !key_value || !handle || !device_addr || device_addr > 0x7fff) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  k = find_unused_key(context);
  if (!k) {
    return SL_STATUS_FULL;
  }

  sc = build_aes_128_ccm_key(key_value, 4, &k->dev.key);
  if (sc == SL_STATUS_OK) {
    k->type = mesh_key_dev;
    k->dev.addr = device_addr;
    *handle = (void *)key_index(context, k);
  }

  return sc;
}

sl_status_t sl_mesh_decoder_remove_key(struct sl_mesh_decoder *context,
                                       sl_mesh_key_handle handle)
{
  uintptr_t k = (uintptr_t)handle;
  if (!context || !valid_key(context, handle)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (context->key[k].type == mesh_key_net) {
#if DECODE_FRIENDSHIP
    // When removing netkey, remove friendships for that key as well
    size_t f;
    for (f = 0; f < MAX_KEYS; f++) {
      if (context->key[f].type == mesh_key_net_friend && context->key[f].net_friend.net_key_index == k) {
        delete_key(&context->key[f]);
      }
    }
#endif

    // When removing netkey, remove reassemblers for that key as well --
    // we won't be adding any more segments without decrypting them
    delete_all_reassemblers_for_key(context, handle);
  }

  delete_key(&context->key[k]);
  return SL_STATUS_OK;
}

static sl_status_t net_decrypt(uint32_t current_ivindex,
                               psa_key_id_t enc_key,
                               psa_key_id_t priv_key,
                               const uint8_t *net_pdu_ptr,
                               size_t net_pdu_len,
                               struct sl_mesh_net_pdu *p)
{
  uint8_t privacy_plaintext[16], pecb[16], nonce[13];
  sl_status_t sc;
  size_t i, mic_len, cipher_len;

  // Privacy Random = (EncDST || EncTransportPDU || NetMIC)[0-6]
  // Privacy Plaintext = 0x0000000000 || IV Index || Privacy Random
  memset(privacy_plaintext, 0, 5);
  value2mem(current_ivindex, privacy_plaintext + 5, 4);
  memcpy(privacy_plaintext + 9, net_pdu_ptr + 7, 7);

  // PECB = e(PrivacyKey, Privacy Plaintext)
  sc = aes_128_ecb_encrypt(priv_key,
                           privacy_plaintext,
                           pecb);
  if (sc) {
    return sc;
  }

  // (CTL || TTL || SEQ || SRC) = ObfuscatedData (+) PECB[0-5]
  for (i = 0; i < 6; i++) {
    p->pdu_data[1 + i] = net_pdu_ptr[1 + i] ^ pecb[i];
  }

  mic_len = sl_mesh_net_pdu_ctl(p) ? MIC64_LEN : MIC32_LEN;
  if (net_pdu_len < NET_HEADER_LEN + 1 + mic_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  cipher_len = net_pdu_len - NET_HEADER_LEN - mic_len;

  nonce[0] = 0x00; // Nonce type
  memcpy(nonce + 1, p->pdu_data + 1, 6); // CTL || TTL || SEQ || SRC
  nonce[7] = nonce[8] = 0x00; // Pad
  value2mem(current_ivindex, nonce + 9, 4); // IV index

  sc = aes_128_ccm_decrypt(enc_key,
                           nonce,
                           net_pdu_ptr + 7,
                           cipher_len + 2,
                           mic_len,
                           NULL,
                           0,
                           p->pdu_data + 7,
                           cipher_len + 2);
  if (sc == SL_STATUS_OK) {
    memcpy(p->pdu_data + NET_HEADER_LEN + cipher_len,
           net_pdu_ptr + NET_HEADER_LEN + cipher_len,
           mic_len);
  }

  return sc;
}

sl_status_t sl_mesh_decoder_decrypt_net_pdu(struct sl_mesh_decoder *context,
                                            uint32_t current_ivindex,
                                            const uint8_t *net_pdu_ptr,
                                            size_t net_pdu_len,
                                            struct sl_mesh_net_pdu *pdu,
                                            sl_mesh_key_handle *matching_net_key,
                                            sl_mesh_key_handle *matching_friend_key)
{
  struct sl_mesh_net_pdu p;
  uint8_t ivi;
  uint8_t nid;
  size_t k;
  sl_status_t sc;

  if (!context || !net_pdu_ptr || !pdu || !matching_net_key || !matching_friend_key) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (net_pdu_len > SL_MESH_MAX_NET_PDU_LEN || net_pdu_len < MIN_PDU_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  p.pdu_data[0] = net_pdu_ptr[0];
  p.pdu_len = net_pdu_len;

  nid = sl_mesh_net_pdu_nid(&p);
  ivi = sl_mesh_net_pdu_ivi(&p);
  if (current_ivindex && (current_ivindex & 0x1) != ivi) {
    // If low bit does not match, use the previous IV index value
    current_ivindex--;
  }

  // Try decrypting with network keys and known friendships

  for (k = 0; k < MAX_KEYS; k++) {
    psa_key_id_t enc_key, priv_key;

    // Potential key?
    if (context->key[k].type == mesh_key_net) {
      if (context->key[k].net.flooding_nid == nid) {
        enc_key = context->key[k].net.flooding_encryption_key;
        priv_key = context->key[k].net.flooding_privacy_key;
#if DECODE_DIRECTED_FORWARDING
      } else if (context->key[k].net.directed_nid == nid) {
        enc_key = context->key[k].net.directed_encryption_key;
        priv_key = context->key[k].net.directed_privacy_key;
#endif
      } else {
        continue;
      }
#if DECODE_FRIENDSHIP
    } else if (context->key[k].type == mesh_key_net_friend) {
      if (context->key[k].net_friend.nid == nid) {
        enc_key = context->key[k].net_friend.encryption_key;
        priv_key = context->key[k].net_friend.privacy_key;
      } else {
        continue;
      }
#endif
    } else {
      continue;
    }

    sc = net_decrypt(current_ivindex, enc_key, priv_key,
                     net_pdu_ptr, net_pdu_len,
                     &p);
    if (sc == SL_STATUS_OK) {
      memcpy(pdu, &p, sizeof(struct sl_mesh_net_pdu));
      if (context->key[k].type == mesh_key_net) {
        *matching_net_key = (void *)k;
        *matching_friend_key = (void *)SL_MESH_KEY_HANDLE_INVALID;
#if DECODE_FRIENDSHIP
      } else {
        *matching_net_key = (void *)context->key[k].net_friend.net_key_index;
        *matching_friend_key = (void *)k;
#endif
      }
      return SL_STATUS_OK;
    }
  }

#if DECODE_FRIENDSHIP
  // If nothing matched, it might be encrypted with friendship
  // credentials we don't know yet but can guess

  size_t r, o;
  for (r = 0; r < context->friend_request_count; r++) {
    sl_mesh_key_handle net_key = context->friend_request[r].net_key;
    sl_mesh_key_handle handle;
    uintptr_t n = (uintptr_t)net_key;

    uint16_t lpn_addr = context->friend_request[r].lpn_addr;
    uint16_t lpn_counter = context->friend_request[r].lpn_counter;
    for (o = 0; o < context->friend_offer_count; o++) {
      if (context->friend_offer[o].lpn_addr != lpn_addr
          || context->friend_offer[o].net_key != net_key) {
        continue;
      }

      psa_key_id_t enc_key, priv_key;
      uint8_t nid, enc[16], priv[16];
      uint16_t friend_addr = context->friend_offer[o].friend_addr;
      uint16_t friend_counter = context->friend_offer[o].friend_counter;
      uint8_t P[9];

      P[0] = 0x01;
      value2mem(lpn_addr, P + 1, 2);
      value2mem(friend_addr, P + 3, 2);
      value2mem(lpn_counter, P + 5, 2);
      value2mem(friend_counter, P + 7, 2);

      if (k2(context->key[n].net.net_key, P, sizeof(P), &nid, enc, priv) != SL_STATUS_OK) {
        continue;
      }

      sc = build_aes_128_ccm_key(enc, 4, &enc_key);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      sc = build_aes_128_ecb_key(priv, &priv_key);
      if (sc != SL_STATUS_OK) {
        psa_destroy_key(enc_key);
        continue;
      }

      sc = net_decrypt(current_ivindex, enc_key, priv_key,
                       net_pdu_ptr, net_pdu_len,
                       &p);
      if (sc != SL_STATUS_OK) {
        psa_destroy_key(enc_key);
        psa_destroy_key(priv_key);
        continue;
      }

      // If decryption worked the friendship exists

      sc = add_friendship_key(context,
                              net_key,
                              enc_key,
                              priv_key,
                              nid,
                              lpn_addr,
                              lpn_counter,
                              friend_addr,
                              friend_counter,
                              &handle);
      if (sc != SL_STATUS_OK) {
        psa_destroy_key(enc_key);
        psa_destroy_key(priv_key);
        continue;
      }

      memcpy(pdu, &p, sizeof(struct sl_mesh_net_pdu));
      *matching_net_key = net_key;
      *matching_friend_key = handle;

      // To finalize, we can remove the matching incomplete structures

      memmove(&context->friend_offer[o],
              &context->friend_offer[o + 1],
              (context->friend_offer_count - o - 1) * sizeof(struct friend_offer));
      context->friend_offer_count--;

      memmove(&context->friend_request[r],
              &context->friend_request[r + 1],
              (context->friend_request_count - r - 1) * sizeof(struct friend_request));
      context->friend_request_count--;

      return SL_STATUS_OK;
    }
  }
#endif

  return SL_STATUS_SECURITY_DECRYPT_ERROR;
}

bool sl_mesh_net_pdu_is_valid(const struct sl_mesh_net_pdu *pdu)
{
  if (!pdu || pdu->pdu_len < MIN_PDU_LEN || pdu->pdu_len > SL_MESH_MAX_NET_PDU_LEN) {
    return false;
  }

  size_t mic_len = sl_mesh_net_pdu_mic_len(pdu);
  if (pdu->pdu_len < NET_HEADER_LEN + 1 + mic_len) {
    return false;
  }

  return true;
}

uint32_t sl_mesh_net_pdu_ivi(const struct sl_mesh_net_pdu *pdu)
{
  return (pdu->pdu_data[0] & 0x80) ? 1 : 0;
}

uint8_t sl_mesh_net_pdu_nid(const struct sl_mesh_net_pdu *pdu)
{
  return (pdu->pdu_data[0] & 0x7f);
}

uint8_t sl_mesh_net_pdu_ctl(const struct sl_mesh_net_pdu *pdu)
{
  return (pdu->pdu_data[1] & 0x80) ? 1 : 0;
}

uint8_t sl_mesh_net_pdu_ttl(const struct sl_mesh_net_pdu *pdu)
{
  return (pdu->pdu_data[1] & 0x7f);
}

uint32_t sl_mesh_net_pdu_seq(const struct sl_mesh_net_pdu *pdu)
{
  uint64_t value;
  mem2value(pdu->pdu_data + 2, 3, &value);
  return (uint32_t)value;
}

uint16_t sl_mesh_net_pdu_src(const struct sl_mesh_net_pdu *pdu)
{
  uint64_t value;
  mem2value(pdu->pdu_data + 5, 2, &value);
  return (uint16_t)value;
}

uint16_t sl_mesh_net_pdu_dst(const struct sl_mesh_net_pdu *pdu)
{
  uint64_t value;
  mem2value(pdu->pdu_data + 7, 2, &value);
  return (uint16_t)value;
}

const uint8_t *sl_mesh_net_pdu_data_ptr(const struct sl_mesh_net_pdu *pdu)
{
  return pdu->pdu_data + NET_HEADER_LEN;
}

size_t sl_mesh_net_pdu_data_len(const struct sl_mesh_net_pdu *pdu)
{
  return pdu->pdu_len - NET_HEADER_LEN - sl_mesh_net_pdu_mic_len(pdu);
}

const uint8_t *sl_mesh_net_pdu_mic_ptr(const struct sl_mesh_net_pdu *pdu)
{
  return pdu->pdu_data + NET_HEADER_LEN + sl_mesh_net_pdu_data_len(pdu);
}

size_t sl_mesh_net_pdu_mic_len(const struct sl_mesh_net_pdu *pdu)
{
  return sl_mesh_net_pdu_ctl(pdu) ? MIC64_LEN : MIC32_LEN;
}

static size_t trans_pdu_header_len(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  uint8_t seg = ptr[0] & 0x80;
  return (seg) ? 4 : 1;
}

bool sl_mesh_net_pdu_contains_valid_trans_pdu(const struct sl_mesh_net_pdu *pdu)
{
  if (!sl_mesh_net_pdu_is_valid(pdu)) {
    return false;
  }

  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  size_t len = sl_mesh_net_pdu_data_len(pdu);

  uint8_t seg = ptr[0] & 0x80;
  uint8_t ctl = sl_mesh_net_pdu_ctl(pdu);
  size_t min = (ctl) ? ((seg) ? 5 : 1) : ((seg) ? 5 : 6);

  if (len < min) {
    return false;
  }

  return true;
}

uint8_t sl_mesh_trans_pdu_seg(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return (ptr[0] & 0x80) ? 1 : 0;
}

uint16_t sl_mesh_trans_pdu_seg_seq0(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  uint16_t result = 0;
  result += ((uint16_t)ptr[1] & 0x7f) << 6;
  result += ((uint16_t)ptr[2] & 0xfc) >> 2;
  return result;
}

uint8_t sl_mesh_trans_pdu_seg_o(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  uint8_t result = 0;
  result += (ptr[2] & 0x03) << 3;
  result += (ptr[3] & 0xe0) >> 5;
  return result;
}

uint8_t sl_mesh_trans_pdu_seg_n(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return (ptr[3] & 0x1f);
}

uint8_t sl_mesh_trans_pdu_access_akf(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return (ptr[0] & 0x40) ? 1 : 0;
}

uint8_t sl_mesh_trans_pdu_access_aid(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return (ptr[0] & 0x3f);
}

uint8_t sl_mesh_trans_pdu_access_szmic(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return sl_mesh_trans_pdu_seg(pdu) ? ((ptr[1] & 0x80) ? 1 : 0) : 0;
}

uint8_t sl_mesh_trans_pdu_control_opcode(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  return (ptr[0] & 0x7f);
}

const uint8_t *sl_mesh_trans_pdu_data_ptr(const struct sl_mesh_net_pdu *pdu)
{
  const uint8_t *ptr = sl_mesh_net_pdu_data_ptr(pdu);
  size_t offset = trans_pdu_header_len(pdu);
  return ptr + offset;
}

size_t sl_mesh_trans_pdu_data_len(const struct sl_mesh_net_pdu *pdu)
{
  const size_t offset = trans_pdu_header_len(pdu);
  return sl_mesh_net_pdu_data_len(pdu) - offset;
}

static struct reassembler *find_reassembler(struct sl_mesh_decoder *context,
                                            uint16_t src,
                                            uint16_t dst,
                                            uint16_t seq_0)
{
  struct reassembler *r = context->reassemblers;
  while (r) {
    if (r->src == src && r->dst == dst && r->seq_0 == seq_0) {
      break;
    }
    r = r->next;
  }
  return r;
}

static void raise_reassembler_to_top(struct sl_mesh_decoder *context,
                                     struct reassembler *r)
{
  if (context->reassemblers == r) {
    return;
  }

  if (r->prev) {
    r->prev->next = r->next;
  }
  if (r->next) {
    r->next->prev = r->prev;
  }

  r->prev = NULL;
  r->next = context->reassemblers;
  r->next->prev = r;
  context->reassemblers = r;
}

static void delete_reassembler(struct sl_mesh_decoder *context,
                               struct reassembler *r)
{
  if (r->prev) {
    r->prev->next = r->next;
  }
  if (r->next) {
    r->next->prev = r->prev;
  }
  if (context->reassemblers == r) {
    context->reassemblers = r->next;
  }
  free(r);
  context->reassembler_count--;
}

static void delete_last_reassembler(struct sl_mesh_decoder *context)
{
  struct reassembler *r = context->reassemblers;
  if (!r) {
    return;
  }

  while (r->next) {
    r = r->next;
  }

  if (r->prev) {
    r->prev->next = NULL;
  }
  if (context->reassemblers == r) {
    context->reassemblers = NULL;
  }
  free(r);
  context->reassembler_count--;
}

static void delete_all_reassemblers(struct sl_mesh_decoder *context)
{
  while (context->reassemblers) {
    delete_reassembler(context, context->reassemblers);
  }
}

static void delete_all_reassemblers_for_key(struct sl_mesh_decoder *context,
                                            sl_mesh_key_handle net_key)
{
  struct reassembler *r = context->reassemblers;

  while (r) {
    struct reassembler *next = r->next;
    if (r->net_key == net_key) {
      delete_reassembler(context, r);
    }
    r = next;
  }
}

static sl_status_t create_reassembler(struct sl_mesh_decoder *context,
                                      sl_mesh_key_handle net_key,
                                      uint8_t ctl,
                                      uint32_t seq,
                                      uint16_t src,
                                      uint16_t dst,
                                      uint16_t seq_0,
                                      uint8_t seg_n,
                                      uint8_t ctl_opcode,
                                      uint8_t akf,
                                      uint8_t aid,
                                      uint8_t szmic,
                                      struct reassembler **ret)
{
  while (context->reassembler_count >= MAX_REASSEMBLER_COUNT) {
    delete_last_reassembler(context);
  }

  size_t pdu_len = ((ctl ? MAX_TRANS_CONTROL_PDU_LEN : MAX_TRANS_ACCESS_PDU_LEN) - 4) * (seg_n + 1);
  struct reassembler *r = calloc(1, sizeof(struct reassembler) + pdu_len);
  if (!r) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  r->net_key = net_key;
  r->src = src;
  r->dst = dst;
  r->seq_0 = seq_0;
  r->seg_n = seg_n;
  r->ctl = ctl;
  r->ctl_opcode = ctl_opcode;
  r->akf = akf;
  r->aid = aid;
  r->szmic = szmic;
  r->bitmap = 0x00000000;
  r->pdu_len = pdu_len;

  // Sequence number for the access layer nonce is the sequence number
  // of the first transmission of the first PDU of the segment
  // train. This is partly contained in the 13-bit SeqZero and must be
  // reconstructed from received data.
  if (seq < 0x2000) {
    r->seq_auth = seq_0;
  } else {
    r->seq_auth = (seq & 0xffffe000) | seq_0;
    if (r->seq_auth > seq) {
      r->seq_auth -= 8192;
    }
  }

  r->prev = NULL;
  r->next = context->reassemblers;
  if (r->next) {
    r->next->prev = r;
  }
  context->reassemblers = r;
  context->reassembler_count++;
  *ret = r;

  return SL_STATUS_OK;
}

static bool reassembler_complete(struct reassembler *r)
{
  uint32_t check_bitmap = 0x00000000;
  if (r->seg_n == 31) {
    check_bitmap = 0xffffffff;
  } else {
    check_bitmap = (1 << (r->seg_n + 1)) - 1;
  }

  if (check_bitmap != r->bitmap) {
    return false;
  }

  return true;
}

sl_status_t sl_mesh_reassembler_add_segment(struct sl_mesh_decoder *context,
                                            sl_mesh_key_handle net_key,
                                            uint8_t ctl,
                                            uint32_t seq,
                                            uint16_t src,
                                            uint16_t dst,
                                            uint16_t seq_0,
                                            uint8_t seg_o,
                                            uint8_t seg_n,
                                            uint8_t ctl_opcode,
                                            uint8_t akf,
                                            uint8_t aid,
                                            uint8_t szmic,
                                            const uint8_t *segment_ptr,
                                            size_t segment_len,
                                            bool *now_completed)
{
  if (!context || !valid_net_key(context, net_key) || !src || src > 0x7fff || !dst || seg_o > seg_n || !segment_ptr || !segment_len || !now_completed) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // All but last segment must be full length
  size_t expected_len = (ctl ? MAX_TRANS_CONTROL_PDU_LEN : MAX_TRANS_ACCESS_PDU_LEN) - 4;
  if (seg_o < seg_n) {
    if (segment_len != expected_len) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  struct reassembler *r = find_reassembler(context, src, dst, seq_0);
  if (!r) {
    sl_status_t sc = create_reassembler(context, net_key, ctl, seq, src, dst, seq_0, seg_n,
                                        ctl_opcode, akf, aid, szmic, &r);
    if (sc) {
      return sc;
    }
  }

  // Parameters must match across segments
  if (ctl != r->ctl || net_key != r->net_key || seg_n != r->seg_n) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (r->ctl) {
    if (ctl_opcode != r->ctl_opcode) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  } else {
    if (akf != r->akf || aid != r->aid || szmic != r->szmic) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  *now_completed = false;

  // Already received?
  if (r->bitmap & (1 << seg_o)) {
    // If it's a retransmission of the last segment we should have the
    // same truncation result -- otherwise someone's playing games
    // with us
    if (seg_o == r->seg_n) {
      size_t expected_pdu_len = expected_len * (r->seg_n + 1);
      expected_pdu_len -= (expected_len - segment_len);
      if (expected_pdu_len != r->pdu_len) {
        return SL_STATUS_INVALID_PARAMETER;
      }
    }
    // Retransmission is not a problem, but make no modification to struct
    return SL_STATUS_OK;
  }

  size_t pdu_off = seg_o * expected_len;
  memcpy(r->pdu + pdu_off, segment_ptr, segment_len);
  r->bitmap |= (1 << seg_o);

  // Last segment may need trimming
  if (seg_o == r->seg_n) {
    r->pdu_len -= (expected_len - segment_len);
  }

  raise_reassembler_to_top(context, r);

  // Signal the moment when all payload is at hand and can be decoded
  if (reassembler_complete(r)) {
    *now_completed = true;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_reassembler_get_data(struct sl_mesh_decoder *context,
                                         uint16_t src,
                                         uint16_t dst,
                                         uint16_t seq_0,
                                         const uint8_t **ptr,
                                         size_t *len,
                                         uint32_t *seq_auth)
{
  if (!context || !ptr || !len || !seq_auth || !src || src > 0x7fff || !dst) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  struct reassembler *r = find_reassembler(context, src, dst, seq_0);
  if (!r) {
    return SL_STATUS_NOT_FOUND;
  }

  if (reassembler_complete(r)) {
    *ptr = r->pdu;
    *len = r->pdu_len;
    *seq_auth = r->seq_auth;

    return SL_STATUS_OK;
  }

  return SL_STATUS_NOT_READY;
}

static const size_t control_parameter_len[18][2] = {
  { 6, 6, },
  { 1, 1, },
  { 6, 6, },
  { 10, 10, },
  { 6, 6, },
  { 4, 4, },
  { 4, 4, },
  { 1, 255, },
  { 1, 255, },
  { 1, 1, },
  { 3, 3, },
  { 7, 11, },
  { 4, 10, },
  { 4, 4, },
  { 0, 0, },
  { 2, 2, },
  { 5, 6, },
  { 0, 256, },
};

enum control_parameter_length_constraint {
  control_parameter_length_any = 0,
  control_parameter_length_odd = 1,
  control_parameter_length_even = 2,
};

static const enum control_parameter_length_constraint control_parameter_con[18] = {
  control_parameter_length_even,
  control_parameter_length_odd,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_odd,
  control_parameter_length_odd,
  control_parameter_length_odd,
  control_parameter_length_odd,
  control_parameter_length_any,
  control_parameter_length_any,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_even,
  control_parameter_length_any,
  control_parameter_length_even,
};

bool sl_mesh_pointer_contains_valid_control_message(uint8_t opcode,
                                                    const uint8_t *ptr,
                                                    size_t len)
{
  if (opcode > sl_mesh_control_opcode_path_request_solicitation || !ptr || len > SL_MESH_MAX_CONTROL_PDU_LEN) {
    return false;
  }

  if (control_parameter_len[opcode][0] > len || control_parameter_len[opcode][1] < len) {
    return false;
  }

  if (control_parameter_con[opcode] == control_parameter_length_odd && (len & 0x1) == 0) {
    return false;
  }

  if (control_parameter_con[opcode] == control_parameter_length_even && (len & 0x1) == 1) {
    return false;
  }

  // PATH_REQUEST, PATH_REPLY, and DEPENDENT_NODE_UPDATE length
  // validity checking requires peeking into payload

  switch (opcode) {
    case sl_mesh_control_opcode_path_request:
    {
      size_t fixed_len = 5;
      size_t variable_len = 0;
      variable_len += (ptr[fixed_len] & 0x80) ? 3 : 2; // Path_Origin_Unicast_Addr_Range:LengthPresent
      if (ptr[0] & 0x80) { // On_Behalf_Of_Dependent_Origin
        if (fixed_len + variable_len + 2 > len) {
          return false;
        }
        variable_len += (ptr[fixed_len + variable_len] & 0x80) ? 3 : 2; // Dependent_Origin_Unicast_Addr_Range:LengthPresent
      }
      if (len != fixed_len + variable_len) {
        return false;
      }
    }
    break;
    case sl_mesh_control_opcode_path_reply:
    {
      size_t fixed_len = 4;
      size_t variable_len = 0;
      if (ptr[0] & 0x80) { // Unicast_Destination
        if (fixed_len + 2 > len) {
          return false;
        }
        variable_len += (ptr[fixed_len] & 0x80) ? 3 : 2; // Path_Target_Unicast_Addr_Range:LengthPresent
        if (ptr[0] & 0x40) { // On_Behalf_Of_Dependent_Origin
          if (fixed_len + variable_len + 2 > len) {
            return false;
          }
          variable_len += (ptr[fixed_len + variable_len] & 0x80) ? 3 : 2; // Dependent_Target_Unicast_Addr_Range:LengthPresent
        }
      }
      if (len != fixed_len + variable_len) {
        return false;
      }
    }
    break;
    case sl_mesh_control_opcode_dependent_node_update:
    {
      size_t fixed_len = 3;
      size_t variable_len = 0;
      variable_len += (ptr[fixed_len] & 0x80) ? 3 : 2; // Dependent_Node_Unicast_Addr_Range:LengthPresent
      if (len != fixed_len + variable_len) {
        return false;
      }
    }
    break;
    default:
      break;
  }

  return true;
}

bool sl_mesh_pointer_contains_valid_access_pdu(uint8_t szmic,
                                               const uint8_t *pdu_ptr,
                                               size_t pdu_len)
{
  size_t mic_len = szmic ? 8 : 4;
  return (pdu_len < 1 + mic_len) ? false : true;
}

sl_status_t sl_mesh_decoder_decrypt_access_pdu(struct sl_mesh_decoder *context,
                                               uint8_t ivi,
                                               uint8_t akf,
                                               uint8_t aid,
                                               uint8_t szmic,
                                               uint32_t seq,
                                               uint16_t src,
                                               uint16_t dst,
                                               uint32_t current_ivindex,
                                               const uint8_t *access_pdu_ptr,
                                               size_t access_pdu_len,
                                               struct sl_mesh_access_pdu *pdu,
                                               sl_mesh_key_handle *matching_app_key,
                                               const uint8_t **matching_label_uuid)
{
  struct sl_mesh_access_pdu p;
  uint8_t nonce[13];
  size_t k, cipher_len;
  size_t mic_len = szmic ? 8 : 4;
  bool virtual_addr;
  sl_status_t sc;

  if (!context || !access_pdu_ptr || !pdu || !matching_app_key || !matching_label_uuid) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!src || src > 0x7fff || !dst) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (dst > 0x7fff && !akf) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (access_pdu_len > SL_MESH_MAX_ACCESS_PDU_LEN || access_pdu_len < 1 + mic_len) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  virtual_addr = ((dst & 0xc000) == 0x8000) ? true : false;

  cipher_len = access_pdu_len - mic_len;

  if (current_ivindex && (current_ivindex & 0x1) != ivi) {
    // If low bit does not match, use the previous IV index value
    current_ivindex--;
  }

  for (k = 0; k < MAX_KEYS; k++) {
    // Potential key?
    if (akf) {
      if (context->key[k].type != mesh_key_app) {
        continue;
      }
      if (aid != context->key[k].app.aid) {
        continue;
      }
    } else {
      if (context->key[k].type != mesh_key_dev) {
        continue;
      }
      if (src != context->key[k].dev.addr && dst != context->key[k].dev.addr) {
        continue;
      }
    }

    nonce[0] = akf ? 0x01 : 0x02; // Nonce type
    nonce[1] = szmic ? 0x80 : 0x00; // ASZMIC and Pad
    value2mem(seq, nonce + 2, 3); // SEQ
    value2mem(src, nonce + 5, 2); // SRC
    value2mem(dst, nonce + 7, 2); // DST
    value2mem(current_ivindex, nonce + 9, 4); // IV index

    if (virtual_addr) { // Virtual address -- try matching labels
      size_t v;
      for (v = 0; v < context->virtual_label_count; v++) {
        if (context->virtual_label[v].hash == dst) {
          sc = aes_128_ccm_decrypt(context->key[k].app.key,
                                   nonce,
                                   access_pdu_ptr,
                                   cipher_len,
                                   mic_len,
                                   context->virtual_label[v].uuid,
                                   16,
                                   p.pdu_data,
                                   cipher_len);
          if (sc == SL_STATUS_OK) {
            p.pdu_len = cipher_len;
            memcpy(pdu, &p, sizeof(struct sl_mesh_access_pdu));
            *matching_app_key = (void *)k;
            *matching_label_uuid = context->virtual_label[v].uuid;
            return SL_STATUS_OK;
          }
        }
      }
    } else { // Non-virtual address
      sc = aes_128_ccm_decrypt(akf
                               ? context->key[k].app.key
                               : context->key[k].dev.key,
                               nonce,
                               access_pdu_ptr,
                               cipher_len,
                               mic_len,
                               NULL,
                               0,
                               p.pdu_data,
                               cipher_len);
      if (sc == SL_STATUS_OK) {
        p.pdu_len = cipher_len;
        memcpy(pdu, &p, sizeof(struct sl_mesh_access_pdu));
        *matching_app_key = (void *)k;
        *matching_label_uuid = NULL;
        return SL_STATUS_OK;
      }
    }
  }

  return SL_STATUS_SECURITY_DECRYPT_ERROR;
}

// Opcodes can be 1, 2, or 3 octets, depending on opcode value
static size_t opcode_len(const uint8_t p)
{
  if (p == 0x7f) { // RFU
    return 0;
  } else if ((p & 0x80) == 0x00) { // 1 octet opcode
    return 1;
  } else if ((p & 0xc0) == 0x80) { // 2 octet opcode
    return 2;
  } else { // 3 octet opcode
    return 3;
  }
}

bool sl_mesh_access_pdu_contains_valid_access_message(const struct sl_mesh_access_pdu *pdu)
{
  if (!pdu || !pdu->pdu_len) {
    return false;
  }

  size_t l = opcode_len(pdu->pdu_data[0]);
  return (l && pdu->pdu_len >= l);
}

const uint8_t *sl_mesh_access_pdu_opcode_ptr(const struct sl_mesh_access_pdu *pdu)
{
  return pdu->pdu_data;
}

size_t sl_mesh_access_pdu_opcode_len(const struct sl_mesh_access_pdu *pdu)
{
  return opcode_len(pdu->pdu_data[0]);
}

const uint8_t *sl_mesh_access_pdu_data_ptr(const struct sl_mesh_access_pdu *pdu)
{
  return pdu->pdu_data + opcode_len(pdu->pdu_data[0]);
}

size_t sl_mesh_access_pdu_data_len(const struct sl_mesh_access_pdu *pdu)
{
  return pdu->pdu_len - opcode_len(pdu->pdu_data[0]);
}

//
// Friendship handling
//

#if DECODE_FRIENDSHIP

static sl_status_t add_friendship_key(struct sl_mesh_decoder *context,
                                      sl_mesh_key_handle net_key,
                                      psa_key_id_t enc_key,
                                      psa_key_id_t priv_key,
                                      uint8_t nid,
                                      uint16_t lpn_addr,
                                      uint16_t lpn_counter,
                                      uint16_t friend_addr,
                                      uint16_t friend_counter,
                                      sl_mesh_key_handle *handle)
{
  struct mesh_key *k;

  k = find_unused_key(context);
  if (!k) {
    return SL_STATUS_FULL;
  }

  k->type = mesh_key_net_friend;
  k->net_friend.encryption_key = enc_key;
  k->net_friend.privacy_key = priv_key;
  k->net_friend.nid = nid;
  k->net_friend.net_key_index = (uintptr_t)net_key;
  k->net_friend.friendship.lpn_addr = lpn_addr;
  k->net_friend.friendship.lpn_counter = lpn_counter;
  k->net_friend.friendship.friend_addr = friend_addr;
  k->net_friend.friendship.friend_counter = friend_counter;

  *handle = (void *)key_index(context, k);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_decoder_remove_friendship_key(struct sl_mesh_decoder *context,
                                                  uint16_t lpn_addr,
                                                  sl_mesh_key_handle net_key_handle)
{
  if (!context || !lpn_addr || lpn_addr > 0x7fff || !valid_net_key(context, net_key_handle)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  size_t f;
  for (f = 0; f < MAX_KEYS; f++) {
    if (context->key[f].type == mesh_key_net_friend
        && context->key[f].net_friend.friendship.lpn_addr == lpn_addr
        && context->key[f].net_friend.net_key_index == (uintptr_t)net_key_handle) {
      delete_key(&context->key[f]);
      return SL_STATUS_OK;
    }
  }

  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_mesh_decoder_get_friendship_key(struct sl_mesh_decoder *context,
                                               uint16_t lpn_addr,
                                               uint16_t lpn_counter,
                                               sl_mesh_key_handle *handle)
{
  if (!context || !lpn_addr || lpn_addr > 0x7fff || !handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  size_t f;
  for (f = 0; f < MAX_KEYS; f++) {
    if (context->key[f].type == mesh_key_net_friend
        && context->key[f].net_friend.friendship.lpn_addr == lpn_addr
        && context->key[f].net_friend.friendship.lpn_counter == lpn_counter) {
      *handle = (void *)key_index(context, &context->key[f]);
      return SL_STATUS_OK;
    }
  }

  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_mesh_decoder_get_friendship_key_context(struct sl_mesh_decoder *context,
                                                       sl_mesh_key_handle handle,
                                                       struct sl_mesh_friendship *friendship)
{
  if (!context || !valid_friend_key(context, handle) || !friendship) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uintptr_t k = (uintptr_t)handle;
  memcpy(friendship,
         &context->key[k].net_friend.friendship,
         sizeof(struct sl_mesh_friendship));
  return SL_STATUS_OK;
}

//
// Friend Request and Friend Offer are sent with flooding credentials that do
// not yet contain the full context (lpn address, lpn counter, friend address,
// friend counter). We know which friendship is established only when we
// observe a message that decrypts using a context we can put together from
// the partial values we have seen so far.
//
// We thus need to keep track of incomplete potential friendship contexts
// until we see a Friend Poll or Friend Update that can be decrypted using
// one potential friendship context.
//

static struct friend_request *find_friend_request(struct sl_mesh_decoder *context,
                                                  sl_mesh_key_handle net_key,
                                                  uint16_t lpn_addr,
                                                  uint16_t lpn_counter)
{
  size_t i;
  for (i = 0; i < context->friend_request_count; i++) {
    if (context->friend_request[i].net_key == net_key
        && context->friend_request[i].lpn_addr == lpn_addr
        && context->friend_request[i].lpn_counter == lpn_counter) {
      // Raise matching item to top of the pile
      if (i) {
        struct friend_request tmp = context->friend_request[i];
        memmove(&context->friend_request[1],
                &context->friend_request[0],
                i * sizeof(struct friend_request));
        context->friend_request[0] = tmp;
      }
      return &context->friend_request[0];
    }
  }

  return NULL;
}

sl_status_t sl_mesh_friendship_add_friend_request(struct sl_mesh_decoder *context,
                                                  sl_mesh_key_handle net_key,
                                                  uint16_t lpn_addr,
                                                  uint16_t lpn_counter)
{
  if (!context || !valid_net_key(context, net_key) || !lpn_addr || lpn_addr > 0x7fff) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (find_friend_request(context, net_key, lpn_addr, lpn_counter)) {
    return SL_STATUS_OK;
  }

  // Remove oldest if array is full
  if (context->friend_request_count == MAX_INCOMPLETE_FRIENDSHIP_COUNT) {
    context->friend_request_count--;
  }

  // Insert on top
  if (context->friend_request_count) {
    memmove(&context->friend_request[1],
            &context->friend_request[0],
            context->friend_request_count * sizeof(struct friend_request));
  }

  struct friend_request r = {
    .net_key = net_key,
    .lpn_addr = lpn_addr,
    .lpn_counter = lpn_counter,
  };
  context->friend_request[0] = r;
  context->friend_request_count++;

  return SL_STATUS_OK;
}

static struct friend_offer *find_friend_offer(struct sl_mesh_decoder *context,
                                              sl_mesh_key_handle net_key,
                                              uint16_t lpn_addr,
                                              uint16_t friend_addr,
                                              uint16_t friend_counter)
{
  size_t i;
  for (i = 0; i < context->friend_offer_count; i++) {
    if (context->friend_offer[i].net_key == net_key
        && context->friend_offer[i].lpn_addr == lpn_addr
        && context->friend_offer[i].friend_addr == friend_addr
        && context->friend_offer[i].friend_counter == friend_counter) {
      // Raise matching item to top of the pile
      if (i) {
        struct friend_offer tmp = context->friend_offer[i];
        memmove(&context->friend_offer[1],
                &context->friend_offer[0],
                i * sizeof(struct friend_offer));
        context->friend_offer[0] = tmp;
      }
      return &context->friend_offer[0];
    }
  }

  return NULL;
}

sl_status_t sl_mesh_friendship_add_friend_offer(struct sl_mesh_decoder *context,
                                                sl_mesh_key_handle net_key,
                                                uint16_t lpn_addr,
                                                uint16_t friend_addr,
                                                uint16_t friend_counter)
{
  if (!context || !valid_net_key(context, net_key) || !lpn_addr || lpn_addr > 0x7fff || !friend_addr || friend_addr > 0x7fff) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (find_friend_offer(context, net_key, lpn_addr, friend_addr, friend_counter)) {
    return SL_STATUS_OK;
  }

  // Remove oldest if array is full
  if (context->friend_offer_count == MAX_INCOMPLETE_FRIENDSHIP_COUNT) {
    context->friend_offer_count--;
  }

  // Insert on top
  if (context->friend_offer_count) {
    memmove(&context->friend_offer[1],
            &context->friend_offer[0],
            context->friend_offer_count * sizeof(struct friend_offer));
  }

  struct friend_offer o = {
    .net_key = net_key,
    .lpn_addr = lpn_addr,
    .friend_addr = friend_addr,
    .friend_counter = friend_counter,
  };
  context->friend_offer[0] = o;
  context->friend_offer_count++;

  return SL_STATUS_OK;
}

#else

sl_status_t sl_mesh_decoder_remove_friendship_key(struct sl_mesh_decoder *context,
                                                  uint16_t lpn_addr,
                                                  sl_mesh_key_handle net_key_handle)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mesh_decoder_get_friendship_key(struct sl_mesh_decoder *context,
                                               uint16_t lpn_addr,
                                               uint16_t lpn_counter,
                                               sl_mesh_key_handle *handle)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mesh_friendship_add_friend_request(struct sl_mesh_decoder *context,
                                                  sl_mesh_key_handle net_key,
                                                  uint16_t lpn_addr,
                                                  uint16_t lpn_counter)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mesh_friendship_add_friend_offer(struct sl_mesh_decoder *context,
                                                sl_mesh_key_handle net_key,
                                                uint16_t lpn_addr,
                                                uint16_t friend_addr,
                                                uint16_t friend_counter)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mesh_decoder_get_friendship_key_context(struct sl_mesh_decoder *context,
                                                       sl_mesh_key_handle handle,
                                                       struct sl_mesh_friendship *friendship)
{
  return SL_STATUS_NOT_SUPPORTED;
}

#endif

sl_status_t sl_mesh_decoder_authenticate_network_beacon(struct sl_mesh_decoder *context,
                                                        const uint8_t *beacon_ptr,
                                                        size_t beacon_len,
                                                        enum sl_mesh_beacon *type,
                                                        uint8_t *flags,
                                                        uint32_t *iv_index,
                                                        sl_mesh_key_handle *matching_net_key)
{
  sl_status_t sc;
  size_t k;
  uint64_t i;

  if (!context || !beacon_ptr || !beacon_len
      || !type || !flags || !iv_index || !matching_net_key) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (beacon_ptr[0] == sl_mesh_beacon_secure) {
    if (beacon_len != 22) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    const uint8_t *network_id_ptr = beacon_ptr + 2;
    size_t network_id_len = 8;

    const uint8_t *input_ptr = beacon_ptr + 1;
    size_t input_len = 13;

    const uint8_t *auth_ptr = beacon_ptr + 14;
    size_t auth_len = 8;

    for (k = 0; k < MAX_KEYS; k++) {
      if (context->key[k].type != mesh_key_net) {
        continue;
      }

      if (memcmp(context->key[k].net.network_id, network_id_ptr, network_id_len) != 0) {
        continue;
      }

      uint8_t auth_value[16];
      sc = aes_128_cmac(context->key[k].net.beacon_key, input_ptr, input_len, auth_value);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      if (memcmp(auth_value, auth_ptr, auth_len) == 0) {
        *type = sl_mesh_beacon_secure;
        *flags = beacon_ptr[1];
        mem2value(beacon_ptr + 10, 4, &i);
        *iv_index = (uint32_t)i;
        *matching_net_key = (void *)k;
        return SL_STATUS_OK;
      }
    }

    return SL_STATUS_FAIL; // Not authenticated
  } else if (beacon_ptr[0] == sl_mesh_beacon_private) {
    if (beacon_len != 27) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    const uint8_t *random_ptr = beacon_ptr + 1;
    size_t random_len = 13;

    const uint8_t *obfuscated_ptr = beacon_ptr + 14;
    size_t obfuscated_len = 5;

    const uint8_t *auth_ptr = beacon_ptr + 19;
    size_t auth_len = 8;

    for (k = 0; k < MAX_KEYS; k++) {
      if (context->key[k].type != mesh_key_net) {
        continue;
      }

      uint8_t S[16], C1[16];

      C1[0] = 0x01;
      memcpy(C1 + 1, random_ptr, random_len);
      C1[14] = 0x00;
      C1[15] = 0x01;

      sc = aes_128_ecb_encrypt(context->key[k].net.private_beacon_key, C1, S);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      uint8_t T0[16], T1[16], T2[16], B0[16], C0[16], P[16];
      uint8_t deobfuscated[5];
      size_t d;
      for (d = 0; d < obfuscated_len; d++) {
        deobfuscated[d] = S[d] ^ obfuscated_ptr[d];
      }

      B0[0] = 0x19;
      memcpy(B0 + 1, random_ptr, random_len);
      B0[14] = 0x00;
      B0[15] = 0x05;

      sc = aes_128_ecb_encrypt(context->key[k].net.private_beacon_key, B0, T0);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      memset(P, 0, 16);
      memcpy(P, deobfuscated, obfuscated_len);
      for (d = 0; d < 16; d++) {
        T0[d] = T0[d] ^ P[d];
      }

      sc = aes_128_ecb_encrypt(context->key[k].net.private_beacon_key, T0, T1);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      C0[0] = 0x01;
      memcpy(C0 + 1, random_ptr, random_len);
      C0[14] = 0x00;
      C0[15] = 0x00;

      sc = aes_128_ecb_encrypt(context->key[k].net.private_beacon_key, C0, T2);
      if (sc != SL_STATUS_OK) {
        continue;
      }

      for (d = 0; d < 16; d++) {
        T2[d] = T1[d] ^ T2[d];
      }

      if (memcmp(T2, auth_ptr, auth_len) == 0) {
        *type = sl_mesh_beacon_private;
        *flags = deobfuscated[0];
        mem2value(deobfuscated + 1, 4, &i);
        *iv_index = (uint32_t)i;
        *matching_net_key = (void *)k;
        return SL_STATUS_OK;
      }
    }

    return SL_STATUS_FAIL; // Not authenticated
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }
}

/*******************************************************************************
 *  Get a 16-bit property ID from a buffer.
 ******************************************************************************/
static uint16_t get_property_id(const uint8_t *buf)
{
  return buf[0] | (buf[1] << 8);
}

/*******************************************************************************
 *  Swaps the endianness of a given byte sequence.
 ******************************************************************************/
static uint8_t* swap_endianness(uint8_t *ptr, size_t len)
{
  uint8_t tmp[len];
  memcpy(tmp, ptr, len);
  size_t pos = 0;
  while (pos < len) {
    ptr[pos] = tmp[len - pos - 1];
    pos++;
  }
  return ptr;
}

float sl_mesh_decoder_time_to_sec(uint32_t interval)
{
  return powf(1.1f, (float)(interval - 64));
}

float sl_mesh_decoder_tolerance_to_error_percentage(uint16_t tolerance)
{
  return (float)tolerance / 4095.0f * 100.0f;
}

sl_status_t sl_mesh_sensor_descriptors_from_buf(sensor_descriptor_t *descriptor,
                                                const uint8_t *pdu_data,
                                                const size_t pdu_data_len)
{
  size_t pos = 0;
  size_t idx = 0;

  // only property id is present
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    descriptor[idx].property_id = get_property_id(pdu_data);
    return SL_STATUS_OK;
  }

  // Descriptor array length should be a multiply of 8
  if (pdu_data_len & 0x07) {
    return SL_STATUS_INVALID_PARAMETER;
  } else {
    while (pos < pdu_data_len) {
      descriptor[idx].property_id = pdu_data[pos] | (pdu_data[pos + 1] << 8);
      descriptor[idx].positive_tolerance = pdu_data[pos + 2] | ((pdu_data[pos + 3] & 0x0F) << 8);
      descriptor[idx].negative_tolerance = (pdu_data[pos + 3] >> 4) | (pdu_data[pos + 4] << 4);
      descriptor[idx].sampling_function = pdu_data[pos + 5];
      descriptor[idx].measurement_period = pdu_data[pos + 6];
      descriptor[idx].update_interval = pdu_data[pos + 7];
      pos += 8;
      idx++;
    }
  }
  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_cadence_from_buf(struct sensort_cadence_state *cadence,
                                            uint8_t *pdu_data,
                                            const size_t pdu_data_len)
{
  uint8_t sensor_data_size = 0;
  uint16_t pos = 0;

  // Ensure buffer contains property property ID
  if (pdu_data_len < SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  cadence->property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;

  // only property id is present
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_OK;
  }

  // Period divisor is 7 bits, status trigger type is 1 bit
  cadence->period_divisor = (pdu_data[pos] & 0xFE) >> 1;
  cadence->status_trigger_type = (pdu_data[pos] & 0x01);
  pos++;

  sensor_data_size = sl_mesh_decoder_util_get_sensor_data_size(cadence->property_id);

  // status trigger format depends on status_trigger_type: UINT16 if
  // status_trigger_type is equal to 1, otherwise matches sensor data size
  if (cadence->status_trigger_type == 1) {
    cadence->status_trigger_delta_down.size = 2;
    cadence->status_trigger_delta_up.size = 2;
  } else {
    cadence->status_trigger_delta_down.size = sensor_data_size;
    cadence->status_trigger_delta_up.size = sensor_data_size;
  }

  cadence->status_trigger_delta_down.value = swap_endianness(pdu_data + pos, cadence->status_trigger_delta_down.size);
  pos = pos + cadence->status_trigger_delta_down.size;

  cadence->status_trigger_delta_up.value = swap_endianness(pdu_data + pos, cadence->status_trigger_delta_up.size);
  pos = pos + cadence->status_trigger_delta_up.size;

  cadence->min_interval = pdu_data[pos];
  pos++;

  cadence->fast_cadence_low.size = sensor_data_size;
  cadence->fast_cadence_low.value = swap_endianness(pdu_data + pos, sensor_data_size);
  pos = pos + sensor_data_size;

  cadence->fast_cadence_high.size = sensor_data_size;
  cadence->fast_cadence_high.value = swap_endianness(pdu_data + pos, sensor_data_size);
  pos = pos + sensor_data_size;

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_setting_set_from_buf(sensor_set_setting_msg_t *setting_set,
                                                uint8_t *pdu_data,
                                                const size_t pdu_data_len)
{
  uint8_t sensor_data_size = 0;
  uint16_t pos = 0;
  // Ensure buffer contains property and setting property IDs
  if (pdu_data_len < SL_MESH_PROPERTY_ID_LEN * 2) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  setting_set->property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;
  setting_set->setting_property_id = get_property_id(pdu_data + pos);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;

  sensor_data_size = sl_mesh_decoder_util_get_sensor_data_size(setting_set->setting_property_id);

  setting_set->setting_raw_len = sensor_data_size;
  setting_set->setting_raw = swap_endianness(pdu_data + pos, sensor_data_size);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_setting_get_from_buf(sensor_get_setting_msg_t *setting_get,
                                                uint8_t *pdu_data,
                                                const size_t pdu_data_len)
{
  if (pdu_data_len != SL_MESH_PROPERTY_ID_LEN * 2) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  setting_get->property_id = get_property_id(pdu_data);
  setting_get->setting_property_id = get_property_id(pdu_data + SL_MESH_PROPERTY_ID_LEN);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_setting_status_from_buf(struct sensor_setting_status *setting_status,
                                                   uint8_t *pdu_data,
                                                   const size_t pdu_data_len)
{
  uint8_t sensor_data_size = 0;
  uint16_t pos = 0;

  // Ensure buffer contains property and setting property IDs
  if (pdu_data_len < SL_MESH_PROPERTY_ID_LEN * 2) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  setting_status->property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;
  setting_status->settings_property_id = get_property_id(pdu_data + pos);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;

  // access and setting raw fields are optional
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN * 2) {
    return SL_STATUS_OK;
  }

  sensor_data_size = sl_mesh_decoder_util_get_sensor_data_size(setting_status->settings_property_id);
  // if access field is present, then raw field also should be present
  if ((pdu_data_len - pos) == 1) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  setting_status->access = pdu_data[pos];
  pos++;
  setting_status->setting_raw = swap_endianness(pdu_data + pos, sensor_data_size);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_series_get_from_buf(sensor_get_series_msg_t *series_get,
                                               uint8_t *pdu_data,
                                               const size_t pdu_data_len)
{
  uint16_t pos = 0;

  series_get->property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;

  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_OK;
  }

  series_get->raw_value_length = sl_mesh_decoder_util_get_sensor_data_size(series_get->property_id);

  // Buffer length should be 2 times the raw value length (raw value x1 and x2) + property ID.
  // This is required to correctly interpret and process the series data according to the protocol.
  if (pdu_data_len != series_get->raw_value_length * 2 + SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  series_get->raw_value_x1 = swap_endianness(pdu_data + pos, series_get->raw_value_length);
  pos = pos + series_get->raw_value_length;
  series_get->raw_value_x2 = swap_endianness(pdu_data + pos, series_get->raw_value_length);

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_series_status_from_buf(sensor_series_status_msg_t *series_status,
                                                  uint8_t *pdu_data,
                                                  const size_t pdu_data_len)
{
  size_t idx = 0;
  uint16_t pos = 0;

  series_status[idx].property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;

  // only property id is present
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_OK;
  }

  series_status->raw_value_length = sl_mesh_decoder_util_get_sensor_data_size(series_status[idx].property_id);

  while (pos < pdu_data_len) {
    // remaining buffer should be at least three times of raw value length
    // (raw value x1, column width and raw value y)
    if (pdu_data_len - pos < 3 * series_status->raw_value_length) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    series_status[idx].raw_value_x = swap_endianness(pdu_data + pos, series_status->raw_value_length);
    pos = pos + series_status->raw_value_length;
    series_status[idx].column_width = swap_endianness(pdu_data + pos, series_status->raw_value_length);
    pos = pos + series_status->raw_value_length;
    series_status[idx].raw_value_y = swap_endianness(pdu_data + pos, series_status->raw_value_length);
    pos = pos + series_status->raw_value_length;
    idx++;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_column_status_from_buf(sensor_column_status_msg_t *column_status,
                                                  uint8_t *pdu_data,
                                                  const size_t pdu_data_len)
{
  uint16_t pos = 0;

  column_status->property_id = get_property_id(pdu_data);
  pos = pos + SL_MESH_PROPERTY_ID_LEN;
  // only property id is present, raw value is optional
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_OK;
  }

  column_status->raw_value_length = sl_mesh_decoder_util_get_sensor_data_size(column_status->property_id);

  // The buffer length must be at least the sum of the property ID length and three times the raw value length
  // (for raw value x, column width, and raw value y).
  if (pdu_data_len < (column_status->raw_value_length * 3) + SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  column_status->raw_value_x = swap_endianness(pdu_data + pos, column_status->raw_value_length);
  pos = pos + column_status->raw_value_length;
  column_status->column_width = swap_endianness(pdu_data + pos, column_status->raw_value_length);
  pos = pos + column_status->raw_value_length;
  column_status->raw_value_y = swap_endianness(pdu_data + pos, column_status->raw_value_length);
  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_status_from_buf(sensor_status_msg_t *sensor_status,
                                           uint8_t *pdu_data,
                                           const size_t pdu_data_len)
{
  uint8_t sensor_data_size = 0;
  size_t pos = 0;
  size_t idx = 0;

  while (pos < pdu_data_len) {
    sensor_status[idx].marshalled_property_id.format = pdu_data[0] & 0x01;

    if (sensor_status[idx].marshalled_property_id.format == 0) {
      sensor_status[idx].marshalled_property_id.length = (pdu_data[0] & 0x1E) >> 1;
      sensor_status[idx].marshalled_property_id.property_id = ((pdu_data[0] & 0xE0) | (pdu_data[1] << 8)) >> 5;
    } else {
      sensor_status[idx].marshalled_property_id.length = (pdu_data[0] & 0xFE) >> 1;
      sensor_status[idx].marshalled_property_id.property_id = get_property_id(pdu_data + 1);
    }

    sensor_data_size = sl_mesh_decoder_util_get_sensor_data_size(sensor_status->marshalled_property_id.property_id);

    sensor_status[idx].raw_value_length = sensor_data_size;
    if (sensor_status[idx].marshalled_property_id.format == 0) {
      sensor_status[idx].raw_value = pdu_data + 2;
      pos = 2 + sensor_data_size + pos;
    } else {
      sensor_status[idx].raw_value = pdu_data + 3;
      pos = 3 + sensor_data_size + pos;
    }
    idx++;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_settings_status_from_buf(sensor_settings_status_t *settings_status,
                                                    uint8_t *pdu_data,
                                                    const size_t pdu_data_len)
{
  // Ensure the buffer is long enough to contain at least one property ID
  // and that pdu_data_len is a multiple of SL_MESH_PROPERTY_ID_LEN for proper parsing.
  if (pdu_data_len < SL_MESH_PROPERTY_ID_LEN || pdu_data_len % SL_MESH_PROPERTY_ID_LEN != 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  settings_status->property_id = get_property_id(pdu_data);
  // setting property id field is optional
  if (pdu_data_len == SL_MESH_PROPERTY_ID_LEN) {
    return SL_STATUS_OK;
  }

  settings_status->setting_property = (sensor_setting_property_id_t *)swap_endianness(
    pdu_data + SL_MESH_PROPERTY_ID_LEN,
    pdu_data_len - SL_MESH_PROPERTY_ID_LEN
    );
  return SL_STATUS_OK;
}

sl_status_t sl_mesh_sensor_column_get_from_buf(sensor_get_column_msg_t *column_get,
                                               uint8_t *pdu_data,
                                               const size_t pdu_data_len)
{
  uint8_t sensor_data_size = sl_mesh_decoder_util_get_sensor_data_size(column_get->property_id);
  // check if the buffer is long enough to contain property id and column width
  if (pdu_data_len != SL_MESH_PROPERTY_ID_LEN + sensor_data_size) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  column_get->property_id = get_property_id(pdu_data);
  column_get->raw_value_length = sl_mesh_decoder_util_get_sensor_data_size(column_get->property_id);
  column_get->raw_value = swap_endianness(pdu_data + SL_MESH_PROPERTY_ID_LEN, column_get->property_id);

  return SL_STATUS_OK;
}
