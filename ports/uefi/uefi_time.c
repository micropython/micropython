/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// UEFI HAL timebase: ticks_us/ms/cpu from a free-running hardware counter
// (CNTVCT_EL0 on aarch64, the TSC on x86-64), plus a yielding millisecond delay.
//
// We deliberately use a counter rather than a periodic tick interrupt: it is
// monotonic, high-resolution, ring-0 cheap, and needs no ISR. The aarch64
// counter is self-describing (CNTFRQ_EL0); the x86-64 TSC is calibrated once
// against BootServices->Stall at init (CPUID 0x15 is unreliable under TCG).

#include "py/mphal.h"
#include "py/runtime.h"
#include "efi.h"
#include "uefi_port.h"

// Counter frequency in Hz (ticks per second). Cached at init.
static uint64_t counter_hz = 1;

#if defined(__aarch64__)

static inline uint64_t read_counter(void) {
    uint64_t v;
    // ISB so the read isn't speculated ahead of preceding instructions.
    __asm__ volatile ("isb; mrs %0, cntvct_el0" : "=r" (v));
    return v;
}

static inline uint64_t read_counter_hz(void) {
    uint64_t v;
    __asm__ volatile ("mrs %0, cntfrq_el0" : "=r" (v));
    return v;
}

#elif defined(__x86_64__)

static inline uint64_t read_counter(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#else
#error "uefi_time.c: unsupported architecture"
#endif

// Scale a raw counter value to microseconds/milliseconds without overflowing:
// split into whole-second and remainder parts so the intermediate product
// stays < 2^64 (remainder < counter_hz, times 1e6, fits comfortably).
static inline uint64_t counter_to_us(uint64_t t) {
    return (t / counter_hz) * 1000000ULL + ((t % counter_hz) * 1000000ULL) / counter_hz;
}

static inline uint64_t counter_to_ms(uint64_t t) {
    return (t / counter_hz) * 1000ULL + ((t % counter_hz) * 1000ULL) / counter_hz;
}

void mp_uefi_time_init(void) {
    #if defined(__aarch64__)
    counter_hz = read_counter_hz();
    if (counter_hz == 0) {
        counter_hz = 1;
    }
    #elif defined(__x86_64__)
    // Calibrate the TSC against a known Stall. 100 ms is long enough for a
    // stable rate yet short enough not to slow boot perceptibly.
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;
    uint64_t t0 = read_counter();
    bs->Stall(100000); // microseconds
    uint64_t t1 = read_counter();
    counter_hz = (t1 - t0) * 10ULL; // ticks in 100 ms -> ticks per second
    if (counter_hz == 0) {
        counter_hz = 1;
    }
    #endif
}

mp_uint_t mp_hal_ticks_us(void) {
    return (mp_uint_t)counter_to_us(read_counter());
}

mp_uint_t mp_hal_ticks_ms(void) {
    return (mp_uint_t)counter_to_ms(read_counter());
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return (mp_uint_t)read_counter();
}

// The monotonic counter frequency in Hz (for machine.freq()).
uint64_t mp_uefi_counter_hz(void) {
    return counter_hz;
}

void mp_hal_delay_us(mp_uint_t us) {
    // Short, exact busy wait (callable at any TPL).
    mp_uefi_st->BootServices->Stall((UINTN)us);
}

void mp_hal_delay_ms(mp_uint_t ms) {
    // Yielding wait: park in WaitForEvent so timers, scheduled callbacks and
    // async events keep flowing (and a pending KeyboardInterrupt is raised)
    // while we sleep, instead of spinning in Stall.
    mp_uint_t start = mp_hal_ticks_ms();
    for (;;) {
        mp_uint_t elapsed = mp_hal_ticks_ms() - start;
        if (elapsed >= ms) {
            break;
        }
        mp_event_wait_ms(ms - elapsed);
    }
}
