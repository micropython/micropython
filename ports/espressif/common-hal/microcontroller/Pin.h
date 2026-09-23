// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"

#include "peripherals/pins.h"

// 64-bit pin mask for a single bit
#define PIN_BIT(pin_number) (((uint64_t)1) << pin_number)

extern void common_hal_reset_pin(const mcu_pin_obj_t *pin);

extern void reset_pin_number(gpio_num_t pin_number);
// reset all pins in `bitmask`
extern void reset_pin_mask(uint64_t bitmask);
extern void reset_pin_state(void);
extern void claim_pin(const mcu_pin_obj_t *pin);
extern void claim_pin_number(gpio_num_t pin_number);
extern bool pin_number_is_free(gpio_num_t pin_number);

extern uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin);
extern void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin);

extern void preserve_pin_number(gpio_num_t pin_number);

// Allow the board to reset a pin in a board-specific way. This can be used
// for LEDs or enable pins to put them in a state beside the default pull-up.
// Return true to indicate that the pin was reset. Returning false will lead to
// the port-default reset behavior.
extern bool espressif_board_reset_pin_number(gpio_num_t pin_number);

// Configure the IOMUX for the pin as GPIO, set the pin as output, and then set output the level.
// This ensures the IOMUX setting is correct.
extern void config_pin_as_output_with_level(gpio_num_t pin_number, bool level);
