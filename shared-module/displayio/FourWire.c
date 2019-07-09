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

#include "shared-bindings/displayio/FourWire.h"

#include <stdint.h>

#include "py/gc.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"

#include "tick.h"

void common_hal_displayio_fourwire_construct(displayio_fourwire_obj_t* self,
    busio_spi_obj_t* spi, const mcu_pin_obj_t* command,
    const mcu_pin_obj_t* chip_select, const mcu_pin_obj_t* reset, uint32_t baudrate) {

    self->bus = spi;
    common_hal_busio_spi_never_reset(self->bus);
    // Our object is statically allocated off the heap so make sure the bus object lives to the end
    // of the heap as well.
    gc_never_free(self->bus);

    self->frequency = baudrate;
    self->polarity = common_hal_busio_spi_get_polarity(spi);
    self->phase = common_hal_busio_spi_get_phase(spi);

    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    if (reset != NULL) {
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        never_reset_pin_number(reset->number);
    }

    never_reset_pin_number(command->number);
    never_reset_pin_number(chip_select->number);
}

void common_hal_displayio_fourwire_deinit(displayio_fourwire_obj_t* self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_spi_deinit(self->bus);
    }

    reset_pin_number(self->command.pin->number);
    reset_pin_number(self->chip_select.pin->number);
    reset_pin_number(self->reset.pin->number);
}

bool common_hal_displayio_fourwire_begin_transaction(mp_obj_t obj) {
    displayio_fourwire_obj_t* self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_configure(self->bus, self->frequency, self->polarity,
                                   self->phase, 8);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_fourwire_send(mp_obj_t obj, bool command, uint8_t *data, uint32_t data_length) {
    displayio_fourwire_obj_t* self = MP_OBJ_TO_PTR(obj);
    if (command) {
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
        common_hal_mcu_delay_us(1);
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    }
    common_hal_digitalio_digitalinout_set_value(&self->command, !command);
    common_hal_busio_spi_write(self->bus, data, data_length);
}

void common_hal_displayio_fourwire_end_transaction(mp_obj_t obj) {
    displayio_fourwire_obj_t* self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    common_hal_busio_spi_unlock(self->bus);
}
