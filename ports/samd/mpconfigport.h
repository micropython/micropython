/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

// Board specific definitions
#include "mpconfigboard.h"

// Memory allocation policies
#define MICROPY_GC_STACK_ENTRY_TYPE         uint16_t
#define MICROPY_GC_ALLOC_THRESHOLD          (0)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (32)
#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_QSTR_BYTES_IN_HASH          (1)

// Compiler configuration
#define MICROPY_COMP_CONST                  (0)

// Python internal features
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_KBD_EXCEPTION               (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_CPYTHON_COMPAT              (0)
#define MICROPY_CAN_OVERRIDE_BUILTINS       (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       samd_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)

// fixes sys/usys import issue
#define MICROPY_MODULE_WEAK_LINKS           (1)
// Control over Python builtins
#define MICROPY_PY_ASYNC_AWAIT              (0)
#define MICROPY_PY_BUILTINS_STR_COUNT       (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_SET             (0)
#define MICROPY_PY_BUILTINS_FROZENSET       (0)
#define MICROPY_PY_BUILTINS_PROPERTY        (0)
#define MICROPY_PY_BUILTINS_ENUMERATE       (0)
#define MICROPY_PY_BUILTINS_FILTER          (0)
#define MICROPY_PY_BUILTINS_REVERSED        (0)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED  (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (0)
#define MICROPY_PY___FILE__                 (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN       (1)
#define MICROPY_PY_ATTRTUPLE                (0)
#define MICROPY_PY_COLLECTIONS              (0)
#define MICROPY_PY_SYS                      (1)
#define MICROPY_PY_SYS_PLATFORM             "samd"
#define MICROPY_PY_SYS_EXIT                 (1)
#define MICROPY_PY_SYS_STDFILES             (1)
#define MICROPY_PY_SYS_MAXSIZE              (1)
#define MICROPY_PY_IO_FILEIO                (1)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)

// Extended modules
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_UOS                      (1)
#define MICROPY_READER_VFS                  (1)
#define MICROPY_VFS                         (1)
#define MICROPY_PY_UJSON                    (1)
#define MICROPY_PY_URE                      (1)
#define MICROPY_PY_UBINASCII                (1)
#define MICROPY_PY_UCTYPES                  (1)
#define MICROPY_PY_UHEAPQ                   (1)
#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_UZLIB                    (1)
#define MICROPY_PY_UASYNCIO                 (1)

// Use VfsLfs's types for fileio/textio
#define mp_type_fileio mp_type_vfs_lfs1_fileio
#define mp_type_textio mp_type_vfs_lfs1_textio

// Use VFS's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open_obj mp_vfs_open_obj

// Hooks to add builtins

#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_samd;
extern const struct _mp_obj_module_t mp_module_utime;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_samd), MP_ROM_PTR(&mp_module_samd) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFI(); \
    } while (0);

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
