/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/softtimer.h"
#include "shared/timeutils/timeutils.h"
#include "shared/tinyusb/mp_usbd.h"
#include "pendsv.h"
#include "tusb.h"
#include "uart.h"
#include "hardware/rtc.h"
#include "pico/unique_id.h"

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

// This needs to be added to the result of time_us_64() to get the number of
// microseconds since the Epoch.
static uint64_t time_us_64_offset_from_epoch;

static alarm_id_t soft_timer_alarm_id = 0;

#if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 512
#endif

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

#endif

#if MICROPY_HW_USB_CDC

uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll

void poll_cdc_interfaces(void) {
    if (!cdc_itf_pending) {
        // Explicitly run the USB stack as the scheduler may be locked (eg we are in
        // an interrupt handler) while there is data pending.
        mp_usbd_task();
    }

    // any CDC interfaces left to poll?
    if (cdc_itf_pending && ringbuf_free(&stdin_ringbuf)) {
        for (uint8_t itf = 0; itf < 8; ++itf) {
            if (cdc_itf_pending & (1 << itf)) {
                tud_cdc_rx_cb(itf);
                if (!cdc_itf_pending) {
                    break;
                }
            }
        }
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            if (data_char == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
}

#endif

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_HW_USB_CDC
    poll_cdc_interfaces();
    #endif
    #if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        #if MICROPY_HW_ENABLE_UART_REPL
        ret |= MP_STREAM_POLL_WR;
        #else
        if (tud_cdc_connected() && tud_cdc_write_available() > 0) {
            ret |= MP_STREAM_POLL_WR;
        }
        #endif
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
        poll_cdc_interfaces();
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
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    bool did_write = false;
    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_write_strn(str, len);
    did_write = true;
    #endif

    #if MICROPY_HW_USB_CDC
    if (tud_cdc_connected()) {
        size_t i = 0;
        while (i < len) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            int timeout = 0;
            // Wait with a max of USC_CDC_TIMEOUT ms
            while (n > tud_cdc_write_available() && timeout++ < MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                mp_event_wait_ms(1);

                // Explicitly run the USB stack as the scheduler may be locked (eg we
                // are in an interrupt handler), while there is data pending.
                mp_usbd_task();
            }
            if (timeout >= MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                ret = i;
                break;
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }
        ret = MIN(i, ret);
        did_write = true;
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

void mp_hal_delay_ms(mp_uint_t ms) {
    absolute_time_t t = make_timeout_time_ms(ms);
    do {
        mp_event_handle_nowait();
    } while (!best_effort_wfe_or_timeout(t));
}

void mp_hal_time_ns_set_from_rtc(void) {
    // Delay at least one RTC clock cycle so it's registers have updated with the most
    // recent time settings.
    sleep_us(23);

    // Sample RTC and time_us_64() as close together as possible, so the offset
    // calculated for the latter can be as accurate as possible.
    datetime_t t;
    rtc_get_datetime(&t);
    uint64_t us = time_us_64();

    // Calculate the difference between the RTC Epoch seconds and time_us_64().
    uint64_t s = timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.min, t.sec);
    time_us_64_offset_from_epoch = (uint64_t)s * 1000000ULL - us;
}

uint64_t mp_hal_time_ns(void) {
    // The RTC only has seconds resolution, so instead use time_us_64() to get a more
    // precise measure of Epoch time.  Both these "clocks" are clocked from the same
    // source so they remain synchronised, and only differ by a fixed offset (calculated
    // in mp_hal_time_ns_set_from_rtc).
    return (time_us_64_offset_from_epoch + time_us_64()) * 1000ULL;
}

// Generate a random locally administered MAC address (LAA)
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]) {
    #ifndef NDEBUG
    printf("Warning: No MAC in OTP, generating MAC from board id\n");
    #endif
    pico_unique_board_id_t pid;
    pico_get_unique_board_id(&pid);
    buf[0] = 0x02; // LAA range
    buf[1] = (pid.id[7] << 4) | (pid.id[6] & 0xf);
    buf[2] = (pid.id[5] << 4) | (pid.id[4] & 0xf);
    buf[3] = (pid.id[3] << 4) | (pid.id[2] & 0xf);
    buf[4] = pid.id[1];
    buf[5] = (pid.id[0] << 2) | idx;
}

// A board can override this if needed
MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    #if MICROPY_PY_NETWORK_CYW43
    // The mac should come from cyw43 otp when CYW43_USE_OTP_MAC is defined
    // This is loaded into the state after the driver is initialised
    // cyw43_hal_generate_laa_mac is only called by the driver to generate a mac if otp is not set
    if (idx == MP_HAL_MAC_WLAN0) {
        memcpy(buf, cyw43_state.mac, 6);
        return;
    }
    #endif
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

// Shouldn't be used, needed by cyw43-driver in debug build.
uint32_t storage_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    panic_unsupported();
}

static int64_t soft_timer_callback(alarm_id_t id, void *user_data) {
    soft_timer_alarm_id = 0;
    pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);
    return 0; // don't reschedule this alarm
}

uint32_t soft_timer_get_ms(void) {
    return mp_hal_ticks_ms();
}

void soft_timer_schedule_at_ms(uint32_t ticks_ms) {
    if (soft_timer_alarm_id != 0) {
        cancel_alarm(soft_timer_alarm_id);
    }
    int32_t ms = soft_timer_ticks_diff(ticks_ms, mp_hal_ticks_ms());
    ms = MAX(0, ms);
    soft_timer_alarm_id = add_alarm_in_ms(ms, soft_timer_callback, NULL, true);
}
