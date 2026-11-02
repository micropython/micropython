// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

// The one port-specific step: turn an eFuse key block into a PSA key id.
// Everything after that -- hmac.new()'s use of the key id, sign() -- lives in
// shared-module/hardwarekey/HardwareKey.c. Digital Signature key handling
// lives in
// ports/espressif/common-hal/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.c.

#include "py/runtime.h"

#include "common-hal/hardwarekey/__init__.h"
#include "common-hal/hardwarekey/board.h"

#include "shared-module/hardwarekey/HardwareKey.h"

#include "esp_efuse.h"

// board.h hardcodes the slot count (enum values can't be used in #if); make sure
// it still matches this chip's eFuse layout.
_Static_assert(HARDWAREKEY_EFUSE_SLOT_COUNT == EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0,
    "eFuse key block count changed; update common-hal/hardwarekey/board.h");

// Pulls in MBEDTLS_CONFIG_FILE (esp_config.h), which is what defines
// ESP_HMAC_OPAQUE_DRIVER_ENABLED / ESP_RSA_DS_DRIVER_ENABLED on chips that
// have those peripherals. Including only <psa/crypto.h> goes through the
// tf-psa-crypto config path and does NOT define either, so the opaque-driver
// headers below would compile to nothing.
#include "mbedtls/build_info.h"
#include "psa/crypto.h"
// Public header of the ESP-IDF mbedtls component's PSA opaque-key driver for
// eFuse HMAC keys (components/mbedtls/port/psa_driver/include/).
#include "psa_crypto_driver_esp_hmac_opaque.h"

#if !defined(ESP_HMAC_OPAQUE_DRIVER_ENABLED)
#error "hardwarekey requires the ESP-IDF PSA opaque HMAC driver (SOC_HMAC_SUPPORTED targets only)"
#endif

// The ESP HMAC peripheral consumes a 256-bit eFuse key.
#define HMAC_KEY_BITS 256

// One PSA key is imported per eFuse block. The imports are volatile references
// (no key material) and survive a CircuitPython soft reset -- ESP-IDF initializes
// PSA once at boot and never frees it -- so this only runs once per block.
static psa_key_id_t import_efuse_hmac_key(mp_int_t slot) {
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attr, HMAC_KEY_BITS);
    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

    // Import data is a *reference* to the eFuse block, not key material. The
    // driver independently re-checks the HMAC_UP purpose and refuses anything else.
    esp_hmac_opaque_key_t keyref = { .efuse_key_id = (uint8_t)slot };

    psa_key_id_t key_id = 0;
    if (psa_import_key(&attr, (const uint8_t *)&keyref, sizeof(keyref), &key_id) != PSA_SUCCESS) {
        return 0;
    }
    return key_id;
}

bool hardwarekey_efuse_slot_load(mp_int_t slot, hardwarekey_hardwarekey_obj_t *key) {
    key->key_slot = slot;
    key->key_id = 0;
    key->purpose = HARDWAREKEY_PURPOSE_UNUSED;
    key->exportable = false;

    esp_efuse_block_t block = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + slot);
    esp_efuse_purpose_t block_purpose = esp_efuse_get_key_purpose(block);

    if (block_purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE) {
        // No PSA import yet: unlike HMAC_UP, this purpose alone doesn't name a
        // full key -- the caller still has to supply ds_params via
        // cryptography.hazmat.primitives.asymmetric.rsa.load_digital_signature_key().
        // Just record that the slot is provisioned for it.
        #if defined(ESP_RSA_DS_DRIVER_ENABLED)
        key->purpose = HARDWAREKEY_PURPOSE_DS;
        key->exportable = !esp_efuse_get_key_dis_read(block);
        return true;
        #else
        return false;
        #endif
    }

    if (block_purpose != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        return false;
    }

    // PSA is already initialized by ssl / hashlib, but psa_crypto_init() is
    // idempotent and keeps hardwarekey working on a build with neither.
    if (psa_crypto_init() != PSA_SUCCESS) {
        return false;
    }

    psa_key_id_t key_id = import_efuse_hmac_key(slot);
    if (key_id == 0) {
        return false;
    }

    key->key_id = key_id;
    key->purpose = HARDWAREKEY_PURPOSE_HMAC;
    key->exportable = !esp_efuse_get_key_dis_read(block);
    return true;
}
