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
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/display_core.h"

void common_hal_displayio_fourwire_construct(displayio_fourwire_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *reset, uint32_t baudrate,
    uint8_t polarity, uint8_t phase) {

    self->bus = spi;
    common_hal_busio_spi_never_reset(self->bus);
    // Our object is statically allocated off the heap so make sure the bus object lives to the end
    // of the heap as well.
    gc_never_free(self->bus);

    self->frequency = baudrate;
    self->polarity = polarity;
    self->phase = phase;

    common_hal_digitalio_digitalinout_construct(&self->command, command);
    common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);

    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(reset);
        common_hal_displayio_fourwire_reset(self);
    }

    common_hal_never_reset_pin(command);
    common_hal_never_reset_pin(chip_select);
}

void common_hal_displayio_fourwire_deinit(displayio_fourwire_obj_t *self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_spi_deinit(self->bus);
    }

    common_hal_reset_pin(self->command.pin);
    common_hal_reset_pin(self->chip_select.pin);
    common_hal_reset_pin(self->reset.pin);
}

bool common_hal_displayio_fourwire_reset(mp_obj_t obj) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }
    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(1000);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    common_hal_mcu_delay_us(1000);
    return true;
}

bool common_hal_displayio_fourwire_bus_free(mp_obj_t obj) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_unlock(self->bus);
    return true;
}

bool common_hal_displayio_fourwire_begin_transaction(mp_obj_t obj) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_configure(self->bus, self->frequency, self->polarity,
        self->phase, 8);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

void common_hal_displayio_fourwire_send(mp_obj_t obj, display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->command, data_type == DISPLAY_DATA);
    if (chip_select == CHIP_SELECT_TOGGLE_EVERY_BYTE) {
        // Toggle chip select after each command byte in case the display driver
        // IC latches commands based on it.
        for (size_t i = 0; i < data_length; i++) {
            common_hal_busio_spi_write(self->bus, &data[i], 1);
            common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
            common_hal_mcu_delay_us(1);
            common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
        }
    } else {
        common_hal_busio_spi_write(self->bus, data, data_length);
    }
}

void common_hal_displayio_fourwire_end_transaction(mp_obj_t obj) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    common_hal_busio_spi_unlock(self->bus);
}
