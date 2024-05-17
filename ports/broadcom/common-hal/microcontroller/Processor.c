// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Processor.h"

#include <math.h>

#include "peripherals/broadcom/vcmailbox.h"

float common_hal_mcu_processor_get_temperature(void) {
    uint32_t temp = vcmailbox_get_temperature();
    return temp / 1000.0f;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return vcmailbox_get_clock_rate_measured(VCMAILBOX_CLOCK_ARM);
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    uint64_t serial = vcmailbox_get_serial_number();
    for (size_t i = 0; i < sizeof(uint64_t); i++) {
        raw_id[i] = (serial >> (64 - ((i + 1) * 8))) & 0xff;
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
