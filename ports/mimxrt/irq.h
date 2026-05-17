/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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
#ifndef MICROPY_INCLUDED_MIMXRT_IRQ_H
#define MICROPY_INCLUDED_MIMXRT_IRQ_H

#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)

static inline uint32_t query_irq(void) {
    return __get_PRIMASK();
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

// IRQ priority definitions.
//
// Lower number implies higher interrupt priority.
//
// The default priority grouping used in this port is NVIC_PRIORITYGROUP_4.
// This corresponds to 4 bits for the priority field and 0 bits for the
// sub-priority field (which means that for all intents and purposes the
// sub-priorities below are ignored).
//
// While a given interrupt is being processed, only higher priority (lower number)
// interrupts will preempt a given interrupt. If sub-priorities are active
// then the sub-priority determines the order that pending interrupts of
// a given priority are executed. This is only meaningful if 2 or more
// interrupts of the same priority are pending at the same time.
//
// The following interrupts are arranged from highest priority to lowest
// priority to make it a bit easier to figure out.

#define IRQ_PRI_SYSTICK         NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 0, 0)

#define IRQ_PRI_CSI             NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 3, 0)

#define IRQ_PRI_OTG_HS          NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 6, 0)

#define IRQ_PRI_EXTINT          NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 14, 0)

// PENDSV should be at the lowst priority so that other interrupts complete
// before exception is raised.
#define IRQ_PRI_PENDSV          NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 15, 0)

#endif // MICROPY_INCLUDED_MIMXRT_IRQ_H
