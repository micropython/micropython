/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Damien P. George
 * Copyright (c) 2023-2024 Infineon Technologies AG
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
#include "stdio.h"

// micropython includes
#include "extmod/modtime.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

// MTB includes
#include "mtb_hal.h"
#include "cybsp.h"

mtb_hal_timer_t psoc_edge_timer;

#define TIMER_HW CYBSP_GENERAL_PURPOSE_TIMER_HW    // TCPWM0
#define TIMER_NUM CYBSP_GENERAL_PURPOSE_TIMER_NUM  // 2
#define TIMER_INPUT_DISABLED 0x7U
#define RTC_ACCESS_RETRY_COUNT      (500U)
#define RTC_RETRY_DELAY_MS          (5U)

static const mtb_hal_peri_div_t clock_ref =
{
    .clk_dst = (en_clk_dst_t)PCLK_TCPWM0_CLOCK_COUNTER_EN2,
    .div_type = CY_SYSCLK_DIV_16_BIT,
    .div_num = 2,
};
static const mtb_hal_clock_t hal_clock =
{
    .clock_ref = &clock_ref,
    .interface = &mtb_hal_clock_peri_interface,
};

static const cy_stc_tcpwm_counter_config_t timer_config =
{
    .period = 15000000,
    .clockPrescaler = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,
    .runMode = CY_TCPWM_COUNTER_CONTINUOUS,
    .countDirection = CY_TCPWM_COUNTER_COUNT_UP,
    .compareOrCapture = CY_TCPWM_COUNTER_MODE_COMPARE,
    .compare0 = 0,
    .compare1 = 16384,
    .enableCompareSwap = false,
    .interruptSources = CY_TCPWM_INT_NONE,
    .captureInputMode = 0x3U,
    .captureInput = CY_TCPWM_INPUT_0,
    .reloadInputMode = 0x3U,
    .reloadInput = CY_TCPWM_INPUT_0,
    .startInputMode = 0x3U,
    .startInput = CY_TCPWM_INPUT_0,
    .stopInputMode = 0x3U,
    .stopInput = CY_TCPWM_INPUT_0,
    .countInputMode = 0x3U,
    .countInput = CY_TCPWM_INPUT_1,
    .capture1InputMode = 0x3U,
    .capture1Input = CY_TCPWM_INPUT_0,
    .compare2 = 16384,
    .compare3 = 16384,
    .enableCompare1Swap = false,
    .trigger0Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
    .trigger1Event = CY_TCPWM_CNT_TRIGGER_ON_DISABLED,
};

static const mtb_hal_timer_configurator_t timer_hal_config =
{
    .tcpwm_base = TIMER_HW,
    .clock = &hal_clock,
    .tcpwm_cntnum = TIMER_NUM,
};

void time_init(void) {
    cy_rslt_t rslt;

    rslt = Cy_TCPWM_Counter_Init(TIMER_HW, TIMER_NUM, &timer_config);

    if (rslt == CY_RSLT_SUCCESS) {
        rslt = mtb_hal_timer_setup(&psoc_edge_timer, &timer_hal_config, NULL);
    }

    if (rslt == CY_RSLT_SUCCESS) {
        rslt = mtb_hal_clock_set_peri_clock_freq(&clock_ref, 1000000, 1000);
    }

    if (rslt == CY_RSLT_SUCCESS) {
        rslt = mtb_hal_timer_start(&psoc_edge_timer);
    }

    // TODO: Code below needs to be removed when rtc machine module is implemented.

    cy_en_rtc_status_t rtc_status;
    uint32_t rtc_access_retry = RTC_ACCESS_RETRY_COUNT;

    do
    {
        rtc_status = Cy_RTC_Init(&CYBSP_RTC_config);
        rtc_access_retry--;
        Cy_SysLib_Delay(RTC_RETRY_DELAY_MS);
    } while ((rtc_status != CY_RTC_SUCCESS) && (rtc_access_retry != 0));

    Cy_RTC_SetDateAndTimeDirect(10, 45, 15, 15, 10, 25);

}

void time_deinit(void) {
    mtb_hal_timer_stop(&psoc_edge_timer);
}

static void mp_time_localtime_get(timeutils_struct_time_t *time) {
    // TODO: This is not fully functional until rtc machine module is implemented.
    cy_stc_rtc_config_t current_date_time = {0};
    Cy_RTC_GetDateAndTime(&current_date_time);

    time->tm_year = current_date_time.year;
    time->tm_mon = current_date_time.month;
    time->tm_mday = current_date_time.date;
    time->tm_hour = current_date_time.hour;
    time->tm_min = current_date_time.min;
    time->tm_sec = current_date_time.sec;
    time->tm_wday = current_date_time.dayOfWeek - 1u;
    time->tm_yday = timeutils_year_day(current_date_time.year, current_date_time.month, current_date_time.date);
}

// Return the number of seconds since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    // TODO: This is not fully functional until rtc machine module is implemented.
    cy_stc_rtc_config_t current_date_time = {0};
    Cy_RTC_GetDateAndTime(&current_date_time);

    return timeutils_obj_from_timestamp(timeutils_seconds_since_epoch(current_date_time.year, current_date_time.month, current_date_time.date,
        current_date_time.hour, current_date_time.min, current_date_time.sec));
}
