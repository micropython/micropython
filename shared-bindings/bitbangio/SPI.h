/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/SPI.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t bitbangio_spi_type;

// Construct an underlying SPI object.
extern void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso);

extern void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self);
extern bool shared_module_bitbangio_spi_deinited(bitbangio_spi_obj_t *self);

extern void shared_module_bitbangio_spi_configure(bitbangio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits);

extern bool shared_module_bitbangio_spi_try_lock(bitbangio_spi_obj_t *self);
extern bool shared_module_bitbangio_spi_has_lock(bitbangio_spi_obj_t *self);
extern void shared_module_bitbangio_spi_unlock(bitbangio_spi_obj_t *self);

// Writes out the given data.
extern bool shared_module_bitbangio_spi_write(bitbangio_spi_obj_t *self, const uint8_t *data, size_t len);

// Reads in len bytes while outputting zeroes.
extern bool shared_module_bitbangio_spi_read(bitbangio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value);

// Transfer out len bytes while reading len bytes
extern bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self, const uint8_t *dout, uint8_t *din, size_t len);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_SPI_H
