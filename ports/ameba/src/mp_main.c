/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Realtek Corporation
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

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ameba_rtos_version.h"
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"

// Allocate memory for the MicroPython GC heap.
static char heap[MICROPY_GC_INITIAL_HEAP_SIZE];
// This is the static memory (TCB and stack) for the main MicroPython task
static StaticTask_t mpTaskTCB;
static StackType_t mpTaskStack[MICROPY_TASK_STACK_SIZE / sizeof(portSTACK_TYPE)];

static inline uintptr_t get_sp(void) {
    uintptr_t result;
    __asm__ ("mov %0, sp\n" : "=r" (result));
    return result;
}
extern int rtk_diag_req_log_enable(u8 state);

void mp_main(void *para) {
    int a = rtk_diag_req_log_enable(0);
    UNUSED(a);

    volatile uint32_t sp = (uint32_t)get_sp();
    mp_printf(MP_PYTHON_PRINTER, "mp_main mp-sp %p %p heap %p\n", (void *)(uintptr_t)sp, (void *)mpTaskStack, (void *)heap);
    #if MICROPY_PY_THREAD
    mp_thread_init(mpTaskStack, MICROPY_TASK_STACK_SIZE / sizeof(portSTACK_TYPE));
    #endif
    extern void rtk_loguart_init(void);
    rtk_loguart_init();
soft_reset:
    mp_cstack_init_with_top((void *)sp, MICROPY_TASK_STACK_SIZE);
    gc_init(heap, heap + MICROPY_GC_INITIAL_HEAP_SIZE);
    mp_init();
    readline_init0();
    // run boot-up scripts
    pyexec_frozen_module("_boot.py", false);
    int ret = pyexec_file_if_exists("boot.py");
    if (ret & PYEXEC_FORCED_EXIT) {
        goto soft_reset_exit;
    }
    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
        int ret = pyexec_file_if_exists("main.py");
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
    #if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif
    mp_deinit();

    goto soft_reset;
}

// Handle uncaught exceptions (should never be reached in a correct C implementation).
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

// Do a garbage collection cycle.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif
    gc_collect_end();
}

#if MICROPY_GC_SPLIT_HEAP_AUTO
extern const size_t xHeapStructSize;
// The largest new region that is available to become Python heap is the largest
// free block in the FreeRTOS heap.
size_t gc_get_max_new_split(void) {
    HeapStats_t xHeapStats;
    // calling vPortGetHeapStats will crash when freeheapsize is 0, so just return
    if (xPortGetFreeHeapSize() == 0) {
        return 0;
    }
    vPortGetHeapStats(&xHeapStats);
    // Reserve 16 KB so GC expansion cannot exhaust the system (FreeRTOS) heap.
    const size_t SYSTEM_HEAP_RESERVE = 16 * 1024;
    size_t available = xHeapStats.xSizeOfLargestFreeBlockInBytes - xHeapStructSize;
    return available > SYSTEM_HEAP_RESERVE ? available - SYSTEM_HEAP_RESERVE : 0;
}

#endif


#if !MICROPY_VFS
// There is no filesystem so opening a file raises an exception.
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_printf(MP_PYTHON_PRINTER, "mp_lexer_new_from_file\n");
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#endif

void app_pre_example(void) {
}

void app_example(void) {
    if (NULL == xTaskCreateStatic(mp_main, "mp_main", MICROPY_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, 4, mpTaskStack, &mpTaskTCB)) {
        mp_printf(MP_PYTHON_PRINTER, "mp_main create failed\n");
    }
}
