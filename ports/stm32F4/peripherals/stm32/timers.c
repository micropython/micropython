/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>

#include "nrfx.h"
#include "nrfx_timer.h"

#include "py/mpconfig.h"
#include "py/runtime.h"

STATIC nrfx_timer_t nrfx_timers[] = {
#if NRFX_CHECK(NRFX_TIMER0_ENABLED)
    // Note that TIMER0 is reserved for use by the SoftDevice, so it should not usually be enabled.
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

void timers_reset(void) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i ++) {
        nrfx_timer_uninit(&nrfx_timers[i]);
        nrfx_timer_allocated[i] = false;
    }
}

// Returns a free nrfx_timer instance, and marks it as allocated.
// The caller should init as with the desired config.
// Returns NULL if no timer is available.
nrfx_timer_t* nrf_peripherals_allocate_timer(void) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i ++) {
        if (!nrfx_timer_allocated[i]) {
            nrfx_timer_allocated[i] = true;
            return &nrfx_timers[i];
        }
    }
    return NULL;
}

// Free a timer, which may or may not have been initialized.
void nrf_peripherals_free_timer(nrfx_timer_t* timer) {
    for (size_t i = 0; i < ARRAY_SIZE(nrfx_timers); i ++) {
        if (timer == &nrfx_timers[i]) {
            nrfx_timer_allocated[i] = false;
            // Safe to call even if not initialized.
            nrfx_timer_uninit(timer);
            return;
        }
    }
}
