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
#include <assert.h>

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/runtime0.h"
#include "py/runtime.h"

/******************************************************************************/
/* tuple                                                                      */

void mp_obj_tuple_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_tuple_t *o = MP_OBJ_TO_PTR(o_in);
    if (MICROPY_PY_UJSON && kind == PRINT_JSON) {
        mp_print_str(print, "[");
    } else {
        mp_print_str(print, "(");
        kind = PRINT_REPR;
    }
    for (size_t i = 0; i < o->len; i++) {
        if (i > 0) {
            mp_print_str(print, ", ");
        }
        mp_obj_print_helper(print, o->items[i], kind);
    }
    if (MICROPY_PY_UJSON && kind == PRINT_JSON) {
        mp_print_str(print, "]");
    } else {
        if (o->len == 1) {
            mp_print_str(print, ",");
        }
        mp_print_str(print, ")");
    }
}

STATIC mp_obj_t mp_obj_tuple_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;

    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    switch (n_args) {
        case 0:
            // return a empty tuple
            return mp_const_empty_tuple;

        case 1:
        default: {
            // 1 argument, an iterable from which we make a new tuple
            if (MP_OBJ_IS_TYPE(args[0], &mp_type_tuple)) {
                return args[0];
            }

            // TODO optimise for cases where we know the length of the iterator

            size_t alloc = 4;
            size_t len = 0;
            mp_obj_t *items = m_new(mp_obj_t, alloc);

            mp_obj_t iterable = mp_getiter(args[0], NULL);
            mp_obj_t item;
            while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                if (len >= alloc) {
                    items = m_renew(mp_obj_t, items, alloc, alloc * 2);
                    alloc *= 2;
                }
                items[len++] = item;
            }

            mp_obj_t tuple = mp_obj_new_tuple(len, items);
            m_del(mp_obj_t, items, alloc);

            return tuple;
        }
    }
}

// Don't pass MP_BINARY_OP_NOT_EQUAL here
STATIC bool tuple_cmp_helper(mp_uint_t op, mp_obj_t self_in, mp_obj_t another_in) {
    // type check is done on getiter method to allow tuple, namedtuple, attrtuple
    mp_check_self(mp_obj_get_type(self_in)->getiter == mp_obj_tuple_getiter);
    mp_obj_type_t *another_type = mp_obj_get_type(another_in);
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
    if (another_type->getiter != mp_obj_tuple_getiter) {
        // Slow path for user subclasses
        another_in = mp_instance_cast_to_native_base(another_in, MP_OBJ_FROM_PTR(&mp_type_tuple));
        if (another_in == MP_OBJ_NULL) {
            return false;
        }
    }
    mp_obj_tuple_t *another = MP_OBJ_TO_PTR(another_in);

    return mp_seq_cmp_objs(op, self->items, self->len, another->items, another->len);
}

mp_obj_t mp_obj_tuple_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(self->len != 0);
        case MP_UNARY_OP_HASH: {
            // start hash with pointer to empty tuple, to make it fairly unique
            mp_int_t hash = (mp_int_t)mp_const_empty_tuple;
            for (size_t i = 0; i < self->len; i++) {
                hash += MP_OBJ_SMALL_INT_VALUE(mp_unary_op(MP_UNARY_OP_HASH, self->items[i]));
            }
            return MP_OBJ_NEW_SMALL_INT(hash);
        }
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

mp_obj_t mp_obj_tuple_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_tuple_t *o = MP_OBJ_TO_PTR(lhs);
    switch (op) {
        case MP_BINARY_OP_ADD:
        case MP_BINARY_OP_INPLACE_ADD: {
            if (!mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(rhs)), MP_OBJ_FROM_PTR(&mp_type_tuple))) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_obj_tuple_t *p = MP_OBJ_TO_PTR(rhs);
            mp_obj_tuple_t *s = MP_OBJ_TO_PTR(mp_obj_new_tuple(o->len + p->len, NULL));
            mp_seq_cat(s->items, o->items, o->len, p->items, p->len, mp_obj_t);
            return MP_OBJ_FROM_PTR(s);
        }
        case MP_BINARY_OP_MULTIPLY:
        case MP_BINARY_OP_INPLACE_MULTIPLY: {
            mp_int_t n;
            if (!mp_obj_get_int_maybe(rhs, &n)) {
                return MP_OBJ_NULL; // op not supported
            }
            if (n <= 0) {
                return mp_const_empty_tuple;
            }
            mp_obj_tuple_t *s = MP_OBJ_TO_PTR(mp_obj_new_tuple(o->len * n, NULL));
            mp_seq_multiply(o->items, sizeof(*o->items), o->len, n, s->items);
            return MP_OBJ_FROM_PTR(s);
        }
        case MP_BINARY_OP_EQUAL:
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
            return mp_obj_new_bool(tuple_cmp_helper(op, lhs, rhs));

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

mp_obj_t mp_obj_tuple_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value == MP_OBJ_SENTINEL) {
        // load
        mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice)) {
                mp_raise_NotImplementedError("only slices with step=1 (aka None) are supported");
            }
            mp_obj_tuple_t *res = MP_OBJ_TO_PTR(mp_obj_new_tuple(slice.stop - slice.start, NULL));
            mp_seq_copy(res->items, self->items + slice.start, res->len, mp_obj_t);
            return MP_OBJ_FROM_PTR(res);
        }
#endif
        size_t index_value = mp_get_index(self->base.type, self->len, index, false);
        return self->items[index_value];
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t tuple_count(mp_obj_t self_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_tuple));
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_seq_count_obj(self->items, self->len, value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tuple_count_obj, tuple_count);

STATIC mp_obj_t tuple_index(size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_TYPE(args[0], &mp_type_tuple));
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(args[0]);
    return mp_seq_index_obj(self->items, self->len, n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(tuple_index_obj, 2, 4, tuple_index);

STATIC const mp_rom_map_elem_t tuple_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&tuple_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&tuple_index_obj) },
};

STATIC MP_DEFINE_CONST_DICT(tuple_locals_dict, tuple_locals_dict_table);

const mp_obj_type_t mp_type_tuple = {
    { &mp_type_type },
    .name = MP_QSTR_tuple,
    .print = mp_obj_tuple_print,
    .make_new = mp_obj_tuple_make_new,
    .unary_op = mp_obj_tuple_unary_op,
    .binary_op = mp_obj_tuple_binary_op,
    .subscr = mp_obj_tuple_subscr,
    .getiter = mp_obj_tuple_getiter,
    .locals_dict = (mp_obj_dict_t*)&tuple_locals_dict,
};

// the zero-length tuple
const mp_obj_tuple_t mp_const_empty_tuple_obj = {{&mp_type_tuple}, 0};

mp_obj_t mp_obj_new_tuple(size_t n, const mp_obj_t *items) {
    if (n == 0) {
        return mp_const_empty_tuple;
    }
    mp_obj_tuple_t *o = m_new_obj_var(mp_obj_tuple_t, mp_obj_t, n);
    o->base.type = &mp_type_tuple;
    o->len = n;
    if (items) {
        for (size_t i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return MP_OBJ_FROM_PTR(o);
}

void mp_obj_tuple_get(mp_obj_t self_in, size_t *len, mp_obj_t **items) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_tuple));
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
    *len = self->len;
    *items = &self->items[0];
}

void mp_obj_tuple_del(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_tuple));
    mp_obj_tuple_t *self = MP_OBJ_TO_PTR(self_in);
    m_del_var(mp_obj_tuple_t, mp_obj_t, self->len, self);
}

/******************************************************************************/
/* tuple iterator                                                             */

typedef struct _mp_obj_tuple_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_tuple_t *tuple;
    size_t cur;
} mp_obj_tuple_it_t;

STATIC mp_obj_t tuple_it_iternext(mp_obj_t self_in) {
    mp_obj_tuple_it_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->cur < self->tuple->len) {
        mp_obj_t o_out = self->tuple->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t mp_obj_tuple_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_tuple_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_tuple_it_t *o = (mp_obj_tuple_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = tuple_it_iternext;
    o->tuple = MP_OBJ_TO_PTR(o_in);
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
}
