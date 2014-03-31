#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"

STATIC mp_obj_t mp_obj_new_dict_iterator(mp_obj_dict_t *dict, int cur);
STATIC mp_map_elem_t *dict_it_iternext_elem(mp_obj_t self_in);

STATIC void dict_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_dict_t *self = self_in;
    bool first = true;
    print(env, "{");
    mp_obj_t *dict_iter = mp_obj_new_dict_iterator(self, 0);
    mp_map_elem_t *next = NULL;
    while ((next = dict_it_iternext_elem(dict_iter)) != NULL) {
        if (!first) {
            print(env, ", ");
        }
        first = false;
        mp_obj_print_helper(print, env, next->key, PRINT_REPR);
        print(env, ": ");
        mp_obj_print_helper(print, env, next->value, PRINT_REPR);
    }
    print(env, "}");
}

STATIC mp_obj_t dict_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO create from an iterable!
    return mp_obj_new_dict(0);
}

STATIC mp_obj_t dict_unary_op(int op, mp_obj_t self_in) {
    mp_obj_dict_t *self = self_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(self->map.used != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT((machine_int_t)self->map.used);
        default: return MP_OBJ_NULL; // op not supported for None
    }
}

STATIC mp_obj_t dict_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_dict_t *o = lhs_in;
    switch (op) {
        case MP_BINARY_OP_SUBSCR:
        {
            // dict load
            mp_map_elem_t *elem = mp_map_lookup(&o->map, rhs_in, MP_MAP_LOOKUP);
            if (elem == NULL) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "<value>"));
            } else {
                return elem->value;
            }
        }
        case MP_BINARY_OP_IN:
        {
            mp_map_elem_t *elem = mp_map_lookup(&o->map, rhs_in, MP_MAP_LOOKUP);
            return MP_BOOL(elem != NULL);
        }
        default:
            // op not supported
            return NULL;
    }
}


/******************************************************************************/
/* dict iterator                                                              */

typedef struct _mp_obj_dict_it_t {
    mp_obj_base_t base;
    mp_obj_dict_t *dict;
    machine_uint_t cur;
} mp_obj_dict_it_t;

STATIC mp_map_elem_t *dict_it_iternext_elem(mp_obj_t self_in) {
    mp_obj_dict_it_t *self = self_in;
    machine_uint_t max = self->dict->map.alloc;
    mp_map_elem_t *table = self->dict->map.table;

    for (int i = self->cur; i < max; i++) {
        if (table[i].key != NULL) {
            self->cur = i + 1;
            return &(table[i]);
        }
    }

    return NULL;
}

mp_obj_t dict_it_iternext(mp_obj_t self_in) {
    mp_map_elem_t *next = dict_it_iternext_elem(self_in);

    if (next != NULL) {
        return next->key;
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t mp_type_dict_it = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = dict_it_iternext,
};

STATIC mp_obj_t mp_obj_new_dict_iterator(mp_obj_dict_t *dict, int cur) {
    mp_obj_dict_it_t *o = m_new_obj(mp_obj_dict_it_t);
    o->base.type = &mp_type_dict_it;
    o->dict = dict;
    o->cur = cur;
    return o;
}

STATIC mp_obj_t dict_getiter(mp_obj_t o_in) {
    return mp_obj_new_dict_iterator(o_in, 0);
}

/******************************************************************************/
/* dict methods                                                               */

STATIC mp_obj_t dict_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;

    mp_map_clear(&self->map);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_clear_obj, dict_clear);

STATIC mp_obj_t dict_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    mp_obj_dict_t *other = mp_obj_new_dict(self->map.alloc);
    other->map.used = self->map.used;
    memcpy(other->map.table, self->map.table, self->map.alloc * sizeof(mp_map_elem_t));
    return other;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_copy_obj, dict_copy);

// this is a classmethod
STATIC mp_obj_t dict_fromkeys(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    mp_obj_t iter = mp_getiter(args[1]);
    mp_obj_t len = mp_obj_len_maybe(iter);
    mp_obj_t value = mp_const_none;
    mp_obj_t next = NULL;
    mp_obj_dict_t *self = NULL;

    if (n_args > 2) {
        value = args[2];
    }

    if (len == MP_OBJ_NULL) {
        /* object's type doesn't have a __len__ slot */
        self = mp_obj_new_dict(0);
    } else {
        self = mp_obj_new_dict(MP_OBJ_SMALL_INT_VALUE(len));
    }

    while ((next = mp_iternext(iter)) != MP_OBJ_NULL) {
        mp_map_lookup(&self->map, next, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    }

    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_fromkeys_fun_obj, 2, 3, dict_fromkeys);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(dict_fromkeys_obj, (const mp_obj_t)&dict_fromkeys_fun_obj);

STATIC mp_obj_t dict_get_helper(mp_map_t *self, mp_obj_t key, mp_obj_t deflt, mp_map_lookup_kind_t lookup_kind) {
    mp_map_elem_t *elem = mp_map_lookup(self, key, lookup_kind);
    mp_obj_t value;
    if (elem == NULL || elem->value == NULL) {
        if (deflt == NULL) {
            if (lookup_kind == MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "<value>"));
            } else {
                value = mp_const_none;
            }
        } else {
            value = deflt;
        }
    } else {
        value = elem->value;
        if (lookup_kind == MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
            // catch the leak (from mp_map_lookup)
            m_free(elem, sizeof(mp_map_elem_t));
        }
    }
    if (lookup_kind == MP_MAP_LOOKUP_ADD_IF_NOT_FOUND) {
        elem->value = value;
    }
    return value;
}

STATIC mp_obj_t dict_get(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_dict));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_get_obj, 2, 3, dict_get);

STATIC mp_obj_t dict_pop(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_dict));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_pop_obj, 2, 3, dict_pop);


STATIC mp_obj_t dict_setdefault(uint n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &mp_type_dict));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_setdefault_obj, 2, 3, dict_setdefault);


STATIC mp_obj_t dict_popitem(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    if (self->map.used == 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_KeyError, "popitem(): dictionary is empty"));
    }
    mp_obj_dict_it_t *iter = mp_obj_new_dict_iterator(self, 0);

    mp_map_elem_t *next = dict_it_iternext_elem(iter);
    self->map.used--;
    mp_obj_t items[] = {next->key, next->value};
    next->key = NULL;
    next->value = NULL;
    mp_obj_t tuple = mp_obj_new_tuple(2, items);

    return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_popitem_obj, dict_popitem);

STATIC mp_obj_t dict_update(mp_obj_t self_in, mp_obj_t iterable) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    /* TODO: check for the "keys" method */
    mp_obj_t iter = mp_getiter(iterable);
    mp_obj_t next = NULL;
    while ((next = mp_iternext(iter)) != MP_OBJ_NULL) {
        mp_obj_t inneriter = mp_getiter(next);
        mp_obj_t key = mp_iternext(inneriter);
        mp_obj_t value = mp_iternext(inneriter);
        mp_obj_t stop = mp_iternext(inneriter);
        if (key == MP_OBJ_NULL
            || value == MP_OBJ_NULL
            || stop != MP_OBJ_NULL) {
            nlr_jump(mp_obj_new_exception_msg(
                         &mp_type_ValueError,
                         "dictionary update sequence has the wrong length"));
        } else {
            mp_map_lookup(&self->map, key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(dict_update_obj, dict_update);


/******************************************************************************/
/* dict views                                                                 */

STATIC const mp_obj_type_t dict_view_type;
STATIC const mp_obj_type_t dict_view_it_type;

typedef enum _mp_dict_view_kind_t {
    MP_DICT_VIEW_ITEMS,
    MP_DICT_VIEW_KEYS,
    MP_DICT_VIEW_VALUES,
} mp_dict_view_kind_t;

STATIC char *mp_dict_view_names[] = {"dict_items", "dict_keys", "dict_values"};

typedef struct _mp_obj_dict_view_it_t {
    mp_obj_base_t base;
    mp_dict_view_kind_t kind;
    mp_obj_dict_it_t *iter;
    machine_uint_t cur;
} mp_obj_dict_view_it_t;

typedef struct _mp_obj_dict_view_t {
    mp_obj_base_t base;
    mp_obj_dict_t *dict;
    mp_dict_view_kind_t kind;
} mp_obj_dict_view_t;

STATIC mp_obj_t dict_view_it_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_view_it_type));
    mp_obj_dict_view_it_t *self = self_in;
    mp_map_elem_t *next = dict_it_iternext_elem(self->iter);

    if (next != NULL) {
        switch (self->kind) {
            case MP_DICT_VIEW_ITEMS:
            {
                mp_obj_t items[] = {next->key, next->value};
                return mp_obj_new_tuple(2, items);
            }
            case MP_DICT_VIEW_KEYS:
                return next->key;
            case MP_DICT_VIEW_VALUES:
                return next->value;
            default:
                assert(0);          /* can't happen */
                return mp_const_none;
        }
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t dict_view_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = dict_view_it_iternext,
};

STATIC mp_obj_t dict_view_getiter(mp_obj_t view_in) {
    assert(MP_OBJ_IS_TYPE(view_in, &dict_view_type));
    mp_obj_dict_view_t *view = view_in;
    mp_obj_dict_view_it_t *o = m_new_obj(mp_obj_dict_view_it_t);
    o->base.type = &dict_view_it_type;
    o->kind = view->kind;
    o->iter = mp_obj_new_dict_iterator(view->dict, 0);
    return o;
}

STATIC void dict_view_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_view_type));
    mp_obj_dict_view_t *self = self_in;
    bool first = true;
    print(env, mp_dict_view_names[self->kind]);
    print(env, "([");
    mp_obj_t *self_iter = dict_view_getiter(self);
    mp_obj_t *next = NULL;
    while ((next = dict_view_it_iternext(self_iter)) != MP_OBJ_NULL) {
        if (!first) {
            print(env, ", ");
        }
        first = false;
        mp_obj_print_helper(print, env, next, PRINT_REPR);
    }
    print(env, "])");
}

STATIC mp_obj_t dict_view_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    /* only supported for the 'keys' kind until sets and dicts are refactored */
    mp_obj_dict_view_t *o = lhs_in;
    if (o->kind != MP_DICT_VIEW_KEYS) return NULL;
    if (op != MP_BINARY_OP_IN) return NULL;
    return dict_binary_op(op, o->dict, rhs_in);
}


STATIC const mp_obj_type_t dict_view_type = {
    { &mp_type_type },
    .name = MP_QSTR_dict_view,
    .print = dict_view_print,
    .binary_op = dict_view_binary_op,
    .getiter = dict_view_getiter,
};

mp_obj_t mp_obj_new_dict_view(mp_obj_dict_t *dict, mp_dict_view_kind_t kind) {
    mp_obj_dict_view_t *o = m_new_obj(mp_obj_dict_view_t);
    o->base.type = &dict_view_type;
    o->dict = dict;
    o->kind = kind;
    return o;
}

STATIC mp_obj_t dict_view(mp_obj_t self_in, mp_dict_view_kind_t kind) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    return mp_obj_new_dict_view(self, kind);
}

STATIC mp_obj_t dict_items(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_ITEMS);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_items_obj, dict_items);

STATIC mp_obj_t dict_keys(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_KEYS);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_keys_obj, dict_keys);

STATIC mp_obj_t dict_values(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_VALUES);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(dict_values_obj, dict_values);

/******************************************************************************/
/* dict constructors & public C API                                           */

STATIC const mp_map_elem_t dict_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear), (mp_obj_t)&dict_clear_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_copy), (mp_obj_t)&dict_copy_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fromkeys), (mp_obj_t)&dict_fromkeys_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get), (mp_obj_t)&dict_get_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_items), (mp_obj_t)&dict_items_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_keys), (mp_obj_t)&dict_keys_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pop), (mp_obj_t)&dict_pop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_popitem), (mp_obj_t)&dict_popitem_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setdefault), (mp_obj_t)&dict_setdefault_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_update), (mp_obj_t)&dict_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_values), (mp_obj_t)&dict_values_obj },
};

STATIC MP_DEFINE_CONST_DICT(dict_locals_dict, dict_locals_dict_table);

const mp_obj_type_t mp_type_dict = {
    { &mp_type_type },
    .name = MP_QSTR_dict,
    .print = dict_print,
    .make_new = dict_make_new,
    .unary_op = dict_unary_op,
    .binary_op = dict_binary_op,
    .getiter = dict_getiter,
    .locals_dict = (mp_obj_t)&dict_locals_dict,
};

mp_obj_t mp_obj_new_dict(int n_args) {
    mp_obj_dict_t *o = m_new_obj(mp_obj_dict_t);
    o->base.type = &mp_type_dict;
    mp_map_init(&o->map, n_args);
    return o;
}

uint mp_obj_dict_len(mp_obj_t self_in) {
    return ((mp_obj_dict_t *)self_in)->map.used;
}

mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    mp_map_lookup(&self->map, key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    return self_in;
}

mp_map_t *mp_obj_dict_get_map(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_dict));
    mp_obj_dict_t *self = self_in;
    return &self->map;
}
