/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "py/runtime.h"
#include "py/mphal.h"

#include "fsl_common.h"
#include "fsl_common_arm.h"

volatile mp_uint_t s_current_tick;

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t t_start = s_current_tick;

    while (s_current_tick < (t_start + ms)) {
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    mp_uint_t t_end = mp_hal_ticks_us() + us;

    while (mp_hal_ticks_us() < t_end) {
        /* Do not poll events here */
    }

    /* TODO: handle condition when interrupts are disabled. */
}

mp_uint_t mp_hal_ticks_ms(void) {
    return s_current_tick * 1000 / MICROPY_HAL_SYSTICK_RATE;
}

mp_uint_t mp_hal_ticks_us(void) {
    mp_uint_t reload = SysTick->LOAD;
    mp_uint_t usec = (reload - SysTick->VAL) * (1000000 / MICROPY_HAL_SYSTICK_RATE) / reload;

    return s_current_tick * (1000000 / MICROPY_HAL_SYSTICK_RATE) + usec;
}

uint64_t mp_hal_time_ns(void) {
    return mp_hal_ticks_us() * 1000;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    for (;;) {
        /* -- */
    }
}

void SysTick_Handler(void) {
    s_current_tick++;
}
