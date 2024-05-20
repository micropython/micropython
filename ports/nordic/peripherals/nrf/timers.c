// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/pulseio/PulseOut.h"
#include "peripherals/nrf/timers.h"

#include <stdint.h>

#include "nrfx.h"
#include "nrfx_timer.h"

#include "py/mpconfig.h"
#include "py/runtime.h"

static nrfx_timer_t nrfx_timers[] = {
    #if NRFX_CHECK(NRFX_TIMER0_ENABLED)
    #error NRFX_TIMER0_ENABLED should not be on: TIMER0 is used by the SoftDevice
    NRFX_TIMER_INSTANCE(0),
    #endif
    #if NRFX_CHECK(NRFX_TIMER1_ENABLED)
    NRFX_TIMER_INSTANCE(1),
    #endif
    #if NRFX_CHECK(NRFX_TIMER2_ENABLED)
    NRFX_TIMER_INSTANCE(2),
    #endif
    #if NRFX_CHECK(NRFX_TIMER3_ENABLED)
    NRFX_TIMER_INSTANCE(3),
    #endif
    #if NRFX_CHECK(NRFX_TIMER4_ENABLED)
    NRFX_TIMER_INSTANCE(4),
    #endif
};

static bool nrfx_timer_allocated[ARRAY_SIZE(nrfx_timers)];
static bool nrfx_timer_never_reset[ARRAY_SIZE(nrfx_timers)];

void timers_reset(void) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i++) {
        if (nrfx_timer_never_reset[i]) {
            continue;
        }
        nrfx_timer_uninit(&nrfx_timers[i]);
        nrfx_timer_allocated[i] = false;
    }
}

void nrf_peripherals_timer_never_reset(nrfx_timer_t *timer) {
    int idx = nrf_peripherals_timer_idx_from_timer(timer);
    nrfx_timer_never_reset[idx] = true;
}

void nrf_peripherals_timer_reset_ok(nrfx_timer_t *timer) {
    int idx = nrf_peripherals_timer_idx_from_timer(timer);
    nrfx_timer_never_reset[idx] = false;
}

nrfx_timer_t *nrf_peripherals_timer_from_reg(NRF_TIMER_Type *ptr) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i++) {
        if (nrfx_timers[i].p_reg == ptr) {
            return &nrfx_timers[i];
        }
    }
    return NULL;
}

size_t nrf_peripherals_timer_idx_from_timer(nrfx_timer_t *ptr) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i++) {
        if (&nrfx_timers[i] == ptr) {
            return i;
        }
    }
    return ~(size_t)0;
}


// Returns a free nrfx_timer instance, and marks it as allocated.
// The caller should init as with the desired config.
// Returns NULL if no timer is available.
nrfx_timer_t *nrf_peripherals_allocate_timer(void) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i++) {
        if (!nrfx_timer_allocated[i]) {
            nrfx_timer_allocated[i] = true;
            return &nrfx_timers[i];
        }
    }
    return NULL;
}

nrfx_timer_t *nrf_peripherals_allocate_timer_or_throw(void) {
    nrfx_timer_t *result = nrf_peripherals_allocate_timer();
    if (!result) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All timers in use"));
    }
    return result;
}

// Free a timer, which may or may not have been initialized.
void nrf_peripherals_free_timer(nrfx_timer_t *timer) {
    size_t idx = nrf_peripherals_timer_idx_from_timer(timer);
    if (idx != ~(size_t)0) {
        nrfx_timer_allocated[idx] = false;
        nrfx_timer_never_reset[idx] = false;
        // Safe to call even if not initialized.
        nrfx_timer_uninit(timer);
    }
}
