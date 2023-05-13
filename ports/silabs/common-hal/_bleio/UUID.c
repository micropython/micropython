/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/_bleio/__init__.h"
#include "common-hal/_bleio/UUID.h"

void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self,
    mp_int_t uuid16,
    const uint8_t uuid128[16]) {

    if (uuid128 == NULL) {
        self->efr_ble_uuid.uuid16.value = uuid16;
        self->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_16;
    } else {
        memcpy(self->efr_ble_uuid.uuid128.value, uuid128, 16);
        self->efr_ble_uuid.uuid128.value[12] = uuid16 & 0xff;
        self->efr_ble_uuid.uuid128.value[13] = (uuid16 >> 8) & 0xff;
        self->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_128;
    }
}

uint32_t common_hal_bleio_uuid_get_size(bleio_uuid_obj_t *self) {
    return self->efr_ble_uuid.uuid.type == BLE_UUID_TYPE_16 ? 16 : 128;
}

uint32_t common_hal_bleio_uuid_get_uuid16(bleio_uuid_obj_t *self) {
    return self->efr_ble_uuid.uuid16.value;
}

void common_hal_bleio_uuid_get_uuid128(bleio_uuid_obj_t *self,
    uint8_t uuid128[16]) {
    memcpy(uuid128, self->efr_ble_uuid.uuid128.value, 16);
}

void common_hal_bleio_uuid_pack_into(bleio_uuid_obj_t *self, uint8_t *buf) {
    if (self->efr_ble_uuid.uuid.type == BLE_UUID_TYPE_16) {
        buf[0] = self->efr_ble_uuid.uuid16.value & 0xff;
        buf[1] = self->efr_ble_uuid.uuid16.value >> 8;
    } else {
        common_hal_bleio_uuid_get_uuid128(self, buf);
    }
}

void bleio_uuid_construct_from_efr_ble_uuid(bleio_uuid_obj_t *self,
    ble_uuid_any_t *efr_ble_uuid) {

    if (self->efr_ble_uuid.uuid.type == BLE_UUID_TYPE_16) {
        mp_raise_bleio_BluetoothError(translate("Unexpected efr uuid type"));
    }
    self->efr_ble_uuid.uuid16.value = efr_ble_uuid->uuid16.value;
}

void bleio_uuid_convert_to_efr_ble_uuid(bleio_uuid_obj_t *self,
    ble_uuid_any_t *efr_ble_uuid) {
    efr_ble_uuid->uuid16.value = self->efr_ble_uuid.uuid16.value;
}
