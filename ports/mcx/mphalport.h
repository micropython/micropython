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

#ifndef MP_PORT_MCX_MPHALPORT_H
#define MP_PORT_MCX_MPHALPORT_H

void mp_hal_init(void);

/* UUID*/
void mp_hal_get_unique_id(uint8_t *uid);

/* STDIO */
int mp_hal_stdio_init(void);

/* Pin */
#include "hal_pin.h"

static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

static inline uint32_t raise_irq_pri(uint32_t pri) {
    uint32_t basepri = __get_BASEPRI();
    // If non-zero, the processor does not process any exception with a
    // priority value greater than or equal to BASEPRI.
    // When writing to BASEPRI_MAX the write goes to BASEPRI only if either:
    //   - Rn is non-zero and the current BASEPRI value is 0
    //   - Rn is non-zero and less than the current BASEPRI value
    pri <<= (8 - __NVIC_PRIO_BITS);
    __ASM volatile ("msr basepri_max, %0" : : "r" (pri) : "memory");
    return basepri;
}

// "basepri" should be the value returned from raise_irq_pri
static inline void restore_irq_pri(uint32_t basepri) {
    __set_BASEPRI(basepri);
}

#define mp_hal_quiet_timing_enter() raise_irq_pri(1)
#define mp_hal_quiet_timing_exit(irq_state) restore_irq_pri(irq_state)

/* Additional delay */
#define mp_hal_delay_us_fast mp_hal_delay_us

extern int mp_interrupt_char;
static inline void mp_hal_set_interrupt_char(char c) {
    mp_interrupt_char = c;
}

#endif
