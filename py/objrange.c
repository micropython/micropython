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

#include "py/runtime.h"

/******************************************************************************/
/* range iterator                                                             */

typedef struct _mp_obj_range_it_t {
    mp_obj_base_t base;
    mp_int_t cur;
    mp_int_t stop;
    mp_int_t step;
} mp_obj_range_it_t;

static mp_obj_t range_it_iternext(mp_obj_t o_in) {
    mp_obj_range_it_t *o = MP_OBJ_TO_PTR(o_in);
    if ((o->step > 0 && o->cur < o->stop) || (o->step < 0 && o->cur > o->stop)) {
        mp_int_t cur = o->cur;
        o->cur += o->step;
        return mp_obj_new_int(cur);
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_range_it,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, range_it_iternext
    );

static mp_obj_t mp_obj_new_range_iterator(mp_int_t cur, mp_int_t stop, mp_int_t step, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_range_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_range_it_t *o = (mp_obj_range_it_t *)iter_buf;
    o->base.type = &mp_type_range_it;
    o->cur = cur;
    o->stop = stop;
    o->step = step;
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
/* range                                                                      */

typedef struct _mp_obj_range_t {
    mp_obj_base_t base;
    mp_int_t start;
    mp_int_t stop;
    mp_int_t step;
} mp_obj_range_t;

static void range_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_range_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "range(" INT_FMT ", " INT_FMT "", self->start, self->stop);
    if (self->step == 1) {
        mp_print_str(print, ")");
    } else {
        mp_printf(print, ", " INT_FMT ")", self->step);
    }
}

static mp_obj_t range_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    mp_obj_range_t *o = mp_obj_malloc(mp_obj_range_t, type);
    o->start = 0;
    o->step = 1;

    if (n_args == 1) {
        o->stop = mp_obj_get_int(args[0]);
    } else {
        o->start = mp_obj_get_int(args[0]);
        o->stop = mp_obj_get_int(args[1]);
        if (n_args == 3) {
            o->step = mp_obj_get_int(args[2]);
            if (o->step == 0) {
                mp_raise_ValueError(MP_ERROR_TEXT("zero step"));
            }
        }
    }

    return MP_OBJ_FROM_PTR(o);
}

static mp_int_t range_len(mp_obj_range_t *self) {
    // When computing length, need to take into account step!=1 and step<0.
    mp_int_t len = self->stop - self->start + self->step;
    if (self->step > 0) {
        len -= 1;
    } else {
        len += 1;
    }
    len = len / self->step;
    if (len < 0) {
        len = 0;
    }
    return len;
}

static mp_obj_t range_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_range_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t len = range_len(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len > 0);
        case MP_UNARY_OP_LEN:
            return mp_obj_new_int(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

#if MICROPY_PY_BUILTINS_RANGE_BINOP
static mp_obj_t range_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (!mp_obj_is_type(rhs_in, &mp_type_range) || op != MP_BINARY_OP_EQUAL) {
        return MP_OBJ_NULL; // op not supported
    }
    mp_obj_range_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    mp_obj_range_t *rhs = MP_OBJ_TO_PTR(rhs_in);
    mp_int_t lhs_len = range_len(lhs);
    mp_int_t rhs_len = range_len(rhs);
    return mp_obj_new_bool(
        lhs_len == rhs_len
        && (lhs_len == 0
            || (lhs->start == rhs->start
                && (lhs_len == 1 || lhs->step == rhs->step)))
        );
}
#endif

static mp_obj_t range_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value == MP_OBJ_SENTINEL) {
        // load
        mp_obj_range_t *self = MP_OBJ_TO_PTR(self_in);
        mp_int_t len = range_len(self);
        #if MICROPY_PY_BUILTINS_SLICE
        if (mp_obj_is_type(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            mp_obj_slice_indices(index, len, &slice);
            mp_obj_range_t *o = mp_obj_malloc(mp_obj_range_t, &mp_type_range);
            o->start = self->start + slice.start * self->step;
            o->stop = self->start + slice.stop * self->step;
            o->step = slice.step * self->step;
            return MP_OBJ_FROM_PTR(o);
        }
        #endif
        size_t index_val = mp_get_index(self->base.type, len, index, false);
        return mp_obj_new_int(self->start + index_val * self->step);
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t range_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_range_t *o = MP_OBJ_TO_PTR(o_in);
    return mp_obj_new_range_iterator(o->start, o->stop, o->step, iter_buf);
}


#if MICROPY_PY_BUILTINS_RANGE_ATTRS
static void range_attr(mp_obj_t o_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    mp_obj_range_t *o = MP_OBJ_TO_PTR(o_in);
    if (attr == MP_QSTR_start) {
        dest[0] = mp_obj_new_int(o->start);
    } else if (attr == MP_QSTR_stop) {
        dest[0] = mp_obj_new_int(o->stop);
    } else if (attr == MP_QSTR_step) {
        dest[0] = mp_obj_new_int(o->step);
    }
}
#endif

#if MICROPY_PY_BUILTINS_RANGE_BINOP
#define RANGE_TYPE_BINOP binary_op, range_binary_op,
#else
#define RANGE_TYPE_BINOP
#endif

#if MICROPY_PY_BUILTINS_RANGE_ATTRS
#define RANGE_TYPE_ATTR attr, range_attr,
#else
#define RANGE_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_range,
    MP_QSTR_range,
    MP_TYPE_FLAG_NONE,
    make_new, range_make_new,
    RANGE_TYPE_BINOP
    RANGE_TYPE_ATTR
    print, range_print,
    unary_op, range_unary_op,
    subscr, range_subscr,
    iter, range_getiter
    );
