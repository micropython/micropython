#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"

typedef struct _mp_obj_none_t {
    mp_obj_base_t base;
} mp_obj_none_t;

static void none_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "None");
}

static mp_obj_t none_unary_op(int op, mp_obj_t o_in) {
    switch (op) {
        case RT_UNARY_OP_NOT: return mp_const_true;
        default: return MP_OBJ_NULL; // op not supported for None
    }
}

const mp_obj_type_t none_type = {
    { &mp_const_type },
    "NoneType",
    .print = none_print,
    .unary_op = none_unary_op,
};

static const mp_obj_none_t none_obj = {{&none_type}};
const mp_obj_t mp_const_none = (mp_obj_t)&none_obj;

// the stop-iteration object just needs to be something unique
// it's not the StopIteration exception
static const mp_obj_none_t stop_it_obj = {{&none_type}};
const mp_obj_t mp_const_stop_iteration = (mp_obj_t)&stop_it_obj;
