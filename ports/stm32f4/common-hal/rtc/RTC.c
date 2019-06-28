/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "supervisor/shared/translate.h"

#include "nrfx_rtc.h"
#include "nrf_clock.h"

// We clock the RTC very slowly (8Hz) so that it won't overflow often.
// But the counter is only 24 bits, so overflow is about every 24 days ...
// For testing, set this to 32768 and it'll overflow every few minutes

#define RTC_CLOCK_HZ (8)

volatile static uint32_t rtc_offset = 0;

const nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(2);

const nrfx_rtc_config_t rtc_config = {
	.prescaler = RTC_FREQ_TO_PRESCALER(RTC_CLOCK_HZ),
	.reliable = 0,
	.tick_latency = 0,
	.interrupt_priority = 6
};

void rtc_handler(nrfx_rtc_int_type_t int_type) {
    if (int_type == NRFX_RTC_INT_OVERFLOW) {
        rtc_offset += (1L<<24) / RTC_CLOCK_HZ;
    }
}

void rtc_init(void) {
    if (!nrf_clock_lf_is_running()) {
        nrf_clock_task_trigger(NRF_CLOCK_TASK_LFCLKSTART);
    }
    nrfx_rtc_counter_clear(&rtc_instance);
    nrfx_rtc_init(&rtc_instance, &rtc_config, rtc_handler);
    nrfx_rtc_enable(&rtc_instance);
    nrfx_rtc_overflow_enable(&rtc_instance, 1);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    uint32_t t = rtc_offset + (nrfx_rtc_counter_get(&rtc_instance) / RTC_CLOCK_HZ );
    timeutils_seconds_since_2000_to_struct_time(t, tm);
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    rtc_offset = timeutils_seconds_since_2000(
        tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec
    );
    nrfx_rtc_counter_clear(&rtc_instance);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError(translate("RTC calibration is not supported on this board"));
}

