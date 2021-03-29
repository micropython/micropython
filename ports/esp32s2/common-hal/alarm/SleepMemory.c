/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"

#include "esp_log.h"
#include "esp_sleep.h"

// Data storage for singleton instance of SleepMemory.
// Might be RTC_SLOW_MEM or RTC_FAST_MEM, depending on setting of CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM.
static RTC_DATA_ATTR uint8_t _sleep_mem[SLEEP_MEMORY_LENGTH];

void alarm_sleep_memory_reset(void) {
    // ESP-IDF build system takes care of doing esp_sleep_pd_config() or the equivalentwith
    // the correct settings, depending on which RTC mem we are using.
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/system/sleep_modes.html#power-down-of-rtc-peripherals-and-memories
}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    return sizeof(_sleep_mem);
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len) {

    if (start_index + len > sizeof(_sleep_mem)) {
        return false;
    }

    memcpy((uint8_t *)(_sleep_mem + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len) {

    if (start_index + len > sizeof(_sleep_mem)) {
        return;
    }
    memcpy(values, (uint8_t *)(_sleep_mem + start_index), len);
}
