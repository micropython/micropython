/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Jeff Epler
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

#include "py/runtime.h"

#if MICROPY_PY_MPYCROSS

#include "py/builtin.h"
#include "py/compile.h"
#include "py/persistentcode.h"

static bool parse_arch(qstr arch) {
    #if MICROPY_EMIT_X86
    if (arch == MP_QSTR_x86) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X86;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_X86;
        return true;
    }
    #endif
    #if MICROPY_EMIT_X64
    if (arch == MP_QSTR_x64) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X64;
        mp_dynamic_compiler.nlr_buf_num_regs = MAX(MICROPY_NLR_NUM_REGS_X64, MICROPY_NLR_NUM_REGS_X64_WIN);
        return true;
    }
    #endif
    #if MICROPY_EMIT_INLINE_THUMB_FLOAT
    if (arch == MP_QSTR_armv6) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
    }
    if (arch == MP_QSTR_armv6m) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6M;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP; // need to be conservative so this code can run on armv7emdp
        return true;
    }
    #endif
    #if MICROPY_EMIT_THUMB_ARMV7M
    if (arch == MP_QSTR_armv7m) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7M;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
    }
    if (arch == MP_QSTR_armv7em) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EM;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
    }
    if (arch == MP_QSTR_armv7emsp) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EMSP;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
    }
    if (arch == MP_QSTR_armv7emdp) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV7EMDP;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
    }
    #endif
    #if MICROPY_EMIT_XTENSA
    if (arch == MP_QSTR_xtensa) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_XTENSA;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_XTENSA;
        return true;
    }
    #endif
    #if MICROPY_EMIT_XTENSAWIN
    if (arch == MP_QSTR_xtensawin) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_XTENSAWIN;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_XTENSAWIN;
        return true;
    }
    #endif
    #if MICROPY_EMIT_RV32
    if (arch == MP_QSTR_rv32imc) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_RV32IMC;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_RV32I;
        return true;
    }
    #endif
    #if MICROPY_EMIT_NATIVE_DEBUG
    if (arch == MP_QSTR_debug) {
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_DEBUG;
        mp_dynamic_compiler.nlr_buf_num_regs = 0;
        return true;
    }
    #endif
    if (arch == MP_QSTR_host) {
        #if defined(__i386__) || defined(_M_IX86)
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X86;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_X86;
        return true;
        #elif defined(__x86_64__) || defined(_M_X64)
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_X64;
        mp_dynamic_compiler.nlr_buf_num_regs = MAX(MICROPY_NLR_NUM_REGS_X64, MICROPY_NLR_NUM_REGS_X64_WIN);
        return true;
        #elif defined(__arm__) && !defined(__thumb2__)
        mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_ARMV6;
        mp_dynamic_compiler.nlr_buf_num_regs = MICROPY_NLR_NUM_REGS_ARM_THUMB_FP;
        return true;
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("unable to determine host architecture for emit_arch=\"host\"\n"));
        #endif
    }
    return false;
}

static mp_obj_t mpy_compile(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_source, ARG_filename,
           ARG_emit_arch, ARG_small_int_bits, ARG_nlr_buf_num_regs,
           #if MICROPY_EMIT_NATIVE
           ARG_emit_opt,
           #endif
    };
    mp_arg_t allowed_args[] = {
        {MP_QSTR_source, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE}},
        {MP_QSTR_filename, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE}},
        {MP_QSTR_emit_arch, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE}},
        {MP_QSTR_small_int_bits, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 31}},
        {MP_QSTR_nlr_buf_num_regs, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1}},
        #if MICROPY_EMIT_NATIVE
        {MP_QSTR_emit_opt, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = MP_EMIT_OPT_NONE}},
        #endif
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    struct mp_dynamic_compiler_t restore_compiler = mp_dynamic_compiler;
    #if MICROPY_EMIT_NATIVE
    uint restore_emit_opt = MP_STATE_VM(default_emit_opt);
    #endif

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_dynamic_compiler.small_int_bits = args[ARG_small_int_bits].u_int;

        if (args[ARG_emit_arch].u_obj == MP_ROM_NONE) {
            mp_dynamic_compiler.native_arch = MP_NATIVE_ARCH_NONE;
        } else {

            if (!parse_arch(mp_obj_str_get_qstr(args[ARG_emit_arch].u_obj))) {
                mp_raise_ValueError(MP_ERROR_TEXT("Invalid emit_arch"));
            }
        }

        if (args[ARG_nlr_buf_num_regs].u_int >= 0) {
            mp_dynamic_compiler.nlr_buf_num_regs = args[ARG_nlr_buf_num_regs].u_int;
        }

        #if MICROPY_EMIT_NATIVE
        // Set default emitter options
        if (args[ARG_emit_opt].u_int < 0 || args[ARG_emit_opt].u_int > MP_EMIT_OPT_ASM) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid emit_opt"));
        }
        MP_STATE_VM(default_emit_opt) = args[ARG_emit_opt].u_int;

        if ((MP_STATE_VM(default_emit_opt) == MP_EMIT_OPT_NATIVE_PYTHON
             || MP_STATE_VM(default_emit_opt) == MP_EMIT_OPT_VIPER)
            && mp_dynamic_compiler.native_arch == MP_NATIVE_ARCH_NONE) {
            mp_raise_ValueError(MP_ERROR_TEXT("arch not specified"));
        }
        #endif

        size_t str_len;
        const char *str = mp_obj_str_get_data(args[ARG_source].u_obj, &str_len);

        // get the filename
        qstr filename = mp_obj_str_get_qstr(args[ARG_filename].u_obj);

        // create the lexer
        mp_lexer_t *lex = mp_lexer_new_from_str_len(filename, str, str_len, 0);

        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_module_context_t ctx;
        ctx.module.globals = NULL;
        mp_compiled_module_t cm;
        cm.context = &ctx;
        mp_compile_to_raw_code(&parse_tree, lex->source_name, false, &cm);

        vstr_t vstr;
        vstr_init(&vstr, 64);
        mp_print_t print = {&vstr, (mp_print_strn_t)vstr_add_strn};
        mp_raw_code_save(&cm, &print);

        mp_obj_t result = mp_obj_new_bytes_from_vstr(&vstr);

        nlr_pop();
        mp_dynamic_compiler = restore_compiler;
        #if MICROPY_EMIT_NATIVE
        MP_STATE_VM(default_emit_opt) = restore_emit_opt;
        #endif

        return result;
    } else {
        mp_dynamic_compiler = restore_compiler;
        #if MICROPY_EMIT_NATIVE
        MP_STATE_VM(default_emit_opt) = restore_emit_opt;
        #endif

        nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mpy_compile_obj, 1, mpy_compile);

static const mp_rom_map_elem_t mp_module_mpycross_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mpycross) },
    { MP_ROM_QSTR(MP_QSTR_compile), MP_ROM_PTR(&mpy_compile_obj) },
    { MP_ROM_QSTR(MP_QSTR_EMIT_OPT_NONE), MP_ROM_INT(MP_EMIT_OPT_NONE) },
    { MP_ROM_QSTR(MP_QSTR_EMIT_OPT_BYTECODE), MP_ROM_INT(MP_EMIT_OPT_BYTECODE) },
    #if MICROPY_EMIT_NATIVE
    { MP_ROM_QSTR(MP_QSTR_EMIT_OPT_NATIVE_PYTHON), MP_ROM_INT(MP_EMIT_OPT_NATIVE_PYTHON) },
    { MP_ROM_QSTR(MP_QSTR_EMIT_OPT_VIPER), MP_ROM_INT(MP_EMIT_OPT_VIPER) },
    { MP_ROM_QSTR(MP_QSTR_EMIT_OPT_ASM), MP_ROM_INT(MP_EMIT_OPT_ASM) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_mpycross_globals, mp_module_mpycross_globals_table);
const mp_obj_module_t mp_module_mpycross = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_mpycross_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mpycross, mp_module_mpycross);
#endif
