/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2020 Jim Mussared
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/qstr.h"

// Use a fixed static buffer for the heap.
static char heap[16384];

mp_obj_t execute_from_str(const char *str) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Use the empty string as the "filename".
        qstr src_name = MP_QSTR_;
        mp_lexer_t *lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&pt, src_name, false);
        mp_call_function_0(module_fun);
        nlr_pop();
        return 0;
    } else {
        // uncaught exception
        return (mp_obj_t)nlr.ret_val;
    }
}

void shutdown_hook(void) {
    printf("Shutdown hook called.\n");
}

int main() {
    // Configure stack limit and heap (with our static buffer).
    mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));
    gc_init(heap, heap + sizeof(heap));

    // Initialise MicroPython.
    mp_init();

    nlr_buf_t nlr;

    // Execute a print statement.
    const char str_print[] = "print('Hello, MicroPython World!')";
    if (execute_from_str(str_print)) {
        printf("Error\n");
    }

    // Set a global variable.
    const char str_set_global[] = "hello_embed_world = 22";
    if (execute_from_str(str_set_global)) {
        printf("Error\n");
    }

    // Read back the global variable (see qstrdefsembed.h for where the QSTR is defined).
    if (nlr_push(&nlr) == 0) {
        mp_obj_dict_t *globals = mp_globals_get();
        mp_obj_t v = mp_obj_dict_get(MP_OBJ_FROM_PTR(globals), MP_ROM_QSTR(MP_QSTR_hello_embed_world));
        printf("hello_embed_world = %d\n", mp_obj_get_int(v));
        nlr_pop();
    } else {
        // uncaught exception
        printf("Error\n");
    }

    // Simple exception handling.
    if (nlr_push(&nlr) == 0) {
        printf("Raising a test exception...");
        mp_raise_ValueError(MP_ERROR_TEXT("embed exception"));
        nlr_pop();
    } else {
        printf(" caught: ");
        mp_obj_t exc = (mp_obj_t)nlr.ret_val;
        mp_obj_print_exception(&mp_plat_print, exc);
    }

    // Deinitialise MicroPython.
    mp_deinit();
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}
