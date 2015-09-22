/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#ifndef PYBRTC_H_
#define PYBRTC_H_

// RTC triggers
#define PYB_RTC_ALARM0                      (0x01)

#define RTC_ACCESS_TIME_MSEC                (5)
#define PYB_RTC_MIN_ALARM_TIME_MS           (RTC_ACCESS_TIME_MSEC * 2)

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
    mp_obj_t irq_obj;
    uint32_t irq_flags;
    uint32_t alarm_ms;
    uint32_t alarm_time_s;
    uint16_t alarm_time_ms;
    byte pwrmode;
    bool alarmset;
    bool repeat;
    bool irq_enabled;
} pyb_rtc_obj_t;

extern const mp_obj_type_t pyb_rtc_type;

extern void pyb_rtc_pre_init(void);
extern void pyb_rtc_get_time (uint32_t *secs, uint16_t *msecs);
extern uint32_t pyb_rtc_get_seconds (void);
extern void pyb_rtc_calc_future_time (uint32_t a_mseconds, uint32_t *f_seconds, uint16_t *f_mseconds);
extern void pyb_rtc_repeat_alarm (pyb_rtc_obj_t *self);
extern void pyb_rtc_disable_alarm (void);

#endif  // PYBRTC_H_
