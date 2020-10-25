/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/busdevice/I2CDevice.h"
#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/nlr.h"

void common_hal_busdevice_i2cdevice_construct(busdevice_i2cdevice_obj_t *self, busio_i2c_obj_t *i2c, uint8_t device_address, bool probe) {
    self->i2c = i2c;
    self->device_address = device_address;
    self->probe = probe;

    if (self->probe == true) {
        common_hal_busdevice_i2cdevice___probe_for_device(self);
    }
}

void common_hal_busdevice_i2cdevice_lock(busdevice_i2cdevice_obj_t *self) {
    bool success = false;
    while (!success) {
        success = common_hal_busio_i2c_try_lock(self->i2c);
    }
}

void common_hal_busdevice_i2cdevice_unlock(busdevice_i2cdevice_obj_t *self) {
    common_hal_busio_i2c_unlock(self->i2c);
}

uint8_t common_hal_busdevice_i2cdevice_readinto(busdevice_i2cdevice_obj_t *self, mp_obj_t buffer, size_t length) {
    uint8_t status = common_hal_busio_i2c_read(self->i2c, self->device_address, buffer, length);

    return status;
}

uint8_t common_hal_busdevice_i2cdevice_write(busdevice_i2cdevice_obj_t *self, mp_obj_t buffer, size_t length) {
    uint8_t status = common_hal_busio_i2c_write(self->i2c, self->device_address, buffer, length, true);

    return status;
}

uint8_t common_hal_busdevice_i2cdevice_write_then_readinto(busdevice_i2cdevice_obj_t *self, mp_obj_t out_buffer, mp_obj_t in_buffer,
                    size_t out_length, size_t in_length) {
    uint8_t status = 0;

    status = common_hal_busio_i2c_write(self->i2c, self->device_address, out_buffer, out_length, true);

    status = common_hal_busio_i2c_read(self->i2c, self->device_address, in_buffer, in_length);

    return status;
}

uint8_t common_hal_busdevice_i2cdevice___probe_for_device(busdevice_i2cdevice_obj_t *self) {




    // write ""


/*
            while not self.i2c.try_lock():
            pass
        try:
            self.i2c.writeto(self.device_address, b"")
        except OSError:
            # some OS's dont like writing an empty bytesting...
            # Retry by reading a byte
            try:
                result = bytearray(1)
                self.i2c.readfrom_into(self.device_address, result)
            except OSError:
                raise ValueError("No I2C device at address: %x" % self.device_address)
        finally:
            self.i2c.unlock()
*/
    return 0;
}
