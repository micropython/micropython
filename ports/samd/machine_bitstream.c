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

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "clock_config.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#if __CORTEX_M == 0

// No cycle counter on M0, do manual cycle counting instead.

// SAMD21 @ 48MHz
#define NS_CYCLES_PER_ITER_HIGH (3)
#define NS_CYCLES_PER_ITER_LOW (3)
#define NS_OVERHEAD_CYCLES_HIGH (12)
#define NS_OVERHEAD_CYCLES_LOW (15)

uint32_t mp_hal_delay_ns_calc(uint32_t ns, bool high) {
    uint32_t ncycles = (get_cpu_freq() / 1000000 * ns + 500) / 1000; // + 500 for proper rounding
    uint32_t overhead = MIN(ncycles, high ? NS_OVERHEAD_CYCLES_HIGH : NS_OVERHEAD_CYCLES_LOW);
    return MAX(1, MP_ROUND_DIVIDE(ncycles - overhead, high ? NS_CYCLES_PER_ITER_HIGH : NS_CYCLES_PER_ITER_LOW));
}

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    volatile const uint32_t mask = 1 << (pin % 32);
    volatile uint32_t *outclr = &PORT->Group[pin / 32].OUTCLR.reg;
    volatile uint32_t *outset = &PORT->Group[pin / 32].OUTSET.reg;
    mp_hal_pin_output(pin);

    // Convert ns to loop iterations [high_time_0, low_time_0, high_time_1, low_time_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = mp_hal_delay_ns_calc(timing_ns[i], i % 2 == 0);
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    // Measured timing for SAMD21 at 48MHz (cycle=20.83ns)
    // timing_ns = (1,1,1,1)
    //   high:  310
    //   low:   375
    //   high0: 375
    //   low0:  400
    // timing_ns = (500, 500, 500, 500)
    //   high:  500
    //   low:   500
    //   high0: 565
    //   low0:  540
    // timing_ns = (1000, 1000, 1000, 1000)
    //   high:  1000
    //   low:   1000
    //   high0: 1065
    //   low0:  1040

    // --> high is 12 + n*3 cycles
    //     low is 15 + n*3 cycles

    // NeoPixel timing (400, 850, 800, 450) (+/-150ns) gives timing_ns=(2, 9, 8, 3) which in cycles is
    // (12 + 6, 15 + 27, 15 + 24, 12 + 9) = (18, 42, 39, 21)
    // --> (375, 875, 812, 437) nanoseconds.
    // Measured output on logic analyser is (375, 875, 815, 435) (+/-5ns at 200MHz)

    // Note: the first high/low cycle is longer by 2-3 cycles (40-60ns).
    // This is slightly outside spec, but doesn't seem to cause a problem.

    __asm volatile (
        // Force consistent register assignment.
        // r6 = len
        "ldr r6, %0\n"
        // r4 = buf
        "ldr r4, %1\n"
        // r5 = timing_ms
        "ldr r5, %2\n"

        // Must align for consistent timing.
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
        "  .inner:\n"
        //   *outset = mask
        "    ldr r2, %3\n"
        "    ldr r1, %5\n"
        "    str r1, [r2, #0]\n"

        //   r3 = (r0 >> 4) & 8    (r0 is 8 if high bit is 1 else 0)
        "    mov r8, r6\n"
        "    lsr r3, r0, #4\n"
        "    mov r6, #8\n"
        "    and r3, r6\n"
        "    mov r6, r8\n"

        //   r2 = timing_ns[r2]
        "    ldr r2, [r5, r3]\n"
        "    .loop1:\n sub r2, #1\n bne .loop1\n"

        //   *outclr = mask
        "    ldr r2, %4\n"
        "    str r1, [r2, #0]\n"

        //   r2 = timing_ns[r3 + 4]
        "    add r3, #4\n"
        "    ldr r2, [r5, r3]\n"
        "    .loop2:\n sub r2, #1\n bne .loop2\n"

        //   b >>= 1
        "    lsl r0, r0, #1\n"

        "    sub r7, #1\n"
        //   end of inner loop
        "    beq .outer\n"
        //   continue inner loop
        "    b .inner\n"

        ".done:\n"
        :
        : "m" (len), "m" (buf), "m" (timing_ns), "m" (outset), "m" (outclr), "m" (mask)
        : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8"
        );

    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

#else // > CORTEX_M0

#define NS_TICKS_OVERHEAD (40)

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint32_t fcpu_mhz = get_cpu_freq() / 1000000;
    uint32_t ticks_overhead = fcpu_mhz * NS_TICKS_OVERHEAD / 1000;
    mp_hal_pin_output(pin);
    // Convert ns to us ticks [high_time_0, period_0, high_time_1, period_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
        if (timing_ns[i] > ticks_overhead) {
            timing_ns[i] -= ticks_overhead;
        }
        if (i % 2 == 1) {
            // Convert low_time to period (i.e. add high_time).
            timing_ns[i] += timing_ns[i - 1] - ticks_overhead;
        }
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    DWT->CYCCNT = 0;

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            uint32_t start_ticks = mp_hal_ticks_cpu();
            uint32_t *t = &timing_ns[b >> 6 & 2];
            mp_hal_pin_high(pin);
            while ((mp_hal_ticks_cpu() - start_ticks) < t[0]) {
            }
            mp_hal_pin_low(pin);
            b <<= 1;
            while ((mp_hal_ticks_cpu() - start_ticks) < t[1]) {
            }
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);

}

#endif // > CORTEX_M0

#endif // MICROPY_PY_MACHINE_BITSTREAM
