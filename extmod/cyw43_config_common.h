/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George, Angus Gratton
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
#ifndef MICROPY_INCLUDED_EXTMOD_CYW43_CONFIG_COMMON_H
#define MICROPY_INCLUDED_EXTMOD_CYW43_CONFIG_COMMON_H

// The board-level config will be included here, so it can set some CYW43 values.
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/modnetwork.h"
#include "lwip/apps/mdns.h"
#include "pendsv.h"

// This file is included at the top of port-specific cyw43_configport.h files,
// and holds the MicroPython-specific but non-port-specific parts.
//
// It's included into both MicroPython sources and the lib/cyw43-driver sources.

#define CYW43_IOCTL_TIMEOUT_US          (1000000)
#define CYW43_NETUTILS                  (1)
#define CYW43_PRINTF(...)               mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)

#define CYW43_EPERM                     MP_EPERM // Operation not permitted
#define CYW43_EIO                       MP_EIO // I/O error
#define CYW43_EINVAL                    MP_EINVAL // Invalid argument
#define CYW43_ETIMEDOUT                 MP_ETIMEDOUT // Connection timed out

#define CYW43_THREAD_ENTER              MICROPY_PY_LWIP_ENTER
#define CYW43_THREAD_EXIT               MICROPY_PY_LWIP_EXIT
#define CYW43_THREAD_LOCK_CHECK

#define CYW43_HOST_NAME                 mod_network_hostname_data

#define CYW43_ARRAY_SIZE(a)             MP_ARRAY_SIZE(a)

#define CYW43_HAL_PIN_MODE_INPUT        MP_HAL_PIN_MODE_INPUT
#define CYW43_HAL_PIN_MODE_OUTPUT       MP_HAL_PIN_MODE_OUTPUT
#define CYW43_HAL_PIN_PULL_NONE         MP_HAL_PIN_PULL_NONE
#define CYW43_HAL_PIN_PULL_UP           MP_HAL_PIN_PULL_UP
#define CYW43_HAL_PIN_PULL_DOWN         MP_HAL_PIN_PULL_DOWN

#define CYW43_HAL_MAC_WLAN0             MP_HAL_MAC_WLAN0
#define CYW43_HAL_MAC_BDADDR            MP_HAL_MAC_BDADDR

#define cyw43_hal_ticks_us              mp_hal_ticks_us
#define cyw43_hal_ticks_ms              mp_hal_ticks_ms

#define cyw43_hal_pin_obj_t             mp_hal_pin_obj_t
#define cyw43_hal_pin_config            mp_hal_pin_config
#define cyw43_hal_pin_read              mp_hal_pin_read
#define cyw43_hal_pin_low               mp_hal_pin_low
#define cyw43_hal_pin_high              mp_hal_pin_high

#define cyw43_hal_get_mac               mp_hal_get_mac
#define cyw43_hal_get_mac_ascii         mp_hal_get_mac_ascii
#define cyw43_hal_generate_laa_mac      mp_hal_generate_laa_mac

#define cyw43_schedule_internal_poll_dispatch(func) pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, func)

// Note: this function is only called if CYW43_POST_POLL_HOOK is defined in cyw43_configport.h
void cyw43_post_poll_hook(void);

#ifdef MICROPY_EVENT_POLL_HOOK
// Older style hook macros on some ports
#define CYW43_EVENT_POLL_HOOK MICROPY_EVENT_POLL_HOOK
#else
// Newer style hooks on other ports
#define CYW43_EVENT_POLL_HOOK mp_event_handle_nowait()
#endif

static inline void cyw43_delay_us(uint32_t us) {
    uint32_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

static inline void cyw43_delay_ms(uint32_t ms) {
    // PendSV may be disabled via CYW43_THREAD_ENTER, so this delay is a busy loop.
    uint32_t us = ms * 1000;
    uint32_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
        CYW43_EVENT_POLL_HOOK;
    }
}

#if LWIP_MDNS_RESPONDER == 1

// Hook for any additional TCP/IP initialization than needs to be done.
// Called after the netif specified by `itf` has been set up.
#ifndef CYW43_CB_TCPIP_INIT_EXTRA
#define CYW43_CB_TCPIP_INIT_EXTRA(self, itf) mdns_resp_add_netif(&self->netif[itf], mod_network_hostname_data)
#endif

// Hook for any additional TCP/IP deinitialization than needs to be done.
// Called before the netif specified by `itf` is removed.
#ifndef CYW43_CB_TCPIP_DEINIT_EXTRA
#define CYW43_CB_TCPIP_DEINIT_EXTRA(self, itf) mdns_resp_remove_netif(&self->netif[itf])
#endif

#endif

#endif // MICROPY_INCLUDED_EXTMOD_CYW43_CONFIG_COMMON_H
