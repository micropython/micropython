/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "iters.h"


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

MP_DEFINE_CONST_OBJ_TYPE(
    microbit_repeat_iterator_type,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, microbit_repeat_iter_next
    );

mp_obj_t microbit_repeat_iterator(mp_obj_t iterable) {
    repeat_iterator_t *result = mp_obj_malloc(repeat_iterator_t, &microbit_repeat_iterator_type);
    result->iterable = iterable;
    result->index = -1;
    return result;
}
