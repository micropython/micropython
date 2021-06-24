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

#include "ble.h"
#include "ble_drv.h"
#include "nrf_error.h"

// If uuid128 is NULL, this is a Bluetooth SIG 16-bit UUID.
// If uuid128 is not NULL, it's a 128-bit (16-byte) UUID, with bytes 12 and 13 zero'd out, where
// the 16-bit part goes. Those 16 bits are passed in uuid16.
void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, mp_int_t uuid16, const uint8_t uuid128[16]) {
    self->nrf_ble_uuid.uuid = uuid16;
    if (uuid128 == NULL) {
        self->nrf_ble_uuid.type = BLE_UUID_TYPE_BLE;
    } else {
        ble_uuid128_t vs_uuid;
        memcpy(vs_uuid.uuid128, uuid128, sizeof(vs_uuid.uuid128));

        // Register this vendor-specific UUID. Bytes 12 and 13 will be zero.
        check_nrf_error(sd_ble_uuid_vs_add(&vs_uuid, &self->nrf_ble_uuid.type));
    }
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->nrf_ble_uuid.type == BLE_UUID_TYPE_BLE ? 16 : 128;
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->nrf_ble_uuid.uuid;
}

void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self, uint8_t uuid128[16]) {
    uint8_t length;
    check_nrf_error(sd_ble_uuid_encode(&self->nrf_ble_uuid, &length, uuid128));
}

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf) {
    if (self->nrf_ble_uuid.type == BLE_UUID_TYPE_BLE) {
        buf[0] = self->nrf_ble_uuid.uuid & 0xff;
        buf[1] = self->nrf_ble_uuid.uuid >> 8;
    } else {
        common_hal_bleio_uuid_get_uuid128(self, buf);
    }
}

void bleio_uuid_construct_from_nrf_ble_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_ble_uuid) {
    if (nrf_ble_uuid->type == BLE_UUID_TYPE_UNKNOWN) {
        mp_raise_bleio_BluetoothError(translate("Unexpected nrfx uuid type"));
    }
    self->nrf_ble_uuid.uuid = nrf_ble_uuid->uuid;
    self->nrf_ble_uuid.type = nrf_ble_uuid->type;
}

// Fill in a ble_uuid_t from my values.
void bleio_uuid_convert_to_nrf_ble_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_ble_uuid) {
    nrf_ble_uuid->uuid = self->nrf_ble_uuid.uuid;
    nrf_ble_uuid->type = self->nrf_ble_uuid.type;
}
