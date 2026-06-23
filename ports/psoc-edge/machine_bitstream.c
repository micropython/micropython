/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "py/mpconfig.h"
#include "py/mphal.h"
#include <stdbool.h>
#include <string.h>
#include "py/runtime.h"
#include "extmod/modmachine.h"

#if MICROPY_PY_MACHINE_BITSTREAM

/**
 * machine_bitstream_high_low: Bit-bang protocol implementation for WS2812 and similar LED protocols.
 *
 * Uses absolute DWT cycle-counter timing for nanosecond-precision GPIO bit-banging.
 * Timing array layout: [T0H, T0_period, T1H, T1_period]
 * - Index 0,1: bit-0 timings (high pulse, total period)
 * - Index 2,3: bit-1 timings (high pulse, total period)
 */

// Convert nanoseconds to CPU cycles, ensuring at least 1 cycle.
// At 50 MHz: 1 cycle = 20 ns
static inline uint32_t psoc_edge_ns_to_cycles(uint32_t cpu_hz, uint32_t ns) {
    uint32_t cycles = ((uint64_t)cpu_hz * ns) / 1000000000ULL;
    return cycles == 0 ? 1 : cycles;
}

// Enable ARM Cortex-M33 DWT cycle counter for precise timing.
// CoreDebug->DEMCR: enables debug/trace infrastructure.
// DWT->CTRL: enables the cycle counter register (increments every CPU clock).
static inline void psoc_edge_enable_cycle_counter(void) {
    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0) {
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

/**
 * machine_bitstream_high_low - Transmit bit-stream with custom timing.
 *
 * @pin: GPIO pin object
 * @timing_ns: 4-element array [T0H, T0L, T1H, T1L] in nanoseconds
 * @buf: byte buffer to transmit
 * @len: length of buffer
 *
 * Transmits MSB-first (bit 7 to bit 0) using absolute DWT cycle-counter timing.
 * Minimum per-element timing: 300 ns. Maximum: platform-dependent (typically 10+ µs).
 */
void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {

    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(pin->port);
    uint32_t pin_num = pin->pin;
    uint32_t cpu_hz = SystemCoreClock;
    uint32_t timing_cycles[4];  // Layout: [T0H, T0_period, T1H, T1_period] in cycles

    mp_hal_pin_output(pin);

    // Validate CPU clock is available
    if (cpu_hz == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid CPU clock"));
    }

    // Validate user-provided timings meet minimum granularity (300 ns = 15 cycles at 50 MHz)
    for (size_t i = 0; i < 4; ++i) {
        if (timing_ns[i] < 300) {
            mp_raise_ValueError(MP_ERROR_TEXT("timing must be >= 300 ns"));
        }
    }

    // Convert nanosecond timings to CPU cycles based on runtime clock frequency
    uint32_t high0 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[0]);
    uint32_t low0 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[1]);
    uint32_t high1 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[2]);
    uint32_t low1 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[3]);

    // Precompute total bit periods for quick reference in loop
    uint32_t period0 = high0 + low0;
    uint32_t period1 = high1 + low1;

    // Populate timing array: bit-0 and bit-1 high + period times
    // Indexed by MSB value: if bit=0 use [0,1], if bit=1 use [2,3]
    timing_cycles[0] = high0;    // bit-0 high time
    timing_cycles[1] = period0;  // bit-0 total period
    timing_cycles[2] = high1;    // bit-1 high time
    timing_cycles[3] = period1;  // bit-1 total period

    // Initialize cycle counter for absolute timestamp measurements
    psoc_edge_enable_cycle_counter();

    // Disable interrupts to maintain timing precision
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    // Bit-bang loop: MSB-first (bit 7 to bit 0) over all bytes
    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            // Select timing based on MSB: (b >> 7) gives 1 or 0, multiply by 2 for array index
            uint32_t *t = &timing_cycles[(b >> 7) * 2];
            uint32_t start_cycles = DWT->CYCCNT;  // Capture start timestamp

            // Transmit high pulse with absolute timing
            Cy_GPIO_Write(port, pin_num, 1);
            while ((uint32_t)(DWT->CYCCNT - start_cycles) < t[0]) {
                // Busy-wait using cycle counter for nanosecond precision
            }

            // Transmit low pulse, filling remainder of bit period
            Cy_GPIO_Write(port, pin_num, 0);
            b <<= 1;  // Shift to next bit
            while ((uint32_t)(DWT->CYCCNT - start_cycles) < t[1]) {
                // Busy-wait until full bit period elapsed
            }
        }
    }

    // Re-enable interrupts
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
