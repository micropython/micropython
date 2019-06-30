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

// This code glues the code emitters to the runtime.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/emitglue.h"
#include "py/runtime0.h"
#include "py/bc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define WRITE_CODE (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

#if MICROPY_DEBUG_PRINTERS
mp_uint_t mp_verbose_flag = 0;
#endif

mp_raw_code_t *mp_emit_glue_new_raw_code(void) {
    mp_raw_code_t *rc = m_new0(mp_raw_code_t, 1);
    rc->kind = MP_CODE_RESERVED;
    return rc;
}

void mp_emit_glue_assign_bytecode(mp_raw_code_t *rc, const byte *code,
    #if MICROPY_PERSISTENT_CODE_SAVE || MICROPY_DEBUG_PRINTERS
    size_t len,
    #endif
    const mp_uint_t *const_table,
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint16_t n_obj, uint16_t n_raw_code,
    #endif
    mp_uint_t scope_flags) {

    rc->kind = MP_CODE_BYTECODE;
    rc->scope_flags = scope_flags;
    rc->fun_data = code;
    rc->const_table = const_table;
    #if MICROPY_PERSISTENT_CODE_SAVE
    rc->fun_data_len = len;
    rc->n_obj = n_obj;
    rc->n_raw_code = n_raw_code;
    #endif

#ifdef DEBUG_PRINT
    #if !MICROPY_DEBUG_PRINTERS
    const size_t len = 0;
    #endif
    DEBUG_printf("assign byte code: code=%p len=" UINT_FMT " flags=%x\n", code, len, (uint)scope_flags);
#endif
#if MICROPY_DEBUG_PRINTERS
    if (mp_verbose_flag >= 2) {
        mp_bytecode_print(rc, code, len, const_table);
    }
#endif
}

#if MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM
void mp_emit_glue_assign_native(mp_raw_code_t *rc, mp_raw_code_kind_t kind, void *fun_data, mp_uint_t fun_len, const mp_uint_t *const_table,
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint16_t prelude_offset,
    uint16_t n_obj, uint16_t n_raw_code,
    uint16_t n_qstr, mp_qstr_link_entry_t *qstr_link,
    #endif
    mp_uint_t n_pos_args, mp_uint_t scope_flags, mp_uint_t type_sig) {

    assert(kind == MP_CODE_NATIVE_PY || kind == MP_CODE_NATIVE_VIPER || kind == MP_CODE_NATIVE_ASM);

    rc->kind = kind;
    rc->scope_flags = scope_flags;
    rc->n_pos_args = n_pos_args;
    rc->fun_data = fun_data;
    rc->const_table = const_table;
    rc->type_sig = type_sig;

    #if MICROPY_PERSISTENT_CODE_SAVE
    rc->fun_data_len = fun_len;
    rc->prelude_offset = prelude_offset;
    rc->n_obj = n_obj;
    rc->n_raw_code = n_raw_code;
    rc->n_qstr= n_qstr;
    rc->qstr_link = qstr_link;
    #endif

#ifdef DEBUG_PRINT
    DEBUG_printf("assign native: kind=%d fun=%p len=" UINT_FMT " n_pos_args=" UINT_FMT " flags=%x\n", kind, fun_data, fun_len, n_pos_args, (uint)scope_flags);
    for (mp_uint_t i = 0; i < fun_len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", ((byte*)fun_data)[i]);
    }
    DEBUG_printf("\n");

#ifdef WRITE_CODE
    FILE *fp_write_code = fopen("out-code", "wb");
    fwrite(fun_data, fun_len, 1, fp_write_code);
    fclose(fp_write_code);
#endif
#else
    (void)fun_len;
#endif
}
#endif

mp_obj_t mp_make_function_from_raw_code(const mp_raw_code_t *rc, mp_obj_t def_args, mp_obj_t def_kw_args) {
    DEBUG_OP_printf("make_function_from_raw_code %p\n", rc);
    assert(rc != NULL);

    // def_args must be MP_OBJ_NULL or a tuple
    assert(def_args == MP_OBJ_NULL || mp_obj_is_type(def_args, &mp_type_tuple));

    // def_kw_args must be MP_OBJ_NULL or a dict
    assert(def_kw_args == MP_OBJ_NULL || mp_obj_is_type(def_kw_args, &mp_type_dict));

    // make the function, depending on the raw code kind
    mp_obj_t fun;
    switch (rc->kind) {
        #if MICROPY_EMIT_NATIVE
        case MP_CODE_NATIVE_PY:
        case MP_CODE_NATIVE_VIPER:
            fun = mp_obj_new_fun_native(def_args, def_kw_args, rc->fun_data, rc->const_table);
            // Check for a generator function, and if so change the type of the object
            if ((rc->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
                ((mp_obj_base_t*)MP_OBJ_TO_PTR(fun))->type = &mp_type_native_gen_wrap;
            }
            break;
        #endif
        #if MICROPY_EMIT_INLINE_ASM
        case MP_CODE_NATIVE_ASM:
            fun = mp_obj_new_fun_asm(rc->n_pos_args, rc->fun_data, rc->type_sig);
            break;
        #endif
        default:
            // rc->kind should always be set and BYTECODE is the only remaining case
            assert(rc->kind == MP_CODE_BYTECODE);
            fun = mp_obj_new_fun_bc(def_args, def_kw_args, rc->fun_data, rc->const_table);
            // check for generator functions and if so change the type of the object
            if ((rc->scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0) {
                ((mp_obj_base_t*)MP_OBJ_TO_PTR(fun))->type = &mp_type_gen_wrap;
            }
            break;
    }

    return fun;
}

mp_obj_t mp_make_closure_from_raw_code(const mp_raw_code_t *rc, mp_uint_t n_closed_over, const mp_obj_t *args) {
    DEBUG_OP_printf("make_closure_from_raw_code %p " UINT_FMT " %p\n", rc, n_closed_over, args);
    // make function object
    mp_obj_t ffun;
    if (n_closed_over & 0x100) {
        // default positional and keyword args given
        ffun = mp_make_function_from_raw_code(rc, args[0], args[1]);
    } else {
        // default positional and keyword args not given
        ffun = mp_make_function_from_raw_code(rc, MP_OBJ_NULL, MP_OBJ_NULL);
    }
    // wrap function in closure object
    return mp_obj_new_closure(ffun, n_closed_over & 0xff, args + ((n_closed_over >> 7) & 2));
}
