#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"
#include "map.h"

typedef struct _mp_obj_class_t {
    mp_obj_base_t base;
    mp_map_t *locals;
} mp_obj_class_t;

// args are in reverse order in the array
mp_obj_t class_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_class_t *self = self_in;

    // make instance
    mp_obj_t o = mp_obj_new_instance(self_in);

    // look for __init__ function
    mp_map_elem_t *init_fn = mp_qstr_map_lookup(self->locals, MP_QSTR___init__, MP_FALSE);

    if (init_fn != NULL) {
        // call __init__ function
        mp_obj_t init_ret;
        if (n_args == 0) {
            init_ret = rt_call_function_n(init_fn->value, 1, (mp_obj_t*)&o);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, n_args + 1);
            memcpy(args2, args, n_args * sizeof(mp_obj_t));
            args2[n_args] = o;
            init_ret = rt_call_function_n(init_fn->value, n_args + 1, args2);
            m_del(mp_obj_t, args2, n_args + 1);
        }
        if (init_ret != mp_const_none) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "__init__() should return None, not '%s'", mp_obj_get_type_str(init_ret)));
        }

    } else {
        // TODO
        if (n_args != 0) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "function takes 0 positional arguments but %d were given", (void*)(machine_int_t)n_args));
        }
    }

    return o;
}

mp_map_t *mp_obj_class_get_locals(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &class_type));
    mp_obj_class_t *self = self_in;
    return self->locals;
}

const mp_obj_type_t class_type = {
    { &mp_const_type },
    "class",
    NULL, // print
    NULL, // make_new
    class_call_n, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    .methods = NULL,
};

mp_obj_t mp_obj_new_class(mp_map_t *class_locals) {
    mp_obj_class_t *o = m_new_obj(mp_obj_class_t);
    o->base.type = &class_type;
    o->locals = class_locals;
    return o;
}
