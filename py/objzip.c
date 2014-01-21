#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_zip_t {
    mp_obj_base_t base;
    int n_iters;
    mp_obj_t iters[];
} mp_obj_zip_t;

static mp_obj_t zip_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    mp_obj_zip_t *o = m_new_obj_var(mp_obj_zip_t, mp_obj_t, n_args);
    o->base.type = &zip_type;
    o->n_iters = n_args;
    for (int i = 0; i < n_args; i++) {
        o->iters[i] = rt_getiter(args[i]);
    }
    return o;
}

static mp_obj_t zip_getiter(mp_obj_t self_in) {
    return self_in;
}

static mp_obj_t zip_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &zip_type));
    mp_obj_zip_t *self = self_in;
    mp_obj_t *items;
    if (self->n_iters == 0) {
        return mp_const_stop_iteration;
    }
    mp_obj_t o = mp_obj_new_tuple(self->n_iters, NULL);
    mp_obj_tuple_get(o, NULL, &items);

    for (int i = 0; i < self->n_iters; i++) {
        mp_obj_t next = rt_iternext(self->iters[i]);
        if (next == mp_const_stop_iteration) {
            mp_obj_tuple_del(o);
            return mp_const_stop_iteration;
        }
        items[i] = next;
    }
    return o;
}

const mp_obj_type_t zip_type = {
    { &mp_const_type },
    "zip",
    .make_new = zip_make_new,
    .getiter = zip_getiter,
    .iternext = zip_iternext,
};
