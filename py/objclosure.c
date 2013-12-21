#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime.h"

typedef struct _mp_obj_closure_t {
    mp_obj_base_t base;
    mp_obj_t fun;
    mp_obj_t vars;
} mp_obj_closure_t;

const mp_obj_type_t closure_type = {
    { &mp_const_type },
    "closure",
    NULL, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};

mp_obj_t mp_obj_new_closure(mp_obj_t fun, mp_obj_t closure_tuple) {
    mp_obj_closure_t *o = m_new_obj(mp_obj_closure_t);
    o->base.type = &closure_type;
    o->fun = fun;
    o->vars = closure_tuple;
    return o;
}
