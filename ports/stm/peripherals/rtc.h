// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Wireless Inc
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PERIPHERALS_ALARM_A 0
#define PERIPHERALS_ALARM_B 1

uint32_t stm32_peripherals_get_rtc_freq(void);
void stm32_peripherals_rtc_init(void);
uint64_t stm32_peripherals_rtc_monotonic_ticks(uint8_t *subticks);

void stm32_peripherals_rtc_assign_wkup_callback(void (*callback)(void));
void stm32_peripherals_rtc_set_wakeup_mode_seconds(uint32_t seconds);
void stm32_peripherals_rtc_set_wakeup_mode_tick(void);
void stm32_peripherals_rtc_enable_wakeup_timer(void);
void stm32_peripherals_rtc_disable_wakeup_timer(void);

void stm32_peripherals_rtc_reset_alarms(void);
void stm32_peripherals_rtc_assign_alarm_callback(uint8_t alarm_idx, void (*callback)(void));
void stm32_peripherals_rtc_set_alarm(uint8_t alarm_idx, uint32_t ticks);
bool stm32_peripherals_rtc_alarm_triggered(uint8_t alarm_idx);

#if CIRCUITPY_RTC
typedef struct _timeutils_struct_time_t timeutils_struct_time_t;
void stm32_peripherals_rtc_get_time(timeutils_struct_time_t *tm);
void stm32_peripherals_rtc_set_time(timeutils_struct_time_t *tm);
#endif
