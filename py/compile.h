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
#ifndef MICROPY_INCLUDED_PY_COMPILE_H
#define MICROPY_INCLUDED_PY_COMPILE_H

#include "py/lexer.h"
#include "py/parse.h"
#include "py/emitglue.h"

// These must fit in 8 bits; see scope.h
enum {
    MP_EMIT_OPT_NONE,
    MP_EMIT_OPT_BYTECODE,
    MP_EMIT_OPT_NATIVE_PYTHON,
    MP_EMIT_OPT_VIPER,
    MP_EMIT_OPT_ASM,
};

// the compiler will raise an exception if an error occurred
// the compiler will clear the parse tree before it returns
mp_obj_t mp_compile(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl);

#if MICROPY_PERSISTENT_CODE_SAVE
// this has the same semantics as mp_compile
mp_raw_code_t *mp_compile_to_raw_code(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl);
#endif

// this is implemented in runtime.c
mp_obj_t mp_parse_compile_execute(mp_lexer_t *lex, mp_parse_input_kind_t parse_input_kind, mp_obj_dict_t *globals, mp_obj_dict_t *locals);

#endif // MICROPY_INCLUDED_PY_COMPILE_H
