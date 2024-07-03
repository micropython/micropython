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

#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"

typedef struct _mp_obj_bound_meth_t {
    mp_obj_base_t base;
    mp_obj_t meth;
    mp_obj_t self;
} mp_obj_bound_meth_t;

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
static void bound_meth_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_bound_meth_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "<bound_method %p ", o);
    mp_obj_print_helper(print, o->self, PRINT_REPR);
    mp_print_str(print, ".");
    mp_obj_print_helper(print, o->meth, PRINT_REPR);
    mp_print_str(print, ">");
}
#endif

mp_obj_t mp_call_method_self_n_kw(mp_obj_t meth, mp_obj_t self, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // need to insert self before all other args and then call meth
    size_t n_total = n_args + 2 * n_kw;
    mp_obj_t *args2 = NULL;
    #if MICROPY_ENABLE_PYSTACK
    args2 = mp_pystack_alloc(sizeof(mp_obj_t) * (1 + n_total));
    #else
    mp_obj_t *free_args2 = NULL;
    if (n_total > 4) {
        // try to use heap to allocate temporary args array
        args2 = m_new_maybe(mp_obj_t, 1 + n_total);
        free_args2 = args2;
    }
    if (args2 == NULL) {
        // (fallback to) use stack to allocate temporary args array
        args2 = alloca(sizeof(mp_obj_t) * (1 + n_total));
    }
    #endif
    args2[0] = self;
    memcpy(args2 + 1, args, n_total * sizeof(mp_obj_t));
    mp_obj_t res = mp_call_function_n_kw(meth, n_args + 1, n_kw, args2);
    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_free(args2);
    #else
    if (free_args2 != NULL) {
        m_del(mp_obj_t, free_args2, 1 + n_total);
    }
    #endif
    return res;
}

static mp_obj_t bound_meth_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_bound_meth_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_call_method_self_n_kw(self->meth, self->self, n_args, n_kw, args);
}

static mp_obj_t bound_meth_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_bound_meth_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_HASH:
            return MP_OBJ_NEW_SMALL_INT((mp_uint_t)self->self ^ (mp_uint_t)self->meth);
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t bound_meth_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    // The MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE flag is clear for this type, so if this
    // function is called with MP_BINARY_OP_EQUAL then lhs_in and rhs_in must have the
    // same type, which is mp_type_bound_meth.
    if (op != MP_BINARY_OP_EQUAL) {
        return MP_OBJ_NULL; // op not supported
    }
    mp_obj_bound_meth_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    mp_obj_bound_meth_t *rhs = MP_OBJ_TO_PTR(rhs_in);
    return mp_obj_new_bool(lhs->self == rhs->self && lhs->meth == rhs->meth);
}

#if MICROPY_PY_FUNCTION_ATTRS
static void bound_meth_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    // Delegate the load to the method object
    mp_obj_bound_meth_t *self = MP_OBJ_TO_PTR(self_in);
    mp_load_method_maybe(self->meth, attr, dest);
}
#endif

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_DETAILED
#define BOUND_METH_TYPE_PRINT print, bound_meth_print,
#else
#define BOUND_METH_TYPE_PRINT
#endif

#if MICROPY_PY_FUNCTION_ATTRS
#define BOUND_METH_TYPE_ATTR attr, bound_meth_attr,
#else
#define BOUND_METH_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_bound_meth,
    MP_QSTR_bound_method,
    MP_TYPE_FLAG_NONE,
    BOUND_METH_TYPE_PRINT
    BOUND_METH_TYPE_ATTR
    call, bound_meth_call,
    unary_op, bound_meth_unary_op,
    binary_op, bound_meth_binary_op
    );

mp_obj_t mp_obj_new_bound_meth(mp_obj_t meth, mp_obj_t self) {
    mp_obj_bound_meth_t *o = mp_obj_malloc(mp_obj_bound_meth_t, &mp_type_bound_meth);
    o->meth = meth;
    o->self = self;
    return MP_OBJ_FROM_PTR(o);
}
