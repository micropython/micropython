/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANENTRY_H
#define MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANENTRY_H

#include "py/obj.h"
#include "py/objstr.h"
#include "shared-bindings/_bleio/Address.h"

typedef struct {
    mp_obj_base_t base;
    bool connectable;
    bool scan_response;
    int8_t rssi;
    bleio_address_obj_t *address;
    mp_obj_str_t *data;
    uint64_t time_received;
} bleio_scanentry_obj_t;

bool bleio_scanentry_data_matches(const uint8_t *data, size_t len, const uint8_t *prefixes, size_t prefix_length, bool any);

#endif // MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANENTRY_H
