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

#include <stdint.h>
#include <stdlib.h>
#include "ameba_soc.h"

// Board-specific configuration (board/MCU names, pin assignments).
// Definitions here take precedence over the #ifndef defaults further below.
#include "mpconfigboard.h"

// Python internal features.
#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL            (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// Task stack and GC heap sizes for the MicroPython FreeRTOS task.
#ifndef MICROPY_TASK_STACK_SIZE
#define MICROPY_TASK_STACK_SIZE             (32 * 1024)
#endif
#define MICROPY_GC_INITIAL_HEAP_SIZE        (64 * 1024)

#define MICROPY_ALLOC_PATH_MAX              (128)

// Emitters and compiler options.
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
#define MICROPY_EMIT_THUMB                  (1)
#define MICROPY_EMIT_INLINE_THUMB           (1)
#define MICROPY_MAKE_POINTER_CALLABLE(p)    ((void *)((uint32_t)(p) | 1))
#define MICROPY_OPT_COMPUTED_GOTO           (1)

// Runtime features.
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_FINALISER            (1)
#define MICROPY_STACK_CHECK_MARGIN          (1024)
#define MICROPY_WARNINGS                    (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_DETAILED)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)

// Allow the GC heap to grow from the FreeRTOS heap on demand.
#define MICROPY_GC_SPLIT_HEAP               (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO          (1)

// Control over Python builtins.
#define MICROPY_PY_STR_BYTES_CMP_WARN       (1)
#define MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       ameba_help_text
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_MATH_GAMMA_FIX_NEGINF    (1)

// time module (provided by extmod, with port hooks in src/modtime.c).
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS        (1)
#define MICROPY_PY_TIME_INCLUDEFILE         "src/modtime.c"

// os module (provided by extmod).
#define MICROPY_PY_OS_UNAME                 (1)

// random module seed source.
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC    (_rand())

// Board specifics.
#define MICROPY_PY_SYS_PLATFORM             "ameba"

// REPL/LOGUART baud rate.  The ROM brings LOGUART up at 1500000; defining this
// macro makes rtk_loguart_init() switch to the given rate (115200 matches the
// hard-coded rate in upstream test tooling).  Comment out to keep the ROM default.
#define MICROPY_HW_LOGUART_BAUDRATE         (115200)

typedef long mp_off_t;

// We need to provide a declaration/definition of alloca().
#include <alloca.h>

// Define the port's name and hardware.
// These are overridden per-board by boards/<BOARD>/mpconfigboard.h.
#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME "ameba-board"
#endif
#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME   "ameba-cpu"
#endif

#define MP_STATE_PORT MP_STATE_VM

#define MP_SSIZE_MAX (0x7fffffff)

// No threads in the minimal port: spin on WFI between pending-event handling.
#define MICROPY_PY_WAIT_FOR_INTERRUPT __ASM volatile ("wfi" ::: "memory")
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS); \
        MICROPY_PY_WAIT_FOR_INTERRUPT; \
    } while (0);

#define MP_HAL_CLEAN_DCACHE(addr, size) DCache_Clean((uint32_t)(uintptr_t)(addr), (uint32_t)(size))

// Place hot interpreter functions in SRAM for performance.
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_LOAD_GLOBAL(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_LOAD_NAME(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_MAP_LOOKUP(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_OBJ_GET_TYPE(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_SCHED_EXCEPTION(f) SRAM_ONLY_TEXT_SECTION f
#define MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(f) SRAM_ONLY_TEXT_SECTION f

#define MICROPY_DEBUG_PRINTERS (0)
#define MICROPY_DEBUG_PARSE_RULE_NAME (0)
