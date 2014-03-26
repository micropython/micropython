#include <stdlib.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_enumerate_t {
    mp_obj_base_t base;
    mp_obj_t iter;
    machine_int_t cur;
} mp_obj_enumerate_t;

STATIC mp_obj_t enumerate_iternext(mp_obj_t self_in);

/* TODO: enumerate is one of the ones that can take args or kwargs.
   Sticking to args for now */
STATIC mp_obj_t enumerate_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    assert(n_args > 0);
    mp_obj_enumerate_t *o = m_new_obj(mp_obj_enumerate_t);
    o->base.type = &enumerate_type;
    o->iter = rt_getiter(args[0]);
    o->cur = n_args > 1 ? mp_obj_get_int(args[1]) : 0;
    return o;
}

const mp_obj_type_t enumerate_type = {
    { &mp_type_type },
    .name = MP_QSTR_enumerate,
    .make_new = enumerate_make_new,
    .iternext = enumerate_iternext,
    .getiter = mp_identity,
};

STATIC mp_obj_t enumerate_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &enumerate_type));
    mp_obj_enumerate_t *self = self_in;
    mp_obj_t next = rt_iternext(self->iter);
    if (next == MP_OBJ_NULL) {
        return MP_OBJ_NULL;
    } else {
        mp_obj_t items[] = {MP_OBJ_NEW_SMALL_INT(self->cur++), next};
        return mp_obj_new_tuple(2, items);
    }
}
