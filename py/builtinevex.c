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

#include <stdint.h>

#include "py/objfun.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"

#if MICROPY_PY_BUILTINS_COMPILE

typedef struct _mp_obj_code_t {
    mp_obj_base_t base;
    mp_obj_t module_fun;
} mp_obj_code_t;

STATIC const mp_obj_type_t mp_type_code = {
    { &mp_type_type },
    .name = MP_QSTR_code,
};

STATIC mp_obj_t code_execute(mp_obj_code_t *self, mp_obj_dict_t *globals, mp_obj_dict_t *locals) {
    // save context and set new context
    mp_obj_dict_t *old_globals = mp_globals_get();
    mp_obj_dict_t *old_locals = mp_locals_get();
    mp_globals_set(globals);
    mp_locals_set(locals);

    // a bit of a hack: fun_bc will re-set globals, so need to make sure it's
    // the correct one
    if (mp_obj_is_type(self->module_fun, &mp_type_fun_bc)) {
        mp_obj_fun_bc_t *fun_bc = MP_OBJ_TO_PTR(self->module_fun);
        fun_bc->globals = globals;
    }

    // execute code
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t ret = mp_call_function_0(self->module_fun);
        nlr_pop();
        mp_globals_set(old_globals);
        mp_locals_set(old_locals);
        return ret;
    } else {
        // exception; restore context and re-raise same exception
        mp_globals_set(old_globals);
        mp_locals_set(old_locals);
        nlr_jump(nlr.ret_val);
    }
}

STATIC mp_obj_t mp_builtin_compile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;

    // get the source
    size_t str_len;
    const char *str = mp_obj_str_get_data(args[0], &str_len);

    // get the filename
    qstr filename = mp_obj_str_get_qstr(args[1]);

    // create the lexer
    mp_lexer_t *lex = mp_lexer_new_from_str_len(filename, str, str_len, 0);

    // get the compile mode
    qstr mode = mp_obj_str_get_qstr(args[2]);
    mp_parse_input_kind_t parse_input_kind;
    switch (mode) {
        case MP_QSTR_single: parse_input_kind = MP_PARSE_SINGLE_INPUT; break;
        case MP_QSTR_exec: parse_input_kind = MP_PARSE_FILE_INPUT; break;
        case MP_QSTR_eval: parse_input_kind = MP_PARSE_EVAL_INPUT; break;
        default:
            mp_raise_ValueError("bad compile mode");
    }

    mp_obj_code_t *code = m_new_obj(mp_obj_code_t);
    code->base.type = &mp_type_code;
    code->module_fun = mp_parse_compile_execute(lex, parse_input_kind, NULL, NULL);
    return MP_OBJ_FROM_PTR(code);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_compile_obj, 3, 6, mp_builtin_compile);

#endif // MICROPY_PY_BUILTINS_COMPILE

#if MICROPY_PY_BUILTINS_EVAL_EXEC

STATIC mp_obj_t eval_exec_helper(size_t n_args, const mp_obj_t *args, mp_parse_input_kind_t parse_input_kind) {
    // work out the context
    mp_obj_dict_t *globals = mp_globals_get();
    mp_obj_dict_t *locals = mp_locals_get();
    for (size_t i = 1; i < 3 && i < n_args; ++i) {
        if (args[i] != mp_const_none) {
            if (!mp_obj_is_type(args[i], &mp_type_dict)) {
                mp_raise_TypeError(NULL);
            }
            locals = MP_OBJ_TO_PTR(args[i]);
            if (i == 1) {
                globals = locals;
            }
        }
    }

    #if MICROPY_PY_BUILTINS_COMPILE
    if (mp_obj_is_type(args[0], &mp_type_code)) {
        return code_execute(MP_OBJ_TO_PTR(args[0]), globals, locals);
    }
    #endif

    size_t str_len;
    const char *str = mp_obj_str_get_data(args[0], &str_len);

    // create the lexer
    // MP_PARSE_SINGLE_INPUT is used to indicate a file input
    mp_lexer_t *lex;
    if (MICROPY_PY_BUILTINS_EXECFILE && parse_input_kind == MP_PARSE_SINGLE_INPUT) {
        lex = mp_lexer_new_from_file(str);
        parse_input_kind = MP_PARSE_FILE_INPUT;
    } else {
        lex = mp_lexer_new_from_str_len(MP_QSTR__lt_string_gt_, str, str_len, 0);
    }

    return mp_parse_compile_execute(lex, parse_input_kind, globals, locals);
}

STATIC mp_obj_t mp_builtin_eval(size_t n_args, const mp_obj_t *args) {
    return eval_exec_helper(n_args, args, MP_PARSE_EVAL_INPUT);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_eval_obj, 1, 3, mp_builtin_eval);

STATIC mp_obj_t mp_builtin_exec(size_t n_args, const mp_obj_t *args) {
    return eval_exec_helper(n_args, args, MP_PARSE_FILE_INPUT);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_exec_obj, 1, 3, mp_builtin_exec);

#endif // MICROPY_PY_BUILTINS_EVAL_EXEC

#if MICROPY_PY_BUILTINS_EXECFILE
STATIC mp_obj_t mp_builtin_execfile(size_t n_args, const mp_obj_t *args) {
    // MP_PARSE_SINGLE_INPUT is used to indicate a file input
    return eval_exec_helper(n_args, args, MP_PARSE_SINGLE_INPUT);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_execfile_obj, 1, 3, mp_builtin_execfile);
#endif
