/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include "py/objtuple.h"

#if MICROPY_PY_ATTRTUPLE || MICROPY_PY_COLLECTIONS

// this helper function is used by collections.namedtuple
#if !MICROPY_PY_COLLECTIONS
STATIC
#endif
void mp_obj_attrtuple_print_helper(const mp_print_t *print, const qstr *fields, mp_obj_tuple_t *o) {
    mp_print_str(print, "(");
    for (size_t i = 0; i < o->len; i++) {
        if (i > 0) {
            mp_print_str(print, ", ");
        }
        mp_printf(print, "%q=", fields[i]);
        mp_obj_print_helper(print, o->items[i], PRINT_REPR);
    }
    mp_print_str(print, ")");
}

#endif

#if MICROPY_PY_ATTRTUPLE

STATIC void mp_obj_attrtuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_tuple_t *o = MP_OBJ_TO_PTR(o_in);
    const qstr *fields = (const qstr *)MP_OBJ_TO_PTR(o->items[o->len]);
    mp_obj_attrtuple_print_helper(print, fields, o);
}

STATIC void mp_obj_attrtuple_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
        size_t len = self->len;
        const qstr *fields = (const qstr *)MP_OBJ_TO_PTR(self->items[len]);
        for (size_t i = 0; i < len; i++) {
            if (fields[i] == attr) {
                dest[0] = self->items[i];
                return;
            }
        }
    }
}

mp_obj_t mp_obj_new_attrtuple(const qstr *fields, size_t n, const mp_obj_t *items) {
    mp_obj_tuple_t *o = m_new_obj_var(mp_obj_tuple_t, mp_obj_t, n + 1);
    o->base.type = &mp_type_attrtuple;
    o->len = n;
    for (size_t i = 0; i < n; i++) {
        o->items[i] = items[i];
    }
    o->items[n] = MP_OBJ_FROM_PTR(fields);
    return MP_OBJ_FROM_PTR(o);
}

const mp_obj_type_t mp_type_attrtuple = {
    { &mp_type_type },
    .name = MP_QSTR_tuple, // reuse tuple to save on a qstr
    .print = mp_obj_attrtuple_print,
    .unary_op = mp_obj_tuple_unary_op,
    .binary_op = mp_obj_tuple_binary_op,
    .attr = mp_obj_attrtuple_attr,
    .subscr = mp_obj_tuple_subscr,
    .getiter = mp_obj_tuple_getiter,
};

#endif // MICROPY_PY_ATTRTUPLE
