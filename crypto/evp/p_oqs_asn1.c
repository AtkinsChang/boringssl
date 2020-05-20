/* Copyright (c) 2017, Google Inc., modifications by the Open Quantum Safe project 2020.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#include <openssl/evp.h>

#include <openssl/bytestring.h>
#include <openssl/err.h>
#include <openssl/mem.h>

#include <oqs/oqs.h>

#include "internal.h"
#include "../internal.h"


static void oqs_free(EVP_PKEY *pkey) {
  OPENSSL_free(pkey->pkey.ptr);
  pkey->pkey.ptr = NULL;
}

#define DEFINE_OQS_SET_PRIV_RAW(ALG, OQS_METH)                                 \
static int ALG##_set_priv_raw(EVP_PKEY *pkey, const uint8_t *in, size_t len) { \
  OQS_KEY *key = OPENSSL_malloc(sizeof(OQS_KEY));                              \
  if (key == NULL) {                                                           \
    OPENSSL_PUT_ERROR(EVP, ERR_R_MALLOC_FAILURE);                              \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  key->ctx = OQS_SIG_new(OQS_METH);                                            \
  if (!key->ctx) {                                                             \
    OPENSSL_PUT_ERROR(EVP, EVP_R_UNSUPPORTED_ALGORITHM);                       \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  if (len != key->ctx->length_secret_key) {                                    \
    OPENSSL_PUT_ERROR(EVP, EVP_R_DECODE_ERROR);                                \
    return 0;                                                                  \
  }                                                                            \
                                                                               \
  key->priv = malloc(key->ctx->length_secret_key);                             \
  key->pub = malloc(key->ctx->length_public_key);                              \
  if (OQS_SIG_keypair(key->ctx, key->pub, key->priv) != OQS_SUCCESS) {         \
    OPENSSL_PUT_ERROR(EVP, EVP_R_KEYS_NOT_SET);                                \
    return 0;                                                                  \
  }                                                                            \
  key->has_private = 1;                                                        \
                                                                               \
  oqs_free(pkey);                                                              \
  pkey->pkey.ptr = key;                                                        \
  return 1;                                                                    \
}

#define DEFINE_OQS_SET_PUB_RAW(ALG, OQS_METH)                                 \
static int ALG##_set_pub_raw(EVP_PKEY *pkey, const uint8_t *in, size_t len) { \
  OQS_KEY *key = OPENSSL_malloc(sizeof(OQS_KEY));                             \
  if (key == NULL) {                                                          \
    OPENSSL_PUT_ERROR(EVP, ERR_R_MALLOC_FAILURE);                             \
    return 0;                                                                 \
  }                                                                           \
                                                                              \
  key->ctx = OQS_SIG_new(OQS_METH);                                           \
  if (!key->ctx) {                                                            \
    OPENSSL_PUT_ERROR(EVP, EVP_R_UNSUPPORTED_ALGORITHM);                      \
    return 0;                                                                 \
  }                                                                           \
                                                                              \
  if (len != key->ctx->length_public_key) {                                   \
    OPENSSL_PUT_ERROR(EVP, EVP_R_DECODE_ERROR);                               \
    return 0;                                                                 \
  }                                                                           \
                                                                              \
  key->pub = malloc(key->ctx->length_public_key);                             \
  OPENSSL_memcpy(key->pub, in, key->ctx->length_public_key);                  \
  key->has_private = 0;                                                       \
                                                                              \
  oqs_free(pkey);                                                             \
  pkey->pkey.ptr = key;                                                       \
  return 1;                                                                   \
}

static int oqs_get_priv_raw(const EVP_PKEY *pkey, uint8_t *out,
                                size_t *out_len) {
  const OQS_KEY *key = pkey->pkey.ptr;
  if (!key->has_private) {
    OPENSSL_PUT_ERROR(EVP, EVP_R_NOT_A_PRIVATE_KEY);
    return 0;
  }

  if (out == NULL) {
    *out_len = key->ctx->length_secret_key;
    return 1;
  }

  if (*out_len < key->ctx->length_secret_key) {
    OPENSSL_PUT_ERROR(EVP, EVP_R_BUFFER_TOO_SMALL);
    return 0;
  }

  OPENSSL_memcpy(out, key->priv, key->ctx->length_secret_key);
  *out_len = key->ctx->length_secret_key;
  return 1;
}

static int oqs_get_pub_raw(const EVP_PKEY *pkey, uint8_t *out,
                               size_t *out_len) {
  const OQS_KEY *key = pkey->pkey.ptr;
  if (out == NULL) {
    *out_len = key->ctx->length_public_key;
    return 1;
  }

  if (*out_len < key->ctx->length_public_key) {
    OPENSSL_PUT_ERROR(EVP, EVP_R_BUFFER_TOO_SMALL);
    return 0;
  }

  OPENSSL_memcpy(out, key->pub, key->ctx->length_public_key);
  *out_len = key->ctx->length_public_key;
  return 1;
}

#define DEFINE_OQS_PUB_DECODE(ALG)                                  \
static int ALG##_pub_decode(EVP_PKEY *out, CBS *params, CBS *key) { \
  if (CBS_len(params) != 0) {                                       \
    OPENSSL_PUT_ERROR(EVP, EVP_R_DECODE_ERROR);                     \
    return 0;                                                       \
  }                                                                 \
                                                                    \
  return ALG##_set_pub_raw(out, CBS_data(key), CBS_len(key));       \
}

#define DEFINE_OQS_PUB_ENCODE(ALG)                                           \
static int ALG##_pub_encode(CBB *out, const EVP_PKEY *pkey) {                \
  const OQS_KEY *key = pkey->pkey.ptr;                                       \
                                                                             \
  /* See RFC 8410, section 4. */                                             \
  CBB spki, algorithm, oid, key_bitstring;                                   \
  if (!CBB_add_asn1(out, &spki, CBS_ASN1_SEQUENCE) ||                        \
    !CBB_add_asn1(&spki, &algorithm, CBS_ASN1_SEQUENCE) ||                   \
    !CBB_add_asn1(&algorithm, &oid, CBS_ASN1_OBJECT) ||                      \
    !CBB_add_bytes(&oid, ALG##_asn1_meth.oid, ALG##_asn1_meth.oid_len) ||    \
    !CBB_add_asn1(&spki, &key_bitstring, CBS_ASN1_BITSTRING) ||              \
    !CBB_add_u8(&key_bitstring, 0 /* padding */) ||                          \
    !CBB_add_bytes(&key_bitstring, key->pub, key->ctx->length_public_key) || \
    !CBB_flush(out)) {                                                       \
       OPENSSL_PUT_ERROR(EVP, EVP_R_ENCODE_ERROR);                           \
       return 0;                                                             \
    }                                                                        \
                                                                             \
  return 1;                                                                  \
}

static int oqs_pub_cmp(const EVP_PKEY *a, const EVP_PKEY *b) {
  const OQS_KEY *a_key = a->pkey.ptr;
  const OQS_KEY *b_key = b->pkey.ptr;
  return OPENSSL_memcmp(a_key->pub, b_key->pub, a_key->ctx->length_public_key) == 0;
}

#define DEFINE_OQS_PRIV_DECODE(ALG)                                  \
static int ALG##_priv_decode(EVP_PKEY *out, CBS *params, CBS *key) { \
/* See RFC 8410, section 7. */                                       \
                                                                     \
  CBS inner;                                                         \
  if (CBS_len(params) != 0 ||                                        \
      !CBS_get_asn1(key, &inner, CBS_ASN1_OCTETSTRING) ||            \
      CBS_len(key) != 0) {                                           \
    OPENSSL_PUT_ERROR(EVP, EVP_R_DECODE_ERROR);                      \
    return 0;                                                        \
  }                                                                  \
                                                                     \
  return ALG##_set_priv_raw(out, CBS_data(&inner), CBS_len(&inner)); \
}

#define DEFINE_OQS_PRIV_ENCODE(ALG)                                         \
static int ALG##_priv_encode(CBB *out, const EVP_PKEY *pkey) {              \
  OQS_KEY *key = pkey->pkey.ptr;                                            \
  if (!key->has_private) {                                                  \
    OPENSSL_PUT_ERROR(EVP, EVP_R_NOT_A_PRIVATE_KEY);                        \
    return 0;                                                               \
  }                                                                         \
                                                                            \
  /* See RFC 8410, section 7. */                                            \
  CBB pkcs8, algorithm, oid, private_key, inner;                            \
  if (!CBB_add_asn1(out, &pkcs8, CBS_ASN1_SEQUENCE) ||                      \
      !CBB_add_asn1_uint64(&pkcs8, 0 /* version */) ||                      \
      !CBB_add_asn1(&pkcs8, &algorithm, CBS_ASN1_SEQUENCE) ||               \
      !CBB_add_asn1(&algorithm, &oid, CBS_ASN1_OBJECT) ||                   \
      !CBB_add_bytes(&oid, ALG##_asn1_meth.oid, ALG##_asn1_meth.oid_len) || \
      !CBB_add_asn1(&pkcs8, &private_key, CBS_ASN1_OCTETSTRING) ||          \
      !CBB_add_asn1(&private_key, &inner, CBS_ASN1_OCTETSTRING) ||          \
      !CBB_add_bytes(&inner, key->priv, key->ctx->length_secret_key) ||     \
      !CBB_flush(out)) {                                                    \
    OPENSSL_PUT_ERROR(EVP, EVP_R_ENCODE_ERROR);                             \
    return 0;                                                               \
  }                                                                         \
                                                                            \
  return 1;                                                                 \
}

static size_t oqs_size(const EVP_PKEY *pkey) {
  const OQS_KEY *key = pkey->pkey.ptr;
  return key->ctx->length_signature;
}

// Dummy wrapper to improve readability
#define OID(...) __VA_ARGS__

#define OID_LEN(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

#define DEFINE_OQS_ASN1_METHODS(ALG, OQS_METH, ALG_PKEY) \
DEFINE_OQS_SET_PUB_RAW(ALG, OQS_METH)                      \
DEFINE_OQS_SET_PRIV_RAW(ALG, OQS_METH)                     \
DEFINE_OQS_PUB_ENCODE(ALG)                                 \
DEFINE_OQS_PUB_DECODE(ALG)                                 \
DEFINE_OQS_PRIV_ENCODE(ALG)                                \
DEFINE_OQS_PRIV_DECODE(ALG)

#define DEFINE_OQS_PKEY_ASN1_METHOD(ALG, ALG_PKEY, ...) \
const EVP_PKEY_ASN1_METHOD ALG##_asn1_meth = {          \
    ALG_PKEY,                                           \
    {__VA_ARGS__},                                      \
    OID_LEN(__VA_ARGS__),                               \
    ALG##_pub_decode,                                   \
    ALG##_pub_encode,                                   \
    oqs_pub_cmp,                                        \
    ALG##_priv_decode,                                  \
    ALG##_priv_encode,                                  \
    ALG##_set_priv_raw,                                 \
    ALG##_set_pub_raw,                                  \
    oqs_get_priv_raw,                                   \
    oqs_get_pub_raw,                                    \
    NULL /* pkey_opaque */,                             \
    oqs_size,                                           \
    NULL /* pkey_bits */,                               \
    NULL /* param_missing */,                           \
    NULL /* param_copy */,                              \
    NULL /* param_cmp */,                               \
    oqs_free,                                           \
};

// the OIDs can also be found in the kObjectData array in crypto/obj/obj_dat.h
///// OQS_TEMPLATE_FRAGMENT_DEF_ASN1_METHODS_START
DEFINE_OQS_ASN1_METHODS(oqs_sig_default, OQS_SIG_alg_default, EVP_PKEY_OQS_SIG_DEFAULT)
DEFINE_OQS_PKEY_ASN1_METHOD(oqs_sig_default, EVP_PKEY_OQS_SIG_DEFAULT, OID(0x2B, 0xCE, 0x0F, 0x01, 0x01))

DEFINE_OQS_ASN1_METHODS(dilithium2, OQS_SIG_alg_dilithium_2, EVP_PKEY_DILITHIUM2)
DEFINE_OQS_PKEY_ASN1_METHOD(dilithium2, EVP_PKEY_DILITHIUM2, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x02, 0x82, 0x0B, 0x06, 0x04, 0x03))

DEFINE_OQS_ASN1_METHODS(dilithium3, OQS_SIG_alg_dilithium_3, EVP_PKEY_DILITHIUM3)
DEFINE_OQS_PKEY_ASN1_METHOD(dilithium3, EVP_PKEY_DILITHIUM3, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x02, 0x82, 0x0B, 0x06, 0x05, 0x04))

DEFINE_OQS_ASN1_METHODS(dilithium4, OQS_SIG_alg_dilithium_4, EVP_PKEY_DILITHIUM4)
DEFINE_OQS_PKEY_ASN1_METHOD(dilithium4, EVP_PKEY_DILITHIUM4, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x02, 0x82, 0x0B, 0x06, 0x06, 0x05))

DEFINE_OQS_ASN1_METHODS(picnicl1fs, OQS_SIG_alg_picnic_L1_FS, EVP_PKEY_PICNICL1FS)
DEFINE_OQS_PKEY_ASN1_METHOD(picnicl1fs, EVP_PKEY_PICNICL1FS, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x59, 0x02, 0x01, 0x01))

DEFINE_OQS_ASN1_METHODS(picnic2l1fs, OQS_SIG_alg_picnic2_L1_FS, EVP_PKEY_PICNIC2L1FS)
DEFINE_OQS_PKEY_ASN1_METHOD(picnic2l1fs, EVP_PKEY_PICNIC2L1FS, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x59, 0x02, 0x01, 0x0B))

DEFINE_OQS_ASN1_METHODS(qteslapi, OQS_SIG_alg_qTesla_p_I, EVP_PKEY_QTESLAPI)
DEFINE_OQS_PKEY_ASN1_METHOD(qteslapi, EVP_PKEY_QTESLAPI, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x59, 0x02, 0x02, 0x0A))

DEFINE_OQS_ASN1_METHODS(qteslapiii, OQS_SIG_alg_qTesla_p_III, EVP_PKEY_QTESLAPIII)
DEFINE_OQS_PKEY_ASN1_METHOD(qteslapiii, EVP_PKEY_QTESLAPIII, OID(0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x59, 0x02, 0x02, 0x14))

DEFINE_OQS_ASN1_METHODS(sphincs_haraka_128f_robust, OQS_SIG_alg_sphincs_haraka_128f_robust, EVP_PKEY_SPHINCS_HARAKA_128F_ROBUST)
DEFINE_OQS_PKEY_ASN1_METHOD(sphincs_haraka_128f_robust, EVP_PKEY_SPHINCS_HARAKA_128F_ROBUST, OID(0x2B, 0xCE, 0x0F, 0x03, 0x01))

///// OQS_TEMPLATE_FRAGMENT_DEF_ASN1_METHODS_END
