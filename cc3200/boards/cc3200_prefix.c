/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

// cc3200_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>
#include <stdint.h>

#include "py/mpconfig.h"
#include "py/obj.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "pin.h"
#include "gpio.h"
#include "pybpin.h"


#define AF(af_name, af_idx, af_fn, af_unit, af_type) \
{ \
    .name = MP_QSTR_ ## af_name, \
    .idx = (af_idx), \
    .fn = PIN_FN_ ## af_fn, \
    .unit = (af_unit), \
    .type = PIN_TYPE_ ## af_fn ## _ ## af_type, \
}


#define PIN(p_pin_name, p_port, p_bit, p_pin_num, p_af_list, p_num_afs) \
{ \
    { &pin_type }, \
    .name           = MP_QSTR_ ## p_pin_name, \
    .port           = PORT_A ## p_port, \
    .af_list        = (p_af_list), \
    .pull           = PIN_TYPE_STD, \
    .bit            = (p_bit), \
    .pin_num        = (p_pin_num), \
    .af             = PIN_MODE_0, \
    .strength       = PIN_STRENGTH_4MA, \
    .mode           = GPIO_DIR_MODE_IN, \
    .num_afs        = (p_num_afs), \
    .value          = 0, \
    .used           = false, \
    .irq_trigger    = 0, \
    .irq_flags      = 0, \
}
