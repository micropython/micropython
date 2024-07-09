// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"
#include "shared-bindings/alarm/SleepMemory.h"

#include "esp_sleep.h"

// Data storage for singleton instance of SleepMemory.
// Might be RTC_SLOW_MEM or RTC_FAST_MEM, depending on setting of CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM.
static RTC_DATA_ATTR uint8_t _sleep_mem[SLEEP_MEMORY_LENGTH];

void alarm_sleep_memory_reset(void) {
    // ESP-IDF build system takes care of doing esp_sleep_pd_config() or the equivalent with
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
