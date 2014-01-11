#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"
#include "runtime0.h"
#include "map.h"

typedef struct _mp_obj_set_t {
    mp_obj_base_t base;
    mp_set_t set;
} mp_obj_set_t;

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


static mp_obj_t set_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_set_t *o = lhs_in;
    switch (op) {
        case RT_COMPARE_OP_IN:
        case RT_COMPARE_OP_NOT_IN:
        {
            mp_obj_t elem = mp_set_lookup(&o->set, rhs_in, false);
            return ((op == RT_COMPARE_OP_IN) ^ (elem == NULL))
                ? mp_const_true : mp_const_false;
        }
        default:
            // op not supported
            return NULL;
    }
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

const mp_obj_type_t set_type = {
    { &mp_const_type },
    "set",
    .print = set_print,
    .binary_op = set_binary_op,
    .make_new = set_make_new,
};

mp_obj_t mp_obj_new_set(int n_args, mp_obj_t *items) {
    mp_obj_set_t *o = m_new_obj(mp_obj_set_t);
    o->base.type = &set_type;
    mp_set_init(&o->set, n_args);
    for (int i = 0; i < n_args; i++) {
        mp_set_lookup(&o->set, items[i], true);
    }
    return o;
}

void mp_obj_set_store(mp_obj_t self_in, mp_obj_t item) {
    assert(MP_OBJ_IS_TYPE(self_in, &set_type));
    mp_obj_set_t *self = self_in;
    mp_set_lookup(&self->set, item, true);
}
