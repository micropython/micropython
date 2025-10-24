/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_OBJFUN_H
#define MICROPY_INCLUDED_PY_OBJFUN_H

#include "py/bc.h"
#include "py/obj.h"

// There is no common structure mp_obj_fun_t valid for all kinds of function objects,
// each code emitter may use a specific structure. Therefore, any 'generic' function
// working on function objects should check the function type before accessing internal
// fields

#if MICROPY_PY_SYS_SETTRACE
// As sys.settrace features requires an extra pointer to the mp_raw_code_t,
// avoid duplicating the pointer to bytecode and child_nodes to prevent
// growing mp_obj_fun_bc above one GC unit

typedef struct _mp_obj_fun_bc_t {
    mp_obj_base_t base;
    const mp_module_context_t *context;         // context within which this function was defined
    const struct _mp_raw_code_t *rc;
    // the following extra_args array is allocated space to take (in order):
    //  - values of positional default args (if any)
    //  - a single slot for default kw args dict (if it has them)
    mp_obj_t extra_args[];
} mp_obj_fun_bc_t;
#define MP_FUN_BC_GET_BYTECODE(fun_bc) ((fun_bc)->rc->fun_data)
#define MP_FUN_BC_GET_CHILDREN(fun_bc) ((fun_bc)->rc->children)

#else

typedef struct _mp_obj_fun_bc_t {
    mp_obj_base_t base;
    const mp_module_context_t *context;         // context within which this function was defined
    struct _mp_raw_code_t *const *child_table;  // table of children
    const byte *bytecode;                       // bytecode for the function
    // the following extra_args array is allocated space to take (in order):
    //  - values of positional default args (if any)
    //  - a single slot for default kw args dict (if it has them)
    mp_obj_t extra_args[];
} mp_obj_fun_bc_t;
#define MP_FUN_BC_GET_BYTECODE(fun_bc) ((fun_bc)->bytecode)
#define MP_FUN_BC_GET_CHILDREN(fun_bc) ((fun_bc)->child_table)

#endif

typedef struct _mp_obj_fun_native_t {
    mp_obj_base_t base;
    const mp_module_context_t *context;         // context within which this function was defined
    struct _mp_raw_code_t *const *child_table;  // table of children
    const byte *bytecode;                       // bytecode for the function
    // the following extra_args array is allocated space to take (in order):
    //  - values of positional default args (if any)
    //  - a single slot for default kw args dict (if it has them)
    mp_obj_t extra_args[];
} mp_obj_fun_native_t;

typedef struct _mp_obj_fun_viper_t {
    mp_obj_base_t base;
    const mp_module_context_t *context;         // context within which this function was defined
    struct _mp_raw_code_t *const *child_table;  // table of children
    const byte *bytecode;                       // bytecode for the function
} mp_obj_fun_viper_t;

typedef struct _mp_obj_fun_asm_t {
    mp_obj_base_t base;
    size_t n_args;
    const void *fun_data; // GC must be able to trace this pointer
    mp_uint_t type_sig;
} mp_obj_fun_asm_t;

// The following functions can be used for BYTECODE or NATIVE_PYTHON functions
mp_obj_t mp_obj_new_fun_py(const mp_obj_t *def_args, const byte *code, const mp_module_context_t *cm, struct _mp_raw_code_t *const *raw_code_table, const mp_obj_type_t *type);
qstr mp_obj_fun_get_name(mp_const_obj_t fun);
void mp_obj_fun_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);

#if MICROPY_EMIT_NATIVE

static inline mp_obj_t mp_obj_new_fun_viper(const void *fun_data, const mp_module_context_t *mc, struct _mp_raw_code_t *const *child_table) {
    mp_obj_fun_viper_t *o = mp_obj_malloc(mp_obj_fun_viper_t, &mp_type_fun_viper);
    o->bytecode = (const byte *)fun_data;
    o->context = mc;
    o->child_table = child_table;
    return MP_OBJ_FROM_PTR(o);
}

static inline const uint8_t *mp_obj_fun_native_get_prelude_ptr(const mp_obj_fun_native_t *fun_native) {
    // Obtain a pointer to the start of the function prelude, based on prelude_ptr_index.
    uintptr_t prelude_ptr_index = ((uintptr_t *)fun_native->bytecode)[0];
    const uint8_t *prelude_ptr;
    if (prelude_ptr_index == 0) {
        prelude_ptr = (const uint8_t *)fun_native->child_table;
    } else {
        prelude_ptr = (const uint8_t *)fun_native->child_table[prelude_ptr_index];
    }
    return prelude_ptr;
}

static inline void *mp_obj_fun_native_get_function_start(const mp_obj_fun_native_t *fun_native) {
    // Obtain a pointer to the start of the function executable machine code.
    return MICROPY_MAKE_POINTER_CALLABLE((void *)(fun_native->bytecode + sizeof(uintptr_t)));
}

static inline void *mp_obj_fun_native_get_generator_start(const mp_obj_fun_native_t *fun_native) {
    // Obtain a pointer to the start of the generator executable machine code.
    uintptr_t start_offset = ((uintptr_t *)fun_native->bytecode)[1];
    return MICROPY_MAKE_POINTER_CALLABLE((void *)(fun_native->bytecode + start_offset));
}

static inline void *mp_obj_fun_native_get_generator_resume(const mp_obj_fun_native_t *fun_native) {
    // Obtain a pointer to the resume location of the generator executable machine code.
    return MICROPY_MAKE_POINTER_CALLABLE((void *)&((uintptr_t *)fun_native->bytecode)[2]);
}

#endif

#if MICROPY_EMIT_INLINE_ASM
static inline mp_obj_t mp_obj_new_fun_asm(size_t n_args, const void *fun_data, mp_uint_t type_sig) {
    mp_obj_fun_asm_t *o = (mp_obj_fun_asm_t *)mp_obj_malloc(mp_obj_fun_asm_t, &mp_type_fun_asm);
    o->n_args = n_args;
    o->fun_data = (const byte *)fun_data;
    o->type_sig = type_sig;
    return MP_OBJ_FROM_PTR(o);
}
#endif

#endif // MICROPY_INCLUDED_PY_OBJFUN_H
