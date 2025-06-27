/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
 * Copyright (c) 2024 OpenMV LLC.
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
#ifndef MICROPY_INCLUDED_ALIF_IRQ_H
#define MICROPY_INCLUDED_ALIF_IRQ_H

#include <stdint.h>
#include ALIF_CMSIS_H

// IRQ priority definitions.
//
// The M55-HP CPU has __NVIC_PRIO_BITS==8 bits for setting the IRQ priority.
// It uses NVIC_SetPriorityGrouping(0) which is 7 bits for preempt priority
// and 1 bit for the sub-priority.
//
// Lower number implies higher interrupt priority.

#define NVIC_PRIORITYGROUP_7    ((uint32_t)0x00000000U)
#define IRQ_PRI_SYSTEM_TICK     NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 0, 0)
#define IRQ_PRI_MHU             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 0, 0)
#define IRQ_PRI_QUIET_TIMING    NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 1, 0)
#define IRQ_PRI_UART_REPL       NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 1, 0)
#define IRQ_PRI_ADC             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 3, 0)
#define IRQ_PRI_CSI             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 5, 0)
#define IRQ_PRI_USB             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 7, 0)
#define IRQ_PRI_HWSEM           NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 8, 0)
#define IRQ_PRI_GPU             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 10, 0)
#define IRQ_PRI_GPIO            NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 50, 0)
#define IRQ_PRI_RTC             NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 100, 0)
#define IRQ_PRI_CYW43           NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 126, 0)
#define IRQ_PRI_PENDSV          NVIC_EncodePriority(NVIC_PRIORITYGROUP_7, 127, 0)

// these states correspond to values from query_irq, enable_irq and disable_irq
#define IRQ_STATE_DISABLED (0x00000001)
#define IRQ_STATE_ENABLED  (0x00000000)

static inline uint32_t query_irq(void) {
    return __get_PRIMASK();
}

static inline void enable_irq(uint32_t state) {
    __set_PRIMASK(state);
}

static inline uint32_t disable_irq(void) {
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

// irqs with a priority value greater or equal to "pri" will be disabled
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

#endif // MICROPY_INCLUDED_ALIF_IRQ_H
