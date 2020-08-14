/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.] */

#include <openssl/x509.h>

#include <openssl/asn1.h>
#include <openssl/digest.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/obj.h>

#include "internal.h"


int x509_digest_sign_algorithm(EVP_MD_CTX *ctx, X509_ALGOR *algor) {
  EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(ctx->pctx);
  if (pkey == NULL) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_CONTEXT_NOT_INITIALISED);
    return 0;
  }

  if (EVP_PKEY_id(pkey) == EVP_PKEY_RSA) {
    int pad_mode;
    if (!EVP_PKEY_CTX_get_rsa_padding(ctx->pctx, &pad_mode)) {
      return 0;
    }
    /* RSA-PSS has special signature algorithm logic. */
    if (pad_mode == RSA_PKCS1_PSS_PADDING) {
      return x509_rsa_ctx_to_pss(ctx, algor);
    }
  }

  int pkey_id = EVP_PKEY_id(pkey);
  if (pkey_id == EVP_PKEY_ED25519 ||
///// OQS_TEMPLATE_FRAGMENT_CHECK_PKEY_ID_START
      pkey_id == EVP_PKEY_OQS_SIG_DEFAULT ||
      pkey_id == EVP_PKEY_DILITHIUM2 ||
      pkey_id == EVP_PKEY_DILITHIUM3 ||
      pkey_id == EVP_PKEY_DILITHIUM4 ||
      pkey_id == EVP_PKEY_FALCON512 ||
      pkey_id == EVP_PKEY_FALCON1024 ||
      pkey_id == EVP_PKEY_PICNICL1FS ||
      pkey_id == EVP_PKEY_PICNICL1UR ||
      pkey_id == EVP_PKEY_PICNICL1FULL ||
      pkey_id == EVP_PKEY_PICNIC3L1 ||
      pkey_id == EVP_PKEY_PICNIC3L3 ||
      pkey_id == EVP_PKEY_PICNIC3L5 ||
      pkey_id == EVP_PKEY_RAINBOWIACLASSIC ||
      pkey_id == EVP_PKEY_RAINBOWIACYCLIC ||
      pkey_id == EVP_PKEY_RAINBOWIACYCLICCOMPRESSED ||
      pkey_id == EVP_PKEY_RAINBOWIIICCLASSIC ||
      pkey_id == EVP_PKEY_RAINBOWIIICCYCLIC ||
      pkey_id == EVP_PKEY_RAINBOWIIICCYCLICCOMPRESSED ||
      pkey_id == EVP_PKEY_RAINBOWVCCLASSIC ||
      pkey_id == EVP_PKEY_RAINBOWVCCYCLIC ||
      pkey_id == EVP_PKEY_RAINBOWVCCYCLICCOMPRESSED ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA128FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA128FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA128SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA128SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA192FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA192FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA192SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA192SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA256FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA256FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA256SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSHARAKA256SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256128FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256128FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256128SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256128SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256192FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256192FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256192SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256192SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256256FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256256FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHA256256SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHA256256SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256128FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256128FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256128SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256128SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256192FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256192FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256192SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256192SSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256256FROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256256FSIMPLE ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256256SROBUST ||
      pkey_id == EVP_PKEY_SPHINCSSHAKE256256SSIMPLE
///// OQS_TEMPLATE_FRAGMENT_CHECK_PKEY_ID_END
      ) {
    // The NID == EVP_PKEY_id for ED25519 and the OQS schemes
    return X509_ALGOR_set0(algor, OBJ_nid2obj(pkey_id), V_ASN1_UNDEF, NULL);
  }

  /* Default behavior: look up the OID for the algorithm/hash pair and encode
   * that. */
  const EVP_MD *digest = EVP_MD_CTX_md(ctx);
  if (digest == NULL) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_CONTEXT_NOT_INITIALISED);
    return 0;
  }

  int sign_nid;
  if (!OBJ_find_sigid_by_algs(&sign_nid, EVP_MD_type(digest),
                              EVP_PKEY_id(pkey))) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_DIGEST_AND_KEY_TYPE_NOT_SUPPORTED);
    return 0;
  }

  /* RSA signature algorithms include an explicit NULL parameter. Others omit
   * it. */
  int paramtype =
      (EVP_PKEY_id(pkey) == EVP_PKEY_RSA) ? V_ASN1_NULL : V_ASN1_UNDEF;
  X509_ALGOR_set0(algor, OBJ_nid2obj(sign_nid), paramtype, NULL);
  return 1;
}

int x509_digest_verify_init(EVP_MD_CTX *ctx, X509_ALGOR *sigalg,
                            EVP_PKEY *pkey) {
  /* Convert the signature OID into digest and public key OIDs. */
  int sigalg_nid = OBJ_obj2nid(sigalg->algorithm);
  int digest_nid, pkey_nid;
  if (!OBJ_find_sigid_algs(sigalg_nid, &digest_nid, &pkey_nid)) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
    return 0;
  }

  /* Check the public key OID matches the public key type. */
  if (pkey_nid != EVP_PKEY_id(pkey)) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_WRONG_PUBLIC_KEY_TYPE);
    return 0;
  }

  /* NID_undef signals that there are custom parameters to set. */
  if (digest_nid == NID_undef) {
    if (sigalg_nid == NID_rsassaPss) {
      return x509_rsa_pss_to_ctx(ctx, sigalg, pkey);
    }
    if (sigalg_nid == NID_ED25519) {
      if (sigalg->parameter != NULL) {
        OPENSSL_PUT_ERROR(X509, X509_R_INVALID_PARAMETER);
        return 0;
      }
      return EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey);
    }
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM);
    return 0;
  }

  /* Otherwise, initialize with the digest from the OID. */
  const EVP_MD *digest = EVP_get_digestbynid(digest_nid);
  if (digest == NULL) {
    OPENSSL_PUT_ERROR(ASN1, ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM);
    return 0;
  }

  return EVP_DigestVerifyInit(ctx, NULL, digest, NULL, pkey);
}
