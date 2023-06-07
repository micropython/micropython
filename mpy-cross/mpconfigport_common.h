/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2023 Jim Mussared
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

// This file should be included by the active top-level mpconfigport.h.

// mpy-cross has no need to load .mpy files, but can generate them.
#define MICROPY_PERSISTENT_CODE_LOAD   (0)
#define MICROPY_PERSISTENT_CODE_SAVE   (1)

// Allow compiler configuration to be changed at runtime rather than
// hard-coded at compile-time like it would be for a regular port.
#define MICROPY_DYNAMIC_COMPILER       (1)

// Enable all emitters (selectable at runtime via the dynamic compiler).
#define MICROPY_EMIT_X64               (1)
#define MICROPY_EMIT_X86               (1)
#define MICROPY_EMIT_THUMB             (1)
#define MICROPY_EMIT_INLINE_THUMB      (1)
#define MICROPY_EMIT_ARM               (1)
#define MICROPY_EMIT_XTENSA            (1)
#define MICROPY_EMIT_INLINE_XTENSA     (1)
#define MICROPY_EMIT_XTENSAWIN         (1)

// Enable all compiler optimisation/features.
#define MICROPY_COMP_CONST_FOLDING     (1)
#define MICROPY_COMP_MODULE_CONST      (1)
#define MICROPY_COMP_CONST             (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_RETURN_IF_EXPR    (1)
#define MICROPY_PY_FSTRINGS            (1)
#define MICROPY_ENABLE_SOURCE_LINE     (1)
#define MICROPY_ENABLE_DOC_STRING      (0)
#define MICROPY_CPYTHON_COMPAT         (1)

// Full details of compile/syntax errors.
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_DETAILED)

// Use maximum feature/capability for str/int/float.
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)

// mpy-cross never executes any code. Disable all built-in modules and the
// Python runtime.
#define MICROPY_ENABLE_RUNTIME         (0)
#define MICROPY_PY___FILE__            (0)
#define MICROPY_PY_ARRAY               (0)
#define MICROPY_PY_ATTRTUPLE           (0)
#define MICROPY_PY_COLLECTIONS         (0)
#define MICROPY_PY_MATH                (0)
#define MICROPY_PY_CMATH               (0)
#define MICROPY_PY_GC                  (0)
#define MICROPY_PY_IO                  (0)
#define MICROPY_PY_SYS                 (0)

// As code cannot be executed, there is no need for import (or implementing
// mp_import_stat).
#define MICROPY_ENABLE_EXTERNAL_IMPORT (0)

// Because code never executes and error handling is done by mpy-cross itself,
// there is no need to be able to print.
#define MICROPY_USE_INTERNAL_PRINTF (0)
#define MP_PLAT_PRINT_STRN(str, len) (void)0
