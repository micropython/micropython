/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
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
#include <alloca.h>

// Include Zephyr's autoconf.h, which should be made first by Zephyr makefiles
#include "autoconf.h"
// Included here to get basic Zephyr environment (macros, etc.)
#include <zephyr/kernel.h>
#include <zephyr/drivers/spi.h>

// Usually passed from Makefile
#ifndef MICROPY_HEAP_SIZE
#define MICROPY_HEAP_SIZE (16 * 1024)
#endif

#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (MICROPY_VFS)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_BUILTINS_BYTES_HEX (1)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_STR_COUNT (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT zephyr_help_text
#define MICROPY_PY_ARRAY            (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/zephyr/modmachine.c"
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_SPI      (1)
#define MICROPY_PY_MACHINE_SPI_MSB (SPI_TRANSFER_MSB)
#define MICROPY_PY_MACHINE_SPI_LSB (SPI_TRANSFER_LSB)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "ports/zephyr/machine_uart.c"
#define MICROPY_PY_STRUCT           (0)
#ifdef CONFIG_NETWORKING
// If we have networking, we likely want errno comfort
#define MICROPY_PY_ERRNO            (1)
#define MICROPY_PY_SOCKET           (1)
#endif
#ifdef CONFIG_BT
#define MICROPY_PY_BLUETOOTH        (1)
#ifdef CONFIG_BT_CENTRAL
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
#endif
#define MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT (0)
#endif
#define MICROPY_PY_BINASCII         (1)
#define MICROPY_PY_HASHLIB          (1)
#define MICROPY_PY_OS               (1)
#define MICROPY_PY_TIME             (1)
#define MICROPY_PY_TIME_TIME_TIME_NS (1)
#define MICROPY_PY_TIME_INCLUDEFILE "ports/zephyr/modtime.c"
#define MICROPY_PY_ZEPHYR           (1)
#define MICROPY_PY_ZSENSOR          (1)
#define MICROPY_PY_SYS_MODULES      (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_ENABLE_SCHEDULER    (1)
#define MICROPY_VFS                 (1)
#define MICROPY_READER_VFS          (MICROPY_VFS)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_NORTC            (1)

// Saving extra crumbs to make sure binary fits in 128K
#define MICROPY_COMP_CONST_FOLDING  (0)
#define MICROPY_COMP_CONST (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)

// When CONFIG_THREAD_CUSTOM_DATA is enabled, MICROPY_PY_THREAD is enabled automatically
#ifdef CONFIG_THREAD_CUSTOM_DATA
#define MICROPY_PY_THREAD                   (1)
#define MICROPY_PY_THREAD_GIL               (1)
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR    (32)
#endif

void mp_hal_signal_event(void);
#define MICROPY_SCHED_HOOK_SCHEDULED mp_hal_signal_event()

#define MICROPY_PY_SYS_PLATFORM "zephyr"

#ifdef CONFIG_BOARD
#define MICROPY_HW_BOARD_NAME "zephyr-" CONFIG_BOARD
#else
#define MICROPY_HW_BOARD_NAME "zephyr-generic"
#endif

#ifdef CONFIG_SOC
#define MICROPY_HW_MCU_NAME CONFIG_SOC
#else
#define MICROPY_HW_MCU_NAME "unknown-cpu"
#endif

typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MP_THREAD_GIL_EXIT(); \
        k_msleep(1); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        k_msleep(1); \
    } while (0);
#endif
