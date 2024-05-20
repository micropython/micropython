// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"

#include "peripherals/broadcom/gpio.h"

static bool pin_in_use[BCM_PIN_COUNT];
static bool never_reset_pin[BCM_PIN_COUNT];

void reset_all_pins(void) {
    for (size_t i = 0; i < BCM_PIN_COUNT; i++) {
        if (never_reset_pin[i]) {
            continue;
        }
        reset_pin_number(i);
    }
}

void never_reset_pin_number(uint8_t pin_number) {
    never_reset_pin[pin_number] = true;
}

void reset_pin_number(uint8_t pin_number) {
    pin_in_use[pin_number] = false;
    never_reset_pin[pin_number] = false;
    // Reset JTAG pins back to JTAG.
    BP_PULL_Enum pull = BP_PULL_NONE;
    if (22 <= pin_number && pin_number <= 27) {
        gpio_set_function(pin_number, GPIO_FUNCTION_ALT4);
    } else {
        gpio_set_function(pin_number, GPIO_FUNCTION_INPUT);
    }
    // Set the pull to match the datasheet.
    if (pin_number < 9 ||
        (33 < pin_number && pin_number < 37) ||
        pin_number > 45) {
        pull = BP_PULL_UP;
    } else if (pin_number != 28 &&
               pin_number != 29 &&
               pin_number != 44 &&
               pin_number != 45) {
        // Most pins are pulled low so we only exclude the four pins that aren't
        // pulled at all. This will also set the JTAG pins 22-27
        pull = BP_PULL_DOWN;
    }
    gpio_set_pull(pin_number, pull);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    pin_in_use[pin->number] = true;
}

bool pin_number_is_free(uint8_t pin_number) {
    return !pin_in_use[pin_number];
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    return claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}
