// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Junji Sakai
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include "py/runtime.h"
#include "common-hal/alarm/__init__.h"
#include "common-hal/alarm/SleepMemory.h"
#include "shared-bindings/alarm/SleepMemory.h"
#include "nrf_power.h"

__attribute__((section(".uninitialized"))) static uint8_t _sleepmem[SLEEP_MEMORY_LENGTH];
__attribute__((section(".uninitialized"))) uint8_t sleepmem_wakeup_event;
__attribute__((section(".uninitialized"))) uint8_t sleepmem_wakeup_pin;
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

void set_memory_retention(void) {
    // set RAM[n].POWER register for RAM retention
    // nRF52840 has RAM[0..7].Section[0..1] and RAM[8].Section[0..5]
    // nRF52833 has RAM[0..7].Section[0..1] and RAM[8].Section[0,1]
    for (int block = 0; block <= 7; ++block) {
        nrf_power_rampower_mask_on(NRF_POWER, block,
            NRF_POWER_RAMPOWER_S0RETENTION_MASK |
            NRF_POWER_RAMPOWER_S1RETENTION_MASK);
    };
    #ifdef NRF52840
    nrf_power_rampower_mask_on(NRF_POWER, 8,
        NRF_POWER_RAMPOWER_S0RETENTION_MASK |
        NRF_POWER_RAMPOWER_S1RETENTION_MASK |
        NRF_POWER_RAMPOWER_S2RETENTION_MASK |
        NRF_POWER_RAMPOWER_S3RETENTION_MASK |
        NRF_POWER_RAMPOWER_S4RETENTION_MASK |
        NRF_POWER_RAMPOWER_S5RETENTION_MASK);
    #endif
    #ifdef NRF52833
    nrf_power_rampower_mask_on(NRF_POWER, 8,
        NRF_POWER_RAMPOWER_S0RETENTION_MASK |
        NRF_POWER_RAMPOWER_S1RETENTION_MASK);
    #endif
}

static void initialize_sleep_memory(void) {
    memset((uint8_t *)_sleepmem, 0, SLEEP_MEMORY_LENGTH);
    sleepmem_wakeup_event = 0;
    sleepmem_wakeup_pin = 0;

    set_memory_retention();

    _sleepmem_magicnum = SLEEP_MEMORY_DATA_GUARD;
}

void alarm_sleep_memory_reset(void) {
    if (!is_sleep_memory_valid()) {
        initialize_sleep_memory();
        #ifdef NRF_DEBUG_PRINT
        mp_printf(&mp_plat_print, "sleep memory initialized\r\n");
        #endif
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
