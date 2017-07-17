/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H

#include "py/obj.h"

// I2C protocol
// the first 4 methods can be NULL, meaning operation is not supported
typedef struct _mp_machine_i2c_p_t {
    int (*start)(mp_obj_base_t *obj);
    int (*stop)(mp_obj_base_t *obj);
    int (*read)(mp_obj_base_t *obj, uint8_t *dest, size_t len, bool nack);
    int (*write)(mp_obj_base_t *obj, const uint8_t *src, size_t len);
    int (*readfrom)(mp_obj_base_t *obj, uint16_t addr, uint8_t *dest, size_t len, bool stop);
    int (*writeto)(mp_obj_base_t *obj, uint16_t addr, const uint8_t *src, size_t len, bool stop);
} mp_machine_i2c_p_t;

typedef struct _mp_machine_soft_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t us_delay;
    uint32_t us_timeout;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} mp_machine_soft_i2c_obj_t;

extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_dict_t mp_machine_soft_i2c_locals_dict;

int mp_machine_soft_i2c_readfrom(mp_obj_base_t *self_in, uint16_t addr, uint8_t *dest, size_t len, bool stop);
int mp_machine_soft_i2c_writeto(mp_obj_base_t *self_in, uint16_t addr, const uint8_t *src, size_t len, bool stop);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H
