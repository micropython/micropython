/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
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

void mp_hal_set_interrupt_char(int c) {
    // TODO: Implement this function
}

extern uint32_t get_drive_mode(uint8_t mode, uint8_t pull);

void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull) {
    uint32_t drive_mode = get_drive_mode(mode, pull);
    Cy_GPIO_Pin_FastInit(Cy_GPIO_PortToAddr(pin->port), pin->pin, drive_mode, 0, HSIOM_SEL_GPIO);
}

extern uint8_t pin_get_mode(const machine_pin_obj_t *self);

uint32_t mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    uint8_t mode = pin_get_mode(pin);
    if (mode == GPIO_MODE_OUT ||
        mode == GPIO_MODE_OPEN_DRAIN) {
        return Cy_GPIO_ReadOut(Cy_GPIO_PortToAddr(pin->port), pin->pin);
    }
    return Cy_GPIO_Read(Cy_GPIO_PortToAddr(pin->port), pin->pin);
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity) {
    Cy_GPIO_Write(Cy_GPIO_PortToAddr(pin->port), pin->pin, polarity);
}
