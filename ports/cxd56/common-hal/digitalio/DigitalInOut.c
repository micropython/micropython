/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <arch/board/board.h>

#include "py/runtime.h"

#include "shared-bindings/digitalio/DigitalInOut.h"

digitalinout_result_t common_hal_digitalio_digitalinout_construct(digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->analog) {
        mp_raise_ValueError(translate("DigitalInOut not supported on given pin"));
    }

    claim_pin(pin);

    self->pin = pin;
    self->input = true;
    self->open_drain = false;

    board_gpio_write(self->pin->number, -1);
    board_gpio_config(self->pin->number, 0, true, true, PIN_FLOAT);

    return DIGITALINOUT_OK;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    board_gpio_write(self->pin->number, -1);
    board_gpio_config(self->pin->number, 0, false, true, PIN_FLOAT);

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_switch_to_input(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    self->input = true;
    self->pull = pull;
    board_gpio_write(self->pin->number, -1);
    board_gpio_config(self->pin->number, 0, true, true, pull);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(digitalio_digitalinout_obj_t *self, bool value, digitalio_drive_mode_t drive_mode) {
    self->input = false;
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    board_gpio_write(self->pin->number, -1);
    board_gpio_config(self->pin->number, 0, false, true, PIN_FLOAT);

    if (self->open_drain) {
        board_gpio_write(self->pin->number, 0);
    }
    common_hal_digitalio_digitalinout_set_value(self, value);
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(digitalio_digitalinout_obj_t *self) {
    return self->input ? DIRECTION_INPUT : DIRECTION_OUTPUT;
}

void common_hal_digitalio_digitalinout_set_value(digitalio_digitalinout_obj_t *self, bool value) {
    if (self->open_drain) {
        if (value) {
            board_gpio_write(self->pin->number, -1);
            board_gpio_config(self->pin->number, 0, true, true, PIN_PULLUP);
        } else {
            board_gpio_config(self->pin->number, 0, false, true, PIN_FLOAT);
            board_gpio_write(self->pin->number, 0);
        }
    } else {
        board_gpio_write(self->pin->number, value);
    }
}

bool common_hal_digitalio_digitalinout_get_value(digitalio_digitalinout_obj_t *self) {
    return board_gpio_read(self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(digitalio_digitalinout_obj_t *self, digitalio_drive_mode_t drive_mode) {
    if (drive_mode == DRIVE_MODE_PUSH_PULL) {
        board_gpio_write(self->pin->number, -1);
        board_gpio_config(self->pin->number, 0, false, true, PIN_FLOAT);
        self->open_drain = false;
    } else {
        board_gpio_write(self->pin->number, -1);
        board_gpio_config(self->pin->number, 0, false, true, PIN_FLOAT);
        board_gpio_write(self->pin->number, 0);
        self->open_drain = true;
    }
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(digitalio_digitalinout_obj_t *self) {
    return self->open_drain ? DRIVE_MODE_OPEN_DRAIN : DRIVE_MODE_PUSH_PULL;
}

void common_hal_digitalio_digitalinout_set_pull(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    self->pull = pull;
    board_gpio_write(self->pin->number, -1);
    board_gpio_config(self->pin->number, 0, true, true, pull);
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(digitalio_digitalinout_obj_t *self) {
    return self->pull;
}

void common_hal_digitalio_digitalinout_never_reset(digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->number);
}
