#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"

typedef struct mp_obj_exception_t {
    mp_obj_base_t base;
    qstr id;
    int n_args;
    const void *args[];
} mp_obj_exception_t;

void exception_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_exception_t *o = o_in;
    switch (o->n_args) {
        case 0:
            print(env, "%s", qstr_str(o->id));
            break;
        case 1:
            print(env, "%s: %s", qstr_str(o->id), (const char*)o->args[0]);
            break;
        case 2:
            print(env, "%s: ", qstr_str(o->id));
            print(env, (const char*)o->args[0], o->args[1]);
            break;
        default: // here we just assume at least 3 args, but only use first 3
            print(env, "%s: ", qstr_str(o->id));
            print(env, (const char*)o->args[0], o->args[1], o->args[2]);
            break;
    }
}

const mp_obj_type_t exception_type = {
    { &mp_const_type },
    "exception",
    exception_print, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};

mp_obj_t mp_obj_new_exception(qstr id) {
    mp_obj_exception_t *o = m_new_obj(mp_obj_exception_t);
    o->base.type = &exception_type;
    o->id = id;
    o->n_args = 0;
    return o;
}

mp_obj_t mp_obj_new_exception_msg(qstr id, const char *msg) {
    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, void*, 1);
    o->base.type = &exception_type;
    o->id = id;
    o->n_args = 1;
    o->args[0] = msg;
    return o;
}

mp_obj_t mp_obj_new_exception_msg_1_arg(qstr id, const char *fmt, const char *a1) {
    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, void*, 2);
    o->base.type = &exception_type;
    o->id = id;
    o->n_args = 2;
    o->args[0] = fmt;
    o->args[1] = a1;
    return o;
}

mp_obj_t mp_obj_new_exception_msg_2_args(qstr id, const char *fmt, const char *a1, const char *a2) {
    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, void*, 3);
    o->base.type = &exception_type;
    o->id = id;
    o->n_args = 3;
    o->args[0] = fmt;
    o->args[1] = a1;
    o->args[2] = a2;
    return o;
}
