/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
 * Copyright (c) 2024 OpenMV LLC.
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
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "extmod/misc.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/softtimer.h"
#include "shared/timeutils/timeutils.h"
#include "shared/tinyusb/mp_usbd.h"
#include "shared/tinyusb/mp_usbd_cdc.h"
#include "tusb.h"
#include "mpuart.h"
#include "pendsv.h"
#include "se_services.h"
#include "system_tick.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 512
#endif

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_HW_USB_CDC
    ret |= mp_usbd_cdc_poll_interfaces(poll_flags);
    #endif
    #if MICROPY_HW_ENABLE_UART_REPL
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR;
    }
    #endif
    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_os_dupterm_poll(poll_flags);
    #endif
    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if MICROPY_HW_USB_CDC
        mp_usbd_cdc_poll_interfaces(0);
        #endif
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #if MICROPY_PY_OS_DUPTERM
        int dupterm_c = mp_os_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        #endif
        mp_event_wait_indefinite();
    }
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    #if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC || MICROPY_PY_OS_DUPTERM
    mp_uint_t ret = len;
    bool did_write = false;
    #endif

    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_write_strn_repl(str, len);
    did_write = true;
    #endif

    #if MICROPY_HW_USB_CDC
    mp_uint_t cdc_res = mp_usbd_cdc_tx_strn(str, len);
    if (cdc_res > 0) {
        did_write = true;
        ret = MIN(cdc_res, ret);
    }
    #endif

    #if MICROPY_PY_OS_DUPTERM
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        did_write = true;
        ret = MIN((mp_uint_t)dupterm_res, ret);
    }
    #endif

    return did_write ? ret : 0;
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return system_tick_get_u32();
}

mp_uint_t mp_hal_ticks_us(void) {
    // Convert system tick to microsecond counter.
    #if MICROPY_HW_SYSTEM_TICK_USE_SYSTICK
    return system_tick_get_u64();
    #elif MICROPY_HW_SYSTEM_TICK_USE_LPTIMER
    return system_tick_get_u64() * 1000000 / system_tick_source_hz;
    #else
    return system_tick_get_u64() / system_core_clock_mhz;
    #endif
}

mp_uint_t mp_hal_ticks_ms(void) {
    // Convert system tick to millisecond counter.
    #if MICROPY_HW_SYSTEM_TICK_USE_SYSTICK
    return system_tick_get_u64() / 1000ULL;
    #elif MICROPY_HW_SYSTEM_TICK_USE_LPTIMER
    return system_tick_get_u64() * 1000ULL / system_tick_source_hz;
    #else
    return system_tick_get_u64() / (SystemCoreClock / 1000);
    #endif
}

void mp_hal_delay_us(mp_uint_t us) {
    #if MICROPY_HW_SYSTEM_TICK_USE_SYSTICK
    uint64_t ticks_delay = (uint64_t)us;
    #elif MICROPY_HW_SYSTEM_TICK_USE_LPTIMER
    uint64_t ticks_delay = (uint64_t)us * system_tick_source_hz / 1000000;
    #else
    uint64_t ticks_delay = (uint64_t)us * system_core_clock_mhz;
    #endif
    uint64_t start = system_tick_get_u64();
    while (system_tick_get_u64() - start < ticks_delay) {
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint32_t t0 = mp_hal_ticks_ms();
    mp_event_handle_nowait();
    for (;;) {
        uint32_t t1 = mp_hal_ticks_ms();
        if (t1 - t0 >= ms) {
            break;
        }
        mp_event_wait_ms(ms - (t1 - t0));
    }
}

uint64_t mp_hal_time_ns(void) {
    uint32_t microseconds;
    uint32_t s = mp_hal_time_get(&microseconds);
    return (uint64_t)s * 1000000000ULL + (uint64_t)microseconds * 1000ULL;
}

void mp_hal_pin_config(const machine_pin_obj_t *pin, uint32_t mode,
    uint32_t pull, uint32_t speed, uint32_t drive, uint32_t alt, bool ren) {
    uint8_t alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    uint8_t pad_ctrl = drive | speed | (ren ? PADCTRL_READ_ENABLE : 0);

    // Configure pull-up or pull-down.
    if (pull & MP_HAL_PIN_PULL_UP) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_UP;
    }

    if (pull & MP_HAL_PIN_PULL_DOWN) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_DOWN;
    }

    // Configure open-drain mode.
    if (mode == MP_HAL_PIN_MODE_OPEN_DRAIN) {
        pad_ctrl |= PADCTRL_DRIVER_OPEN_DRAIN;
    }

    // For ALT mode, find alternate function.
    if (mode == MP_HAL_PIN_MODE_ALT) {
        for (mp_uint_t i = 0; i < MP_ARRAY_SIZE(pin->alt); i++) {
            if (alt == pin->alt[i]) {
                alt_func = i;
                break;
            }
        }
        if (alt_func == PINMUX_ALTERNATE_FUNCTION_0) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin af: %d"), alt);
        }
    }

    // Set pad config.
    pinconf_set(pin->port, pin->pin, alt_func, pad_ctrl);

    // For INPUT/OUTPUT/OD modes, set the GPIO direction.
    switch (mode) {
        case MP_HAL_PIN_MODE_INPUT:
            gpio_set_direction_input(pin->gpio, pin->pin);
            break;
        case MP_HAL_PIN_MODE_OUTPUT:
        case MP_HAL_PIN_MODE_OPEN_DRAIN:
            gpio_set_direction_output(pin->gpio, pin->pin);
            break;
        default:
            break;
    }
}


void system_tick_schedule_callback(void) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);
}

#if !defined(MICROPY_SOFT_TIMER_TICKS_MS)

uint32_t soft_timer_get_ms(void) {
    return mp_hal_ticks_ms();
}

void soft_timer_schedule_at_ms(uint32_t ticks_ms) {
    int32_t ms = soft_timer_ticks_diff(ticks_ms, mp_hal_ticks_ms());
    ms = MAX(0, ms);
    ms = MIN(ms, 4000000); // ensure ms * 1000 doesn't overflow
    system_tick_schedule_after_us(ms * 1000);
}

#endif

/*******************************************************************************/
// MAC address

// Generate a random locally administered MAC address (LAA)
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]) {
    uint8_t id[8];
    se_services_get_unique_id(id);
    buf[0] = 0x02; // LAA range
    buf[1] = id[4];
    buf[2] = id[3];
    buf[3] = id[2];
    buf[4] = id[1];
    buf[5] = (id[0] << 2) | idx;
}

// A board can override this if needed
MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    mp_hal_generate_laa_mac(idx, buf);
}

void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    mp_hal_get_mac(idx, mac);
    for (; chr_len; ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
}
