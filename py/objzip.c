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

STATIC mp_obj_t zip_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    mp_obj_zip_t *o = m_new_obj_var(mp_obj_zip_t, mp_obj_t, n_args);
    o->base.type = &mp_type_zip;
    o->n_iters = n_args;
    for (int i = 0; i < n_args; i++) {
        o->iters[i] = mp_getiter(args[i]);
    }
    return o;
}

STATIC mp_obj_t zip_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_zip));
    mp_obj_zip_t *self = self_in;
    mp_obj_t *items;
    if (self->n_iters == 0) {
        return MP_OBJ_NULL;
    }
    mp_obj_t o = mp_obj_new_tuple(self->n_iters, NULL);
    mp_obj_tuple_get(o, NULL, &items);

    for (int i = 0; i < self->n_iters; i++) {
        mp_obj_t next = mp_iternext(self->iters[i]);
        if (next == MP_OBJ_NULL) {
            mp_obj_tuple_del(o);
            return MP_OBJ_NULL;
        }
        items[i] = next;
    }
    return o;
}

const mp_obj_type_t mp_type_zip = {
    { &mp_type_type },
    .name = MP_QSTR_zip,
    .make_new = zip_make_new,
    .getiter = mp_identity,
    .iternext = zip_iternext,
};
