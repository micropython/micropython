/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "driver/gpio.h"


#include "esp_log.h"

static const char* TAG = "CircuitPython digitalio";

void common_hal_digitalio_digitalinout_never_reset(
        digitalio_digitalinout_obj_t *self) {
    (void)self;
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
        digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {

    // claim_pin(pin);
    self->pin = pin;

    ESP_EARLY_LOGW(TAG, "construct %d", pin->number);

    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == mp_const_none;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    // reset_pin_number(0, self->pin->number);
    self->pin = mp_const_none;
}

void common_hal_digitalio_digitalinout_switch_to_input(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    gpio_config_t config = {
        .pin_bit_mask = 1 << self->pin->number,
        .mode = GPIO_MODE_INPUT
    };
    if (pull == PULL_UP) {
        config.pull_up_en = GPIO_PULLUP_ENABLE;
    } else if (pull == PULL_DOWN) {
        config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    gpio_config(&config);
}

void common_hal_digitalio_digitalinout_switch_to_output(
        digitalio_digitalinout_obj_t *self, bool value,
        digitalio_drive_mode_t drive_mode) {
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << self->pin->number,
        .mode = GPIO_MODE_OUTPUT
    };
    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        config.mode = GPIO_MODE_OUTPUT_OD;
    }
    gpio_set_level(self->pin->number, value);
    gpio_config(&config);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
        digitalio_digitalinout_obj_t *self) {

    return DIRECTION_OUTPUT;
}

void common_hal_digitalio_digitalinout_set_value(
        digitalio_digitalinout_obj_t *self, bool value) {
    ESP_EARLY_LOGW(TAG, "set %d %d", self->pin->number, value);
    gpio_set_level(self->pin->number, value);
}

bool common_hal_digitalio_digitalinout_get_value(
        digitalio_digitalinout_obj_t *self) {
    return true;
}

void common_hal_digitalio_digitalinout_set_drive_mode(
        digitalio_digitalinout_obj_t *self,
        digitalio_drive_mode_t drive_mode) {
    (void)self;
    (void)drive_mode;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
        digitalio_digitalinout_obj_t *self) {
    // if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_OUTPUT)
    //     return DRIVE_MODE_PUSH_PULL;
    // else
    return DRIVE_MODE_OPEN_DRAIN;
}

void common_hal_digitalio_digitalinout_set_pull(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    (void)self;
    (void)pull;
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
        digitalio_digitalinout_obj_t *self) {
    return PULL_NONE;
}
