#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_closure_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    uint n_closed;
    mp_obj_t *closed;
} mp_obj_closure_t;

// args are in reverse order in the array
mp_obj_t closure_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    mp_obj_closure_t *self = self_in;

    // concatenate args and closed-over-vars, in reverse order
    // TODO perhaps cache this array so we don't need to create it each time we are called
    mp_obj_t *args2 = m_new(mp_obj_t, self->n_closed + n_args);
    memcpy(args2, args, n_args * sizeof(mp_obj_t));
    for (int i = 0; i < self->n_closed; i++) {
        args2[n_args + i] = self->closed[self->n_closed - 1 - i];
    }

    // call the function with the new vars array
    return rt_call_function_n(self->fun, n_args + self->n_closed, args2);
}

const mp_obj_type_t closure_type = {
    { &mp_const_type },
    "closure",
    NULL, // print
    NULL, // make_new
    closure_call_n, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};

mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple) {
    mp_obj_closure_t *o = m_new_obj(mp_obj_closure_t);
    o->base.type = &closure_type;
    o->fun = fun;
    mp_obj_tuple_get(closure_tuple, &o->n_closed, &o->closed);
    return o;
}
