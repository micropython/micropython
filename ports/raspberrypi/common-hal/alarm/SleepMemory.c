// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"
#include "shared-bindings/alarm/SleepMemory.h"

__attribute__((section(".uninitialized"))) static uint8_t _sleepmem[SLEEP_MEMORY_LENGTH];
__attribute__((section(".uninitialized"))) static uint32_t _sleepmem_magicnum;
#define SLEEP_MEMORY_DATA_GUARD 0xad0000af
#define SLEEP_MEMORY_DATA_GUARD_MASK 0xff0000ff

static int is_sleep_memory_valid(void) {
    if ((_sleepmem_magicnum & SLEEP_MEMORY_DATA_GUARD_MASK)
        == SLEEP_MEMORY_DATA_GUARD) {
        return 1;
    }
    return 0;
}

static void initialize_sleep_memory(void) {
    memset((uint8_t *)_sleepmem, 0, SLEEP_MEMORY_LENGTH);

    _sleepmem_magicnum = SLEEP_MEMORY_DATA_GUARD;
}

void alarm_sleep_memory_reset(void) {
    if (!is_sleep_memory_valid()) {
        initialize_sleep_memory();
    }
}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    return sizeof(_sleepmem);
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len) {
    if (start_index + len > sizeof(_sleepmem)) {
        return false;
    }

    memcpy((uint8_t *)(_sleepmem + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len) {
    if (start_index + len > sizeof(_sleepmem)) {
        return;
    }
    memcpy(values, (uint8_t *)(_sleepmem + start_index), len);
}
