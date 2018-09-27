/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014-2017 Paul Sokolovsky
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

#include "py/runtime.h"
#include "py/bc.h"
#include "py/objgenerator.h"
#include "py/objfun.h"
#include "py/stackctrl.h"

/******************************************************************************/
/* generator wrapper                                                          */

typedef struct _mp_obj_gen_instance_t {
    mp_obj_base_t base;
    mp_obj_dict_t *globals;
    mp_code_state_t code_state;
} mp_obj_gen_instance_t;

STATIC mp_obj_t gen_wrap_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // A generating function is just a bytecode function with type mp_type_gen_wrap
    mp_obj_fun_bc_t *self_fun = MP_OBJ_TO_PTR(self_in);

    // bytecode prelude: get state size and exception stack size
    size_t n_state = mp_decode_uint_value(self_fun->bytecode);
    size_t n_exc_stack = mp_decode_uint_value(mp_decode_uint_skip(self_fun->bytecode));

    // allocate the generator object, with room for local stack and exception stack
    mp_obj_gen_instance_t *o = m_new_obj_var(mp_obj_gen_instance_t, byte,
        n_state * sizeof(mp_obj_t) + n_exc_stack * sizeof(mp_exc_stack_t));
    o->base.type = &mp_type_gen_instance;

    o->globals = self_fun->globals;
    o->code_state.fun_bc = self_fun;
    o->code_state.ip = 0;
    mp_setup_code_state(&o->code_state, n_args, n_kw, args);
    return MP_OBJ_FROM_PTR(o);
}

const mp_obj_type_t mp_type_gen_wrap = {
    { &mp_type_type },
    .name = MP_QSTR_generator,
    .call = gen_wrap_call,
    .unary_op = mp_generic_unary_op,
    #if MICROPY_PY_FUNCTION_ATTRS
    .attr = mp_obj_fun_bc_attr,
    #endif
};

/******************************************************************************/
/* generator instance                                                         */

STATIC void gen_instance_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<generator object '%q' at %p>", mp_obj_fun_get_name(MP_OBJ_FROM_PTR(self->code_state.fun_bc)), self);
}

mp_vm_return_kind_t mp_obj_gen_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    MP_STACK_CHECK();
    mp_check_self(MP_OBJ_IS_TYPE(self_in, &mp_type_gen_instance));
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->code_state.ip == 0) {
        // Trying to resume already stopped generator
        *ret_val = MP_OBJ_STOP_ITERATION;
        return MP_VM_RETURN_NORMAL;
    }
    if (self->code_state.sp == self->code_state.state - 1) {
        if (send_value != mp_const_none) {
            mp_raise_TypeError("can't send non-None value to a just-started generator");
        }
    } else {
        #if MICROPY_PY_GENERATOR_PEND_THROW
        // If exception is pending (set using .pend_throw()), process it now.
        if (*self->code_state.sp != mp_const_none) {
            throw_value = *self->code_state.sp;
            *self->code_state.sp = MP_OBJ_NULL;
        } else
        #endif
        {
            *self->code_state.sp = send_value;
        }
    }

    // We set self->globals=NULL while executing, for a sentinel to ensure the generator
    // cannot be reentered during execution
    if (self->globals == NULL) {
        mp_raise_ValueError("generator already executing");
    }

    // Set up the correct globals context for the generator and execute it
    self->code_state.old_globals = mp_globals_get();
    mp_globals_set(self->globals);
    self->globals = NULL;
    mp_vm_return_kind_t ret_kind = mp_execute_bytecode(&self->code_state, throw_value);
    self->globals = mp_globals_get();
    mp_globals_set(self->code_state.old_globals);

    switch (ret_kind) {
        case MP_VM_RETURN_NORMAL:
        default:
            // Explicitly mark generator as completed. If we don't do this,
            // subsequent next() may re-execute statements after last yield
            // again and again, leading to side effects.
            self->code_state.ip = 0;
            *ret_val = *self->code_state.sp;
            break;

        case MP_VM_RETURN_YIELD:
            *ret_val = *self->code_state.sp;
            #if MICROPY_PY_GENERATOR_PEND_THROW
            *self->code_state.sp = mp_const_none;
            #endif
            break;

        case MP_VM_RETURN_EXCEPTION: {
            size_t n_state = mp_decode_uint_value(self->code_state.fun_bc->bytecode);
            self->code_state.ip = 0;
            *ret_val = self->code_state.state[n_state - 1];
            // PEP479: if StopIteration is raised inside a generator it is replaced with RuntimeError
            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(*ret_val)), MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                *ret_val = mp_obj_new_exception_msg(&mp_type_RuntimeError, "generator raised StopIteration");
            }
            break;
        }
    }

    return ret_kind;
}

STATIC mp_obj_t gen_resume_and_raise(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value) {
    mp_obj_t ret;
    switch (mp_obj_gen_resume(self_in, send_value, throw_value, &ret)) {
        case MP_VM_RETURN_NORMAL:
        default:
            // Optimize return w/o value in case generator is used in for loop
            if (ret == mp_const_none || ret == MP_OBJ_STOP_ITERATION) {
                return MP_OBJ_STOP_ITERATION;
            } else {
                nlr_raise(mp_obj_new_exception_args(&mp_type_StopIteration, 1, &ret));
            }

        case MP_VM_RETURN_YIELD:
            return ret;

        case MP_VM_RETURN_EXCEPTION:
            nlr_raise(ret);
    }
}

STATIC mp_obj_t gen_instance_iternext(mp_obj_t self_in) {
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
STATIC mp_obj_t gen_instance_throw(size_t n_args, const mp_obj_t *args) {
    mp_obj_t exc = (n_args == 2) ? args[1] : args[2];

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
    switch (mp_obj_gen_resume(self_in, mp_const_none, MP_OBJ_FROM_PTR(&mp_const_GeneratorExit_obj), &ret)) {
        case MP_VM_RETURN_YIELD:
            mp_raise_msg(&mp_type_RuntimeError, "generator ignored GeneratorExit");

        // Swallow GeneratorExit (== successful close), and re-raise any other
        case MP_VM_RETURN_EXCEPTION:
            // ret should always be an instance of an exception class
            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(ret)), MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) {
                return mp_const_none;
            }
            nlr_raise(ret);

        default:
            // The only choice left is MP_VM_RETURN_NORMAL which is successful close
            return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(gen_instance_close_obj, gen_instance_close);

STATIC mp_obj_t gen_instance_pend_throw(mp_obj_t self_in, mp_obj_t exc_in) {
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->code_state.sp == self->code_state.state - 1) {
        mp_raise_TypeError("can't pend throw to just-started generator");
    }
    mp_obj_t prev = *self->code_state.sp;
    *self->code_state.sp = exc_in;
    return prev;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gen_instance_pend_throw_obj, gen_instance_pend_throw);

STATIC const mp_rom_map_elem_t gen_instance_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&gen_instance_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&gen_instance_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&gen_instance_throw_obj) },
    #if MICROPY_PY_GENERATOR_PEND_THROW
    { MP_ROM_QSTR(MP_QSTR_pend_throw), MP_ROM_PTR(&gen_instance_pend_throw_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(gen_instance_locals_dict, gen_instance_locals_dict_table);

const mp_obj_type_t mp_type_gen_instance = {
    { &mp_type_type },
    .name = MP_QSTR_generator,
    .print = gen_instance_print,
    .unary_op = mp_generic_unary_op,
    .getiter = mp_identity_getiter,
    .iternext = gen_instance_iternext,
    .locals_dict = (mp_obj_dict_t*)&gen_instance_locals_dict,
};
