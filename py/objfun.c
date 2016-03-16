/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <string.h>
#include <assert.h>

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objfun.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/bc.h"
#include "py/stackctrl.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

// Note: the "name" entry in mp_obj_type_t for a function type must be
// MP_QSTR_function because it is used to determine if an object is of generic
// function type.

/******************************************************************************/
/* builtin functions                                                          */

// mp_obj_fun_builtin_t defined in obj.h

STATIC mp_obj_t fun_builtin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_fun_builtin));
    mp_obj_fun_builtin_t *self = MP_OBJ_TO_PTR(self_in);

    // check number of arguments
    mp_arg_check_num(n_args, n_kw, self->n_args_min, self->n_args_max, self->is_kw);

    if (self->is_kw) {
        // function allows keywords

        // we create a map directly from the given args array
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

        return self->fun.kw(n_args, args, &kw_args);

    } else if (self->n_args_min <= 3 && self->n_args_min == self->n_args_max) {
        // function requires a fixed number of arguments

        // dispatch function call
        switch (self->n_args_min) {
            case 0:
                return self->fun._0();

            case 1:
                return self->fun._1(args[0]);

            case 2:
                return self->fun._2(args[0], args[1]);

            case 3:
            default:
                return self->fun._3(args[0], args[1], args[2]);
        }

    } else {
        // function takes a variable number of arguments, but no keywords

        return self->fun.var(n_args, args);
    }
}

const mp_obj_type_t mp_type_fun_builtin = {
    { &mp_type_type },
    .name = MP_QSTR_function,
    .call = fun_builtin_call,
    .unary_op = mp_generic_unary_op,
};

/******************************************************************************/
/* byte code functions                                                        */

qstr mp_obj_code_get_name(const byte *code_info) {
    mp_decode_uint(&code_info); // skip code_info_size entry
    #if MICROPY_PERSISTENT_CODE
    return code_info[0] | (code_info[1] << 8);
    #else
    return mp_decode_uint(&code_info);
    #endif
}

#if MICROPY_EMIT_NATIVE
STATIC const mp_obj_type_t mp_type_fun_native;
#endif

qstr mp_obj_fun_get_name(mp_const_obj_t fun_in) {
    const mp_obj_fun_bc_t *fun = MP_OBJ_TO_PTR(fun_in);
    #if MICROPY_EMIT_NATIVE
    if (fun->base.type == &mp_type_fun_native) {
        // TODO native functions don't have name stored
        return MP_QSTR_;
    }
    #endif

    const byte *bc = fun->bytecode;
    mp_decode_uint(&bc); // skip n_state
    mp_decode_uint(&bc); // skip n_exc_stack
    bc++; // skip scope_params
    bc++; // skip n_pos_args
    bc++; // skip n_kwonly_args
    bc++; // skip n_def_pos_args
    return mp_obj_code_get_name(bc);
}

#if MICROPY_CPYTHON_COMPAT
STATIC void fun_bc_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_fun_bc_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "<function %q at 0x%p>", mp_obj_fun_get_name(o_in), o);
}
#endif

#if DEBUG_PRINT
STATIC void dump_args(const mp_obj_t *a, mp_uint_t sz) {
    DEBUG_printf("%p: ", a);
    for (mp_uint_t i = 0; i < sz; i++) {
        DEBUG_printf("%p ", a[i]);
    }
    DEBUG_printf("\n");
}
#else
#define dump_args(...) (void)0
#endif

// With this macro you can tune the maximum number of function state bytes
// that will be allocated on the stack.  Any function that needs more
// than this will try to use the heap, with fallback to stack allocation.
#define VM_MAX_STATE_ON_STACK (11 * sizeof(mp_uint_t))

// Set this to enable a simple stack overflow check.
#define VM_DETECT_STACK_OVERFLOW (0)

#if MICROPY_STACKLESS
mp_code_state *mp_obj_fun_bc_prepare_codestate(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    MP_STACK_CHECK();
    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);

    // get start of bytecode
    const byte *ip = self->bytecode;

    // bytecode prelude: state size and exception stack size
    size_t n_state = mp_decode_uint(&ip);
    size_t n_exc_stack = mp_decode_uint(&ip);

    // allocate state for locals and stack
    size_t state_size = n_state * sizeof(mp_obj_t) + n_exc_stack * sizeof(mp_exc_stack_t);
    mp_code_state *code_state;
    code_state = m_new_obj_var_maybe(mp_code_state, byte, state_size);
    if (!code_state) {
        return NULL;
    }

    code_state->ip = (byte*)(ip - self->bytecode); // offset to after n_state/n_exc_stack
    code_state->n_state = n_state;
    mp_setup_code_state(code_state, self, n_args, n_kw, args);

    // execute the byte code with the correct globals context
    code_state->old_globals = mp_globals_get();
    mp_globals_set(self->globals);

    return code_state;
}
#endif

STATIC mp_obj_t fun_bc_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    MP_STACK_CHECK();

    DEBUG_printf("Input n_args: " UINT_FMT ", n_kw: " UINT_FMT "\n", n_args, n_kw);
    DEBUG_printf("Input pos args: ");
    dump_args(args, n_args);
    DEBUG_printf("Input kw args: ");
    dump_args(args + n_args, n_kw * 2);
    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);
    DEBUG_printf("Func n_def_args: %d\n", self->n_def_args);

    // get start of bytecode
    const byte *ip = self->bytecode;

    // bytecode prelude: state size and exception stack size
    mp_uint_t n_state = mp_decode_uint(&ip);
    mp_uint_t n_exc_stack = mp_decode_uint(&ip);

#if VM_DETECT_STACK_OVERFLOW
    n_state += 1;
#endif

    // allocate state for locals and stack
    mp_uint_t state_size = n_state * sizeof(mp_obj_t) + n_exc_stack * sizeof(mp_exc_stack_t);
    mp_code_state *code_state = NULL;
    if (state_size > VM_MAX_STATE_ON_STACK) {
        code_state = m_new_obj_var_maybe(mp_code_state, byte, state_size);
    }
    if (code_state == NULL) {
        code_state = alloca(sizeof(mp_code_state) + state_size);
        state_size = 0; // indicate that we allocated using alloca
    }

    code_state->ip = (byte*)(ip - self->bytecode); // offset to after n_state/n_exc_stack
    code_state->n_state = n_state;
    mp_setup_code_state(code_state, self, n_args, n_kw, args);

    // execute the byte code with the correct globals context
    code_state->old_globals = mp_globals_get();
    mp_globals_set(self->globals);
    mp_vm_return_kind_t vm_return_kind = mp_execute_bytecode(code_state, MP_OBJ_NULL);
    mp_globals_set(code_state->old_globals);

#if VM_DETECT_STACK_OVERFLOW
    if (vm_return_kind == MP_VM_RETURN_NORMAL) {
        if (code_state->sp < code_state->state) {
            printf("VM stack underflow: " INT_FMT "\n", code_state->sp - code_state->state);
            assert(0);
        }
    }
    // We can't check the case when an exception is returned in state[n_state - 1]
    // and there are no arguments, because in this case our detection slot may have
    // been overwritten by the returned exception (which is allowed).
    if (!(vm_return_kind == MP_VM_RETURN_EXCEPTION && self->n_pos_args + self->n_kwonly_args == 0)) {
        // Just check to see that we have at least 1 null object left in the state.
        bool overflow = true;
        for (mp_uint_t i = 0; i < n_state - self->n_pos_args - self->n_kwonly_args; i++) {
            if (code_state->state[i] == MP_OBJ_NULL) {
                overflow = false;
                break;
            }
        }
        if (overflow) {
            printf("VM stack overflow state=%p n_state+1=" UINT_FMT "\n", code_state->state, n_state);
            assert(0);
        }
    }
#endif

    mp_obj_t result;
    switch (vm_return_kind) {
        case MP_VM_RETURN_NORMAL:
            // return value is in *sp
            result = *code_state->sp;
            break;

        case MP_VM_RETURN_EXCEPTION:
            // return value is in state[n_state - 1]
            result = code_state->state[n_state - 1];
            break;

        case MP_VM_RETURN_YIELD: // byte-code shouldn't yield
        default:
            assert(0);
            result = mp_const_none;
            vm_return_kind = MP_VM_RETURN_NORMAL;
            break;
    }

    // free the state if it was allocated on the heap
    if (state_size != 0) {
        m_del_var(mp_code_state, byte, state_size, code_state);
    }

    if (vm_return_kind == MP_VM_RETURN_NORMAL) {
        return result;
    } else { // MP_VM_RETURN_EXCEPTION
        nlr_raise(result);
    }
}

#if MICROPY_PY_FUNCTION_ATTRS
STATIC void fun_bc_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    if (attr == MP_QSTR___name__) {
        dest[0] = MP_OBJ_NEW_QSTR(mp_obj_fun_get_name(self_in));
    }
}
#endif

const mp_obj_type_t mp_type_fun_bc = {
    { &mp_type_type },
    .name = MP_QSTR_function,
#if MICROPY_CPYTHON_COMPAT
    .print = fun_bc_print,
#endif
    .call = fun_bc_call,
    .unary_op = mp_generic_unary_op,
#if MICROPY_PY_FUNCTION_ATTRS
    .attr = fun_bc_attr,
#endif
};

mp_obj_t mp_obj_new_fun_bc(mp_obj_t def_args_in, mp_obj_t def_kw_args, const byte *code, const mp_uint_t *const_table) {
    mp_uint_t n_def_args = 0;
    mp_uint_t n_extra_args = 0;
    mp_obj_tuple_t *def_args = MP_OBJ_TO_PTR(def_args_in);
    if (def_args_in != MP_OBJ_NULL) {
        assert(MP_OBJ_IS_TYPE(def_args_in, &mp_type_tuple));
        n_def_args = def_args->len;
        n_extra_args = def_args->len;
    }
    if (def_kw_args != MP_OBJ_NULL) {
        n_extra_args += 1;
    }
    mp_obj_fun_bc_t *o = m_new_obj_var(mp_obj_fun_bc_t, mp_obj_t, n_extra_args);
    o->base.type = &mp_type_fun_bc;
    o->globals = mp_globals_get();
    o->bytecode = code;
    o->const_table = const_table;
    if (def_args != NULL) {
        memcpy(o->extra_args, def_args->items, n_def_args * sizeof(mp_obj_t));
    }
    if (def_kw_args != MP_OBJ_NULL) {
        o->extra_args[n_def_args] = def_kw_args;
    }
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
/* native functions                                                           */

#if MICROPY_EMIT_NATIVE

STATIC mp_obj_t fun_native_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    MP_STACK_CHECK();
    mp_obj_fun_bc_t *self = self_in;
    mp_call_fun_t fun = MICROPY_MAKE_POINTER_CALLABLE((void*)self->bytecode);
    return fun(self_in, n_args, n_kw, args);
}

STATIC const mp_obj_type_t mp_type_fun_native = {
    { &mp_type_type },
    .name = MP_QSTR_function,
    .call = fun_native_call,
    .unary_op = mp_generic_unary_op,
};

mp_obj_t mp_obj_new_fun_native(mp_obj_t def_args_in, mp_obj_t def_kw_args, const void *fun_data, const mp_uint_t *const_table) {
    mp_obj_fun_bc_t *o = mp_obj_new_fun_bc(def_args_in, def_kw_args, (const byte*)fun_data, const_table);
    o->base.type = &mp_type_fun_native;
    return o;
}

#endif // MICROPY_EMIT_NATIVE

/******************************************************************************/
/* viper functions                                                            */

#if MICROPY_EMIT_NATIVE

typedef struct _mp_obj_fun_viper_t {
    mp_obj_base_t base;
    mp_uint_t n_args;
    void *fun_data; // GC must be able to trace this pointer
    mp_uint_t type_sig;
} mp_obj_fun_viper_t;

typedef mp_uint_t (*viper_fun_0_t)(void);
typedef mp_uint_t (*viper_fun_1_t)(mp_uint_t);
typedef mp_uint_t (*viper_fun_2_t)(mp_uint_t, mp_uint_t);
typedef mp_uint_t (*viper_fun_3_t)(mp_uint_t, mp_uint_t, mp_uint_t);
typedef mp_uint_t (*viper_fun_4_t)(mp_uint_t, mp_uint_t, mp_uint_t, mp_uint_t);

STATIC mp_obj_t fun_viper_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_fun_viper_t *self = self_in;

    mp_arg_check_num(n_args, n_kw, self->n_args, self->n_args, false);

    void *fun = MICROPY_MAKE_POINTER_CALLABLE(self->fun_data);

    mp_uint_t ret;
    if (n_args == 0) {
        ret = ((viper_fun_0_t)fun)();
    } else if (n_args == 1) {
        ret = ((viper_fun_1_t)fun)(mp_convert_obj_to_native(args[0], self->type_sig >> 4));
    } else if (n_args == 2) {
        ret = ((viper_fun_2_t)fun)(mp_convert_obj_to_native(args[0], self->type_sig >> 4), mp_convert_obj_to_native(args[1], self->type_sig >> 8));
    } else if (n_args == 3) {
        ret = ((viper_fun_3_t)fun)(mp_convert_obj_to_native(args[0], self->type_sig >> 4), mp_convert_obj_to_native(args[1], self->type_sig >> 8), mp_convert_obj_to_native(args[2], self->type_sig >> 12));
    } else if (n_args == 4) {
        ret = ((viper_fun_4_t)fun)(
            mp_convert_obj_to_native(args[0], self->type_sig >> 4),
            mp_convert_obj_to_native(args[1], self->type_sig >> 8),
            mp_convert_obj_to_native(args[2], self->type_sig >> 12),
            mp_convert_obj_to_native(args[3], self->type_sig >> 16)
        );
    } else {
        // TODO 5 or more arguments not supported for viper call
        assert(0);
        ret = 0;
    }

    return mp_convert_native_to_obj(ret, self->type_sig);
}

STATIC const mp_obj_type_t mp_type_fun_viper = {
    { &mp_type_type },
    .name = MP_QSTR_function,
    .call = fun_viper_call,
    .unary_op = mp_generic_unary_op,
};

mp_obj_t mp_obj_new_fun_viper(mp_uint_t n_args, void *fun_data, mp_uint_t type_sig) {
    mp_obj_fun_viper_t *o = m_new_obj(mp_obj_fun_viper_t);
    o->base.type = &mp_type_fun_viper;
    o->n_args = n_args;
    o->fun_data = fun_data;
    o->type_sig = type_sig;
    return o;
}

#endif // MICROPY_EMIT_NATIVE

/******************************************************************************/
/* inline assembler functions                                                 */

#if MICROPY_EMIT_INLINE_THUMB

typedef struct _mp_obj_fun_asm_t {
    mp_obj_base_t base;
    mp_uint_t n_args;
    void *fun_data; // GC must be able to trace this pointer
    mp_uint_t type_sig;
} mp_obj_fun_asm_t;

typedef mp_uint_t (*inline_asm_fun_0_t)(void);
typedef mp_uint_t (*inline_asm_fun_1_t)(mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_2_t)(mp_uint_t, mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_3_t)(mp_uint_t, mp_uint_t, mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_4_t)(mp_uint_t, mp_uint_t, mp_uint_t, mp_uint_t);

// convert a Micro Python object to a sensible value for inline asm
STATIC mp_uint_t convert_obj_for_inline_asm(mp_obj_t obj) {
    // TODO for byte_array, pass pointer to the array
    if (MP_OBJ_IS_SMALL_INT(obj)) {
        return MP_OBJ_SMALL_INT_VALUE(obj);
    } else if (obj == mp_const_none) {
        return 0;
    } else if (obj == mp_const_false) {
        return 0;
    } else if (obj == mp_const_true) {
        return 1;
    } else if (MP_OBJ_IS_TYPE(obj, &mp_type_int)) {
        return mp_obj_int_get_truncated(obj);
    } else if (MP_OBJ_IS_STR(obj)) {
        // pointer to the string (it's probably constant though!)
        mp_uint_t l;
        return (mp_uint_t)mp_obj_str_get_data(obj, &l);
    } else {
        mp_obj_type_t *type = mp_obj_get_type(obj);
        if (0) {
#if MICROPY_PY_BUILTINS_FLOAT
        } else if (type == &mp_type_float) {
            // convert float to int (could also pass in float registers)
            return (mp_int_t)mp_obj_float_get(obj);
#endif
        } else if (type == &mp_type_tuple) {
            // pointer to start of tuple (could pass length, but then could use len(x) for that)
            mp_uint_t len;
            mp_obj_t *items;
            mp_obj_tuple_get(obj, &len, &items);
            return (mp_uint_t)items;
        } else if (type == &mp_type_list) {
            // pointer to start of list (could pass length, but then could use len(x) for that)
            mp_uint_t len;
            mp_obj_t *items;
            mp_obj_list_get(obj, &len, &items);
            return (mp_uint_t)items;
        } else {
            mp_buffer_info_t bufinfo;
            if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_WRITE)) {
                // supports the buffer protocol, return a pointer to the data
                return (mp_uint_t)bufinfo.buf;
            } else {
                // just pass along a pointer to the object
                return (mp_uint_t)obj;
            }
        }
    }
}

STATIC mp_obj_t fun_asm_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_fun_asm_t *self = self_in;

    mp_arg_check_num(n_args, n_kw, self->n_args, self->n_args, false);

    void *fun = MICROPY_MAKE_POINTER_CALLABLE(self->fun_data);

    mp_uint_t ret;
    if (n_args == 0) {
        ret = ((inline_asm_fun_0_t)fun)();
    } else if (n_args == 1) {
        ret = ((inline_asm_fun_1_t)fun)(convert_obj_for_inline_asm(args[0]));
    } else if (n_args == 2) {
        ret = ((inline_asm_fun_2_t)fun)(convert_obj_for_inline_asm(args[0]), convert_obj_for_inline_asm(args[1]));
    } else if (n_args == 3) {
        ret = ((inline_asm_fun_3_t)fun)(convert_obj_for_inline_asm(args[0]), convert_obj_for_inline_asm(args[1]), convert_obj_for_inline_asm(args[2]));
    } else {
        // compiler allows at most 4 arguments
        assert(n_args == 4);
        ret = ((inline_asm_fun_4_t)fun)(
            convert_obj_for_inline_asm(args[0]),
            convert_obj_for_inline_asm(args[1]),
            convert_obj_for_inline_asm(args[2]),
            convert_obj_for_inline_asm(args[3])
        );
    }

    return mp_convert_native_to_obj(ret, self->type_sig);
}

STATIC const mp_obj_type_t mp_type_fun_asm = {
    { &mp_type_type },
    .name = MP_QSTR_function,
    .call = fun_asm_call,
    .unary_op = mp_generic_unary_op,
};

mp_obj_t mp_obj_new_fun_asm(mp_uint_t n_args, void *fun_data, mp_uint_t type_sig) {
    mp_obj_fun_asm_t *o = m_new_obj(mp_obj_fun_asm_t);
    o->base.type = &mp_type_fun_asm;
    o->n_args = n_args;
    o->fun_data = fun_data;
    o->type_sig = type_sig;
    return o;
}

#endif // MICROPY_EMIT_INLINE_THUMB
