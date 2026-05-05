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
#include "mpconfigboard.h"

// Use core features for Thonny compatibility (larger firmware)
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#define MICROPY_EMIT_THUMB                      (1)
#define MICROPY_EMIT_INLINE_THUMB               (1)
#define MICROPY_MAKE_POINTER_CALLABLE(p)        ((void *)((mp_uint_t)(p) | 1))

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
#define MICROPY_ALLOC_PARSE_CHUNK_INIT          (16)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_SYS_PLATFORM                 "psoc-edge"
#define MICROPY_PY_BUILTINS_HELP_TEXT           psoc_edge_help_text
#define MICROPY_REPL_INFO                       (1)

// Extended modules
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS            (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/psoc-edge/modtime.c"
#define MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE   (1)
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_VFS                             (1)
#define MICROPY_ALLOC_PATH_MAX                  (256)

// Machine module
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/psoc-edge/modmachine.c"

#define MICROPY_PY_MACHINE_PIN_MAKE_NEW         mp_pin_make_new

// type definitions for the specific machine
#define MP_SSIZE_MAX (0x7fffffff)

typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM
