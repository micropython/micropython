#include <stdlib.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"

typedef struct _mp_obj_none_t {
    mp_obj_base_t base;
} mp_obj_none_t;

STATIC void none_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "None");
}

STATIC mp_obj_t none_unary_op(int op, mp_obj_t o_in) {
    switch (op) {
        case RT_UNARY_OP_BOOL: return mp_const_false;
        default: return MP_OBJ_NULL; // op not supported for None
    }
}

const mp_obj_type_t none_type = {
    { &mp_type_type },
    .name = MP_QSTR_NoneType,
    .print = none_print,
    .unary_op = none_unary_op,
};

STATIC const mp_obj_none_t none_obj = {{&none_type}};
const mp_obj_t mp_const_none = (mp_obj_t)&none_obj;
