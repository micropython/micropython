#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "runtime0.h"
#include "map.h"

typedef struct _mp_obj_set_t {
    mp_obj_base_t base;
    mp_set_t set;
} mp_obj_set_t;

typedef struct _mp_obj_set_it_t {
    mp_obj_base_t base;
    mp_obj_set_t *set;
    machine_uint_t cur;
} mp_obj_set_it_t;

STATIC mp_obj_t set_it_iternext(mp_obj_t self_in);

STATIC void set_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_set_t *self = self_in;
    if (self->set.used == 0) {
        print(env, "set()");
        return;
    }
    bool first = true;
    print(env, "{");
    for (int i = 0; i < self->set.alloc; i++) {
        if (self->set.table[i] != MP_OBJ_NULL) {
            if (!first) {
                print(env, ", ");
            }
            first = false;
            mp_obj_print_helper(print, env, self->set.table[i], PRINT_REPR);
        }
    }
    print(env, "}");
}


STATIC mp_obj_t set_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0:
            // return a new, empty set
            return mp_obj_new_set(0, NULL);

        case 1:
        {
            // 1 argument, an iterable from which we make a new set
            mp_obj_t set = mp_obj_new_set(0, NULL);
            mp_obj_t iterable = rt_getiter(args[0]);
            mp_obj_t item;
            while ((item = rt_iternext(iterable)) != MP_OBJ_NULL) {
                mp_obj_set_store(set, item);
            }
            return set;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "set takes at most 1 argument, %d given", n_args));
    }
}

const mp_obj_type_t set_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .iternext = set_it_iternext,
};

STATIC mp_obj_t set_it_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_it_type));
    mp_obj_set_it_t *self = self_in;
    machine_uint_t max = self->set->set.alloc;
    mp_obj_t *table = self->set->set.table;

    for (machine_uint_t i = self->cur; i < max; i++) {
        if (table[i] != NULL) {
            self->cur = i + 1;
            return table[i];
        }
    }

    return MP_OBJ_NULL;
}

STATIC mp_obj_t set_getiter(mp_obj_t set_in) {
    mp_obj_set_it_t *o = m_new_obj(mp_obj_set_it_t);
    o->base.type = &set_it_type;
    o->set = (mp_obj_set_t *)set_in;
    o->cur = 0;
    return o;
}


/******************************************************************************/
/* set methods                                                                */

STATIC mp_obj_t set_add(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_add_obj, set_add);

STATIC mp_obj_t set_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    mp_set_clear(&self->set);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(set_clear_obj, set_clear);

STATIC mp_obj_t set_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    mp_obj_set_t *other = m_new_obj(mp_obj_set_t);
    other->base.type = &set_type;
    mp_set_init(&other->set, self->set.alloc - 1);
    other->set.used = self->set.used;
    memcpy(other->set.table, self->set.table, self->set.alloc * sizeof(mp_obj_t));

    return other;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(set_copy_obj, set_copy);

STATIC mp_obj_t set_discard(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_REMOVE_IF_FOUND);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_discard_obj, set_discard);

STATIC mp_obj_t set_diff_int(int n_args, const mp_obj_t *args, bool update) {
    assert(n_args > 0);
    assert(MP_OBJ_IS_TYPE(args[0], &set_type));
    mp_obj_set_t *self;
    if (update) {
        self = args[0];
    } else {
        self = set_copy(args[0]);
    }


    for (int i = 1; i < n_args; i++) {
        mp_obj_t other = args[i];
        if (self == other) {
            set_clear(self);
        } else {
            mp_obj_t iter = rt_getiter(other);
            mp_obj_t next;
            while ((next = rt_iternext(iter)) != MP_OBJ_NULL) {
                set_discard(self, next);
            }
        }
    }

    return self;
}

STATIC mp_obj_t set_diff(uint n_args, const mp_obj_t *args) {
    return set_diff_int(n_args, args, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(set_diff_obj, 1, set_diff);

STATIC mp_obj_t set_diff_update(uint n_args, const mp_obj_t *args) {
    set_diff_int(n_args, args, true);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(set_diff_update_obj, 1, set_diff_update);

STATIC mp_obj_t set_intersect_int(mp_obj_t self_in, mp_obj_t other, bool update) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    if (self_in == other) {
        return update ? mp_const_none : set_copy(self_in);
    }

    mp_obj_set_t *self = self_in;
    mp_obj_set_t *out = mp_obj_new_set(0, NULL);

    mp_obj_t iter = rt_getiter(other);
    mp_obj_t next;
    while ((next = rt_iternext(iter)) != MP_OBJ_NULL) {
        if (mp_set_lookup(&self->set, next, MP_MAP_LOOKUP)) {
            set_add(out, next);
        }
    }

    if (update) {
        m_del(mp_obj_t, self->set.table, self->set.alloc);
        self->set.alloc = out->set.alloc;
        self->set.used = out->set.used;
        self->set.table = out->set.table;
    }

    return update ? mp_const_none : out;
}

STATIC mp_obj_t set_intersect(mp_obj_t self_in, mp_obj_t other) {
    return set_intersect_int(self_in, other, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_intersect_obj, set_intersect);

STATIC mp_obj_t set_intersect_update(mp_obj_t self_in, mp_obj_t other) {
    return set_intersect_int(self_in, other, true);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_intersect_update_obj, set_intersect_update);

STATIC mp_obj_t set_isdisjoint(mp_obj_t self_in, mp_obj_t other) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    mp_obj_t iter = rt_getiter(other);
    mp_obj_t next;
    while ((next = rt_iternext(iter)) != MP_OBJ_NULL) {
        if (mp_set_lookup(&self->set, next, MP_MAP_LOOKUP)) {
            return mp_const_false;
        }
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_isdisjoint_obj, set_isdisjoint);

STATIC mp_obj_t set_issubset_internal(mp_obj_t self_in, mp_obj_t other_in, bool proper) {
    mp_obj_set_t *self;
    bool cleanup_self = false;
    if (MP_OBJ_IS_TYPE(self_in, &set_type)) {
        self = self_in;
    } else {
        self = set_make_new((mp_obj_t)&set_type, 1, 0, &self_in);
        cleanup_self = true;
    }

    mp_obj_set_t *other;
    bool cleanup_other = false;
    if (MP_OBJ_IS_TYPE(other_in, &set_type)) {
        other = other_in;
    } else {
        other = set_make_new((mp_obj_t)&set_type, 1, 0, &other_in);
        cleanup_other = true;
    }
    bool out = true;
    if (proper && self->set.used == other->set.used) {
        out = false;
    } else {
        mp_obj_t iter = set_getiter(self);
        mp_obj_t next;
        while ((next = set_it_iternext(iter)) != MP_OBJ_NULL) {
            if (!mp_set_lookup(&other->set, next, MP_MAP_LOOKUP)) {
                out = false;
                break;
            }
        }
    }
    if (cleanup_self) {
        set_clear(self);
    }
    if (cleanup_other) {
        set_clear(other);
    }
    return MP_BOOL(out);
}
STATIC mp_obj_t set_issubset(mp_obj_t self_in, mp_obj_t other_in) {
    return set_issubset_internal(self_in, other_in, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_issubset_obj, set_issubset);

STATIC mp_obj_t set_issubset_proper(mp_obj_t self_in, mp_obj_t other_in) {
    return set_issubset_internal(self_in, other_in, true);
}

STATIC mp_obj_t set_issuperset(mp_obj_t self_in, mp_obj_t other_in) {
    return set_issubset_internal(other_in, self_in, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_issuperset_obj, set_issuperset);

STATIC mp_obj_t set_issuperset_proper(mp_obj_t self_in, mp_obj_t other_in) {
    return set_issubset_internal(other_in, self_in, true);
}

STATIC mp_obj_t set_equal(mp_obj_t self_in, mp_obj_t other_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    if (!MP_OBJ_IS_TYPE(other_in, &set_type)) {
        return mp_const_false;
    }
    mp_obj_set_t *other = other_in;
    if (self->set.used != other->set.used) {
        return mp_const_false;
    }
    return set_issubset(self_in, other_in);
}

STATIC mp_obj_t set_pop(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    if (self->set.used == 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "pop from an empty set"));
    }
    mp_obj_t obj = mp_set_lookup(&self->set, NULL,
                         MP_MAP_LOOKUP_REMOVE_IF_FOUND | MP_MAP_LOOKUP_FIRST);
    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(set_pop_obj, set_pop);

STATIC mp_obj_t set_remove(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    if (mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_REMOVE_IF_FOUND) == MP_OBJ_NULL) {
        nlr_jump(mp_obj_new_exception(&mp_type_KeyError));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_remove_obj, set_remove);

STATIC mp_obj_t set_symmetric_difference_update(mp_obj_t self_in, mp_obj_t other_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_obj_t iter = rt_getiter(other_in);
    mp_obj_t next;
    while ((next = rt_iternext(iter)) != MP_OBJ_NULL) {
        mp_set_lookup(&self->set, next, MP_MAP_LOOKUP_REMOVE_IF_FOUND | MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_symmetric_difference_update_obj, set_symmetric_difference_update);

STATIC mp_obj_t set_symmetric_difference(mp_obj_t self_in, mp_obj_t other_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    self_in = set_copy(self_in);
    set_symmetric_difference_update(self_in, other_in);
    return self_in;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_symmetric_difference_obj, set_symmetric_difference);

STATIC void set_update_int(mp_obj_set_t *self, mp_obj_t other_in) {
    mp_obj_t iter = rt_getiter(other_in);
    mp_obj_t next;
    while ((next = rt_iternext(iter)) != MP_OBJ_NULL) {
        mp_set_lookup(&self->set, next, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    }
}

STATIC mp_obj_t set_update(uint n_args, const mp_obj_t *args) {
    assert(n_args > 0);
    assert(MP_OBJ_IS_TYPE(args[0], &set_type));

    for (int i = 1; i < n_args; i++) {
        set_update_int(args[0], args[i]);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(set_update_obj, 1, set_update);

STATIC mp_obj_t set_union(mp_obj_t self_in, mp_obj_t other_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = set_copy(self_in);
    set_update_int(self, other_in);
    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_union_obj, set_union);


STATIC mp_obj_t set_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_t args[] = {lhs, rhs};
    switch (op) {
    case RT_BINARY_OP_OR:
        return set_union(lhs, rhs);
    case RT_BINARY_OP_XOR:
        return set_symmetric_difference(lhs, rhs);
    case RT_BINARY_OP_AND:
        return set_intersect(lhs, rhs);
    case RT_BINARY_OP_SUBTRACT:
        return set_diff(2, args);
    case RT_BINARY_OP_INPLACE_OR:
        return set_union(lhs, rhs);
    case RT_BINARY_OP_INPLACE_XOR:
        return set_symmetric_difference(lhs, rhs);
    case RT_BINARY_OP_INPLACE_AND:
        return set_intersect(lhs, rhs);
    case RT_BINARY_OP_INPLACE_SUBTRACT:
        return set_diff(2, args);
    case RT_BINARY_OP_LESS:
        return set_issubset_proper(lhs, rhs);
    case RT_BINARY_OP_MORE:
        return set_issuperset_proper(lhs, rhs);
    case RT_BINARY_OP_EQUAL:
        return set_equal(lhs, rhs);
    case RT_BINARY_OP_LESS_EQUAL:
        return set_issubset(lhs, rhs);
    case RT_BINARY_OP_MORE_EQUAL:
        return set_issuperset(lhs, rhs);
    case RT_BINARY_OP_NOT_EQUAL:
        return MP_BOOL(set_equal(lhs, rhs) == mp_const_false);
    case RT_BINARY_OP_IN:
    {
        mp_obj_set_t *o = lhs;
        mp_obj_t elem = mp_set_lookup(&o->set, rhs, MP_MAP_LOOKUP);
        return MP_BOOL(elem != NULL);
    }
    default:
        // op not supported
        return NULL;
    }
}

/******************************************************************************/
/* set constructors & public C API                                            */


STATIC const mp_map_elem_t set_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_add), (mp_obj_t)&set_add_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&set_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_copy), (mp_obj_t)&set_copy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_discard), (mp_obj_t)&set_discard_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_difference), (mp_obj_t)&set_diff_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_difference_update), (mp_obj_t)&set_diff_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_intersection), (mp_obj_t)&set_intersect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_intersection_update), (mp_obj_t)&set_intersect_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isdisjoint), (mp_obj_t)&set_isdisjoint_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_issubset), (mp_obj_t)&set_issubset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_issuperset), (mp_obj_t)&set_issuperset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pop), (mp_obj_t)&set_pop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&set_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_symmetric_difference), (mp_obj_t)&set_symmetric_difference_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_symmetric_difference_update), (mp_obj_t)&set_symmetric_difference_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_union), (mp_obj_t)&set_union_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_update), (mp_obj_t)&set_update_obj },
};

STATIC MP_DEFINE_CONST_DICT(set_locals_dict, set_locals_dict_table);

const mp_obj_type_t set_type = {
    { &mp_type_type },
    .name = MP_QSTR_set,
    .print = set_print,
    .make_new = set_make_new,
    .binary_op = set_binary_op,
    .getiter = set_getiter,
    .locals_dict = (mp_obj_t)&set_locals_dict,
};

mp_obj_t mp_obj_new_set(int n_args, mp_obj_t *items) {
    mp_obj_set_t *o = m_new_obj(mp_obj_set_t);
    o->base.type = &set_type;
    mp_set_init(&o->set, n_args);
    for (int i = 0; i < n_args; i++) {
        mp_set_lookup(&o->set, items[i], MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    }
    return o;
}

void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
}
