/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#ifndef __MICROPY_INCLUDED_STM32_PERIPHERALS_RTC_H__
#define __MICROPY_INCLUDED_STM32_PERIPHERALS_RTC_H__

#include <stdint.h>
#include <stdbool.h>

#define PERIPHERALS_ALARM_A 0
#define PERIPHERALS_ALARM_B 1

uint32_t stm32_peripherals_get_rtc_freq(void);
void stm32_peripherals_rtc_init(void);
uint64_t stm32_peripherals_rtc_raw_ticks(uint8_t *subticks);

void stm32_peripherals_rtc_assign_wkup_callback(void (*callback)(void));
void stm32_peripherals_rtc_set_wakeup_mode_seconds(uint32_t seconds);
void stm32_peripherals_rtc_set_wakeup_mode_tick(void);
void stm32_peripherals_rtc_enable_wakeup_timer(void);
void stm32_peripherals_rtc_disable_wakeup_timer(void);

void stm32_peripherals_rtc_reset_alarms(void);
void stm32_peripherals_rtc_assign_alarm_callback(uint8_t alarm_idx, void (*callback)(void));
void stm32_peripherals_rtc_set_alarm(uint8_t alarm_idx, uint32_t ticks);
bool stm32_peripherals_rtc_alarm_triggered(uint8_t alarm_idx);

#endif // __MICROPY_INCLUDED_STM32_PERIPHERALS_RTC_H__
