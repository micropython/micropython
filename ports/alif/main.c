/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/tinyusb/mp_usbd.h"
#include "tusb.h"
#include "mpuart.h"
#include "ospi_flash.h"
#include "pendsv.h"
#include "system_tick.h"

extern uint8_t __StackTop, __StackLimit;
extern uint8_t __GcHeapStart, __GcHeapEnd;

NORETURN void panic(const char *msg) {
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
    for (;;) {
        *(volatile uint32_t *)0x4900C000 ^= 9;
        for (volatile uint delay = 0; delay < 10000000; delay++) {
        }
    }
}

void _start(void) {
    system_tick_init();

    MICROPY_BOARD_STARTUP();

    pendsv_init();

    MICROPY_BOARD_EARLY_INIT();

    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_init();
    #endif

    if (ospi_flash_init() != 0) {
        MICROPY_BOARD_FATAL_ERROR("ospi_init failed");
    }

    #if MICROPY_HW_ENABLE_USBDEV
    NVIC_ClearPendingIRQ(USB_IRQ_IRQn);
    NVIC_SetPriority(USB_IRQ_IRQn, IRQ_PRI_USB);
    #endif

    // Initialise stack extents and GC heap.
    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit(&__StackTop - &__StackLimit - 1024);
    gc_init(&__GcHeapStart, &__GcHeapEnd);

    for (;;) {
        // Initialise MicroPython runtime.
        mp_init();

        // Initialise sub-systems.
        readline_init0();

        // Execute _boot.py to set up the filesystem.
        pyexec_frozen_module("_boot.py", false);

        // Execute user scripts.
        int ret = pyexec_file_if_exists("boot.py");
        #if MICROPY_HW_ENABLE_USBDEV
        mp_usbd_init();
        #endif
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
            ret = pyexec_file_if_exists("main.py");
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
        }

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

    soft_reset_exit:
        mp_printf(MP_PYTHON_PRINTER, "MPY: soft reboot\n");
        gc_sweep_all();
        mp_deinit();
    }
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
        __WFE();
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    panic("Assertion failed");
}
#endif
