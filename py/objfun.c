/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/emitglue.h"
#include "py/objcode.h"
#include "py/objtuple.h"
#include "py/objfun.h"
#include "py/runtime.h"
#include "py/bc.h"
#include "py/cstack.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
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

static mp_obj_t fun_builtin_0_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)args;
    assert(mp_obj_is_type(self_in, &mp_type_fun_builtin_0));
    mp_obj_fun_builtin_fixed_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return self->fun._0();
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_builtin_0, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, fun_builtin_0_call
    );

static mp_obj_t fun_builtin_1_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_type(self_in, &mp_type_fun_builtin_1));
    mp_obj_fun_builtin_fixed_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    return self->fun._1(args[0]);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_builtin_1, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, fun_builtin_1_call
    );

static mp_obj_t fun_builtin_2_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_type(self_in, &mp_type_fun_builtin_2));
    mp_obj_fun_builtin_fixed_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    return self->fun._2(args[0], args[1]);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_builtin_2, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, fun_builtin_2_call
    );

static mp_obj_t fun_builtin_3_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_type(self_in, &mp_type_fun_builtin_3));
    mp_obj_fun_builtin_fixed_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, 3, 3, false);
    return self->fun._3(args[0], args[1], args[2]);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_builtin_3, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, fun_builtin_3_call
    );

static mp_obj_t fun_builtin_var_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_type(self_in, &mp_type_fun_builtin_var));
    mp_obj_fun_builtin_var_t *self = MP_OBJ_TO_PTR(self_in);

    // check number of arguments
    mp_arg_check_num_sig(n_args, n_kw, self->sig);

    if (self->sig & 1) {
        // function allows keywords

        // we create a map directly from the given args array; self->fun.kw does still
        // expect args to have both positional and keyword arguments, ordered as:
        // arg0 arg1 ... arg<n_args> key0 value0 key1 value1 ... key<n_kw> value<n_kw>
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

        return self->fun.kw(n_args, args, &kw_args);

    } else {
        // function takes a variable number of arguments, but no keywords

        return self->fun.var(n_args, args);
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_builtin_var, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, fun_builtin_var_call
    );

/******************************************************************************/
/* byte code functions                                                        */

qstr mp_obj_fun_get_name(mp_const_obj_t fun_in) {
    const mp_obj_fun_bc_t *fun = MP_OBJ_TO_PTR(fun_in);
    const byte *bc = fun->bytecode;

    #if MICROPY_EMIT_NATIVE
    if (fun->base.type == &mp_type_fun_native || fun->base.type == &mp_type_native_gen_wrap) {
        bc = mp_obj_fun_native_get_prelude_ptr(fun);
    }
    #endif

    MP_BC_PRELUDE_SIG_DECODE(bc);
    MP_BC_PRELUDE_SIZE_DECODE(bc);

    mp_uint_t name = mp_decode_uint_value(bc);
    #if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
    name = fun->context->constants.qstr_table[name];
    #endif

    return name;
}

#if MICROPY_PY_FUNCTION_ATTRS_CODE
static mp_obj_t fun_bc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    if (!mp_obj_is_type(args[0], &mp_type_code)) {
        mp_raise_TypeError(NULL);
    }
    if (!mp_obj_is_type(args[1], &mp_type_dict)) {
        mp_raise_TypeError(NULL);
    }

    mp_obj_code_t *code = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t globals = args[1];

    mp_module_context_t *module_context = m_new_obj(mp_module_context_t);
    module_context->module.base.type = &mp_type_module;
    module_context->module.globals = MP_OBJ_TO_PTR(globals);
    module_context->constants = *mp_code_get_constants(code);

    return mp_make_function_from_proto_fun(mp_code_get_proto_fun(code), module_context, NULL);
}
#endif

#if MICROPY_CPYTHON_COMPAT
static void fun_bc_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_fun_bc_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "<function %q at 0x%p>", mp_obj_fun_get_name(o_in), o);
}
#endif

#if DEBUG_PRINT
static void dump_args(const mp_obj_t *a, size_t sz) {
    DEBUG_printf("%p: ", a);
    for (size_t i = 0; i < sz; i++) {
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
#define VM_MAX_STATE_ON_STACK (sizeof(mp_uint_t) * 11)

#define DECODE_CODESTATE_SIZE(bytecode, n_state_out_var, state_size_out_var) \
    { \
        const uint8_t *ip = bytecode; \
        size_t n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_args; \
        MP_BC_PRELUDE_SIG_DECODE_INTO(ip, n_state_out_var, n_exc_stack, scope_flags, n_pos_args, n_kwonly_args, n_def_args); \
        (void)scope_flags; (void)n_pos_args; (void)n_kwonly_args; (void)n_def_args; \
        \
        /* state size in bytes */                                                 \
        state_size_out_var = n_state_out_var * sizeof(mp_obj_t)                   \
            + n_exc_stack * sizeof(mp_exc_stack_t);                \
    }

#define INIT_CODESTATE(code_state, _fun_bc, _n_state, n_args, n_kw, args) \
    code_state->fun_bc = _fun_bc; \
    code_state->n_state = _n_state; \
    mp_setup_code_state(code_state, n_args, n_kw, args); \
    code_state->old_globals = mp_globals_get();

#if MICROPY_STACKLESS
mp_code_state_t *mp_obj_fun_bc_prepare_codestate(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_cstack_check();
    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);

    size_t n_state, state_size;
    DECODE_CODESTATE_SIZE(self->bytecode, n_state, state_size);

    mp_code_state_t *code_state;
    #if MICROPY_ENABLE_PYSTACK
    code_state = mp_pystack_alloc(sizeof(mp_code_state_t) + state_size);
    #else
    // If we use m_new_obj_var(), then on no memory, MemoryError will be
    // raised. But this is not correct exception for a function call,
    // RuntimeError should be raised instead. So, we use m_new_obj_var_maybe(),
    // return NULL, then vm.c takes the needed action (either raise
    // RuntimeError or fallback to stack allocation).
    code_state = m_new_obj_var_maybe(mp_code_state_t, state, byte, state_size);
    if (!code_state) {
        return NULL;
    }
    #endif

    INIT_CODESTATE(code_state, self, n_state, n_args, n_kw, args);

    // execute the byte code with the correct globals context
    mp_globals_set(self->context->module.globals);

    return code_state;
}
#endif

static mp_obj_t fun_bc_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_cstack_check();

    DEBUG_printf("Input n_args: " UINT_FMT ", n_kw: " UINT_FMT "\n", n_args, n_kw);
    DEBUG_printf("Input pos args: ");
    dump_args(args, n_args);
    DEBUG_printf("Input kw args: ");
    dump_args(args + n_args, n_kw * 2);

    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);

    size_t n_state, state_size;
    DECODE_CODESTATE_SIZE(self->bytecode, n_state, state_size);

    // allocate state for locals and stack
    mp_code_state_t *code_state = NULL;
    #if MICROPY_ENABLE_PYSTACK
    code_state = mp_pystack_alloc(offsetof(mp_code_state_t, state) + state_size);
    #else
    if (state_size > VM_MAX_STATE_ON_STACK) {
        code_state = m_new_obj_var_maybe(mp_code_state_t, state, byte, state_size);
        #if MICROPY_DEBUG_VM_STACK_OVERFLOW
        if (code_state != NULL) {
            memset(code_state->state, 0, state_size);
        }
        #endif
    }
    if (code_state == NULL) {
        code_state = alloca(offsetof(mp_code_state_t, state) + state_size);
        #if MICROPY_DEBUG_VM_STACK_OVERFLOW
        memset(code_state->state, 0, state_size);
        #endif
        state_size = 0; // indicate that we allocated using alloca
    }
    #endif

    INIT_CODESTATE(code_state, self, n_state, n_args, n_kw, args);

    // execute the byte code with the correct globals context
    mp_globals_set(self->context->module.globals);
    mp_vm_return_kind_t vm_return_kind = mp_execute_bytecode(code_state, MP_OBJ_NULL);
    mp_globals_set(code_state->old_globals);

    #if MICROPY_DEBUG_VM_STACK_OVERFLOW
    if (vm_return_kind == MP_VM_RETURN_NORMAL) {
        if (code_state->sp < code_state->state) {
            mp_printf(MICROPY_DEBUG_PRINTER, "VM stack underflow: " INT_FMT "\n", code_state->sp - code_state->state);
            assert(0);
        }
    }
    const byte *bytecode_ptr = self->bytecode;
    size_t n_state_unused, n_exc_stack_unused, scope_flags_unused;
    size_t n_pos_args, n_kwonly_args, n_def_args_unused;
    MP_BC_PRELUDE_SIG_DECODE_INTO(bytecode_ptr, n_state_unused, n_exc_stack_unused,
        scope_flags_unused, n_pos_args, n_kwonly_args, n_def_args_unused);
    // We can't check the case when an exception is returned in state[0]
    // and there are no arguments, because in this case our detection slot may have
    // been overwritten by the returned exception (which is allowed).
    if (!(vm_return_kind == MP_VM_RETURN_EXCEPTION && n_pos_args + n_kwonly_args == 0)) {
        // Just check to see that we have at least 1 null object left in the state.
        bool overflow = true;
        for (size_t i = 0; i < n_state - n_pos_args - n_kwonly_args; ++i) {
            if (code_state->state[i] == MP_OBJ_NULL) {
                overflow = false;
                break;
            }
        }
        if (overflow) {
            mp_printf(MICROPY_DEBUG_PRINTER, "VM stack overflow state=%p n_state+1=" UINT_FMT "\n", code_state->state, n_state);
            assert(0);
        }
    }
    #endif

    mp_obj_t result;
    if (vm_return_kind == MP_VM_RETURN_NORMAL) {
        // return value is in *sp
        result = *code_state->sp;
    } else {
        // must be an exception because normal functions can't yield
        assert(vm_return_kind == MP_VM_RETURN_EXCEPTION);
        // returned exception is in state[0]
        result = code_state->state[0];
    }

    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_free(code_state);
    #else
    // free the state if it was allocated on the heap
    if (state_size != 0) {
        m_del_var(mp_code_state_t, state, byte, state_size, code_state);
    }
    #endif

    if (vm_return_kind == MP_VM_RETURN_NORMAL) {
        return result;
    } else { // MP_VM_RETURN_EXCEPTION
        nlr_raise(result);
    }
}

#if MICROPY_PY_FUNCTION_ATTRS
void mp_obj_fun_bc_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }
    if (attr == MP_QSTR___name__) {
        dest[0] = MP_OBJ_NEW_QSTR(mp_obj_fun_get_name(self_in));
    }
    if (attr == MP_QSTR___globals__) {
        mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);
        dest[0] = MP_OBJ_FROM_PTR(self->context->module.globals);
    }
    #if MICROPY_PY_FUNCTION_ATTRS_CODE
    if (attr == MP_QSTR___code__) {
        const mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);
        if ((self->base.type == &mp_type_fun_bc
             || self->base.type == &mp_type_gen_wrap)
            && self->child_table == NULL) {
            #if MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_BASIC
            dest[0] = mp_obj_new_code(self->context->constants, self->bytecode);
            #else
            dest[0] = mp_obj_new_code(self->context, self->rc, true);
            #endif
        }
    }
    #endif
}
#endif

#if MICROPY_PY_FUNCTION_ATTRS_CODE
#define FUN_BC_MAKE_NEW make_new, fun_bc_make_new,
#else
#define FUN_BC_MAKE_NEW
#endif

#if MICROPY_CPYTHON_COMPAT
#define FUN_BC_TYPE_PRINT print, fun_bc_print,
#else
#define FUN_BC_TYPE_PRINT
#endif

#if MICROPY_PY_FUNCTION_ATTRS
#define FUN_BC_TYPE_ATTR attr, mp_obj_fun_bc_attr,
#else
#define FUN_BC_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_bc,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    FUN_BC_MAKE_NEW
    FUN_BC_TYPE_PRINT
    FUN_BC_TYPE_ATTR
    call, fun_bc_call
    );

mp_obj_t mp_obj_new_fun_bc(const mp_obj_t *def_args, const byte *code, const mp_module_context_t *context, struct _mp_raw_code_t *const *child_table) {
    size_t n_def_args = 0;
    size_t n_extra_args = 0;
    mp_obj_tuple_t *def_pos_args = NULL;
    mp_obj_t def_kw_args = MP_OBJ_NULL;
    if (def_args != NULL && def_args[0] != MP_OBJ_NULL) {
        assert(mp_obj_is_type(def_args[0], &mp_type_tuple));
        def_pos_args = MP_OBJ_TO_PTR(def_args[0]);
        n_def_args = def_pos_args->len;
        n_extra_args = def_pos_args->len;
    }
    if (def_args != NULL && def_args[1] != MP_OBJ_NULL) {
        assert(mp_obj_is_type(def_args[1], &mp_type_dict));
        def_kw_args = def_args[1];
        n_extra_args += 1;
    }
    mp_obj_fun_bc_t *o = mp_obj_malloc_var(mp_obj_fun_bc_t, extra_args, mp_obj_t, n_extra_args, &mp_type_fun_bc);
    o->bytecode = code;
    o->context = context;
    o->child_table = child_table;
    if (def_pos_args != NULL) {
        memcpy(o->extra_args, def_pos_args->items, n_def_args * sizeof(mp_obj_t));
    }
    if (def_kw_args != MP_OBJ_NULL) {
        o->extra_args[n_def_args] = def_kw_args;
    }
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
/* native functions                                                           */

#if MICROPY_EMIT_NATIVE

static mp_obj_t fun_native_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_cstack_check();
    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);
    mp_call_fun_t fun = mp_obj_fun_native_get_function_start(self);
    return fun(self_in, n_args, n_kw, args);
}

#if MICROPY_CPYTHON_COMPAT
#define FUN_BC_TYPE_PRINT print, fun_bc_print,
#else
#define FUN_BC_TYPE_PRINT
#endif
#if MICROPY_PY_FUNCTION_ATTRS
#define FUN_BC_TYPE_ATTR attr, mp_obj_fun_bc_attr,
#else
#define FUN_BC_TYPE_ATTR
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_native,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    FUN_BC_TYPE_PRINT
    FUN_BC_TYPE_ATTR
    call, fun_native_call
    );

#endif // MICROPY_EMIT_NATIVE

/******************************************************************************/
/* viper functions                                                           */

#if MICROPY_EMIT_NATIVE

static mp_obj_t fun_viper_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_cstack_check();
    mp_obj_fun_bc_t *self = MP_OBJ_TO_PTR(self_in);
    mp_call_fun_t fun = MICROPY_MAKE_POINTER_CALLABLE((void *)self->bytecode);
    return fun(self_in, n_args, n_kw, args);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_viper,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    call, fun_viper_call
    );

#endif // MICROPY_EMIT_NATIVE

/******************************************************************************/
/* inline assembler functions                                                 */

#if MICROPY_EMIT_INLINE_ASM

typedef mp_uint_t (*inline_asm_fun_0_t)(void);
typedef mp_uint_t (*inline_asm_fun_1_t)(mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_2_t)(mp_uint_t, mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_3_t)(mp_uint_t, mp_uint_t, mp_uint_t);
typedef mp_uint_t (*inline_asm_fun_4_t)(mp_uint_t, mp_uint_t, mp_uint_t, mp_uint_t);

// convert a MicroPython object to a sensible value for inline asm
static mp_uint_t convert_obj_for_inline_asm(mp_obj_t obj) {
    // TODO for byte_array, pass pointer to the array
    if (mp_obj_is_small_int(obj)) {
        return MP_OBJ_SMALL_INT_VALUE(obj);
    } else if (obj == mp_const_none) {
        return 0;
    } else if (obj == mp_const_false) {
        return 0;
    } else if (obj == mp_const_true) {
        return 1;
    } else if (mp_obj_is_exact_type(obj, &mp_type_int)) {
        return mp_obj_int_get_truncated(obj);
    } else if (mp_obj_is_str(obj)) {
        // pointer to the string (it's probably constant though!)
        size_t l;
        return (mp_uint_t)mp_obj_str_get_data(obj, &l);
    } else {
        const mp_obj_type_t *type = mp_obj_get_type(obj);
        #if MICROPY_PY_BUILTINS_FLOAT
        if (type == &mp_type_float) {
            // convert float to int (could also pass in float registers)
            return (mp_int_t)mp_obj_float_get(obj);
        }
        #endif
        if (type == &mp_type_tuple || type == &mp_type_list) {
            // pointer to start of tuple (could pass length, but then could use len(x) for that)
            size_t len;
            mp_obj_t *items;
            mp_obj_get_array(obj, &len, &items);
            return (mp_uint_t)items;
        } else {
            mp_buffer_info_t bufinfo;
            if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_READ)) {
                // supports the buffer protocol, return a pointer to the data
                return (mp_uint_t)bufinfo.buf;
            } else {
                // just pass along a pointer to the object
                return (mp_uint_t)obj;
            }
        }
    }
}

static mp_obj_t fun_asm_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_fun_asm_t *self = MP_OBJ_TO_PTR(self_in);

    mp_arg_check_num(n_args, n_kw, self->n_args, self->n_args, false);

    const void *fun = MICROPY_MAKE_POINTER_CALLABLE(self->fun_data);

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

    return mp_native_to_obj(ret, self->type_sig);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fun_asm,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    call, fun_asm_call
    );

#endif // MICROPY_EMIT_INLINE_ASM
