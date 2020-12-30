/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_ALARM_SLEEPMEMORY_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_ALARM_SLEEPMEMORY_H

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
#define SLEEP_MEMORY_LENGTH (4096)

typedef struct {
    mp_obj_base_t base;
} alarm_sleep_memory_obj_t;

extern void alarm_sleep_memory_reset(void);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_ALARM_SLEEPMEMORY_H
