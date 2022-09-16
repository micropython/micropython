/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Paul Sokolovsky
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

#include <unistd.h> // for ssize_t
#include <string.h>

#include "py/mpconfig.h"
#if MICROPY_PY_COLLECTIONS_DEQUE

#include "py/runtime.h"

typedef struct _mp_obj_deque_t {
    mp_obj_base_t base;
    size_t alloc;
    size_t i_get;
    size_t i_put;
    mp_obj_t *items;
    uint32_t flags;
    #define FLAG_CHECK_OVERFLOW 1
} mp_obj_deque_t;

STATIC mp_obj_t deque_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 3, false);

    /* Initialization from existing sequence is not supported, so an empty
       tuple must be passed as such. */
    if (args[0] != mp_const_empty_tuple) {
        mp_raise_ValueError(NULL);
    }

    // Protect against -1 leading to zero-length allocation and bad array access
    mp_int_t maxlen = mp_obj_get_int(args[1]);
    if (maxlen < 0) {
        mp_raise_ValueError(NULL);
    }

    mp_obj_deque_t *o = mp_obj_malloc(mp_obj_deque_t, type);
    o->alloc = maxlen + 1;
    o->i_get = o->i_put = 0;
    o->items = m_new0(mp_obj_t, o->alloc);

    if (n_args > 2) {
        o->flags = mp_obj_get_int(args[2]);
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t deque_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(self->i_get != self->i_put);
        case MP_UNARY_OP_LEN: {
            ssize_t len = self->i_put - self->i_get;
            if (len < 0) {
                len += self->alloc;
            }
            return MP_OBJ_NEW_SMALL_INT(len);
        }
        #if MICROPY_PY_SYS_GETSIZEOF
        case MP_UNARY_OP_SIZEOF: {
            size_t sz = sizeof(*self) + sizeof(mp_obj_t) * self->alloc;
            return MP_OBJ_NEW_SMALL_INT(sz);
        }
        #endif
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t mp_obj_deque_append(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);

    size_t new_i_put = self->i_put + 1;
    if (new_i_put == self->alloc) {
        new_i_put = 0;
    }

    if (self->flags & FLAG_CHECK_OVERFLOW && new_i_put == self->i_get) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("full"));
    }

    self->items[self->i_put] = arg;
    self->i_put = new_i_put;

    if (self->i_get == new_i_put) {
        if (++self->i_get == self->alloc) {
            self->i_get = 0;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_append_obj, mp_obj_deque_append);

STATIC mp_obj_t deque_popleft(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->i_get == self->i_put) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("empty"));
    }

    mp_obj_t ret = self->items[self->i_get];
    self->items[self->i_get] = MP_OBJ_NULL;

    if (++self->i_get == self->alloc) {
        self->i_get = 0;
    }

    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_popleft_obj, deque_popleft);

#if 0
STATIC mp_obj_t deque_clear(mp_obj_t self_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    self->i_get = self->i_put = 0;
    mp_seq_clear(self->items, 0, self->alloc, sizeof(*self->items));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_clear_obj, deque_clear);
#endif

STATIC const mp_rom_map_elem_t deque_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&deque_append_obj) },
    #if 0
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&deque_clear_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_popleft), MP_ROM_PTR(&deque_popleft_obj) },
};

STATIC MP_DEFINE_CONST_DICT(deque_locals_dict, deque_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_deque,
    MP_QSTR_deque,
    MP_TYPE_FLAG_NONE,
    make_new, deque_make_new,
    unary_op, deque_unary_op,
    locals_dict, &deque_locals_dict
    );

#endif // MICROPY_PY_COLLECTIONS_DEQUE
