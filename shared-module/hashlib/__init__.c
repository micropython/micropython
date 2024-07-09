// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/__init__.h"
#include "shared-module/hashlib/__init__.h"

#include "mbedtls/ssl.h"


bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm) {
    if (strcmp(algorithm, "sha1") == 0) {
        self->hash_type = MBEDTLS_SSL_HASH_SHA1;
        mbedtls_sha1_init(&self->sha1);
        mbedtls_sha1_starts_ret(&self->sha1);
        return true;
    }
    return false;
}
