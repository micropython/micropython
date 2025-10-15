/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include "stdbool.h"   // because of missing include in shared/timeutils/timeutils.h
#include "stdio.h"

// MTB includes
#include "retarget_io_init.h"

// micropython includes
#include "mpconfigport.h"
#include "mphalport.h"

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

extern mtb_hal_uart_t DEBUG_UART_hal_obj;
extern mtb_hal_timer_t psoc_edge_timer;

void mp_hal_delay_ms(mp_uint_t ms) {
    mtb_hal_system_delay_ms(ms);
}

void mp_hal_delay_us(mp_uint_t us) {
    mtb_hal_system_delay_us(us);
}

uint64_t mp_hal_time_ns(void) {
    return 0; // TODO: Implement this function properly
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
    printf("mp_hal_stdio_poll\n");
    mp_raise_NotImplementedError(MP_ERROR_TEXT("mp_hal_stdio_poll not implemented !"));
    uintptr_t ret = 0;
    return ret;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    int r = write(STDOUT_FILENO, str, len);
    (void)r;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        uint8_t c = 0;
        cy_rslt_t result;
        result = mtb_hal_uart_get(&DEBUG_UART_hal_obj, &c, 1);
        if (result == CY_RSLT_SUCCESS) {
            return c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
}


void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
}

int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return 0; // TODO: Implement this function properly
}

void mp_hal_set_interrupt_char(int c) {

}

void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
}

uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity) {
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
}
