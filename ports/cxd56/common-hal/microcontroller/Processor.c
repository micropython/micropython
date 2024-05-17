// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <stdbool.h> // for cxd56_clock.h
#include <cxd56_clock.h>
#include <sys/boardctl.h>

// For NAN: remove when not needed.
#include <math.h>
#include "py/mphal.h"
#include "shared-bindings/microcontroller/ResetReason.h"

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return cxd56_get_cpu_baseclk();
}

float common_hal_mcu_processor_get_temperature(void) {
    return NAN;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    boardctl(BOARDIOC_UNIQUEID, (uintptr_t)raw_id);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
