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

#include <mpconfigboard.h>

#ifndef MICROPY_CONFIG_ROM_LEVEL

// Set default feature levels for each processor

#if defined(NRF51822)
#if defined(BLUETOOTH_SD)
// If SoftDevice is used there is less flash/ram available for application
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_MINIMUM)
#else
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
#endif

#elif defined(NRF52832)
  #define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)

#elif defined(NRF52840)
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#elif defined(NRF9160)
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#else
  #pragma error "Device not defined"
#endif

#endif // MICROPY_CONFIG_ROM_LEVEL

// pre-defined shortcuts to use below in #if queries or define values
#define CORE_FEAT (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
#define EXTRA_FEAT (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// options to control how MicroPython is built

#ifndef MICROPY_VFS
#define MICROPY_VFS                        (CORE_FEAT)
#endif

// micro:bit filesystem
#ifndef MICROPY_MBFS
#define MICROPY_MBFS                       (!MICROPY_VFS)
#endif

#ifndef MICROPY_ENABLE_SOURCE_LINE
#define MICROPY_ENABLE_SOURCE_LINE         (CORE_FEAT)
#endif

#ifndef MICROPY_PY_ARRAY_SLICE_ASSIGN
#define MICROPY_PY_ARRAY_SLICE_ASSIGN      (CORE_FEAT)
#endif

#ifndef MICROPY_PY_SYS_STDFILES
#define MICROPY_PY_SYS_STDFILES            (CORE_FEAT)
#endif

#ifndef MICROPY_PY_UBINASCII
#define MICROPY_PY_UBINASCII               (CORE_FEAT)
#endif

#ifndef MICROPY_PY_NRF
#define MICROPY_PY_NRF                     (CORE_FEAT)
#endif

#ifndef MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (CORE_FEAT)
#endif

#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB          (EXTRA_FEAT)
#endif

#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB   (EXTRA_FEAT)
#endif

#define MICROPY_ALLOC_PATH_MAX      (512)
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_READER_VFS          (MICROPY_VFS)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_INFO           (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#if NRF51
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)
#else
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#endif
#if NRF51
#define MICROPY_ALLOC_GC_STACK_SIZE (32)
#endif

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (0)

#if NRF51
    #define MICROPY_FATFS_MAX_SS       (1024)
#else
    #define MICROPY_FATFS_MAX_SS       (4096)
#endif

// Use port specific uos module rather than extmod variant.
#define MICROPY_PY_UOS              (0)

#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#if MICROPY_HW_USB_CDC_1200BPS_TOUCH
#define MICROPY_ENABLE_SCHEDULER    (1)
#define MICROPY_SCHEDULER_STATIC_NODES (1)
#endif
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_COMPILE (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT nrf5_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_MODULE_BUILTIN_INIT (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_URANDOM          (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_PULSE    (0)
#define MICROPY_PY_MACHINE_SOFTI2C  (MICROPY_PY_MACHINE_I2C)
#define MICROPY_PY_MACHINE_SPI      (0)
#define MICROPY_PY_MACHINE_SPI_MIN_DELAY (0)

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

#ifndef MICROPY_PY_MACHINE_TIMER_NRF
#define MICROPY_PY_MACHINE_TIMER_NRF (1)
#endif

#ifndef MICROPY_PY_MACHINE_RTCOUNTER
#define MICROPY_PY_MACHINE_RTCOUNTER (0)
#endif

#ifndef MICROPY_PY_TIME_TICKS
#define MICROPY_PY_TIME_TICKS       (1)
#endif

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE  (0)

// if sdk is in use, import configuration and enable some core features
#if BLUETOOTH_SD
#include "bluetooth_conf.h"
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (1)
#define MICROPY_COMP_CONST                    (1)
#define MICROPY_COMP_CONST_FOLDING            (1)
#define MICROPY_COMP_CONST_LITERAL            (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN      (1)
#define MICROPY_CPYTHON_COMPAT                (1)
#define MICROPY_ENABLE_COMPILER               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT        (1)
#define MICROPY_ERROR_REPORTING               (2)
#define MICROPY_FULL_CHECKS                   (1)
#define MICROPY_GC_ALLOC_THRESHOLD            (1)
#define MICROPY_MODULE_GETATTR                (1)
#define MICROPY_MULTIPLE_INHERITANCE          (1)
#define MICROPY_PY_ARRAY                      (1)
#define MICROPY_PY_ASSIGN_EXPR                (1)
#define MICROPY_PY_ASYNC_AWAIT                (1)
#define MICROPY_PY_ATTRTUPLE                  (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY         (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS     (1)
#define MICROPY_PY_BUILTINS_ENUMERATE         (1)
#define MICROPY_PY_BUILTINS_EVAL_EXEC         (1)
#define MICROPY_PY_BUILTINS_FILTER            (1)
#define MICROPY_PY_BUILTINS_MIN_MAX           (1)
#define MICROPY_PY_BUILTINS_PROPERTY          (1)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS       (1)
#define MICROPY_PY_BUILTINS_REVERSED          (1)
#define MICROPY_PY_BUILTINS_SET               (1)
#define MICROPY_PY_BUILTINS_SLICE             (1)
#define MICROPY_PY_BUILTINS_STR_COUNT         (1)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO     (1)
#define MICROPY_PY_COLLECTIONS                (1)
#define MICROPY_PY_GC                         (1)
#define MICROPY_PY_GENERATOR_PEND_THROW       (1)
#define MICROPY_PY_MATH                       (1)
#define MICROPY_PY_STRUCT                     (1)
#define MICROPY_PY_SYS                        (1)
#define MICROPY_PY_SYS_PATH_ARGV_DEFAULTS     (1)
#define MICROPY_PY___FILE__                   (1)
#define MICROPY_QSTR_BYTES_IN_HASH            (2)
#endif

#ifndef MICROPY_PY_UBLUEPY
#define MICROPY_PY_UBLUEPY                       (0)
#endif

#ifndef MICROPY_PY_BLE_NUS
#define MICROPY_PY_BLE_NUS                       (0)
#endif

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%u"
#define INT_FMT "%d"
#define HEX2_FMT "%02x"

typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
typedef long mp_off_t;

#if MICROPY_HW_ENABLE_RNG
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (rng_generate_random_word())
long unsigned int rng_generate_random_word(void);
#endif

#if BOARD_SPECIFIC_MODULES
#include "boardmodules.h"
#endif // BOARD_SPECIFIC_MODULES

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_help), MP_ROM_PTR(&mp_builtin_help_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) }, \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) }, \

#define MP_STATE_PORT MP_STATE_VM

#if MICROPY_HW_USB_CDC
#include "device/usbd.h"
#define MICROPY_HW_USBDEV_TASK_HOOK extern void tud_task(void); tud_task();
#else
#define MICROPY_HW_USBDEV_TASK_HOOK ;
#endif

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        MICROPY_HW_USBDEV_TASK_HOOK \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFI(); \
    } while (0);

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_PIN_DEFS_PORT_H "pin_defs_nrf5.h"

#ifndef MP_NEED_LOG2
#define MP_NEED_LOG2                (1)
#endif

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP()
#endif

#ifndef MICROPY_BOARD_ENTER_BOOTLOADER
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)
#endif

#ifndef MICROPY_BOARD_EARLY_INIT
#define MICROPY_BOARD_EARLY_INIT()
#endif
