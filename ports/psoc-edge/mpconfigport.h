/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Damien P. George
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

// Options to control how MicroPython is built

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#define MICROPY_PERSISTENT_CODE_LOAD_NATIVE     (MICROPY_EMIT_MACHINE_CODE)

// Python internal features
#define MICROPY_READER_VFS                      (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_STACK_CHECK_MARGIN              (1024)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_SCHEDULER_DEPTH                 (8)
#define MICROPY_SCHEDULER_STATIC_NODES          (1)
#define MICROPY_USE_INTERNAL_ERRNO              (1)

// Fine control over Python builtins, classes, modules, etc.
#define MICROPY_PY_SYS_PLATFORM                 "psoc-edge"

// Extended modules
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS            (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/psoc-edge/modtime.c"
#define MICROPY_VFS                             (1)

#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/psoc-edge/modmachine.c"

// Miscellaneous settings

#define MP_STATE_PORT MP_STATE_VM

// Need an implementation of the log2 function which is not a macro.
#define MP_NEED_LOG2 (1)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

typedef intptr_t mp_off_t;

// Need to provide a declaration/definition of alloca().
#include <alloca.h>
