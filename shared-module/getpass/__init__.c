// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

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
