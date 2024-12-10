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
#include "shared/tinyusb/mp_usbd_cdc.h"
#include "pendsv.h"
#include "tusb.h"
#include "uart.h"
#include "hardware/irq.h"
#include "pico/unique_id.h"
#include "pico/aon_timer.h"

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

#if PICO_RP2040
// This needs to be added to the result of time_us_64() to get the number of
// microseconds since the Epoch.
static uint64_t time_us_64_offset_from_epoch;
#endif

#if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 512
#endif

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

#endif

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
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    bool did_write = false;
    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_write_strn(str, len);
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

#if PICO_RISCV
__attribute__((naked)) mp_uint_t mp_hal_ticks_cpu(void) {
    __asm volatile (
        "li a0, 4\n" // mask value to uninhibit mcycle counter
        "csrw mcountinhibit, a0\n" // uninhibit mcycle counter
        "csrr a0, mcycle\n" // get mcycle counter
        "ret\n"
        );
}
#endif

void mp_hal_delay_us(mp_uint_t us) {
    // Avoid calling sleep_us() and invoking the alarm pool by splitting long
    // sleeps into an optional longer sleep and a shorter busy-wait
    uint64_t end = time_us_64() + us;
    if (us > 1000) {
        mp_hal_delay_ms(us / 1000);
    }
    while (time_us_64() < end) {
        // Tight loop busy-wait for accurate timing
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    mp_uint_t elapsed = 0;
    do {
        mp_event_wait_ms(ms - elapsed);
        elapsed = mp_hal_ticks_ms() - start;
    } while (elapsed < ms);
}

void mp_hal_time_ns_set_from_rtc(void) {
    #if PICO_RP2040
    // Outstanding RTC register writes need at least two RTC clock cycles to
    // update. (See RP2040 datasheet section 4.8.4 "Reference clock").
    mp_hal_delay_us(44);

    // Sample RTC and time_us_64() as close together as possible, so the offset
    // calculated for the latter can be as accurate as possible.
    struct timespec ts;
    aon_timer_get_time(&ts);
    uint64_t us = time_us_64();

    // Calculate the difference between the RTC Epoch and time_us_64().
    time_us_64_offset_from_epoch = ((uint64_t)ts.tv_sec * 1000000ULL) + ((uint64_t)ts.tv_nsec / 1000ULL) - us;
    #endif
}

uint64_t mp_hal_time_ns(void) {
    #if PICO_RP2040
    // The RTC probably has limited resolution, so instead use time_us_64() to get a more
    // precise measure of Epoch time.  Both these "clocks" are clocked from the same
    // source so they remain synchronised, and only differ by a fixed offset (calculated
    // in mp_hal_time_ns_set_from_rtc).
    return (time_us_64_offset_from_epoch + time_us_64()) * 1000ULL;
    #else
    // aon timer has ms resolution
    struct timespec ts;
    aon_timer_get_time(&ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
    #endif
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

uint32_t soft_timer_get_ms(void) {
    return mp_hal_ticks_ms();
}

void soft_timer_schedule_at_ms(uint32_t ticks_ms) {
    int32_t ms = soft_timer_ticks_diff(ticks_ms, mp_hal_ticks_ms());
    ms = MAX(0, ms);
    if (hardware_alarm_set_target(MICROPY_HW_SOFT_TIMER_ALARM_NUM, delayed_by_ms(get_absolute_time(), ms))) {
        // "missed" hardware alarm target
        hardware_alarm_force_irq(MICROPY_HW_SOFT_TIMER_ALARM_NUM);
    }
}

static void soft_timer_hardware_callback(unsigned int alarm_num) {
    // The timer alarm ISR needs to call here and trigger PendSV dispatch via
    // a second ISR, as PendSV may be currently suspended by the other CPU.
    pendsv_schedule_dispatch(PENDSV_DISPATCH_SOFT_TIMER, soft_timer_handler);

    // This ISR only runs on core0, but if core1 is running Python code then it
    // may be blocked in WFE so wake it up as well. Unfortunately this also sets
    // the event flag on core0, so a subsequent WFE on this core will not suspend
    #if MICROPY_PY_THREAD
    if (core1_entry != NULL) {
        __sev();
    }
    #endif
}

void soft_timer_init(void) {
    hardware_alarm_claim(MICROPY_HW_SOFT_TIMER_ALARM_NUM);
    hardware_alarm_set_callback(MICROPY_HW_SOFT_TIMER_ALARM_NUM, soft_timer_hardware_callback);
}

void mp_wfe_or_timeout(uint32_t timeout_ms) {
    best_effort_wfe_or_timeout(delayed_by_ms(get_absolute_time(), timeout_ms));
}

int mp_hal_is_pin_reserved(int n) {
    #if MICROPY_PY_NETWORK_CYW43
    return n == CYW43_PIN_WL_HOST_WAKE;
    #else
    return false;
    #endif
}
