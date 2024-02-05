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
// MCU-Specific definitions
#include "mpconfigmcu.h"

// Memory allocation policies
#define MICROPY_GC_STACK_ENTRY_TYPE         uint16_t
#define MICROPY_GC_ALLOC_THRESHOLD          (0)
#define MICROPY_ALLOC_PATH_MAX              (256)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)

// Python internal features
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX         (0)
#endif
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_PY_BUILTINS_HELP_TEXT       samd_help_text
#define MICROPY_USE_INTERNAL_ERRNO          (1)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)

#define MICROPY_HW_ENABLE_USBDEV            (1)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)

#if MICROPY_HW_ENABLE_USBDEV
// Enable USB-CDC serial port
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC (1)
#endif
// SAMD unique ID is 16 bytes (hex string == 32)
#ifndef MICROPY_HW_USB_DESC_STR_MAX
#define MICROPY_HW_USB_DESC_STR_MAX (32)
#endif
#endif

#define MICROPY_PY_SYS_PLATFORM             "samd"

// Extended modules
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS        (1)
#define MICROPY_PY_TIME_INCLUDEFILE         "ports/samd/modtime.c"
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE      "ports/samd/modmachine.c"
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_BOOTLOADER       (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_OS_INCLUDEFILE           "ports/samd/modos.c"
#define MICROPY_READER_VFS                  (1)
#define MICROPY_VFS                         (1)
#ifndef MICROPY_PY_MACHINE_ADC
#define MICROPY_PY_MACHINE_ADC              (1)
#endif
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE  "ports/samd/machine_adc.c"
#define MICROPY_PY_MACHINE_ADC_DEINIT       (1)
#ifndef MICROPY_PY_MACHINE_DAC
#define MICROPY_PY_MACHINE_DAC              (1)
#endif
#ifndef MICROPY_PY_MACHINE_I2C
#define MICROPY_PY_MACHINE_I2C              (1)
#endif
#ifndef MICROPY_PY_MACHINE_SPI
#define MICROPY_PY_MACHINE_SPI              (1)
#endif
#ifndef MICROPY_PY_MACHINE_SOFTI2C
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#endif
#ifndef MICROPY_PY_MACHINE_SOFTSPI
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#endif
#ifndef MICROPY_PY_MACHINE_UART
#define MICROPY_PY_MACHINE_UART             (1)
#endif
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "ports/samd/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_SENDBREAK   (1)
#define MICROPY_PY_MACHINE_TIMER            (1)
#define MICROPY_SOFT_TIMER_TICKS_MS         systick_ms
#define MICROPY_PY_OS_DUPTERM               (3)
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#ifndef MICROPY_PY_MACHINE_PULSE
#define MICROPY_PY_MACHINE_PULSE            (1)
#endif
#ifndef MICROPY_PY_MACHINE_PWM
#define MICROPY_PY_MACHINE_PWM              (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/samd/machine_pwm.c"
#endif
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_DHT_READINTO     (1)
#define MICROPY_PY_MACHINE_WDT              (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE  "ports/samd/machine_wdt.c"
#define MICROPY_PY_MACHINE_WDT_TIMEOUT_MS   (1)
#define MICROPY_PLATFORM_VERSION            "ASF4"

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings

#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID (0xf055)
#endif
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID (0x9802)
#endif

// Additional entries for use with pendsv_schedule_dispatch.
#ifndef MICROPY_BOARD_PENDSV_ENTRIES
#define MICROPY_BOARD_PENDSV_ENTRIES
#endif

// Use internal flash for the file system if no flash file system is selected.
#if !defined(MICROPY_HW_MCUFLASH) && !defined(MICROPY_HW_QSPIFLASH) && !(defined(MICROPY_HW_SPIFLASH) && defined(MICROPY_HW_SPIFLASH_ID))
#define MICROPY_HW_MCUFLASH                 (1)
#endif  // !defined(MICROPY_HW_MCUFLASH) ....

// Miscellaneous settings

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFE(); \
    } while (0);

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

// Need an implementation of the log2 function which is not a macro.
#define MP_NEED_LOG2 (1)

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
