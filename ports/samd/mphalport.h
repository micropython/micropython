/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2021 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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
#ifndef MICROPY_INCLUDED_SAMD_MPHALPORT_H
#define MICROPY_INCLUDED_SAMD_MPHALPORT_H

#include "py/mpconfig.h"

// ASF4
#include "hal_gpio.h"
#include "hpl_time_measure.h"
#include "sam.h"

extern int mp_interrupt_char;
extern volatile uint32_t systick_ms;
extern volatile uint32_t systick_ms_upper;

void mp_hal_set_interrupt_char(int c);

// Define an alias fo systick_ms, because the shared softtimer.c uses
// the symbol uwTick for the systick ms counter.
#define uwTick systick_ms

#define mp_hal_delay_us_fast  mp_hal_delay_us

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return systick_ms;
}

static inline uint64_t mp_hal_ticks_ms_64(void) {
    return ((uint64_t)systick_ms_upper << 32) + systick_ms;
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    #if defined(MCU_SAMD21)

    return REG_TC4_COUNT32_COUNT;

    #elif defined(MCU_SAMD51)

    TC0->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
    while (TC0->COUNT32.CTRLBSET.reg != 0) {
    }
    return REG_TC0_COUNT32_COUNT >> 3;

    #else
    return systick_ms * 1000;
    #endif
}

#if defined(MCU_SAMD21)

#define mp_hal_ticks_cpu mp_hal_ticks_us

#elif defined(MCU_SAMD51)
static inline void mp_hal_ticks_cpu_enable(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return (system_time_t)DWT->CYCCNT;
}
#endif

static inline uint64_t mp_hal_time_ns(void) {
    return mp_hal_ticks_ms_64() * 1000000;
}

// C-level pin HAL

#include "py/obj.h"

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_obj_t uint

extern uint32_t machine_pin_open_drain_mask[];

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in);
void mp_hal_set_pin_mux(mp_hal_pin_obj_t pin, uint8_t mux);

static inline unsigned int mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

static inline void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    machine_pin_open_drain_mask[pin / 32] &= ~(1 << (pin % 32));
}

static inline void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    gpio_set_pin_direction(pin, GPIO_DIRECTION_OUT);
    machine_pin_open_drain_mask[pin / 32] &= ~(1 << (pin % 32));
}

static inline void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_UP);
    machine_pin_open_drain_mask[pin / 32] |= 1 << (pin % 32);
}

static inline unsigned int mp_hal_get_pin_direction(mp_hal_pin_obj_t pin) {
    return (PORT->Group[pin / 32].DIR.reg & (1 << (pin % 32))) >> (pin % 32);
}

static inline int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return gpio_get_pin_level(pin);
}

static inline void mp_hal_pin_write(mp_hal_pin_obj_t pin, int v) {
    gpio_set_pin_level(pin, v);
}

static inline void mp_hal_pin_low(mp_hal_pin_obj_t pin) {
    gpio_set_pin_level(pin, 0);
}

static inline void mp_hal_pin_high(mp_hal_pin_obj_t pin) {
    gpio_set_pin_level(pin, 1);
}

static inline void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    gpio_set_pin_direction(pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(pin, 0);
}

static inline void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    gpio_set_pin_direction(pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin, GPIO_PULL_UP);
}

#endif // MICROPY_INCLUDED_SAMD_MPHALPORT_H
