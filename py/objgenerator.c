/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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
#include "py/objstr.h"
#include "py/objgenerator.h"
#include "py/objfun.h"
#include "py/cstack.h"

// Instance of GeneratorExit exception - needed by generator.close()
const mp_obj_exception_t mp_const_GeneratorExit_obj = {{&mp_type_GeneratorExit}, 0, 0, NULL, (mp_obj_tuple_t *)&mp_const_empty_tuple_obj};

/******************************************************************************/
/* generator wrapper                                                          */

typedef struct _mp_obj_gen_instance_t {
    mp_obj_base_t base;
    // mp_const_none: Not-running, no exception.
    // MP_OBJ_NULL: Running, no exception.
    // other: Not running, pending exception.
    mp_obj_t pend_exc;
    mp_code_state_t code_state;
} mp_obj_gen_instance_t;

static mp_obj_t gen_wrap_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // A generating function is just a bytecode function with type mp_type_gen_wrap
    mp_obj_fun_bc_t *self_fun = MP_OBJ_TO_PTR(self_in);

    // bytecode prelude: get state size and exception stack size
    const uint8_t *ip = self_fun->bytecode;
    MP_BC_PRELUDE_SIG_DECODE(ip);

    // allocate the generator object, with room for local stack and exception stack
    mp_obj_gen_instance_t *o = mp_obj_malloc_var(mp_obj_gen_instance_t, code_state.state, byte,
        n_state * sizeof(mp_obj_t) + n_exc_stack * sizeof(mp_exc_stack_t),
        &mp_type_gen_instance);

    o->pend_exc = mp_const_none;
    o->code_state.fun_bc = self_fun;
    o->code_state.n_state = n_state;
    mp_setup_code_state(&o->code_state, n_args, n_kw, args);
    return MP_OBJ_FROM_PTR(o);
}

#if MICROPY_PY_FUNCTION_ATTRS
#define GEN_WRAP_TYPE_ATTR attr, mp_obj_fun_bc_attr,
#else
#define GEN_WRAP_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_gen_wrap,
    MP_QSTR_generator,
    MP_TYPE_FLAG_BINDS_SELF,
    GEN_WRAP_TYPE_ATTR
    call, gen_wrap_call
    );

/******************************************************************************/
// native generator wrapper

#if MICROPY_EMIT_NATIVE

// Based on mp_obj_gen_instance_t.
typedef struct _mp_obj_gen_instance_native_t {
    mp_obj_base_t base;
    mp_obj_t pend_exc;
    mp_code_state_native_t code_state;
} mp_obj_gen_instance_native_t;

static mp_obj_t native_gen_wrap_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // The state for a native generating function is held in the same struct as a bytecode function
    mp_obj_fun_bc_t *self_fun = MP_OBJ_TO_PTR(self_in);

    // Determine start of prelude.
    const uint8_t *prelude_ptr = mp_obj_fun_native_get_prelude_ptr(self_fun);

    // Extract n_state from the prelude.
    const uint8_t *ip = prelude_ptr;
    MP_BC_PRELUDE_SIG_DECODE(ip);

    // Allocate the generator object, with room for local stack (exception stack not needed).
    mp_obj_gen_instance_native_t *o = mp_obj_malloc_var(mp_obj_gen_instance_native_t, code_state.state, byte, n_state * sizeof(mp_obj_t), &mp_type_gen_instance);

    // Parse the input arguments and set up the code state
    o->pend_exc = mp_const_none;
    o->code_state.fun_bc = self_fun;
    o->code_state.n_state = n_state;
    mp_setup_code_state_native(&o->code_state, n_args, n_kw, args);

    // Indicate we are a native function, which doesn't use this variable
    o->code_state.exc_sp_idx = MP_CODE_STATE_EXC_SP_IDX_SENTINEL;

    // Prepare the generator instance for execution
    o->code_state.ip = mp_obj_fun_native_get_generator_start(self_fun);

    return MP_OBJ_FROM_PTR(o);
}

#if MICROPY_PY_FUNCTION_ATTRS
#define NATIVE_GEN_WRAP_TYPE_ATTR , attr, mp_obj_fun_bc_attr
#else
#define NATIVE_GEN_WRAP_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_native_gen_wrap,
    MP_QSTR_generator,
    MP_TYPE_FLAG_BINDS_SELF,
    call, native_gen_wrap_call
    NATIVE_GEN_WRAP_TYPE_ATTR
    );

#endif // MICROPY_EMIT_NATIVE

/******************************************************************************/
/* generator instance                                                         */

static void gen_instance_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<generator object '%q' at %p>", mp_obj_fun_get_name(MP_OBJ_FROM_PTR(self->code_state.fun_bc)), self);
}

mp_vm_return_kind_t mp_obj_gen_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    mp_cstack_check();
    mp_check_self(mp_obj_is_type(self_in, &mp_type_gen_instance));
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->code_state.ip == 0) {
        // Trying to resume an already stopped generator.
        // This is an optimised "raise StopIteration(None)".
        *ret_val = mp_const_none;
        return MP_VM_RETURN_NORMAL;
    }

    // Ensure the generator cannot be reentered during execution
    if (self->pend_exc == MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("generator already executing"));
    }

    #if MICROPY_PY_GENERATOR_PEND_THROW
    // If exception is pending (set using .pend_throw()), process it now.
    if (self->pend_exc != mp_const_none) {
        throw_value = self->pend_exc;
    }
    #endif

    // If the generator is started, allow sending a value.
    void *state_start = self->code_state.state - 1;
    #if MICROPY_EMIT_NATIVE
    if (self->code_state.exc_sp_idx == MP_CODE_STATE_EXC_SP_IDX_SENTINEL) {
        state_start = ((mp_obj_gen_instance_native_t *)self)->code_state.state - 1;
    }
    #endif
    if (self->code_state.sp == state_start) {
        if (send_value != mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't send non-None value to a just-started generator"));
        }
    } else {
        *self->code_state.sp = send_value;
    }

    // Mark as running
    self->pend_exc = MP_OBJ_NULL;

    // Set up the correct globals context for the generator and execute it
    self->code_state.old_globals = mp_globals_get();
    mp_globals_set(self->code_state.fun_bc->context->module.globals);

    mp_vm_return_kind_t ret_kind;

    #if MICROPY_EMIT_NATIVE
    if (self->code_state.exc_sp_idx == MP_CODE_STATE_EXC_SP_IDX_SENTINEL) {
        // A native generator.
        typedef uintptr_t (*mp_fun_native_gen_t)(void *, mp_obj_t);
        mp_fun_native_gen_t fun = mp_obj_fun_native_get_generator_resume(self->code_state.fun_bc);
        ret_kind = fun((void *)&self->code_state, throw_value);
    } else
    #endif
    {
        // A bytecode generator
        ret_kind = mp_execute_bytecode(&self->code_state, throw_value);
    }

    mp_globals_set(self->code_state.old_globals);

    // Mark as not running
    self->pend_exc = mp_const_none;

    switch (ret_kind) {
        case MP_VM_RETURN_NORMAL:
        default:
            // Explicitly mark generator as completed. If we don't do this,
            // subsequent next() may re-execute statements after last yield
            // again and again, leading to side effects.
            self->code_state.ip = 0;
            // This is an optimised "raise StopIteration(*ret_val)".
            *ret_val = *self->code_state.sp;
            break;

        case MP_VM_RETURN_YIELD:
            *ret_val = *self->code_state.sp;
            #if MICROPY_PY_GENERATOR_PEND_THROW
            *self->code_state.sp = mp_const_none;
            #endif
            break;

        case MP_VM_RETURN_EXCEPTION: {
            self->code_state.ip = 0;
            #if MICROPY_EMIT_NATIVE
            if (self->code_state.exc_sp_idx == MP_CODE_STATE_EXC_SP_IDX_SENTINEL) {
                *ret_val = ((mp_obj_gen_instance_native_t *)self)->code_state.state[0];
            } else
            #endif
            {
                *ret_val = self->code_state.state[0];
            }
            // PEP479: if StopIteration is raised inside a generator it is replaced with RuntimeError
            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(*ret_val)), MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                *ret_val = mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("generator raised StopIteration"));
            }
            break;
        }
    }

    return ret_kind;
}

static mp_obj_t gen_resume_and_raise(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, bool raise_stop_iteration) {
    mp_obj_t ret;
    switch (mp_obj_gen_resume(self_in, send_value, throw_value, &ret)) {
        case MP_VM_RETURN_NORMAL:
        default:
            // A normal return is a StopIteration, either raise it or return
            // MP_OBJ_STOP_ITERATION as an optimisation.
            if (ret == mp_const_none) {
                ret = MP_OBJ_NULL;
            }
            if (raise_stop_iteration) {
                mp_raise_StopIteration(ret);
            } else {
                return mp_make_stop_iteration(ret);
            }

        case MP_VM_RETURN_YIELD:
            return ret;

        case MP_VM_RETURN_EXCEPTION:
            nlr_raise(ret);
    }
}

static mp_obj_t gen_instance_iternext(mp_obj_t self_in) {
    return gen_resume_and_raise(self_in, mp_const_none, MP_OBJ_NULL, false);
}

static mp_obj_t gen_instance_send(mp_obj_t self_in, mp_obj_t send_value) {
    return gen_resume_and_raise(self_in, send_value, MP_OBJ_NULL, true);
}
static MP_DEFINE_CONST_FUN_OBJ_2(gen_instance_send_obj, gen_instance_send);

static mp_obj_t gen_instance_throw(size_t n_args, const mp_obj_t *args) {
    // The signature of this function is: throw(type[, value[, traceback]])
    // CPython will pass all given arguments through the call chain and process them
    // at the point they are used (native generators will handle them differently to
    // user-defined generators with a throw() method).  To save passing multiple
    // values, MicroPython instead does partial processing here to reduce it down to
    // one argument and passes that through:
    // - if only args[1] is given, or args[2] is given but is None, args[1] is
    //   passed through (in the standard case it is an exception class or instance)
    // - if args[2] is given and not None it is passed through (in the standard
    //   case it would be an exception instance and args[1] its corresponding class)
    // - args[3] is always ignored

    mp_obj_t exc = args[1];
    if (n_args > 2 && args[2] != mp_const_none) {
        exc = args[2];
    }

    return gen_resume_and_raise(args[0], mp_const_none, exc, true);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gen_instance_throw_obj, 2, 4, gen_instance_throw);

static mp_obj_t gen_instance_close(mp_obj_t self_in) {
    mp_obj_t ret;
    switch (mp_obj_gen_resume(self_in, mp_const_none, MP_OBJ_FROM_PTR(&mp_const_GeneratorExit_obj), &ret)) {
        case MP_VM_RETURN_YIELD:
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("generator ignored GeneratorExit"));

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
static MP_DEFINE_CONST_FUN_OBJ_1(gen_instance_close_obj, gen_instance_close);

#if MICROPY_PY_GENERATOR_PEND_THROW
static mp_obj_t gen_instance_pend_throw(mp_obj_t self_in, mp_obj_t exc_in) {
    mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->pend_exc == MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("generator already executing"));
    }
    mp_obj_t prev = self->pend_exc;
    self->pend_exc = exc_in;
    return prev;
}
static MP_DEFINE_CONST_FUN_OBJ_2(gen_instance_pend_throw_obj, gen_instance_pend_throw);
#endif

static const mp_rom_map_elem_t gen_instance_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&gen_instance_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&gen_instance_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&gen_instance_throw_obj) },
    #if MICROPY_PY_GENERATOR_PEND_THROW
    { MP_ROM_QSTR(MP_QSTR_pend_throw), MP_ROM_PTR(&gen_instance_pend_throw_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(gen_instance_locals_dict, gen_instance_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_gen_instance,
    MP_QSTR_generator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    print, gen_instance_print,
    iter, gen_instance_iternext,
    locals_dict, &gen_instance_locals_dict
    );
