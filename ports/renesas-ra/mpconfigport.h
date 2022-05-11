/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// board specific definitions
#include "mpconfigboard.h"
#include "mpconfigboard_common.h"

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// memory allocation policies
#ifndef MICROPY_GC_STACK_ENTRY_TYPE
#if MICROPY_HW_SDRAM_SIZE
#define MICROPY_GC_STACK_ENTRY_TYPE uint32_t
#else
#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t
#endif
#endif
#define MICROPY_ALLOC_PATH_MAX      (128)

// optimisations
#ifndef MICROPY_OPT_COMPUTED_GOTO
#define MICROPY_OPT_COMPUTED_GOTO   (1)
#endif

// Don't enable lookup cache on M0 (low RAM)
#ifndef MICROPY_OPT_MAP_LOOKUP_CACHE
#define MICROPY_OPT_MAP_LOOKUP_CACHE (__CORTEX_M > 0)
#endif

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB          (1)
#endif
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB   (1)
#endif

// Python internal features
#define MICROPY_READER_VFS          (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE (0)
#define MICROPY_REPL_INFO           (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#ifndef MICROPY_FLOAT_IMPL // can be configured by each board via mpconfigboard.mk
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#endif
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_SCHEDULER_DEPTH     (8)
#define MICROPY_VFS                 (1)

// control over Python builtins
#ifndef MICROPY_PY_BUILTINS_HELP_TEXT
#define MICROPY_PY_BUILTINS_HELP_TEXT ra_help_text
#endif
#define MICROPY_PY_IO_FILEIO        (MICROPY_VFS_FAT || MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2)
#ifndef MICROPY_PY_SYS_PLATFORM     // let boards override it if they want
#define MICROPY_PY_SYS_PLATFORM     "renesas-ra"
#endif
#ifndef MICROPY_PY_THREAD
#define MICROPY_PY_THREAD           (0)
#endif

// extended modules
#define MICROPY_PY_UOS_INCLUDEFILE  "ports/renesas-ra/moduos.c"
#define MICROPY_PY_OS_DUPTERM       (3)
#define MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM (1)
#define MICROPY_PY_UOS_DUPTERM_STREAM_DETACHED_ATTACHED (1)
#define MICROPY_PY_UOS_SEP          (1)
#define MICROPY_PY_UOS_SYNC         (1)
#define MICROPY_PY_UOS_UNAME        (1)
#define MICROPY_PY_UOS_URANDOM      (MICROPY_HW_ENABLE_RNG)
#ifndef MICROPY_PY_UTIME
#define MICROPY_PY_UTIME            (1)
#endif
#define MICROPY_PY_UTIME_MP_HAL     (MICROPY_PY_UTIME)
#ifndef MICROPY_PY_UTIMEQ
#define MICROPY_PY_UTIMEQ           (1)
#endif
#ifndef MICROPY_PY_MACHINE
#define MICROPY_PY_MACHINE          (1)
#ifndef MICROPY_PY_MACHINE_BITSTREAM
#define MICROPY_PY_MACHINE_BITSTREAM (1)
#endif
#define MICROPY_PY_MACHINE_PULSE    (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_SOFTI2C  (1)
#define MICROPY_PY_MACHINE_SPI      (1)
#define MICROPY_PY_MACHINE_SPI_MSB  (SPI_FIRSTBIT_MSB)
#define MICROPY_PY_MACHINE_SPI_LSB  (SPI_FIRSTBIT_LSB)
#define MICROPY_PY_MACHINE_SOFTSPI  (1)
#endif
#define MICROPY_HW_SOFTSPI_MIN_DELAY (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE (48000000 / 48)
#ifndef MICROPY_PY_ONEWIRE
#define MICROPY_PY_ONEWIRE          (1)
#endif
#ifndef MICROPY_PY_UPLATFORM
#define MICROPY_PY_UPLATFORM        (1)
#endif

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)

// TODO these should be generic, not bound to a particular FS implementation
#if MICROPY_VFS_FAT
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio
#elif MICROPY_VFS_LFS1
#define mp_type_fileio mp_type_vfs_lfs1_fileio
#define mp_type_textio mp_type_vfs_lfs1_textio
#elif MICROPY_VFS_LFS2
#define mp_type_fileio mp_type_vfs_lfs2_fileio
#define mp_type_textio mp_type_vfs_lfs2_textio
#endif

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t mp_module_ubinascii;
extern const struct _mp_obj_module_t mp_module_ure;
extern const struct _mp_obj_module_t mp_module_uzlib;
extern const struct _mp_obj_module_t mp_module_ujson;
extern const struct _mp_obj_module_t mp_module_uheapq;
extern const struct _mp_obj_module_t mp_module_uhashlib;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_onewire;

#if MICROPY_PY_MACHINE
#define MACHINE_BUILTIN_MODULE_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) },
#else
#define MACHINE_BUILTIN_MODULE_CONSTANTS
#endif

#if MICROPY_PY_UTIME
#define UTIME_BUILTIN_MODULE                { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) },
#else
#define UTIME_BUILTIN_MODULE
#endif

#if MICROPY_PY_ONEWIRE
#define ONEWIRE_BUILTIN_MODULE              { MP_ROM_QSTR(MP_QSTR__onewire), MP_ROM_PTR(&mp_module_onewire) },
#else
#define ONEWIRE_BUILTIN_MODULE
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    UTIME_BUILTIN_MODULE \
    ONEWIRE_BUILTIN_MODULE \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    MACHINE_BUILTIN_MODULE_CONSTANTS \

#define MP_STATE_PORT MP_STATE_VM

#ifndef MICROPY_BOARD_ROOT_POINTERS
#define MICROPY_BOARD_ROOT_POINTERS
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    \
    mp_obj_t pyb_hid_report_desc; \
    \
    mp_obj_t pyb_config_main; \
    \
    mp_obj_t pyb_switch_callback; \
    \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    \
    mp_obj_t pyb_extint_callback[PYB_EXTI_NUM_VECTORS]; \
    \
    /* pointers to all Timer objects (if they have been created) */ \
    struct _pyb_timer_obj_t *pyb_timer_obj_all[MICROPY_HW_MAX_TIMER]; \
    \
    /* stdio is repeated on this UART object if it's not null */ \
    struct _machine_uart_obj_t *pyb_stdio_uart; \
    \
    /* pointers to all UART objects (if they have been created) */ \
    struct _machine_uart_obj_t *machine_uart_obj_all[MICROPY_HW_MAX_UART + MICROPY_HW_MAX_LPUART]; \
    \
    /* list of registered NICs */ \
    /* mp_obj_list_t mod_network_nic_list; */ \
    \
    /* root pointers for sub-systems */ \
    \
    /* root pointers defined by a board */ \
    MICROPY_BOARD_ROOT_POINTERS \

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((uint32_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

// Assume that if we already defined the obj repr then we also defined these items
#ifndef MICROPY_OBJ_REPR
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif

typedef long mp_off_t;

// We have inlined IRQ functions for efficiency (they are generally
// 1 machine instruction).
//
// Note on IRQ state: you should not need to know the specific
// value of the state variable, but rather just pass the return
// value from disable_irq back to enable_irq.  If you really need
// to know the machine-specific values, see irq.h.

static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        if (pyb_thread_enabled) { \
            MP_THREAD_GIL_EXIT(); \
            pyb_thread_yield(); \
            MP_THREAD_GIL_ENTER(); \
        } else { \
            __WFI(); \
        } \
    } while (0);

#define MICROPY_THREAD_YIELD() pyb_thread_yield()
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFI(); \
    } while (0);

#define MICROPY_THREAD_YIELD()
#endif

// We need an implementation of the log2 function which is not a macro
#define MP_NEED_LOG2 (1)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>
