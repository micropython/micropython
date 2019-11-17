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
#include "py/runtime.h"
#include "py/misc.h"

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

#if MICROPY_PY_BUILTINS_SLICE_INDICES
STATIC mp_obj_t slice_indices(mp_obj_t self_in, mp_obj_t length_obj) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t length = mp_obj_int_get_checked(length_obj);
    mp_int_t start, stop, step;
    mp_obj_t results[3]; // In start, stop, step order

    if (self->step == mp_const_none) {
        step = 1;
    } else {
        step = mp_obj_int_get_checked(self->step);
        if (step == 0) {
            mp_raise_ValueError("slice step cannot be zero");
        }
    }

    if (step > 0) {
        // Positive step
        if (self->start == mp_const_none) {
            start = 0;
        } else {
            start = mp_obj_int_get_checked(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length, MAX(start, 0));
        }

        if (self->stop == mp_const_none) {
            stop = length;
        } else {
            stop = mp_obj_int_get_checked(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length, MAX(stop, 0));
        }
    } else {
        // Negative step
        if (self->start == mp_const_none) {
            start = length - 1;
        } else {
            start = mp_obj_int_get_checked(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length-1, MAX(start, -1));
        }

        if (self->stop == mp_const_none) {
            stop = -1;
        } else {
            stop = mp_obj_int_get_checked(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length-1, MAX(stop, -1));
        }
    }

    results[0] = MP_OBJ_NEW_SMALL_INT(start);
    results[1] = MP_OBJ_NEW_SMALL_INT(stop);
    results[2] = MP_OBJ_NEW_SMALL_INT(step);

    return mp_obj_new_tuple(3, results);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(slice_indices_obj, slice_indices);
#endif

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
#if MICROPY_PY_BUILTINS_SLICE_INDICES
    } else if (attr == MP_QSTR_indices) {
        dest[0] = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&slice_indices_obj),
                                        self_in);
#endif
    }
}
#endif

#if MICROPY_PY_BUILTINS_SLICE_INDICES && !MICROPY_PY_BUILTINS_SLICE_ATTRS
STATIC const mp_rom_map_elem_t slice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_indices), MP_ROM_PTR(&slice_indices_obj) },
    { MP_ROM_QSTR(MP_QSTR_wombat), MP_ROM_PTR(&slice_indices_obj) },
};

STATIC MP_DEFINE_CONST_DICT(slice_locals_dict, slice_locals_dict_table);
#endif

const mp_obj_type_t mp_type_slice = {
    { &mp_type_type },
    .name = MP_QSTR_slice,
    .print = slice_print,
#if MICROPY_PY_BUILTINS_SLICE_ATTRS
    .attr = slice_attr,
#elif MICROPY_PY_BUILTINS_SLICE_INDICES
    .locals_dict = (mp_obj_dict_t*)&slice_locals_dict,
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
