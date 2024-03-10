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
#include "common-hal/_bleio/__init__.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"

// List ble service of central device
bleio_service_obj_list bleio_service_list;

uint32_t _common_hal_bleio_service_construct(
    bleio_service_obj_t *self,
    bleio_uuid_obj_t *uuid,
    bool is_secondary,
    mp_obj_list_t *characteristic_list) {

    uint8_t service_type;
    sl_status_t sc = SL_STATUS_FAIL;
    uint16_t gattdb_session;
    sl_bt_uuid_16_t bt_uuid;

    self->handle = 0xFFFF;
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->connection = NULL;
    self->is_secondary = is_secondary;

    if (self->is_secondary) {
        service_type = sl_bt_gattdb_secondary_service;
    } else {
        service_type = sl_bt_gattdb_primary_service;
    }

    sc = sl_bt_gattdb_new_session(&gattdb_session);
    if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Create new session fail."));
        return sc;
    }
    if (BLE_UUID_TYPE_16 == self->uuid->efr_ble_uuid.uuid.type) {

        bt_uuid.data[0] = self->uuid->efr_ble_uuid.uuid16.value & 0xff;
        bt_uuid.data[1] = self->uuid->efr_ble_uuid.uuid16.value >> 8;
        sc = sl_bt_gattdb_add_service(gattdb_session,
            service_type,
            0, 2, bt_uuid.data,
            (uint16_t *)&self->handle);
    } else if (BLE_UUID_TYPE_128 == self->uuid->efr_ble_uuid.uuid.type) {
        sc = sl_bt_gattdb_add_service(gattdb_session,
            service_type, 0, 16,
            self->uuid->efr_ble_uuid.uuid128.value,
            (uint16_t *)&self->handle);
    }

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Create new session fail."));
        return sc;
    }

    sc = sl_bt_gattdb_start_service(gattdb_session, self->handle);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Start service fail."));
    }

    sc = sl_bt_gattdb_commit(gattdb_session);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Commit service fail."));
    }

    bleio_service_list.data[bleio_service_list.len] = self;
    bleio_service_list.len++;
    return sc;
}

// Create a new Service identified by the specified UUID
void common_hal_bleio_service_construct(bleio_service_obj_t *self,
    bleio_uuid_obj_t *uuid,
    bool is_secondary) {
    _common_hal_bleio_service_construct(self, uuid,
        is_secondary,
        mp_obj_new_list(0, NULL));
}

// Get service from connection
void bleio_service_from_connection(bleio_service_obj_t *self,
    mp_obj_t connection) {
    self->handle = BLEIO_HANDLE_INVALID;
    self->uuid = NULL;
    self->characteristic_list = mp_obj_new_list(0, NULL);
    self->is_remote = true;
    self->is_secondary = false;
    self->connection = connection;
}

// Get service uuid
bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self) {
    return self->uuid;
}

// Get tuple charateristic of service
mp_obj_tuple_t *common_hal_bleio_service_get_characteristics(
    bleio_service_obj_t *self) {
    return mp_obj_new_tuple(self->characteristic_list->len,
        self->characteristic_list->items);
}

// This is a service provided by a remote device or not
bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self) {
    return self->is_remote;
}

// If the service is a secondary one
bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self) {
    return self->is_secondary;
}

// Add new dynamic characteristic to service
void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {

    bool broadcast = (characteristic->props & CHAR_PROP_BROADCAST) ? 1 : 0;
    bool read = (characteristic->props & CHAR_PROP_READ) ? 1 : 0;
    bool write_wo_resp =
        (characteristic->props & CHAR_PROP_WRITE_NO_RESPONSE) ? 1 : 0;
    bool write =
        write_wo_resp ? 1 : (characteristic->props & CHAR_PROP_WRITE) ? 1
                                                                      : 0;
    bool notify = (characteristic->props & CHAR_PROP_NOTIFY) ? 1 : 0;
    bool indicate = (characteristic->props & CHAR_PROP_INDICATE) ? 1 : 0;

    sl_status_t sc = SL_STATUS_FAIL;
    sl_bt_uuid_16_t bt_uuid;
    uint16_t gattdb_session;

    sc = sl_bt_gattdb_new_session(&gattdb_session);

    if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Create new session fail."));
        return;
    }
    characteristic->props = (broadcast << 0) | (read << 1) |
        (write_wo_resp << 2) | (write << 3) | (notify << 4) | (indicate << 5);

    if (BLE_UUID_TYPE_16 == characteristic->uuid->efr_ble_uuid.uuid.type) {
        bt_uuid.data[0] = characteristic->uuid->efr_ble_uuid.uuid16.value & 0xff;
        bt_uuid.data[1] = characteristic->uuid->efr_ble_uuid.uuid16.value >> 8;

        sc = sl_bt_gattdb_add_uuid16_characteristic(
            gattdb_session,
            self->handle,
            characteristic->props,
            0,
            0,
            bt_uuid,
            sl_bt_gattdb_variable_length_value,
            characteristic->max_length,
            0,
            initial_value_bufinfo->buf,
            &characteristic->handle);

    } else if (BLE_UUID_TYPE_128 ==
               characteristic->uuid->efr_ble_uuid.uuid.type) {
        uuid_128 uuid;
        memcpy(uuid.data, characteristic->uuid->efr_ble_uuid.uuid128.value, 16);

        sc = sl_bt_gattdb_add_uuid128_characteristic(gattdb_session,
            self->handle,
            characteristic->props,
            0,
            0,
            uuid,
            sl_bt_gattdb_variable_length_value,
            characteristic->max_length,
            0,
            initial_value_bufinfo->buf,
            &characteristic->handle);
    }

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Add charateristic fail."));
    }

    sc = sl_bt_gattdb_start_characteristic(gattdb_session,
        characteristic->handle);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Start charateristic fail."));
        return;
    }

    sc = sl_bt_gattdb_commit(gattdb_session);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Commit charateristic fail."));
        return;
    }
    mp_obj_list_append(self->characteristic_list,
        MP_OBJ_FROM_PTR(characteristic));
}

// Remove dynamic service when reload
void reset_dynamic_service() {

    uint16_t gattdb_session;
    uint8_t svc_index;
    // Remove dynamic service
    for (svc_index = 0; svc_index < bleio_service_list.len; svc_index++) {
        sl_bt_gattdb_new_session(&gattdb_session);
        sl_bt_gattdb_remove_service(gattdb_session,
            bleio_service_list.data[svc_index]->handle);
        sl_bt_gattdb_commit(gattdb_session);
    }
    bleio_service_list.len = 0;
}
