/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-module/_bleio/ScanEntry.h"

//| class ScanEntry:
//|     """Encapsulates information about a device that was received during scanning. It can be
//|     advertisement or scan response data. This object may only be created by a `_bleio.ScanResults`:
//|     it has no user-visible constructor."""
//|

//|     def __init__(self) -> None:
//|         """Cannot be instantiated directly. Use `_bleio.Adapter.start_scan`."""
//|         ...
//|
//|     def matches(self, prefixes: ScanEntry, *, all: bool = True) -> bool:
//|         """Returns True if the ScanEntry matches all prefixes when ``all`` is True. This is stricter
//|         than the scan filtering which accepts any advertisements that match any of the prefixes
//|         where all is False."""
//|         ...
//|
STATIC mp_obj_t bleio_scanentry_matches(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_prefixes, ARG_all };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_prefixes, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_all, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_prefixes].u_obj, &bufinfo, MP_BUFFER_READ);
    return mp_obj_new_bool(common_hal_bleio_scanentry_matches(self, bufinfo.buf, bufinfo.len, args[ARG_all].u_bool));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_scanentry_matches_obj, 2, bleio_scanentry_matches);

//|     address: Address
//|     """The address of the device (read-only), of type `_bleio.Address`."""
//|
STATIC mp_obj_t bleio_scanentry_get_address(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_bleio_scanentry_get_address(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_address_obj, bleio_scanentry_get_address);

const mp_obj_property_t bleio_scanentry_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_address_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     advertisement_bytes: bytes
//|     """All the advertisement data present in the packet, returned as a ``bytes`` object. (read-only)"""
//|
STATIC mp_obj_t scanentry_get_advertisement_bytes(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_bleio_scanentry_get_advertisement_bytes(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_advertisement_bytes_obj, scanentry_get_advertisement_bytes);

const mp_obj_property_t bleio_scanentry_advertisement_bytes_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_advertisement_bytes_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     rssi: int
//|     """The signal strength of the device at the time of the scan, in integer dBm. (read-only)"""
//|
STATIC mp_obj_t scanentry_get_rssi(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_bleio_scanentry_get_rssi(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_rssi_obj, scanentry_get_rssi);

const mp_obj_property_t bleio_scanentry_rssi_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_rssi_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     connectable: bool
//|     """True if the device can be connected to. (read-only)"""
//|
STATIC mp_obj_t scanentry_get_connectable(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_bleio_scanentry_get_connectable(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_connectable_obj, scanentry_get_connectable);

const mp_obj_property_t bleio_scanentry_connectable_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_connectable_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     scan_response: bool
//|     """True if the entry was a scan response. (read-only)"""
//|
STATIC mp_obj_t scanentry_get_scan_response(mp_obj_t self_in) {
    bleio_scanentry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_bleio_scanentry_get_scan_response(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_scanentry_get_scan_response_obj, scanentry_get_scan_response);

const mp_obj_property_t bleio_scanentry_scan_response_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_scanentry_get_scan_response_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

STATIC const mp_rom_map_elem_t bleio_scanentry_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_address),             MP_ROM_PTR(&bleio_scanentry_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_advertisement_bytes), MP_ROM_PTR(&bleio_scanentry_advertisement_bytes_obj) },
    { MP_ROM_QSTR(MP_QSTR_rssi),                MP_ROM_PTR(&bleio_scanentry_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_connectable),         MP_ROM_PTR(&bleio_scanentry_connectable_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan_response),       MP_ROM_PTR(&bleio_scanentry_scan_response_obj) },
    { MP_ROM_QSTR(MP_QSTR_matches),             MP_ROM_PTR(&bleio_scanentry_matches_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_scanentry_locals_dict, bleio_scanentry_locals_dict_table);

const mp_obj_type_t bleio_scanentry_type = {
    { &mp_type_type },
    .name = MP_QSTR_ScanEntry,
    .locals_dict = (mp_obj_dict_t *)&bleio_scanentry_locals_dict
};
