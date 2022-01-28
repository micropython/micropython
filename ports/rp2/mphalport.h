/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
 *
 */
#ifndef MICROPY_INCLUDED_RP2_MPHALPORT_H
#define MICROPY_INCLUDED_RP2_MPHALPORT_H

#include "py/mpconfig.h"
#include "py/ringbuf.h"
#include "pico/time.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"

#define SYSTICK_MAX (0xffffff)

extern int mp_interrupt_char;
extern ringbuf_t stdin_ringbuf;

void mp_hal_set_interrupt_char(int c);

static inline void mp_hal_delay_us(mp_uint_t us) {
    sleep_us(us);
}

static inline void mp_hal_delay_us_fast(mp_uint_t us) {
    busy_wait_us(us);
}

#define mp_hal_quiet_timing_enter() MICROPY_BEGIN_ATOMIC_SECTION()
#define mp_hal_quiet_timing_exit(irq_state) MICROPY_END_ATOMIC_SECTION(irq_state)

static inline mp_uint_t mp_hal_ticks_us(void) {
    return time_us_32();
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return to_ms_since_boot(get_absolute_time());
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be.
    return time_us_32();
}

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return clock_get_hz(clk_sys);
}

// C-level pin HAL

#include "py/obj.h"
#include "hardware/gpio.h"

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t uint

extern uint32_t machine_pin_open_drain_mask;

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in);

static inline unsigned int mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_IN);
    machine_pin_open_drain_mask &= ~(1 << pin);
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_OUT);
    machine_pin_open_drain_mask &= ~(1 << pin);
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    gpio_set_function(pin, GPIO_FUNC_SIO);
    gpio_set_dir(pin, GPIO_IN);
    gpio_put(pin, 0);
    machine_pin_open_drain_mask |= 1 << pin;
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get(pin);
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    gpio_put(pin, v);
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_OUT);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    gpio_set_dir(pin, GPIO_IN);
}

static inline void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    gpio_clr_mask(1 << pin);
}

static inline void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    gpio_set_mask(1 << pin);
}

#endif // MICROPY_INCLUDED_RP2_MPHALPORT_H
