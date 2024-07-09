// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"

#include "peripherals/pins.h"

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number);
void claim_pin(uint8_t pin_port, uint8_t pin_number);
bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number);
GPIO_TypeDef *pin_port(uint8_t pin_port);
uint16_t pin_mask(uint8_t pin_number);
