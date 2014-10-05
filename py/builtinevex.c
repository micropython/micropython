/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdint.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"
#include "lexerunix.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"
#include "builtin.h"

STATIC mp_obj_t eval_exec_helper(mp_uint_t n_args, const mp_obj_t *args, mp_parse_input_kind_t parse_input_kind) {
    mp_uint_t str_len;
    const char *str = mp_obj_str_get_data(args[0], &str_len);

    // create the lexer
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_string_gt_, str, str_len, 0);

    // work out the context
    mp_obj_dict_t *globals = mp_globals_get();
    mp_obj_dict_t *locals = mp_locals_get();
    if (n_args > 1) {
        globals = args[1];
        if (n_args > 2) {
            locals = args[2];
        } else {
            locals = globals;
        }
    }

    return mp_parse_compile_execute(lex, parse_input_kind, globals, locals);
}

STATIC mp_obj_t mp_builtin_eval(mp_uint_t n_args, const mp_obj_t *args) {
    return eval_exec_helper(n_args, args, MP_PARSE_EVAL_INPUT);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_eval_obj, 1, 3, mp_builtin_eval);

STATIC mp_obj_t mp_builtin_exec(mp_uint_t n_args, const mp_obj_t *args) {
    return eval_exec_helper(n_args, args, MP_PARSE_FILE_INPUT);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_exec_obj, 1, 3, mp_builtin_exec);
