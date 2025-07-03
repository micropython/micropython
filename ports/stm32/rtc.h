/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_RTC_H
#define MICROPY_INCLUDED_STM32_RTC_H

#include "py/obj.h"

extern RTC_HandleTypeDef RTCHandle;
extern const mp_obj_type_t pyb_rtc_type;

void rtc_init_start(bool force_init);
void rtc_init_finalise(void);

mp_obj_t pyb_rtc_wakeup(size_t n_args, const mp_obj_t *args);

#if defined(MICROPY_HW_RTC_USE_US) && MICROPY_HW_RTC_USE_US
uint32_t rtc_subsec_to_us(uint32_t ss);
uint32_t rtc_us_to_subsec(uint32_t us);
#endif

#endif // MICROPY_INCLUDED_STM32_RTC_H
