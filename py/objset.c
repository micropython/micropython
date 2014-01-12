#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"
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

static mp_obj_t set_it_iternext(mp_obj_t self_in);

void set_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_set_t *self = self_in;
    bool first = true;
    print(env, "{");
    for (int i = 0; i < self->set.alloc; i++) {
        if (self->set.table[i] != MP_OBJ_NULL) {
            if (!first) {
                print(env, ", ");
            }
            first = false;
            mp_obj_print_helper(print, env, self->set.table[i]);
        }
    }
    print(env, "}");
}

static mp_obj_t set_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
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
            while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
                mp_obj_set_store(set, item);
            }
            return set;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "set takes at most 1 argument, %d given", (void*)(machine_int_t)n_args));
    }
}

const mp_obj_type_t set_it_type = {
    { &mp_const_type },
    "set_iterator",
    .iternext = set_it_iternext,
};

static mp_obj_t set_it_iternext(mp_obj_t self_in) {
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

    return mp_const_stop_iteration;
}

static mp_obj_t set_getiter(mp_obj_t set_in) {
    mp_obj_set_it_t *o = m_new_obj(mp_obj_set_it_t);
    o->base.type = &set_it_type;
    o->set = (mp_obj_set_t *)set_in;
    o->cur = 0;
    return o;
}


/******************************************************************************/
/* set methods                                                                */

static mp_obj_t set_add(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(set_add_obj, set_add);

static mp_obj_t set_clear(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    mp_set_clear(&self->set);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_clear_obj, set_clear);

static mp_obj_t set_copy(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;

    mp_obj_set_t *other = m_new_obj(mp_obj_set_t);
    other->base.type = &set_type;
    mp_set_init(&other->set, self->set.alloc);
    other->set.used = self->set.used;
    memcpy(other->set.table, self->set.table, self->set.alloc * sizeof(mp_obj_t));

    return other;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_copy_obj, set_copy);

static mp_obj_t set_discard(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, MP_MAP_LOOKUP_REMOVE_IF_FOUND);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(set_discard_obj, set_discard);

static mp_obj_t set_diff_int(int n_args, const mp_obj_t *args, bool update) {
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
            while ((next = rt_iternext(iter)) != mp_const_stop_iteration) {
                set_discard(self, next);
            }
        }
    }

    return self;
}

static mp_obj_t set_diff(int n_args, const mp_obj_t *args) {
    return set_diff_int(n_args, args, false);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(set_diff_obj, 1, set_diff);

static mp_obj_t set_diff_update(int n_args, const mp_obj_t *args) {
    set_diff_int(n_args, args, true);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(set_diff_update_obj, 1, set_diff_update);


/******************************************************************************/
/* set constructors & public C API                                            */


static const mp_method_t set_type_methods[] = {
    { "add", &set_add_obj },
    { "clear", &set_clear_obj },
    { "copy", &set_copy_obj },
    { "discard", &set_discard_obj },
    { "difference", &set_diff_obj },
    { "difference_update", &set_diff_update_obj },
    { NULL, NULL }, // end-of-list sentinel
};

const mp_obj_type_t set_type = {
    { &mp_const_type },
    "set",
    .print = set_print,
    .make_new = set_make_new,
    .getiter = set_getiter,
    .methods = set_type_methods,
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
