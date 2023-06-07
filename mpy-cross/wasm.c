/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
 * Copyright (c) 2023 Jim Mussared
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

#include <emscripten.h>

#include <string.h>
#include <stdlib.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/persistentcode.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "genhdr/mpversion.h"

// This must be zero, as there is currently no handling for printing/stdout.
mp_uint_t mp_verbose_flag = 0;

// See documentation in library.js.
extern void mpycross_error(const char *buf, size_t len);

// No-op NLR. We accept a push (and do nothing). If anything attempts to jump,
// fail immediately with no cleanup. The idea is that the host code running
// the WASM binary will just handle the exception directly and then throw
// away the entire execution environment.
NORETURN void nlr_terminate(void *val) {
    vstr_t buf;
    mp_print_t pr;
    vstr_init_print(&buf, 1024, &pr);
    mp_obj_print_exception(&pr, MP_OBJ_FROM_PTR(val));

    mpycross_error(buf.buf, buf.len);

    // The host code must not attempt to resume the WASM execution after
    // handling mpycross_error.
    MP_UNREACHABLE
}

STATIC int compile_and_save(const char *input_name, const uint8_t *input_data, size_t input_len, const char *source_file, mp_print_t *pr) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(qstr_from_str(input_name), (const char *)input_data, input_len, 0);

        qstr source_name;
        if (source_file == NULL) {
            source_name = lex->source_name;
        } else {
            source_name = qstr_from_str(source_file);
        }

        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_compiled_module_t cm;
        cm.context = m_new_obj(mp_module_context_t);
        mp_compile_to_raw_code(&parse_tree, source_name, false, &cm);

        mp_raw_code_save(&cm, pr);

        nlr_pop();

        return 0;
    } else {
        // NLR is not implemented, so:
        // a) we cannot get here (as no jump is implemented)
        // b) any attempt to jump would have invoked mpycross_error already.
        MP_UNREACHABLE
    }
}

EMSCRIPTEN_KEEPALIVE
uint8_t *mpycross_malloc(size_t len) {
    return malloc(len);
}

EMSCRIPTEN_KEEPALIVE
void mpycross_free(void *ptr) {
    return free(ptr);
}

EMSCRIPTEN_KEEPALIVE
const uint8_t *mpycross_compile(const char *input_name, const uint8_t *input_data, size_t input_len, uint8_t default_emit_opt, uint8_t native_arch, uint8_t small_int_bits, size_t *output_len, int *result) {
    mp_init();

    #if MICROPY_EMIT_NATIVE
    // Set default emitter options
    MP_STATE_VM(default_emit_opt) = default_emit_opt;
    #endif

    // set default compiler configuration
    mp_dynamic_compiler.small_int_bits = small_int_bits == 0 ? 31 : small_int_bits;
    mp_dynamic_compiler.native_arch = native_arch;
    mp_dynamic_compiler.nlr_buf_num_regs = mp_nlr_num_regs_for_arch(mp_dynamic_compiler.native_arch);

    vstr_t out;
    mp_print_t pr;
    vstr_init_print(&out, 1024, &pr);

    const char *source_file = NULL;
    *result = compile_and_save(input_name, input_data, input_len, source_file, &pr);

    mp_deinit();

    *output_len = out.len;
    return (const uint8_t *)out.buf;
}

EMSCRIPTEN_KEEPALIVE
uint8_t mpycross_bytecode_version(void) {
    return MPY_VERSION;
}

EMSCRIPTEN_KEEPALIVE
uint8_t mpycross_bytecode_sub_version(void) {
    return MPY_SUB_VERSION;
}

EMSCRIPTEN_KEEPALIVE
const char *mpycross_version(size_t *len) {
    const char *version = "MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE;
    *len = strlen(version);
    return version;
}
