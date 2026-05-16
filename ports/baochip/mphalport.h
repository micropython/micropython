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

#ifndef MICROPY_INCLUDED_BAOCHIP_MPHALPORT_H
#define MICROPY_INCLUDED_BAOCHIP_MPHALPORT_H

#include "py/mpconfig.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"

// Atomic interrupt-disable / re-enable for RV32 (mstatus.MIE = bit 3).
//
// disable_irq() returns the pre-disable mstatus (with MIE in bit 3),
// then clears MIE.  enable_irq() ORs only the saved MIE bit back into
// mstatus -- if the saved state had MIE clear, csrrs sets nothing and
// MIE stays disabled, which is the correct behaviour for nested
// disable/enable calls.  Pair only ever touches MIE, not other mstatus
// bits, so it's safe to call from any context.
static inline mp_uint_t disable_irq(void) {
    mp_uint_t state;
    __asm__ volatile ("csrrci %0, mstatus, 8" : "=r" (state));
    return state;
}

static inline void enable_irq(mp_uint_t state) {
    __asm__ volatile ("csrrs zero, mstatus, %0" : : "r" (state & 8));
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#define MICROPY_INTERNAL_WFE(TIMEOUT_MS)   __asm__ volatile ("wfi")

#include "hardware/gpio.h"
#include "machine_pin.h"

// REPL stdin ringbuf, fed by the UART RX interrupt service in
// mphalport.c.  Other input sources (dupterm, future native USB CDC)
// can push into the same buffer.
extern ringbuf_t stdin_ringbuf;

// Initialise the TickTimer block used by mp_hal_ticks_*.  Must be
// called from main() before any time-dependent code runs.
void mp_hal_ticktimer_init(void);

// Register and enable the UART RX interrupt that feeds stdin_ringbuf.
// Must be called from main() after uart_init() configures the REPL
// UART instance.
void mp_hal_stdin_uart_irq_init(void);

// Pin HAL type and access helpers.
#define mp_hal_pin_obj_t const machine_pin_obj_t *

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    PIN_DISABLE_OPEN_DRAIN(pin->port, pin->pin);
    gpio_set_dir(pin->port, pin->pin, false);
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    PIN_DISABLE_OPEN_DRAIN(pin->port, pin->pin);
    gpio_set_dir(pin->port, pin->pin, true);
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    gpio_put(pin->port, pin->pin, false);
    gpio_set_dir(pin->port, pin->pin, false);
    PIN_ENABLE_OPEN_DRAIN(pin->port, pin->pin);
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get(pin->port, pin->pin) ? 1 : 0;
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    if (PIN_IS_OPEN_DRAIN(pin->port, pin->pin)) {
        gpio_set_dir(pin->port, pin->pin, !v);
    } else {
        gpio_put(pin->port, pin->pin, v != 0);
    }
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin->port, pin->pin, true);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin->port, pin->pin, false);
}

static inline void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    if (PIN_IS_OPEN_DRAIN(pin->port, pin->pin)) {
        mp_hal_pin_od_low(pin);
    } else {
        gpio_put(pin->port, pin->pin, false);
    }
}

static inline void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    if (PIN_IS_OPEN_DRAIN(pin->port, pin->pin)) {
        mp_hal_pin_od_high(pin);
    } else {
        gpio_put(pin->port, pin->pin, true);
    }
}

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

#endif // MICROPY_INCLUDED_BAOCHIP_MPHALPORT_H
