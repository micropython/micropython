// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/Hash.h"
#include "shared-module/hashlib/__init__.h"

#include "mbedtls/ssl.h"

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen) {
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        mbedtls_sha1_update_ret(&self->sha1, data, datalen);
        return;
    }
}

void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen) {
    if (datalen < common_hal_hashlib_hash_get_digest_size(self)) {
        return;
    }
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        // We copy the sha1 state so we can continue to update if needed or get
        // the digest a second time.
        mbedtls_sha1_context copy;
        mbedtls_sha1_clone(&copy, &self->sha1);
        mbedtls_sha1_finish_ret(&self->sha1, data);
        mbedtls_sha1_clone(&self->sha1, &copy);
    }
}

size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self) {
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        return 20;
    }
    return 0;
}
