/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/microcontroller/__init__.h"
#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/rp2pio/StateMachine.h"

static const uint16_t parallel_program[] = {
// .side_set 1
// .wrap_target
    0x6008, // out pins, 8 side 0
    0xB042  // nop         side 1
// .wrap
};

void common_hal_displayio_parallelbus_construct(displayio_parallelbus_obj_t* self,
    const mcu_pin_obj_t* data0, const mcu_pin_obj_t* command, const mcu_pin_obj_t* chip_select,
    const mcu_pin_obj_t* write, const mcu_pin_obj_t* read, const mcu_pin_obj_t* reset) {

    uint8_t data_pin = data0->number;
    for (uint8_t i = 0; i < 8; i++) {
        if (!pin_number_is_free(data_pin + i)) {
            mp_raise_ValueError_varg(translate("Bus pin %d is already in use"), i);
        }
    }

    uint8_t write_pin = write->number;
    if (!pin_number_is_free(write_pin)) {
        mp_raise_ValueError_varg(translate("Bus pin %d is already in use"), write_pin);
    }

    self->command.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);

    self->chip_select.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    self->read.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->read, read);
    common_hal_digitalio_digitalinout_switch_to_output(&self->read, true, DRIVE_MODE_PUSH_PULL);

    self->data0_pin = data_pin;
    self->write = write_pin;

    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        never_reset_pin_number(reset->number);
        common_hal_displayio_parallelbus_reset(self);
    }

    never_reset_pin_number(command->number);
    never_reset_pin_number(chip_select->number);
    never_reset_pin_number(write_pin);
    never_reset_pin_number(read->number);
    for (uint8_t i = 0; i < 8; i++) {
        never_reset_pin_number(data_pin + i);
    }

    // Calculate pin usage all data pins + write pin
    uint32_t pin_usage = 0;
    for (uint8_t pin_number = data_pin; pin_number < data_pin+8; pin_number++) {
        pin_usage += (1 << pin_number);
    }
    pin_usage += (1 << write_pin);

    bool ok = rp2pio_statemachine_construct(&self->state_machine,
        parallel_program, sizeof(parallel_program) / sizeof(parallel_program[0]),
        60000000, //48000000, //125000000, // freq 24Mhz
        NULL, 0, // init
        data0, 8, // first out pin, # out pins
        NULL, 0, // first in pin, # in pins
        NULL, 0, // first set pin
        write, 1, // first sideset pin
        pin_usage, // pins we use
        true, // tx fifo
        false, // rx fifo
        true, 8, true, // TX, auto pull every 8 bits. shift left to output msb first
        false, 32, true, // RX setting we don't use
        false); // claim pins
    if (!ok) {
        // Do nothing. Maybe bitbang?
        return;
    }
}

void common_hal_displayio_parallelbus_deinit(displayio_parallelbus_obj_t* self) {
    for (uint8_t i = 0; i < 8; i++) {
        reset_pin_number(self->data0_pin + i);
    }

    reset_pin_number(self->command.pin->number);
    reset_pin_number(self->chip_select.pin->number);
    reset_pin_number(self->write);
    reset_pin_number(self->read.pin->number);
    reset_pin_number(self->reset.pin->number);
}

bool common_hal_displayio_parallelbus_reset(mp_obj_t obj) {
	displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(4);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    return true;
}

bool common_hal_displayio_parallelbus_bus_free(mp_obj_t obj) {
    return true;
}

bool common_hal_displayio_parallelbus_begin_transaction(mp_obj_t obj) {
    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
        display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {

    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);

    common_hal_digitalio_digitalinout_set_value(&self->command, byte_type == DISPLAY_DATA);
    common_hal_rp2pio_statemachine_write(&self->state_machine, data, data_length);
}

void common_hal_displayio_parallelbus_end_transaction(mp_obj_t obj) {
    displayio_parallelbus_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
}
