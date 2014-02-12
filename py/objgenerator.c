#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "bc.h"

/******************************************************************************/
/* generator wrapper                                                          */

typedef struct _mp_obj_gen_wrap_t {
    mp_obj_base_t base;
    mp_obj_t *fun;
} mp_obj_gen_wrap_t;

STATIC mp_obj_t gen_wrap_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_gen_wrap_t *self = self_in;
    mp_obj_t self_fun = self->fun;
    assert(MP_OBJ_IS_TYPE(self_fun, &fun_bc_type));
    int bc_n_args;
    uint bc_n_state;
    const byte *bc_code;
    mp_obj_fun_bc_get(self_fun, &bc_n_args, &bc_n_state, &bc_code);
    if (n_args != bc_n_args) {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "function takes %d positional arguments but %d were given", bc_n_args, n_args));
    }
    if (n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "function does not take keyword arguments"));
    }

    return mp_obj_new_gen_instance(bc_code, bc_n_state, n_args, args);
}

const mp_obj_type_t gen_wrap_type = {
    { &mp_const_type },
    "generator",
    .call = gen_wrap_call,
};

mp_obj_t mp_obj_new_gen_wrap(mp_obj_t fun) {
    mp_obj_gen_wrap_t *o = m_new_obj(mp_obj_gen_wrap_t);
    o->base.type = &gen_wrap_type;
    o->fun = fun;
    return o;
}

/******************************************************************************/
/* generator instance                                                         */

typedef struct _mp_obj_gen_instance_t {
    mp_obj_base_t base;
    const byte *code_info;
    const byte *ip;
    mp_obj_t *sp;
    uint n_state;
    mp_obj_t state[];
} mp_obj_gen_instance_t;

void gen_instance_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "<generator object 'fun-name' at %p>", self_in);
}

mp_obj_t gen_instance_getiter(mp_obj_t self_in) {
    return self_in;
}

STATIC mp_obj_t gen_next_send(mp_obj_t self_in, mp_obj_t send_value) {
    mp_obj_gen_instance_t *self = self_in;
    if (self->ip == 0) {
        return mp_const_stop_iteration;
    }
    if (self->sp == self->state - 1) {
        if (send_value != mp_const_none) {
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "can't send non-None value to a just-started generator"));
        }
    } else {
        *self->sp = send_value;
    }
    bool yield = mp_execute_byte_code_2(self->code_info, &self->ip, &self->state[self->n_state - 1], &self->sp);
    if (yield) {
        return *self->sp;
    } else {
        // Explicitly mark generator as completed. If we don't do this,
        // subsequent next() may re-execute statements after last yield
        // again and again, leading to side effects.
        // TODO: check how return with value behaves under such conditions
        // in CPython.
        self->ip = 0;
        if (*self->sp == mp_const_none) {
            return mp_const_stop_iteration;
        } else {
            // TODO return StopIteration with value *self->sp
            return mp_const_stop_iteration;
        }
    }
}

mp_obj_t gen_instance_iternext(mp_obj_t self_in) {
    return gen_next_send(self_in, mp_const_none);
}

STATIC mp_obj_t gen_instance_send(mp_obj_t self_in, mp_obj_t send_value) {
    mp_obj_t ret = gen_next_send(self_in, send_value);
    if (ret == mp_const_stop_iteration) {
        nlr_jump(mp_obj_new_exception(MP_QSTR_StopIteration));
    }
    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gen_instance_send_obj, gen_instance_send);

STATIC const mp_method_t gen_type_methods[] = {
    { "send", &gen_instance_send_obj },
    { NULL, NULL }, // end-of-list sentinel
};

const mp_obj_type_t gen_instance_type = {
    { &mp_const_type },
    "generator",
    .print = gen_instance_print,
    .getiter = gen_instance_getiter,
    .iternext = gen_instance_iternext,
    .methods = gen_type_methods,
};

mp_obj_t mp_obj_new_gen_instance(const byte *bytecode, uint n_state, int n_args, const mp_obj_t *args) {
    mp_obj_gen_instance_t *o = m_new_obj_var(mp_obj_gen_instance_t, mp_obj_t, n_state);
    o->base.type = &gen_instance_type;
    o->code_info = bytecode;
    o->ip = bytecode;
    o->sp = &o->state[0] - 1; // sp points to top of stack, which starts off 1 below the state
    o->n_state = n_state;

    // copy args to end of state array, in reverse (that's how mp_execute_byte_code_2 needs it)
    for (int i = 0; i < n_args; i++) {
        o->state[n_state - 1 - i] = args[i];
    }

    // TODO
    // prelude for making cells (closed over variables)
    // for now we just make sure there are no cells variables
    // need to work out how to implement closed over variables in generators

    // get code info size
    machine_uint_t code_info_size = bytecode[0] | (bytecode[1] << 8) | (bytecode[2] << 16) | (bytecode[3] << 24);
    o->ip += code_info_size;
    assert(o->ip[0] == 0);
    o->ip += 1;

    return o;
}
