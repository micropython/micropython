// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include <assert.h>

#include "peripherals/samd/pins.h"

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_number);
// Pins are claimed in a table, not detected from their hardware configuration, so code
// that configures a pin directly must claim it.
void claim_pin(const mcu_pin_obj_t *pin);
void claim_pin_number(uint8_t pin_number);
// Returns true when the pin is not claimed, and is not an SWD pin while a debugger is attached.
bool pin_number_is_free(uint8_t pin_number);

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin);

typedef struct {
    const mcu_pin_obj_t *obj;
    uint8_t instance;
    uint8_t pin;
    uint16_t function;
} mcu_pin_function_t;

mcu_pin_function_t *mcu_find_pin_function(mcu_pin_function_t *table, const mcu_pin_obj_t *pin, int instance, uint16_t name);
