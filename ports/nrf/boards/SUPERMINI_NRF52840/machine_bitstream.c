/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jim Mussared
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

// This is a translation of the cycle counter implementation in ports/stm32/machine_bitstream.c.
// part of the code from robert-h w600 micropython port

#include "py/mpconfig.h"
#include "py/mphal.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#define mp_hal_quiet_timing_enter() MICROPY_BEGIN_ATOMIC_SECTION()
#define mp_hal_quiet_timing_exit(irq_state) MICROPY_END_ATOMIC_SECTION(irq_state)

// NRF52840 @ 64MHz (cycle=15.625ns)
#define NS_CYCLES_PER_ITER_HIGH (4)  // 6 => 4
#define NS_CYCLES_PER_ITER_LOW (4)   // 6 => 4
#define NS_OVERHEAD_CYCLES_HIGH (16) // 12 => 16
#define NS_OVERHEAD_CYCLES_LOW (12)  // 18 => 12

uint32_t mp_hal_delay_ns_calc(uint32_t ns, bool high) {
    uint32_t ncycles = 64 * ns / 1000; // hard coded system clock 64MHz for NRF52. the system clock cannot be change anyway
    uint32_t overhead = MIN(ncycles, high ? NS_OVERHEAD_CYCLES_HIGH : NS_OVERHEAD_CYCLES_LOW);
    return MAX(1, MP_ROUND_DIVIDE(ncycles - overhead, high ? NS_CYCLES_PER_ITER_HIGH : NS_CYCLES_PER_ITER_LOW));
}

void machine_bitstream_high_low(mp_hal_pin_obj_t p, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint32_t pin = p->pin;
    uint32_t reg;
    if (pin >= 32) {
        pin -= 32;
        reg = NRF_P1_BASE + 0x504;
        NRF_P1->DIRSET = (1 << pin);
    } else {
        reg = NRF_P0_BASE + 0x504;
        NRF_P0->DIRSET = (1 << pin);
    }
    uint32_t lo_mask = ~(1 << pin);
    uint32_t hi_mask = 1 << pin;

    // Convert ns to loop iterations [high_time_0, low_time_0, high_time_1, low_time_1].

    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = mp_hal_delay_ns_calc(timing_ns[i], i % 2 == 0);
    }

    uint32_t irq_state = mp_hal_quiet_timing_enter();

    __asm volatile (
        // Force consistent register assignment.
        // r6 = len
        "ldr r6, %0\n"
        // r4 = buf
        "ldr r4, %1\n"
        // r5 = timing_ms
        "ldr r5, %2\n"
        // r1 = GPIO reg
        "ldr r1, %5\n"
        // r8 GPIO reg value
        "ldr r8, [r1, #0]\n"
        // r9 Hi-Mask
        "ldr r9, %3\n"
        // r10 Lo-Mask
        "ldr r10, %4\n"

        // // Must align for consistent timing.
        ".align 4\n"

        // Don't increment/decrement before first iteration.
        "b .outer2\n"
        ".outer:\n"
        // ++buf, --len
        "  add r4, #1\n"
        "  sub r6, #1\n"

        // len iterations
        ".outer2:\n"
        "  cmp r6, #0\n"
        "  beq .done\n"
        // r0 = *buf
        "  ldrb r0, [r4, #0]\n"

        // 8 bits in byte
        "  mov r7, #8\n"
        // Reload the port value at every byte
        "  ldr r8, [r1, #0]\n"
        "  .inner:\n"
        //   *(TLS_REG *)reg |= hi_mask;
        "    orr r8, r9\n"
        "    str r8, [r1, #0]\n"

        //   r3 = (r0 >> 4) & 8    (r0 is 8 if high bit is 1 else 0)
        "    mov r2, r6\n"
        "    lsr r3, r0, #4\n"
        "    mov r6, #8\n"
        "    and r3, r6\n"
        "    mov r6, r2\n"

        //   r2 = timing_ns[r3]
        "    ldr r2, [r5, r3]\n"
        "    .loop1:\n sub r2, #1\n cmp r2, #0\n bne .loop1\n"

        //   *(TLS_REG *)reg &= lo_mask;
        "    and r8, r10\n"
        "    str r8, [r1, #0]\n"

        //   r2 = timing_ns[r3 + 4]
        "    add r3, #4\n"
        "    ldr r2, [r5, r3]\n"
        "    .loop2:\n sub r2, #1\n cmp r2, #0\n bne .loop2\n"

        //   b >>= 1
        "    lsl r0, r0, #1\n"
        "    sub r7, #1\n"
        //   continue inner loop
        "    cmp r7, #0\n"
        "    bne .inner\n"
        //   continue outer loop
        "    b .outer\n"

        ".done:\n"
        :
        : "m" (len), "m" (buf), "m" (timing_ns), "m" (hi_mask), "m" (lo_mask), "m" (reg)
        : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
        );

    mp_hal_quiet_timing_exit(irq_state);

}


#endif // MICROPY_PY_MACHINE_BITSTREAM
