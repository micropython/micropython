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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB_HASH_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB_HASH_H

#include "py/obj.h"

#include "common-hal/hashlib/Hash.h"

extern const mp_obj_type_t hashlib_hash_type;

// So that new can call it when given data.
mp_obj_t hashlib_hash_update(mp_obj_t self_in, mp_obj_t buf_in);

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen);
void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen);
size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB_HASH_H
