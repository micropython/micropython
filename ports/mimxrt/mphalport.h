/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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
#ifndef MICROPY_INCLUDED_MIMXRT_MPHALPORT_H
#define MICROPY_INCLUDED_MIMXRT_MPHALPORT_H

#include <stdint.h>
#include "ticks.h"
#include "fsl_gpio.h"
#include "pin.h"

#define mp_hal_pin_obj_t pin_obj_t *

#define mp_hal_pin_high(p) (GPIO_PinWrite(p->gpio, p->pin, 1U))
#define mp_hal_pin_low(p) (GPIO_PinWrite(p->gpio, p->pin, 0U))
#define mp_hal_pin_write(p, value) (GPIO_PinWrite(p->gpio, p->pin, value))
#define mp_hal_pin_toggle(p) (GPIO_PortToggle(p->gpio, (1 << p->pin)))
#define mp_hal_pin_od_high(p) (mp_hal_pin_high((p)))
#define mp_hal_pin_od_low(p) (mp_hal_pin_low((p)))
#define mp_hal_pin_read(p)(GPIO_PinRead(p->gpio, p->pin))

static inline mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t o) {
    // Todo: Implement pin_find function
    return NULL;
}

// Todo: Analyse what this function is used for and adapt implementation if necessary!
#define mp_hal_quiet_timing_enter() (1)
#define mp_hal_quiet_timing_exit(irq_state) (void)(irq_state)

extern volatile uint32_t systick_ms;

void mp_hal_set_interrupt_char(int c);

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return ticks_ms32();
}

static inline mp_uint_t mp_hal_ticks_us(void) {
    return ticks_us32();
}

static inline void mp_hal_delay_ms(mp_uint_t ms) {
    uint64_t us = (uint64_t)ms * 1000;
    ticks_delay_us64(us);
}

static inline void mp_hal_delay_us(mp_uint_t us) {
    ticks_delay_us64(us);
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}

void mp_hal_delay_us_fast(mp_uint_t us);


#endif // MICROPY_INCLUDED_MIMXRT_MPHALPORT_H
