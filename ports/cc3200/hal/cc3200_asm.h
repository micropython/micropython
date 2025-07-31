/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#ifndef CC3200_ASM_H_
#define CC3200_ASM_H_

// We have inlined IRQ functions for efficiency (they are generally
// 1 machine instruction).
//
// Note on IRQ state: you should not need to know the specific
// value of the state variable, but rather just pass the return
// value from disable_irq back to enable_irq.

// these states correspond to values from query_irq, enable_irq and disable_irq
#define IRQ_STATE_DISABLED (0x00000001)
#define IRQ_STATE_ENABLED  (0x00000000)

#ifndef __disable_irq
#define __disable_irq() __asm__ volatile ("cpsid i");
#endif

#ifndef DEBUG
__attribute__(( always_inline ))
static inline void __WFI(void) {
    __asm volatile ("    dsb      \n"
                    "    isb      \n"
                    "    wfi      \n");
}
#else
// For some reason the debugger gets disconnected when entering any of the sleep modes
__attribute__(( always_inline ))
static inline void __WFI(void) {
    __asm volatile ("    dsb      \n"
                    "    isb      \n");
}
#endif

__attribute__(( always_inline ))
static inline uint32_t __get_PRIMASK(void) {
    uint32_t result;
    __asm volatile ("mrs %0, primask" : "=r" (result));
    return(result);
}

__attribute__(( always_inline ))
static inline void __set_PRIMASK(uint32_t priMask) {
    __asm volatile ("msr primask, %0" : : "r" (priMask) : "memory");
}

__attribute__(( always_inline ))
static inline uint32_t __get_BASEPRI(void) {
    uint32_t result;
    __asm volatile ("mrs %0, basepri" : "=r" (result));
    return(result);
}

__attribute__(( always_inline ))
static inline void __set_BASEPRI(uint32_t value) {
    __asm volatile ("msr basepri, %0" : : "r" (value) : "memory");
}

__attribute__(( always_inline ))
static inline uint32_t query_irq(void) {
    return __get_PRIMASK();
}

__attribute__(( always_inline ))
static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

__attribute__(( always_inline ))
static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#endif /* CC3200_ASM_H_ */
