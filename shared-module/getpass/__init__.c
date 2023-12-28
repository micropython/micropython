/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "py/mphal.h"
#include "shared/readline/readline.h"
#include "shared-module/getpass/__init__.h"

mp_obj_t shared_module_getpass_getpass(const char *prompt, mp_print_t *print) {
    vstr_t vstr;
    vstr_init(&vstr, 16);

    if (print == NULL) {
        mp_hal_stdout_tx_str(prompt);
    } else {
        mp_printf(print, prompt);
    }

    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (c == CHAR_CTRL_C) {
            mp_raise_type(&mp_type_KeyboardInterrupt);
        } else if (c == CHAR_CTRL_D && vstr.len == 0) {
            mp_raise_type(&mp_type_EOFError);
        } else if (c == 8 || c == 127) {
            // backspace
            vstr_cut_tail_bytes(&vstr, 1);
        } else if (c >= 32) {
            // printable character
            vstr_ins_char(&vstr, vstr.len, c);
        } else if (c == '\r') {
            // newline
            mp_hal_stdout_tx_str("\r\n");
            break;
        }
    }

    return mp_obj_new_str_from_vstr(&vstr);
}
