#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"

typedef struct _mp_obj_object_t {
    mp_obj_base_t base;
} mp_obj_object_t;

const mp_obj_type_t mp_type_object;

STATIC mp_obj_t object_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args != 0 || n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "object takes no arguments"));
    }

    mp_obj_object_t *o = m_new_obj(mp_obj_object_t);
    o->base.type = &mp_type_object;
    return o;
}

const mp_obj_type_t mp_type_object = {
    { &mp_type_type },
    .name = MP_QSTR_object,
    .make_new = object_make_new,
};
