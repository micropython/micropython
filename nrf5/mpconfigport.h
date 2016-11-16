/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#ifndef NRF5_MPCONFIGPORT_H__
#define NRF5_MPCONFIGPORT_H__

#include <stdint.h>

// options to control how Micro Python is built
// options to control how Micro Python is built

#define MICROPY_ALLOC_PATH_MAX      (512)
#define MICROPY_PERSISTENT_CODE_LOAD (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (0)
#define MICROPY_STACK_CHECK         (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_EMACS_KEYS     (0)
#define MICROPY_REPL_AUTO_INDENT    (0)
#define MICROPY_ENABLE_SOURCE_LINE  (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_NONE)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_OPT_COMPUTED_GOTO   (0)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE (0)
#define MICROPY_OPT_MPZ_BITWISE     (0)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (0)
#define MICROPY_FATFS_LFN_CODE_PAGE    (437) /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (0)
#define MICROPY_FATFS_RPATH            (0)
#define MICROPY_FATFS_VOLUMES          (0)
#define MICROPY_FATFS_MULTI_PARTITION  (0)
#define MICROPY_FSUSERMOUNT            (0)

#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (0)
#define MICROPY_PY_BUILTINS_STR_CENTER (0)
#define MICROPY_PY_BUILTINS_STR_PARTITION (0)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#define MICROPY_PY_BUILTINS_EXECFILE (0)
#define MICROPY_PY_BUILTINS_COMPILE (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (0)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (0)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (0)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_STDFILES     (0)
#define MICROPY_PY_SYS_STDIO_BUFFER (0)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (0)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_IO_FILEIO        (0)
#define MICROPY_PY_UERRNO           (0)
#define MICROPY_PY_UBINASCII        (0)
#define MICROPY_PY_URANDOM          (0)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (0)
#define MICROPY_PY_UCTYPES          (0)
#define MICROPY_PY_UZLIB            (0)
#define MICROPY_PY_UJSON            (0)
#define MICROPY_PY_URE              (0)
#define MICROPY_PY_UHEAPQ           (0)
#define MICROPY_PY_UHASHLIB         (0)
#define MICROPY_PY_UTIME_MP_HAL     (0)
#define MICROPY_PY_MACHINE          (0)
#define MICROPY_PY_MACHINE_PULSE    (0)
#define MICROPY_PY_MACHINE_I2C      (0)
#define MICROPY_PY_MACHINE_SPI      (0)
#define MICROPY_PY_MACHINE_SPI_MIN_DELAY (0)
#define MICROPY_PY_FRAMEBUF         (0)

#ifndef MICROPY_PY_USOCKET
#define MICROPY_PY_USOCKET          (0)
#endif

#ifndef MICROPY_PY_NETWORK
#define MICROPY_PY_NETWORK          (0)
#endif

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE  (0)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%u"
#define INT_FMT "%d"
#define HEX2_FMT "%02x"

typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
typedef long mp_off_t;

// board specific definitions
#include "mpconfigboard.h"

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t pyb_module;

#if BLUETOOTH_SD
extern const struct _mp_obj_module_t ble_module;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ble), (mp_obj_t)&ble_module }, \

#else
extern const struct _mp_obj_module_t ble_module;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \

#endif // BLUETOOTH_SD

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj }, \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ble), (mp_obj_t)&ble_module }, \

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t pyb_config_main; \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    /* Used to do callbacks to Python code on interrupt */ \
    struct _pyb_timer_obj_t *pyb_timer_obj_all[14]; \
    \
    /* stdio is repeated on this UART object if it's not null */ \
    struct _pyb_uart_obj_t *pyb_stdio_uart; \
    \
    /* pointers to all UART objects (if they have been created) */ \
    struct _pyb_uart_obj_t *pyb_uart_obj_all[1]; \

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#include "mpconfigboard.h"

#endif
