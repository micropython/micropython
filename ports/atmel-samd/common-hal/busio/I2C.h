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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_H

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_i2c_m_sync.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct i2c_m_sync_desc i2c_desc;
    bool has_lock;
    uint8_t scl_pin;
    uint8_t sda_pin;
} busio_i2c_obj_t;

extern Sercom *samd_i2c_get_sercom(const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *sercom_index, uint32_t *sda_pinmux, uint32_t *scl_pinmux);

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_H
