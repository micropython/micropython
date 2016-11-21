/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef __MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H__
#define __MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H__

#include "py/obj.h"

#include "common-hal/microcontroller/types.h"
#include "shared-module/bitbangio/types.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t bitbangio_spi_type;

// Construct an underlying SPI object.
extern void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
    const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
    const mcu_pin_obj_t * miso, uint32_t baudrate);

extern void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self);

// Write out write_buffer then read read_buffer. Returns true on success, false
// otherwise.
extern bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self,
    const uint8_t *write_buffer, size_t write_buffer_len,
    uint8_t *read_buffer, size_t read_buffer_len);

#endif // __MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H__
