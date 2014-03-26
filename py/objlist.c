#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime0.h"
#include "runtime.h"

typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    machine_uint_t alloc;
    machine_uint_t len;
    mp_obj_t *items;
} mp_obj_list_t;

STATIC mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur);
STATIC mp_obj_list_t *list_new(uint n);
STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in);

// TODO: Move to mpconfig.h
#define LIST_MIN_ALLOC 4

/******************************************************************************/
/* list                                                                       */

STATIC void list_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_list_t *o = o_in;
    print(env, "[");
    for (int i = 0; i < o->len; i++) {
        if (i > 0) {
            print(env, ", ");
        }
        mp_obj_print_helper(print, env, o->items[i], PRINT_REPR);
    }
    print(env, "]");
}

STATIC mp_obj_t list_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            // return a new, empty list
            return mp_obj_new_list(0, NULL);

        case 1:
        {
            // make list from iterable
            mp_obj_t iterable = rt_getiter(args[0]);
            mp_obj_t list = mp_obj_new_list(0, NULL);
            mp_obj_t item;
            while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
                mp_obj_list_append(list, item);
            }
            return list;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "list takes at most 1 argument, %d given", n_args));
    }
    return NULL;
}

// Don't pass RT_BINARY_OP_NOT_EQUAL here
STATIC bool list_cmp_helper(int op, mp_obj_t self_in, mp_obj_t another_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    if (!MP_OBJ_IS_TYPE(another_in, &list_type)) {
        return false;
    }
    mp_obj_list_t *self = self_in;
    mp_obj_list_t *another = another_in;

    return mp_seq_cmp_objs(op, self->items, self->len, another->items, another->len);
}

STATIC mp_obj_t list_unary_op(int op, mp_obj_t self_in) {
    mp_obj_list_t *self = self_in;
    switch (op) {
        case RT_UNARY_OP_BOOL: return MP_BOOL(self->len != 0);
        case RT_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported for None
    }
}

STATIC mp_obj_t list_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_list_t *o = lhs;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
#if MICROPY_ENABLE_SLICE
            if (MP_OBJ_IS_TYPE(rhs, &slice_type)) {
                machine_uint_t start, stop;
                if (!m_seq_get_fast_slice_indexes(o->len, rhs, &start, &stop)) {
                    assert(0);
                }
                mp_obj_list_t *res = list_new(stop - start);
                m_seq_copy(res->items, o->items + start, res->len, mp_obj_t);
                return res;
            }
#endif
            uint index = mp_get_index(o->base.type, o->len, rhs, false);
            return o->items[index];
        }
        case RT_BINARY_OP_ADD:
        {
            if (!MP_OBJ_IS_TYPE(rhs, &list_type)) {
                return NULL;
            }
            mp_obj_list_t *p = rhs;
            mp_obj_list_t *s = list_new(o->len + p->len);
            m_seq_cat(s->items, o->items, o->len, p->items, p->len, mp_obj_t);
            return s;
        }
        case RT_BINARY_OP_INPLACE_ADD:
        {
            if (!MP_OBJ_IS_TYPE(rhs, &list_type)) {
                return NULL;
            }
            list_extend(lhs, rhs);
            return o;
        }
        case RT_BINARY_OP_MULTIPLY:
        {
            if (!MP_OBJ_IS_SMALL_INT(rhs)) {
                return NULL;
            }
            int n = MP_OBJ_SMALL_INT_VALUE(rhs);
            mp_obj_list_t *s = list_new(o->len * n);
            mp_seq_multiply(o->items, sizeof(*o->items), o->len, n, s->items);
            return s;
        }
        case RT_BINARY_OP_EQUAL:
        case RT_BINARY_OP_LESS:
        case RT_BINARY_OP_LESS_EQUAL:
        case RT_BINARY_OP_MORE:
        case RT_BINARY_OP_MORE_EQUAL:
            return MP_BOOL(list_cmp_helper(op, lhs, rhs));
        case RT_BINARY_OP_NOT_EQUAL:
            return MP_BOOL(!list_cmp_helper(RT_BINARY_OP_EQUAL, lhs, rhs));

        default:
            // op not supported
            return NULL;
    }
}

STATIC mp_obj_t list_getiter(mp_obj_t o_in) {
    return mp_obj_new_list_iterator(o_in, 0);
}

mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    if (self->len >= self->alloc) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc * 2);
        assert(self->items);
        self->alloc *= 2;
    }
    self->items[self->len++] = arg;
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_extend(mp_obj_t self_in, mp_obj_t arg_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    assert(MP_OBJ_IS_TYPE(arg_in, &list_type));
    mp_obj_list_t *self = self_in;
    mp_obj_list_t *arg = arg_in;

    if (self->len + arg->len > self->alloc) {
        // TODO: use alloc policy for "4"
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->len + arg->len + 4);
        self->alloc = self->len + arg->len + 4;
    }

    memcpy(self->items + self->len, arg->items, sizeof(mp_obj_t) * arg->len);
    self->len += arg->len;
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_pop(uint n_args, const mp_obj_t *args) {
    assert(1 <= n_args && n_args <= 2);
    assert(MP_OBJ_IS_TYPE(args[0], &list_type));
    mp_obj_list_t *self = args[0];
    if (self->len == 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_IndexError, "pop from empty list"));
    }
    uint index = mp_get_index(self->base.type, self->len, n_args == 1 ? mp_obj_new_int(-1) : args[1], false);
    mp_obj_t ret = self->items[index];
    self->len -= 1;
    memcpy(self->items + index, self->items + index + 1, (self->len - index) * sizeof(mp_obj_t));
    if (self->alloc > LIST_MIN_ALLOC && self->alloc > 2 * self->len) {
        self->items = m_renew(mp_obj_t, self->items, self->alloc, self->alloc/2);
        self->alloc /= 2;
    }
    return ret;
}

// TODO make this conform to CPython's definition of sort
STATIC void mp_quicksort(mp_obj_t *head, mp_obj_t *tail, mp_obj_t key_fn, bool reversed) {
    int op = reversed ? RT_BINARY_OP_MORE : RT_BINARY_OP_LESS;
    while (head < tail) {
        mp_obj_t *h = head - 1;
        mp_obj_t *t = tail;
        mp_obj_t v = key_fn == NULL ? tail[0] : rt_call_function_1(key_fn, tail[0]); // get pivot using key_fn
        for (;;) {
            do ++h; while (rt_binary_op(op, key_fn == NULL ? h[0] : rt_call_function_1(key_fn, h[0]), v) == mp_const_true);
            do --t; while (h < t && rt_binary_op(op, v, key_fn == NULL ? t[0] : rt_call_function_1(key_fn, t[0])) == mp_const_true);
            if (h >= t) break;
            mp_obj_t x = h[0];
            h[0] = t[0];
            t[0] = x;
        }
        mp_obj_t x = h[0];
        h[0] = tail[0];
        tail[0] = x;
        mp_quicksort(head, t, key_fn, reversed);
        head = h + 1;
    }
}

mp_obj_t mp_obj_list_sort(uint n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    assert(n_args >= 1);
    assert(MP_OBJ_IS_TYPE(args[0], &list_type));
    if (n_args > 1) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError,
                                          "list.sort takes no positional arguments"));
    }
    mp_obj_list_t *self = args[0];
    if (self->len > 1) {
        mp_map_elem_t *keyfun = mp_map_lookup(kwargs, MP_OBJ_NEW_QSTR(MP_QSTR_key), MP_MAP_LOOKUP);
        mp_map_elem_t *reverse = mp_map_lookup(kwargs, MP_OBJ_NEW_QSTR(MP_QSTR_reverse), MP_MAP_LOOKUP);
        mp_quicksort(self->items, self->items + self->len - 1,
                     keyfun ? keyfun->value : NULL,
                     reverse && reverse->value ? rt_is_true(reverse->value) : false);
    }
    return mp_const_none; // return None, as per CPython
}

STATIC mp_obj_t list_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    self->len = 0;
    self->items = m_renew(mp_obj_t, self->items, self->alloc, LIST_MIN_ALLOC);
    self->alloc = LIST_MIN_ALLOC;
    return mp_const_none;
}

STATIC mp_obj_t list_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    return mp_obj_new_list(self->len, self->items);
}

STATIC mp_obj_t list_count(mp_obj_t self_in, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    return mp_seq_count_obj(self->items, self->len, value);
}

STATIC mp_obj_t list_index(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 4);
    assert(MP_OBJ_IS_TYPE(args[0], &list_type));
    mp_obj_list_t *self = args[0];
    return mp_seq_index_obj(self->items, self->len, n_args, args);
}

STATIC mp_obj_t list_insert(mp_obj_t self_in, mp_obj_t idx, mp_obj_t obj) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    // insert has its own strange index logic
    int index = MP_OBJ_SMALL_INT_VALUE(idx);
    if (index < 0) {
         index += self->len;
    }
    if (index < 0) {
         index = 0;
    }
    if (index > self->len) {
         index = self->len;
    }

    mp_obj_list_append(self_in, mp_const_none);

    for (int i = self->len-1; i > index; i--) {
         self->items[i] = self->items[i-1];
    }
    self->items[index] = obj;

    return mp_const_none;
}

STATIC mp_obj_t list_remove(mp_obj_t self_in, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_t args[] = {self_in, value};
    args[1] = list_index(2, args);
    list_pop(2, args);

    return mp_const_none;
}

STATIC mp_obj_t list_reverse(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;

    int len = self->len;
    for (int i = 0; i < len/2; i++) {
         mp_obj_t *a = self->items[i];
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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(list_remove_obj, list_remove);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_reverse_obj, list_reverse);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(list_sort_obj, 0, mp_obj_list_sort);

STATIC const mp_map_elem_t list_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_append), (mp_obj_t)&list_append_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&list_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_copy), (mp_obj_t)&list_copy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&list_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_extend), (mp_obj_t)&list_extend_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_index), (mp_obj_t)&list_index_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_insert), (mp_obj_t)&list_insert_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pop), (mp_obj_t)&list_pop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&list_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reverse), (mp_obj_t)&list_reverse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&list_sort_obj },
};

STATIC MP_DEFINE_CONST_DICT(list_locals_dict, list_locals_dict_table);

const mp_obj_type_t list_type = {
    { &mp_type_type },
    .name = MP_QSTR_list,
    .print = list_print,
    .make_new = list_make_new,
    .unary_op = list_unary_op,
    .binary_op = list_binary_op,
    .getiter = list_getiter,
    .locals_dict = (mp_obj_t)&list_locals_dict,
};

STATIC mp_obj_list_t *list_new(uint n) {
    mp_obj_list_t *o = m_new_obj(mp_obj_list_t);
    o->base.type = &list_type;
    o->alloc = n < LIST_MIN_ALLOC ? LIST_MIN_ALLOC : n;
    o->len = n;
    o->items = m_new(mp_obj_t, o->alloc);
    return o;
}

mp_obj_t mp_obj_new_list(uint n, mp_obj_t *items) {
    mp_obj_list_t *o = list_new(n);
    if (items != NULL) {
        for (int i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return o;
}

void mp_obj_list_get(mp_obj_t self_in, uint *len, mp_obj_t **items) {
    mp_obj_list_t *self = self_in;
    *len = self->len;
    *items = self->items;
}

void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_list_t *self = self_in;
    uint i = mp_get_index(self->base.type, self->len, index, false);
    self->items[i] = value;
}

/******************************************************************************/
/* list iterator                                                              */

typedef struct _mp_obj_list_it_t {
    mp_obj_base_t base;
    mp_obj_list_t *list;
    machine_uint_t cur;
} mp_obj_list_it_t;

mp_obj_t list_it_iternext(mp_obj_t self_in) {
    mp_obj_list_it_t *self = self_in;
    if (self->cur < self->list->len) {
        mp_obj_t o_out = self->list->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t list_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .iternext = list_it_iternext,
};

mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur) {
    mp_obj_list_it_t *o = m_new_obj(mp_obj_list_it_t);
    o->base.type = &list_it_type;
    o->list = list;
    o->cur = cur;
    return o;
}
