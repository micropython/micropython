/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2015-2016 Paul Sokolovsky
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

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/gc.h"

#include "extmod/misc.h"
#include "extmod/modmachine.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "gccollect.h"
#include "user_interface.h"

#if MICROPY_PY_ESPNOW
#include "modespnow.h"
#endif

static char heap[38 * 1024];

#if MICROPY_HW_HARD_FAULT_DEBUG

static void format_hex(uint32_t hex, char *buffer) {
    static const char table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    int offset = 7;
    uint32_t value = hex;
    while (offset >= 0) {
        buffer[offset--] = table[value & 0x0F];
        value >>= 4;
    }
}

static void print_reset_info(void) {
    struct rst_info *rst_info = system_get_rst_info();
    if ((rst_info->reason == REASON_WDT_RST) || (rst_info->reason == REASON_EXCEPTION_RST) || (rst_info->reason == REASON_SOFT_WDT_RST)) {
        char buffer[8];
        mp_hal_stdout_tx_str("\r\n\r\nThe system restarted due to an error.\r\n\r\nReason: ");
        switch (rst_info->reason) {
            case REASON_WDT_RST:
                mp_hal_stdout_tx_str("WDT");
                break;

            case REASON_EXCEPTION_RST:
                mp_hal_stdout_tx_str("EXCEPTION");
                break;

            case REASON_SOFT_WDT_RST:
                mp_hal_stdout_tx_str("SOFT_WDT");
                break;

            default:
                assert(!"Should not ever get here.");
                break;
        }
        mp_hal_stdout_tx_str(" Cause: ");
        format_hex(rst_info->exccause, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str(" EPC1: ");
        format_hex(rst_info->epc1, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str(" EPC2: ");
        format_hex(rst_info->epc2, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str(" EPC3: ");
        format_hex(rst_info->epc3, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str(" Exception Vector address: ");
        format_hex(rst_info->excvaddr, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str(" DEPC: ");
        format_hex(rst_info->depc, buffer);
        mp_hal_stdout_tx_strn(buffer, sizeof(buffer));
        mp_hal_stdout_tx_str("\r\n\r\n");
    }
}

#endif

static void mp_reset(void) {
    mp_stack_set_top((void *)0x40000000);
    mp_stack_set_limit(8192);
    mp_hal_init();
    gc_init(heap, heap + sizeof(heap));
    mp_init();
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    #if MICROPY_EMIT_XTENSA || MICROPY_EMIT_INLINE_XTENSA
    extern void esp_native_code_init(void);
    esp_native_code_init();
    #endif
    pin_init0();
    readline_init0();

    // Activate UART(0) on dupterm slot 1 for the REPL
    {
        mp_obj_t args[2];
        args[0] = MP_OBJ_NEW_SMALL_INT(0);
        args[1] = MP_OBJ_NEW_SMALL_INT(115200);
        args[0] = MP_OBJ_TYPE_GET_SLOT(&machine_uart_type, make_new)(&machine_uart_type, 2, 0, args);
        args[1] = MP_OBJ_NEW_SMALL_INT(1);
        mp_os_dupterm_obj.fun.var(2, args);
    }

    #if MICROPY_PY_ESPNOW
    espnow_deinit(mp_const_none);
    #endif

    #if MICROPY_MODULE_FROZEN
    pyexec_frozen_module("_boot.py", false);
    int ret = pyexec_file_if_exists("boot.py");
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
        pyexec_file_if_exists("main.py");
    }
    #endif
}

void soft_reset(void) {
    gc_sweep_all();
    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");
    mp_hal_delay_us(10000); // allow UART to flush output
    mp_reset();
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    #endif
}

void init_done(void) {
    // Configure sleep, and put the radio to sleep if no interfaces are active
    wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
    if (wifi_get_opmode() == NULL_MODE) {
        wifi_fpm_open();
        wifi_fpm_do_sleep(0xfffffff);
    }

    #if MICROPY_REPL_EVENT_DRIVEN
    uart_task_init();
    #endif
    mp_reset();
    mp_hal_stdout_tx_str("\r\n");
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    #endif

    #if MICROPY_HW_HARD_FAULT_DEBUG
    print_reset_info();
    #endif

    #if !MICROPY_REPL_EVENT_DRIVEN
soft_reset:
    for (;;) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl() != 0) {
                break;
            }
        } else {
            if (pyexec_friendly_repl() != 0) {
                break;
            }
        }
    }
    soft_reset();
    goto soft_reset;
    #endif
}

void user_init(void) {
    system_timer_reinit();
    system_init_done_cb(init_done);
}

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
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

// void __assert(const char *file, int line, const char *func, const char *expr) {
void __assert(const char *file, int line, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    for (;;) {
    }
}

#if !MICROPY_DEBUG_PRINTERS
// With MICROPY_DEBUG_PRINTERS disabled DEBUG_printf is not defined but it
// is still needed by esp-open-lwip for debugging output, so define it here.
#include <stdarg.h>
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
int DEBUG_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(MICROPY_DEBUG_PRINTER, fmt, ap);
    va_end(ap);
    return ret;
}
#endif
