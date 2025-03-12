/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien P. George
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
#ifndef MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H
#define MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H

// The board-level config will be included here, so it can set some CYW43 values.
#include <stdio.h>
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/modnetwork.h"
#include "pendsv.h"

#define CYW43_INCLUDE_LEGACY_F1_OVERFLOW_WORKAROUND_VARIABLES (1)
#define CYW43_WIFI_NVRAM_INCLUDE_FILE   "wifi_nvram_43439.h"
#define CYW43_IOCTL_TIMEOUT_US          (1000000)
#define CYW43_SLEEP_MAX                 (10)
#define CYW43_NETUTILS                  (1)
#define CYW43_USE_OTP_MAC               (1)
#define CYW43_PRINTF(...)               mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)

#define CYW43_EPERM                     MP_EPERM // Operation not permitted
#define CYW43_EIO                       MP_EIO // I/O error
#define CYW43_EINVAL                    MP_EINVAL // Invalid argument
#define CYW43_ETIMEDOUT                 MP_ETIMEDOUT // Connection timed out

#define CYW43_THREAD_ENTER              MICROPY_PY_LWIP_ENTER
#define CYW43_THREAD_EXIT               MICROPY_PY_LWIP_EXIT
#define CYW43_THREAD_LOCK_CHECK

#define CYW43_HOST_NAME                 mod_network_hostname_data

#if CYW43_PIN_WL_DYNAMIC

// Dynamic pins can be changed at runtime before initialising the CYW43

typedef enum cyw43_pin_index_t {
    CYW43_PIN_INDEX_WL_REG_ON,
    CYW43_PIN_INDEX_WL_DATA_OUT,
    CYW43_PIN_INDEX_WL_DATA_IN,
    CYW43_PIN_INDEX_WL_HOST_WAKE,
    CYW43_PIN_INDEX_WL_CLOCK,
    CYW43_PIN_INDEX_WL_CS,
    CYW43_PIN_INDEX_WL_COUNT // last
} cyw43_pin_index_t;

// Function to retrieve a cyw43 dynamic pin
uint cyw43_get_pin_wl(cyw43_pin_index_t pin_id);

#define CYW43_PIN_WL_REG_ON cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_REG_ON)
#define CYW43_PIN_WL_DATA_OUT cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_DATA_OUT)
#define CYW43_PIN_WL_DATA_IN cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_DATA_IN)
#define CYW43_PIN_WL_HOST_WAKE cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_HOST_WAKE)
#define CYW43_PIN_WL_CLOCK cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_CLOCK)
#define CYW43_PIN_WL_CS cyw43_get_pin_wl(CYW43_PIN_INDEX_WL_CS)

#else

#define CYW43_PIN_WL_REG_ON             CYW43_DEFAULT_PIN_WL_REG_ON
#define CYW43_PIN_WL_DATA_OUT           CYW43_DEFAULT_PIN_WL_DATA_OUT
#define CYW43_PIN_WL_DATA_IN            CYW43_DEFAULT_PIN_WL_DATA_IN
#define CYW43_PIN_WL_HOST_WAKE          CYW43_DEFAULT_PIN_WL_HOST_WAKE
#define CYW43_PIN_WL_CLOCK              CYW43_DEFAULT_PIN_WL_CLOCK
#define CYW43_PIN_WL_CS                 CYW43_DEFAULT_PIN_WL_CS

#endif

#define CYW43_SDPCM_SEND_COMMON_WAIT \
    if (get_core_num() == 0) { \
        cyw43_yield(); \
    } \

#define CYW43_DO_IOCTL_WAIT \
    if (get_core_num() == 0) { \
        cyw43_yield(); \
    } \

#define CYW43_ARRAY_SIZE(a)             MP_ARRAY_SIZE(a)

#define CYW43_HAL_PIN_MODE_INPUT        MP_HAL_PIN_MODE_INPUT
#define CYW43_HAL_PIN_MODE_OUTPUT       MP_HAL_PIN_MODE_OUTPUT
#define CYW43_HAL_PIN_PULL_NONE         MP_HAL_PIN_PULL_NONE
#define CYW43_HAL_PIN_PULL_UP           MP_HAL_PIN_PULL_UP
#define CYW43_HAL_PIN_PULL_DOWN         MP_HAL_PIN_PULL_DOWN

#define CYW43_HAL_MAC_WLAN0             MP_HAL_MAC_WLAN0

// set in SDK board header
#define CYW43_NUM_GPIOS                 CYW43_WL_GPIO_COUNT

#define CYW43_POST_POLL_HOOK            cyw43_post_poll_hook();

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

// Bluetooth requires dynamic memory allocation to load its firmware (the allocation
// call is made from pico-sdk).  This allocation is always done at thread-level, not
// from an IRQ, so is safe to delegate to the MicroPython GC heap.
#ifndef cyw43_malloc
#define cyw43_malloc(nmemb) m_tracked_calloc(nmemb, 1)
#endif
#ifndef cyw43_free
#define cyw43_free m_tracked_free
#endif

void cyw43_post_poll_hook(void);
extern volatile int cyw43_has_pending;

static inline void cyw43_yield(void) {
    if (!cyw43_has_pending) {
        best_effort_wfe_or_timeout(make_timeout_time_ms(1));
    }
}

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
        mp_event_handle_nowait();
    }
}

#define CYW43_EVENT_POLL_HOOK mp_event_handle_nowait()

#endif // MICROPY_INCLUDED_RP2_CYW43_CONFIGPORT_H
