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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/netutils/netutils.h"
#include "systick.h"
#include "pendsv.h"
#include "extmod/modnetwork.h"

#if MICROPY_PY_LWIP
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/apps/mdns.h"

#if MICROPY_PY_NETWORK_CYW43
#include "extmod/network_cyw43.h"
#include "lib/cyw43-driver/src/cyw43.h"
#endif

// Poll lwIP every 128ms
#define LWIP_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & 0x7f) == 0)

#if MICROPY_PY_NETWORK_WIZNET5K
void wiznet5k_poll(void);
void wiznet5k_deinit(void);

void wiznet5k_try_poll(void) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_WIZNET, wiznet5k_poll);
}
#endif

u32_t sys_now(void) {
    return mp_hal_ticks_ms();
}

STATIC void pyb_lwip_poll(void) {
    #if MICROPY_PY_NETWORK_WIZNET5K
    // Poll the NIC for incoming data
    wiznet5k_poll();
    #endif

    // Run the lwIP internal updates
    sys_check_timeouts();
}

void mod_network_lwip_poll_wrapper(uint32_t ticks_ms) {
    if (LWIP_TICK(ticks_ms)) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_LWIP, pyb_lwip_poll);
    }

    #if MICROPY_PY_NETWORK_CYW43
    if (cyw43_poll) {
        if (cyw43_sleep != 0) {
            if (--cyw43_sleep == 0) {
                pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
            }
        }
    }
    #endif
}

#endif // MICROPY_PY_LWIP
