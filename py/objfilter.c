#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_filter_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    mp_obj_t iter;
} mp_obj_filter_t;

static mp_obj_t filter_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    /* NOTE: args are backwards */
    if (n_args != 2) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "filter expected 2 arguments"));
    }
    assert(n_args == 2);
    mp_obj_filter_t *o = m_new_obj(mp_obj_filter_t);
    o->base.type = &filter_type;
    o->fun = args[1];
    o->iter = rt_getiter(args[0]);
    return o;
}

static mp_obj_t filter_getiter(mp_obj_t self_in) {
    return self_in;
}

static mp_obj_t filter_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &filter_type));
    mp_obj_filter_t *self = self_in;
    mp_obj_t next;
    while ((next = rt_iternext(self->iter)) != mp_const_stop_iteration) {
        mp_obj_t val;
        if (self->fun != mp_const_none) {
            val = rt_call_function_n(self->fun, 1, &next);
        } else {
            val = next;
        }
        if (rt_is_true(val)) {
            return next;
        }
    }
    return mp_const_stop_iteration;
}

const mp_obj_type_t filter_type = {
    { &mp_const_type },
    "filter",
    .make_new = filter_make_new,
    .getiter = filter_getiter,
    .iternext = filter_iternext,
};
