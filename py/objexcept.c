#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
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
    if (o->n_args == 0) {
        print(env, "%s", qstr_str(o->id));
    } else {
        print(env, "%s: ", qstr_str(o->id));
        print(env, "%V", &o->args);
    }
}

const mp_obj_type_t exception_type = {
    { &mp_const_type },
    "exception",
    .print = exception_print,
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

mp_obj_t mp_obj_new_exception_msg_varg(qstr id, const char *fmt, ...) {
    // count number of arguments by number of % signs, excluding %%
    int n_args = 1; // count fmt
    for (const char *s = fmt; *s; s++) {
        if (*s == '%') {
            if (s[1] == '%') {
                s += 1;
            } else {
                n_args += 1;
            }
        }
    }

    // make exception object
    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, void*, n_args);
    o->base.type = &exception_type;
    o->id = id;
    o->n_args = n_args;
    o->args[0] = fmt;

    // extract args and store them
    va_list ap;
    va_start(ap, fmt);
    for (int i = 1; i < n_args; i++) {
        o->args[i] = va_arg(ap, void*);
    }
    va_end(ap);

    return o;
}

qstr mp_obj_exception_get_type(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &exception_type));
    mp_obj_exception_t *self = self_in;
    return self->id;
}
