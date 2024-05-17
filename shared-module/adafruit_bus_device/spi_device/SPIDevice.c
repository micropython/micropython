// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Komus
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/adafruit_bus_device/spi_device/SPIDevice.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

void common_hal_adafruit_bus_device_spidevice_construct(adafruit_bus_device_spidevice_obj_t *self, busio_spi_obj_t *spi,  digitalio_digitalinout_obj_t *cs,
    bool cs_active_value, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t extra_clocks) {
    self->spi = spi;
    self->baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->extra_clocks = extra_clocks;
    // May be mp_const_none if CS not used.
    self->chip_select = cs;
    self->cs_active_value = cs_active_value;
}

mp_obj_t common_hal_adafruit_bus_device_spidevice_enter(adafruit_bus_device_spidevice_obj_t *self) {
    {
        mp_obj_t dest[2];
        mp_load_method(self->spi, MP_QSTR_try_lock, dest);

        while (!mp_obj_is_true(mp_call_method_n_kw(0, 0, dest))) {
            RUN_BACKGROUND_TASKS;
            // Break out on ctrl-C.
            if (mp_hal_is_interrupted()) {
                mp_handle_pending(true);
            }
        }
    }

    {
        mp_obj_t dest[10];
        mp_load_method(self->spi, MP_QSTR_configure, dest);
        dest[2] = MP_OBJ_NEW_QSTR(MP_QSTR_baudrate);
        dest[3] = MP_OBJ_NEW_SMALL_INT(self->baudrate);
        dest[4] = MP_OBJ_NEW_QSTR(MP_QSTR_polarity);
        dest[5] = MP_OBJ_NEW_SMALL_INT(self->polarity);
        dest[6] = MP_OBJ_NEW_QSTR(MP_QSTR_phase);
        dest[7] = MP_OBJ_NEW_SMALL_INT(self->phase);
        dest[8] = MP_OBJ_NEW_QSTR(MP_QSTR_bits);
        dest[9] = MP_OBJ_NEW_SMALL_INT(8);
        mp_call_method_n_kw(0, 4, dest);
    }

    if (self->chip_select != mp_const_none) {
        common_hal_digitalio_digitalinout_set_value(MP_OBJ_TO_PTR(self->chip_select), self->cs_active_value);
    }
    return self->spi;
}

void common_hal_adafruit_bus_device_spidevice_exit(adafruit_bus_device_spidevice_obj_t *self) {
    if (self->chip_select != mp_const_none) {
        common_hal_digitalio_digitalinout_set_value(MP_OBJ_TO_PTR(self->chip_select), !(self->cs_active_value));
    }

    if (self->extra_clocks > 0) {

        mp_buffer_info_t bufinfo;
        mp_obj_t buffer = mp_obj_new_bytearray_of_zeros(1);

        mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_WRITE);
        ((uint8_t *)bufinfo.buf)[0] = 0xFF;

        uint8_t clocks = (self->extra_clocks + 7) / 8;

        mp_obj_t dest[3];
        mp_load_method(self->spi, MP_QSTR_write, dest);
        dest[2] = buffer;
        while (clocks > 0) {
            mp_call_method_n_kw(1, 0, dest);
            clocks--;
        }
    }

    mp_obj_t dest[2];
    mp_load_method(self->spi, MP_QSTR_unlock, dest);
    mp_call_method_n_kw(0, 0, dest);
}
