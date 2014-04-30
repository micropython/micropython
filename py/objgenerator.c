/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <assert.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "bc.h"
#include "objgenerator.h"
#include "objfun.h"

/******************************************************************************/
/* generator wrapper                                                          */

typedef struct _mp_obj_gen_wrap_t {
    mp_obj_base_t base;
    mp_obj_t *fun;
} mp_obj_gen_wrap_t;

mp_obj_t mp_obj_new_gen_instance(mp_obj_dict_t *globals, const byte *bytecode, uint n_args, const mp_obj_t *args,
    uint n_args2, const mp_obj_t *args2);

STATIC mp_obj_t gen_wrap_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_gen_wrap_t *self = self_in;
    mp_obj_fun_bc_t *self_fun = (mp_obj_fun_bc_t*)self->fun;
    assert(MP_OBJ_IS_TYPE(self_fun, &mp_type_fun_bc));

    const mp_obj_t *args1, *args2;
    uint len1, len2;
    if (!mp_obj_fun_prepare_simple_args(self_fun, n_args, n_kw, args, &len1, &args1, &len2, &args2)) {
        assert(0);
    }

    return mp_obj_new_gen_instance(self_fun->globals, self_fun->bytecode, len1, args1, len2, args2);
}

const mp_obj_type_t mp_type_gen_wrap = {
    { &mp_type_type },
    .name = MP_QSTR_generator,
    .call = gen_wrap_call,
};

mp_obj_t mp_obj_new_gen_wrap(mp_obj_t fun) {
    mp_obj_gen_wrap_t *o = m_new_obj(mp_obj_gen_wrap_t);
    o->base.type = &mp_type_gen_wrap;
    o->fun = fun;
    return o;
}

/******************************************************************************/
/* generator instance                                                         */

typedef struct _mp_obj_gen_instance_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;
    const byte *code_info;
    const byte *ip;
    mp_obj_t *sp;
    // bit 0 is saved currently_in_except_block value
    mp_exc_stack_t *exc_sp;
    uint n_state;
    // Variable-length
    mp_obj_t state[0];
    // Variable-length, never accessed by name, only as (void*)(state + n_state)
    //mp_exc_stack_t exc_state[0];
} mp_obj_gen_instance_t;

void gen_instance_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_gen_instance_t *self = self_in;
    print(env, "<generator object '%s' at %p>", mp_obj_code_get_name(self->code_info), self_in);
}

mp_obj_t gen_instance_getiter(mp_obj_t self_in) {
    return self_in;
}

mp_vm_return_kind_t mp_obj_gen_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_gen_instance));
    mp_obj_gen_instance_t *self = self_in;
    if (self->ip == 0) {
        *ret_val = MP_OBJ_STOP_ITERATION;
        return MP_VM_RETURN_NORMAL;
    }
    if (self->sp == self->state - 1) {
        if (send_value != mp_const_none) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "can't send non-None value to a just-started generator"));
        }
    } else {
        *self->sp = send_value;
    }
    mp_obj_dict_t *old_globals = mp_globals_get();
    mp_globals_set(self->globals);
    mp_vm_return_kind_t ret_kind = mp_execute_byte_code_2(self->code_info, &self->ip,
        &self->state[self->n_state - 1], &self->sp, (mp_exc_stack_t*)(self->state + self->n_state),
        &self->exc_sp, throw_value);
    mp_globals_set(old_globals);

    switch (ret_kind) {
        case MP_VM_RETURN_NORMAL:
            // Explicitly mark generator as completed. If we don't do this,
            // subsequent next() may re-execute statements after last yield
            // again and again, leading to side effects.
            // TODO: check how return with value behaves under such conditions
            // in CPython.
            self->ip = 0;
            *ret_val = *self->sp;
            break;

        case MP_VM_RETURN_YIELD:
            *ret_val = *self->sp;
            break;

        case MP_VM_RETURN_EXCEPTION:
            self->ip = 0;
            *ret_val = self->state[self->n_state - 1];
            break;

        default:
            assert(0);
            *ret_val = mp_const_none;
            break;
    }

    return ret_kind;
}

STATIC mp_obj_t gen_resume_and_raise(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value) {
    mp_obj_t ret;
    switch (mp_obj_gen_resume(self_in, send_value, throw_value, &ret)) {
        case MP_VM_RETURN_NORMAL:
            // Optimize return w/o value in case generator is used in for loop
            if (ret == mp_const_none || ret == MP_OBJ_STOP_ITERATION) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                nlr_raise(mp_obj_new_exception_args(&mp_type_StopIteration, 1, &ret));
            }

        case MP_VM_RETURN_YIELD:
            if (throw_value != MP_OBJ_NULL && mp_obj_is_subclass_fast(mp_obj_get_type(throw_value), &mp_type_GeneratorExit)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, "generator ignored GeneratorExit"));
            }
            return ret;

        case MP_VM_RETURN_EXCEPTION:
            // TODO: Optimization of returning MP_OBJ_STOP_ITERATION is really part
            // of mp_iternext() protocol, but this function is called by other methods
            // too, which may not handled MP_OBJ_STOP_ITERATION.
            if (mp_obj_is_subclass_fast(mp_obj_get_type(ret), &mp_type_StopIteration)) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                nlr_raise(ret);
            }

        default:
            assert(0);
            return mp_const_none;
    }
}

mp_obj_t gen_instance_iternext(mp_obj_t self_in) {
    return gen_resume_and_raise(self_in, mp_const_none, MP_OBJ_NULL);
}

STATIC mp_obj_t gen_instance_send(mp_obj_t self_in, mp_obj_t send_value) {
    mp_obj_t ret = gen_resume_and_raise(self_in, send_value, MP_OBJ_NULL);
    if (ret == MP_OBJ_STOP_ITERATION) {
        nlr_raise(mp_obj_new_exception(&mp_type_StopIteration));
    } else {
        return ret;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(gen_instance_send_obj, gen_instance_send);

STATIC mp_obj_t gen_instance_close(mp_obj_t self_in);
STATIC mp_obj_t gen_instance_throw(uint n_args, const mp_obj_t *args) {
    mp_obj_t exc = (n_args == 2) ? args[1] : args[2];
    exc = mp_make_raise_obj(exc);

    mp_obj_t ret = gen_resume_and_raise(args[0], mp_const_none, exc);
    if (ret == MP_OBJ_STOP_ITERATION) {
        nlr_raise(mp_obj_new_exception(&mp_type_StopIteration));
    } else {
        return ret;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gen_instance_throw_obj, 2, 4, gen_instance_throw);

STATIC mp_obj_t gen_instance_close(mp_obj_t self_in) {
    mp_obj_t ret;
    switch (mp_obj_gen_resume(self_in, mp_const_none, (mp_obj_t)&mp_const_GeneratorExit_obj, &ret)) {
        case MP_VM_RETURN_YIELD:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError, "generator ignored GeneratorExit"));

        // Swallow StopIteration & GeneratorExit (== successful close), and re-raise any other
        case MP_VM_RETURN_EXCEPTION:
            // ret should always be an instance of an exception class
            if (mp_obj_is_subclass_fast(mp_obj_get_type(ret), &mp_type_GeneratorExit) ||
                mp_obj_is_subclass_fast(mp_obj_get_type(ret), &mp_type_StopIteration)) {
                return mp_const_none;
            }
            nlr_raise(ret);

        default:
            // The only choice left is MP_VM_RETURN_NORMAL which is successful close
            return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(gen_instance_close_obj, gen_instance_close);

STATIC const mp_map_elem_t gen_instance_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&gen_instance_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&gen_instance_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_throw), (mp_obj_t)&gen_instance_throw_obj },
};

STATIC MP_DEFINE_CONST_DICT(gen_instance_locals_dict, gen_instance_locals_dict_table);

const mp_obj_type_t mp_type_gen_instance = {
    { &mp_type_type },
    .name = MP_QSTR_generator,
    .print = gen_instance_print,
    .getiter = gen_instance_getiter,
    .iternext = gen_instance_iternext,
    .locals_dict = (mp_obj_t)&gen_instance_locals_dict,
};

mp_obj_t mp_obj_new_gen_instance(mp_obj_dict_t *globals, const byte *bytecode, uint n_args, const mp_obj_t *args,
                                 uint n_args2, const mp_obj_t *args2) {
    const byte *code_info = bytecode;
    // get code info size, and skip the line number table
    machine_uint_t code_info_size = bytecode[0] | (bytecode[1] << 8) | (bytecode[2] << 16) | (bytecode[3] << 24);
    bytecode += code_info_size;

    // bytecode prelude: get state size and exception stack size
    machine_uint_t n_state = bytecode[0] | (bytecode[1] << 8);
    machine_uint_t n_exc_stack = bytecode[2] | (bytecode[3] << 8);
    bytecode += 4;

    // allocate the generator object, with room for local stack and exception stack
    mp_obj_gen_instance_t *o = m_new_obj_var(mp_obj_gen_instance_t, byte, n_state * sizeof(mp_obj_t) + n_exc_stack * sizeof(mp_exc_stack_t));
    o->base.type = &mp_type_gen_instance;
    o->globals = globals;
    o->code_info = code_info;
    o->sp = &o->state[0] - 1; // sp points to top of stack, which starts off 1 below the state
    o->exc_sp = (mp_exc_stack_t*)(o->state + n_state) - 1;
    o->n_state = n_state;

    // copy args to end of state array, in reverse (that's how mp_execute_byte_code_2 needs it)
    for (uint i = 0; i < n_args; i++) {
        o->state[n_state - 1 - i] = args[i];
    }
    for (uint i = 0; i < n_args2; i++) {
        o->state[n_state - 1 - n_args - i] = args2[i];
    }

    // set rest of state to MP_OBJ_NULL
    for (uint i = 0; i < n_state - n_args - n_args2; i++) {
        o->state[i] = MP_OBJ_NULL;
    }

    // bytecode prelude: initialise closed over variables
    for (uint n_local = *bytecode++; n_local > 0; n_local--) {
        uint local_num = *bytecode++;
        o->state[n_state - 1 - local_num] = mp_obj_new_cell(o->state[n_state - 1 - local_num]);
    }

    // set ip to start of actual byte code
    o->ip = bytecode;

    return o;
}
