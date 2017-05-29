/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#include <stdint.h>

#ifndef BOOTLOADER
#include "FreeRTOS.h"
#include "semphr.h"
#endif

// options to control how Micro Python is built

#define MICROPY_ALLOC_PATH_MAX                      (128)
#define MICROPY_PERSISTENT_CODE_LOAD                (1)
#define MICROPY_EMIT_THUMB                          (0)
#define MICROPY_EMIT_INLINE_THUMB                   (0)
#define MICROPY_COMP_MODULE_CONST                   (1)
#define MICROPY_ENABLE_GC                           (1)
#define MICROPY_ENABLE_FINALISER                    (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN            (0)
#define MICROPY_STACK_CHECK                         (0)
#define MICROPY_HELPER_REPL                         (1)
#define MICROPY_ENABLE_SOURCE_LINE                  (1)
#define MICROPY_ENABLE_DOC_STRING                   (0)
#define MICROPY_REPL_AUTO_INDENT                    (1)
#define MICROPY_ERROR_REPORTING                     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_LONGINT_IMPL                        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                          (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_OPT_COMPUTED_GOTO                   (0)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE    (0)
#define MICROPY_READER_VFS                          (1)
#ifndef DEBUG // we need ram on the launchxl while debugging
#define MICROPY_CPYTHON_COMPAT                      (1)
#else
#define MICROPY_CPYTHON_COMPAT                      (0)
#endif
#define MICROPY_QSTR_BYTES_IN_HASH                  (1)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN                    (2)
#define MICROPY_FATFS_MAX_LFN                       (MICROPY_ALLOC_PATH_MAX)
#define MICROPY_FATFS_LFN_CODE_PAGE                 (437) // 1=SFN/ANSI 437=LFN/U.S.(OEM)
#define MICROPY_FATFS_RPATH                         (2)
#define MICROPY_FATFS_REENTRANT                     (1)
#define MICROPY_FATFS_TIMEOUT                       (2500)
#define MICROPY_FATFS_SYNC_T                        SemaphoreHandle_t

#define MICROPY_STREAMS_NON_BLOCK                   (1)
#define MICROPY_MODULE_WEAK_LINKS                   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS               (1)
#define MICROPY_USE_INTERNAL_ERRNO                  (1)
#define MICROPY_VFS                                 (1)
#define MICROPY_VFS_FAT                             (1)
#define MICROPY_PY_ASYNC_AWAIT (0)
#define MICROPY_PY_ALL_SPECIAL_METHODS              (1)
#define MICROPY_PY_BUILTINS_HELP                    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT               cc3200_help_text
#ifndef DEBUG
#define MICROPY_PY_BUILTINS_STR_UNICODE             (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES          (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW              (1)
#define MICROPY_PY_BUILTINS_FROZENSET               (1)
#define MICROPY_PY_BUILTINS_EXECFILE                (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN               (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT          (1)
#else
#define MICROPY_PY_BUILTINS_STR_UNICODE             (0)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES          (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW              (0)
#define MICROPY_PY_BUILTINS_FROZENSET               (0)
#define MICROPY_PY_BUILTINS_EXECFILE                (0)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN               (0)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT          (0)
#endif
#define MICROPY_PY_MICROPYTHON_MEM_INFO             (0)
#define MICROPY_PY_SYS_MAXSIZE                      (1)
#define MICROPY_PY_SYS_EXIT                         (1)
#define MICROPY_PY_SYS_STDFILES                     (1)
#define MICROPY_PY_CMATH                            (0)
#define MICROPY_PY_IO                               (1)
#define MICROPY_PY_IO_FILEIO                        (1)
#define MICROPY_PY_UERRNO                           (1)
#define MICROPY_PY_UERRNO_ERRORCODE                 (0)
#define MICROPY_PY_THREAD                           (1)
#define MICROPY_PY_THREAD_GIL                       (1)
#define MICROPY_PY_UBINASCII                        (0)
#define MICROPY_PY_UCTYPES                          (0)
#define MICROPY_PY_UZLIB                            (0)
#define MICROPY_PY_UJSON                            (1)
#define MICROPY_PY_URE                              (1)
#define MICROPY_PY_UHEAPQ                           (0)
#define MICROPY_PY_UHASHLIB                         (0)
#define MICROPY_PY_USELECT                          (1)
#define MICROPY_PY_UTIME_MP_HAL                     (1)

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF      (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE        (0)

// We define our own list of errno constants to include in uerrno module
#define MICROPY_PY_UERRNO_LIST \
    X(EPERM) \
    X(EIO) \
    X(ENODEV) \
    X(EINVAL) \
    X(ETIMEDOUT) \

// TODO these should be generic, not bound to fatfs
#define mp_type_fileio fatfs_type_fileio
#define mp_type_textio fatfs_type_textio

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj },  \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open),  (mp_obj_t)&mp_builtin_open_obj },   \

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t machine_module;
extern const struct _mp_obj_module_t wipy_module;
extern const struct _mp_obj_module_t mp_module_ure;
extern const struct _mp_obj_module_t mp_module_ujson;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_uselect;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_network;
extern const struct _mp_obj_module_t mp_module_ubinascii;
extern const struct _mp_obj_module_t mp_module_ussl;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_umachine),     (mp_obj_t)&machine_module },      \
	{ MP_OBJ_NEW_QSTR(MP_QSTR_wipy),        (mp_obj_t)&wipy_module },         \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uos),         (mp_obj_t)&mp_module_uos },       \
    { MP_OBJ_NEW_QSTR(MP_QSTR_utime),       (mp_obj_t)&mp_module_utime },     \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uselect),     (mp_obj_t)&mp_module_uselect },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_usocket),     (mp_obj_t)&mp_module_usocket },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_network),     (mp_obj_t)&mp_module_network },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ubinascii),   (mp_obj_t)&mp_module_ubinascii }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ussl),        (mp_obj_t)&mp_module_ussl },      \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_errno),       (mp_obj_t)&mp_module_uerrno },    \
    { MP_OBJ_NEW_QSTR(MP_QSTR_struct),      (mp_obj_t)&mp_module_ustruct },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_re),          (mp_obj_t)&mp_module_ure },       \
    { MP_OBJ_NEW_QSTR(MP_QSTR_json),        (mp_obj_t)&mp_module_ujson },     \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os),          (mp_obj_t)&mp_module_uos },       \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time),        (mp_obj_t)&mp_module_utime },     \
    { MP_OBJ_NEW_QSTR(MP_QSTR_select),      (mp_obj_t)&mp_module_uselect },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket),      (mp_obj_t)&mp_module_usocket },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_binascii),    (mp_obj_t)&mp_module_ubinascii }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ssl),         (mp_obj_t)&mp_module_ussl },      \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine),     (mp_obj_t)&machine_module },      \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_umachine),     (mp_obj_t)&machine_module },      \

// vm state and root pointers for the gc
#define MP_STATE_PORT MP_STATE_VM
#define MICROPY_PORT_ROOT_POINTERS                                        \
    const char *readline_hist[8];                                         \
    mp_obj_t mp_const_user_interrupt;                                     \
    mp_obj_t machine_config_main;                                         \
    mp_obj_list_t pyb_sleep_obj_list;                                     \
    mp_obj_list_t mp_irq_obj_list;                                        \
    mp_obj_list_t pyb_timer_channel_obj_list;                             \
    struct _pyb_uart_obj_t *pyb_uart_objs[2];                             \
    struct _os_term_dup_obj_t *os_term_dup_obj;                           \


// type definitions for the specific machine
#define MICROPY_MAKE_POINTER_CALLABLE(p)            ((void*)((mp_uint_t)(p) | 1))
#define MP_SSIZE_MAX                                (0x7FFFFFFF)

#define UINT_FMT                                    "%u"
#define INT_FMT                                     "%d"

typedef int32_t         mp_int_t;                   // must be pointer size
typedef unsigned int    mp_uint_t;                  // must be pointer size
typedef long            mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

#define MICROPY_BEGIN_ATOMIC_SECTION()              disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)           enable_irq(state)
#define MICROPY_EVENT_POLL_HOOK                     __WFI();

// assembly functions to handle critical sections, interrupt
// disabling/enabling and sleep mode enter/exit
#include "cc3200_asm.h"

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// Include board specific configuration
#include "mpconfigboard.h"

#define MICROPY_MPHALPORT_H                         "cc3200_hal.h"
#define MICROPY_PORT_HAS_TELNET                     (1)
#define MICROPY_PORT_HAS_FTP                        (1)
#define MICROPY_PY_SYS_PLATFORM                     "WiPy"

#define MICROPY_PORT_WLAN_AP_SSID                   "wipy-wlan"
#define MICROPY_PORT_WLAN_AP_KEY                    "www.wipy.io"
#define MICROPY_PORT_WLAN_AP_SECURITY               SL_SEC_TYPE_WPA_WPA2
#define MICROPY_PORT_WLAN_AP_CHANNEL                5

#endif // __INCLUDED_MPCONFIGPORT_H
