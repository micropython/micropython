// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "sdk/drivers/igpio/fsl_gpio.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 5U

void pin_config(const mcu_pin_obj_t *pin, bool open_drain, digitalio_pull_t pull) {
    IOMUXC_SetPinConfig(0, 0, 0, 0, pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_PUS((pull == PULL_UP) ? 2 : 0)
        #if IMXRT10XX
        | IOMUXC_SW_PAD_CTL_PAD_HYS(1)
        | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
        | IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
        #endif
        | IOMUXC_SW_PAD_CTL_PAD_PUE(pull != PULL_NONE)
        | IOMUXC_SW_PAD_CTL_PAD_ODE(open_drain)
        | IOMUXC_SW_PAD_CTL_PAD_DSE(1)
        | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;
    self->output = false;
    self->open_drain = false;
    self->pull = PULL_NONE;

    // GPIO is always IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 until proven otherwise
    IOMUXC_SetPinMux(pin->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);

    pin_config(pin, self->open_drain, self->pull);

    const gpio_pin_config_t config = { kGPIO_DigitalInput, 0, kGPIO_NoIntmode };
    GPIO_PinInit(self->pin->gpio, self->pin->number, &config);

    return DIGITALINOUT_OK;
}

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    common_hal_never_reset_pin(self->pin);
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
    self->output = false;

    // This also sets direction to input.
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    return DIGITALINOUT_OK;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    self->output = true;
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    self->pull = PULL_NONE;

    pin_config(self->pin, self->open_drain, self->pull);

    const gpio_pin_config_t config = { kGPIO_DigitalOutput, value, kGPIO_NoIntmode };
    GPIO_PinInit(self->pin->gpio, self->pin->number, &config);
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    return self->output ? DIRECTION_OUTPUT : DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    GPIO_Type *gpio = self->pin->gpio;
    if (value) {
        gpio->DR_SET = 1 << self->pin->number;
    } else {
        gpio->DR_CLEAR = 1 << self->pin->number;
    }
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    return GPIO_PinRead(self->pin->gpio, self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;

    pin_config(self->pin, self->open_drain, self->pull);

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
    self->pull = pull;

    pin_config(self->pin, self->open_drain, self->pull);

    const gpio_pin_config_t config = { kGPIO_DigitalInput, 0, kGPIO_NoIntmode };
    GPIO_PinInit(self->pin->gpio, self->pin->number, &config);
    return DIGITALINOUT_OK;
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    if (self->output) {
        mp_raise_AttributeError(MP_ERROR_TEXT("Cannot get pull while in output mode"));
        return PULL_NONE;
    } else {
        return self->pull;
    }
}
