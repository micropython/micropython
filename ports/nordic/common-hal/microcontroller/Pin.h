// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"

#include "peripherals/nrf/pins.h"

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin);
void claim_pin(const mcu_pin_obj_t *pin);
bool pin_number_is_free(uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_number);

// Lower 5 bits of a pin number are the pin number in a port.
// upper bits (just one bit for current chips) is port number.

static inline uint8_t nrf_pin_port(uint8_t absolute_pin) {
    return absolute_pin >> 5;
}

static inline uint8_t nrf_relative_pin_number(uint8_t absolute_pin) {
    return absolute_pin & 0x1f;
}
