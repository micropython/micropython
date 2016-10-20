/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include "py/objlist.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

STATIC mp_obj_t mp_obj_new_list_iterator(mp_obj_t list, mp_uint_t cur);
STATIC mp_obj_list_t *list_new(mp_uint_t n);
STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in);
STATIC mp_obj_t list_pop(size_t n_args, const mp_obj_t *args);

// TODO: Move to mpconfig.h
#define LIST_MIN_ALLOC 4

/******************************************************************************/
/* list                                                                       */

STATIC void list_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_list_t *o = MP_OBJ_TO_PTR(o_in);
    if (!(MICROPY_PY_UJSON && kind == PRINT_JSON)) {
        kind = PRINT_REPR;
    }
    mp_print_str(print, "[");
    for (mp_uint_t i = 0; i < o->len; i++) {
        if (i > 0) {
            mp_print_str(print, ", ");
        }
        mp_obj_print_helper(print, o->items[i], kind);
    }
    mp_print_str(print, "]");
}

STATIC mp_obj_t list_extend_from_iter(mp_obj_t list, mp_obj_t iterable) {
    mp_obj_t iter = mp_getiter(iterable);
    mp_obj_t item;
    while ((item = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        mp_obj_list_append(list, item);
    }
    return list;
}

STATIC mp_obj_t list_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    switch (n_args) {
        case 0:
            // return a new, empty list
            return mp_obj_new_list(0, NULL);

        case 1:
        default: {
            // make list from iterable
            // TODO: optimize list/tuple
            mp_obj_t list = mp_obj_new_list(0, NULL);
            return list_extend_from_iter(list, args[0]);
        }
    }
}

// Don't pass MP_BINARY_OP_NOT_EQUAL here
STATIC bool list_cmp_helper(mp_uint_t op, mp_obj_t self_in, mp_obj_t another_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    if (!MP_OBJ_IS_TYPE(another_in, &mp_type_list)) {
        return false;
    }
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *another = MP_OBJ_TO_PTR(another_in);

    return mp_seq_cmp_objs(op, self->items, self->len, another->items, another->len);
}

STATIC mp_obj_t list_unary_op(mp_uint_t op, mp_obj_t self_in) {
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(self->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t list_binary_op(mp_uint_t op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_list_t *o = MP_OBJ_TO_PTR(lhs);
    switch (op) {
        case MP_BINARY_OP_ADD: {
            if (!MP_OBJ_IS_TYPE(rhs, &mp_type_list)) {
                return MP_OBJ_NULL; // op not supported
            }
            mp_obj_list_t *p = MP_OBJ_TO_PTR(rhs);
            mp_obj_list_t *s = list_new(o->len + p->len);
            mp_seq_cat(s->items, o->items, o->len, p->items, p->len, mp_obj_t);
            return MP_OBJ_FROM_PTR(s);
        }
        case MP_BINARY_OP_INPLACE_ADD: {
            list_extend(lhs, rhs);
            return lhs;
        }
        case MP_BINARY_OP_MULTIPLY: {
            mp_int_t n;
            if (!mp_obj_get_int_maybe(rhs, &n)) {
                return MP_OBJ_NULL; // op not supported
            }
            if (n < 0) {
                n = 0;
            }
            mp_obj_list_t *s = list_new(o->len * n);
            mp_seq_multiply(o->items, sizeof(*o->items), o->len, n, s->items);
            return MP_OBJ_FROM_PTR(s);
        }
        case MP_BINARY_OP_EQUAL:
        case MP_BINARY_OP_LESS:
        case MP_BINARY_OP_LESS_EQUAL:
        case MP_BINARY_OP_MORE:
        case MP_BINARY_OP_MORE_EQUAL:
            return mp_obj_new_bool(list_cmp_helper(op, lhs, rhs));

        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t list_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice)) {
                mp_not_implemented("");
            }

            mp_int_t len_adj = slice.start - slice.stop;
            //printf("Len adj: %d\n", len_adj);
            assert(len_adj <= 0);
            mp_seq_replace_slice_no_grow(self->items, self->len, slice.start, slice.stop, self->items/*NULL*/, 0, sizeof(*self->items));
            // Clear "freed" elements at the end of list
            mp_seq_clear(self->items, self->len + len_adj, self->len, sizeof(*self->items));
            self->len += len_adj;
            return mp_const_none;
        }
#endif
        mp_obj_t args[2] = {self_in, index};
        list_pop(2, args);
        return mp_const_none;
    } else if (value == MP_OBJ_SENTINEL) {
        // load
        mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_bound_slice_t slice;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice)) {
                return mp_seq_extract_slice(self->len, self->items, &slice);
            }
            mp_obj_list_t *res = list_new(slice.stop - slice.start);
            mp_seq_copy(res->items, self->items + slice.start, res->len, mp_obj_t);
            return MP_OBJ_FROM_PTR(res);
        }
#endif
        mp_uint_t index_val = mp_get_index(self->base.type, self->len, index, false);
        return self->items[index_val];
    } else {
#if MICROPY_PY_BUILTINS_SLICE
        if (MP_OBJ_IS_TYPE(index, &mp_type_slice)) {
            mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
            mp_check_self(MP_OBJ_IS_TYPE(value, &mp_type_list));
            mp_obj_list_t *slice = MP_OBJ_TO_PTR(value);
            mp_bound_slice_t slice_out;
            if (!mp_seq_get_fast_slice_indexes(self->len, index, &slice_out)) {
                mp_not_implemented("");
            }
            mp_int_t len_adj = slice->len - (slice_out.stop - slice_out.start);
            //printf("Len adj: %d\n", len_adj);
            if (len_adj > 0) {
                if (self->len + len_adj > self->alloc) {
                    // TODO: Might optimize memory copies here by checking if block can
                    // be grown inplace or not
                    self->items = m_renew(mp_obj_t, self->items, self->alloc, self->len + len_adj);
                    self->alloc = self->len + len_adj;
                }
                mp_seq_replace_slice_grow_inplace(self->items, self->len,
                    slice_out.start, slice_out.stop, slice->items, slice->len, len_adj, sizeof(*self->items));
            } else {
                mp_seq_replace_slice_no_grow(self->items, self->len,
                    slice_out.start, slice_out.stop, slice->items, slice->len, sizeof(*self->items));
                // Clear "freed" elements at the end of list
                mp_seq_clear(self->items, self->len + len_adj, self->len, sizeof(*self->items));
                // TODO: apply allocation policy re: alloc_size
            }
            self->len += len_adj;
            return mp_const_none;
        }
#endif
        mp_obj_list_store(self_in, index, value);
        return mp_const_none;
    }
}

STATIC mp_obj_t list_getiter(mp_obj_t o_in) {
    return mp_obj_new_list_iterator(o_in, 0);
}

mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->len >= self->alloc) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc * 2);
        self->alloc *= 2;
        mp_seq_clear(self->items, self->len + 1, self->alloc, sizeof(*self->items));
    }
    self->items[self->len++] = arg;
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    if (MP_OBJ_IS_TYPE(arg_in, &mp_type_list)) {
        mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
        mp_obj_list_t *arg = MP_OBJ_TO_PTR(arg_in);

        if (self->len + arg->len > self->alloc) {
            // TODO: use alloc policy for "4"
            self->items = m_renew(mp_obj_t, self->items, self->alloc, self->len + arg->len + 4);
            self->alloc = self->len + arg->len + 4;
            mp_seq_clear(self->items, self->len + arg->len, self->alloc, sizeof(*self->items));
        }

        memcpy(self->items + self->len, arg->items, sizeof(mp_obj_t) * arg->len);
        self->len += arg->len;
    } else {
        list_extend_from_iter(self_in, arg_in);
    }
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_pop(size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_TYPE(args[0], &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->len == 0) {
        mp_raise_msg(&mp_type_IndexError, "pop from empty list");
    }
    mp_uint_t index = mp_get_index(self->base.type, self->len, n_args == 1 ? MP_OBJ_NEW_SMALL_INT(-1) : args[1], false);
    mp_obj_t ret = self->items[index];
    self->len -= 1;
    memmove(self->items + index, self->items + index + 1, (self->len - index) * sizeof(mp_obj_t));
    // Clear stale pointer from slot which just got freed to prevent GC issues
    self->items[self->len] = MP_OBJ_NULL;
    if (self->alloc > LIST_MIN_ALLOC && self->alloc > 2 * self->len) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc/2);
        self->alloc /= 2;
    }
    return ret;
}

STATIC void mp_quicksort(mp_obj_t *head, mp_obj_t *tail, mp_obj_t key_fn, mp_obj_t binop_less_result) {
    MP_STACK_CHECK();
    while (head < tail) {
        mp_obj_t *h = head - 1;
        mp_obj_t *t = tail;
        mp_obj_t v = key_fn == MP_OBJ_NULL ? tail[0] : mp_call_function_1(key_fn, tail[0]); // get pivot using key_fn
        for (;;) {
            do ++h; while (h < t && mp_binary_op(MP_BINARY_OP_LESS, key_fn == MP_OBJ_NULL ? h[0] : mp_call_function_1(key_fn, h[0]), v) == binop_less_result);
            do --t; while (h < t && mp_binary_op(MP_BINARY_OP_LESS, v, key_fn == MP_OBJ_NULL ? t[0] : mp_call_function_1(key_fn, t[0])) == binop_less_result);
            if (h >= t) break;
            mp_obj_t x = h[0];
            h[0] = t[0];
            t[0] = x;
        }
        mp_obj_t x = h[0];
        h[0] = tail[0];
        tail[0] = x;
        // do the smaller recursive call first, to keep stack within O(log(N))
        if (t - head < tail - h - 1) {
            mp_quicksort(head, t, key_fn, binop_less_result);
            head = h + 1;
        } else {
            mp_quicksort(h + 1, tail, key_fn, binop_less_result);
            tail = t;
        }
    }
}

// TODO Python defines sort to be stable but ours is not
mp_obj_t mp_obj_list_sort(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
        { MP_QSTR_reverse, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    struct {
        mp_arg_val_t key, reverse;
    } args;
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    mp_check_self(MP_OBJ_IS_TYPE(pos_args[0], &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (self->len > 1) {
        mp_quicksort(self->items, self->items + self->len - 1,
                     args.key.u_obj == mp_const_none ? MP_OBJ_NULL : args.key.u_obj,
                     args.reverse.u_bool ? mp_const_false : mp_const_true);
    }

    return mp_const_none;
}

STATIC mp_obj_t list_clear(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    self->len = 0;
    self->items = m_renew(mp_obj_t, self->items, self->alloc, LIST_MIN_ALLOC);
    self->alloc = LIST_MIN_ALLOC;
    mp_seq_clear(self->items, 0, self->alloc, sizeof(*self->items));
    return mp_const_none;
}

STATIC mp_obj_t list_copy(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_list(self->len, self->items);
}

STATIC mp_obj_t list_count(mp_obj_t self_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_seq_count_obj(self->items, self->len, value);
}

STATIC mp_obj_t list_index(size_t n_args, const mp_obj_t *args) {
    mp_check_self(MP_OBJ_IS_TYPE(args[0], &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(args[0]);
    return mp_seq_index_obj(self->items, self->len, n_args, args);
}

STATIC mp_obj_t list_insert(mp_obj_t self_in, mp_obj_t idx, mp_obj_t obj) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    // insert has its own strange index logic
    mp_int_t index = MP_OBJ_SMALL_INT_VALUE(idx);
    if (index < 0) {
         index += self->len;
    }
    if (index < 0) {
         index = 0;
    }
    if ((mp_uint_t)index > self->len) {
         index = self->len;
    }

    mp_obj_list_append(self_in, mp_const_none);

    for (mp_int_t i = self->len-1; i > index; i--) {
         self->items[i] = self->items[i-1];
    }
    self->items[index] = obj;

    return mp_const_none;
}

mp_obj_t mp_obj_list_remove(mp_obj_t self_in, mp_obj_t value) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_t args[] = {self_in, value};
    args[1] = list_index(2, args);
    list_pop(2, args);

    return mp_const_none;
}

STATIC mp_obj_t list_reverse(mp_obj_t self_in) {
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_list));
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t len = self->len;
    for (mp_int_t i = 0; i < len/2; i++) {
         mp_obj_t a = self->items[i];
         self->items[i] = self->items[len-i-1];
         self->items[len-i-1] = a;
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_append_obj, mp_obj_list_append);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_extend_obj, list_extend);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_clear_obj, list_clear);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_copy_obj, list_copy);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_count_obj, list_count);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_index_obj, 2, 4, list_index);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(list_insert_obj, list_insert);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(list_pop_obj, 1, 2, list_pop);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_remove_obj, mp_obj_list_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_reverse_obj, list_reverse);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(list_sort_obj, 1, mp_obj_list_sort);

STATIC const mp_rom_map_elem_t list_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_append), MP_ROM_PTR(&list_append_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&list_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&list_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&list_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_extend), MP_ROM_PTR(&list_extend_obj) },
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&list_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_insert), MP_ROM_PTR(&list_insert_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop), MP_ROM_PTR(&list_pop_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&list_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_reverse), MP_ROM_PTR(&list_reverse_obj) },
    { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&list_sort_obj) },
};

STATIC MP_DEFINE_CONST_DICT(list_locals_dict, list_locals_dict_table);

const mp_obj_type_t mp_type_list = {
    { &mp_type_type },
    .name = MP_QSTR_list,
    .print = list_print,
    .make_new = list_make_new,
    .unary_op = list_unary_op,
    .binary_op = list_binary_op,
    .subscr = list_subscr,
    .getiter = list_getiter,
    .locals_dict = (mp_obj_dict_t*)&list_locals_dict,
};

void mp_obj_list_init(mp_obj_list_t *o, mp_uint_t n) {
    o->base.type = &mp_type_list;
    o->alloc = n < LIST_MIN_ALLOC ? LIST_MIN_ALLOC : n;
    o->len = n;
    o->items = m_new(mp_obj_t, o->alloc);
    mp_seq_clear(o->items, n, o->alloc, sizeof(*o->items));
}

STATIC mp_obj_list_t *list_new(mp_uint_t n) {
    mp_obj_list_t *o = m_new_obj(mp_obj_list_t);
    mp_obj_list_init(o, n);
    return o;
}

mp_obj_t mp_obj_new_list(mp_uint_t n, mp_obj_t *items) {
    mp_obj_list_t *o = list_new(n);
    if (items != NULL) {
        for (mp_uint_t i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return MP_OBJ_FROM_PTR(o);
}

void mp_obj_list_get(mp_obj_t self_in, mp_uint_t *len, mp_obj_t **items) {
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    *len = self->len;
    *items = self->items;
}

void mp_obj_list_set_len(mp_obj_t self_in, mp_uint_t len) {
    // trust that the caller knows what it's doing
    // TODO realloc if len got much smaller than alloc
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    self->len = len;
}

void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_list_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t i = mp_get_index(self->base.type, self->len, index, false);
    self->items[i] = value;
}

/******************************************************************************/
/* list iterator                                                              */

typedef struct _mp_obj_list_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t list;
    mp_uint_t cur;
} mp_obj_list_it_t;

STATIC mp_obj_t list_it_iternext(mp_obj_t self_in) {
    mp_obj_list_it_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_list_t *list = MP_OBJ_TO_PTR(self->list);
    if (self->cur < list->len) {
        mp_obj_t o_out = list->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t mp_obj_new_list_iterator(mp_obj_t list, mp_uint_t cur) {
    mp_obj_list_it_t *o = m_new_obj(mp_obj_list_it_t);
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = list_it_iternext;
    o->list = list;
    o->cur = cur;
    return MP_OBJ_FROM_PTR(o);
}
