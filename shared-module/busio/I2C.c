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

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/bitbangio/I2C.h"
#include "py/mperrno.h"
#include "py/nlr.h"

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t freq, uint32_t timeout) {
    shared_module_bitbangio_i2c_construct(&self->bitbang, scl, sda, freq, timeout);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return shared_module_bitbangio_i2c_deinited(&self->bitbang);
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    shared_module_bitbangio_i2c_deinit(&self->bitbang);
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return shared_module_bitbangio_i2c_probe(&self->bitbang, addr);
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    return shared_module_bitbangio_i2c_try_lock(&self->bitbang);
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return shared_module_bitbangio_i2c_has_lock(&self->bitbang);
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    shared_module_bitbangio_i2c_unlock(&self->bitbang);
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    return shared_module_bitbangio_i2c_write(&self->bitbang, addr, data, len,
        transmit_stop_bit);
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    return shared_module_bitbangio_i2c_read(&self->bitbang, addr, data, len);
}
