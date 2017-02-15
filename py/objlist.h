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
#ifndef __MICROPY_INCLUDED_PY_OBJLIST_H__
#define __MICROPY_INCLUDED_PY_OBJLIST_H__

#include "py/obj.h"

typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    mp_uint_t alloc;
    mp_uint_t len;
    mp_obj_t *items;
} mp_obj_list_t;


mp_obj_t list_getiter(mp_obj_t o_in);
mp_obj_t list_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value);
mp_obj_t list_binary_op(mp_uint_t op, mp_obj_t lhs, mp_obj_t rhs);
mp_obj_t list_unary_op(mp_uint_t op, mp_obj_t self_in);

mp_obj_t list_extend_from_iter(mp_obj_t list, mp_obj_t iterable);
mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in);
mp_obj_t list_pop(size_t n_args, const mp_obj_t *args);
mp_obj_t list_clear(mp_obj_t self_in);
mp_obj_t list_count(mp_obj_t self_in, mp_obj_t value);
mp_obj_t list_index(size_t n_args, const mp_obj_t *args);
mp_obj_t list_insert(mp_obj_t self_in, mp_obj_t idx, mp_obj_t obj);
mp_obj_t list_reverse(mp_obj_t self_in);

#endif // __MICROPY_INCLUDED_PY_OBJLIST_H__
