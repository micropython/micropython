/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Michael Neuling, IBM Corporation.
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

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "shared/runtime/pyexec.h"

#include "io.h"
#include "microwatt_soc.h"
#include "uart_std.h"

unsigned long ppc_tb_freq = 512000000;

#define UART_BAUDS 115200

void __stack_chk_fail(void);
void __stack_chk_fail(void) {
    static bool failed_once;

    if (failed_once) {
        return;
    }
    failed_once = true;
    printf("Stack corruption detected !\n");
    assert(0);
}

/* fill in __assert_fail for libc */
void __assert_fail(const char *__assertion, const char *__file,
    unsigned int __line, const char *__function) {
    printf("Assert at %s:%d:%s() \"%s\" failed\n", __file, __line, __function, __assertion);
    for (;;) {;
    }
}

static char *stack_top;
#if MICROPY_ENABLE_GC
static char heap[32 * 1024];
#endif

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    return std_uart_rx_chr();
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    std_uart_tx_strn(str, len);
}

static void init_microwatt(void) {
    uint64_t sys_info;
    uint64_t proc_freq;
    uint64_t uart_info = 0;
    uint64_t uart_freq = 0;

    proc_freq = readq(SYSCON_BASE + SYS_REG_CLKINFO) & SYS_REG_CLKINFO_FREQ_MASK;
    sys_info = readq(SYSCON_BASE + SYS_REG_INFO);

    if (!(sys_info & SYS_REG_INFO_HAS_ARTB)) {
        ppc_tb_freq = proc_freq;
    }

    if (sys_info & SYS_REG_INFO_HAS_LARGE_SYSCON) {
        uart_info = readq(SYSCON_BASE + SYS_REG_UART0_INFO);
        uart_freq = uart_info & 0xffffffff;
    }
    if (uart_freq == 0) {
        uart_freq = proc_freq;
    }

    if (uart_info & SYS_REG_UART_IS_16550) {
        std_uart_init(UART_BASE, 2, uart_freq, UART_BAUDS);
    } else {
        /* PANIC */
        while (1) {
            ;
        }
    }
}

static void init_devicetree(const void *devtree) {
#define QEMU_UART_BASE 0x60300d00103f8
    /* TODO: Use libfdt to parse the device-tree, in the meantime
     * we assume POWER9 powernv (as provided by qemu)
     */
    std_uart_init(QEMU_UART_BASE, 0, 1843200, UART_BAUDS);
}

int main(const void *devtree) {
    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    /*
     * Platform detection. Eventually assume r3 will contain
     * a device-tree pointer and use libfdt to parse it, in
     * the meantime, if r3 is 0, assume standalone microwatt
     */
    if (devtree == 0) {
        init_microwatt();
    } else {
        init_devicetree(devtree);
    }

    #if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[1024];
    mp_pystack_init(pystack, &pystack[1024]);
    #endif

    #if MICROPY_STACK_CHECK
    mp_stack_ctrl_init();
    mp_stack_set_limit(48 * 1024);
    #endif

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    #if MICROPY_ENABLE_COMPILER
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
    #else
    pyexec_friendly_repl();
    #endif
    #else
    pyexec_frozen_module("frozentest.py");
    #endif
    mp_deinit();
    return 0;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
