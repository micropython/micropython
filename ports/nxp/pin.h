/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#ifndef _PIN_H_
#define _PIN_H_

// This file requires pin_defs_xxx.h (which has port specific enums and
// defines, so we include it here. It should never be included directly
#if defined(MICROPY_PY_LED) && MICROPY_PY_LED
#include "py/obj.h"

#include "fsl_common.h"


typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint32_t pin;
    GPIO_Type *gpio;
}pin_obj_t; 

extern const mp_obj_type_t pin_type;

#define PIN(gpio_num, pin_num,gpio_base) \
{\
    { &pin_type }, \
    .name = MP_QSTR_GPIO ## gpio_num ## _IO ## pin_num, \
    .pin = pin_num, \
    .gpio = gpio_base, \
}


void pin_init0(void);
uint32_t pin_get_direction(const pin_obj_t *pin);
#endif /* MICROPY_PY_LED */
#endif