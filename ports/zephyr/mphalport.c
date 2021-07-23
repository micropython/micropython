/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"

static struct k_poll_signal wait_signal;
static struct k_poll_event wait_events[2] = {
    K_POLL_EVENT_INITIALIZER(K_POLL_TYPE_SIGNAL,
        K_POLL_MODE_NOTIFY_ONLY,
        &wait_signal),
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_SEM_AVAILABLE,
        K_POLL_MODE_NOTIFY_ONLY,
        NULL, 0),
};

void mp_hal_init(void) {
    k_poll_signal_init(&wait_signal);
}

void mp_hal_signal_event(void) {
    k_poll_signal_raise(&wait_signal, 0);
}

void mp_hal_wait_sem(struct k_sem *sem, uint32_t timeout_ms) {
    mp_uint_t t0 = mp_hal_ticks_ms();
    if (sem) {
        k_poll_event_init(&wait_events[1], K_POLL_TYPE_SEM_AVAILABLE, K_POLL_MODE_NOTIFY_ONLY, sem);
    }
    for (;;) {
        k_timeout_t wait;
        if (timeout_ms == (uint32_t)-1) {
            wait = K_FOREVER;
        } else {
            uint32_t dt = mp_hal_ticks_ms() - t0;
            if (dt >= timeout_ms) {
                return;
            }
            wait = K_MSEC(timeout_ms - dt);
        }
        k_poll(wait_events, sem ? 2 : 1, wait);
        if (wait_events[0].state == K_POLL_STATE_SIGNALED) {
            wait_events[0].signal->signaled = 0;
            wait_events[0].state = K_POLL_STATE_NOT_READY;
            mp_handle_pending(true);
        } else if (sem && wait_events[1].state == K_POLL_STATE_SEM_AVAILABLE) {
            wait_events[1].state = K_POLL_STATE_NOT_READY;
            return;
        }
    }
}
