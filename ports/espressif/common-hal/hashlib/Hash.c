/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/hashlib/Hash.h"

#include "components/mbedtls/mbedtls/include/mbedtls/ssl.h"

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
