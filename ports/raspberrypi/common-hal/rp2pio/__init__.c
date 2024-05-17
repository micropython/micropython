// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/__init__.h"

bool common_hal_rp2pio_pins_are_sequential(size_t len, const mcu_pin_obj_t **pins) {
    if (len == 0) {
        return true;
    }
    const mcu_pin_obj_t *last_pin = pins[0];
    for (size_t i = 1; i < len; i++) {
        const mcu_pin_obj_t *pin = pins[i];
        if (pin->number != last_pin->number + 1) {
            return false;
        }
    }
    return true;
}
