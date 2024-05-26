// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include <string.h>

#include "common-hal/microcontroller/Processor.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

float common_hal_mcu_processor_get_temperature(void) {
    return NAN;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return 100000000;
}

void common_hal_mcu_processor_set_frequency(mcu_processor_obj_t *self, uint32_t frequency) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    raw_id[0] = 0xaf;
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_POWER_ON;
}
