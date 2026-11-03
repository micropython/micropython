// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/alarm/SleepMemory.h"
#include "common-hal/alarm/pin/PinAlarm.h"
#include "common-hal/alarm/time/TimeAlarm.h"

#include "hardware/regs/clocks.h"

#define RP_SLEEP_WAKEUP_UNDEF   0
#define RP_SLEEP_WAKEUP_GPIO    1
#define RP_SLEEP_WAKEUP_RTC     2

// Watchdog scratch registers survive the reset that ends a deep sleep.
// The SDK and bootrom use scratch registers 4 to 7.
//
// RP_WKUP_SCRATCH_REG      holds the RP_SLEEP_WAKEUP_* cause.
// RP_WKUP_PIN_SCRATCH_REG  holds the GPIO number that woke an RP2040 from deep sleep,
//                          plus 1. 0 means no pin.
#define RP_WKUP_SCRATCH_REG     0
#define RP_WKUP_PIN_SCRATCH_REG 1

// Bits in powman_hw->last_swcore_pwrup. Bit 0 is a chip reset, bits 1 to 4 are
// the four GPIO wakeups and bit 6 is the timer alarm.
#define RP_POWMAN_PWRUP_GPIO_LSB  1
#define RP_POWMAN_PWRUP_GPIO_BITS 0x1e
#define RP_POWMAN_PWRUP_ALARM_BITS 0x40

#if PICO_RP2350
// Returns true when this boot is powman powering the core back up for a deep
// sleep alarm. Returns false after any other kind of reset.
bool alarm_woke_from_powman(void);
#endif

typedef union {
    alarm_pin_pinalarm_obj_t pin_alarm;
    alarm_time_timealarm_obj_t time_alarm;
} alarm_wake_alarm_union_t;

extern alarm_wake_alarm_union_t alarm_wake_alarm;
extern const alarm_sleep_memory_obj_t alarm_sleep_memory_obj;

extern void alarm_reset(void);
