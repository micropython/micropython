/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <stdlib.h>
#include <assert.h>

#include "py/obj.h"

/******************************************************************************/
/* slice object                                                               */

#if MICROPY_PY_BUILTINS_SLICE

typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;

STATIC void slice_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_slice_t *o = MP_OBJ_TO_PTR(o_in);
    mp_print_str(print, "slice(");
    mp_obj_print_helper(print, o->start, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, o->stop, PRINT_REPR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, o->step, PRINT_REPR);
    mp_print_str(print, ")");
}

#if MICROPY_PY_BUILTINS_SLICE_ATTRS
STATIC void slice_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    if (attr == MP_QSTR_start) {
        dest[0] = self->start;
    } else if (attr == MP_QSTR_stop) {
        dest[0] = self->stop;
    } else if (attr == MP_QSTR_step) {
        dest[0] = self->step;
    }
}
#endif

const mp_obj_type_t mp_type_slice = {
    { &mp_type_type },
    .name = MP_QSTR_slice,
    .print = slice_print,
#if MICROPY_PY_BUILTINS_SLICE_ATTRS
    .attr = slice_attr,
#endif
};

mp_obj_t mp_obj_new_slice(mp_obj_t ostart, mp_obj_t ostop, mp_obj_t ostep) {
    mp_obj_slice_t *o = m_new_obj(mp_obj_slice_t);
    o->base.type = &mp_type_slice;
    o->start = ostart;
    o->stop = ostop;
    o->step = ostep;
    return MP_OBJ_FROM_PTR(o);
}

void mp_obj_slice_get(mp_obj_t self_in, mp_obj_t *start, mp_obj_t *stop, mp_obj_t *step) {
    assert(mp_obj_is_type(self_in, &mp_type_slice));
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    *start = self->start;
    *stop = self->stop;
    *step = self->step;
}

#endif
