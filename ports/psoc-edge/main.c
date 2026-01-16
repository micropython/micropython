/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>


// MTB includes
#include "cybsp.h"
#include "retarget_io_init.h"

// micropython includes
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "py/runtime.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"

// port-specific includes
#include "mplogger.h"

#if MICROPY_ENABLE_GC
extern uint8_t __StackTop, __StackSize;
extern uint8_t __HeapBase, __HeapLimit;
#endif

extern void time_init(void);
extern void machine_pin_irq_deinit_all(void);

int main(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cybsp_init failed !\n"));
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io middleware */
    init_retarget_io();

    // Initialise the MicroPython runtime.
    #if MICROPY_ENABLE_GC
    gc_init(&__HeapBase, &__HeapLimit);
    mp_cstack_init_with_top((void *)&__StackTop, __StackSize);
    #endif

    time_init();

soft_reset:
    mp_init();

    readline_init0();
   
    #if MICROPY_VFS
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));

    #if MICROPY_VFS_LFS2
    pyexec_frozen_module("vfs_lfs2.py", false);
    #endif
    #endif

        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            int ret = pyexec_file_if_exists("/main.py");

            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset;
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

    mp_printf(&mp_plat_print, "MPY: soft reboot\n");

    machine_pin_irq_deinit_all();

    #if MICROPY_ENABLE_GC
    gc_sweep_all();
    #endif
    mp_deinit();

    goto soft_reset;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

// #if !MICROPY_VFS
// mp_import_stat_t mp_import_stat(const char *path) {
//     return MP_IMPORT_STAT_NO_EXIST;
// }

// mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
//     mp_raise_OSError(MP_ENOENT);
// }
// #endif
