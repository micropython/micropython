/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Yonatan Goldschmidt
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


#include <linux/bitops.h> // will define BITS_PER_BYTE so mpconfig.h won't define it again.


#define MICROPY_ENABLE_COMPILER     (1)
#define MICROPY_ALLOC_PATH_MAX      (256) // PATH_MAX is 4096, too much
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (64)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_LAZY_LOAD_GLOBAL    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_PY_DELATTR_SETATTR  (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_DETAILED)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_UERRNO           (1) // TODO add all kernel errnos?
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_CPYTHON_COMPAT      (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)

#ifdef INCLUDE_STRUCT_LAYOUT
#define MICROPY_MODULE_FROZEN_MPY   (1)
#define MICROPY_QSTR_EXTRA_POOL     mp_qstr_frozen_const_pool
#endif

#define INT_FMT "%ld"
typedef long mp_int_t;
#define UINT_FMT "%lu"
typedef unsigned long mp_uint_t;

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// We need to provide a declaration/definition of alloca(), yet Linux doesn't have/use it.
// We can force MPY out of it using MICROPY_NO_ALLOCA but I don't think it matters that
// much. MPY doesn't use alloca() too often.
#define alloca(n) __builtin_alloca(n)

#include <generated/utsrelease.h> // for UTS_RELEASE

#define MICROPY_HW_BOARD_NAME "Linux version " UTS_RELEASE

#ifdef __amd64__
#define MICROPY_HW_MCU_NAME "x86_64"
#else
#error "unknown arch!"
#endif

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[512];
