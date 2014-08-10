/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "binary.h"

#if MICROPY_PY_ARRAY || MICROPY_PY_BUILTINS_BYTEARRAY

typedef struct _mp_obj_array_t {
    mp_obj_base_t base;
    mp_uint_t typecode : 8;
    // free is number of unused elements after len used elements
    // alloc size = len + free
    mp_uint_t free : (8 * sizeof(mp_uint_t) - 8);
    mp_uint_t len; // in elements
    void *items;
} mp_obj_array_t;

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in);
STATIC mp_obj_array_t *array_new(char typecode, uint n);
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg);
STATIC mp_int_t array_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, int flags);

/******************************************************************************/
/* array                                                                       */

STATIC void array_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_array_t *o = o_in;
    if (o->typecode == BYTEARRAY_TYPECODE) {
        print(env, "bytearray(b");
        mp_str_print_quoted(print, env, o->items, o->len, true);
    } else {
        print(env, "array('%c'", o->typecode);
        if (o->len > 0) {
            print(env, ", [");
            for (int i = 0; i < o->len; i++) {
                if (i > 0) {
                    print(env, ", ");
                }
                mp_obj_print_helper(print, env, mp_binary_get_val_array(o->typecode, o->items, i), PRINT_REPR);
            }
            print(env, "]");
        }
    }
    print(env, ")");
}

STATIC mp_obj_t array_construct(char typecode, mp_obj_t initializer) {
    uint len;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(initializer);
    if (len_in == MP_OBJ_NULL) {
        len = 0;
    } else {
        len = MP_OBJ_SMALL_INT_VALUE(len_in);
    }

    mp_obj_array_t *array = array_new(typecode, len);

    mp_obj_t iterable = mp_getiter(initializer);
    mp_obj_t item;
    int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (len == 0) {
            array_append(array, item);
        } else {
            mp_binary_set_val_array(typecode, array->items, i++, item);
        }
    }

    return array;
}

STATIC mp_obj_t array_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    // get typecode
    uint l;
    const char *typecode = mp_obj_str_get_data(args[0], &l);

    if (n_args == 1) {
        // 1 arg: make an empty array
        return array_new(*typecode, 0);
    } else {
        // 2 args: construct the array from the given iterator
        return array_construct(*typecode, args[1]);
    }
}

STATIC mp_obj_t bytearray_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args == 0) {
        // no args: construct an empty bytearray
        return array_new(BYTEARRAY_TYPECODE, 0);
    } else if (MP_OBJ_IS_SMALL_INT(args[0])) {
        // 1 arg, an integer: construct a blank bytearray of that length
        uint len = MP_OBJ_SMALL_INT_VALUE(args[0]);
        mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, len);
        memset(o->items, 0, len);
        return o;
    } else {
        // 1 arg, an iterator: construct the bytearray from that
        return array_construct(BYTEARRAY_TYPECODE, args[0]);
    }
}

STATIC mp_obj_t array_unary_op(int op, mp_obj_t o_in) {
    mp_obj_array_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(o->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(o->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t array_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    switch (op) {
        case MP_BINARY_OP_EQUAL: {
            mp_buffer_info_t lhs_bufinfo;
            mp_buffer_info_t rhs_bufinfo;
            array_get_buffer(lhs_in, &lhs_bufinfo, MP_BUFFER_READ);
            if (!mp_get_buffer(rhs_in, &rhs_bufinfo, MP_BUFFER_READ)) {
                    return mp_const_false;
            }
            return MP_BOOL(mp_seq_cmp_bytes(op, lhs_bufinfo.buf, lhs_bufinfo.len, rhs_bufinfo.buf, rhs_bufinfo.len));
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_array) || MP_OBJ_IS_TYPE(self_in, &mp_type_bytearray));
    mp_obj_array_t *self = self_in;
    if (self->free == 0) {
        int item_sz = mp_binary_get_size('@', self->typecode, NULL);
        // TODO: alloc policy
        self->free = 8;
        self->items = m_realloc(self->items,  item_sz * self->len, item_sz * (self->len + self->free));
        mp_seq_clear(self->items, self->len + 1, self->len + self->free, item_sz);
    }
    mp_binary_set_val_array(self->typecode, self->items, self->len++, arg);
    self->free--;
    return mp_const_none; // return None, as per CPython
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(array_append_obj, array_append);

STATIC mp_obj_t array_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // TODO implement
        // TODO: confirmed that both bytearray and array.array support
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } else {
        mp_obj_array_t *o = self_in;
        if (0) {
#if MICROPY_PY_BUILTINS_SLICE
        } else if (MP_OBJ_IS_TYPE(index_in, &mp_type_slice)) {
            if (value != MP_OBJ_SENTINEL) {
                // Only getting a slice is suported so far, not assignment
                // TODO: confirmed that both bytearray and array.array support
                // slice assignment (incl. of different size)
                return MP_OBJ_NULL; // op not supported
            }
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(o->len, index_in, &slice)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_NotImplementedError,
                    "only slices with step=1 (aka None) are supported"));
            }
            mp_obj_array_t *res = array_new(o->typecode, slice.stop - slice.start);
            int sz = mp_binary_get_size('@', o->typecode, NULL);
            assert(sz > 0);
            byte *p = o->items;
            memcpy(res->items, p + slice.start * sz, (slice.stop - slice.start) * sz);
            return res;
#endif
        } else {
            uint index = mp_get_index(o->base.type, o->len, index_in, false);
            if (value == MP_OBJ_SENTINEL) {
                // load
                return mp_binary_get_val_array(o->typecode, o->items, index);
            } else {
                // store
                mp_binary_set_val_array(o->typecode, o->items, index, value);
                return mp_const_none;
            }
        }
    }
}

STATIC mp_int_t array_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, int flags) {
    mp_obj_array_t *o = o_in;
    bufinfo->buf = o->items;
    bufinfo->len = o->len * mp_binary_get_size('@', o->typecode, NULL);
    bufinfo->typecode = o->typecode;
    return 0;
}

STATIC const mp_map_elem_t array_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_append), (mp_obj_t)&array_append_obj },
};

STATIC MP_DEFINE_CONST_DICT(array_locals_dict, array_locals_dict_table);

const mp_obj_type_t mp_type_array = {
    { &mp_type_type },
    .name = MP_QSTR_array,
    .print = array_print,
    .make_new = array_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .subscr = array_subscr,
    .buffer_p = { .get_buffer = array_get_buffer },
    .locals_dict = (mp_obj_t)&array_locals_dict,
};

const mp_obj_type_t mp_type_bytearray = {
    { &mp_type_type },
    .name = MP_QSTR_bytearray,
    .print = array_print,
    .make_new = bytearray_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .subscr = array_subscr,
    .buffer_p = { .get_buffer = array_get_buffer },
    .locals_dict = (mp_obj_t)&array_locals_dict,
};

STATIC mp_obj_array_t *array_new(char typecode, uint n) {
    int typecode_size = mp_binary_get_size('@', typecode, NULL);
    if (typecode_size <= 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "bad typecode"));
    }
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = (typecode == BYTEARRAY_TYPECODE) ? &mp_type_bytearray : &mp_type_array;
    o->typecode = typecode;
    o->free = 0;
    o->len = n;
    o->items = m_malloc(typecode_size * o->len);
    return o;
}

uint mp_obj_array_len(mp_obj_t self_in) {
    return ((mp_obj_array_t *)self_in)->len;
}

mp_obj_t mp_obj_new_bytearray(uint n, void *items) {
    mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, n);
    memcpy(o->items, items, n);
    return o;
}

// Create bytearray which references specified memory area
mp_obj_t mp_obj_new_bytearray_by_ref(uint n, void *items) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &mp_type_array;
    o->typecode = BYTEARRAY_TYPECODE;
    o->free = 0;
    o->len = n;
    o->items = items;
    return o;
}

/******************************************************************************/
/* array iterator                                                              */

typedef struct _mp_obj_array_it_t {
    mp_obj_base_t base;
    mp_obj_array_t *array;
    mp_uint_t cur;
} mp_obj_array_it_t;

STATIC mp_obj_t array_it_iternext(mp_obj_t self_in) {
    mp_obj_array_it_t *self = self_in;
    if (self->cur < self->array->len) {
        return mp_binary_get_val_array(self->array->typecode, self->array->items, self->cur++);
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

STATIC const mp_obj_type_t array_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = array_it_iternext,
};

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in) {
    mp_obj_array_t *array = array_in;
    mp_obj_array_it_t *o = m_new_obj(mp_obj_array_it_t);
    o->base.type = &array_it_type;
    o->array = array;
    o->cur = 0;
    return o;
}

#endif // MICROPY_PY_ARRAY || MICROPY_PY_BUILTINS_BYTEARRAY
