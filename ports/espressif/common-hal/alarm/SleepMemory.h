// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// There are several places we could store persistent data for SleepMemory:
//
// RTC registers: There are a few 32-bit registers maintained during deep sleep.
// We are already using one for saving sleep information during deep sleep.
//
// RTC Fast Memory: 8kB, also used for deep-sleep power-on stub.
// RTC Slow Memory: 8kB, also used for the ULP (tiny co-processor available during sleep).
//
// The ESP-IDF build system takes care of the power management of these regions.
// RTC_DATA_ATTR will allocate storage in RTC_SLOW_MEM unless CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM
// is set. Any memory not allocated by us can be used by the ESP-IDF for heap or other purposes.

// Use half of RTC_SLOW_MEM or RTC_FAST_MEM.
#if defined(CONFIG_IDF_TARGET_ESP32H2)
// H2 has 4k of low power RAM
#define SLEEP_MEMORY_LENGTH (2 * 1024)
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define SLEEP_MEMORY_LENGTH (3 * 1024)
#else
#define SLEEP_MEMORY_LENGTH (4 * 1024)
#endif

typedef struct {
    mp_obj_base_t base;
} alarm_sleep_memory_obj_t;

extern void alarm_sleep_memory_reset(void);
