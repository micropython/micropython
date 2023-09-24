/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "supervisor/shared/translate/translate.h"

// Never reset pin when reload
void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    common_hal_never_reset_pin(self->pin);
}

// Construct Digitalio obj
digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    common_hal_mcu_pin_claim(pin);
    self->pin = pin;
    GPIO_PinModeSet(pin->port, pin->number, gpioModeInput, 1);
    return DIGITALINOUT_OK;
}

// Check Digitalio status, deinited or not
bool common_hal_digitalio_digitalinout_deinited(
    digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

// Deinit Digitalio obj
void common_hal_digitalio_digitalinout_deinit(
    digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

//  Switch pin to input
digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    if (pull == PULL_NONE) {
        GPIO_PinModeSet(self->pin->port, self->pin->number, gpioModeInput, 1);
    } else if (pull == PULL_UP) {
        GPIO_PinModeSet(self->pin->port, self->pin->number, gpioModeInputPull, 1);
    } else {
        GPIO_PinModeSet(self->pin->port, self->pin->number, gpioModeInputPull, 0);
    }
    return DIGITALINOUT_OK;
}

// Switch pin to output
digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        GPIO_PinModeSet(self->pin->port, self->pin->number,
            gpioModeWiredAnd, value);
    } else {
        GPIO_PinModeSet(self->pin->port, self->pin->number,
            gpioModePushPull, value);
    }

    if (value) {
        GPIO_PinOutSet(self->pin->port, self->pin->number);
    } else {
        GPIO_PinOutClear(self->pin->port, self->pin->number);
    }

    return DIGITALINOUT_OK;
}

// Get direction of the pin
digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    GPIO_Mode_TypeDef mode = GPIO_PinModeGet(self->pin->port, self->pin->number);
    if (mode >= gpioModePushPull) {
        return DIRECTION_OUTPUT;
    }
    return DIRECTION_INPUT;
}

// Set digital logic level of the pin
void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    if (value) {
        GPIO_PinOutSet(self->pin->port, self->pin->number);
    } else {
        GPIO_PinOutClear(self->pin->port, self->pin->number);
    }
}

//  The digital logic level of the pin
bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_get_direction(self)
        == DIRECTION_OUTPUT) {
        return GPIO_PinOutGet(self->pin->port, self->pin->number);
    }
    return GPIO_PinInGet(self->pin->port, self->pin->number);
}

// Set drive mode
digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        GPIO_PinModeSet(self->pin->port, self->pin->number, gpioModeWiredAnd, 1);
    } else {
        GPIO_PinModeSet(self->pin->port, self->pin->number, gpioModePushPull, 1);
    }
    return DIGITALINOUT_OK;
}

// Get drive mode
digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    GPIO_Mode_TypeDef mode = GPIO_PinModeGet(self->pin->port, self->pin->number);
    if (mode >= gpioModeWiredAnd) {
        return DRIVE_MODE_OPEN_DRAIN;
    }
    return DRIVE_MODE_PUSH_PULL;
}

// Set pull direction
digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    return DIGITALINOUT_OK;
}

// Get pull direction
digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    return PULL_NONE;
}
