/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <stdint.h>

#ifndef BOOTLOADER
#include "FreeRTOS.h"
#include "semphr.h"
#endif

// options to control how MicroPython is built

#define MICROPY_GC_STACK_ENTRY_TYPE                 uint16_t
#define MICROPY_ALLOC_PATH_MAX                      (128)
#define MICROPY_PERSISTENT_CODE_LOAD                (1)
#define MICROPY_EMIT_THUMB                          (0)
#define MICROPY_EMIT_INLINE_THUMB                   (0)
#define MICROPY_COMP_CONST_LITERAL                  (0)
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
#define MICROPY_FATFS_LFN_CODE_PAGE                 437 // 1=SFN/ANSI 437=LFN/U.S.(OEM)
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
#define MICROPY_PY_BUILTINS_BYTES_HEX               (1)
#define MICROPY_PY_BUILTINS_INPUT                   (1)
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
#define MICROPY_PY_UERRNO                           (1)
#define MICROPY_PY_UERRNO_ERRORCODE                 (0)
#define MICROPY_PY_THREAD                           (1)
#define MICROPY_PY_THREAD_GIL                       (1)
#define MICROPY_PY_UBINASCII                        (1)
#define MICROPY_PY_UCTYPES                          (0)
#define MICROPY_PY_UZLIB                            (0)
#define MICROPY_PY_UJSON                            (1)
#define MICROPY_PY_URE                              (1)
#define MICROPY_PY_UHEAPQ                           (0)
#define MICROPY_PY_UHASHLIB                         (0)
#define MICROPY_PY_USELECT                          (1)
#define MICROPY_PY_UTIME                            (1)
#define MICROPY_PY_UTIME_GMTIME_LOCALTIME_MKTIME    (1)
#define MICROPY_PY_UTIME_TIME_TIME_NS               (1)
#define MICROPY_PY_UTIME_INCLUDEFILE                "ports/cc3200/mods/modutime.c"

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF      (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE        (0)
#define MICROPY_KBD_EXCEPTION                       (1)

// We define our own list of errno constants to include in uerrno module
#define MICROPY_PY_UERRNO_LIST \
    X(EPERM) \
    X(EIO) \
    X(ENODEV) \
    X(EINVAL) \
    X(ETIMEDOUT) \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_umachine),     MP_ROM_PTR(&mp_module_machine) },  \

#define MP_STATE_PORT MP_STATE_VM

// type definitions for the specific machine
#define MICROPY_MAKE_POINTER_CALLABLE(p)            ((void *)((mp_uint_t)(p) | 1))
#define MP_SSIZE_MAX                                (0x7FFFFFFF)

#define UINT_FMT                                    "%u"
#define INT_FMT                                     "%d"

typedef int32_t mp_int_t;                           // must be pointer size
typedef unsigned int mp_uint_t;                     // must be pointer size
typedef long mp_off_t;

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
