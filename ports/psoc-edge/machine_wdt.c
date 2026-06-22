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

// PSoC Edge WDT implementation using the PDL cy_wdt driver.
//
// Hardware: CY_IP_MXS22SRSS, 22-bit counter clocked by PILO at 32768 Hz.
//
// The WDT resets the device on the 3rd consecutive unhandled match event.
// Reset period:
//   ticks = MatchValue + 2 * 2^(match_bits + 1)
//
// Cy_WDT_SetMatchBits(N) writes N to SRSS_WDT_MATCH2.IGNORE_BITS_ABOVE, making
// bits [N:0] the active comparison window → effective period = 2^(N+1) ticks.
// MatchValue offsets the first match within that period.
//
// Feed (Cy_WDT_ClearWatchdog) clears the pending match flag, resetting the
// 3-match counter to zero.

#include "cy_wdt.h"
#include "cy_sysclk.h"

#include "modmachine.h"

// PILO frequency used by the WDT on PSoC Edge (CY_IP_MXS22SRSS).
#define WDT_CLK_FREQ_HZ     (CY_SYSCLK_PILO_FREQ)   // 32768 Hz

// WDT counter bit-width on PSoC Edge.
#define WDT_COUNTER_BITS    (22U)

// Maximum achievable timeout:
//   max_ticks = (2^22 - 1) + 2 * 2^22 = 3 * 2^22 - 1 = 12582911
//   max_ms    = 12582911 * 1000 / 32768 = 383999 ms
#define WDT_TIMEOUT_MAX_MS  ((uint32_t)(((3ULL * (1UL << WDT_COUNTER_BITS) - 1ULL) \
    * 1000ULL) / WDT_CLK_FREQ_HZ))

// Minimum achievable timeout (IgnoreBits = WDT_COUNTER_BITS - 1 = 21,
// period = 2 ticks, MatchValue = 0 → ticks = 2):
//   min_ms = 2 * 1000 / 32768 < 1ms; we clamp to 1 ms.
#define WDT_TIMEOUT_MIN_MS  (1U)

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

static const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};
static bool wdt_active = false;

// Compute floor(log2(x)) for x >= 1.
static uint32_t wdt_log2_floor(uint32_t x) {
    uint32_t n = 0;
    while (x > 1) {
        x >>= 1;
        n++;
    }
    return n;
}

// Configure and start the WDT with the given timeout in milliseconds.
// The WDT will reset the device if not fed within approximately timeout_ms.
static void wdt_init(mp_int_t timeout_ms) {
    // Convert ms to PILO ticks (no overflow: max is 12582911 < 2^24).
    uint32_t timeout_ticks = (uint32_t)((uint64_t)timeout_ms * WDT_CLK_FREQ_HZ / 1000UL);

    // Choose IgnoreBits such that period = 2^(22 - IgnoreBits) <= timeout_ticks / 2.
    // This ensures MatchValue = timeout_ticks - 2 * period >= 0.
    uint32_t half_ticks = timeout_ticks / 2U;
    uint32_t period_bits = (half_ticks > 0U) ? wdt_log2_floor(half_ticks) : 1U;
    // Clamp to the valid counter width.
    if (period_bits > WDT_COUNTER_BITS) {
        period_bits = WDT_COUNTER_BITS;
    }
    if (period_bits < 1U) {
        period_bits = 1U;
    }

    // SetMatchBits(N) makes bits [N:0] active → (N+1)-bit counter → period = 2^(N+1).
    // We want period = 2^period_bits, so bitPos = period_bits - 1.
    uint32_t match_bits = period_bits - 1U;
    uint32_t period = (1UL << period_bits);
    uint32_t match = timeout_ticks - 2U * period;
    // Clamp match to valid range [0, period - 1].
    if (match >= period) {
        match = period - 1U;
    }

    // --- Configure WDT ---
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
    if (wdt_active) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT already active"));
    }
    if (timeout_ms < WDT_TIMEOUT_MIN_MS) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    }
    if ((uint32_t)timeout_ms > WDT_TIMEOUT_MAX_MS) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too long"));
    }

    wdt_init(timeout_ms);
    wdt_active = true;
    return (machine_wdt_obj_t *)&machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    // Unlock → clear → lock, matching the official Infineon example pattern.
    Cy_WDT_Unlock();
    Cy_WDT_ClearWatchdog();
    Cy_WDT_Lock();
}

void machine_wdt_deinit() {
    Cy_WDT_Unlock();
    Cy_WDT_Disable();
    Cy_WDT_Lock();
}
