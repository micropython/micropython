/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "ble_drv.h"
#include "ble.h"
#include "py/runtime.h"
#include "common-hal/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Adapter.h"


STATIC void _indicate_service_change(uint16_t start, uint16_t end) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        uint16_t conn_handle = connection->conn_handle;
        if (connection->conn_handle == BLE_CONN_HANDLE_INVALID) {
            continue;
        }

        sd_ble_gatts_service_changed(conn_handle, start, end);
    }
}

uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list) {
    self->handle = 0xFFFF;
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->connection = NULL;
    self->is_secondary = is_secondary;

    ble_uuid_t nordic_uuid;
    bleio_uuid_convert_to_nrf_ble_uuid(uuid, &nordic_uuid);

    uint8_t service_type = BLE_GATTS_SRVC_TYPE_PRIMARY;
    if (is_secondary) {
        service_type = BLE_GATTS_SRVC_TYPE_SECONDARY;
    }

    uint32_t result = sd_ble_gatts_service_add(service_type, &nordic_uuid, &self->handle);
    // Do a service changed indication to all connected peers.
    if (result == NRF_SUCCESS) {
        _indicate_service_change(self->handle, self->handle);
    }

    return result;
}

void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary) {
    check_nrf_error(_common_hal_bleio_service_construct(self, uuid, is_secondary,
        mp_obj_new_list(0, NULL)));
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

STATIC void _expand_range(uint16_t new_value, uint16_t *start, uint16_t *end) {
    if (new_value == 0) {
        return;
    }
    *start = MIN(*start, new_value);
    *end = MAX(*end, new_value);
}

void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    ble_gatts_char_md_t char_md = {
        .char_props.broadcast = (characteristic->props & CHAR_PROP_BROADCAST) ? 1 : 0,
        .char_props.read = (characteristic->props & CHAR_PROP_READ) ? 1 : 0,
        .char_props.write_wo_resp = (characteristic->props & CHAR_PROP_WRITE_NO_RESPONSE) ? 1 : 0,
        .char_props.write = (characteristic->props & CHAR_PROP_WRITE) ? 1 : 0,
        .char_props.notify = (characteristic->props & CHAR_PROP_NOTIFY) ? 1 : 0,
        .char_props.indicate = (characteristic->props & CHAR_PROP_INDICATE) ? 1 : 0,
    };

    ble_gatts_attr_md_t cccd_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
    };

    ble_uuid_t char_uuid;
    bleio_uuid_convert_to_nrf_ble_uuid(characteristic->uuid, &char_uuid);

    ble_gatts_attr_md_t char_attr_md = {
        .vloc = BLE_GATTS_VLOC_STACK,
        .vlen = !characteristic->fixed_length,
    };

    if (char_md.char_props.notify || char_md.char_props.indicate) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        // Make CCCD write permission match characteristic read permission.
        bleio_attribute_gatts_set_security_mode(&cccd_md.write_perm, characteristic->read_perm);

        char_md.p_cccd_md = &cccd_md;
    }

    ble_gatts_attr_md_t user_desc_md;
    if (user_description != NULL && strlen(user_description) > 0) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&user_desc_md.read_perm);
        // If the description is on the Python heap, then have the SD copy it. If not, assume it's
        // static and will live for longer than the SD.
        user_desc_md.vloc = gc_nbytes(user_description) > 0 ? BLE_GATTS_VLOC_STACK : BLE_GATTS_VLOC_USER;
        char_md.p_user_desc_md = &user_desc_md;
        char_md.p_char_user_desc = (const uint8_t *)user_description;
        char_md.char_user_desc_max_size = strlen(user_description);
        char_md.char_user_desc_size = strlen(user_description);
    }

    bleio_attribute_gatts_set_security_mode(&char_attr_md.read_perm, characteristic->read_perm);
    bleio_attribute_gatts_set_security_mode(&char_attr_md.write_perm, characteristic->write_perm);
    #if CIRCUITPY_VERBOSE_BLE
    // Turn on read authorization so that we receive an event to print on every read.
    char_attr_md.rd_auth = true;
    #endif

    ble_gatts_attr_t char_attr = {
        .p_uuid = &char_uuid,
        .p_attr_md = &char_attr_md,
        .init_len = characteristic->initial_value_len,
        .p_value = (uint8_t *)characteristic->initial_value,
        .init_offs = 0,
        .max_len = characteristic->max_length,
    };

    ble_gatts_char_handles_t char_handles;

    check_nrf_error(sd_ble_gatts_characteristic_add(self->handle, &char_md, &char_attr, &char_handles));

    characteristic->user_desc_handle = char_handles.user_desc_handle;
    characteristic->cccd_handle = char_handles.cccd_handle;
    characteristic->sccd_handle = char_handles.sccd_handle;
    characteristic->handle = char_handles.value_handle;

    // Indicate that the attribute table has changed.
    uint16_t start = char_handles.value_handle;
    uint16_t end = char_handles.value_handle;
    _expand_range(char_handles.cccd_handle, &start, &end);
    _expand_range(char_handles.sccd_handle, &start, &end);
    _expand_range(char_handles.user_desc_handle, &start, &end);
    _indicate_service_change(start, end);

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "Char handle %x user %x cccd %x sccd %x\n", characteristic->handle, characteristic->user_desc_handle, characteristic->cccd_handle, characteristic->sccd_handle);
    #endif

    mp_obj_list_append(self->characteristic_list, MP_OBJ_FROM_PTR(characteristic));
}
