/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_NEOPIXEL_WRITE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_NEOPIXEL_WRITE_H

#include <stdint.h>
#include <stdbool.h>

#include "shared-bindings/busio/I2C.h"

extern void common_hal_is31fl3741_write(busio_i2c_obj_t *i2c, uint8_t addr, const mp_obj_t *mapping, const uint8_t *pixels, size_t numBytes);
void is31fl3741_begin_transaction(busio_i2c_obj_t *i2c);
void is31fl3741_end_transaction(busio_i2c_obj_t *i2c);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_NEOPIXEL_WRITE_H
