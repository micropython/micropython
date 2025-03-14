/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include <stdint.h>
#include <alloca.h> // for alloca()

// board specific definitions
#include "mpconfigboard.h"

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)
#endif

// Select the low-level system tick implementation.
#if !defined(MICROPY_HW_SYSTEM_TICK_USE_SYSTICK) \
    && !defined(MICROPY_HW_SYSTEM_TICK_USE_LPTIMER) \
    && !defined(MICROPY_HW_SYSTEM_TICK_USE_UTIMER)
#if CORE_M55_HP
#define MICROPY_HW_SYSTEM_TICK_USE_UTIMER       (1)
#else
#define MICROPY_HW_SYSTEM_TICK_USE_SYSTICK      (1)
#endif
#endif
#if MICROPY_HW_SYSTEM_TICK_USE_SYSTICK
#define MICROPY_SOFT_TIMER_TICKS_MS             system_tick_ms_counter
#endif

#ifndef MICROPY_HW_ENABLE_OSPI
#define MICROPY_HW_ENABLE_OSPI                  (CORE_M55_HP)
#endif
#ifndef MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_ENABLE_USBDEV                (CORE_M55_HP)
#endif
#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "Board in HS mode"
#endif
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC                      (CORE_M55_HP)
#endif
#define MICROPY_HW_USB_CDC_TX_TIMEOUT           (500)
#ifndef MICROPY_HW_USB_MSC
#define MICROPY_HW_USB_MSC                      (0)
#endif
#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID                      (0xf055)
#endif
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID                      (0x9802) // interface has CDC only
#endif
#ifndef MICROPY_HW_ENABLE_UART_REPL
#define MICROPY_HW_ENABLE_UART_REPL             (0)
#endif
#define MICROPY_HW_FLASH_BLOCK_SIZE_BYTES       (4096)

// Memory allocation policies
#ifndef MICROPY_ALLOC_GC_STACK_SIZE
#define MICROPY_ALLOC_GC_STACK_SIZE             (128)
#endif
#define MICROPY_ALLOC_PATH_MAX                  (128)
#define MICROPY_QSTR_BYTES_IN_HASH              (1)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#define MICROPY_EMIT_THUMB                      (1)
#define MICROPY_EMIT_THUMB_ARMV7M               (1)
#define MICROPY_EMIT_INLINE_THUMB               (1)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT         (1)

// Optimisations
#define MICROPY_OPT_COMPUTED_GOTO               (1)

// Python internal features
#define MICROPY_READER_VFS                      (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)
#ifndef MICROPY_FLOAT_IMPL
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#endif
#define MICROPY_SCHEDULER_DEPTH                 (8)
#define MICROPY_SCHEDULER_STATIC_NODES          (1)
#define MICROPY_USE_INTERNAL_ERRNO              (1)
#define MICROPY_TRACKED_ALLOC                   (MICROPY_PY_OPENAMP)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_SYS_PLATFORM                 "alif"

// Extended modules
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_PY_OS_INCLUDEFILE               "ports/alif/modos.c"
#define MICROPY_PY_OS_DUPTERM                   (1)
#define MICROPY_PY_OS_SEP                       (1)
#define MICROPY_PY_OS_SYNC                      (1)
#define MICROPY_PY_OS_UNAME                     (1)
#define MICROPY_PY_OS_URANDOM                   (1)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC        (se_services_rand64())
#define MICROPY_PY_TIME                         (1)
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/alif/modmachine.c"
#define MICROPY_PY_MACHINE_RESET                (1)
#define MICROPY_PY_MACHINE_BOOTLOADER           (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS     (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_ADC                  (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE      "ports/alif/machine_adc.c"
#define MICROPY_PY_MACHINE_DHT_READINTO         (1)
#define MICROPY_PY_MACHINE_PULSE                (1)
#define MICROPY_PY_MACHINE_I2C                  (MICROPY_HW_ENABLE_HW_I2C)
#define MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1  (1)
#define MICROPY_PY_MACHINE_SOFTI2C              (1)
#define MICROPY_PY_MACHINE_SPI                  (1)
#define MICROPY_PY_MACHINE_SOFTSPI              (1)
#define MICROPY_PY_MACHINE_TIMER                (1)
#define MICROPY_PY_MACHINE_UART                 (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE     "ports/alif/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_IRQ             (1)
#define MICROPY_PY_NETWORK                      (CORE_M55_HP)
#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "mpy-alif"
#endif
#define MICROPY_VFS                             (1)
#define MICROPY_VFS_ROM                         (1)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN                (1)
#define MICROPY_FATFS_LFN_CODE_PAGE             437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_RPATH                     (2)
#if MICROPY_HW_USB_MSC
#define MICROPY_FATFS_USE_LABEL                 (1)
#define MICROPY_FATFS_MULTI_PARTITION           (1)
// Set FatFS block size to flash sector size to avoid caching
// the flash sector in memory to support smaller block sizes.
#define MICROPY_FATFS_MAX_SS                    (MICROPY_HW_FLASH_BLOCK_SIZE_BYTES)
#endif

#if MICROPY_PY_NETWORK_CYW43
extern const struct _mp_obj_type_t mp_network_cyw43_type;
#define MICROPY_HW_NIC_CYW43 { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mp_network_cyw43_type) },
#else
#define MICROPY_HW_NIC_CYW43
#endif

#ifndef MICROPY_BOARD_NETWORK_INTERFACES
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

#define MICROPY_PORT_NETWORK_INTERFACES \
    MICROPY_HW_NIC_CYW43 \
    MICROPY_BOARD_NETWORK_INTERFACES \

// Bluetooth code only runs in the scheduler, no locking/mutex required.
#define MICROPY_PY_BLUETOOTH_ENTER uint32_t atomic_state = 0;
#define MICROPY_PY_BLUETOOTH_EXIT (void)atomic_state;
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
#define MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS (1)

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings

// We need an implementation of the log2 function which is not a macro
#define MP_NEED_LOG2 (1)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

// Assume that if we already defined the obj repr then we also defined these items
#ifndef MICROPY_OBJ_REPR
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef intptr_t mp_off_t;
#endif

// Board configuration settings.

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP()
#endif

#ifndef MICROPY_BOARD_EARLY_INIT
#define MICROPY_BOARD_EARLY_INIT()
#endif

#ifndef MICROPY_BOARD_FATAL_ERROR
extern void panic(const char *);
#define MICROPY_BOARD_FATAL_ERROR panic
#endif

#ifndef MICROPY_BOARD_ENTER_BOOTLOADER
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)
#endif

// Needed for MICROPY_PY_RANDOM_SEED_INIT_FUNC.
uint64_t se_services_rand64(void);
