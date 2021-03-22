/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include "py/runtime.h"
#include "common-hal/_bleio/UUID.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"

// If uuid128 is NULL, this is a Bluetooth SIG 16-bit UUID.
// If uuid128 is not NULL, it's a 128-bit (16-byte) UUID, with bytes 12 and 13 zero'd out, where
// the 16-bit part goes. Those 16 bits are passed in uuid16.
void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, mp_int_t uuid16, const uint8_t uuid128[16]) {
    self->size = uuid128 == NULL ? 16 : 128;
    self->uuid16 = uuid16;
    if (uuid128) {
        memcpy(self->uuid128, uuid128, 16);
        self->uuid128[12] = uuid16 & 0xff;
        self->uuid128[13] = uuid16 >> 8;
    } else {
        memset(self->uuid128, 0, 16);
    }
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->size;
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->uuid16;
}

void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self, uint8_t uuid128[16]) {
    memcpy(uuid128, self->uuid128, 16);
}

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf) {
    if (self->size == 16) {
        buf[0] = self->uuid16 & 0xff;
        buf[1] = self->uuid16 >> 8;
    } else {
        common_hal_bleio_uuid_get_uuid128(self, buf);
    }
}

// Return a uui16 only if this is a standard uuid. Otherwise return BLE_UUID_UNKNOWN.
uint16_t bleio_uuid_get_uuid16_or_unknown(bleio_uuid_obj_t *uuid) {
    return uuid->size == 16 ? uuid->uuid16 : BLE_UUID_UNKNOWN;
}
