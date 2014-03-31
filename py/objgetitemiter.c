#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

// this is a wrapper object that is turns something that has a __getitem__ method into an iterator

typedef struct _mp_obj_getitem_iter_t {
    mp_obj_base_t base;
    mp_obj_t args[3];
} mp_obj_getitem_iter_t;

STATIC mp_obj_t it_iternext(mp_obj_t self_in) {
    mp_obj_getitem_iter_t *self = self_in;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // try to get next item
        mp_obj_t value = mp_call_method_n_kw(1, 0, self->args);
        self->args[2] = MP_OBJ_NEW_SMALL_INT(MP_OBJ_SMALL_INT_VALUE(self->args[2]) + 1);
        nlr_pop();
        return value;
    } else {
        // an exception was raised
        if (mp_obj_get_type(nlr.ret_val) == &mp_type_StopIteration) {
            // return MP_OBJ_NULL instead of raising StopIteration
            return MP_OBJ_NULL;
        } else {
            // re-raise exception
            nlr_jump(nlr.ret_val);
        }
    }
}

STATIC const mp_obj_type_t it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = it_iternext
};

// args are those returned from mp_load_method_maybe (ie either an attribute or a method)
mp_obj_t mp_obj_new_getitem_iter(mp_obj_t *args) {
    mp_obj_getitem_iter_t *o = m_new_obj(mp_obj_getitem_iter_t);
    o->base.type = &it_type;
    o->args[0] = args[0];
    o->args[1] = args[1];
    o->args[2] = MP_OBJ_NEW_SMALL_INT(0);
    return o;
}
