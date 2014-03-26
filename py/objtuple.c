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
#include "objtuple.h"

STATIC mp_obj_t mp_obj_new_tuple_iterator(mp_obj_tuple_t *tuple, int cur);

/******************************************************************************/
/* tuple                                                                      */

void tuple_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_tuple_t *o = o_in;
    print(env, "(");
    for (int i = 0; i < o->len; i++) {
        if (i > 0) {
            print(env, ", ");
        }
        mp_obj_print_helper(print, env, o->items[i], PRINT_REPR);
    }
    if (o->len == 1) {
        print(env, ",");
    }
    print(env, ")");
}

STATIC mp_obj_t tuple_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            // return a empty tuple
            return mp_const_empty_tuple;

        case 1:
        {
            // 1 argument, an iterable from which we make a new tuple
            if (MP_OBJ_IS_TYPE(args[0], &tuple_type)) {
                return args[0];
            }

            // TODO optimise for cases where we know the length of the iterator

            uint alloc = 4;
            uint len = 0;
            mp_obj_t *items = m_new(mp_obj_t, alloc);

            mp_obj_t iterable = rt_getiter(args[0]);
            mp_obj_t item;
            while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
                if (len >= alloc) {
                    items = m_renew(mp_obj_t, items, alloc, alloc * 2);
                    alloc *= 2;
                }
                items[len++] = item;
            }

            mp_obj_t tuple = mp_obj_new_tuple(len, items);
            m_free(items, alloc);

            return tuple;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "tuple takes at most 1 argument, %d given", n_args));
    }
}

// Don't pass RT_BINARY_OP_NOT_EQUAL here
STATIC bool tuple_cmp_helper(int op, mp_obj_t self_in, mp_obj_t another_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &tuple_type));
    if (!MP_OBJ_IS_TYPE(another_in, &tuple_type)) {
        return false;
    }
    mp_obj_tuple_t *self = self_in;
    mp_obj_tuple_t *another = another_in;

    return mp_seq_cmp_objs(op, self->items, self->len, another->items, another->len);
}

mp_obj_t tuple_unary_op(int op, mp_obj_t self_in) {
    mp_obj_tuple_t *self = self_in;
    switch (op) {
        case RT_UNARY_OP_BOOL: return MP_BOOL(self->len != 0);
        case RT_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported for None
    }
}

mp_obj_t tuple_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_tuple_t *o = lhs;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
#if MICROPY_ENABLE_SLICE
            if (MP_OBJ_IS_TYPE(rhs, &slice_type)) {
                machine_uint_t start, stop;
                if (!m_seq_get_fast_slice_indexes(o->len, rhs, &start, &stop)) {
                    assert(0);
                }
                mp_obj_tuple_t *res = mp_obj_new_tuple(stop - start, NULL);
                m_seq_copy(res->items, o->items + start, res->len, mp_obj_t);
                return res;
            }
#endif
            uint index = mp_get_index(o->base.type, o->len, rhs, false);
            return o->items[index];
        }
        case RT_BINARY_OP_ADD:
        {
            if (!mp_obj_is_subclass_fast(mp_obj_get_type(rhs), (mp_obj_t)&tuple_type)) {
                return NULL;
            }
            mp_obj_tuple_t *p = rhs;
            mp_obj_tuple_t *s = mp_obj_new_tuple(o->len + p->len, NULL);
            m_seq_cat(s->items, o->items, o->len, p->items, p->len, mp_obj_t);
            return s;
        }
        case RT_BINARY_OP_MULTIPLY:
        {
            if (!MP_OBJ_IS_SMALL_INT(rhs)) {
                return NULL;
            }
            int n = MP_OBJ_SMALL_INT_VALUE(rhs);
            mp_obj_tuple_t *s = mp_obj_new_tuple(o->len * n, NULL);
            mp_seq_multiply(o->items, sizeof(*o->items), o->len, n, s->items);
            return s;
        }
        case RT_BINARY_OP_EQUAL:
        case RT_BINARY_OP_LESS:
        case RT_BINARY_OP_LESS_EQUAL:
        case RT_BINARY_OP_MORE:
        case RT_BINARY_OP_MORE_EQUAL:
            return MP_BOOL(tuple_cmp_helper(op, lhs, rhs));
        case RT_BINARY_OP_NOT_EQUAL:
            return MP_BOOL(!tuple_cmp_helper(RT_BINARY_OP_EQUAL, lhs, rhs));

        default:
            // op not supported
            return NULL;
    }
}

STATIC mp_obj_t tuple_getiter(mp_obj_t o_in) {
    return mp_obj_new_tuple_iterator(o_in, 0);
}

STATIC mp_obj_t tuple_count(mp_obj_t self_in, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &tuple_type));
    mp_obj_tuple_t *self = self_in;
    return mp_seq_count_obj(self->items, self->len, value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(tuple_count_obj, tuple_count);

STATIC mp_obj_t tuple_index(uint n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(args[0], &tuple_type));
    mp_obj_tuple_t *self = args[0];
    return mp_seq_index_obj(self->items, self->len, n_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(tuple_index_obj, 2, 4, tuple_index);

STATIC const mp_map_elem_t tuple_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_count), (mp_obj_t)&tuple_count_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_index), (mp_obj_t)&tuple_index_obj },
};

STATIC MP_DEFINE_CONST_DICT(tuple_locals_dict, tuple_locals_dict_table);

const mp_obj_type_t tuple_type = {
    { &mp_type_type },
    .name = MP_QSTR_tuple,
    .print = tuple_print,
    .make_new = tuple_make_new,
    .unary_op = tuple_unary_op,
    .binary_op = tuple_binary_op,
    .getiter = tuple_getiter,
    .locals_dict = (mp_obj_t)&tuple_locals_dict,
};

// the zero-length tuple
STATIC const mp_obj_tuple_t empty_tuple_obj = {{&tuple_type}, 0};
const mp_obj_t mp_const_empty_tuple = (mp_obj_t)&empty_tuple_obj;

mp_obj_t mp_obj_new_tuple(uint n, const mp_obj_t *items) {
    if (n == 0) {
        return mp_const_empty_tuple;
    }
    mp_obj_tuple_t *o = m_new_obj_var(mp_obj_tuple_t, mp_obj_t, n);
    o->base.type = &tuple_type;
    o->len = n;
    if (items) {
        for (int i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return o;
}

void mp_obj_tuple_get(mp_obj_t self_in, uint *len, mp_obj_t **items) {
    assert(MP_OBJ_IS_TYPE(self_in, &tuple_type));
    mp_obj_tuple_t *self = self_in;
    if (len) {
        *len = self->len;
    }
    if (items) {
        *items = &self->items[0];
    }
}

void mp_obj_tuple_del(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &tuple_type));
    mp_obj_tuple_t *self = self_in;
    m_del_var(mp_obj_tuple_t, mp_obj_t, self->len, self);
}

machine_int_t mp_obj_tuple_hash(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &tuple_type));
    mp_obj_tuple_t *self = self_in;
    // start hash with pointer to empty tuple, to make it fairly unique
    machine_int_t hash = (machine_int_t)mp_const_empty_tuple;
    for (uint i = 0; i < self->len; i++) {
        hash += mp_obj_hash(self->items[i]);
    }
    return hash;
}

/******************************************************************************/
/* tuple iterator                                                             */

typedef struct _mp_obj_tuple_it_t {
    mp_obj_base_t base;
    mp_obj_tuple_t *tuple;
    machine_uint_t cur;
} mp_obj_tuple_it_t;

STATIC mp_obj_t tuple_it_iternext(mp_obj_t self_in) {
    mp_obj_tuple_it_t *self = self_in;
    if (self->cur < self->tuple->len) {
        mp_obj_t o_out = self->tuple->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t tuple_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .iternext = tuple_it_iternext,
};

STATIC mp_obj_t mp_obj_new_tuple_iterator(mp_obj_tuple_t *tuple, int cur) {
    mp_obj_tuple_it_t *o = m_new_obj(mp_obj_tuple_it_t);
    o->base.type = &tuple_it_type;
    o->tuple = tuple;
    o->cur = cur;
    return o;
}
