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

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "cy_wdt.h"
#include "cy_sysclk.h"

#include "modmachine.h"

#define WDT_CLK_FREQ_HZ     (CY_SYSCLK_PILO_FREQ)   // 32768 Hz

// Maximum timeout in ms: (3 * 2^SRSS_NUM_WDT_A_BITS - 1) * 1000 / WDT_CLK_FREQ_HZ
#define WDT_TIMEOUT_MAX_MS  ((uint32_t)(((3ULL * (1ULL << SRSS_NUM_WDT_A_BITS) - 1ULL) \
    * 1000ULL) / WDT_CLK_FREQ_HZ))

#define WDT_TIMEOUT_MIN_MS  (1U)

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

static const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};

// Compute floor(log2(x)) for x >= 1.
static uint32_t wdt_log2_floor(uint32_t x) {
    uint32_t n = 0;
    while (x > 1) {
        x >>= 1;
        n++;
    }
    return n;
}

static void wdt_init(mp_int_t timeout_ms) {
    // Convert timeout to PILO ticks, then compute match/period registers.
    uint32_t timeout_ticks = (uint32_t)((uint64_t)timeout_ms * WDT_CLK_FREQ_HZ / 1000UL);
    uint32_t half_ticks = timeout_ticks / 2U;
    uint32_t period_bits = (half_ticks > 0U) ? wdt_log2_floor(half_ticks) : 1U;
    uint32_t match_bits = period_bits - 1U;
    uint32_t period = (1UL << period_bits);
    uint32_t match = timeout_ticks - 2U * period;
    if (match >= period) {
        match = period - 1U;
    }

    Cy_WDT_Unlock();
    Cy_WDT_SetMatchBits(match_bits);
    Cy_WDT_SetMatch(match);
    Cy_WDT_ClearInterrupt();
    Cy_WDT_Enable();
    Cy_WDT_Lock();
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }
    if (timeout_ms < WDT_TIMEOUT_MIN_MS) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    }
    if ((uint32_t)timeout_ms > WDT_TIMEOUT_MAX_MS) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too long"));
    }

    wdt_init(timeout_ms);
    return (machine_wdt_obj_t *)&machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;

    Cy_WDT_Unlock();
    Cy_WDT_ClearWatchdog();
    Cy_WDT_Lock();
}

void machine_wdt_deinit(void) {
    Cy_WDT_Unlock();
    Cy_WDT_Disable();
    Cy_WDT_Lock();
}
