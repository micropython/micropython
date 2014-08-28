/*
 * This file is part of the Micro Python project, http://micropython.org/
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

// options to control how Micro Python is built

// Linking with GNU readline causes binary to be licensed under GPL
#ifndef MICROPY_USE_READLINE
#define MICROPY_USE_READLINE        (0)
#endif

#define MICROPY_ALLOC_PATH_MAX      (260) //see minwindef.h for msvc or limits.h for mingw
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_DEBUG_PRINTERS      (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_OPT_COMPUTED_GOTO   (0)
#define MICROPY_PY_BUILTINS_STR_UNICODE (0)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_PLATFORM     "win32"
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_CMATH            (1)
#define MICROPY_PY_IO_FILEIO        (1)
#define MICROPY_PY_GC_COLLECT_RETVAL (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_DETAILED)
#ifdef _MSC_VER
#define MICROPY_GCREGS_SETJMP       (1)
#endif

#define MICROPY_PORT_INIT_FUNC      init()
#define MICROPY_PORT_DEINIT_FUNC    deinit()

// type definitions for the specific machine

#if defined( __MINGW32__ ) && defined( __LP64__ )
typedef long mp_int_t; // must be pointer size
typedef unsigned long mp_uint_t; // must be pointer size
#elif defined ( _MSC_VER ) && defined( _WIN64 )
typedef __int64 mp_int_t;
typedef unsigned __int64 mp_uint_t;
#else
// These are definitions for machines where sizeof(int) == sizeof(void*),
// regardless for actual size.
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif

#define BYTES_PER_WORD sizeof(mp_int_t)

typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size

extern const struct _mp_obj_fun_builtin_t mp_builtin_open_obj;
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

extern const struct _mp_obj_module_t mp_module_time;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&mp_module_time }, \

// We need to provide a declaration/definition of alloca()
#include <malloc.h>

#include "realpath.h"
#include "init.h"

// sleep for given number of milliseconds
void msec_sleep(double msec);

// MSVC specifics
#ifdef _MSC_VER

// Sanity check

#if ( _MSC_VER < 1800 )
    #error Can only build with Visual Studio 2013 toolset
#endif


// CL specific overrides from mpconfig

#define NORETURN                   __declspec(noreturn)
#define MICROPY_PORT_CONSTANTS     { "dummy", 0 } //can't have zero-sized array


// CL specific definitions

#define restrict
#define inline                      __inline
#define alignof(t)                  __alignof(t)
#define STDIN_FILENO                0
#define STDOUT_FILENO               1
#define STDERR_FILENO               2
#define PATH_MAX                    MICROPY_ALLOC_PATH_MAX
#define S_ISREG(m)                  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)                  (((m) & S_IFMT) == S_IFDIR)


// Put static/global variables in sections with a known name we can lookup for the GC
// For this to work this header must be included by all sources, which is the case normally
#define MICROPY_PORT_DATASECTION "upydata"
#define MICROPY_PORT_BSSSECTION "upybss"
#pragma data_seg(MICROPY_PORT_DATASECTION)
#pragma bss_seg(MICROPY_PORT_BSSSECTION)


// System headers (needed e.g. for nlr.h)

#include <stddef.h> //for NULL
#include <assert.h> //for assert

// Functions implemented in platform code

int snprintf(char *dest, size_t count, const char *format, ...);
#endif

// MingW specifics
#ifdef __MINGW32__
#define MICROPY_PORT_BSSSECTION ".bss"
#endif
