/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2017 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_PY_OBJNAMEDTUPLE_H
#define MICROPY_INCLUDED_PY_OBJNAMEDTUPLE_H

#include <string.h>

#include "py/objtuple.h"
#include "py/runtime.h"
#include "py/objstr.h"

#if MICROPY_PY_COLLECTIONS

typedef struct _mp_obj_namedtuple_type_t {
    // This is a mp_obj_type_t with eight slots.
    mp_obj_empty_type_t base;
    void *slots[8];
    size_t n_fields;
    qstr fields[];
} mp_obj_namedtuple_type_t;

typedef struct _mp_obj_namedtuple_t {
    mp_obj_tuple_t tuple;
} mp_obj_namedtuple_t;

void namedtuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);
size_t mp_obj_namedtuple_find_field(const mp_obj_namedtuple_type_t *type, qstr name);
void namedtuple_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
mp_obj_namedtuple_type_t *mp_obj_new_namedtuple_base(size_t n_fields, mp_obj_t *fields);
mp_obj_t namedtuple_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);

// CIRCUITPY-CHANGE: yikes
#define NAMEDTUPLE_TYPE_BASE_AND_SLOTS_MAKE_NEW(type_name, make_new_fun) \
    .base = { \
        .base = { .type = &mp_type_type }, \
        .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE, \
        .name = type_name, \
        .slot_index_make_new = 1, \
        .slot_index_print = 2, \
        .slot_index_unary_op = 3, \
        .slot_index_binary_op = 4, \
        .slot_index_attr = 5, \
        .slot_index_subscr = 6, \
        .slot_index_iter = 7, \
        .slot_index_parent = 8, \
    }, \
    .slots = { \
        make_new_fun, \
        namedtuple_print, \
        mp_obj_tuple_unary_op, \
        mp_obj_tuple_binary_op, \
        namedtuple_attr, \
        mp_obj_tuple_subscr, \
        mp_obj_tuple_getiter, \
        (void *)&mp_type_tuple, \
    }

#define NAMEDTUPLE_TYPE_BASE_AND_SLOTS(type_name) \
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS_MAKE_NEW(type_name, namedtuple_make_new)

#endif // MICROPY_PY_COLLECTIONS

#endif  // MICROPY_INCLUDED_PY_OBJNAMEDTUPLE_H
