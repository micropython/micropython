/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_MPHAL_H
#define MICROPY_INCLUDED_PY_MPHAL_H

#include <stdint.h>
#include "py/mpconfig.h"

#ifdef MICROPY_MPHALPORT_H
#include MICROPY_MPHALPORT_H
#else
#include <mphalport.h>
#endif

// On embedded platforms, these will typically enable/disable irqs.
#ifndef MICROPY_BEGIN_ATOMIC_SECTION
#define MICROPY_BEGIN_ATOMIC_SECTION() (0)
#endif
#ifndef MICROPY_END_ATOMIC_SECTION
#define MICROPY_END_ATOMIC_SECTION(state) (void)(state)
#endif

#ifndef mp_hal_stdio_poll
uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);
#endif

#ifndef mp_hal_stdin_rx_chr
int mp_hal_stdin_rx_chr(void);
#endif

#ifndef mp_hal_stdout_tx_str
void mp_hal_stdout_tx_str(const char *str);
#endif

#ifndef mp_hal_stdout_tx_strn
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len);
#endif

#ifndef mp_hal_stdout_tx_strn_cooked
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len);
#endif

#ifndef mp_hal_delay_ms
void mp_hal_delay_ms(mp_uint_t ms);
#endif

#ifndef mp_hal_delay_us
void mp_hal_delay_us(mp_uint_t us);
#endif

#ifndef mp_hal_ticks_ms
mp_uint_t mp_hal_ticks_ms(void);
#endif

#ifndef mp_hal_ticks_us
mp_uint_t mp_hal_ticks_us(void);
#endif

#ifndef mp_hal_ticks_cpu
mp_uint_t mp_hal_ticks_cpu(void);
#endif

#ifndef mp_hal_time_ns
// Nanoseconds since the Epoch.
uint64_t mp_hal_time_ns(void);
#endif

// If port HAL didn't define its own pin API, use generic
// "virtual pin" API from the core.
#ifndef mp_hal_pin_obj_t
#define mp_hal_pin_obj_t mp_obj_t
#define mp_hal_get_pin_obj(pin) (pin)
#define mp_hal_pin_read(pin) mp_virtual_pin_read(pin)
#define mp_hal_pin_write(pin, v) mp_virtual_pin_write(pin, v)
#include "extmod/virtpin.h"
#endif

// Event handling and wait-for-event functions.

#ifndef MICROPY_INTERNAL_WFE
// Fallback definition for ports that don't need to suspend the CPU.
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS) (void)0
#endif

#ifndef MICROPY_INTERNAL_EVENT_HOOK
// Fallback definition for ports that don't need any port-specific
// non-blocking event processing.
#define MICROPY_INTERNAL_EVENT_HOOK (void)0
#endif

#endif // MICROPY_INCLUDED_PY_MPHAL_H
