/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_UUID_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_UUID_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // If non-zero, `uuid_vs_idx` is an index into the SoftDevice's table of registered vendor-specific UUID's.
    // If zero, `value` is a 16-bit Bluetooth SIG UUID, which would convert to this 128-bit UUID.
    // 0000xxxx-0000-1000-8000-00805F9B34FB
    uint8_t uuid_vs_idx;
    // The 16-bit part of the UUID. This replaces bytes 12 and 13 in the registered 128-bit UUID.
    uint16_t uuid16;
} bleio_uuid_obj_t;

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_UUID_H
