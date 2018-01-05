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

#include "hal/hal_gpio.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
        digitalio_digitalinout_obj_t* self, const mcu_pin_obj_t* pin) {
    self->pin = pin;
    hal_gpio_cfg_pin(pin->port, pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);
    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t* self) {
    return self->pin == mp_const_none;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t* self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    reset_pin(self->pin->pin);
    self->pin = mp_const_none;
}

void common_hal_digitalio_digitalinout_switch_to_input(
        digitalio_digitalinout_obj_t* self, digitalio_pull_t pull) {
    self->output = false;

    hal_gpio_cfg_pin(self->pin->port, self->pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);
    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

void common_hal_digitalio_digitalinout_switch_to_output(
        digitalio_digitalinout_obj_t* self, bool value,
        digitalio_drive_mode_t drive_mode) {
    const uint8_t pin = self->pin->pin;

    self->output = true;
    self->open_drain = (drive_mode == DRIVE_MODE_OPEN_DRAIN);

    hal_gpio_cfg_pin(self->pin->port, pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    common_hal_digitalio_digitalinout_set_value(self, value);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
        digitalio_digitalinout_obj_t* self) {
    return self->output? DIRECTION_OUTPUT : DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
        digitalio_digitalinout_obj_t* self, bool value) {
    if (value) {
        if (self->open_drain) {
            hal_gpio_dir_set(self->pin->port, self->pin->pin, HAL_GPIO_MODE_INPUT);
        } else {
            hal_gpio_pin_set(self->pin->port, self->pin->pin);
            hal_gpio_dir_set(self->pin->port, self->pin->pin, HAL_GPIO_MODE_OUTPUT);
        }
    } else {
        hal_gpio_pin_clear(self->pin->port, self->pin->pin);
        hal_gpio_dir_set(self->pin->port, self->pin->pin, HAL_GPIO_MODE_OUTPUT);
    }
}

bool common_hal_digitalio_digitalinout_get_value(
        digitalio_digitalinout_obj_t* self) {
    const uint8_t pin = self->pin->pin;
    if (!self->output) {
        return hal_gpio_pin_read(self->pin);
    } else {
        if (self->open_drain && hal_gpio_dir_get(self->pin->port, self->pin->pin) == HAL_GPIO_MODE_INPUT) {
            return true;
        } else {
            return (GPIO_BASE(self->pin->port)->OUT >> pin) & 1;
        }
    }
}

void common_hal_digitalio_digitalinout_set_drive_mode(
        digitalio_digitalinout_obj_t* self,
        digitalio_drive_mode_t drive_mode) {
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    // True is implemented differently between modes so reset the value to make
    // sure its correct for the new mode.
    if (value) {
        common_hal_digitalio_digitalinout_set_value(self, value);
    }
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
        digitalio_digitalinout_obj_t* self) {
    if (self->open_drain) {
        return DRIVE_MODE_OPEN_DRAIN;
    } else {
        return DRIVE_MODE_PUSH_PULL;
    }
}

void common_hal_digitalio_digitalinout_set_pull(
        digitalio_digitalinout_obj_t* self, digitalio_pull_t pull) {
    hal_gpio_pull_t asf_pull = HAL_GPIO_PULL_DISABLED;
    switch (pull) {
        case PULL_UP:
            asf_pull = HAL_GPIO_PULL_UP;
            break;
        case PULL_DOWN:
            asf_pull = HAL_GPIO_PULL_DOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }
    hal_gpio_pull_set(self->pin->port, self->pin->pin, asf_pull);
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
        digitalio_digitalinout_obj_t* self) {
    uint32_t pin = self->pin->pin;
    if (self->output) {
        mp_raise_AttributeError("Cannot get pull while in output mode");
        return PULL_NONE;
    } else {
        hal_gpio_pull_t pull = hal_gpio_pull_get(self->pin->port, pin);

        switch(pull)
        {
          case HAL_GPIO_PULL_UP:
          return PULL_UP;

          case HAL_GPIO_PULL_DOWN:
          return PULL_DOWN;

          default: return PULL_NONE;
        }
    }
}
