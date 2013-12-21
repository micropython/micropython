#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"

void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    print(env, "type?");
}

const mp_obj_type_t mp_const_type = {
    { &mp_const_type },
    "type?",
    type_print, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};
