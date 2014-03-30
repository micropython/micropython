#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_map_t {
    mp_obj_base_t base;
    machine_uint_t n_iters;
    mp_obj_t fun;
    mp_obj_t iters[];
} mp_obj_map_t;

STATIC mp_obj_t map_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args < 2 || n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "map must have at least 2 arguments and no keyword arguments"));
    }
    assert(n_args >= 2);
    mp_obj_map_t *o = m_new_obj_var(mp_obj_map_t, mp_obj_t, n_args - 1);
    o->base.type = &mp_type_map;
    o->n_iters = n_args - 1;
    o->fun = args[0];
    for (int i = 0; i < n_args - 1; i++) {
        o->iters[i] = mp_getiter(args[i + 1]);
    }
    return o;
}

STATIC mp_obj_t map_getiter(mp_obj_t self_in) {
    return self_in;
}

STATIC mp_obj_t map_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_map));
    mp_obj_map_t *self = self_in;
    mp_obj_t *nextses = m_new(mp_obj_t, self->n_iters);

    for (int i = 0; i < self->n_iters; i++) {
        mp_obj_t next = mp_iternext(self->iters[i]);
        if (next == MP_OBJ_NULL) {
            m_del(mp_obj_t, nextses, self->n_iters);
            return MP_OBJ_NULL;
        }
        nextses[i] = next;
    }
    return mp_call_function_n_kw(self->fun, self->n_iters, 0, nextses);
}

const mp_obj_type_t mp_type_map = {
    { &mp_type_type },
    .name = MP_QSTR_map,
    .make_new = map_make_new,
    .getiter = map_getiter,
    .iternext = map_iternext,
};
