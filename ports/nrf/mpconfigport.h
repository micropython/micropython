/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <mpconfigboard.h>

// options to control how MicroPython is built
#ifndef MICROPY_VFS
#define MICROPY_VFS                 (0)
#endif
#define MICROPY_VFS_FAT             (MICROPY_VFS)
#define MICROPY_ALLOC_PATH_MAX      (512)
#define MICROPY_PERSISTENT_CODE_LOAD (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_READER_VFS          (MICROPY_VFS)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_EMACS_KEYS     (0)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (0)
#define MICROPY_ENABLE_SOURCE_LINE  (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#if NRF51
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)
#else
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#endif
#if NRF51
#define MICROPY_ALLOC_GC_STACK_SIZE (32)
#endif

#define MICROPY_OPT_COMPUTED_GOTO   (0)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE (0)
#define MICROPY_OPT_MPZ_BITWISE     (0)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    (437) /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)

// TODO these should be generic, not bound to fatfs
#define mp_type_fileio fatfs_type_fileio
#define mp_type_textio fatfs_type_textio

// use vfs's functions for import stat and builtin open
#if MICROPY_VFS
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj
#endif

// Enable micro:bit filesystem by default.
#ifndef MICROPY_MBFS
#define MICROPY_MBFS (1)
#endif

#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (0)
#define MICROPY_PY_BUILTINS_STR_CENTER (0)
#define MICROPY_PY_BUILTINS_STR_PARTITION (0)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_EXECFILE (0)
#define MICROPY_PY_BUILTINS_COMPILE (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT nrf5_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_MODULE_BUILTIN_INIT (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
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
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_PULSE    (0)
#define MICROPY_PY_MACHINE_I2C_MAKE_NEW machine_hard_i2c_make_new
#define MICROPY_PY_MACHINE_SPI      (0)
#define MICROPY_PY_MACHINE_SPI_MIN_DELAY (0)
#define MICROPY_PY_FRAMEBUF         (0)

#ifndef MICROPY_HW_LED_COUNT
#define MICROPY_HW_LED_COUNT        (0)
#endif

#ifndef MICROPY_HW_LED_PULLUP
#define MICROPY_HW_LED_PULLUP       (0)
#endif

#ifndef MICROPY_PY_MUSIC
#define MICROPY_PY_MUSIC            (0)
#endif

#ifndef MICROPY_PY_MACHINE_ADC
#define MICROPY_PY_MACHINE_ADC      (0)
#endif

#ifndef MICROPY_PY_MACHINE_I2C
#define MICROPY_PY_MACHINE_I2C      (0)
#endif

#ifndef MICROPY_PY_MACHINE_HW_SPI
#define MICROPY_PY_MACHINE_HW_SPI   (1)
#endif

#ifndef MICROPY_PY_MACHINE_HW_PWM
#define MICROPY_PY_MACHINE_HW_PWM   (0)
#endif

#ifndef MICROPY_PY_MACHINE_SOFT_PWM
#define MICROPY_PY_MACHINE_SOFT_PWM (0)
#endif

#ifndef MICROPY_PY_MACHINE_TIMER
#define MICROPY_PY_MACHINE_TIMER    (0)
#endif

#ifndef MICROPY_PY_MACHINE_RTCOUNTER
#define MICROPY_PY_MACHINE_RTCOUNTER (0)
#endif

#ifndef MICROPY_PY_RANDOM_HW_RNG
#define MICROPY_PY_RANDOM_HW_RNG    (0)
#endif


#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE  (0)

// if sdk is in use, import configuration
#if BLUETOOTH_SD
#include "bluetooth_conf.h"
#endif

#ifndef MICROPY_PY_UBLUEPY
#define MICROPY_PY_UBLUEPY                       (0)
#endif

#ifndef MICROPY_PY_BLE_NUS
#define MICROPY_PY_BLE_NUS                       (0)
#endif

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

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t board_module;
extern const struct _mp_obj_module_t machine_module;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_ubluepy;
extern const struct _mp_obj_module_t music_module;
extern const struct _mp_obj_module_t random_module;

#if MICROPY_PY_UBLUEPY
#define UBLUEPY_MODULE                      { MP_ROM_QSTR(MP_QSTR_ubluepy), MP_ROM_PTR(&mp_module_ubluepy) },
#else
#define UBLUEPY_MODULE
#endif

#if MICROPY_PY_MUSIC
#define MUSIC_MODULE                        { MP_ROM_QSTR(MP_QSTR_music), MP_ROM_PTR(&music_module) },
#else
#define MUSIC_MODULE
#endif

#if MICROPY_PY_RANDOM_HW_RNG
#define RANDOM_MODULE                       { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&random_module) },
#else
#define RANDOM_MODULE
#endif

#if BOARD_SPECIFIC_MODULES
#include "boardmodules.h"
#define MICROPY_BOARD_BUILTINS BOARD_MODULES
#else
#define MICROPY_BOARD_BUILTINS
#endif // BOARD_SPECIFIC_MODULES

#if BLUETOOTH_SD

#if MICROPY_PY_BLE
extern const struct _mp_obj_module_t ble_module;
#define BLE_MODULE                        { MP_ROM_QSTR(MP_QSTR_ble), MP_ROM_PTR(&ble_module) },
#else
#define BLE_MODULE
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&board_module) }, \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&machine_module) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_utime) }, \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    BLE_MODULE \
    MUSIC_MODULE \
    UBLUEPY_MODULE \
    RANDOM_MODULE \
    MICROPY_BOARD_BUILTINS \


#else
extern const struct _mp_obj_module_t ble_module;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&board_module) }, \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&machine_module) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    MUSIC_MODULE \
    RANDOM_MODULE \
    MICROPY_BOARD_BUILTINS \


#endif // BLUETOOTH_SD

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_ROM_QSTR(MP_QSTR_os), MP_ROM_PTR(&mp_module_uos) }, \
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_utime) }, \

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_help), MP_ROM_PTR(&mp_builtin_help_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) }, \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&board_module) }, \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&machine_module) }, \
    BLE_MODULE \

#define MP_STATE_PORT MP_STATE_VM

#if MICROPY_PY_MUSIC
#define ROOT_POINTERS_MUSIC \
    struct _music_data_t *music_data;
#else
#define ROOT_POINTERS_MUSIC
#endif

#if MICROPY_PY_MACHINE_SOFT_PWM
#define ROOT_POINTERS_SOFTPWM \
    const struct _pwm_events *pwm_active_events; \
    const struct _pwm_events *pwm_pending_events;
#else
#define ROOT_POINTERS_SOFTPWM
#endif

#if defined(NRF52840_XXAA)
#define NUM_OF_PINS 48
#else
#define NUM_OF_PINS 32
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    mp_obj_t pin_irq_handlers[NUM_OF_PINS]; \
    \
    /* stdio is repeated on this UART object if it's not null */ \
    struct _machine_hard_uart_obj_t *board_stdio_uart; \
    \
    ROOT_POINTERS_MUSIC \
    ROOT_POINTERS_SOFTPWM \
    \
    /* micro:bit root pointers */ \
    void *async_data[2]; \

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_PIN_DEFS_PORT_H "pin_defs_nrf5.h"

#endif
