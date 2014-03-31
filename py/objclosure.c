#include <string.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_closure_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    uint n_closed;
    mp_obj_t *closed;
} mp_obj_closure_t;

mp_obj_t closure_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_closure_t *self = self_in;

    // need to concatenate closed-over-vars and args

    int n_total = self->n_closed + n_args + 2 * n_kw;
    if (n_total <= 5) {
        // use stack to allocate temporary args array
        mp_obj_t args2[5];
        memcpy(args2, self->closed, self->n_closed * sizeof(mp_obj_t));
        memcpy(args2 + self->n_closed, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
        return mp_call_function_n_kw(self->fun, self->n_closed + n_args, n_kw, args2);
    } else {
        // use heap to allocate temporary args array
        mp_obj_t *args2 = m_new(mp_obj_t, n_total);
        memcpy(args2, self->closed, self->n_closed * sizeof(mp_obj_t));
        memcpy(args2 + self->n_closed, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
        mp_obj_t res = mp_call_function_n_kw(self->fun, self->n_closed + n_args, n_kw, args2);
        m_del(mp_obj_t, args2, n_total);
        return res;
    }
}

const mp_obj_type_t closure_type = {
    { &mp_type_type },
    .name = MP_QSTR_closure,
    .call = closure_call,
};

mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple) {
    mp_obj_closure_t *o = m_new_obj(mp_obj_closure_t);
    o->base.type = &closure_type;
    o->fun = fun;
    mp_obj_tuple_get(closure_tuple, &o->n_closed, &o->closed);
    return o;
}
