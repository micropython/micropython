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
#include "py/mphal.h"
#include "py/runtime0.h"
#include "py/bc.h"
#include "py/objfun.h"
#include "py/profile.h"

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
    mp_raw_code_t **children,
    #if MICROPY_PERSISTENT_CODE_SAVE
    size_t len,
    uint16_t n_children,
    #endif
    uint16_t scope_flags) {

    rc->kind = MP_CODE_BYTECODE;
    rc->is_generator = (scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0;
    rc->fun_data = code;
    rc->children = children;

    #if MICROPY_PERSISTENT_CODE_SAVE
    #if MICROPY_LIMIT_RAWCODE_SIZE
    assert(len < 65536);
    #endif
    rc->fun_data_len = len;
    rc->n_children = n_children;
    #endif

    #if MICROPY_PY_SYS_SETTRACE
    size_t makefun_lineno = (size_t)rc->specific.bytecode.line_info;
    mp_prof_settrace_data_t *settrace_data = &rc->specific.bytecode;
    mp_prof_extract_prelude(code, settrace_data);
    if (makefun_lineno) {
        uint first_stmt_lineno = mp_prof_bytecode_lineno(rc, 0);
        uint delta_line = first_stmt_lineno - makefun_lineno;
        settrace_data->line_of_definition_delta = (delta_line < 256 ? delta_line : 0);
    }
    #endif

    #if DEBUG_PRINT
    #if !MICROPY_PERSISTENT_CODE_SAVE
    const size_t len = 0;
    #endif
    DEBUG_printf("assign byte code: code=%p len=" UINT_FMT " flags=%x\n", code, len, (uint)scope_flags);
    #endif
}

#if MICROPY_EMIT_MACHINE_CODE
void mp_emit_glue_assign_native(mp_raw_code_t *rc, mp_raw_code_kind_t kind, const void *fun_data, mp_uint_t fun_len,
    mp_raw_code_t **children,
    #if MICROPY_PERSISTENT_CODE_SAVE
    uint16_t n_children,
    uint16_t prelude_offset,
    #endif
    uint16_t scope_flags, uint32_t asm_n_pos_args, uint32_t asm_type_sig
    ) {

    assert(kind == MP_CODE_NATIVE_PY || kind == MP_CODE_NATIVE_VIPER || kind == MP_CODE_NATIVE_ASM);

    // Some architectures require flushing/invalidation of the I/D caches,
    // so that the generated native code which was created in data RAM will
    // be available for execution from instruction RAM.
    #if MICROPY_EMIT_THUMB || MICROPY_EMIT_INLINE_THUMB
    #if __ICACHE_PRESENT == 1
    // Flush D-cache, so the code emitted is stored in RAM.
    MP_HAL_CLEAN_DCACHE(fun_data, fun_len);
    // Invalidate I-cache, so the newly-created code is reloaded from RAM.
    SCB_InvalidateICache();
    #endif
    #elif MICROPY_EMIT_ARM
    #if (defined(__linux__) && defined(__GNUC__)) || __ARM_ARCH == 7
    __builtin___clear_cache((void *)fun_data, (char *)fun_data + fun_len);
    #elif defined(__arm__)
    // Flush I-cache and D-cache.
    asm volatile (
        "0:"
        "mrc p15, 0, r15, c7, c10, 3\n" // test and clean D-cache
        "bne 0b\n"
        "mov r0, #0\n"
        "mcr p15, 0, r0, c7, c7, 0\n" // invalidate I-cache and D-cache
        : : : "r0", "cc");
    #endif
    #elif (MICROPY_EMIT_RV32 || MICROPY_EMIT_INLINE_RV32) && defined(MP_HAL_CLEAN_DCACHE)
    // Flush the D-cache.
    MP_HAL_CLEAN_DCACHE(fun_data, fun_len);
    #endif

    rc->kind = kind;
    rc->is_generator = (scope_flags & MP_SCOPE_FLAG_GENERATOR) != 0;
    rc->fun_data = fun_data;

    #if MICROPY_PERSISTENT_CODE_SAVE
    #if MICROPY_LIMIT_RAWCODE_SIZE
    assert(fun_len < 65536);
    #endif
    rc->fun_data_len = fun_len;
    #endif
    rc->children = children;

    #if MICROPY_PERSISTENT_CODE_SAVE
    rc->n_children = n_children;
    if (kind == MP_CODE_NATIVE_PY) {
        rc->specific.native_py.prelude_offset = prelude_offset;
    #if MICROPY_EMIT_INLINE_ASM
    } else if (kind == MP_CODE_NATIVE_ASM) {
        rc->specific.native_asm.n_pos_args = asm_n_pos_args;
        rc->specific.native_asm.type_sig = asm_type_sig;
    #endif
    }
    #elif MICROPY_EMIT_INLINE_ASM
    // MICROPY_PERSISTENT_CODE_SAVE is false: in this specific case
    // the .specific field is not an union, so we don't need to
    // check the code kind before assignment (to keep code small)
    rc->specific.native_asm.n_pos_args = asm_n_pos_args;
    rc->specific.native_asm.type_sig = asm_type_sig;
    #endif

    #if DEBUG_PRINT
    DEBUG_printf("assign native: kind=%d fun=%p len=" UINT_FMT " flags=%x\n", kind, fun_data, fun_len, (uint)scope_flags);
    for (mp_uint_t i = 0; i < fun_len; i++) {
        if (i > 0 && i % 16 == 0) {
            DEBUG_printf("\n");
        }
        DEBUG_printf(" %02x", ((const byte *)fun_data)[i]);
    }
    DEBUG_printf("\n");

    #if WRITE_CODE
    FILE *fp_write_code = fopen("out-code", "wb");
    fwrite(fun_data, fun_len, 1, fp_write_code);
    fclose(fp_write_code);
    #endif
    #else
    (void)fun_len;
    #endif
}
#endif

mp_obj_t mp_make_function_from_proto_fun(mp_proto_fun_t proto_fun, const mp_module_context_t *context, const mp_obj_t *def_args) {
    DEBUG_OP_printf("make_function_from_proto_fun %p\n", proto_fun);
    assert(proto_fun != NULL);

    // def_args must be MP_OBJ_NULL or a tuple
    assert(def_args == NULL || def_args[0] == MP_OBJ_NULL || mp_obj_is_type(def_args[0], &mp_type_tuple));

    // def_kw_args must be MP_OBJ_NULL or a dict
    assert(def_args == NULL || def_args[1] == MP_OBJ_NULL || mp_obj_is_type(def_args[1], &mp_type_dict));

    #if MICROPY_MODULE_FROZEN_MPY && !MICROPY_PERSISTENT_CODE_SAVE && !MICROPY_MODULE_FROZEN_MPY_FREEZE_FUN_BC
    if (mp_proto_fun_is_bytecode(proto_fun)) {
        const uint8_t *bc = proto_fun;
        MP_BC_PRELUDE_SIG_DECODE(bc);
        const mp_obj_type_t *bc_type = (scope_flags & MP_SCOPE_FLAG_GENERATOR ? &mp_type_gen_wrap : &mp_type_fun_bc);
        return mp_obj_new_fun_py(def_args, proto_fun, context, NULL, bc_type);
    }
    #else
    assert(!mp_proto_fun_is_bytecode(proto_fun));
    #endif

    // the proto-function is a mp_raw_code_t
    const mp_raw_code_t *rc = proto_fun;
    #if MICROPY_MODULE_FROZEN_MPY_FREEZE_FUN_BC
    if (rc->has_const_fun_obj) {
        // we have a ready-to-use frozen function object
        const mp_obj_fun_bc_t *fun_bc = (void *)((mp_raw_code_truncated_t *)rc)->fun_obj;
        return MP_OBJ_FROM_PTR(fun_bc);
    }
    #endif

    // make the function, depending on the raw code kind
    mp_obj_t fun;
    switch (rc->kind) {
        #if MICROPY_EMIT_NATIVE
        case MP_CODE_NATIVE_PY:
            ;
            const mp_obj_type_t *native_type = (rc->is_generator ? &mp_type_native_gen_wrap : &mp_type_fun_native);
            fun = mp_obj_new_fun_py(def_args, rc->fun_data, context, rc->children, native_type);
            break;
        case MP_CODE_NATIVE_VIPER:
            fun = mp_obj_new_fun_viper(rc->fun_data, context, rc->children);
            break;
        #endif
        #if MICROPY_EMIT_INLINE_ASM
        case MP_CODE_NATIVE_ASM:
            fun = mp_obj_new_fun_asm(rc->specific.native_asm.n_pos_args, rc->fun_data, rc->specific.native_asm.type_sig);
            break;
        #endif
        default:
            // rc->kind should always be set and BYTECODE is the only remaining case
            assert(rc->kind == MP_CODE_BYTECODE);
            const mp_obj_type_t *bc_type = (rc->is_generator ? &mp_type_gen_wrap : &mp_type_fun_bc);
            fun = mp_obj_new_fun_py(def_args, rc->fun_data, context, rc->children, bc_type);

            #if MICROPY_PY_SYS_SETTRACE
            mp_obj_fun_bc_t *self_fun = (mp_obj_fun_bc_t *)MP_OBJ_TO_PTR(fun);
            self_fun->rc = rc;
            #endif

            break;
    }

    return fun;
}

mp_obj_t mp_make_closure_from_proto_fun(mp_proto_fun_t proto_fun, const mp_module_context_t *context, mp_uint_t n_closed_over, const mp_obj_t *args) {
    DEBUG_OP_printf("make_closure_from_proto_fun %p " UINT_FMT " %p\n", proto_fun, n_closed_over, args);
    // make function object
    mp_obj_t ffun;
    if (n_closed_over & 0x100) {
        // default positional and keyword args given
        ffun = mp_make_function_from_proto_fun(proto_fun, context, args);
    } else {
        // default positional and keyword args not given
        ffun = mp_make_function_from_proto_fun(proto_fun, context, NULL);
    }
    // wrap function in closure object
    return mp_obj_new_closure(ffun, n_closed_over & 0xff, args + ((n_closed_over >> 7) & 2));
}
