// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/touchio/TouchIn.h"

#include "py/runtime.h"
#include "peripherals/touch.h"
#include "shared-bindings/microcontroller/Pin.h"

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (pin->touch_channel == NO_TOUCH_CHANNEL) {
        raise_ValueError_invalid_pin();
    }
    claim_pin(pin);

    // initialize touchpad
    peripherals_touch_init(pin->touch_channel);

    // Set a "touched" threshold not too far above the initial value.
    // For simple finger touch, the values may vary as much as a factor of two,
    // but for touches using fruit or other objects, the difference is much less.

    self->pin = pin;
    self->threshold = common_hal_touchio_touchin_get_raw_value(self) + 100;
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t *self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->touch_channel);
    self->pin = NULL;
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    return common_hal_touchio_touchin_get_raw_value(self) > self->threshold;
}

uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self) {
    return peripherals_touch_read(self->pin->touch_channel);
}

uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self) {
    return self->threshold;
}

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold) {
    self->threshold = new_threshold;
}
