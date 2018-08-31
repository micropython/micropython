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

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "nrf_gpio.h"

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
        digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    self->pin = pin;

    nrf_gpio_cfg_input(pin->number, NRF_GPIO_PIN_NOPULL);

    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == mp_const_none;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    nrf_gpio_cfg_default(self->pin->number);

    self->pin = mp_const_none;
}

void common_hal_digitalio_digitalinout_switch_to_input(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    nrf_gpio_cfg_input(self->pin->number, NRF_GPIO_PIN_NOPULL);
    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

void common_hal_digitalio_digitalinout_switch_to_output(
        digitalio_digitalinout_obj_t *self, bool value,
        digitalio_drive_mode_t drive_mode) {
    self->open_drain = (drive_mode == DRIVE_MODE_OPEN_DRAIN);

    nrf_gpio_cfg_input(self->pin->number, NRF_GPIO_PIN_NOPULL);

    common_hal_digitalio_digitalinout_set_value(self, value);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
        digitalio_digitalinout_obj_t *self) {
    return (nrf_gpio_pin_dir_get(self->pin->number) == NRF_GPIO_PIN_DIR_OUTPUT)
        ? DIRECTION_OUTPUT : DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
        digitalio_digitalinout_obj_t *self, bool value) {
    if (value && self->open_drain) {
        nrf_gpio_pin_dir_set(self->pin->number, NRF_GPIO_PIN_DIR_INPUT);
    } else {
        nrf_gpio_pin_dir_set(self->pin->number, NRF_GPIO_PIN_DIR_OUTPUT);
        nrf_gpio_pin_write(self->pin->number, value);
    }
}

bool common_hal_digitalio_digitalinout_get_value(
        digitalio_digitalinout_obj_t *self) {
    if (nrf_gpio_pin_dir_get(self->pin->number) == NRF_GPIO_PIN_DIR_INPUT) {
        if (self->open_drain)
            return true;

        return nrf_gpio_pin_read(self->pin->number);
    }

    return nrf_gpio_pin_out_read(self->pin->number);
}

void common_hal_digitalio_digitalinout_set_drive_mode(
        digitalio_digitalinout_obj_t *self,
        digitalio_drive_mode_t drive_mode) {
    const bool value = common_hal_digitalio_digitalinout_get_value(self);
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;

    // True is implemented differently between modes so reset the value to make
    // sure its correct for the new mode.
    if (value)
        common_hal_digitalio_digitalinout_set_value(self, value);
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
        digitalio_digitalinout_obj_t *self) {
    if (self->open_drain)
        return DRIVE_MODE_OPEN_DRAIN;

    return DRIVE_MODE_PUSH_PULL;
}

void common_hal_digitalio_digitalinout_set_pull(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    nrf_gpio_pin_pull_t hal_pull = NRF_GPIO_PIN_NOPULL;

    switch (pull) {
        case PULL_UP:
            hal_pull = NRF_GPIO_PIN_PULLUP;
            break;
        case PULL_DOWN:
            hal_pull = NRF_GPIO_PIN_PULLDOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }

    nrf_gpio_cfg_input(self->pin->number, hal_pull);
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
        digitalio_digitalinout_obj_t *self) {
    uint32_t pin = self->pin->number;
    // Changes pin to be a relative pin number in port.
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

    if (nrf_gpio_pin_dir_get(pin) == NRF_GPIO_PIN_DIR_OUTPUT) {
        mp_raise_AttributeError(translate("Cannot get pull while in output mode"));
        return PULL_NONE;
    }

    switch (reg->PIN_CNF[pin] & GPIO_PIN_CNF_PULL_Msk) {
        case NRF_GPIO_PIN_PULLUP:
            return PULL_UP;

        case NRF_GPIO_PIN_PULLDOWN:
            return PULL_DOWN;

        default:
            return PULL_NONE;
    }
}
