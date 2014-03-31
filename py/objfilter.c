#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_filter_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    mp_obj_t iter;
} mp_obj_filter_t;

STATIC mp_obj_t filter_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args != 2 || n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "filter expected 2 arguments"));
    }
    assert(n_args == 2);
    mp_obj_filter_t *o = m_new_obj(mp_obj_filter_t);
    o->base.type = &mp_type_filter;
    o->fun = args[0];
    o->iter = mp_getiter(args[1]);
    return o;
}

STATIC mp_obj_t filter_iternext(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_filter));
    mp_obj_filter_t *self = self_in;
    mp_obj_t next;
    while ((next = mp_iternext(self->iter)) != MP_OBJ_NULL) {
        mp_obj_t val;
        if (self->fun != mp_const_none) {
            val = mp_call_function_n_kw(self->fun, 1, 0, &next);
        } else {
            val = next;
        }
        if (mp_obj_is_true(val)) {
            return next;
        }
    }
    return MP_OBJ_NULL;
}

const mp_obj_type_t mp_type_filter = {
    { &mp_type_type },
    .name = MP_QSTR_filter,
    .make_new = filter_make_new,
    .getiter = mp_identity,
    .iternext = filter_iternext,
};
