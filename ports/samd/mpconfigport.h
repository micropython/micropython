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
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (32)
#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_QSTR_BYTES_IN_HASH          (1)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)

// Compiler configuration
#define MICROPY_COMP_CONST                  (1)

// Python internal features
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_FINALISER            (1)
#define MICROPY_KBD_EXCEPTION               (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_STREAMS_NON_BLOCK           (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       samd_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_ENABLE_SCHEDULER            (1)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)
#define MICROPY_MODULE_WEAK_LINKS           (1)
#define MICROPY_HW_ENABLE_USBDEV            (1)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)

// Control over Python builtins
#define MICROPY_PY_BUILTINS_BYTES_HEX       (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_INPUT           (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN       (1)
#define MICROPY_PY_SYS                      (1)
#define MICROPY_PY_SYS_PLATFORM             "samd"
#define MICROPY_PY_SYS_EXIT                 (1)
#define MICROPY_PY_SYS_STDFILES             (1)
#define MICROPY_PY_SYS_MAXSIZE              (1)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)

// Extended modules
#define MICROPY_PY_UTIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_UTIME_TIME_TIME_NS       (1)
#define MICROPY_PY_UTIME_INCLUDEFILE        "ports/samd/modutime.c"
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_UOS                      (1)
#define MICROPY_PY_UOS_INCLUDEFILE          "ports/samd/moduos.c"
#define MICROPY_READER_VFS                  (1)
#define MICROPY_VFS                         (1)
#define MICROPY_PY_UJSON                    (1)
#define MICROPY_PY_URE                      (1)
#define MICROPY_PY_UBINASCII                (1)
#define MICROPY_PY_UCTYPES                  (1)
#define MICROPY_PY_UHEAPQ                   (1)
#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_UZLIB                    (1)
#define MICROPY_PY_UASYNCIO                 (1)
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_MACHINE_RTC              (1)
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#define MICROPY_HW_SOFTSPI_MIN_DELAY        (1)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE     (1000000)
#define MICROPY_PY_MACHINE_TIMER            (1)
#define MICROPY_SOFT_TIMER_TICKS_MS         systick_ms
#define MICROPY_PY_OS_DUPTERM               (3)
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_PWM              (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/samd/machine_pwm.c"
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new

#define MP_STATE_PORT MP_STATE_VM

// Additional entries for use with pendsv_schedule_dispatch.
#ifndef MICROPY_BOARD_PENDSV_ENTRIES
#define MICROPY_BOARD_PENDSV_ENTRIES
#endif

// Miscellaneous settings
__attribute__((always_inline)) static inline void enable_irq(uint32_t state) {
    __set_PRIMASK(state);
}

__attribute__((always_inline)) static inline uint32_t disable_irq(void) {
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

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

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
