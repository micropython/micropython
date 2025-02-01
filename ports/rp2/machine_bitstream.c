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
#include "hardware/structs/systick.h"

#if MICROPY_PY_MACHINE_BITSTREAM

#if PICO_RP2350
#define MP_HAL_BITSTREAM_NS_OVERHEAD  (5)
#else
#define MP_HAL_BITSTREAM_NS_OVERHEAD  (9)
#endif

#if PICO_RISCV

__attribute__((naked)) void mcycle_init(void) {
    __asm volatile (
        "li a0, 4\n"
        "csrw mcountinhibit, a0\n"
        "ret\n"
        );
}

__attribute__((naked)) uint32_t mcycle_get(void) {
    __asm volatile (
        "csrr a0, mcycle\n"
        "ret\n"
        );
}

#endif

void __time_critical_func(machine_bitstream_high_low)(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {
    uint32_t fcpu_mhz = mp_hal_get_cpu_freq() / 1000000;
    // Convert ns to clock ticks [high_time_0, period_0, high_time_1, period_1].
    for (size_t i = 0; i < 4; ++i) {
        timing_ns[i] = fcpu_mhz * timing_ns[i] / 1000;
        if (timing_ns[i] > (2 * MP_HAL_BITSTREAM_NS_OVERHEAD)) {
            timing_ns[i] -= MP_HAL_BITSTREAM_NS_OVERHEAD;
        }
        if (i % 2 == 1) {
            // Convert low_time to period (i.e. add high_time).
            timing_ns[i] += timing_ns[i - 1] - MP_HAL_BITSTREAM_NS_OVERHEAD;
        }
    }
    mp_hal_pin_output(pin);

    uint32_t irq_state = mp_hal_quiet_timing_enter();

    #if PICO_ARM

    // Set systick reset value.
    systick_hw->rvr = 0x00FFFFFF;

    // Enable the systick counter, source CPU clock.
    systick_hw->csr = 5;

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            uint32_t *t = &timing_ns[b >> 6 & 2];
            uint32_t start_ticks = systick_hw->cvr = SYSTICK_MAX;
            mp_hal_pin_high(pin);
            while ((start_ticks - systick_hw->cvr) < t[0]) {
            }
            b <<= 1;
            mp_hal_pin_low(pin);
            while ((start_ticks - systick_hw->cvr) < t[1]) {
            }
        }
    }

    #elif PICO_RISCV

    mcycle_init();

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            uint32_t *t = &timing_ns[b >> 6 & 2];
            uint32_t start_ticks = mcycle_get();
            mp_hal_pin_high(pin);
            while ((mcycle_get() - start_ticks) < t[0]) {
            }
            b <<= 1;
            mp_hal_pin_low(pin);
            while ((mcycle_get() - start_ticks) < t[1]) {
            }
        }
    }

    #endif

    mp_hal_quiet_timing_exit(irq_state);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
