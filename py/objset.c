#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
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

static const mp_obj_type_t set_type = {
    { &mp_const_type },
    "set",
    set_print, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    { { NULL, NULL }, }, // method list
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
