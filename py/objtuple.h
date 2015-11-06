/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef __MICROPY_INCLUDED_PY_OBJTUPLE_H__
#define __MICROPY_INCLUDED_PY_OBJTUPLE_H__

#include "py/obj.h"

typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    mp_uint_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

typedef struct _mp_rom_obj_tuple_t {
    mp_obj_base_t base;
    mp_uint_t len;
    mp_rom_obj_t items[];
} mp_rom_obj_tuple_t;

void mp_obj_tuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
mp_obj_t mp_obj_tuple_unary_op(mp_uint_t op, mp_obj_t self_in);
mp_obj_t mp_obj_tuple_binary_op(mp_uint_t op, mp_obj_t lhs, mp_obj_t rhs);
mp_obj_t mp_obj_tuple_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value);
mp_obj_t mp_obj_tuple_getiter(mp_obj_t o_in);

extern const mp_obj_type_t mp_type_attrtuple;

#define MP_DEFINE_ATTRTUPLE(tuple_obj_name, fields, nitems, ...) \
    const mp_rom_obj_tuple_t tuple_obj_name = { \
        .base = {&mp_type_attrtuple}, \
        .len = nitems, \
        .items = { __VA_ARGS__ , MP_ROM_PTR((void*)fields) } \
    }

#if MICROPY_PY_COLLECTIONS
void mp_obj_attrtuple_print_helper(const mp_print_t *print, const qstr *fields, mp_obj_tuple_t *o);
#endif

mp_obj_t mp_obj_new_attrtuple(const qstr *fields, mp_uint_t n, const mp_obj_t *items);

#endif // __MICROPY_INCLUDED_PY_OBJTUPLE_H__
