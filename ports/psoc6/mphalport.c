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


// micropython includes
#include "mpconfigport.h"
#include "mphalport.h"

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

// MTB includes
#include "cyhal.h"
#include "cy_retarget_io.h"


// port-specific includes
#include "machine_pin_phy.h"

extern cyhal_rtc_t psoc6_rtc;
extern cyhal_timer_t psoc6_timer;

void mp_hal_delay_ms(mp_uint_t ms) {
    #if defined(CY_RTOS_AWARE) || defined(COMPONENT_RTOS_AWARE)
    // Check
    // file: mtb-hal-cat1/src/cyhal_system.c
    // function: cyhal_system_delay_ms()
    // lines 65-68
    // for an explanation.

    // An increment of 1 ms is added to the delay. In principle
    // that should be corrected by some internal behaviour or RTOS
    // but it does not seem to work like this, as we are always
    // getting one more second in our Test 1 of tests/psoc/time.py.

    // TODO: Find if there is a more elegant way to avoid the RTOS
    // configuration to propagate to this level.
    ms -= 1;
    #endif
    cyhal_system_delay_ms(ms);
}


void mp_hal_delay_us(mp_uint_t us) {
    cyhal_system_delay_us(us);
}


// Issues may arise if time is incremented only each second.
// Would require proper ns count from epoch of clock the source (see also "extmod/vfs_lfs.c", function "lfs_get_mtime" and "mphalport.c", function "mp_hal_time_ns")
uint64_t mp_hal_time_ns(void) {
    struct tm current_date_time = {0};
    cy_rslt_t result = cyhal_rtc_read(&psoc6_rtc, &current_date_time);

    if (CY_RSLT_SUCCESS != result) {
        mp_raise_ValueError(MP_ERROR_TEXT("cyhal_rtc_read failed !"));
    }

    uint64_t s = timeutils_seconds_since_epoch(current_date_time.tm_year, current_date_time.tm_mon, current_date_time.tm_mday,
        current_date_time.tm_hour, current_date_time.tm_min, current_date_time.tm_sec);

    // add ticks to make sure time is strictly monotonic
    return s * 1000000000ULL + cyhal_timer_read(&psoc6_timer) * 1000ULL;
}


mp_uint_t mp_hal_ticks_ms(void) {
    return cyhal_timer_read(&psoc6_timer) / 1000;
}


mp_uint_t mp_hal_ticks_us(void) {
    return cyhal_timer_read(&psoc6_timer);
}


mp_uint_t mp_hal_ticks_cpu(void) {
    return cyhal_timer_read(&psoc6_timer);
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
        result = cyhal_uart_getc(&cy_retarget_io_uart_obj, &c, 1);
        if (result == CY_RSLT_SUCCESS) {
            return c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_pin_od_low(mp_hal_pin_obj_t pin) {
    cyhal_gpio_write(pin, false);
}


void mp_hal_pin_od_high(mp_hal_pin_obj_t pin) {
    cyhal_gpio_write(pin, true);
}

int mp_hal_pin_read(mp_hal_pin_obj_t pin) {
    return cyhal_gpio_read(pin);
}


void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin) {
    cyhal_gpio_configure(pin, CYHAL_GPIO_DIR_INPUT,  CYHAL_GPIO_DRIVE_OPENDRAINDRIVESLOW);
}


uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin) {
    return pin;
}

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity) {
    if (polarity == 1) {
        cyhal_gpio_write(pin, true);
    } else {
        cyhal_gpio_write(pin, false);
    }
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin) {
    cyhal_gpio_configure(pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_NONE);
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin) {
    cyhal_gpio_configure(pin, CYHAL_GPIO_DIR_INPUT,  CYHAL_GPIO_DRIVE_NONE);
}


mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    return pin_addr_by_name(obj);
}
