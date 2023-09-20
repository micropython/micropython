/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
#include "shared/runtime/softtimer.h"

#if MICROPY_PY_LWIP

#include "lwip/timeouts.h"

static mp_sched_node_t network_poll_node;
static soft_timer_entry_t network_timer;

u32_t sys_now(void) {
    return mp_hal_ticks_ms();
}

static void network_poll(mp_sched_node_t *node) {
    // Run the lwIP internal updates
    sys_check_timeouts();

    #if MICROPY_PY_NETWORK_ESP_HOSTED
    extern int esp_hosted_wifi_poll(void);
    // Poll the NIC for incoming data
    if (esp_hosted_wifi_poll() == -1) {
        soft_timer_remove(&network_timer);
    }
    #endif
}

void mod_network_poll_events(void) {
    mp_sched_schedule_node(&network_poll_node, network_poll);
}

static void network_timer_callback(soft_timer_entry_t *self) {
    mod_network_poll_events();
}

void mod_network_lwip_init(void) {
    static bool timer_started = false;
    if (timer_started) {
        soft_timer_remove(&network_timer);
        timer_started = false;
    }
    // Start poll timer.
    soft_timer_static_init(&network_timer, SOFT_TIMER_MODE_PERIODIC, 50, network_timer_callback);
    soft_timer_reinsert(&network_timer, 50);
    timer_started = true;
}
#endif // MICROPY_PY_LWIP
