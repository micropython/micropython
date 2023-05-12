/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2022 Matthew McGowan for Blues Wireless Inc
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

#include "peripherals/rtc.h"
#include STM32_HAL_H

#include "py/mpconfig.h"
#include "shared/timeutils/timeutils.h"

// Default period for ticks is 1/1024 second
#define TICKS_PER_SECOND 1024
// Based on a 32768 kHz clock
#define SUBTICKS_PER_TICK 32

STATIC RTC_HandleTypeDef hrtc;

#if BOARD_HAS_LOW_SPEED_CRYSTAL
STATIC uint32_t rtc_clock_frequency = LSE_VALUE;
#else
STATIC uint32_t rtc_clock_frequency = LSI_VALUE;
#endif

volatile uint32_t seconds_to_date = 0;
volatile uint32_t cached_date = 0;
volatile uint32_t seconds_to_minute = 0;
volatile uint32_t cached_hours_minutes = 0;

// The RTC starts at 2000-01-01 when it comes up.
// If the RTC is set to a later time, the ticks the RTC returns will be offset by the new time.
// Remember that offset so it can be removed when returning a monotonic tick count.
static int64_t rtc_ticks_offset;
// Normalized to be 0-31 inclusive, so always positive.
static uint8_t rtc_subticks_offset;

volatile bool alarmed_already[2];

bool peripherals_wkup_on = false;

static void (*wkup_callback)(void);
static void (*alarm_callbacks[2])(void);

uint32_t stm32_peripherals_get_rtc_freq(void) {
    return rtc_clock_frequency;
}

void stm32_peripherals_rtc_init(void) {
    rtc_ticks_offset = 0;
    rtc_subticks_offset = 0;

    // RTC oscillator selection is handled in peripherals/<family>/<line>/clocks.c
    __HAL_RCC_RTC_ENABLE();
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    // Divide async as little as possible so that we have rtc_clock_frequency count in subseconds.
    // This ensures our timing > 1 second is correct.
    hrtc.Init.AsynchPrediv = 0x0;
    hrtc.Init.SynchPrediv = rtc_clock_frequency - 1;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;

    HAL_RTC_Init(&hrtc);
    HAL_RTCEx_EnableBypassShadow(&hrtc);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

// This function is called often for timing so we cache the seconds elapsed computation based on the
// register value. The STM HAL always does shifts and conversion if we use it directly.
STATIC uint64_t stm32_peripherals_rtc_raw_ticks(uint8_t *subticks) {
    // Disable IRQs to ensure we read all of the RTC registers as close in time as possible. Read
    // SSR twice to make sure we didn't read across a tick.
    __disable_irq();
    uint32_t first_ssr = (uint32_t)(RTC->SSR);
    uint32_t time = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
    uint32_t date = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);
    uint32_t ssr = (uint32_t)(RTC->SSR);
    if (ssr != first_ssr) {
        first_ssr = ssr;
        time = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);
        date = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);
        ssr = (uint32_t)(RTC->SSR);
    }
    __enable_irq();

    uint32_t subseconds = rtc_clock_frequency - 1 - ssr;

    if (date != cached_date) {
        uint32_t year = (uint8_t)((date & (RTC_DR_YT | RTC_DR_YU)) >> 16U);
        uint8_t month = (uint8_t)((date & (RTC_DR_MT | RTC_DR_MU)) >> 8U);
        uint8_t day = (uint8_t)(date & (RTC_DR_DT | RTC_DR_DU));
        // Add 2000 since the year is only the last two digits.
        year = 2000 + (uint32_t)RTC_Bcd2ToByte(year);
        month = (uint8_t)RTC_Bcd2ToByte(month);
        day = (uint8_t)RTC_Bcd2ToByte(day);
        seconds_to_date = timeutils_seconds_since_2000(year, month, day, 0, 0, 0);
        cached_date = date;
    }
    uint32_t hours_minutes = time & (RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU);
    if (hours_minutes != cached_hours_minutes) {
        uint8_t hours = (uint8_t)((time & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
        uint8_t minutes = (uint8_t)((time & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
        hours = (uint8_t)RTC_Bcd2ToByte(hours);
        minutes = (uint8_t)RTC_Bcd2ToByte(minutes);
        seconds_to_minute = 60 * (60 * hours + minutes);
        cached_hours_minutes = hours_minutes;
    }
    uint8_t seconds = (uint8_t)(time & (RTC_TR_ST | RTC_TR_SU));
    seconds = (uint8_t)RTC_Bcd2ToByte(seconds);
    if (subticks != NULL) {
        *subticks = subseconds % SUBTICKS_PER_TICK;
    }

    uint64_t raw_ticks = ((uint64_t)TICKS_PER_SECOND) * (seconds_to_date + seconds_to_minute + seconds) + subseconds / SUBTICKS_PER_TICK;
    return raw_ticks;
}

// This function returns monotonically increasing ticks by adjusting away the RTC tick offset
// from the last time the date was set.
uint64_t stm32_peripherals_rtc_monotonic_ticks(uint8_t *subticks) {
    uint8_t raw_subticks;
    uint64_t monotonic_ticks = stm32_peripherals_rtc_raw_ticks(&raw_subticks) - rtc_ticks_offset;
    int8_t monotonic_subticks = raw_subticks - rtc_subticks_offset;
    // Difference might be negative. Normalize to 0-31.
    // `while` not really necessary; should only loop 0 or 1 times.
    while (monotonic_subticks < 0) {
        monotonic_ticks--;
        monotonic_subticks += SUBTICKS_PER_TICK;
    }
    *subticks = (uint8_t)monotonic_subticks;
    return monotonic_ticks;
}

#if CIRCUITPY_RTC
void stm32_peripherals_rtc_get_time(timeutils_struct_time_t *tm) {
    RTC_DateTypeDef date = {0};
    RTC_TimeTypeDef time = {0};

    int code;
    if ((code = HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN)) == HAL_OK &&
        (code = HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN)) == HAL_OK) {
        tm->tm_hour = time.Hours;
        tm->tm_min = time.Minutes;
        tm->tm_sec = time.Seconds;
        tm->tm_wday = date.WeekDay - 1;
        tm->tm_mday = date.Date;
        tm->tm_mon = date.Month;
        tm->tm_year = date.Year + 2000;
        tm->tm_yday = -1;
    }
}

void stm32_peripherals_rtc_set_time(timeutils_struct_time_t *tm) {
    RTC_DateTypeDef date = {0};
    RTC_TimeTypeDef time = {0};

    uint8_t current_monotonic_subticks;
    uint64_t current_monotonic_ticks = stm32_peripherals_rtc_monotonic_ticks(&current_monotonic_subticks);

    // SubSeconds will always be set to zero.
    time.Hours = tm->tm_hour;
    time.Minutes = tm->tm_min;
    time.Seconds = tm->tm_sec;
    date.WeekDay = tm->tm_wday + 1;
    date.Date = tm->tm_mday;
    date.Month = tm->tm_mon;
    date.Year = tm->tm_year - 2000;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK ||
        HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
        // todo - throw an exception
    }

    uint8_t raw_subticks;
    rtc_ticks_offset = stm32_peripherals_rtc_raw_ticks(&raw_subticks) - current_monotonic_ticks;
    int8_t rtc_subticks_offset_signed = raw_subticks - current_monotonic_subticks;
    // Difference might be negative. Normalize subticks to 0-31.
    // `while` not really necessary; should only loop 0 or 1 times.
    while (rtc_subticks_offset_signed < 0) {
        rtc_ticks_offset--;
        rtc_subticks_offset_signed += SUBTICKS_PER_TICK;
    }
    rtc_subticks_offset = (uint8_t)rtc_subticks_offset_signed;
}
#endif

void stm32_peripherals_rtc_assign_wkup_callback(void (*callback)(void)) {
    wkup_callback = callback;
}

void stm32_peripherals_rtc_set_wakeup_mode_seconds(uint32_t seconds) {
    // prep stuff from CubeMX
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, (rtc_clock_frequency / 16) * seconds, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
}

void stm32_peripherals_rtc_set_wakeup_mode_tick(void) {
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, (rtc_clock_frequency / 16) / TICKS_PER_SECOND, RTC_WAKEUPCLOCK_RTCCLK_DIV2);
}

void stm32_peripherals_rtc_enable_wakeup_timer(void) {
    peripherals_wkup_on = true;
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 1, 0U);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

void stm32_peripherals_rtc_disable_wakeup_timer(void) {
    peripherals_wkup_on = false;
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
}

void stm32_peripherals_rtc_reset_alarms(void) {
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_B);
}

void stm32_peripherals_rtc_assign_alarm_callback(uint8_t alarm_idx, void (*callback)(void)) {
    alarm_callbacks[alarm_idx] = callback;
}

void stm32_peripherals_rtc_set_alarm(uint8_t alarm_idx, uint32_t ticks) {
    uint64_t raw_ticks = stm32_peripherals_rtc_raw_ticks(NULL) + ticks;

    RTC_AlarmTypeDef alarm;
    if (ticks > TICKS_PER_SECOND) {
        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(raw_ticks / TICKS_PER_SECOND, &tm);
        alarm.AlarmTime.Hours = tm.tm_hour;
        alarm.AlarmTime.Minutes = tm.tm_min;
        alarm.AlarmTime.Seconds = tm.tm_sec;
        alarm.AlarmDateWeekDay = tm.tm_mday;
        // Masking here means that the value is ignored so we set none.
        alarm.AlarmMask = RTC_ALARMMASK_NONE;
    } else {
        // Masking here means that the value is ignored so we set them all. Only the subseconds
        // value matters.
        alarm.AlarmMask = RTC_ALARMMASK_ALL;
    }

    alarm.AlarmTime.SubSeconds = rtc_clock_frequency - 1 -
        ((raw_ticks % TICKS_PER_SECOND) * SUBTICKS_PER_TICK);
    if (alarm.AlarmTime.SubSeconds > rtc_clock_frequency) {
        alarm.AlarmTime.SubSeconds = alarm.AlarmTime.SubSeconds +
            rtc_clock_frequency;
    }
    alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
    // Masking here means that the bits are ignored so we set none of them.
    alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    alarm.Alarm = (alarm_idx == PERIPHERALS_ALARM_A) ? RTC_ALARM_A : RTC_ALARM_B;
    HAL_RTC_SetAlarm_IT(&hrtc, &alarm, RTC_FORMAT_BIN);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    alarmed_already[alarm_idx] = false;
}

bool stm32_peripherals_rtc_alarm_triggered(uint8_t alarm_idx) {
    return alarmed_already[alarm_idx];
}

void RTC_WKUP_IRQHandler(void) {
    if (wkup_callback) {
        wkup_callback();
    }
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();
    hrtc.State = HAL_RTC_STATE_READY;
}

void RTC_Alarm_IRQHandler(void) {
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *_hrtc) {
    if (alarm_callbacks[PERIPHERALS_ALARM_A]) {
        alarm_callbacks[PERIPHERALS_ALARM_A]();
    }
    HAL_RTC_DeactivateAlarm(_hrtc, RTC_ALARM_A);
    alarmed_already[PERIPHERALS_ALARM_A] = true;
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *_hrtc) {
    if (alarm_callbacks[PERIPHERALS_ALARM_B]) {
        alarm_callbacks[PERIPHERALS_ALARM_B]();
    }
    HAL_RTC_DeactivateAlarm(_hrtc, RTC_ALARM_B);
    alarmed_already[PERIPHERALS_ALARM_B] = true;
}
