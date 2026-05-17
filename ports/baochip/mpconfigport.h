/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

#include <alloca.h> // for alloca()

// Options to control how MicroPython is built.

#define MICROPY_CONFIG_ROM_LEVEL    (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// RISC-V code emitters.
#define MICROPY_EMIT_RV32           (1)
#define MICROPY_EMIT_INLINE_RV32    (1)

#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (32)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_PY_BUILTINS_FLOAT   (0)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/baochip/modmachine.c"
#define MICROPY_PY_MACHINE_BOOTLOADER           (1)
#define MICROPY_PY_MACHINE_RESET                (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS     (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_SYS_PLATFORM     "baochip"
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_OS               (1)
#define MICROPY_PY_OS_UNAME         (1)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_TIME             (1)
#define MICROPY_PY_TIME_TIME_TIME_NS (0)

// All port-side root pointers live alongside the vm-side ones.
#define MP_STATE_PORT MP_STATE_VM

// Type definitions for the specific machine.  mp_int_t / mp_uint_t are
// already defined by py/mpconfig.h for the default MP_INT_TYPE_INTPTR.
typedef long mp_off_t;

// SSIZE_MAX isn't exposed by the freestanding C library used for this
// port; define MP_SSIZE_MAX explicitly to the 32-bit signed maximum
// (matches rp2/stm32/zephyr).
#define MP_SSIZE_MAX (0x7fffffff)

// REPL UART defaults; boards may override in mpconfigboard.h.
#ifndef MICROPY_HW_UART_REPL
#define MICROPY_HW_UART_REPL        (2)
#endif
#ifndef MICROPY_HW_UART_REPL_BAUD
#define MICROPY_HW_UART_REPL_BAUD   (115200)
#endif

// Board-specific overrides.
#include "mpconfigboard.h"
