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
#ifndef MICROPY_INCLUDED_PY_EMITGLUE_H
#define MICROPY_INCLUDED_PY_EMITGLUE_H

#include "py/obj.h"
#include "py/bc.h"

// These variables and functions glue the code emitters to the runtime.

// Used with mp_raw_code_t::proto_fun_indicator to detect if a mp_proto_fun_t is a
// mp_raw_code_t struct or a direct pointer to bytecode.
#define MP_PROTO_FUN_INDICATOR_RAW_CODE_0 (0)
#define MP_PROTO_FUN_INDICATOR_RAW_CODE_1 (0)

// These must fit in 8 bits; see scope.h
enum {
    MP_EMIT_OPT_NONE,
    MP_EMIT_OPT_BYTECODE,
    MP_EMIT_OPT_NATIVE_PYTHON,
    MP_EMIT_OPT_VIPER,
    MP_EMIT_OPT_ASM,
};

typedef enum {
    MP_CODE_UNUSED,
    MP_CODE_RESERVED,
    MP_CODE_BYTECODE,
    MP_CODE_NATIVE_PY,
    MP_CODE_NATIVE_VIPER,
    MP_CODE_NATIVE_ASM,
} mp_raw_code_kind_t;

// An mp_proto_fun_t points to static information about a non-instantiated function.
// A function object is created from this information, and that object can then be executed.
// It points either to bytecode, or an mp_raw_code_t struct.
typedef const void *mp_proto_fun_t;

// Bytecode is distinguished from an mp_raw_code_t struct by the first two bytes: bytecode
// is guaranteed to have either its first or second byte non-zero.  So if both bytes are
// zero then the mp_proto_fun_t pointer must be an mp_raw_code_t.
static inline bool mp_proto_fun_is_bytecode(mp_proto_fun_t proto_fun) {
    const uint8_t *header = (const uint8_t *)proto_fun;
    return (header[0] | (header[1] << 8)) != (MP_PROTO_FUN_INDICATOR_RAW_CODE_0 | (MP_PROTO_FUN_INDICATOR_RAW_CODE_1 << 8));
}

// The mp_raw_code_t struct appears in the following places:
// compiled bytecode: instance in RAM, referenced by outer scope, usually freed after first (and only) use
// mpy file: instance in RAM, created when .mpy file is loaded (same comments as above)
// frozen: instance in ROM
typedef struct _mp_raw_code_t {
    uint8_t proto_fun_indicator[2];
    uint8_t kind; // of type mp_raw_code_kind_t; only 3 bits used
    bool is_generator;
    const void *fun_data;
    struct _mp_raw_code_t **children;
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint32_t fun_data_len; // for mp_raw_code_save
    uint16_t n_children;
    #if MICROPY_EMIT_MACHINE_CODE
    uint16_t prelude_offset;
    #endif
    #if MICROPY_PY_SYS_SETTRACE
    // line_of_definition is a Python source line where the raw_code was
    // created e.g. MP_BC_MAKE_FUNCTION. This is different from lineno info
    // stored in prelude, which provides line number for first statement of
    // a function. Required to properly implement "call" trace event.
    uint32_t line_of_definition;
    mp_bytecode_prelude_t prelude;
    #endif
    #endif
    #if MICROPY_EMIT_INLINE_ASM
    uint32_t asm_n_pos_args : 8;
    uint32_t asm_type_sig : 24; // compressed as 2-bit types; ret is MSB, then arg0, arg1, etc
    #endif
} mp_raw_code_t;

// Version of mp_raw_code_t but without the asm_n_pos_args/asm_type_sig entries, which are
// only needed when the kind is MP_CODE_NATIVE_ASM.  So this struct can be used when the
// kind is MP_CODE_BYTECODE, MP_CODE_NATIVE_PY or MP_CODE_NATIVE_VIPER, to reduce its size.
typedef struct _mp_raw_code_truncated_t {
    uint8_t proto_fun_indicator[2];
    uint8_t kind;
    bool is_generator;
    const void *fun_data;
    struct _mp_raw_code_t **children;
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint32_t fun_data_len;
    uint16_t n_children;
    #if MICROPY_EMIT_MACHINE_CODE
    uint16_t prelude_offset;
    #endif
    #if MICROPY_PY_SYS_SETTRACE
    uint32_t line_of_definition;
    mp_bytecode_prelude_t prelude;
    #endif
    #endif
} mp_raw_code_truncated_t;

mp_raw_code_t *mp_emit_glue_new_raw_code(void);

void mp_emit_glue_assign_bytecode(mp_raw_code_t *rc, const byte *code,
    mp_raw_code_t **children,
    #if MICROPY_PERSISTENT_CODE_SAVE
    size_t len,
    uint16_t n_children,
    #endif
    uint16_t scope_flags);

void mp_emit_glue_assign_native(mp_raw_code_t *rc, mp_raw_code_kind_t kind, const void *fun_data, mp_uint_t fun_len,
    mp_raw_code_t **children,
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint16_t n_children,
    uint16_t prelude_offset,
    #endif
    uint16_t scope_flags, uint32_t asm_n_pos_args, uint32_t asm_type_sig);

mp_obj_t mp_make_function_from_proto_fun(mp_proto_fun_t proto_fun, const mp_module_context_t *context, const mp_obj_t *def_args);
mp_obj_t mp_make_closure_from_proto_fun(mp_proto_fun_t proto_fun, const mp_module_context_t *context, mp_uint_t n_closed_over, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_PY_EMITGLUE_H
