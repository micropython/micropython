// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "shared-bindings/nvm/ByteArray.h"

void alarm_sleep_memory_reset(void) {
}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    return BKUPRAM_SIZE;
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len) {
    if (start_index + len > BKUPRAM_SIZE) {
        return false;
    }
    memcpy((uint8_t *)(BKUPRAM_ADDR + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len) {
    if (start_index + len > BKUPRAM_SIZE) {
        return;
    }
    memcpy(values, (uint8_t *)(BKUPRAM_ADDR + start_index), len);
    return;
}
