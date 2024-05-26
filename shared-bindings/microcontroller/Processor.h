// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

extern const mp_obj_type_t mcu_processor_type;

uint32_t common_hal_mcu_processor_get_frequency(void);
mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void);
float common_hal_mcu_processor_get_temperature(void);
void common_hal_mcu_processor_get_uid(uint8_t raw_id[]);
float common_hal_mcu_processor_get_voltage(void);
void common_hal_mcu_processor_set_frequency(mcu_processor_obj_t *self, uint32_t frequency);
