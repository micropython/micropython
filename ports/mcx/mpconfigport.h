/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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

// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// board specific definitions
#include "mpconfigboard_common.h"
#include "mpconfigboard.h"

// Default configuration
#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

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
#define MICROPY_USE_INTERNAL_ERRNO     (1)
#define MICROPY_SCHEDULER_STATIC_NODES (1)
#define MICROPY_SCHEDULER_DEPTH        (8)

#define MICROPY_VFS                    (1)

#ifndef MICROPY_VFS_LFS2
#define MICROPY_VFS_LFS2               (1)
#endif

// Hardware features
#ifndef MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_ENABLE_USBDEV    (0)
#endif

#ifndef MICROPY_HW_ENABLE_UART_REPL
#define MICROPY_HW_ENABLE_UART_REPL (1) // useful if there is no USB
#endif

#if MICROPY_HW_ENABLE_USBDEV

#ifndef MICROPY_HW_USBDEV
#define MICROPY_HW_USBDEV  (0)
#endif

// Enable USB-CDC serial port
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC (1)
#endif

// Enable USB Mass Storage with FatFS filesystem.
#ifndef MICROPY_HW_USB_MSC
#define MICROPY_HW_USB_MSC (0)
#endif

#endif

#if MICROPY_HW_ENABLE_UART_REPL
#ifndef MICROPY_HW_UART_REPL
#define MICROPY_HW_UART_REPL (0)
#endif
#endif

#ifndef MICROPY_HW_ENABLE_IFS
#define MICROPY_HW_ENABLE_IFS (0)
#endif

#if MICROPY_HW_ENABLE_IFS
// IFS start
#ifndef MICROPY_HW_IFS_START
#define MICROPY_HW_IFS_START (0x100000)
#endif
// IFS length
#ifndef MICROPY_HW_IFS_LENGTH
#define MICROPY_HW_IFS_LENGTH (0x100000)
#endif
#endif

#ifndef MICROPY_HW_ENABLE_EFS
#define MICROPY_HW_ENABLE_EFS  (0)
#endif


// Extended modules
#define MICROPY_PY_OS_INCLUDEFILE   "ports/mcx/modos.c"
#define MICROPY_PY_OS_DUPTERM       (3)
#define MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM (1)
#define MICROPY_PY_OS_DUPTERM_STREAM_DETACHED_ATTACHED (1)

#define MICROPY_PY_ONEWIRE (1)

// Machine configs
#define MICROPY_PY_MACHINE              (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE  "ports/mcx/modmachine.c"

#define MICROPY_PY_MACHINE_DHT_READINTO (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_PULSE        (1)

#define MICROPY_PY_MACHINE_ADC             (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE "ports/mcx/machine_adc.c"

#define MICROPY_PY_MACHINE_UART                    (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE        "ports/mcx/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_IRQ                (0)
#define MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR (1)
#define MICROPY_PY_MACHINE_UART_SENDBREAK          (1)

#define MICROPY_PY_MACHINE_I2C         (1)

#define MICROPY_PY_MACHINE_SPI         (1)
#define MICROPY_PY_MACHINE_SPI_MSB     (0)
#define MICROPY_PY_MACHINE_SPI_LSB     (1)

#define MICROPY_PY_MACHINE_PWM         (1)
#define MICROPY_PY_MACHINE_PWM_DUTY    (0)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/mcx/machine_pwm.c"

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME "Unknown MCX series board"
#endif

#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME "Unknown MCX series MCU"
#endif

#ifndef MICROPY_PY_SYS_PLATFORM
#define MICROPY_PY_SYS_PLATFORM "mcx"
#endif

// We need an implementation of the log2 function which is not a macro
#define MP_NEED_LOG2 (1)

#define MP_SSIZE_MAX (0x7fffffff)

#define MP_STATE_PORT MP_STATE_VM

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
