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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "input.h"

#if MICROPY_USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

char *prompt(char *p) {
#if MICROPY_USE_READLINE
    char *line = readline(p);
    if (line) {
        add_history(line);
    }
#else
    static char buf[256];
    fputs(p, stdout);
    char *s = fgets(buf, sizeof(buf), stdin);
    if (!s) {
        return NULL;
    }
    int l = strlen(buf);
    if (buf[l - 1] == '\n') {
        buf[l - 1] = 0;
    } else {
        l++;
    }
    char *line = malloc(l);
    memcpy(line, buf, l);
#endif
    return line;
}

STATIC mp_obj_t mp_builtin_input(uint n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        mp_obj_print(args[0], PRINT_STR);
    }

    char *line = prompt("");
    if (line == NULL) {
        nlr_raise(mp_obj_new_exception(&mp_type_EOFError));
    }
    mp_obj_t o = mp_obj_new_str(line, strlen(line), false);
    free(line);
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, mp_builtin_input);
