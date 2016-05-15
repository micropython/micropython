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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "genhdr/mpversion.h"
#include "input.h"
#include "unixport.h"


STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
    (void)env;
    ssize_t dummy = write(STDERR_FILENO, str, len);
    mp_uos_dupterm_tx_strn(str, len);
    (void)dummy;
}

const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};


// If exc is SystemExit, return value where FORCED_EXIT bit set,
// and lower 8 bits are SystemExit value. For all other exceptions,
// return 1.
int handle_uncaught_exception(mp_obj_base_t *exc) {
    // check for SystemExit
    if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(exc->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
        // None is an exit value of 0; an int is its value; anything else is 1
        mp_obj_t exit_val = mp_obj_exception_get_value(MP_OBJ_FROM_PTR(exc));
        mp_int_t val = 0;
        if (exit_val != mp_const_none && !mp_obj_get_int_maybe(exit_val, &val)) {
            val = 1;
        }
        return FORCED_EXIT | (val & 255);
    }

    // Report all other exceptions
    mp_obj_print_exception(&mp_stderr_print, MP_OBJ_FROM_PTR(exc));
    return 1;
}

// Returns standard error codes: 0 for success, 1 for all other errors,
// except if FORCED_EXIT bit is set then script raised SystemExit and the
// value of the exit is in the lower 8 bits of the return value
int execute_from_lexer(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, uint emit_opt, bool is_repl) {
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return 1;
    }

    mp_hal_set_interrupt_char(CHAR_CTRL_C);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;

        #if MICROPY_PY___FILE__
        if (input_kind == MP_PARSE_FILE_INPUT) {
            mp_store_global(MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
        }
        #endif

        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);

        /*
        printf("----------------\n");
        mp_parse_node_print(parse_tree.root, 0);
        printf("----------------\n");
        */

        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NATIVE_PYTHON, is_repl);

        
        // execute it
        mp_call_function_0(module_fun);
        // check for pending exception
        if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
            mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            nlr_raise(obj);
        }
        

        mp_hal_set_interrupt_char(-1);
        nlr_pop();
        return 0;

    } else {
        // uncaught exception
        mp_hal_set_interrupt_char(-1);
        return handle_uncaught_exception(nlr.ret_val);
    }
}

int do_file(const char *file, uint emit_opt) {
    mp_lexer_t *lex = mp_lexer_new_from_file(file);
    return execute_from_lexer(lex, MP_PARSE_FILE_INPUT, emit_opt, false);
}

int do_str(const char *str, uint emit_opt) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
    return execute_from_lexer(lex, MP_PARSE_FILE_INPUT, emit_opt, false);
}


uint mp_import_stat(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else if (S_ISREG(st.st_mode)) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
