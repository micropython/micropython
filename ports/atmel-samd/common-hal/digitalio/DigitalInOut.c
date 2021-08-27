/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "hal/include/hal_gpio.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "supervisor/shared/translate.h"

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;
    self->output = false;
    self->open_drain = false;

    // Must set pull after setting direction.
    gpio_set_pin_direction(pin->number, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(pin->number, GPIO_PULL_OFF);
    return DIGITALINOUT_OK;
}

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->number);
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

void common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    self->output = false;
    // This also sets direction to input.
    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    const uint8_t pin = self->pin->number;
    gpio_set_pin_pull_mode(pin, GPIO_PULL_OFF);
    // Turn on "strong" pin driving (more current available). See DRVSTR doc in datasheet.
    hri_port_set_PINCFG_DRVSTR_bit(PORT, (enum gpio_port)GPIO_PORT(pin), GPIO_PIN(pin));

    self->output = true;
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;

    // Direction is set in set_value. We don't need to do it here.
    common_hal_digitalio_digitalinout_set_value(self, value);
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    return self->output ? DIRECTION_OUTPUT : DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    const uint8_t pin = self->pin->number;
    const uint8_t port = GPIO_PORT(pin);
    const uint32_t pin_mask = 1U << GPIO_PIN(pin);
    if (value) {
        if (self->open_drain) {
            // Assertion: pull is off, so the pin is floating in this case.
            // We do open-drain high output (no sinking of current)
            // by changing the direction to input with no pulls.
            hri_port_clear_DIR_DIR_bf(PORT, port, pin_mask);
        } else {
            hri_port_set_DIR_DIR_bf(PORT, port, pin_mask);
            hri_port_set_OUT_OUT_bf(PORT, port, pin_mask);
        }
    } else {
        hri_port_set_DIR_DIR_bf(PORT, port, pin_mask);
        hri_port_clear_OUT_OUT_bf(PORT,port, pin_mask);
    }
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    const uint8_t pin = self->pin->number;
    if (!self->output) {
        return gpio_get_pin_level(pin);
    } else {
        if (self->open_drain && hri_port_get_DIR_reg(PORT, GPIO_PORT(pin), 1U << GPIO_PIN(pin)) == 0) {
            return true;
        } else {
            return hri_port_get_OUT_reg(PORT, GPIO_PORT(pin), 1U << GPIO_PIN(pin));
        }
    }
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    // True is implemented differently between modes so reset the value to make
    // sure it's correct for the new mode.
    if (value) {
        common_hal_digitalio_digitalinout_set_value(self, value);
    }
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    if (self->open_drain) {
        return DRIVE_MODE_OPEN_DRAIN;
    } else {
        return DRIVE_MODE_PUSH_PULL;
    }
}

void common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    enum gpio_pull_mode asf_pull = GPIO_PULL_OFF;
    switch (pull) {
        case PULL_UP:
            asf_pull = GPIO_PULL_UP;
            break;
        case PULL_DOWN:
            asf_pull = GPIO_PULL_DOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }
    // Must set pull after setting direction.
    gpio_set_pin_direction(self->pin->number, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(self->pin->number, asf_pull);
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    uint32_t pin = self->pin->number;
    if (self->output) {
        mp_raise_AttributeError(translate("Cannot get pull while in output mode"));
        return PULL_NONE;
    } else {
        if (hri_port_get_PINCFG_PULLEN_bit(PORT, GPIO_PORT(pin), GPIO_PIN(pin)) == 0) {
            return PULL_NONE;
        }
        if (hri_port_get_OUT_reg(PORT, GPIO_PORT(pin), 1U << GPIO_PIN(pin)) > 0) {
            return PULL_UP;
        } else {
            return PULL_DOWN;
        }
    }
}
