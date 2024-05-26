// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/fourwire/FourWire.h"

#include <stdint.h>

#include "py/gc.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"

void common_hal_fourwire_fourwire_construct(fourwire_fourwire_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *reset, uint32_t baudrate,
    uint8_t polarity, uint8_t phase) {

    self->bus = spi;
    common_hal_busio_spi_never_reset(self->bus);

    self->frequency = baudrate;
    self->polarity = polarity;
    self->phase = phase;


    self->command.base.type = &mp_type_NoneType;
    if (command != NULL) {
        self->command.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->command, command);
        common_hal_digitalio_digitalinout_switch_to_output(&self->command, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(command);
    }
    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(reset);
        common_hal_fourwire_fourwire_reset(self);
    }

    self->chip_select.base.type = &mp_type_NoneType;
    if (chip_select != NULL) {
        self->chip_select.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
        common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(chip_select);
    }

}

void common_hal_fourwire_fourwire_deinit(fourwire_fourwire_obj_t *self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_spi_deinit(self->bus);
    }

    common_hal_reset_pin(self->command.pin);
    common_hal_reset_pin(self->chip_select.pin);
    common_hal_reset_pin(self->reset.pin);
}

bool common_hal_fourwire_fourwire_reset(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }
    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(1000);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    common_hal_mcu_delay_us(1000);
    return true;
}

bool common_hal_fourwire_fourwire_bus_free(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_unlock(self->bus);
    return true;
}

bool common_hal_fourwire_fourwire_begin_transaction(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_configure(self->bus, self->frequency, self->polarity,
        self->phase, 8);
    if (self->chip_select.base.type != &mp_type_NoneType) {
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    }
    return true;
}

void common_hal_fourwire_fourwire_send(mp_obj_t obj, display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->command.base.type == &mp_type_NoneType) {
        // When the data/command pin is not specified, we simulate a 9-bit SPI mode, by
        // adding a data/command bit to every byte, and then splitting the resulting data back
        // into 8-bit chunks for transmission. If the length of the data being transmitted
        // is not a multiple of 8, there will be additional bits at the end of the
        // transmission. We toggle the CS pin to make the receiver discard them.
        uint8_t buffer = 0;
        uint8_t bits = 0;
        uint8_t dc = (data_type == DISPLAY_DATA);

        for (size_t i = 0; i < data_length; i++) {
            bits = (bits + 1) % 8;

            if (bits == 0) {
                // send the previous byte and the dc bit
                // we will send the current byte later
                buffer = (buffer << 1) | dc;
                common_hal_busio_spi_write(self->bus, &buffer, 1);
                // send the current byte, because previous byte already filled all bits
                common_hal_busio_spi_write(self->bus, &data[i], 1);
            } else {
                // send remaining bits from previous byte, dc and beginning of current byte
                buffer = (buffer << (9 - bits)) | (dc << (8 - bits)) | (data[i] >> bits);
                common_hal_busio_spi_write(self->bus, &buffer, 1);
            }
            // save the current byte
            buffer = data[i];
        }
        // send any remaining bits
        if (bits > 0) {
            buffer = buffer << (8 - bits);
            common_hal_busio_spi_write(self->bus, &buffer, 1);
            if (self->chip_select.base.type != &mp_type_NoneType) {
                // toggle CS to discard superfluous bits
                common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
                common_hal_mcu_delay_us(1);
                common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
            }
        }
    } else {
        common_hal_digitalio_digitalinout_set_value(&self->command, data_type == DISPLAY_DATA);
        if (chip_select == CHIP_SELECT_TOGGLE_EVERY_BYTE) {
            // Toggle chip select after each command byte in case the display driver
            // IC latches commands based on it.
            for (size_t i = 0; i < data_length; i++) {
                common_hal_busio_spi_write(self->bus, &data[i], 1);
                if (self->chip_select.base.type != &mp_type_NoneType) {
                    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
                    common_hal_mcu_delay_us(1);
                    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
                }
            }
        } else {
            common_hal_busio_spi_write(self->bus, data, data_length);
        }
    }
}

void common_hal_fourwire_fourwire_end_transaction(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->chip_select.base.type != &mp_type_NoneType) {
        common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    }
    common_hal_busio_spi_unlock(self->bus);
}

void common_hal_fourwire_fourwire_collect_ptrs(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    gc_collect_ptr((void *)self->bus);
}
