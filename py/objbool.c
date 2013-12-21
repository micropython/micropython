#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"

typedef struct _mp_obj_bool_t {
    mp_obj_base_t base;
    bool value;
} mp_obj_bool_t;

void bool_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_bool_t *self = self_in;
    if (self->value) {
        print(env, "True");
    } else {
        print(env, "False");
    }
}

const mp_obj_type_t bool_type = {
    { &mp_const_type },
    "bool",
    bool_print, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};

static const mp_obj_bool_t false_obj = {{&bool_type}, false};
static const mp_obj_bool_t true_obj = {{&bool_type}, true};

const mp_obj_t mp_const_false = (mp_obj_t)&false_obj;
const mp_obj_t mp_const_true = (mp_obj_t)&true_obj;
