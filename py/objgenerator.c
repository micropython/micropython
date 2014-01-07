#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"
#include "bc.h"

/******************************************************************************/
/* generator wrapper                                                          */

typedef struct _mp_obj_gen_wrap_t {
    mp_obj_base_t base;
    uint n_state;
    mp_obj_t *fun;
} mp_obj_gen_wrap_t;

// args are in reverse order in the array
mp_obj_t gen_wrap_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    mp_obj_gen_wrap_t *self = self_in;
    mp_obj_t self_fun = self->fun;
    assert(MP_OBJ_IS_TYPE(self_fun, &fun_bc_type));
    int bc_n_args;
    uint bc_n_state;
    const byte *bc_code;
    mp_obj_fun_bc_get(self_fun, &bc_n_args, &bc_n_state, &bc_code);
    if (n_args != bc_n_args) {
        nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_TypeError, "function takes %d positional arguments but %d were given", (const char*)(machine_int_t)bc_n_args, (const char*)(machine_int_t)n_args));
    }

    return mp_obj_new_gen_instance(bc_code, self->n_state, n_args, args);
}

const mp_obj_type_t gen_wrap_type = {
    { &mp_const_type },
    "generator",
    .call_n = gen_wrap_call_n,
};

mp_obj_t mp_obj_new_gen_wrap(uint n_locals, uint n_stack, mp_obj_t fun) {
    mp_obj_gen_wrap_t *o = m_new_obj(mp_obj_gen_wrap_t);
    o->base.type = &gen_wrap_type;
    // we have at least 3 locals so the bc can write back fast[0,1,2] safely; should improve how this is done
    o->n_state = (n_locals < 3 ? 3 : n_locals) + n_stack;
    o->fun = fun;
    return o;
}

/******************************************************************************/
/* generator instance                                                         */

typedef struct _mp_obj_gen_instance_t {
    mp_obj_base_t base;
    const byte *ip;
    mp_obj_t *sp;
    mp_obj_t state[];
} mp_obj_gen_instance_t;

void gen_instance_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    print(env, "<generator object 'fun-name' at %p>", self_in);
}

mp_obj_t gen_instance_getiter(mp_obj_t self_in) {
    return self_in;
}

mp_obj_t gen_instance_iternext(mp_obj_t self_in) {
    mp_obj_gen_instance_t *self = self_in;
    bool yield = mp_execute_byte_code_2(&self->ip, &self->state[0], &self->sp);
    if (yield) {
        return *self->sp;
    } else {
        if (*self->sp == mp_const_none) {
            return mp_const_stop_iteration;
        } else {
            // TODO return StopIteration with value *self->sp
            return mp_const_stop_iteration;
        }
    }
}

const mp_obj_type_t gen_instance_type = {
    { &mp_const_type },
    "generator",
    .print = gen_instance_print,
    .getiter = gen_instance_getiter,
    .iternext = gen_instance_iternext,
};

// args are in reverse order in the array
mp_obj_t mp_obj_new_gen_instance(const byte *bytecode, uint n_state, int n_args, const mp_obj_t *args) {
    mp_obj_gen_instance_t *o = m_new_obj_var(mp_obj_gen_instance_t, mp_obj_t, n_state);
    o->base.type = &gen_instance_type;
    o->ip = bytecode;
    o->sp = o->state + n_state;

    // copy args (which are in reverse order) to start of state array
    for (int i = 0; i < n_args; i++) {
        o->state[i] = args[n_args - 1 - i];
    }

    // TODO
    // prelude for making cells (closed over variables)
    // for now we just make sure there are no cells variables
    // need to work out how to implement closed over variables in generators
    assert(o->ip[0] == 0);
    o->ip += 1;

    return o;
}
