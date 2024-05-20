// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"

#include STM32_HAL_H

#if CPY_STM32L4
    #define __HAL_RCC_BKPSRAM_CLK_ENABLE()
    #define STM_BKPSRAM_SIZE  0
    #define STM_BKPSRAM_START 0
    #define HAL_PWREx_EnableBkUpReg()
// backup RAM disabled for now. Will have the backup region at the top of SRAM3 which is retained.
#else
    #define STM_BKPSRAM_SIZE    0x1000
    #define STM_BKPSRAM_START   BKPSRAM_BASE
#endif


static bool initialized = false;

static void lazy_init(void) {
    if (!initialized) {
        __HAL_RCC_BKPSRAM_CLK_ENABLE();
        HAL_PWREx_EnableBkUpReg();
        HAL_PWR_EnableBkUpAccess();
        initialized = true;
    }
}

void alarm_sleep_memory_reset(void) {

}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    lazy_init();
    return STM_BKPSRAM_SIZE;
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len) {
    if (start_index + len > STM_BKPSRAM_SIZE) {
        return false;
    }
    lazy_init();
    memcpy((uint8_t *)(STM_BKPSRAM_START + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len) {
    if (start_index + len > STM_BKPSRAM_SIZE) {
        return;
    }
    lazy_init();
    memcpy(values, (uint8_t *)(STM_BKPSRAM_START + start_index), len);
    return;
}
