// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "common-hal/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Adapter.h"

#include "host/ble_gatt.h"

uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list) {
    self->handle = 0xFFFF;
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->connection = NULL;
    self->is_secondary = is_secondary;

    self->service_def.type = is_secondary? BLE_GATT_SVC_TYPE_SECONDARY : BLE_GATT_SVC_TYPE_PRIMARY;
    self->service_def.uuid = &uuid->nimble_ble_uuid.u;
    self->service_def.includes = NULL;
    self->service_def.characteristics = self->chr_defs;
    self->next_svc_type = 0;

    // Don't add the service yet because we don't have any characteristics.
    return 0;
}

void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary) {
    _common_hal_bleio_service_construct(self, uuid, is_secondary,
        mp_obj_new_list(0, NULL));
}

void common_hal_bleio_service_deinit(bleio_service_obj_t *self) {
    // Delete the old version of the service.
    if (self->characteristic_list->len > 1) {
        ble_gatts_delete_svc(&self->uuid->nimble_ble_uuid.u);
    }
    self->service_def.type = 0;
}

void bleio_service_from_connection(bleio_service_obj_t *self, mp_obj_t connection) {
    self->handle = BLEIO_HANDLE_INVALID;
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

// Used by characteristics to update their descriptors.
void bleio_service_readd(bleio_service_obj_t *self) {
    // Delete the old version of the service.
    if (self->characteristic_list->len > 1) {
        ble_gatts_delete_svc(&self->uuid->nimble_ble_uuid.u);
    }
    CHECK_NIMBLE_ERROR(ble_gatts_add_dynamic_svcs(&self->service_def));
}


void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    mp_obj_list_append(self->characteristic_list, MP_OBJ_FROM_PTR(characteristic));

    // Delete the old version of the service.
    if (self->characteristic_list->len > 1) {
        ble_gatts_delete_svc(&self->uuid->nimble_ble_uuid.u);
    }
    size_t i = self->characteristic_list->len - 1;
    self->chr_defs[i].uuid = &characteristic->uuid->nimble_ble_uuid.u;
    self->chr_defs[i].access_cb = bleio_characteristic_access_cb;
    self->chr_defs[i].arg = characteristic;
    self->chr_defs[i].descriptors = characteristic->dsc_defs;
    self->chr_defs[i].flags = characteristic->flags;
    self->chr_defs[i].min_key_size = 16;
    self->chr_defs[i].val_handle = &characteristic->handle;
    self->chr_defs[i].cpfd = NULL;
    self->chr_defs[i + 1].uuid = NULL;
    characteristic->chr_def = &self->chr_defs[i];

    CHECK_NIMBLE_ERROR(ble_gatts_add_dynamic_svcs(&self->service_def));
}
