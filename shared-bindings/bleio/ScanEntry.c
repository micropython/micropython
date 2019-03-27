/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "shared-bindings/bleio/Address.h"
#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/UUID.h"
#include "shared-module/bleio/AdvertisementData.h"
#include "shared-module/bleio/ScanEntry.h"

// Work-in-progress: orphaned for now.
//| :orphan:
//|
//| .. currentmodule:: bleio
//|
//| :class:`ScanEntry` -- BLE scan response entry
//| =========================================================
//|
//| Encapsulates information about a device that was received as a
//| response to a BLE scan request.
//|

//|   .. attribute:: address
//|
//|   The address of the device. (read-only)
//|   This attribute is of type `bleio.Address`.
//|

//|   .. attribute:: manufacturer_specific_data
//|
//|   The manufacturer-specific data present in the advertisement packet. (read-only)
//|

//|   .. attribute:: name
//|
//|   The name of the device. (read-only)
//|   This attribute might be `None` if the data was missing from the advertisement packet.
//|

//|   .. attribute:: raw_data
//|
//|   All the advertisement data present in the packet. (read-only)
//|

//|   .. attribute:: rssi
//|
//|   The signal strength of the device at the time of the scan. (read-only)
//|

//|   .. attribute:: service_uuids
//|
//|   The address of the device. (read-only)
//|   This attribute is a list of `bleio.UUID`.
//|   This attribute might be empty or incomplete, depending on the advertisement packet.
//|   Currently only 16-bit UUIDS are listed.
//|

//|   .. attribute:: tx_power_level
//|
//|   The transmit power level of the device. (read-only)
//|   This attribute might be `None` if the data was missing from the advertisement packet.
//|
static uint8_t find_data_item(mp_obj_array_t *data_in, uint8_t type, uint8_t **data_out) {
    uint16_t i = 0;
    while (i < data_in->len) {
        const uint8_t item_len  = ((uint8_t*)data_in->items)[i];
        const uint8_t item_type = ((uint8_t*)data_in->items)[i + 1];
        if (item_type != type) {
            i += (item_len + 1);
            continue;
        }

        *data_out = &((uint8_t*)data_in->items)[i + 2];

        return item_len;
    }

    return 0;
}

STATIC mp_obj_t scanentry_get_name(mp_obj_t self_in);

STATIC void bleio_scanentry_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_scanentry_obj_t *self = (bleio_scanentry_obj_t *)self_in;
    mp_printf(print, "ScanEntry(address: %02x:%02x:%02x:%02x:%02x:%02x",
        self->address.value[5], self->address.value[4], self->address.value[3],
        self->address.value[1], self->address.value[1], self->address.value[0]);

    const mp_obj_t name_obj = scanentry_get_name(self_in);
    if (name_obj != mp_const_none) {
        mp_obj_str_t *str = MP_OBJ_TO_PTR(name_obj);
        mp_printf(print, " name: %s", str->data);
    }

    mp_print_str(print, ")");
}

STATIC mp_obj_t bleio_scanentry_get_address(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t obj = bleio_address_type.make_new(&bleio_address_type, 1, 0, (mp_obj_t)&mp_const_none_obj);
    bleio_address_obj_t *address = MP_OBJ_TO_PTR(obj);

    address->type = self->address.type;
    memcpy(address->value, self->address.value, BLEIO_ADDRESS_BYTES);

    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scanentry_get_address_obj, bleio_scanentry_get_address);

const mp_obj_property_t bleio_scanentry_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bluepy_scanentry_get_address_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_manufacturer_specific_data(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_array_t *data = MP_OBJ_TO_PTR(self->data);
    uint8_t *manuf_data;

    const uint8_t manuf_data_len = find_data_item(data, AdManufacturerSpecificData, &manuf_data);
    if (manuf_data_len == 0) {
        return mp_const_none;
    }

    return mp_obj_new_bytearray_by_ref(manuf_data_len, manuf_data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(scanentry_get_manufacturer_specific_data_obj, scanentry_get_manufacturer_specific_data);

const mp_obj_property_t bleio_scanentry_manufacturer_specific_data_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&scanentry_get_manufacturer_specific_data_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_name(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_array_t *data = MP_OBJ_TO_PTR(self->data);
    uint8_t *name;

    // Try for Complete but settle for Shortened
    uint8_t name_len = find_data_item(data, AdCompleteLocalName, &name);
    if (name_len == 0) {
        name_len = find_data_item(data, AdShortenedLocalName, &name);
    }

    if (name_len == 0) {
        return mp_const_none;
    }

    return mp_obj_new_str((const char*)name, name_len - 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scanentry_get_name_obj, scanentry_get_name);

const mp_obj_property_t bleio_scanentry_name_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bluepy_scanentry_get_name_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_raw_data(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t entries = mp_obj_new_list(0, NULL);

    mp_obj_array_t *data = MP_OBJ_TO_PTR(self->data);

    uint16_t i = 0;
    while (i < data->len) {
        mp_obj_tuple_t *entry = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));

        const uint8_t item_len  = ((uint8_t*)data->items)[i];
        const uint8_t item_type = ((uint8_t*)data->items)[i + 1];

        entry->items[0] = MP_OBJ_NEW_SMALL_INT(item_type);
        entry->items[1] = mp_obj_new_bytearray(item_len - 1, &((uint8_t*)data->items)[i + 2]);
        mp_obj_list_append(entries, MP_OBJ_FROM_PTR(entry));

        i += (item_len + 1);
    }

    return entries;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_raw_data_obj, scanentry_get_raw_data);

const mp_obj_property_t bleio_scanentry_raw_data_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_raw_data_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_rssi(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_int(self->rssi);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scanentry_get_rssi_obj, scanentry_get_rssi);

const mp_obj_property_t bleio_scanentry_rssi_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bluepy_scanentry_get_rssi_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_service_uuids(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_array_t *data = MP_OBJ_TO_PTR(self->data);
    uint8_t *uuids;

    // Try for Complete but settle for Incomplete
    uint8_t uuids_len = find_data_item(data, AdCompleteListOf16BitServiceClassUUIDs, &uuids);
    if (uuids_len == 0) {
        uuids_len = find_data_item(data, AdIncompleteListOf16BitServiceClassUUIDs, &uuids);
    }

    mp_obj_t entries = mp_obj_new_list(0, NULL);
    for (size_t i = 0; i < uuids_len / sizeof(uint16_t); ++i) {
        const mp_obj_t uuid_int = mp_obj_new_int(uuids[sizeof(uint16_t) * i] | (uuids[sizeof(uint16_t) * i + 1] << 8));
        const mp_obj_t uuid_obj = bleio_uuid_type.make_new(&bleio_uuid_type, 1, &uuid_int, NULL);

        mp_obj_list_append(entries, uuid_obj);
    }

    // TODO: 32-bit UUIDs
    // TODO: 128-bit UUIDs

    return entries;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(scanentry_get_service_uuids_obj, scanentry_get_service_uuids);

const mp_obj_property_t bleio_scanentry_service_uuids_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&scanentry_get_service_uuids_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t scanentry_get_tx_power_level(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_array_t *data = MP_OBJ_TO_PTR(self->data);
    uint8_t *tx_power;

    const uint8_t tx_power_len = find_data_item(data, AdTxPowerLevel, &tx_power);
    if (tx_power_len == 0) {
        return mp_const_none;
    }

    return mp_obj_new_int((int8_t)(*tx_power));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(scanentry_get_tx_power_level_obj, scanentry_get_tx_power_level);

const mp_obj_property_t bleio_scanentry_tx_power_level_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&scanentry_get_tx_power_level_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_scanentry_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_address),                     MP_ROM_PTR(&bleio_scanentry_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_manufacturer_specific_data),  MP_ROM_PTR(&bleio_scanentry_manufacturer_specific_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_name),                        MP_ROM_PTR(&bleio_scanentry_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_raw_data),                    MP_ROM_PTR(&bleio_scanentry_raw_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_rssi),                        MP_ROM_PTR(&bleio_scanentry_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_service_uuids),               MP_ROM_PTR(&bleio_scanentry_service_uuids_obj) },
    { MP_ROM_QSTR(MP_QSTR_tx_power_level),              MP_ROM_PTR(&bleio_scanentry_tx_power_level_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_scanentry_locals_dict, bleio_scanentry_locals_dict_table);

const mp_obj_type_t bleio_scanentry_type = {
    { &mp_type_type },
    .name = MP_QSTR_ScanEntry,
    .print = bleio_scanentry_print,
    .locals_dict = (mp_obj_dict_t*)&bleio_scanentry_locals_dict
};
