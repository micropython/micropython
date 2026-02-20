/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
 * Copyright (c) 2026 Damien P. George
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
#include "shared/timeutils/timeutils.h"
#include "uart_stdio.h"
#include "cycfg_peripherals.h"

static uint8_t stdin_ringbuf_array[260];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

extern mtb_hal_timer_t psoc_edge_timer;

void mp_hal_delay_ms(mp_uint_t ms) {
    mtb_hal_system_delay_ms(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    mtb_hal_system_delay_us(us);
}

uint64_t mp_hal_time_ns(void) {
    // TODO: This is not fully functional until rtc machine module is implemented.
    cy_stc_rtc_config_t current_date_time = {0};
    Cy_RTC_GetDateAndTime(&current_date_time);

    uint64_t s = timeutils_seconds_since_epoch(current_date_time.year, current_date_time.month, current_date_time.date,
        current_date_time.hour, current_date_time.min, current_date_time.sec);

    // add ticks to make sure time is strictly monotonic
    return s * 1000000000ULL + mtb_hal_timer_read(&psoc_edge_timer) * 1000ULL;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return mtb_hal_timer_read(&psoc_edge_timer) / 1000;
}

mp_uint_t mp_hal_ticks_us(void) {
    return mtb_hal_timer_read(&psoc_edge_timer);
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return mtb_hal_timer_read(&psoc_edge_timer);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    // printf("mp_hal_stdio_poll\n");
    mp_raise_NotImplementedError(MP_ERROR_TEXT("mp_hal_stdio_poll not implemented !"));
    uintptr_t ret = 0;
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }

        c = uart_stdio_read_char();
        if (c != -1) {
            return c;
        }

        mp_event_wait_indefinite();
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    uart_stdio_write_strn(str, len);
    return len;
}

void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    // TODO: Implement this function
}


void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    // TODO: Implement this function
}

int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return 0; // TODO: Implement this function
}

void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    // TODO: Implement this function
}

uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity) {
    // TODO: Implement this function
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    // TODO: Implement this function
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    // TODO: Implement this function
}
