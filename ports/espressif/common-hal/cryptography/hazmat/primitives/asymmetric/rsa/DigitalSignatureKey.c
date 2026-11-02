// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"

#include "common-hal/hardwarekey/board.h"

#include "shared-module/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.h"
#include "shared-module/hardwarekey/HardwareKey.h"

#include "esp_heap_caps.h"

// Pulls in MBEDTLS_CONFIG_FILE (esp_config.h), which is what defines
// ESP_RSA_DS_DRIVER_ENABLED on chips that have the Digital Signature
// peripheral. Including only <psa/crypto.h> goes through the tf-psa-crypto
// config path and does NOT define it, so the opaque-driver headers below
// would compile to nothing.
#include "mbedtls/build_info.h"
#include "psa/crypto.h"

// The Digital Signature peripheral driver is genuinely optional:
// CIRCUITPY_HARDWAREKEY is on for every HMAC-capable chip, but not every one
// of those also has SOC_DIG_SIGN_SUPPORTED. Where it's absent,
// ESP_RSA_DS_DRIVER_ENABLED is undefined and DS purpose is simply never
// reported by hardwarekey_efuse_slot_load() -- no build-time #error.
#if defined(ESP_RSA_DS_DRIVER_ENABLED)
#include "esp_ds.h"
#include "psa_crypto_driver_esp_rsa_ds.h"
#include "psa_crypto_driver_esp_rsa_ds_contexts.h"

// Persistent (non-GC) storage for a DS slot's imported key. Allocated lazily
// on first load and reused (overwritten in place) on a later call for the
// same slot -- the PSA RSA-DS driver only supports volatile keys (IDF-15427),
// so there is no psa_destroy_key() to pair a replacement with; the old PSA
// key id is simply abandoned along with its one HMAC-key eFuse block's worth
// of state. `generation` is bumped on every (re)load; see the field comment
// in shared-module/.../DigitalSignatureKey.h.
typedef struct {
    esp_ds_data_t *data;
    esp_ds_data_ctx_t *ctx;
    esp_rsa_ds_opaque_key_t *opaque_key;
    uint32_t generation;
} ds_slot_cache_t;
static ds_slot_cache_t ds_slot_cache[HARDWAREKEY_EFUSE_SLOT_COUNT];

void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_construct(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    hardwarekey_hardwarekey_obj_t *source_key, const uint8_t *ds_params, size_t ds_params_len) {
    if (ds_params_len != sizeof(esp_ds_data_t)) {
        mp_raise_ValueError(MP_ERROR_TEXT("ds_params has the wrong length"));
    }

    mp_int_t slot = common_hal_hardwarekey_hardwarekey_get_key_slot(source_key);
    ds_slot_cache_t *cache = &ds_slot_cache[slot];
    if (cache->data == NULL) {
        cache->data = heap_caps_malloc(sizeof(esp_ds_data_t), MALLOC_CAP_8BIT);
        cache->ctx = heap_caps_malloc(sizeof(esp_ds_data_ctx_t), MALLOC_CAP_8BIT);
        cache->opaque_key = heap_caps_malloc(sizeof(esp_rsa_ds_opaque_key_t), MALLOC_CAP_8BIT);
        if (cache->data == NULL || cache->ctx == NULL || cache->opaque_key == NULL) {
            m_malloc_fail(sizeof(esp_ds_data_t));
        }
    }
    memcpy(cache->data, ds_params, sizeof(esp_ds_data_t));

    // rsa_length is stored as (bits / 32) - 1 (see esp_digital_signature_length_t).
    mp_int_t rsa_bits = ((mp_int_t)cache->data->rsa_length + 1) * 32;

    *cache->ctx = (esp_ds_data_ctx_t) {
        .esp_ds_data = cache->data,
        .efuse_key_id = (uint8_t)slot,
        .rsa_length_bits = (uint16_t)rsa_bits,
    };
    *cache->opaque_key = (esp_rsa_ds_opaque_key_t) {
        .ds_data_ctx = cache->ctx,
    };

    // A new load for this slot invalidates any not-yet-committed
    // DigitalSignatureKey previously obtained for it -- see ensure_algorithm().
    cache->generation++;
    self->generation = cache->generation;

    // The actual PSA import is deferred to ensure_algorithm(), on the first
    // sign()/decrypt() call -- see its declaration in shared-module for why.
    self->key_id = 0;
    self->committed_alg = PSA_ALG_NONE;
    self->key_size = rsa_bits;
}

void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_ensure_algorithm(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    psa_algorithm_t alg, psa_key_usage_t usage) {
    if (self->key_id != 0) {
        if (self->committed_alg != alg) {
            mp_raise_ValueError(MP_ERROR_TEXT(
                "This key already committed to a different algorithm; call load_digital_signature_key() again to use a different one"));
        }
        return;
    }

    hardwarekey_hardwarekey_obj_t *source_key = MP_OBJ_TO_PTR(self->source_key);
    mp_int_t slot = common_hal_hardwarekey_hardwarekey_get_key_slot(source_key);
    ds_slot_cache_t *cache = &ds_slot_cache[slot];
    if (self->generation != cache->generation) {
        mp_raise_ValueError(MP_ERROR_TEXT(
            "This key's ds_params were replaced by a later load_digital_signature_key() call for the same key slot"));
    }

    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attr, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attr, self->key_size);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);

    psa_key_id_t key_id = 0;
    psa_status_t status = psa_import_key(&attr,
        (const uint8_t *)cache->opaque_key, sizeof(*cache->opaque_key), &key_id);
    if (status != PSA_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("ds_params is invalid for this key slot"));
    }

    self->key_id = key_id;
    self->committed_alg = alg;
}

#else

void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_construct(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    hardwarekey_hardwarekey_obj_t *source_key, const uint8_t *ds_params, size_t ds_params_len) {
    // Unreachable in practice: a Digital Signature purpose is never reported
    // by hardwarekey_efuse_slot_load() on a chip without the driver, and
    // shared-bindings checks `purpose` before calling this. Kept as a body
    // (not a build error) so this file still compiles on those chips.
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Digital Signature peripheral not available on this chip"));
}

void common_hal_cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_ensure_algorithm(
    cryptography_hazmat_primitives_asymmetric_rsa_digitalsignaturekey_obj_t *self,
    psa_algorithm_t alg, psa_key_usage_t usage) {
    // Also unreachable: construct() above always raises first.
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Digital Signature peripheral not available on this chip"));
}

#endif
