#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"

// This is unified class for C-level and Python-level exceptions
// Python-level exception have empty ->msg and all arguments are in
// args tuple. C-level excepttion likely have ->msg and args is empty.
typedef struct mp_obj_exception_t {
    mp_obj_base_t base;
    mp_obj_t traceback; // a list object, holding (file,line,block) as numbers (not Python objects); a hack for now
    qstr id;
    vstr_t *msg;
    mp_obj_tuple_t args;
} mp_obj_exception_t;

void exception_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_exception_t *o = o_in;
    if (o->msg != NULL) {
        print(env, "%s: %s", qstr_str(o->id), vstr_str(o->msg));
    } else {
        // Yes, that's how CPython has it
        if (kind == PRINT_REPR) {
            print(env, "%s", qstr_str(o->id));
        }
        if (kind == PRINT_STR) {
            if (o->args.len == 0) {
                print(env, "");
                return;
            } else if (o->args.len == 1) {
                mp_obj_print_helper(print, env, o->args.items[0], PRINT_STR);
                return;
            }
        }
        tuple_print(print, env, &o->args, kind);
    }
}

static mp_obj_t exception_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_exception_t *base = self_in;

    if (n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "%s does not take keyword arguments", qstr_str(base->id)));
    }

    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, mp_obj_t, n_args);
    o->base.type = &exception_type;
    o->id = base->id;
    o->msg = NULL;
    o->args.len = n_args;
    memcpy(o->args.items, args, n_args * sizeof(mp_obj_t));
    return o;
}

const mp_obj_type_t exception_type = {
    { &mp_const_type },
    "exception",
    .print = exception_print,
    .call = exception_call,
};

mp_obj_t mp_obj_new_exception(qstr id) {
    return mp_obj_new_exception_msg_varg(id, NULL);
}

mp_obj_t mp_obj_new_exception_msg(qstr id, const char *msg) {
    return mp_obj_new_exception_msg_varg(id, msg);
}

mp_obj_t mp_obj_new_exception_msg_1_arg(qstr id, const char *fmt, const char *a1) {
    return mp_obj_new_exception_msg_varg(id, fmt, a1);
}

mp_obj_t mp_obj_new_exception_msg_2_args(qstr id, const char *fmt, const char *a1, const char *a2) {
    return mp_obj_new_exception_msg_varg(id, fmt, a1, a2);
}

mp_obj_t mp_obj_new_exception_msg_varg(qstr id, const char *fmt, ...) {
    // make exception object
    mp_obj_exception_t *o = m_new_obj_var(mp_obj_exception_t, mp_obj_t*, 0);
    o->base.type = &exception_type;
    o->traceback = MP_OBJ_NULL;
    o->id = id;
    o->args.len = 0;
    if (fmt == NULL) {
        o->msg = NULL;
    } else {
        // render exception message
        o->msg = vstr_new();
        va_list ap;
        va_start(ap, fmt);
        vstr_vprintf(o->msg, fmt, ap);
        va_end(ap);
    }

    return o;
}

qstr mp_obj_exception_get_type(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &exception_type));
    mp_obj_exception_t *self = self_in;
    return self->id;
}

void mp_obj_exception_add_traceback(mp_obj_t self_in, qstr file, machine_uint_t line, qstr block) {
    assert(MP_OBJ_IS_TYPE(self_in, &exception_type));
    mp_obj_exception_t *self = self_in;
    // for traceback, we are just using the list object for convenience, it's not really a list of Python objects
    if (self->traceback == MP_OBJ_NULL) {
        self->traceback = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)file);
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)line);
    mp_obj_list_append(self->traceback, (mp_obj_t)(machine_uint_t)block);
}

void mp_obj_exception_get_traceback(mp_obj_t self_in, machine_uint_t *n, machine_uint_t **values) {
    assert(MP_OBJ_IS_TYPE(self_in, &exception_type));
    mp_obj_exception_t *self = self_in;
    if (self->traceback == MP_OBJ_NULL) {
        *n = 0;
        *values = NULL;
    } else {
        uint n2;
        mp_obj_list_get(self->traceback, &n2, (mp_obj_t**)values);
        *n = n2;
    }
}
