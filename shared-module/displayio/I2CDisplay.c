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

#include "shared-bindings/displayio/I2CDisplay.h"

#include <stdint.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/display_core.h"

void common_hal_displayio_i2cdisplay_construct(displayio_i2cdisplay_obj_t *self,
    busio_i2c_obj_t *i2c, uint16_t device_address, const mcu_pin_obj_t *reset) {

    // Reset the display before probing
    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(reset);
        common_hal_displayio_i2cdisplay_reset(self);
    }

    // Probe the bus to see if a device acknowledges the given address.
    if (!common_hal_busio_i2c_probe(i2c, device_address)) {
        self->base.type = &mp_type_NoneType;
        mp_raise_ValueError_varg(translate("Unable to find I2C Display at %x"), device_address);
    }

    // Write to the device and return 0 on success or an appropriate error code from mperrno.h
    self->bus = i2c;
    common_hal_busio_i2c_never_reset(self->bus);
    // Our object is statically allocated off the heap so make sure the bus object lives to the end
    // of the heap as well.
    gc_never_free(self->bus);

    self->address = device_address;
}

void common_hal_displayio_i2cdisplay_deinit(displayio_i2cdisplay_obj_t *self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_i2c_deinit(self->bus);
    }

    if (self->reset.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->reset);
    }
}

bool common_hal_displayio_i2cdisplay_reset(mp_obj_t obj) {
    displayio_i2cdisplay_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(4);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    return true;
}

bool common_hal_displayio_i2cdisplay_bus_free(mp_obj_t obj) {
    displayio_i2cdisplay_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_i2c_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_i2c_unlock(self->bus);
    return true;
}

bool common_hal_displayio_i2cdisplay_begin_transaction(mp_obj_t obj) {
    displayio_i2cdisplay_obj_t *self = MP_OBJ_TO_PTR(obj);
    return common_hal_busio_i2c_try_lock(self->bus);
}

void common_hal_displayio_i2cdisplay_send(mp_obj_t obj, display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    displayio_i2cdisplay_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (data_type == DISPLAY_COMMAND) {
        uint8_t command_bytes[2 * data_length];
        for (uint32_t i = 0; i < data_length; i++) {
            command_bytes[2 * i] = 0x80;
            command_bytes[2 * i + 1] = data[i];
        }
        common_hal_busio_i2c_write(self->bus, self->address, command_bytes, 2 * data_length, true);
    } else {
        uint8_t data_bytes[data_length + 1];
        data_bytes[0] = 0x40;
        memcpy(data_bytes + 1, data, data_length);
        common_hal_busio_i2c_write(self->bus, self->address, data_bytes, data_length + 1, true);
    }
}

void common_hal_displayio_i2cdisplay_end_transaction(mp_obj_t obj) {
    displayio_i2cdisplay_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_busio_i2c_unlock(self->bus);
}
