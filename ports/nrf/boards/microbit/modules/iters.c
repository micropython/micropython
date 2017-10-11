/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015/6 Mark Shannon
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

#include "py/runtime.h"
#include "lib/iters.h"


typedef struct _repeat_iterator_t {
    mp_obj_base_t base;
    mp_obj_t iterable;
    mp_int_t index;
} repeat_iterator_t;

static mp_obj_t microbit_repeat_iter_next(mp_obj_t iter_in) {
    repeat_iterator_t *iter = (repeat_iterator_t *)iter_in;
    iter->index++;
    if (iter->index >= mp_obj_get_int(mp_obj_len(iter->iterable))) {
        iter->index = 0;
    }
    return mp_obj_subscr(iter->iterable, MP_OBJ_NEW_SMALL_INT(iter->index), MP_OBJ_SENTINEL);
}

const mp_obj_type_t microbit_repeat_iterator_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .print = NULL,
    .make_new = NULL,
    .call = NULL,
    .unary_op = NULL,
    .binary_op = NULL,
    .attr = NULL,
    .subscr = NULL,
    .getiter = mp_identity,
    .iternext = microbit_repeat_iter_next,
    .buffer_p = {NULL},
    .stream_p = NULL,
    .bases_tuple = MP_OBJ_NULL,
    MP_OBJ_NULL
};

mp_obj_t microbit_repeat_iterator(mp_obj_t iterable) {
    repeat_iterator_t *result = m_new_obj(repeat_iterator_t);
    result->base.type = &microbit_repeat_iterator_type;
    result->iterable = iterable;
    result->index = -1;
    return result;
}
