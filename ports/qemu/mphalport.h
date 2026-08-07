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
#include <stdint.h>
#include "shared/runtime/interrupt_char.h"

enum {
    MP_HAL_MAC_ETH0 = 0,
};

void mp_hal_get_mac(int idx, uint8_t buf[6]);
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

#define MICROPY_PY_PENDSV_ENTER uint32_t atomic_state = mp_atomic_begin()
#define MICROPY_PY_PENDSV_EXIT mp_atomic_end(atomic_state)
#endif
