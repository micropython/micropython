/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#if MICROPY_PY_LWIP

#include "lwip/timeouts.h"
#include "pico/time.h"

// Poll lwIP every 64ms by default
#define LWIP_TICK_RATE_MS 64

static alarm_id_t lwip_alarm_id = -1;
static bool lwip_can_poll = true;
static bool lwip_poll_pending = false;

#if MICROPY_PY_NETWORK_WIZNET5K
static bool wiznet_poll_pending = false;

void wiznet5k_poll(void);
void wiznet5k_deinit(void);
#endif

u32_t sys_now(void) {
    // Used by LwIP
    return mp_hal_ticks_ms();
}

STATIC uint32_t lwip_poll(void) {
    // Run the lwIP internal updates
    sys_check_timeouts();

    return MAX(5, MIN(sys_timeouts_sleeptime(), LWIP_TICK_RATE_MS));
}

void lwip_lock_acquire(void) {
    lwip_can_poll = false;
}

void lwip_lock_release(void) {
    lwip_can_poll = false;
    #if MICROPY_PY_NETWORK_WIZNET5K
    if (wiznet_poll_pending) {
        wiznet5k_poll();
        wiznet_poll_pending = false;
    }
    #endif

    if (lwip_poll_pending) {
        lwip_poll();
        lwip_poll_pending = false;
    }
    lwip_can_poll = true;
}

uint32_t lwip_try_poll(void) {
    uint32_t ret = LWIP_TICK_RATE_MS;
    if (lwip_can_poll) {
        lwip_can_poll = false;
        ret = lwip_poll();
        lwip_can_poll = true;
    } else {
        lwip_poll_pending = true;
    }
    return ret;
}

#if MICROPY_PY_NETWORK_WIZNET5K
void wiznet5k_try_poll(void) {
    if (lwip_can_poll) {
        lwip_can_poll = false;
        wiznet5k_poll();
        lwip_can_poll = true;
    } else {
        wiznet_poll_pending = true;
    }
}
#endif

STATIC int64_t alarm_callback(alarm_id_t id, void *user_data) {
    #if MICROPY_PY_NETWORK_WIZNET5K
    wiznet5k_try_poll();
    #endif
    return (int64_t)lwip_try_poll() * 1000;
}

void mod_network_lwip_init(void) {
    #if MICROPY_PY_NETWORK_WIZNET5K
    wiznet5k_deinit();
    #endif
    if (lwip_alarm_id != -1) {
        cancel_alarm(lwip_alarm_id);
    }
    lwip_alarm_id = add_alarm_in_us(LWIP_TICK_RATE_MS * 1000, alarm_callback, mp_const_true, true);
}

#endif // MICROPY_PY_LWIP
