/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef RA_RTC_H_
#define RA_RTC_H_

#include <stdint.h>
#include <stdbool.h>

#define RTC_PERIOD_MINUTE 0x00
#define RTC_PERIOD_SECOND 0x01

typedef struct {
    unsigned short year;
    unsigned char month;
    unsigned char date;
    unsigned char weekday;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} ra_rtc_t;

typedef void (*ra_rtc_cb_t)(void *);

int ra_rtc_get_year(void);
int ra_rtc_get_month(void);
int ra_rtc_get_date(void);
int ra_rtc_get_hour(void);
int ra_rtc_get_minute(void);
int ra_rtc_get_second(void);
int ra_rtc_get_weekday(void);
void ra_rtc_period_on();
void ra_rtc_period_off();
void ra_rtc_set_period_time(uint32_t period);
void ra_rtc_set_period_func(void *cb, void *param);
void ra_rtc_alarm_on(void);
void ra_rtc_alarm_off(void);
void ra_rtc_set_alarm_time(int hour, int min, int week_flag);
void ra_rtc_set_alarm_func(void *cb, void *param);
void ra_rtc_set_adjustment(int adj, int aadjp);
uint8_t ra_rtc_get_adjustment(void);
bool ra_rtc_set_time(ra_rtc_t *time);
bool ra_rtc_get_time(ra_rtc_t *time);
bool ra_rtc_init(uint8_t source);
bool ra_rtc_deinit(void);
void rtc_alarm_periodic_isr(void);
void rtc_carry_isr(void);

#endif /* RA_RTC_H_ */
