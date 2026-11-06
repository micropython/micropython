// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"

#include "mbedtls/psa_util.h"

#include "shared-bindings/cryptography/exceptions/__init__.h"
#include "shared-module/cryptography/hazmat/primitives/asymmetric/ec/EllipticCurvePublicKey.h"

#define P256_KEY_BITS 256
// SEC1 uncompressed point: 0x04 || X || Y, X and Y each 32 bytes for P-256.
#define P256_POINT_LEN 65

void common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_construct(
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self,
    mp_obj_t curve, const uint8_t *point, size_t point_len) {
    if (point_len != P256_POINT_LEN || point[0] != 0x04) {
        mp_raise_ValueError(MP_ERROR_TEXT(
            "data must be a 65-byte uncompressed SEC1 point (0x04 prefix)"));
    }

    // PSA is already initialized by ssl / hashlib / hardwarekey, but
    // psa_crypto_init() is idempotent and keeps this module working on a
    // build with none of those (see common-hal/hardwarekey/HardwareKey.c's
    // identical call for the same reason).
    if (psa_crypto_init() != PSA_SUCCESS) {
        mp_raise_RuntimeError(NULL);
    }

    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attr, P256_KEY_BITS);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    psa_key_id_t key_id = 0;
    psa_status_t status = psa_import_key(&attr, point, point_len, &key_id);
    if (status != PSA_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("data is not a valid point on this curve"));
    }

    self->curve = curve;
    memcpy(self->point, point, P256_POINT_LEN);
    self->key_id = key_id;
}

void common_hal_cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_verify(
    cryptography_hazmat_primitives_asymmetric_ec_ellipticcurvepublickey_obj_t *self,
    const uint8_t *signature, size_t signature_len, const uint8_t *digest, size_t digest_len) {
    if (digest_len != 32) {
        mp_raise_ValueError(MP_ERROR_TEXT("digest must be 32 bytes (SHA-256)"));
    }

    // PSA's own ECDSA signature format is raw r||s, not the ASN.1 DER real
    // PyCA's verify() takes -- mbedtls_ecdsa_der_to_raw() is the same,
    // already-in-this-tree conversion esp_crt_bundle.c uses for ECDSA
    // cert-chain verification via PSA.
    uint8_t raw_sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t raw_sig_len = 0;
    int ret = mbedtls_ecdsa_der_to_raw(P256_KEY_BITS, signature, signature_len,
        raw_sig, sizeof(raw_sig), &raw_sig_len);
    if (ret != 0) {
        mp_raise_cryptography_InvalidSignature(MP_ERROR_TEXT("Signature is malformed"));
    }

    psa_status_t status = psa_verify_hash(self->key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
        digest, digest_len, raw_sig, raw_sig_len);
    if (status != PSA_SUCCESS) {
        mp_raise_cryptography_InvalidSignature(MP_ERROR_TEXT("Signature does not verify"));
    }
}
