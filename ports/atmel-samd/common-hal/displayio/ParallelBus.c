/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/displayio/ParallelBus.h"

#include <stdint.h>

#include "common-hal/microcontroller/Pin.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "tick.h"

void common_hal_displayio_parallelbus_construct(displayio_parallelbus_obj_t* self,
    const mcu_pin_obj_t* data0, const mcu_pin_obj_t* command,
    const mcu_pin_obj_t* chip_select, const mcu_pin_obj_t* reset, const mcu_pin_obj_t* write) {

    uint8_t data_pin = data0->number;
    if (data_pin % 8 != 0 || data_pin % 32 >= 24) {
        mp_raise_ValueError(translate("Data 0 pin must be byte aligned"));
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (!pin_number_is_free(data_pin + i)) {
            mp_raise_ValueError_varg(translate("Bus pin %d is already in use"), i);
        }
    }
    PortGroup *const g = &PORT->Group[data0->number % 32];
    g->DIRSET.reg = 0xff << data_pin;
    self->bus = ((uint8_t*) &g->OUT.reg) + (data0->number % 32 / 8);

    self->command.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);

    self->chip_select.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    self->reset.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->reset, reset);
    common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);

    self->write.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->write, write);
    common_hal_digitalio_digitalinout_switch_to_output(&self->write, true, DRIVE_MODE_PUSH_PULL);

    never_reset_pin_number(command->number);
    never_reset_pin_number(chip_select->number);
    never_reset_pin_number(reset->number);
    never_reset_pin_number(write->number);
    for (uint8_t i = 0; i < 8; i++) {
        never_reset_pin_number(data_pin + i);
    }
}

bool common_hal_displayio_parallelbus_begin_transaction(mp_obj_t obj) {
    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_parallelbus_send(mp_obj_t obj, bool command, uint8_t *data, uint32_t data_length) {
    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->command, !command);
    for (uint32_t i = 0; i < data_length; i++) {
        common_hal_digitalio_digitalinout_set_value(&self->write, false);
        *self->bus = data[i];
        common_hal_digitalio_digitalinout_set_value(&self->write, true);
    }
}

void common_hal_displayio_parallelbus_end_transaction(mp_obj_t obj) {
    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
}
