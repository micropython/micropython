#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_bound_meth_t {
    mp_obj_base_t base;
    mp_obj_t meth;
    mp_obj_t self;
} mp_obj_bound_meth_t;

// args are in reverse order in the array
mp_obj_t bound_meth_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    mp_obj_bound_meth_t *self = self_in;

    if (n_args == 0) {
        return rt_call_function_n(self->meth, 1, &self->self);
    } else if (n_args == 1) {
        mp_obj_t args2[2];
        args2[1] = self->self;
        args2[0] = args[0];
        return rt_call_function_n(self->meth, 2, args2);
    } else {
        // TODO not implemented
        assert(0);
        return mp_const_none;
        //return rt_call_function_2(self->meth, n_args + 1, self->self + args);
    }
}

const mp_obj_type_t bound_meth_type = {
    { &mp_const_type },
    "bound_method",
    NULL, // print
    NULL, // make_new
    bound_meth_call_n, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    .methods = {{NULL, NULL},},
};

mp_obj_t mp_obj_new_bound_meth(mp_obj_t self, mp_obj_t meth) {
    mp_obj_bound_meth_t *o = m_new_obj(mp_obj_bound_meth_t);
    o->base.type = &bound_meth_type;
    o->meth = meth;
    o->self = self;
    return o;
}
