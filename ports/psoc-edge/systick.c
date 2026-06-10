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

#include <stdbool.h>
#include <stdint.h>

#include "cybsp.h"
#include "cy_systick.h"

#include "mphalport.h"

#include "py/runtime.h"

volatile uint64_t psoc_edge_systick_ms_count;
uint32_t psoc_edge_systick_cpu_hz;

// Use the SysTick pending-interrupt state as rollover evidence. This matches our
// callback-based ms counter model ("ISR increment still outstanding") and avoids
// Cy_SysTick_GetCountFlag()'s read-to-clear side effect.
#define MP_HAL_SYSTICK_WRAP_PENDING() (((SCB->ICSR) & SCB_ICSR_PENDSTSET_Msk) != 0u)

typedef struct {
    uint64_t ms_count;
    uint32_t systick_reload;
    uint32_t systick_val;
    uint32_t cpu_hz;
} mp_hal_systick_snapshot_t;

#if !MICROPY_PY_FREERTOS
static void psoc_edge_systick_callback(void) {
    psoc_edge_systick_ms_count += 1;
}
#endif

void mp_hal_ticks_init(void) {
    psoc_edge_systick_cpu_hz = SystemCoreClock;
    psoc_edge_systick_ms_count = 0;
    #if MICROPY_PY_FREERTOS
    // SysTick is already configured by FreeRTOS vPortSetupTimerInterrupt() at
    // 1 ms per tick (configTICK_RATE_HZ == 1000). psoc_edge_systick_ms_count is
    // driven from xTaskGetTickCount() inside mp_hal_systick_snapshot() so no
    // callback setup is needed here.
    #else
    if (psoc_edge_systick_cpu_hz < 1000) {
        // Invalid system clock for a ms ticker.
        // As micropython is not yet initialized, therefore
        // mp_raise_msg() is not usable here.
        CY_ASSERT(0);
    }
    // Configure SysTick for 1ms period and count overflows in software.
    uint32_t systick_interval = psoc_edge_systick_cpu_hz / 1000;
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, systick_interval - 1);
    Cy_SysTick_SetCallback(0, psoc_edge_systick_callback);
    #endif
}

static mp_hal_systick_snapshot_t mp_hal_systick_snapshot(void) {
    mp_hal_systick_snapshot_t snap;

    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    #if MICROPY_PY_FREERTOS
    // Drive ms_count from FreeRTOS tick counter (1 tick == 1 ms, configTICK_RATE_HZ == 1000).
    // Keep psoc_edge_systick_ms_count in sync so any external reader stays consistent.
    snap.ms_count = (uint64_t)xTaskGetTickCount();
    psoc_edge_systick_ms_count = snap.ms_count;
    #else
    snap.ms_count = psoc_edge_systick_ms_count;
    #endif
    snap.systick_reload = Cy_SysTick_GetReload();
    snap.systick_val = Cy_SysTick_GetValue();
    // SysTick may have wrapped but its ISR not run yet; account for that pending rollover
    // so software time does not momentarily go backwards across the wrap boundary.
    if (MP_HAL_SYSTICK_WRAP_PENDING()) {
        snap.ms_count += 1;
    }
    snap.cpu_hz = psoc_edge_systick_cpu_hz;
    MICROPY_END_ATOMIC_SECTION(irq_state);

    if (snap.cpu_hz == 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("invalid systick CPU clock."));
    }

    return snap;
}

uint64_t mp_hal_systick_ticks_us64(void) {
    mp_hal_systick_snapshot_t snap = mp_hal_systick_snapshot();

    uint32_t elapsed_cycles = snap.systick_reload - snap.systick_val;
    return snap.ms_count * 1000ULL + ((uint64_t)elapsed_cycles * 1000000ULL) / snap.cpu_hz;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return mp_hal_systick_ticks_us64() / 1000ULL;
}

mp_uint_t mp_hal_ticks_us(void) {
    return mp_hal_systick_ticks_us64();
}

mp_uint_t mp_hal_ticks_cpu(void) {
    mp_hal_systick_snapshot_t snap = mp_hal_systick_snapshot();
    return snap.ms_count * snap.cpu_hz + (snap.systick_reload - snap.systick_val);
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    while (mp_hal_ticks_ms() - start < ms) {
        mp_event_handle_nowait();
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    mp_uint_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
        mp_event_handle_nowait();
    }
}

extern uint64_t mp_hal_time_get_epoch_seconds(void);
extern bool machine_rtc_read_and_clear_time_updated(void);

uint64_t mp_hal_time_ns(void) {
    static uint64_t time_us_offset_from_epoch = 0;
    // Resync whenever RTC time was updated (set/deinit/init path marks this).
    bool need_resync = machine_rtc_read_and_clear_time_updated();

    while (need_resync) {
        // Wait until RTC second changes so we can anchor the monotonic timer
        // to a known wall-clock second boundary.
        uint64_t prev_seconds = mp_hal_time_get_epoch_seconds();
        uint64_t edge_ticks_us;
        uint64_t edge_seconds;

        do {
            edge_seconds = mp_hal_time_get_epoch_seconds();
        } while (edge_seconds == prev_seconds);

        // Capture the monotonic timestamp immediately after observing the RTC
        // second rollover so the fixed offset stays aligned to wall-clock time.
        // `edge_seconds` is the first observed second after the rollover.
        edge_ticks_us = mp_hal_systick_ticks_us64();
        time_us_offset_from_epoch = edge_seconds * 1000000ULL - edge_ticks_us;

        // If RTC was updated during calibration, repeat once more to align with
        // the latest RTC second boundary.
        need_resync = machine_rtc_read_and_clear_time_updated();
    }

    return (time_us_offset_from_epoch + mp_hal_systick_ticks_us64()) * 1000ULL;
}
