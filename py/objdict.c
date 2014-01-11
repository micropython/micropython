#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "map.h"

typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;

static mp_obj_t mp_obj_new_dict_iterator(mp_obj_dict_t *dict, int cur);
static mp_map_elem_t *dict_it_iternext_elem(mp_obj_t self_in);

static void dict_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
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
        mp_obj_print_helper(print, env, next->key);
        print(env, ": ");
        mp_obj_print_helper(print, env, next->value);
    }
    print(env, "}");
}

// args are reverse in the array
static mp_obj_t dict_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    // TODO create from an iterable!
    return rt_build_map(0);
}

static mp_obj_t dict_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_dict_t *o = lhs_in;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
            // dict load
            mp_map_elem_t *elem = mp_map_lookup(&o->map, rhs_in, MP_MAP_LOOKUP);
            if (elem == NULL) {
                nlr_jump(mp_obj_new_exception_msg(MP_QSTR_KeyError, "<value>"));
            } else {
                return elem->value;
            }
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

static mp_map_elem_t *dict_it_iternext_elem(mp_obj_t self_in) {
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
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t dict_it_type = {
    { &mp_const_type },
    "dict_iterator",
    .iternext = dict_it_iternext,
};

static mp_obj_t mp_obj_new_dict_iterator(mp_obj_dict_t *dict, int cur) {
    mp_obj_dict_it_t *o = m_new_obj(mp_obj_dict_it_t);
    o->base.type = &dict_it_type;
    o->dict = dict;
    o->cur = cur;
    return o;
}

static mp_obj_t dict_getiter(mp_obj_t o_in) {
    return mp_obj_new_dict_iterator(o_in, 0);
}

/******************************************************************************/
/* dict methods                                                               */

static mp_obj_t dict_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;

    mp_map_clear(&self->map);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(dict_clear_obj, dict_clear);

static mp_obj_t dict_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    mp_obj_dict_t *other = mp_obj_new_dict(self->map.alloc);
    other->map.used = self->map.used;
    memcpy(other->map.table, self->map.table, self->map.alloc * sizeof(mp_map_elem_t));
    return other;
}
static MP_DEFINE_CONST_FUN_OBJ_1(dict_copy_obj, dict_copy);

// this is a classmethod
static mp_obj_t dict_fromkeys(int n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    mp_obj_t iter = rt_getiter(args[1]);
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

    while ((next = rt_iternext(iter)) != mp_const_stop_iteration) {
        mp_map_lookup(&self->map, next, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    }

    return self;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_fromkeys_fun_obj, 2, 3, dict_fromkeys);
static MP_DEFINE_CONST_CLASSMETHOD_OBJ(dict_fromkeys_obj, (const mp_obj_t)&dict_fromkeys_fun_obj);

static mp_obj_t dict_get_helper(mp_map_t *self, mp_obj_t key, mp_obj_t deflt, mp_map_lookup_kind_t lookup_kind) {
    mp_map_elem_t *elem = mp_map_lookup(self, key, lookup_kind);
    mp_obj_t value;
    if (elem == NULL || elem->value == NULL) {
        if (deflt == NULL) {
            if (lookup_kind == MP_MAP_LOOKUP_REMOVE_IF_FOUND) {
                nlr_jump(mp_obj_new_exception_msg(MP_QSTR_KeyError, "<value>"));
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

static mp_obj_t dict_get(int n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &dict_type));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_get_obj, 2, 3, dict_get);

static mp_obj_t dict_pop(int n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &dict_type));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_pop_obj, 2, 3, dict_pop);


static mp_obj_t dict_setdefault(int n_args, const mp_obj_t *args) {
    assert(2 <= n_args && n_args <= 3);
    assert(MP_OBJ_IS_TYPE(args[0], &dict_type));

    return dict_get_helper(&((mp_obj_dict_t *)args[0])->map,
                           args[1],
                           n_args == 3 ? args[2] : NULL,
                           MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(dict_setdefault_obj, 2, 3, dict_setdefault);


static mp_obj_t dict_popitem(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    if (self->map.used == 0) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_KeyError, "popitem(): dictionary is empty"));
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
static MP_DEFINE_CONST_FUN_OBJ_1(dict_popitem_obj, dict_popitem);

static mp_obj_t dict_update(mp_obj_t self_in, mp_obj_t iterable) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    /* TODO: check for the "keys" method */
    mp_obj_t iter = rt_getiter(iterable);
    mp_obj_t next = NULL;
    while ((next = rt_iternext(iter)) != mp_const_stop_iteration) {
        mp_obj_t inneriter = rt_getiter(next);
        mp_obj_t key = rt_iternext(inneriter);
        mp_obj_t value = rt_iternext(inneriter);
        mp_obj_t stop = rt_iternext(inneriter);
        if (key == mp_const_stop_iteration
            || value == mp_const_stop_iteration
            || stop != mp_const_stop_iteration) {
            nlr_jump(mp_obj_new_exception_msg(
                         MP_QSTR_ValueError,
                         "dictionary update sequence has the wrong length"));
        } else {
            mp_map_lookup(&self->map, key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(dict_update_obj, dict_update);


/******************************************************************************/
/* dict views                                                                 */

static const mp_obj_type_t dict_view_type;
static const mp_obj_type_t dict_view_it_type;

typedef enum _mp_dict_view_kind_t {
    MP_DICT_VIEW_ITEMS,
    MP_DICT_VIEW_KEYS,
    MP_DICT_VIEW_VALUES,
} mp_dict_view_kind_t;

static char *mp_dict_view_names[] = {"dict_items", "dict_keys", "dict_values"};

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

static mp_obj_t dict_view_it_iternext(mp_obj_t self_in) {
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
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t dict_view_it_type = {
    { &mp_const_type },
    "dict_view_iterator",
    .iternext = dict_view_it_iternext,
    .methods = NULL,            /* set operations still to come */
};

static mp_obj_t dict_view_getiter(mp_obj_t view_in) {
    assert(MP_OBJ_IS_TYPE(view_in, &dict_view_type));
    mp_obj_dict_view_t *view = view_in;
    mp_obj_dict_view_it_t *o = m_new_obj(mp_obj_dict_view_it_t);
    o->base.type = &dict_view_it_type;
    o->kind = view->kind;
    o->iter = mp_obj_new_dict_iterator(view->dict, 0);
    return o;
}

static void dict_view_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_view_type));
    mp_obj_dict_view_t *self = self_in;
    bool first = true;
    print(env, mp_dict_view_names[self->kind]);
    print(env, "([");
    mp_obj_t *self_iter = dict_view_getiter(self);
    mp_obj_t *next = NULL;
    while ((next = dict_view_it_iternext(self_iter)) != mp_const_stop_iteration) {
        if (!first) {
            print(env, ", ");
        }
        first = false;
        mp_obj_print_helper(print, env, next);
    }
    print(env, "])");
}

static const mp_obj_type_t dict_view_type = {
    { &mp_const_type },
    "dict_view",
    .print = dict_view_print,
    .getiter = dict_view_getiter,
};

mp_obj_t mp_obj_new_dict_view(mp_obj_dict_t *dict, mp_dict_view_kind_t kind) {
    mp_obj_dict_view_t *o = m_new_obj(mp_obj_dict_view_t);
    o->base.type = &dict_view_type;
    o->dict = dict;
    o->kind = kind;
    return o;
}

static mp_obj_t dict_view(mp_obj_t self_in, mp_dict_view_kind_t kind) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    return mp_obj_new_dict_view(self, kind);
}

static mp_obj_t dict_items(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_ITEMS);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dict_items_obj, dict_items);

static mp_obj_t dict_keys(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_KEYS);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dict_keys_obj, dict_keys);

static mp_obj_t dict_values(mp_obj_t self_in) {
    return dict_view(self_in, MP_DICT_VIEW_VALUES);
}
static MP_DEFINE_CONST_FUN_OBJ_1(dict_values_obj, dict_values);

/******************************************************************************/
/* dict constructors & public C API                                           */

static const mp_method_t dict_type_methods[] = {
    { "clear", &dict_clear_obj },
    { "copy", &dict_copy_obj },
    { "fromkeys", &dict_fromkeys_obj },
    { "get", &dict_get_obj },
    { "items", &dict_items_obj },
    { "keys", &dict_keys_obj },
    { "pop", &dict_pop_obj },
    { "popitem", &dict_popitem_obj },
    { "setdefault", &dict_setdefault_obj },
    { "update", &dict_update_obj },
    { "values", &dict_values_obj },
    { NULL, NULL }, // end-of-list sentinel
};

const mp_obj_type_t dict_type = {
    { &mp_const_type },
    "dict",
    .print = dict_print,
    .make_new = dict_make_new,
    .binary_op = dict_binary_op,
    .getiter = dict_getiter,
    .methods = dict_type_methods,
};

mp_obj_t mp_obj_new_dict(int n_args) {
    mp_obj_dict_t *o = m_new_obj(mp_obj_dict_t);
    o->base.type = &dict_type;
    mp_map_init(&o->map, n_args);
    return o;
}

uint mp_obj_dict_len(mp_obj_t self_in) {
    return ((mp_obj_dict_t *)self_in)->map.used;
}

mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    mp_map_lookup(&self->map, key, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    return self_in;
}

mp_map_t *mp_obj_dict_get_map(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    return &self->map;
}
