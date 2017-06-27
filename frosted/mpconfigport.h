/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#define MICROPY_PY_IO_BUFFEREDWRITER (1)

#define MICROPY_ALLOC_QSTR_CHUNK_INIT (64)
#define MICROPY_ALLOC_PARSE_RULE_INIT (8)
#define MICROPY_ALLOC_PARSE_RULE_INC  (8)
#define MICROPY_ALLOC_PARSE_RESULT_INIT (8)
#define MICROPY_ALLOC_PARSE_RESULT_INC (8)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (64)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_ENABLE_FINALISER    (0)
#define MICROPY_STACK_CHECK         (0)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_EMACS_KEYS     (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_HELPER_LEXER_UNIX   (1)
#define MICROPY_ENABLE_SOURCE_LINE  (0)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_WARNINGS            (0)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (0)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_STREAMS_NON_BLOCK   (0)
#define MICROPY_OPT_COMPUTED_GOTO   (0)
#define MICROPY_CAN_OVERRIDE_BUILTINS (0)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_CPYTHON_COMPAT      (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
#define MICROPY_PY_BUILTINS_COMPILE (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)
#define MICROPY_PY_BUILTINS_STR_UNICODE (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY___FILE__         (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (0)
#define MICROPY_READER_POSIX        (1)
#define MICROPY_PY_GC               (0)
#define MICROPY_PY_GC_COLLECT_RETVAL (0)
#define MICROPY_PY_ARRAY            (0)
#define MICROPY_PY_COLLECTIONS      (1)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_IO_FILEIO        (1)
#define MICROPY_PY_STRUCT           (0)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_PLATFORM     "frosted"
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_UTIME            (1)
#define MICROPY_PY_UERRNO           (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UZLIB            (0)
#define MICROPY_PY_UJSON            (0)
#define MICROPY_PY_URE              (0)
#define MICROPY_PY_UHEAPQ           (0)
#define MICROPY_PY_UHASHLIB         (0)
#define MICROPY_PY_UBINASCII        (0)
#define MICROPY_PY_MACHINE          (0)
#define MICROPY_PY_MACHINE_PULSE    (0)


#define MICROPY_PORT_ROOT_POINTERS \
    mp_obj_t keyboard_interrupt_obj;

//////////////////////////////////////////
// Do not change anything beyond this line
//////////////////////////////////////////

// Define to 1 to use undertested inefficient GC helper implementation
// (if more efficient arch-specific one is not available).
#ifndef MICROPY_GCREGS_SETJMP
    #ifdef __mips__
        #define MICROPY_GCREGS_SETJMP (1)
    #else
        #define MICROPY_GCREGS_SETJMP (0)
    #endif
#endif

extern const struct _mp_obj_module_t mp_module_os;
extern const struct _mp_obj_module_t mp_module_uos_vfs;
extern const struct _mp_obj_module_t mp_module_uselect;
extern const struct _mp_obj_module_t mp_module_time;
extern const struct _mp_obj_module_t mp_module_termios;
extern const struct _mp_obj_module_t mp_module_socket;
extern const struct _mp_obj_module_t mp_module_ffi;
extern const struct _mp_obj_module_t mp_module_jni;

#if MICROPY_PY_UOS_VFS
#define MICROPY_PY_UOS_VFS_DEF { MP_ROM_QSTR(MP_QSTR_uos_vfs), MP_ROM_PTR(&mp_module_uos_vfs) },
#else
#define MICROPY_PY_UOS_VFS_DEF
#endif
#if MICROPY_PY_FFI
#define MICROPY_PY_FFI_DEF { MP_ROM_QSTR(MP_QSTR_ffi), MP_ROM_PTR(&mp_module_ffi) },
#else
#define MICROPY_PY_FFI_DEF
#endif
#if MICROPY_PY_JNI
#define MICROPY_PY_JNI_DEF { MP_ROM_QSTR(MP_QSTR_jni), MP_ROM_PTR(&mp_module_jni) },
#else
#define MICROPY_PY_JNI_DEF
#endif
#if MICROPY_PY_UTIME
#define MICROPY_PY_UTIME_DEF { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_time) },
#else
#define MICROPY_PY_UTIME_DEF
#endif
#if MICROPY_PY_TERMIOS
#define MICROPY_PY_TERMIOS_DEF { MP_ROM_QSTR(MP_QSTR_termios), MP_ROM_PTR(&mp_module_termios) },
#else
#define MICROPY_PY_TERMIOS_DEF
#endif
#if MICROPY_PY_SOCKET
#define MICROPY_PY_SOCKET_DEF { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_socket) },
#else
#define MICROPY_PY_SOCKET_DEF
#endif
#if MICROPY_PY_USELECT
#define MICROPY_PY_USELECT_DEF { MP_ROM_QSTR(MP_QSTR_uselect), MP_ROM_PTR(&mp_module_uselect) },
#else
#define MICROPY_PY_USELECT_DEF
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_PY_FFI_DEF \
    MICROPY_PY_JNI_DEF \
    MICROPY_PY_UTIME_DEF \
    MICROPY_PY_SOCKET_DEF \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_os) }, \
    MICROPY_PY_UOS_VFS_DEF \
    MICROPY_PY_USELECT_DEF \
    MICROPY_PY_TERMIOS_DEF \

// type definitions for the specific machine

// assume that if we already defined the obj repr then we also defined types
#ifndef MICROPY_OBJ_REPR
#ifdef __LP64__
typedef long mp_int_t; // must be pointer size
typedef unsigned long mp_uint_t; // must be pointer size
#else
// These are definitions for machines where sizeof(int) == sizeof(void*),
// regardless of actual size.
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif
#endif

#define BYTES_PER_WORD sizeof(mp_int_t)

// Cannot include <sys/types.h>, as it may lead to symbol name clashes
#if _FILE_OFFSET_BITS == 64 && !defined(__LP64__)
typedef long long mp_off_t;
#else
typedef long mp_off_t;
#endif

void mp_unix_alloc_exec(mp_uint_t min_size, void** ptr, mp_uint_t *size);
void mp_unix_free_exec(void *ptr, mp_uint_t size);
void mp_unix_mark_exec(void);
#define MP_PLAT_ALLOC_EXEC(min_size, ptr, size) mp_unix_alloc_exec(min_size, ptr, size)
#define MP_PLAT_FREE_EXEC(ptr, size) mp_unix_free_exec(ptr, size)
#ifndef MICROPY_FORCE_PLAT_ALLOC_EXEC
// Use MP_PLAT_ALLOC_EXEC for any executable memory allocation, including for FFI
// (overriding libffi own implementation)
#define MICROPY_FORCE_PLAT_ALLOC_EXEC (1)
#endif

#if MICROPY_PY_OS_DUPTERM
#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)
#else
#include <unistd.h>
#define MP_PLAT_PRINT_STRN(str, len) do { ssize_t ret = write(1, str, len); (void)ret; } while (0)
#endif




#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_input), MP_ROM_PTR(&mp_builtin_input_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

#define MP_STATE_PORT MP_STATE_VM

// We need to provide a declaration/definition of alloca()
#ifdef __FreeBSD__
#include <stdlib.h>
#else
#include <alloca.h>
#endif
