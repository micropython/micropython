/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// options to control how MicroPython is built

// Variant-specific definitions.
#include "mpconfigvariant.h"

// By default use MicroPython version of readline
#ifndef MICROPY_USE_READLINE
#define MICROPY_USE_READLINE        (1)
#endif
#ifndef MICROPY_USE_READLINE_HISTORY
#define MICROPY_USE_READLINE_HISTORY (1)
#endif
#ifndef MICROPY_READLINE_HISTORY_SIZE
#define MICROPY_READLINE_HISTORY_SIZE (50)
#endif

#define MICROPY_ALLOC_PATH_MAX      (260) // see minwindef.h for msvc or limits.h for mingw
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_RETURN_IF_EXPR (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_ENABLE_PYSTACK      (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_DEBUG_PRINTER       (&mp_stderr_print)
#define MICROPY_DEBUG_PRINTERS      (1)
#define MICROPY_READER_POSIX        (1)
#define MICROPY_READER_VFS          (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_EMACS_KEYS     (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_HELPER_LEXER_UNIX   (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#ifndef MICROPY_FLOAT_IMPL
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#endif
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#ifndef MICROPY_STREAMS_NON_BLOCK
#define MICROPY_STREAMS_NON_BLOCK   (1)
#endif
#define MICROPY_STREAMS_POSIX_API   (1)
#define MICROPY_OPT_COMPUTED_GOTO   (0)
#define MICROPY_MODULE_OVERRIDE_MAIN_IMPORT (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#ifndef MICROPY_ENABLE_SCHEDULER
#define MICROPY_ENABLE_SCHEDULER    (1)
#endif
#define MICROPY_VFS                 (1)
#define MICROPY_VFS_POSIX           (1)
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_DESCRIPTORS      (1)
#define MICROPY_PY_DELATTR_SETATTR  (1)
#define MICROPY_PY_FSTRINGS         (1)
#define MICROPY_PY_BUILTINS_BYTES_HEX (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_STR_CENTER (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_COMPILE (1)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED (1)
#define MICROPY_PY_BUILTINS_INPUT   (1)
#define MICROPY_PY_BUILTINS_POW3    (1)
#ifndef MICROPY_PY_BUILTINS_HELP
#define MICROPY_PY_BUILTINS_HELP    (1)
#endif
#ifndef MICROPY_PY_BUILTINS_HELP_MODULES
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#endif
#define MICROPY_PY_BUILTINS_ROUND_INT (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (1)
#define MICROPY_PY_SYS_PATH_ARGV_DEFAULTS (0)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_ATEXIT       (1)
#define MICROPY_PY_SYS_PLATFORM     "win32"
#ifndef MICROPY_PY_SYS_PATH_DEFAULT
#define MICROPY_PY_SYS_PATH_DEFAULT ".frozen;~/.micropython/lib"
#endif
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_SYS_EXC_INFO     (1)
#define MICROPY_PY_COLLECTIONS_DEQUE (1)
#define MICROPY_PY_COLLECTIONS_DEQUE_ITER (1)
#define MICROPY_PY_COLLECTIONS_DEQUE_SUBSCR (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#ifndef MICROPY_PY_MATH_SPECIAL_FUNCTIONS
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (1)
#endif
#define MICROPY_PY_MATH_ISCLOSE     (MICROPY_PY_MATH_SPECIAL_FUNCTIONS)
#define MICROPY_PY_CMATH            (1)
#define MICROPY_PY_IO_IOBASE        (1)
#define MICROPY_PY_GC_COLLECT_RETVAL (1)
#ifndef MICROPY_STACKLESS
#define MICROPY_STACKLESS           (0)
#define MICROPY_STACKLESS_STRICT    (0)
#endif

#define MICROPY_PY_OS               (1)
#define MICROPY_PY_OS_INCLUDEFILE   "ports/unix/modos.c"
#define MICROPY_PY_OS_ERRNO         (1)
#define MICROPY_PY_OS_GETENV_PUTENV_UNSETENV (1)
#define MICROPY_PY_OS_STATVFS       (0)
#define MICROPY_PY_OS_SYSTEM        (1)
#define MICROPY_PY_OS_URANDOM       (1)
#define MICROPY_PY_TIME             (1)
#define MICROPY_PY_TIME_TIME_TIME_NS (1)
#define MICROPY_PY_TIME_CUSTOM_SLEEP (1)
#define MICROPY_PY_TIME_INCLUDEFILE "ports/unix/modtime.c"
#define MICROPY_PY_ERRNO            (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_DEFLATE          (1)
#define MICROPY_PY_DEFLATE_COMPRESS (1)
#define MICROPY_PY_JSON             (1)
#define MICROPY_PY_RE               (1)
#define MICROPY_PY_HEAPQ            (1)
#define MICROPY_PY_HASHLIB          (1)
#define MICROPY_PY_BINASCII         (1)
#define MICROPY_PY_BINASCII_CRC32   (1)
#define MICROPY_PY_RANDOM           (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/unix/modmachine.c"
#define MICROPY_PY_MACHINE_PULSE    (1)
#define MICROPY_PY_MACHINE_PIN_BASE (1)
#define MICROPY_MACHINE_MEM_GET_READ_ADDR   mod_machine_mem_get_addr
#define MICROPY_MACHINE_MEM_GET_WRITE_ADDR  mod_machine_mem_get_addr

#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_DETAILED)
#define MICROPY_ERROR_PRINTER       (&mp_stderr_print)
#define MICROPY_WARNINGS            (1)
#define MICROPY_PY_STR_BYTES_CMP_WARN (1)

// VFS stat functions should return time values relative to 1970/1/1
#define MICROPY_EPOCH_IS_1970       (1)

extern const struct _mp_print_t mp_stderr_print;

#ifdef _MSC_VER
#define MICROPY_GCREGS_SETJMP       (1)
#define MICROPY_USE_INTERNAL_PRINTF (0)
#endif

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE     (256)
#define MICROPY_KBD_EXCEPTION       (1)

#define MICROPY_PORT_INIT_FUNC      init()
#define MICROPY_PORT_DEINIT_FUNC    deinit()

// type definitions for the specific machine

#if defined(__MINGW32__) && defined(__LP64__)
typedef long mp_int_t; // must be pointer size
typedef unsigned long mp_uint_t; // must be pointer size
#elif defined(__MINGW32__) && defined(_WIN64)
#include <stdint.h>
typedef __int64 mp_int_t;
typedef unsigned __int64 mp_uint_t;
#define MP_SSIZE_MAX __INT64_MAX__
#elif defined(_MSC_VER) && defined(_WIN64)
typedef __int64 mp_int_t;
typedef unsigned __int64 mp_uint_t;
#else
// These are definitions for machines where sizeof(int) == sizeof(void*),
// regardless for actual size.
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif

typedef long suseconds_t;

// Just assume Windows is little-endian - mingw32 gcc doesn't
// define standard endianness macros.
#define MP_ENDIANNESS_LITTLE (1)

// Cannot include <sys/types.h>, as it may lead to symbol name clashes
#if _FILE_OFFSET_BITS == 64 && !defined(__LP64__)
typedef long long mp_off_t;
#else
typedef long mp_off_t;
#endif

#define MP_STATE_PORT               MP_STATE_VM

#define MICROPY_MPHALPORT_H         "windows_mphal.h"

// We need to provide a declaration/definition of alloca()
#include <malloc.h>

#include "realpath.h"
#include "init.h"

#ifdef __GNUC__
#define MP_NOINLINE __attribute__((noinline))
#endif

// MSVC specifics
#ifdef _MSC_VER

// Sanity check

#if (_MSC_VER < 1800)
    #error Can only build with Visual Studio 2013 toolset
#endif


// CL specific overrides from mpconfig

#define MP_NORETURN                 __declspec(noreturn)
#define MP_WEAK
#define MP_NOINLINE                 __declspec(noinline)
#define MP_ALWAYSINLINE             __forceinline
#define MP_LIKELY(x)                (x)
#define MP_UNLIKELY(x)              (x)
#define MICROPY_PORT_CONSTANTS      { MP_ROM_QSTR(MP_QSTR_dummy), MP_ROM_PTR(NULL) } // can't have zero-sized array
#ifdef _WIN64
#define MP_SSIZE_MAX                _I64_MAX
#else
#define MP_SSIZE_MAX                _I32_MAX
#endif

// VC++ 12.0 fixes
#if (_MSC_VER <= 1800)
#define MICROPY_PY_MATH_ATAN2_FIX_INFNAN (1)
#define MICROPY_PY_MATH_FMOD_FIX_INFNAN (1)
#ifdef _WIN64
#define MICROPY_PY_MATH_MODF_FIX_NEGZERO (1)
#else
#define MICROPY_PY_MATH_POW_FIX_NAN (1)
#endif
#endif

// VC++ 2017 fixes
#if (_MSC_VER < 1920)
#define MICROPY_PY_MATH_COPYSIGN_FIX_NAN (1)
#endif

// CL specific definitions

#ifndef __cplusplus
#define restrict
#define inline                      __inline
#define alignof(t)                  __alignof(t)
#endif
#define PATH_MAX                    MICROPY_ALLOC_PATH_MAX
#define S_ISREG(m)                  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)                  (((m) & S_IFMT) == S_IFDIR)
#ifdef _WIN64
#define SSIZE_MAX                   _I64_MAX
typedef __int64 ssize_t;
#else
#define SSIZE_MAX                   _I32_MAX
typedef int ssize_t;
#endif
typedef mp_off_t off_t;


// Put static/global variables in sections with a known name
// This used to be required for GC, not the case anymore but keep it as it makes the map file easier to inspect
// For this to work this header must be included by all sources, which is the case normally
#define MICROPY_PORT_DATASECTION "upydata"
#define MICROPY_PORT_BSSSECTION "upybss"
#pragma data_seg(MICROPY_PORT_DATASECTION)
#pragma bss_seg(MICROPY_PORT_BSSSECTION)


// System headers (needed e.g. for nlr.h)

#include <stddef.h> // for NULL
#include <assert.h> // for assert

#endif
