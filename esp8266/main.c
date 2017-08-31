/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "lib/oofatfs/ff.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "gccollect.h"
#include "user_interface.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PWMOut.h"

STATIC char heap[36 * 1024];

bool maybe_run(const char* filename, pyexec_result_t* exec_result) {
    mp_import_stat_t stat = mp_import_stat(filename);
    if (stat != MP_IMPORT_STAT_FILE) {
        return false;
    }
    mp_hal_stdout_tx_str(filename);
    mp_hal_stdout_tx_str(" output:\r\n");
    pyexec_file(filename, exec_result);
    return true;
}

bool serial_active = false;

STATIC bool start_mp(void) {
    pyexec_frozen_module("_boot.py");

    pyexec_result_t result;
    bool found_boot = maybe_run("settings.txt", &result) ||
                      maybe_run("settings.py", &result) ||
                      maybe_run("boot.py", &result) ||
                      maybe_run("boot.txt", &result);

    if (!found_boot || !(result.return_code & PYEXEC_FORCED_EXIT)) {
        maybe_run("code.txt", &result) ||
            maybe_run("code.py", &result) ||
            maybe_run("main.py", &result) ||
            maybe_run("main.txt", &result);
    }

    if (result.return_code & PYEXEC_FORCED_EXIT) {
        return false;
    }

    // We can't detect connections so we wait for any character to mark the serial active.
    if (!serial_active) {
        mp_hal_stdin_rx_chr();
        serial_active = true;
    }
    mp_hal_stdout_tx_str("\r\n\r\n");
    mp_hal_stdout_tx_str("Press any key to enter the REPL. Use CTRL-D to soft reset.\r\n");
    return mp_hal_stdin_rx_chr() == CHAR_CTRL_D;
}

STATIC void mp_reset(void) {
    mp_stack_set_top((void*)0x40000000);
    mp_stack_set_limit(8192);
    mp_hal_init();
    gc_init(heap, heap + sizeof(heap));
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    // Frozen modules are in their own pseudo-dir, ".frozen".
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__dot_frozen));

    mp_obj_list_init(mp_sys_argv, 0);
    MP_STATE_PORT(term_obj) = MP_OBJ_NULL;
    MP_STATE_PORT(dupterm_arr_obj) = MP_OBJ_NULL;
    reset_pins();
    #if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA
    extern void esp_native_code_init(void);
    esp_native_code_init();
    #endif
    pin_init0();
    readline_init0();
    dupterm_task_init();
    pwmout_reset();
}

bool soft_reset(void) {
    mp_hal_stdout_tx_str("PYB: soft reboot\r\n");
    mp_hal_delay_us(10000); // allow UART to flush output
    mp_reset();
    mp_hal_delay_us(1000); // Give the RTOS time to do housekeeping.
    return start_mp();
}

void init_done(void) {
    mp_reset();
    mp_hal_delay_us(1000); // Give the RTOS time to do housekeeping.
    bool skip_repl = start_mp();
    mp_hal_stdout_tx_str("\r\n");

    int exit_code = PYEXEC_FORCED_EXIT;
    while (true) {
        if (!skip_repl) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                exit_code = pyexec_raw_repl();
            } else {
                exit_code = pyexec_friendly_repl();
            }
        }
        if (exit_code == PYEXEC_FORCED_EXIT) {
            skip_repl = soft_reset();
        } else if (exit_code != 0) {
            break;
        }
    }
}

void user_init(void) {
    system_init_done_cb(init_done);
}

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#endif

void MP_FASTCODE(nlr_jump_fail)(void *val) {
    printf("NLR jump failed\n");
    for (;;) {
    }
}

//void __assert(const char *file, int line, const char *func, const char *expr) {
void __assert(const char *file, int line, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    for (;;) {
    }
}
