#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"

static void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_type_t *self = self_in;
    print(env, "<class '%s'>", self->name);
}

// args are reverse in the array
static mp_obj_t type_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 1:
            return mp_obj_get_type(args[0]);

        case 3:
        {
            // args[2] = name
            // args[1] = bases tuple
            // args[0] = locals dict

            mp_obj_type_t *new_type = m_new0(mp_obj_type_t, 1);
            new_type->base.type = &mp_const_type;
            new_type->name = qstr_str(mp_obj_get_qstr(args[2]));
            return new_type;

            //mp_obj_t new_class = mp_obj_new_class(mp_obj_get_qstr(args[2]), args[0]);
            //return new_class;
        }

        default:
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "type takes at 1 or 3 arguments"));
    }
}

static mp_obj_t type_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    mp_obj_type_t *self = self_in;
    if (self->make_new != NULL) {
        // TODO we need to init the object if it's an instance of a type
        return self->make_new(self, n_args, args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "cannot create '%s' instances", self->name));
    }
}

const mp_obj_type_t mp_const_type = {
    { &mp_const_type },
    "type",
    .print = type_print,
    .make_new = type_make_new,
    .call_n = type_call_n,
};
