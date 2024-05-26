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
extern void common_hal_never_reset_pin(const mcu_pin_obj_t *pin);

extern void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
extern void reset_pin_number(gpio_num_t pin_number);
// reset all pins in `bitmask`
extern void reset_pin_mask(uint64_t bitmask);
extern void skip_reset_once_pin_number(gpio_num_t pin_number);
extern void claim_pin(const mcu_pin_obj_t *pin);
extern void claim_pin_number(gpio_num_t pin_number);
extern bool pin_number_is_free(gpio_num_t pin_number);
extern void never_reset_pin_number(gpio_num_t pin_number);

extern void preserve_pin_number(gpio_num_t pin_number);
extern void clear_pin_preservations(void);

// Allow the board to reset a pin in a board-specific way. This can be used
// for LEDs or enable pins to put them in a state beside the default pull-up.
// Return true to indicate that the pin was reset. Returning false will lead to
// the port-default reset behavior.
extern bool espressif_board_reset_pin_number(gpio_num_t pin_number);
