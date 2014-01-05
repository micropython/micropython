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
    .call_n = type_call_n,
    .methods = {{NULL, NULL},},
};
