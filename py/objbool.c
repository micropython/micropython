#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_bool_t {
    mp_obj_base_t base;
    bool value;
} mp_obj_bool_t;

static void bool_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_bool_t *self = self_in;
    if (self->value) {
        print(env, "True");
    } else {
        print(env, "False");
    }
}

// args are reverse in the array
static mp_obj_t bool_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0: return mp_const_false;
        case 1: if (rt_is_true(args[0])) { return mp_const_true; } else { return mp_const_false; }
        default: nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "bool takes at most 1 argument, %d given", (void*)(machine_int_t)n_args));
    }
}

const mp_obj_type_t bool_type = {
    .base = { &mp_const_type },
    .name = "bool",
    .print = bool_print,
    .make_new = bool_make_new,
    .methods = {{NULL, NULL},},
};

static const mp_obj_bool_t false_obj = {{&bool_type}, false};
static const mp_obj_bool_t true_obj = {{&bool_type}, true};

const mp_obj_t mp_const_false = (mp_obj_t)&false_obj;
const mp_obj_t mp_const_true = (mp_obj_t)&true_obj;
