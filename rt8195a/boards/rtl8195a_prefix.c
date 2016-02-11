/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
 * Copyright (c) 2016 Chester Tseng
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

// rtl8195a_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>
#include <stdint.h>

#include "py/mpconfig.h"
#include "py/obj.h"
#include "hardware/objpin.h"
#include "PinNames.h"

#define AF(af_name, af_idx, af_fn, af_unit, af_type) \
{ \
    .name = MP_QSTR_ ## af_name, \
    .idx = (af_idx), \
    .fn = PIN_FN_ ## af_fn, \
    .unit = (af_unit), \
    .type = PIN_TYPE_ ## af_fn ## _ ## af_type, \
}


#define PIN(p_pin_name, p_port, p_pin_num) \
{ \
    { &pin_type }, \
    .name           = MP_QSTR_ ## p_pin_name, \
    .id             = (p_pin_name), \
    .port           = PORT_ ## p_port, \
    .pull           = PullNone, \
    .pin_num        = (p_pin_num), \
    .dir            = PIN_INPUT, \
    .value          = 0, \
    .used           = false, \
    .irq_trigger    = 0, \
    .irq_flags      = 0, \
}
