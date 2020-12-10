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

#define SLEEP_MEMORY_LENGTH (8192)

// There are several places we could store persistent data for SleepMemory:
//
// RTC registers: There are a few 32-bit registers maintained during deep sleep.
// We are already using one for saving sleep information during deep sleep.
//
// RTC Fast Memory: 8kB, also used for deep-sleep power on stub, and for heap
// during normal operation if CONFIG_ESP32S2_ALLOW_RTC_FAST_MEM_AS_HEAP is set.
// Power-on during deep sleep must be enabled.
// I experimented with using RTC Fast Memory. It seemed to work, but occasionally,
// got smashed for unknown reasons.
// Base of RTC Fast memory on the data bus is 0x3FF9E000. The address is different on the instruction bus.
//
// RTC Slow Memory: 8kB, also used for the ULP (tiny co-processor available during sleep).
// Less likely to be used by ESP-IDF.
// Since we may want to use the ULP in the future, we will use the upper half
// of Slow Memory and reserve the lower half for ULP.
// From ulp.h:
// #define RTC_SLOW_MEM ((uint32_t*) 0x50000000)       /*!< RTC slow memory, 8k size */

// Upper half of RTC_SLOW_MEM.
#define SLEEP_MEMORY_LENGTH (4096)
#define SLEEP_MEMORY_BASE (0x50000000 + 4096)

typedef struct {
    mp_obj_base_t base;
} alarm_sleep_memory_obj_t;

extern void alarm_sleep_memory_reset(void);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_ALARM_SLEEPMEMORY_H
