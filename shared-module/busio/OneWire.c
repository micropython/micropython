/*
 * This file is part of the Micro Python project, http://micropython.org/
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

// Wraps the bitbangio implementation of OneWire for use in busio.
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/OneWire.h"
#include "shared-module/busio/OneWire.h"

void common_hal_busio_onewire_construct(busio_onewire_obj_t *self,
    const mcu_pin_obj_t *pin) {
    shared_module_bitbangio_onewire_construct(&self->bitbang, pin);
}

bool common_hal_busio_onewire_deinited(busio_onewire_obj_t *self) {
    return shared_module_bitbangio_onewire_deinited(&self->bitbang);
}

void common_hal_busio_onewire_deinit(busio_onewire_obj_t *self) {
    if (common_hal_busio_onewire_deinited(self)) {
        return;
    }
    shared_module_bitbangio_onewire_deinit(&self->bitbang);
}

bool common_hal_busio_onewire_reset(busio_onewire_obj_t *self) {
    return shared_module_bitbangio_onewire_reset(&self->bitbang);
}

bool common_hal_busio_onewire_read_bit(busio_onewire_obj_t *self) {
    return shared_module_bitbangio_onewire_read_bit(&self->bitbang);
}

void common_hal_busio_onewire_write_bit(busio_onewire_obj_t *self,
    bool bit) {
    shared_module_bitbangio_onewire_write_bit(&self->bitbang, bit);
}
