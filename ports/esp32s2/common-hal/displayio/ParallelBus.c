/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

/*
 *  Current pin limitations for ESP32-S2 ParallelBus:
 *   - data0 pin must be byte aligned
 */

void common_hal_displayio_parallelbus_construct(displayio_parallelbus_obj_t *self,
    const mcu_pin_obj_t *data0, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {

    uint8_t data_pin = data0->number;
    if (data_pin % 8 != 0) {
        mp_raise_ValueError(translate("Data 0 pin must be byte aligned."));
    }

    for (uint8_t i = 0; i < 8; i++) {
        if (!pin_number_is_free(data_pin + i)) {
            mp_raise_ValueError_varg(translate("Bus pin %d is already in use"), i);
        }
    }

    gpio_dev_t *g = &GPIO; // this is the GPIO registers, see "extern gpio_dev_t GPIO" from file:gpio_struct.h

    // Setup the pins as "Simple GPIO outputs" see section 19.3.3 of the ESP32-S2 Reference Manual
    // Enable pins with "enable_w1ts"

    for (uint8_t i = 0; i < 8; i++) {
        g->enable_w1ts = (0x1 << (data_pin + i));
        g->func_out_sel_cfg[data_pin + i].val = 256; /* setup output pin for simple GPIO Output, (0x100 = 256) */

    }

    /* From my understanding, there is a limitation of the ESP32-S2 that does not allow single-byte writes
     *  into the GPIO registers.  See section 10.3.3 regarding "non-aligned writes" into the registers.
     */


    if (data_pin < 31) {
        self->bus = (uint32_t *)&g->out;         // pointer to GPIO output register (for pins 0-31)
    } else {
        self->bus = (uint32_t *)&g->out1.val;         // pointer to GPIO output register (for pins >= 32)
    }

    /* SNIP - common setup of command, chip select, write and read pins, same as from SAMD and NRF ports */
    self->command.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);

    self->chip_select.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    self->write.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->write, write);
    common_hal_digitalio_digitalinout_switch_to_output(&self->write, true, DRIVE_MODE_PUSH_PULL);

    self->read.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->read, read);
    common_hal_digitalio_digitalinout_switch_to_output(&self->read, true, DRIVE_MODE_PUSH_PULL);

    self->data0_pin = data_pin;

    if (write->number < 32) {
        self->write_clear_register = (uint32_t *)&g->out_w1tc;
        self->write_set_register = (uint32_t *)&g->out_w1ts;
    } else {
        self->write_clear_register = (uint32_t *)&g->out1_w1tc.val;
        self->write_set_register = (uint32_t *)&g->out1_w1ts.val;
    }

    // Check to see if the data and write pins are on the same register:
    if ((((self->data0_pin < 32) && (write->number < 32))) ||
        (((self->data0_pin > 31) && (write->number > 31)))) {
        self->data_write_same_register = true;         // data pins and write pin are on the same register
    } else {
        self->data_write_same_register = false; // data pins and write pins are on different registers
    }


    self->write_mask = 1 << (write->number % 32); /* the write pin triggers the LCD to latch the data */

    /* SNIP - common setup of the reset pin, same as from SAMD and NRF ports */
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
    never_reset_pin_number(write->number);
    never_reset_pin_number(read->number);
    for (uint8_t i = 0; i < 8; i++) {
        never_reset_pin_number(data_pin + i);
    }

}

void common_hal_displayio_parallelbus_deinit(displayio_parallelbus_obj_t *self) {
    /* SNIP - same as from SAMD and NRF ports */
    for (uint8_t i = 0; i < 8; i++) {
        reset_pin_number(self->data0_pin + i);
    }

    reset_pin_number(self->command.pin->number);
    reset_pin_number(self->chip_select.pin->number);
    reset_pin_number(self->write.pin->number);
    reset_pin_number(self->read.pin->number);
    reset_pin_number(self->reset.pin->number);
}

bool common_hal_displayio_parallelbus_reset(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    displayio_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(4);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    return true;

}

bool common_hal_displayio_parallelbus_bus_free(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    return true;
}

bool common_hal_displayio_parallelbus_begin_transaction(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    displayio_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    displayio_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->command, byte_type == DISPLAY_DATA);

    uint32_t *clear_write = self->write_clear_register;
    uint32_t *set_write = self->write_set_register;

    const uint32_t mask = self->write_mask;

    /* Setup structures for data writing.  The ESP32-S2 port differs from the SAMD and NRF ports
     * because I have not found a way to write a single byte into the ESP32-S2 registers.
     * For the ESP32-S2, I create a 32-bit data_buffer that is used to transfer the data bytes.
     */

    *clear_write = mask; // Clear the write pin to prepare the registers before storing the
    // register value into data_buffer

    const uint32_t data_buffer = *self->bus; // store the initial output register values into the data output buffer
    uint8_t *data_address = ((uint8_t *)&data_buffer) + (self->data0_pin / 8); /* address inside data_buffer where
                                                        * each data byte will be written to the data pin registers
                                                        */


    if (self->data_write_same_register) {   // data and write pins are on the same register
        for (uint32_t i = 0; i < data_length; i++) {

            /* Note: If the write pin and data pins are controlled by the same GPIO register, we can eliminate
                 * the "clear_write" step below, since the write pin is cleared when the data_buffer is written
                 * to the bus.
                 */

            *(data_address) = data[i];     // stuff the data byte into the data_buffer at the correct offset byte location
            *self->bus = data_buffer;     // write the data to the output register
            *set_write = mask;     // set the write pin
        }
    } else {   // data and write pins are on different registers
        for (uint32_t i = 0; i < data_length; i++) {
            *clear_write = mask;             // clear the write pin (See comment above, this may not be necessary).
            *(data_address) = data[i];     // stuff the data byte into the data_buffer at the correct offset byte location
            *self->bus = data_buffer;     // write the data to the output register
            *set_write = mask;     // set the write pin

        }
    }

}

void common_hal_displayio_parallelbus_end_transaction(mp_obj_t obj) {
    /* SNIP - same as from SAMD and NRF ports */
    displayio_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
}
