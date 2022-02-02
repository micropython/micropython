/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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

#include <string.h>

#include "py/gc.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/is31fl3741/IS31FL3741.h"
#include "shared-bindings/util.h"
#include "shared-bindings/busio/I2C.h"

void common_hal_is31fl3741_IS31FL3741_construct(is31fl3741_IS31FL3741_obj_t *self, busio_i2c_obj_t *i2c, uint8_t addr) {
    // Probe the bus to see if a device acknowledges the given address.
    if (!common_hal_busio_i2c_probe(i2c, addr)) {
        self->base.type = &mp_type_NoneType;
        mp_raise_ValueError_varg(translate("Unable to find I2C Display at %x"), addr);
    }

    self->i2c = i2c;
    self->device_address = addr;
}

void common_hal_is31fl3741_IS31FL3741_deinit(is31fl3741_IS31FL3741_obj_t *self) {
    common_hal_is31fl3741_end_transaction(self); // in case we still had a lock

    if (self->i2c == &self->inline_i2c) {
        common_hal_busio_i2c_deinit(self->i2c);
        self->i2c = NULL;
    }

    self->base.type = NULL;
}

void common_hal_is31fl3741_write(is31fl3741_IS31FL3741_obj_t *is31, const mp_obj_t *mapping, const uint8_t *pixels, size_t numBytes) {
    common_hal_is31fl3741_begin_transaction(is31);

    for (size_t i = 0; i < numBytes; i += 3) {
        uint16_t ridx = mp_obj_get_int(mapping[i]);
        if (ridx != 65535) {
            common_hal_is31fl3741_set_led(is31, ridx, IS31GammaTable[pixels[i]], 0); // red
            common_hal_is31fl3741_set_led(is31, mp_obj_get_int(mapping[i + 1]), IS31GammaTable[pixels[i + 1]], 0); // green
            common_hal_is31fl3741_set_led(is31, mp_obj_get_int(mapping[i + 2]), IS31GammaTable[pixels[i + 2]], 0); // blue
        }
    }

    common_hal_is31fl3741_end_transaction(is31);
}

void common_hal_is31fl3741_begin_transaction(is31fl3741_IS31FL3741_obj_t *self) {
    while (!common_hal_busio_i2c_try_lock(self->i2c)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
}

void common_hal_is31fl3741_end_transaction(is31fl3741_IS31FL3741_obj_t *self) {
    common_hal_busio_i2c_unlock(self->i2c);
}

uint8_t is31fl3741_cur_page = 99; // set to invalid page to start

void common_hal_is31fl3741_send_unlock(is31fl3741_IS31FL3741_obj_t *self) {
    uint8_t unlock[2] = { 0xFE, 0xC5 }; // unlock command
    common_hal_busio_i2c_write(self->i2c, self->device_address, unlock, 2);
}

void common_hal_is31fl3741_set_page(is31fl3741_IS31FL3741_obj_t *self, uint8_t p) {
    if (p == is31fl3741_cur_page) {
        return;
    }

    is31fl3741_cur_page = p;
    common_hal_is31fl3741_send_unlock(self);

    uint8_t page[2] = { 0xFD, 0x00 }; // page command
    page[1] = p;
    common_hal_busio_i2c_write(self->i2c, self->device_address, page, 2);
}

void common_hal_is31fl3741_send_enable(is31fl3741_IS31FL3741_obj_t *self) {
    common_hal_is31fl3741_set_page(self, 4);
    uint8_t enable[2] = { 0x00, 0x01 }; // enable command
    common_hal_busio_i2c_write(self->i2c, self->device_address, enable, 2);
}

void common_hal_is31fl3741_send_reset(is31fl3741_IS31FL3741_obj_t *self) {
    common_hal_is31fl3741_set_page(self, 4);
    uint8_t rst[2] = { 0x3F, 0xAE }; // reset command
    common_hal_busio_i2c_write(self->i2c, self->device_address, rst, 2);
}

void common_hal_is31fl3741_set_current(is31fl3741_IS31FL3741_obj_t *self, uint8_t current) {
    common_hal_is31fl3741_set_page(self, 4);
    uint8_t gcur[2] = { 0x01, 0x00 }; // global current command
    gcur[1] = current;
    common_hal_busio_i2c_write(self->i2c, self->device_address, gcur, 2);
}

uint8_t common_hal_is31fl3741_get_current(is31fl3741_IS31FL3741_obj_t *self) {
    common_hal_is31fl3741_set_page(self, 4);
    uint8_t gcur = 0x01; // global current command
    common_hal_busio_i2c_write_read(self->i2c, self->device_address, &gcur, 1, &gcur, 1);
    return gcur;
}

void common_hal_is31fl3741_set_led(is31fl3741_IS31FL3741_obj_t *self, uint16_t led, uint8_t level, uint8_t page) {
    uint8_t cmd[2] = { 0x00, 0x00 };

    if (led < 180) {
        common_hal_is31fl3741_set_page(self, page);
        cmd[0] = (uint8_t)led;
    } else {
        common_hal_is31fl3741_set_page(self, page + 1);
        cmd[0] = (uint8_t)(led - 180);
    }

    cmd[1] = level;

    common_hal_busio_i2c_write(self->i2c, self->device_address, cmd, 2);
}

void common_hal_is31fl3741_draw_pixel(is31fl3741_IS31FL3741_obj_t *self, int16_t x, int16_t y, uint32_t color, uint16_t *mapping) {
    uint8_t r = color >> 16 & 0xFF;
    uint8_t g = color >> 8 & 0xFF;
    uint8_t b = color & 0xFF;

    int16_t x1 = (x * 5 + y) * 3;
    uint16_t ridx = mapping[x1 + 2];
    if (ridx != 65535) {
        uint16_t gidx = mapping[x1 + 1];
        uint16_t bidx = mapping[x1 + 0];
        common_hal_is31fl3741_set_led(self, ridx, r, 0);
        common_hal_is31fl3741_set_led(self, gidx, g, 0);
        common_hal_is31fl3741_set_led(self, bidx, b, 0);
    }
}
