/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Emil Renner Berthing
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

#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"

#include "lib/rcu.h"
#include "lib/eclic.h"
#include "lib/gpio.h"

#include "usbacm.h"

static void **stack_top;
static char heap[16384];

#ifdef __interrupt
void trap_entry(void) {
    printf("TRAP!\n");
    while (1) {
        /* forever */
    }
}
#endif

int main(void) {
    rcu_sysclk_init();
    eclic_init();
    eclic_global_interrupt_enable();

    #if 0
    /* turn on power to GPIOA */
    RCU->APB2EN |= RCU_APB2EN_PAEN;

    gpio_pin_set(GPIO_PA1);
    gpio_pin_config(GPIO_PA1, GPIO_MODE_PP_50MHZ);
    gpio_pin_set(GPIO_PA2);
    gpio_pin_config(GPIO_PA2, GPIO_MODE_PP_50MHZ);
    #endif

    usbacm_init();

    (void)mp_hal_stdin_rx_chr();

    /* save current stack pointer to stack_top */
    __asm("sw sp, %0\n" : "=m" (stack_top));

soft_reset:
    mp_stack_set_top(stack_top);
    mp_stack_set_limit(1024);

    gc_init(heap, heap + sizeof(heap));

    mp_init();
    /* sys.path = [''] */
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
    /* sys.argv = [] */
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    readline_init0();

    while (1) {
        if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
            if (pyexec_raw_repl()) {
                break;
            }
        } else {
            if (pyexec_friendly_repl()) {
                break;
            }
        }
    }
    gc_sweep_all();
    printf("soft reboot\n");
    goto soft_reset;
}

void gc_collect(void) {
    void *sregs[12];
    void **sp;

    gc_collect_start();

    /* push callee-save registers to
     * stack before sweeping it */
    __asm(
        "sw	s0,   0+%1 \n"
        "sw	s1,   4+%1 \n"
        "sw	s2,   8+%1 \n"
        "sw	s3,  12+%1 \n"
        "sw	s4,  16+%1 \n"
        "sw	s5,  20+%1 \n"
        "sw	s6,  24+%1 \n"
        "sw	s7,  28+%1 \n"
        "sw	s8,  32+%1 \n"
        "sw	s9,  36+%1 \n"
        "sw	s10, 40+%1 \n"
        "sw	s11, 44+%1 \n"
        "mv	%0,  sp    \n"
        : "=r" (sp), "=o" (sregs)
        );

    gc_collect_root(sp, stack_top - sp);
    gc_collect_end();
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
        /* forever */
    }
}

void NORETURN __fatal_error(const char *msg) {
    printf("%s\n", msg);
    while (1) {
        /* forever */
    }
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
