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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_I2C_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_I2C_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/bitbangio/I2C.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t bitbangio_i2c_type;

// Initializes the hardware peripheral.
extern void shared_module_bitbangio_i2c_construct(bitbangio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency,
    uint32_t us_timeout);

extern void shared_module_bitbangio_i2c_deinit(bitbangio_i2c_obj_t *self);
extern bool shared_module_bitbangio_i2c_deinited(bitbangio_i2c_obj_t *self);

extern bool shared_module_bitbangio_i2c_try_lock(bitbangio_i2c_obj_t *self);
extern bool shared_module_bitbangio_i2c_has_lock(bitbangio_i2c_obj_t *self);
extern void shared_module_bitbangio_i2c_unlock(bitbangio_i2c_obj_t *self);

// Probe the bus to see if a device acknowledges the given address.
extern bool shared_module_bitbangio_i2c_probe(bitbangio_i2c_obj_t *self, uint8_t addr);

extern uint8_t shared_module_bitbangio_i2c_write(bitbangio_i2c_obj_t *self,
    uint16_t address,
    const uint8_t *data, size_t len,
    bool stop);

// Reads memory of the i2c device picking up where it left off.
extern uint8_t shared_module_bitbangio_i2c_read(bitbangio_i2c_obj_t *self,
    uint16_t address,
    uint8_t *data, size_t len);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BITBANGIO_I2C_H
