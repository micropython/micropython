/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2021 Damien P. George
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

// Memory allocation policy
#define MICROPY_QSTR_BYTES_IN_HASH              (1)

// Compiler configuration
#define MICROPY_COMP_CONST                      (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN        (0)

// Python internal features
#define MICROPY_ENABLE_EXTERNAL_IMPORT          (0)
#define MICROPY_ERROR_REPORTING                 (MICROPY_ERROR_REPORTING_NONE)
#define MICROPY_CPYTHON_COMPAT                  (0)
#define MICROPY_MODULE_GETATTR                  (0)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG   (0)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_ASYNC_AWAIT                  (0)
#define MICROPY_PY_ASSIGN_EXPR                  (0)
#define MICROPY_PY_BUILTINS_STR_COUNT           (0)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO       (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY           (0)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS       (0)
#define MICROPY_PY_BUILTINS_SET                 (0)
#define MICROPY_PY_BUILTINS_SLICE               (0)
#define MICROPY_PY_BUILTINS_PROPERTY            (0)
#define MICROPY_PY_BUILTINS_ENUMERATE           (0)
#define MICROPY_PY_BUILTINS_REVERSED            (0)
#define MICROPY_PY___FILE__                     (0)
#define MICROPY_PY_ARRAY                        (0)
#define MICROPY_PY_COLLECTIONS                  (0)
#define MICROPY_PY_IO                           (0)
#define MICROPY_PY_STRUCT                       (0)
#define MICROPY_PY_SYS                          (0)

// Type definitions for the specific machine

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
