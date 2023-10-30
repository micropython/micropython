/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#if CIRCUITPY_CYW43
#include "pico/cyw43_arch.h"
#include "bindings/cyw43/__init__.h"
#define IS_CYW(self) ((self)->pin->base.type == &cyw43_pin_type)

const mcu_pin_obj_t *common_hal_digitalio_validate_pin(mp_obj_t obj) {
    return validate_obj_is_free_pin_including_cyw43(obj, MP_QSTR_pin);
}
#endif

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;
    self->output = false;
    self->open_drain = false;

    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        return DIGITALINOUT_OK;
    }
    #endif

    // Set to input. No output value.
    gpio_init(pin->number);
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
    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    // This also sets direction to input.
    return common_hal_digitalio_digitalinout_set_pull(self, pull);
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {

    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        if (drive_mode != DRIVE_MODE_PUSH_PULL) {
            return DIGITALINOUT_INVALID_DRIVE_MODE;
        }
        cyw43_arch_gpio_put(self->pin->number, value);
        self->output = true;
        self->open_drain = false;
        return DIGITALINOUT_OK;
    }
    #endif
    const uint8_t pin = self->pin->number;
    gpio_disable_pulls(pin);

    // Turn on "strong" pin driving (more current available).
    hw_write_masked(&padsbank0_hw->io[pin],
        PADS_BANK0_GPIO0_DRIVE_VALUE_12MA << PADS_BANK0_GPIO0_DRIVE_LSB,
            PADS_BANK0_GPIO0_DRIVE_BITS);

    self->output = true;
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;

    // Pin direction is ultimately set in set_value. We don't need to do it here.
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
    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        cyw43_arch_gpio_put(pin, value);
        return;
    }
    #endif
    if (self->open_drain && value) {
        // If true and open-drain, set the direction -before- setting
        // the pin value, to to avoid a high glitch on the pin before
        // switching from output to input for open-drain.
        gpio_set_dir(pin, GPIO_IN);
        gpio_put(pin, value);
    } else {
        // Otherwise set the direction -after- setting the pin value,
        // to avoid a glitch which might occur if the old value was
        // different and the pin was previously set to input.
        gpio_put(pin, value);
        gpio_set_dir(pin, GPIO_OUT);
    }
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        return cyw43_arch_gpio_get(self->pin->number);
    }
    #endif
    return gpio_get(self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        if (drive_mode != DRIVE_MODE_PUSH_PULL) {
            return DIGITALINOUT_INVALID_DRIVE_MODE;
        }
    }
    #endif
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

digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        if (pull != PULL_NONE) {
            return DIGITALINOUT_INVALID_PULL;
        }
        cyw43_arch_gpio_get(self->pin->number);
        self->output = false;
        return DIGITALINOUT_OK;
    }
    #endif
    const uint8_t pin = self->pin->number;
    gpio_set_pulls(pin, pull == PULL_UP, pull == PULL_DOWN);
    gpio_set_dir(pin, GPIO_IN);
    self->output = false;
    return DIGITALINOUT_OK;
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    uint32_t pin = self->pin->number;
    if (self->output) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Cannot get pull while in output mode"));
        return PULL_NONE;
    } else {
        if (gpio_is_pulled_up(pin)) {
            return PULL_UP;
        } else if (gpio_is_pulled_down(pin)) {
            return PULL_DOWN;
        }
    }
    return PULL_NONE;
}

bool common_hal_digitalio_has_reg_op(digitalinout_reg_op_t op) {
    return true;
}

volatile uint32_t *common_hal_digitalio_digitalinout_get_reg(digitalio_digitalinout_obj_t *self, digitalinout_reg_op_t op, uint32_t *mask) {
    #if CIRCUITPY_CYW43
    if (IS_CYW(self)) {
        return NULL;
    }
    #endif
    const uint8_t pin = self->pin->number;

    *mask = 1u << pin;

    switch (op) {
        case DIGITALINOUT_REG_READ:
            return (volatile uint32_t *)&sio_hw->gpio_in;
        case DIGITALINOUT_REG_WRITE:
            return &sio_hw->gpio_out;
        case DIGITALINOUT_REG_SET:
            return &sio_hw->gpio_set;
        case DIGITALINOUT_REG_RESET:
            return &sio_hw->gpio_clr;
        case DIGITALINOUT_REG_TOGGLE:
            return &sio_hw->gpio_togl;
        default:
            return NULL;
    }
}
