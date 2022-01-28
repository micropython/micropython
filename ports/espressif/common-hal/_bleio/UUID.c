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
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"

// If uuid128 is NULL, this is a Bluetooth SIG 16-bit UUID.
// If uuid128 is not NULL, it's a 128-bit (16-byte) UUID, with bytes 12 and 13 zero'd out, where
// the 16-bit part goes. Those 16 bits are passed in uuid16.
void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, mp_int_t uuid16, const uint8_t uuid128[16]) {
    if (uuid128 == NULL) {
        ble_uuid_init_from_buf(&self->nimble_ble_uuid, (uint8_t *)&uuid16, 2);
    } else {
        ble_uuid_init_from_buf(&self->nimble_ble_uuid, uuid128, 16);
        self->nimble_ble_uuid.u128.value[12] = uuid16 & 0xff;
        self->nimble_ble_uuid.u128.value[13] = (uuid16 >> 8) & 0xff;
    }
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->nimble_ble_uuid.u.type == BLE_UUID_TYPE_16 ? 16 : 128;
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->nimble_ble_uuid.u16.value;
}

void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self, uint8_t uuid128[16]) {
    memcpy(uuid128, self->nimble_ble_uuid.u128.value, 16);
}

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf) {
    if (self->nimble_ble_uuid.u.type == BLE_UUID_TYPE_16) {
        buf[0] = self->nimble_ble_uuid.u16.value & 0xff;
        buf[1] = self->nimble_ble_uuid.u16.value >> 8;
    } else {
        common_hal_bleio_uuid_get_uuid128(self, buf);
    }
}
