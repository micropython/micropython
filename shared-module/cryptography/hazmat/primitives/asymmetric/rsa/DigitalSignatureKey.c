// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-module/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.h"

mp_int_t common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_get_key_size(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self) {
    return self->key_size;
}

psa_key_id_t common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_get_key_id(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self) {
    return self->key_id;
}

// Portable: once ensure_algorithm() (port-specific) has produced a PSA key id
// for the DS peripheral, signing is the same PSA call as any other opaque
// RSA key. The DS peripheral hashes+pads internally when driven this way, so
// callers never see a raw RSA exponentiation primitive.
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_sign(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    const uint8_t *data, size_t data_len, uint8_t *sig_out, size_t sig_out_len) {
    common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_ensure_algorithm(self,
        PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
        PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_SIGN_HASH);

    size_t sig_len = 0;
    psa_status_t status = psa_sign_message(self->key_id,
        PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
        data, data_len, sig_out, sig_out_len, &sig_len);
    if (status != PSA_SUCCESS) {
        mp_raise_RuntimeError(NULL);
    }
}

// Portable: once ensure_algorithm() (port-specific) has produced a PSA key id
// committed to `alg`, decryption is the same PSA call as any other opaque RSA
// key. The DS peripheral removes the padding internally, so *output_len is the
// recovered plaintext length, not key_size / 8.
void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_decrypt(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    psa_algorithm_t alg, const uint8_t *ciphertext, size_t ciphertext_len,
    uint8_t *plaintext_out, size_t plaintext_out_size, size_t *output_len) {
    common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_ensure_algorithm(self,
        alg, PSA_KEY_USAGE_DECRYPT);

    psa_status_t status = psa_asymmetric_decrypt(self->key_id, alg,
        ciphertext, ciphertext_len, NULL, 0, plaintext_out, plaintext_out_size, output_len);
    if (status == PSA_ERROR_NOT_SUPPORTED && PSA_ALG_IS_RSA_OAEP(alg)) {
        mp_raise_NotImplementedError(
            MP_ERROR_TEXT("OAEP requires this build to enable TLS 1.3 support"));
    }
    if (status != PSA_SUCCESS) {
        mp_raise_RuntimeError(NULL);
    }
}
