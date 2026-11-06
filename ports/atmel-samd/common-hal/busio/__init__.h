// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"

// Connect pin to its SERCOM with no pull, optionally with strong drive, and claim it.
// Returns the pin number.
uint8_t sercom_setup_pin(const mcu_pin_obj_t *pin, uint32_t pinmux, enum gpio_direction direction,
    bool strong_drive);
