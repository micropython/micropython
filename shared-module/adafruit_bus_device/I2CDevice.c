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

#include "shared-bindings/adafruit_bus_device/I2CDevice.h"
#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "lib/utils/interrupt_char.h"

void common_hal_adafruit_bus_device_i2cdevice_construct(adafruit_bus_device_i2cdevice_obj_t *self, busio_i2c_obj_t *i2c, uint8_t device_address) {
    self->i2c = i2c;
    self->device_address = device_address;
}

void common_hal_adafruit_bus_device_i2cdevice_lock(adafruit_bus_device_i2cdevice_obj_t *self) {
    bool success = common_hal_busio_i2c_try_lock(self->i2c);

    while (!success) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        success = common_hal_busio_i2c_try_lock(self->i2c);
    }
}

void common_hal_adafruit_bus_device_i2cdevice_unlock(adafruit_bus_device_i2cdevice_obj_t *self) {
    common_hal_busio_i2c_unlock(self->i2c);
}

uint8_t common_hal_adafruit_bus_device_i2cdevice_readinto(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t buffer, size_t length) {
    return common_hal_busio_i2c_read(self->i2c, self->device_address, buffer, length);
}

uint8_t common_hal_adafruit_bus_device_i2cdevice_write(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t buffer, size_t length) {
    return common_hal_busio_i2c_write(self->i2c, self->device_address, buffer, length, true);
}

void common_hal_adafruit_bus_device_i2cdevice_probe_for_device(adafruit_bus_device_i2cdevice_obj_t *self) {
    common_hal_adafruit_bus_device_i2cdevice_lock(self);

    mp_buffer_info_t bufinfo;
    mp_obj_t buffer = mp_obj_new_bytearray_of_zeros(1);

    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_WRITE);

    uint8_t status = common_hal_adafruit_bus_device_i2cdevice_readinto(self, (uint8_t*)bufinfo.buf, 1);
    if (status != 0) {
        common_hal_adafruit_bus_device_i2cdevice_unlock(self);
        mp_raise_ValueError_varg(translate("No I2C device at address: %x"), self->device_address);
    }

    common_hal_adafruit_bus_device_i2cdevice_unlock(self);
}
