/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_BINDINGS_SAMD_CLOCK_H
#define MICROPY_INCLUDED_ATMEL_SAMD_BINDINGS_SAMD_CLOCK_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t type;
    uint8_t index;
} samd_clock_obj_t;

#define CLOCK(_name, _type, _index) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = _type, \
        .index = _index, \
    }

#define CLOCK_SOURCE(_name) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = 0, \
        .index = GCLK_SOURCE_##_name, \
    }

#define CLOCK_GCLK(_name) \
    const samd_clock_obj_t clock_##_name = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name, \
        .type = 1, \
        .index = _name##_GCLK_ID, \
    }

#define CLOCK_GCLK_(_name, _extra) \
    const samd_clock_obj_t clock_##_name##_##_extra = { \
        { &samd_clock_type }, \
        .name = MP_QSTR_##_name##_##_extra, \
        .type = 1, \
        .index = _name##_GCLK_ID_##_extra, \
    }

#define CLOCK_ENTRY(_name)  { MP_ROM_QSTR(MP_QSTR_##_name), MP_ROM_PTR(&clock_##_name) }
#define CLOCK_ENTRY_(_name, _extra)  { MP_ROM_QSTR(MP_QSTR_##_name##_##_extra), MP_ROM_PTR(&clock_##_name##_##_extra) }

extern const mp_obj_type_t samd_clock_type;
extern const mp_obj_dict_t samd_clock_globals;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_BINDINGS_SAMD_CLOCK_H
