/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 Infineon Technologies AG
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

// Options controlling how MicroPython is built, overriding defaults in py/mpconfig.h
#include <alloca.h>
#include <stdint.h>

// #include "shared/runtime/interrupt_char.h"
#include "mpconfigboard.h"

// options to control how MicroPython is built

#define MICROPY_PY_BUILTINS_HELP                (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES        (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT           psoc_edge_help_text
#define MICROPY_USE_INTERNAL_PRINTF             (0)
#define MICROPY_REPL_INFO                       (1)
#define MICROPY_HELPER_REPL                     (1)

// Use core features for Thonny compatibility (larger firmware)
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)

// Enable bytearray for I2C Target memory buffers
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_SLICE (1)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER           (1)

// Enable scheduler for IRQ callbacks (required for PDM_PCM.irq())
#define MICROPY_ENABLE_SCHEDULER          (1)

#define MICROPY_PY_GC                     (1)
#define MICROPY_ENABLE_GC                 (1)

#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)

// Enable essential built-in types
#define MICROPY_PY_BUILTINS_BYTEARRAY     (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW    (1)
#define MICROPY_PY_ARRAY                  (1)

#define MICROPY_ALLOC_PATH_MAX            (256)

// Use the minimum headroom in the chunk allocator for parse nodes.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)

// Enable os module
#define MICROPY_PY_OS                           (1)
#define MICROPY_PY_OS_INCLUDEFILE               "ports/psoc-edge/modos.c"
#define MICROPY_PY_OS_UNAME                     (0)
#define MICROPY_PY_OS_URANDOM                   (0)
#define MICROPY_PY_OS_GETENV_PUTENV_UNSETENV    (0)
#define MICROPY_PY_OS_SYSTEM                    (1)
#define MICROPY_PY_OS_ERRNO                     (1)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_SYS_PLATFORM                 "psoc-edge"
#define MICROPY_PY_SYS_EXC_INFO                 (1)

// Extended modules
#define MICROPY_PY_TIME                         (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS            (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/psoc-edge/modtime.c"
#define MICROPY_PY_MACHINE_FREQ_NUM_ARGS_MAX    (2)

// Logger
#define MICROPY_LOGGER_DEBUG                    (0)

// Machine module
#define MICROPY_PY_MACHINE                      (1)
// Use extmod's modmachine.c which includes I2CTarget
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/psoc-edge/modmachine.c"
#define MICROPY_PY_MACHINE_I2C                  (1)
#define MICROPY_PY_MACHINE_SOFTI2C              (0)

// I2C Target support (requires GC and Scheduler for IRQ framework)
#define MICROPY_PY_MACHINE_I2C_TARGET           (1)
#define MICROPY_PY_MACHINE_I2C_TARGET_MAX       (1)
#define MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE "ports/psoc-edge/machine_i2c_target.c"
#define MICROPY_ENABLE_SCHEDULER                (1)
#define MICROPY_SCHEDULER_DEPTH                 (8)

#define MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE   (1)

#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/psoc-edge/modmachine.c"
#define MICROPY_PY_MACHINE_PDM_PCM              (1)
#define MICROPY_PY_MACHINE_PDM_PCM_RING_BUF     (1)

#define MICROPY_PY_MACHINE_PIN_MAKE_NEW         mp_pin_make_new

// VFS
#define MICROPY_VFS                             (1)         // Can be removed once we add #define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)
#define MICROPY_PY_VFS                          (1)
#define MICROPY_READER_VFS                      (1)
#define MICROPY_PY_IO                           (1)
#define MICROPY_PY_IO_IOBASE                    (1)

#define MICROPY_ENABLE_FINALISER                (1)

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_EVENT_POLL_HOOK_FAST \
    do { \
        mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS); \
    } while (0);

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        MICROPY_EVENT_POLL_HOOK_FAST; \
    } while (0);
