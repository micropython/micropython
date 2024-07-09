// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <assert.h>
#include <stdint.h>

#include <py/obj.h>

#include "peripherals/pins.h"

// If a board needs a different reset state for one or more pins, implement
// board_reset_pin_number so that it sets this state and returns `true` for those
// pin numbers, `false` for others.
// A default weak implementation always returns `false`.
bool board_reset_pin_number(uint8_t pin_number);

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_number);
void claim_pin(const mcu_pin_obj_t *pin);
bool pin_number_is_free(uint8_t pin_number);

#if CIRCUITPY_CYW43
extern bool cyw_ever_init;
void reset_pin_number_cyw(uint8_t pin_number);
#endif
