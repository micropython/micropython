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

// Convert nanoseconds to CPU cycles, ensuring at least 1 cycle.
static inline uint32_t psoc_edge_ns_to_cycles(uint32_t cpu_hz, uint32_t ns) {
    uint32_t cycles = ((uint64_t)cpu_hz * ns) / 1000000000ULL;
    return cycles == 0 ? 1 : cycles;
}

static inline void psoc_edge_enable_cycle_counter(void) {
    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0) {
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

void machine_bitstream_high_low(mp_hal_pin_obj_t pin, uint32_t *timing_ns, const uint8_t *buf, size_t len) {

    GPIO_PRT_Type *port = Cy_GPIO_PortToAddr(pin->port);
    uint32_t pin_num = pin->pin;
    uint32_t cpu_hz = SystemCoreClock;
    uint32_t timing_cycles[4];

    mp_hal_pin_output(pin);

    if (cpu_hz == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid CPU clock"));
    }

    for (size_t i = 0; i < 4; ++i) {
        if (timing_ns[i] < 300) {
            mp_raise_ValueError(MP_ERROR_TEXT("timing must be >= 300 ns"));
        }
    }

    uint32_t high0 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[0]);
    uint32_t low0 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[1]);
    uint32_t high1 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[2]);
    uint32_t low1 = psoc_edge_ns_to_cycles(cpu_hz, timing_ns[3]);

    uint32_t period0 = high0 + low0;
    uint32_t period1 = high1 + low1;

    timing_cycles[0] = high0;
    timing_cycles[1] = period0;
    timing_cycles[2] = high1;
    timing_cycles[3] = period1;

    psoc_edge_enable_cycle_counter();

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = buf[i];
        for (size_t j = 0; j < 8; ++j) {
            uint32_t *t = &timing_cycles[(b >> 7) * 2];
            uint32_t start_cycles = DWT->CYCCNT;

            Cy_GPIO_Write(port, pin_num, 1);
            while ((uint32_t)(DWT->CYCCNT - start_cycles) < t[0]) {
            }

            Cy_GPIO_Write(port, pin_num, 0);
            b <<= 1;
            while ((uint32_t)(DWT->CYCCNT - start_cycles) < t[1]) {
            }
        }
    }

    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

#endif // MICROPY_PY_MACHINE_BITSTREAM
