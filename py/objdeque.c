/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2017 Damien P. George
 * Copyright (c) 2014, 2017 Paul Sokolovsky
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/objint.h"
#include "py/objlist.h"

#if MICROPY_PY_COLLECTIONS

// TODO: Move to mpconfig.h
#define DEQUE_MIN_ALLOC 4

typedef struct _mp_obj_deque_t {
    mp_obj_base_t base;
    mp_uint_t alloc;
    mp_uint_t len;
    mp_obj_t *items;
} mp_obj_deque_t;

STATIC void deque_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    kind = PRINT_REPR;
    mp_obj_deque_t *o = MP_OBJ_TO_PTR(o_in);
    vstr_t vstr;
    mp_print_t pr;
    vstr_init_print(&vstr, DEQUE_MIN_ALLOC, &pr);
    mp_print_str(&pr, "deque([");
    for (mp_uint_t i = 0; i < o->len; i++) {
        if (i > 0) {
            mp_print_str(&pr, ", ");
        }
        mp_obj_print_helper(&pr, o->items[i], kind);
    }
    mp_print_str(&pr, "])");
    print->print_strn(print->data, (const char*)vstr.buf, vstr.len);

    if ((MICROPY_PY_UJSON && kind == PRINT_JSON)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
            "%s is not JSON serializable", vstr.buf));
    }
}

STATIC void mp_obj_deque_init(mp_obj_deque_t *o, mp_uint_t n) {
    o->base.type = &mp_type_deque;
    o->alloc = n < DEQUE_MIN_ALLOC ? DEQUE_MIN_ALLOC : n;
    o->len = n;
    o->items = m_new(mp_obj_t, o->alloc);
    mp_seq_clear(o->items, n, o->alloc, sizeof(*o->items));
}

STATIC mp_obj_deque_t *deque_new(mp_uint_t n) {
    mp_obj_deque_t *o = m_new_obj(mp_obj_deque_t);
    mp_obj_deque_init(o, n);
    return o;
}

STATIC mp_obj_t mp_obj_new_deque(mp_uint_t n, mp_obj_t *items) {
    mp_obj_deque_t *o = deque_new(n);
    if (items != NULL) {
        for (mp_uint_t i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t deque_extend_left_or_right_from_iter(mp_obj_t deque, mp_obj_t iterable, bool right) {
    mp_obj_t iter = mp_getiter(iterable);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        if(right) {
            mp_obj_list_append(deque, item);
        } else {
            list_insert(deque, 0, item);
        }
    }
    return deque;
}

STATIC mp_obj_t deque_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    switch (n_args) {
        case 0:
            // return a new, empty deque
            return mp_obj_new_deque(0, NULL);

        case 1:
        default: {
            // make deque from iterable
            // TODO: optimize list/tuple
            mp_obj_t deque = mp_obj_new_deque(0, NULL);
            return deque_extend_left_or_right_from_iter(deque, args[0], true);
        }
    }
}

STATIC mp_obj_t mp_obj_deque_appendleft(mp_obj_t self_in, mp_obj_t arg) {
    list_insert(self_in, 0, arg);
    return mp_const_none;
}

STATIC mp_obj_t mp_obj_deque_extendleft(mp_obj_t self_in, mp_obj_t arg_in) {
    deque_extend_left_or_right_from_iter(self_in, arg_in, false);
    return mp_const_none;
}

STATIC mp_obj_t mp_obj_deque_popleft(mp_obj_t self_in) {
    mp_obj_t pop_args[2] = {self_in,  mp_obj_new_int_from_uint(0)};
    return list_pop(2, pop_args);
}

STATIC mp_obj_t mp_obj_deque_rotate(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t n_abs = mp_obj_get_int(mp_obj_int_abs(arg_in));
    mp_int_t n = mp_obj_get_int(arg_in);
    
    mp_obj_t pop_args[2] = {self_in};
    mp_obj_t idx;
    for (mp_int_t i = 0; i < n_abs; i++) {
        if (n > 0) {
            pop_args[1] = mp_obj_new_int_from_uint(self->len-1);
            idx = mp_obj_new_int_from_uint(0);
        } else {
            pop_args[1] = mp_obj_new_int_from_uint(0);
            idx = mp_obj_new_int_from_uint(self->len);
        }
        list_insert(self_in, idx, list_pop(2, pop_args));
    }
    return mp_const_none;
}


STATIC mp_obj_t mp_obj_deque_copy(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_deque));
    mp_obj_deque_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_deque(self->len, self->items);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_append_obj, mp_obj_list_append);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_appendleft_obj, mp_obj_deque_appendleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_extend_obj, list_extend);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_extendleft_obj, mp_obj_deque_extendleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_clear_obj, list_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_copy_obj, mp_obj_deque_copy);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_count_obj, list_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_index_obj, 2, 4, list_index);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(list_insert_obj, list_insert);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_pop_obj, 1, 2, list_pop);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(deque_popleft_obj, mp_obj_deque_popleft);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_remove_obj, mp_obj_list_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_reverse_obj, list_reverse);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(deque_rotate_obj, mp_obj_deque_rotate);

STATIC const mp_rom_map_elem_t deque_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&list_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_appendleft), MP_ROM_PTR(&deque_appendleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&list_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&deque_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&list_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&list_extend_obj) },
    { MP_ROM_QSTR(MP_QSTR_extendleft), MP_ROM_PTR(&deque_extendleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&list_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_insert), MP_ROM_PTR(&list_insert_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop), MP_ROM_PTR(&list_pop_obj) },
    { MP_ROM_QSTR(MP_QSTR_popleft), MP_ROM_PTR(&deque_popleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&list_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_reverse), MP_ROM_PTR(&list_reverse_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotate), MP_ROM_PTR(&deque_rotate_obj) },
};

STATIC MP_DEFINE_CONST_DICT(deque_locals_dict, deque_locals_dict_table);

const mp_obj_type_t mp_type_deque = {
    { &mp_type_type },
    .name = MP_QSTR_deque,
    .print = deque_print,
    .make_new = deque_make_new,
    .unary_op = list_unary_op,
    .binary_op = list_binary_op,
    .subscr = list_subscr,
    .getiter = list_getiter,
    .locals_dict = (mp_obj_dict_t*)&deque_locals_dict,
};

#endif
