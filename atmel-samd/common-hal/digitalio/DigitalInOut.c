/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "asf/sam0/drivers/port/port.h"
#include "asf/sam0/drivers/system/pinmux/pinmux.h"

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
        digitalio_digitalinout_obj_t* self, const mcu_pin_obj_t* pin) {
    claim_pin(pin);
    self->pin = pin;

    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    pin_conf.direction  = PORT_PIN_DIR_INPUT;
    pin_conf.input_pull = PORT_PIN_PULL_NONE;
    port_pin_set_config(self->pin->pin, &pin_conf);
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
        digitalio_digitalinout_obj_t* self, enum digitalio_pull_t pull) {
    self->output = false;

    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

void common_hal_digitalio_digitalinout_switch_to_output(
        digitalio_digitalinout_obj_t* self, bool value,
        enum digitalio_drive_mode_t drive_mode) {
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    pin_conf.direction  = PORT_PIN_DIR_INPUT;
    pin_conf.input_pull = PORT_PIN_PULL_NONE;
    port_pin_set_config(self->pin->pin, &pin_conf);

    self->output = true;
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    common_hal_digitalio_digitalinout_set_value(self, value);
}

enum digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
        digitalio_digitalinout_obj_t* self) {
    return self->output? DIRECTION_OUTPUT : DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
        digitalio_digitalinout_obj_t* self, bool value) {
    uint32_t pin = self->pin->pin;
    PortGroup *const port_base = port_get_group_from_gpio_pin(pin);
    uint32_t pin_mask  = (1UL << (pin % 32));

    /* Set the pin to high or low atomically based on the requested level */
    if (value) {
        if (self->open_drain) {
            port_base->DIRCLR.reg = pin_mask;
        } else {
            port_base->DIRSET.reg = pin_mask;
            port_base->OUTSET.reg = pin_mask;
        }
    } else {
        port_base->DIRSET.reg = pin_mask;
        port_base->OUTCLR.reg = pin_mask;
    }
}

bool common_hal_digitalio_digitalinout_get_value(
        digitalio_digitalinout_obj_t* self) {
    uint32_t pin = self->pin->pin;
    PortGroup *const port_base = port_get_group_from_gpio_pin(pin);
    uint32_t pin_mask  = (1UL << (pin % 32));
    if (!self->output) {
        return (port_base->IN.reg & pin_mask);
    } else {
        if (self->open_drain && (port_base->DIR.reg & pin_mask) == 0) {
            return true;
        } else {
            return (port_base->OUT.reg & pin_mask);
        }
    }
}

void common_hal_digitalio_digitalinout_set_drive_mode(
        digitalio_digitalinout_obj_t* self,
        enum digitalio_drive_mode_t drive_mode) {
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    self->open_drain = drive_mode == DRIVE_MODE_OPEN_DRAIN;
    // True is implemented differently between modes so reset the value to make
    // sure its correct for the new mode.
    if (value) {
        common_hal_digitalio_digitalinout_set_value(self, value);
    }
}

enum digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
        digitalio_digitalinout_obj_t* self) {
    if (self->open_drain) {
        return DRIVE_MODE_OPEN_DRAIN;
    } else {
        return DRIVE_MODE_PUSH_PULL;
    }
}

void common_hal_digitalio_digitalinout_set_pull(
        digitalio_digitalinout_obj_t* self, enum digitalio_pull_t pull) {
    enum port_pin_pull asf_pull = PORT_PIN_PULL_NONE;
    switch (pull) {
        case PULL_UP:
            asf_pull = PORT_PIN_PULL_UP;
            break;
        case PULL_DOWN:
            asf_pull = PORT_PIN_PULL_DOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    pin_conf.direction  = PORT_PIN_DIR_INPUT;
    pin_conf.input_pull = asf_pull;
    port_pin_set_config(self->pin->pin, &pin_conf);
}

enum digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
        digitalio_digitalinout_obj_t* self) {
    uint32_t pin = self->pin->pin;
    PortGroup *const port_base = port_get_group_from_gpio_pin(pin);
    uint32_t pin_mask  = (1UL << (pin % 32));
    if (self->output) {
        mp_raise_AttributeError("Cannot get pull while in output mode");
        return PULL_NONE;
    } else {
        if (port_base->PINCFG[pin % 32].bit.PULLEN == 0) {
            return PULL_NONE;
        } if ((port_base->OUT.reg & pin_mask) > 0) {
            return PULL_UP;
        } else {
            return PULL_DOWN;
        }
    }
}
