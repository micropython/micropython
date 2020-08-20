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
#include "py/runtime0.h"

#include "supervisor/shared/translate.h"

/******************************************************************************/
/* slice object                                                               */

#if MICROPY_PY_BUILTINS_SLICE

// TODO: This implements only variant of slice with 2 integer args only.
// CPython supports 3rd arg (step), plus args can be arbitrary Python objects.
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
STATIC mp_obj_t slice_indices(mp_obj_t self_in, mp_obj_t length_obj) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    if (!MP_OBJ_IS_SMALL_INT(length_obj)) {
        mp_raise_TypeError(translate("Length must be an int"));
    }

    int length = MP_OBJ_SMALL_INT_VALUE(length_obj);
    if (length < 0) {
        mp_raise_ValueError(translate("Length must be non-negative"));
    }

    mp_obj_t indices[3] = {MP_OBJ_NEW_SMALL_INT(0), length_obj, MP_OBJ_NEW_SMALL_INT(1)};
    mp_obj_t slice[2] = {self->start, self->stop};

    int step = 1;
    if (self->step != mp_const_none) {
        indices[2] = self->step;
        step = MP_OBJ_SMALL_INT_VALUE(self->step);
        if (step < 0) {
            indices[0] = MP_OBJ_NEW_SMALL_INT(length - 1);
            indices[1] = MP_OBJ_NEW_SMALL_INT(-1);
        }
        if (step == 0) {
            mp_raise_ValueError(translate("slice step cannot be zero"));
        }
    }
    for (int i = 0; i < 2; i++) {
        if (slice[i] == mp_const_none) {
            continue;
        }
        int value = MP_OBJ_SMALL_INT_VALUE(slice[i]);
        if (value < 0) {
            value += length;
        }
        if (value < 0) {
            if (step > 0) {
                value = 0;
            } else if (step < 0) {
                value = -1;
            }
        } else if (value > length) {
            value = length;
        }
        indices[i] = MP_OBJ_NEW_SMALL_INT(value);
    }

    mp_obj_t tuple = mp_obj_new_tuple(3, indices);

    return tuple;
}
MP_DEFINE_CONST_FUN_OBJ_2(slice_indices_obj, slice_indices);

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
    } else if (attr == MP_QSTR_indices) {
        mp_convert_member_lookup(self_in, self->base.type, (mp_obj_t) &slice_indices_obj, dest);
    }
}

STATIC mp_obj_t slice_make_new(const mp_obj_type_t *type,
        size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
#endif

const mp_obj_type_t mp_type_slice = {
    { &mp_type_type },
    .name = MP_QSTR_slice,
    .print = slice_print,
#if MICROPY_PY_BUILTINS_SLICE_ATTRS
    .make_new = slice_make_new,
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

// Return the real index and step values for a slice when applied to a sequence of
// the given length, resolving missing components, negative values and values off
// the end of the sequence.
void mp_obj_slice_indices(mp_obj_t self_in, mp_int_t length, mp_bound_slice_t *result) {
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t start, stop, step;

    if (self->step == mp_const_none) {
        step = 1;
    } else {
        step = mp_obj_get_int(self->step);
        if (step == 0) {
            mp_raise_ValueError(translate("slice step cannot be zero"));
        }
    }

    if (step > 0) {
        // Positive step
        if (self->start == mp_const_none) {
            start = 0;
        } else {
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length, MAX(start, 0));
        }

        if (self->stop == mp_const_none) {
            stop = length;
        } else {
            stop = mp_obj_get_int(self->stop);
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
            start = mp_obj_get_int(self->start);
            if (start < 0) {
                start += length;
            }
            start = MIN(length - 1, MAX(start, -1));
        }

        if (self->stop == mp_const_none) {
            stop = -1;
        } else {
            stop = mp_obj_get_int(self->stop);
            if (stop < 0) {
                stop += length;
            }
            stop = MIN(length - 1, MAX(stop, -1));
        }
    }

    result->start = start;
    result->stop = stop;
    result->step = step;
}

#if MICROPY_PY_BUILTINS_SLICE_ATTRS
STATIC mp_obj_t slice_make_new(const mp_obj_type_t *type,
        size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    if (type != &mp_type_slice) {
        mp_raise_NotImplementedError(translate("Cannot subclass slice"));
    }
    // check number of arguments
    mp_arg_check_num(n_args, kw_args, 1, 3, false);

    // 1st argument is the pin
    mp_obj_t start = mp_const_none;
    mp_obj_t stop = mp_const_none;
    mp_obj_t step = mp_const_none;
    if (n_args == 1) {
        stop = args[0];
    } else {
        start = args[0];
        stop = args[1];
        if (n_args == 3) {
            step = args[2];
        }
    }

    return mp_obj_new_slice(start, stop, step);
}
#endif

void mp_obj_slice_get(mp_obj_t self_in, mp_obj_t *start, mp_obj_t *stop, mp_obj_t *step) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_slice));
    mp_obj_slice_t *self = MP_OBJ_TO_PTR(self_in);
    *start = self->start;
    *stop = self->stop;
    *step = self->step;
}

#endif
