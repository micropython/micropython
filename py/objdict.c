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

static void dict_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_dict_t *self = self_in;
    bool first = true;
    print(env, "{");
    for (int i = 0; i < self->map.alloc; i++) {
        if (self->map.table[i].key != NULL) {
            if (!first) {
                print(env, ", ");
            }
            first = false;
            mp_obj_print_helper(print, env, self->map.table[i].key);
            print(env, ": ");
            mp_obj_print_helper(print, env, self->map.table[i].value);
        }
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
            mp_map_elem_t *elem = mp_map_lookup_helper(&o->map, rhs_in, false);
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

const mp_obj_type_t dict_type = {
    { &mp_const_type },
    "dict",
    .print = dict_print,
    .make_new = dict_make_new,
    .binary_op = dict_binary_op,
};

mp_obj_t mp_obj_new_dict(int n_args) {
    mp_obj_dict_t *o = m_new_obj(mp_obj_dict_t);
    o->base.type = &dict_type;
    mp_map_init(&o->map, MP_MAP_OBJ, n_args);
    return o;
}

uint mp_obj_dict_len(mp_obj_t self_in) {
    mp_obj_dict_t *self = self_in;
    uint len = 0;
    for (int i = 0; i < self->map.alloc; i++) {
        if (self->map.table[i].key != NULL) {
            len += 1;
        }
    }
    return len;
}

mp_obj_t mp_obj_dict_store(mp_obj_t self_in, mp_obj_t key, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &dict_type));
    mp_obj_dict_t *self = self_in;
    mp_map_lookup_helper(&self->map, key, true)->value = value;
    return self_in;
}
