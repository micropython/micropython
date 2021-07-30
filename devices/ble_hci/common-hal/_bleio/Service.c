/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Adapter.h"

uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list) {
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->connection = NULL;
    self->is_secondary = is_secondary;

    vm_used_ble = true;

    self->handle = bleio_adapter_add_attribute(&common_hal_bleio_adapter_obj, MP_OBJ_TO_PTR(self));
    self->start_handle = self->handle;
    self->end_handle = self->handle;
    if (self->handle == BLE_GATT_HANDLE_INVALID) {
        return 1;
    }
    return 0;
}

void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary) {
    if (_common_hal_bleio_service_construct(self, uuid, is_secondary,
        mp_obj_new_list(0, NULL)) != 0) {
        mp_raise_RuntimeError(translate("Failed to add service"));
    }
}

void bleio_service_from_connection(bleio_service_obj_t *self, mp_obj_t connection) {
    self->handle = 0xFFFF;
    self->uuid = NULL;
    self->characteristic_list = mp_obj_new_list(0, NULL);
    self->is_remote = true;
    self->is_secondary = false;
    self->connection = connection;
}

bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self) {
    return self->uuid;
}

mp_obj_tuple_t *common_hal_bleio_service_get_characteristics(bleio_service_obj_t *self) {
    return mp_obj_new_tuple(self->characteristic_list->len, self->characteristic_list->items);
}

bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self) {
    return self->is_remote;
}

bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self) {
    return self->is_secondary;
}

void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {

    if (self->handle != common_hal_bleio_adapter_obj.last_added_service_handle) {
        mp_raise_bleio_BluetoothError(
            translate("Characteristic can only be added to most recently added service"));
    }
    characteristic->decl_handle = bleio_adapter_add_attribute(
        &common_hal_bleio_adapter_obj, MP_OBJ_TO_PTR(characteristic));
    // This is the value handle.
    characteristic->handle = bleio_adapter_add_attribute(
        &common_hal_bleio_adapter_obj, MP_OBJ_TO_PTR(characteristic));

    self->end_handle = characteristic->handle;

    if (characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) {
        // We need a CCCD if this characteristic is doing notify or indicate.
        bleio_descriptor_obj_t *cccd = m_new_obj(bleio_descriptor_obj_t);
        cccd->base.type = &bleio_descriptor_type;

        uint16_t zero = 0;
        mp_buffer_info_t zero_cccd_value = {
            .buf = &zero,
            .len = sizeof(zero),
        };

        common_hal_bleio_descriptor_construct(
            cccd,
            characteristic,
            &cccd_uuid,                // 0x2902
            SECURITY_MODE_OPEN,        // CCCD read perm
            characteristic->read_perm, // Make CCCD write perm match characteristic read perm.
            2,                         // 2 bytes
            true,                      // fixed length
            &zero_cccd_value           // Initial value is 0.
            );

        // Adds CCCD to attribute table, and also extends self->end_handle to include the CCCD.
        common_hal_bleio_characteristic_add_descriptor(characteristic, cccd);
        characteristic->cccd = cccd;
    }

    mp_obj_list_append(self->characteristic_list, MP_OBJ_FROM_PTR(characteristic));
}
