/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
<<<<<<<< HEAD:ports/atmel-samd/common-hal/i2ctarget/I2CTarget.h
 * Copyright (c) 2018 Noralf Trønnes
========
 * Copyright (c) 2013, 2014 Damien P. George
>>>>>>>> v1.20.0:ports/samd/pendsv.h
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
#ifndef MICROPY_INCLUDED_SAMD_PENDSV_H
#define MICROPY_INCLUDED_SAMD_PENDSV_H

<<<<<<<< HEAD:ports/atmel-samd/common-hal/i2ctarget/I2CTarget.h
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_TARGET_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_TARGET_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    uint8_t *addresses;
    unsigned int num_addresses;

    Sercom *sercom;
    uint8_t scl_pin;
    uint8_t sda_pin;
    bool writing;
} i2ctarget_i2c_target_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_I2C_TARGET_H
========
enum {
    PENDSV_DISPATCH_SOFT_TIMER,  // For later & for having at least one entry
    MICROPY_BOARD_PENDSV_ENTRIES
    PENDSV_DISPATCH_MAX
};

#define PENDSV_DISPATCH_NUM_SLOTS PENDSV_DISPATCH_MAX

typedef void (*pendsv_dispatch_t)(void);

void pendsv_init(void);
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f);

#endif // MICROPY_INCLUDED_SAMD_PENDSV_H
>>>>>>>> v1.20.0:ports/samd/pendsv.h
