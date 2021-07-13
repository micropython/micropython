/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int16_t value;
    int16_t name;
} cp_enum_obj_t;

#define MAKE_ENUM_VALUE(type, prefix, name, value) \
    const cp_enum_obj_t prefix##_##name##_obj = {  \
        { &type }, value, MP_QSTR_##name, \
    }

#define MAKE_ENUM_MAP(name) \
    const mp_rom_map_elem_t name##_locals_table[] =

#define MAKE_ENUM_MAP_ENTRY(prefix, name) \
    { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_PTR(&prefix##_##name##_obj) }

#define MAKE_PRINTER(module, typename) \
    STATIC void typename##_##print(const mp_print_t * print, mp_obj_t self_in, mp_print_kind_t kind) { \
        cp_enum_obj_print_helper(MP_QSTR_##module, print, self_in, kind); \
    }

#define MAKE_ENUM_TYPE(module, type, typename) \
    const mp_obj_type_t typename##_type = { \
        { &mp_type_type }, \
        .name = MP_QSTR_##type, \
        .print = typename##_print, \
        .locals_dict = (mp_obj_t)&typename##_locals_dict, \
    }



mp_obj_t cp_enum_find(const mp_obj_type_t *type, int value);
int cp_enum_value(const mp_obj_type_t *type, mp_obj_t *obj);
void cp_enum_obj_print_helper(uint16_t module, const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
