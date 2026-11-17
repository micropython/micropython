/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include "py/mphal.h"
#include "shared/runtime/softtimer.h"

#if MICROPY_PY_LWIP

#include "lwip/timeouts.h"
#include "eth.h"

// Poll lwIP (and the polled NIC RX) at this rate, in milliseconds.
#define LWIP_TICK_RATE_MS (8)

// Soft timer for running lwIP in the background.
static soft_timer_entry_t network_timer;

u32_t sys_now(void) {
    return mp_hal_ticks_ms();
}

// This is called by soft_timer and executes at PendSV level.
static void network_timer_callback(soft_timer_entry_t *self) {
    (void)self;
    // Drain the NIC RX FIFO (there is no RX interrupt on this port) and run the
    // lwIP internal updates.
    eth_poll();
    sys_check_timeouts();
}

void mod_network_lwip_init(void) {
    soft_timer_remove(&network_timer);
    soft_timer_static_init(&network_timer, SOFT_TIMER_MODE_PERIODIC, LWIP_TICK_RATE_MS, network_timer_callback);
    soft_timer_reinsert(&network_timer, LWIP_TICK_RATE_MS);
}

#endif // MICROPY_PY_LWIP
