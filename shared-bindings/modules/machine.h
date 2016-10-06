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

// Machine is the HAL for low-level, hardware accelerated functions. It is not
// meant to simplify APIs, its only meant to unify them so that other modules
// do not require port specific logic.
//
// This file includes externs for all functions a port should implement to
// support the machine module.

#ifndef __MICROPY_INCLUDED_API_MACHINE_H__
#define __MICROPY_INCLUDED_API_MACHINE_H__

#include "py/obj.h"

// Should include these structs which will be passed through to the port
// implementation:
//  * pin_obj_t
//  * machine_i2c_obj_t

// TODO(tannewt): Standardize the type names.

#include "machine_types.h"

#include "modmachine_pin.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t machine_i2c_type;

// Initializes the hardware peripheral.
extern void mp_hal_i2c_construct(machine_i2c_obj_t *self, const pin_obj_t * scl,
                                 const pin_obj_t * sda, uint32_t freq);

extern void mp_hal_i2c_init(machine_i2c_obj_t *self);
extern void mp_hal_i2c_deinit(machine_i2c_obj_t *self);

// Probe the bus to see if a device acknowledges the given address.
extern bool mp_hal_i2c_probe(machine_i2c_obj_t *self, uint8_t addr);

// Reads memory of the i2c device picking up where it left off.
extern void mp_hal_i2c_read(machine_i2c_obj_t *self, uint8_t addr,
                            uint8_t *data, size_t len);

// Reads memory of the i2c device starting at memaddr.
extern void mp_hal_i2c_read_mem(machine_i2c_obj_t *self, uint8_t addr,
                                uint16_t memaddr, uint8_t *dest, size_t len);

extern void mp_hal_i2c_write(machine_i2c_obj_t *self, uint8_t addr,
                             uint8_t *data, size_t len);

// Writes memory of the i2c device starting at memaddr.
extern void mp_hal_i2c_write_mem(machine_i2c_obj_t *self, uint8_t addr,
                                 uint16_t memaddr, const uint8_t *src,
                                 size_t len);


#endif // __MICROPY_INCLUDED_API_MACHINE_H__
