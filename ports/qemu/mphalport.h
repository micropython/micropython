/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Damien P. George
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

#include <stddef.h>
#include "shared/runtime/interrupt_char.h"

void mp_hal_get_random(size_t n, uint8_t *buf);

#if defined(__ARM_32BIT_STATE)
#if __has_builtin(__builtin___clear_cache)
#define MP_HAL_CLEAN_DCACHE(fun_data, fun_len) \
    do { \
        __builtin___clear_cache((void *)fun_data, (char *)fun_data + fun_len); \
    } while (0)
#endif
#endif

#if defined(__ARM_ARCH_PROFILE) && (__ARM_ARCH_PROFILE == 'M')
// Cortex-M atomic section (save/disable then restore interrupts via PRIMASK).
// PRIMASK only exists on the M profile; other ARM profiles (e.g. Cortex-A on
// SABRELITE, which also defines __ARM_ARCH_ISA_THUMB) keep the default no-op.
__attribute__((always_inline)) static inline uint32_t mp_atomic_begin(void) {
    uint32_t state;
    __asm volatile ("mrs %0, primask; cpsid i" : "=r" (state) :: "memory");
    return state;
}
__attribute__((always_inline)) static inline void mp_atomic_end(uint32_t state) {
    __asm volatile ("msr primask, %0" :: "r" (state) : "memory");
}

#define MICROPY_BEGIN_ATOMIC_SECTION() mp_atomic_begin()
#define MICROPY_END_ATOMIC_SECTION(state) mp_atomic_end(state)

// PendSV is set to the lowest exception priority (in ticks_init()) and is used
// to dispatch the soft-timer / lwIP background poll.  Raising BASEPRI to
// IRQ_PRI_PENDSV masks PendSV (and below) but leaves SysTick and other
// higher-priority IRQs running.  Writing 0xff selects the lowest priority for
// any number of implemented priority bits.  BASEPRI needs ARMv7-M or later;
// earlier cores fall back to a full atomic section.
#define IRQ_PRI_PENDSV (0xff)

#if __ARM_ARCH >= 7
// irqs with a priority value greater or equal to "pri" will be disabled
__attribute__((always_inline)) static inline uint32_t raise_irq_pri(uint32_t pri) {
    uint32_t basepri;
    __asm volatile ("mrs %0, basepri" : "=r" (basepri));
    // If non-zero, the processor does not process any exception with a
    // priority value greater than or equal to BASEPRI.
    // When writing to BASEPRI_MAX the write goes to BASEPRI only if either:
    //   - Rn is non-zero and the current BASEPRI value is 0
    //   - Rn is non-zero and less than the current BASEPRI value
    __asm volatile ("msr basepri_max, %0" :: "r" (pri) : "memory");
    return basepri;
}
// "basepri" should be the value returned from raise_irq_pri
__attribute__((always_inline)) static inline void restore_irq_pri(uint32_t basepri) {
    __asm volatile ("msr basepri, %0" :: "r" (basepri) : "memory");
}
#else
#define raise_irq_pri(pri) mp_atomic_begin()
#define restore_irq_pri(state) mp_atomic_end(state)
#endif

// For regular code that wants to prevent "background tasks" from running.
// These background tasks (soft timer, lwIP) run in PENDSV context.
#define MICROPY_PY_PENDSV_ENTER   uint32_t atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_REENTER atomic_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_PENDSV_EXIT    restore_irq_pri(atomic_state);

// Prevent the "lwIP task" from running.
#define MICROPY_PY_LWIP_ENTER   MICROPY_PY_PENDSV_ENTER
#define MICROPY_PY_LWIP_REENTER MICROPY_PY_PENDSV_REENTER
#define MICROPY_PY_LWIP_EXIT    MICROPY_PY_PENDSV_EXIT
#endif
