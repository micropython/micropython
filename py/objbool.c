
#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"

typedef struct _mp_obj_bool_t {
    mp_obj_base_t base;
    bool value;
} mp_obj_bool_t;

STATIC void bool_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_bool_t *self = self_in;
    if (self->value) {
        print(env, "True");
    } else {
        print(env, "False");
    }
}

STATIC mp_obj_t bool_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 0: return mp_const_false;
        case 1: if (rt_is_true(args[0])) { return mp_const_true; } else { return mp_const_false; }
        default: nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "bool takes at most 1 argument, %d given", n_args));
    }
}

STATIC mp_obj_t bool_unary_op(int op, mp_obj_t o_in) {
    machine_int_t value = ((mp_obj_bool_t*)o_in)->value;
    switch (op) {
        case RT_UNARY_OP_BOOL: return o_in;
        case RT_UNARY_OP_POSITIVE: return MP_OBJ_NEW_SMALL_INT(value);
        case RT_UNARY_OP_NEGATIVE: return MP_OBJ_NEW_SMALL_INT(-value);
        case RT_UNARY_OP_INVERT:
        default: // no other cases
            return MP_OBJ_NEW_SMALL_INT(~value);
    }
}

const mp_obj_type_t bool_type = {
    { &mp_type_type },
    .name = MP_QSTR_bool,
    .print = bool_print,
    .make_new = bool_make_new,
    .unary_op = bool_unary_op,
};

STATIC const mp_obj_bool_t false_obj = {{&bool_type}, false};
STATIC const mp_obj_bool_t true_obj = {{&bool_type}, true};

const mp_obj_t mp_const_false = (mp_obj_t)&false_obj;
const mp_obj_t mp_const_true = (mp_obj_t)&true_obj;
