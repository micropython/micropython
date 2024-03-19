/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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
#include <stdio.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"
#include "py/mperrno.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"

#include "extmod/vfs.h"

#include "tusb.h"

#include "drv_uart.h"
#include "pendsv.h"

#include "board.h"

static drv_uart_t s_stdio_uart_obj;

#if MICROPY_HW_ENABLE_USBDEV
static inline void board_usb_irq_handler(void *param) {
    uint8_t rhport = (uint8_t)((int)param & 0xFFU);
    tud_int_handler(rhport);

    /* TODO: Use sched. */
    tud_task();
    __SEV();
}
#endif

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

extern uint32_t __StackTop, __StackLimit, __HeapBase, __HeapLimit;

int main(int argc, char **argv) {
    MCX_BoardEarlyInit();

    #ifdef MICROPY_HW_ENABLE_USBDEV
    MCX_BoardConfigureUSBClock(MICROPY_HW_USBDEV);
    MCX_BoardConfigureUSBPHY(MICROPY_HW_USBDEV);
    MCX_BoardConfigureUSBISR(MICROPY_HW_USBDEV, board_usb_irq_handler, (void *)MICROPY_HW_USBDEV);

    tusb_init();
    #endif

soft_reset:
    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit((char *)&__StackTop - (char *)&__StackLimit - 1024);

    gc_init((void *)&__HeapBase, (void *)&__HeapLimit);

    mp_hal_init();
    pendsv_init();

    #if defined(MICROPY_HW_UART_REPL)
    MP_STATE_PORT(stdio_uart) = &s_stdio_uart_obj;
    drv_uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = DRV_Uart8DataBits,
        .stop_bits = DRV_Uart1StopBits,
        .parity = DRV_UartNoParity,
    };

    drv_uart_init(&s_stdio_uart_obj, MICROPY_HW_UART_REPL, &uart_cfg);

    mp_hal_stdio_init();
    #endif

    mp_init();
    readline_init0();

    /* Exec boot script to set up internal flash */
    pyexec_frozen_module("_boot.py", false);

    /* Execute start-up script. */
    if (pyexec_file_if_exists("boot.py") == PYEXEC_FORCED_EXIT) {
        goto soft_reset_exit;
    }

    if (pyexec_file_if_exists("main.py") == PYEXEC_FORCED_EXIT) {
        goto soft_reset_exit;
    }

    #if MICROPY_ENABLE_COMPILER
    // Main script is finished, so now go into REPL mode.
    // The REPL mode can change, or it can request a soft reset.
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
    #endif
soft_reset_exit:

    mp_printf(&mp_plat_print, "MPY: soft reboot\n");

    drv_uart_deinit(&s_stdio_uart_obj);

    gc_sweep_all();
    mp_deinit();

    goto soft_reset;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
    gc_dump_info(&mp_plat_print);
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    __fatal_error("");
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
