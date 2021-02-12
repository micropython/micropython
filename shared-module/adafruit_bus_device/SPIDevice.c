/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Komus
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

#include "shared-bindings/adafruit_bus_device/SPIDevice.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"

void common_hal_adafruit_bus_device_spidevice_construct(adafruit_bus_device_spidevice_obj_t *self, busio_spi_obj_t *spi,  digitalio_digitalinout_obj_t *cs,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t extra_clocks) {
    self->spi = spi;
    self->baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->extra_clocks = extra_clocks;
    self->chip_select = cs;
}

mp_obj_t common_hal_adafruit_bus_device_spidevice_enter(adafruit_bus_device_spidevice_obj_t *self) {
    bool success = false;
    while (!success) {
        success = common_hal_busio_spi_try_lock(self->spi);
        RUN_BACKGROUND_TASKS;
        mp_handle_pending();
    }

    common_hal_busio_spi_configure(self->spi, self->baudrate, self->polarity, self->phase, 8);

    if (self->chip_select != MP_OBJ_NULL) {
        common_hal_digitalio_digitalinout_set_value(MP_OBJ_TO_PTR(self->chip_select), false);
    }
    return self->spi;
}

void common_hal_adafruit_bus_device_spidevice_exit(adafruit_bus_device_spidevice_obj_t *self) {
    if (self->chip_select != MP_OBJ_NULL) {
        common_hal_digitalio_digitalinout_set_value(MP_OBJ_TO_PTR(self->chip_select), true);
    }

    if (self->extra_clocks > 0) {

        mp_buffer_info_t bufinfo;
        mp_obj_t buffer = mp_obj_new_bytearray_of_zeros(1);

        mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_READ);
        ((uint8_t*)bufinfo.buf)[0]  = 0xFF;

        uint8_t clocks = self->extra_clocks / 8;
        if ((self->extra_clocks % 8) != 0)
            clocks += 1;

        while (clocks > 0) {
            if (!common_hal_busio_spi_write(self->spi, ((uint8_t*)bufinfo.buf), 1)) {
                mp_raise_OSError(MP_EIO);
            }
            clocks--;
        }
    }

    common_hal_busio_spi_unlock(self->spi);
}
