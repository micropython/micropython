/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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
#ifndef MICROPY_INCLUDED_ALIF_CYW43_CONFIGPORT_H
#define MICROPY_INCLUDED_ALIF_CYW43_CONFIGPORT_H

// The board-level config will be included here, so it can set some CYW43 values.
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/modnetwork.h"
#include "extmod/mpbthci.h"
#include "pendsv.h"

#ifndef static_assert
#define static_assert(expr, msg)        typedef int static_assert_##__LINE__[(expr) ? 1 : -1]
#endif

#define CYW43_USE_SPI                   (1)
#define CYW43_ENABLE_BLUETOOTH_OVER_UART (1)
#define CYW43_LWIP                      (1)
#define CYW43_USE_STATS                 (0)
#define CYW43_PRINTF(...)               mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)

#if 0
#define CYW43_VERBOSE_DEBUG             (1)
#define CYW43_VDEBUG(...)               mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#define CYW43_DEBUG(...)                mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#define CYW43_INFO(...)                 mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#define CYW43_WARN(...)                 mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#endif

#define CYW43_IOCTL_TIMEOUT_US          (1000000)
#define CYW43_SLEEP_MAX                 (50)
#define CYW43_NETUTILS                  (1)
#define CYW43_CLEAR_SDIO_INT            (1)

#define CYW43_EPERM                     MP_EPERM // Operation not permitted
#define CYW43_EIO                       MP_EIO // I/O error
#define CYW43_EINVAL                    MP_EINVAL // Invalid argument
#define CYW43_ETIMEDOUT                 MP_ETIMEDOUT // Connection timed out

#define CYW43_THREAD_ENTER              MICROPY_PY_LWIP_ENTER
#define CYW43_THREAD_EXIT               MICROPY_PY_LWIP_EXIT
#define CYW43_THREAD_LOCK_CHECK

#define CYW43_HOST_NAME                 mod_network_hostname_data

#define CYW43_SDPCM_SEND_COMMON_WAIT    __WFE()
#define CYW43_DO_IOCTL_WAIT             // __WFE()
#define CYW43_EVENT_POLL_HOOK           mp_event_handle_nowait()

#define CYW43_ARRAY_SIZE(a)             MP_ARRAY_SIZE(a)

#define CYW43_HAL_PIN_MODE_INPUT        MP_HAL_PIN_MODE_INPUT
#define CYW43_HAL_PIN_MODE_OUTPUT       MP_HAL_PIN_MODE_OUTPUT
#define CYW43_HAL_PIN_PULL_NONE         0

#define CYW43_HAL_MAC_WLAN0             MP_HAL_MAC_WLAN0
#define CYW43_HAL_MAC_BDADDR            MP_HAL_MAC_BDADDR

#define cyw43_hal_ticks_us              mp_hal_ticks_us
#define cyw43_hal_ticks_ms              mp_hal_ticks_ms

#define cyw43_hal_pin_obj_t             mp_hal_pin_obj_t
#define cyw43_hal_pin_read              mp_hal_pin_read
#define cyw43_hal_pin_low               mp_hal_pin_low
#define cyw43_hal_pin_high              mp_hal_pin_high

#define cyw43_hal_uart_set_baudrate     mp_bluetooth_hci_uart_set_baudrate
#define cyw43_hal_uart_write            mp_bluetooth_hci_uart_write
#define cyw43_hal_uart_readchar         mp_bluetooth_hci_uart_readchar

#define cyw43_hal_get_mac               mp_hal_get_mac
#define cyw43_hal_get_mac_ascii         mp_hal_get_mac_ascii
#define cyw43_hal_generate_laa_mac      mp_hal_generate_laa_mac

#define CYW43_PIN_WL_REG_ON             pin_WL_REG_ON
#define CYW43_PIN_WL_IRQ                pin_WL_IRQ

#define CYW43_PIN_BT_REG_ON             pin_BT_REG_ON
#define CYW43_PIN_BT_HOST_WAKE          pin_BT_HOST_WAKE
#define CYW43_PIN_BT_DEV_WAKE           pin_BT_DEV_WAKE
#define CYW43_PIN_BT_CTS                pin_BT_UART_CTS

#define cyw43_schedule_internal_poll_dispatch(func) pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, func)

void cyw43_post_poll_hook(void);

static inline void cyw43_delay_us(uint32_t us) {
    uint32_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

static inline void cyw43_delay_ms(uint32_t ms) {
    mp_hal_delay_ms(ms);
}

static inline void cyw43_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    if (mode == MP_HAL_PIN_MODE_INPUT) {
        mp_hal_pin_input(pin);
    } else {
        mp_hal_pin_output(pin);
    }
}

static inline void cyw43_hal_pin_config_irq_falling(mp_hal_pin_obj_t pin, bool enable) {
    mp_hal_pin_config_irq_falling(pin, enable);
}

#define cyw43_bluetooth_controller_init mp_bluetooth_hci_controller_init
#define cyw43_bluetooth_controller_deinit mp_bluetooth_hci_controller_deinit
#define cyw43_bluetooth_controller_sleep_maybe mp_bluetooth_hci_controller_sleep_maybe
#define cyw43_bluetooth_controller_woken mp_bluetooth_hci_controller_woken
#define cyw43_bluetooth_controller_wakeup mp_bluetooth_hci_controller_wakeup

#endif // MICROPY_INCLUDED_ALIF_CYW43_CONFIGPORT_H
