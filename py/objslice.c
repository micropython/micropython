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

#include "py/runtime.h"

/******************************************************************************/
/* slice object                                                               */

#if MICROPY_PY_BUILTINS_SLICE

static void slice_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
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

static mp_obj_t slice_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    // Needed to explicitly opt out of default __hash__.
    // REVISIT: CPython implements comparison operators for slice.
    return MP_OBJ_NULL;
}

#if MICROPY_PY_BUILTINS_SLICE_INDICES
static mp_obj_t slice_indices(mp_obj_t self_in, mp_obj_t length_obj) {
    mp_int_t length = mp_obj_get_int(length_obj);
    mp_bound_slice_t bound_indices;
    mp_obj_slice_indices(self_in, length, &bound_indices);

    mp_obj_t results[3] = {
        MP_OBJ_NEW_SMALL_INT(bound_indices.start),
        MP_OBJ_NEW_SMALL_INT(bound_indices.stop),
        MP_OBJ_NEW_SMALL_INT(bound_indices.step),
    };
    return mp_obj_new_tuple(3, results);
}
static MP_DEFINE_CONST_FUN_OBJ_2(slice_indices_obj, slice_indices);
#endif

#if MICROPY_PY_BUILTINS_SLICE_ATTRS
static void slice_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
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
        dest[0] = MP_OBJ_FROM_PTR(&slice_indices_obj);
        dest[1] = self_in;
    #endif
    }
}
#endif

#if MICROPY_PY_BUILTINS_SLICE_INDICES && !MICROPY_PY_BUILTINS_SLICE_ATTRS
static const mp_rom_map_elem_t slice_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_indices), MP_ROM_PTR(&slice_indices_obj) },
};
static MP_DEFINE_CONST_DICT(slice_locals_dict, slice_locals_dict_table);
#endif

#if MICROPY_PY_BUILTINS_SLICE_ATTRS
#define SLICE_TYPE_ATTR_OR_LOCALS_DICT attr, slice_attr,
#elif MICROPY_PY_BUILTINS_SLICE_INDICES
#define SLICE_TYPE_ATTR_OR_LOCALS_DICT locals_dict, &slice_locals_dict,
#else
#define SLICE_TYPE_ATTR_OR_LOCALS_DICT
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_slice,
    MP_QSTR_slice,
    MP_TYPE_FLAG_NONE,
    unary_op, slice_unary_op,
    SLICE_TYPE_ATTR_OR_LOCALS_DICT
    print, slice_print
    );

mp_obj_t mp_obj_new_slice(mp_obj_t ostart, mp_obj_t ostop, mp_obj_t ostep) {
    mp_obj_slice_t *o = mp_obj_malloc(mp_obj_slice_t, &mp_type_slice);
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
            mp_raise_ValueError(MP_ERROR_TEXT("slice step can't be zero"));
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

#endif
