/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/_bleio/Connection.h"

#include <string.h>
#include <stdio.h>

#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"

//| class Connection:
//|     """A BLE connection to another device. Used to discover and interact with services on the other
//|     device.
//|
//|     Usage::
//|
//|        import _bleio
//|
//|        my_entry = None
//|        for entry in _bleio.adapter.scan(2.5):
//|            if entry.name is not None and entry.name == 'InterestingPeripheral':
//|                my_entry = entry
//|                break
//|
//|        if not my_entry:
//|            raise Exception("'InterestingPeripheral' not found")
//|
//|        connection = _bleio.adapter.connect(my_entry.address, timeout=10)"""
//|

void bleio_connection_ensure_connected(bleio_connection_obj_t *self) {
    if (!common_hal_bleio_connection_get_connected(self)) {
        mp_raise_ConnectionError(translate("Connection has been disconnected and can no longer be used. Create a new connection."));
    }
}

//|     def __init__(self) -> None:
//|         """Connections cannot be made directly. Instead, to initiate a connection use `Adapter.connect`.
//|         Connections may also be made when another device initiates a connection. To use a Connection
//|         created by a peer, read the `Adapter.connections` property."""
//|         ...
//|
//|     def disconnect(self) -> None:
//|         """Disconnects from the remote peripheral. Does nothing if already disconnected."""
//|         ...
//|
STATIC mp_obj_t bleio_connection_disconnect(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // common_hal_bleio_connection_disconnect() does nothing if already disconnected.
    common_hal_bleio_connection_disconnect(self->connection);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_disconnect_obj, bleio_connection_disconnect);


//|     def pair(self, *, bond: bool = True) -> None:
//|         """Pair to the peer to improve security."""
//|         ...
//|
STATIC mp_obj_t bleio_connection_pair(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_bond };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bond, MP_ARG_BOOL, {.u_bool = true} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_connection_ensure_connected(self);

    common_hal_bleio_connection_pair(self->connection, args[ARG_bond].u_bool);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_connection_pair_obj, 1, bleio_connection_pair);

//|     def discover_remote_services(self, service_uuids_whitelist: Optional[Iterable[UUID]] = None) -> Tuple[Service, ...]:
//|         """Do BLE discovery for all services or for the given service UUIDS,
//|         to find their handles and characteristics, and return the discovered services.
//|         `Connection.connected` must be True.
//|
//|         :param iterable service_uuids_whitelist:
//|
//|           an iterable of :py:class:`UUID` objects for the services provided by the peripheral
//|           that you want to use.
//|
//|           The peripheral may provide more services, but services not listed are ignored
//|           and will not be returned.
//|
//|           If service_uuids_whitelist is None, then all services will undergo discovery, which can be
//|           slow.
//|
//|           If the service UUID is 128-bit, or its characteristic UUID's are 128-bit, you
//|           you must have already created a :py:class:`UUID` object for that UUID in order for the
//|           service or characteristic to be discovered. Creating the UUID causes the UUID to be
//|           registered for use. (This restriction may be lifted in the future.)
//|
//|         :return: A tuple of `_bleio.Service` objects provided by the remote peripheral."""
//|         ...
//|
STATIC mp_obj_t bleio_connection_discover_remote_services(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_service_uuids_whitelist };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service_uuids_whitelist, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_connection_ensure_connected(self);

    return MP_OBJ_FROM_PTR(common_hal_bleio_connection_discover_remote_services(
        self,
        args[ARG_service_uuids_whitelist].u_obj));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_connection_discover_remote_services_obj, 1, bleio_connection_discover_remote_services);

//|     connected: bool
//|     """True if connected to the remote peer."""
//|
STATIC mp_obj_t bleio_connection_get_connected(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_connection_get_connected(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_connected_obj, bleio_connection_get_connected);

const mp_obj_property_t bleio_connection_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_connection_get_connected_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};


//|     paired: bool
//|     """True if paired to the remote peer."""
//|
STATIC mp_obj_t bleio_connection_get_paired(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_connection_get_paired(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_paired_obj, bleio_connection_get_paired);

const mp_obj_property_t bleio_connection_paired_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_connection_get_paired_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};


//|     connection_interval: float
//|     """Time between transmissions in milliseconds. Will be multiple of 1.25ms. Lower numbers
//|     increase speed and decrease latency but increase power consumption.
//|
//|     When setting connection_interval, the peer may reject the new interval and
//|     `connection_interval` will then remain the same.
//|
//|     Apple has additional guidelines that dictate should be a multiple of 15ms except if HID is
//|     available. When HID is available Apple devices may accept 11.25ms intervals."""
//|
STATIC mp_obj_t bleio_connection_get_connection_interval(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_connection_ensure_connected(self);
    return mp_obj_new_float(common_hal_bleio_connection_get_connection_interval(self->connection));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_connection_interval_obj, bleio_connection_get_connection_interval);

//|     max_packet_length: int
//|     """The maximum number of data bytes that can be sent in a single transmission,
//|     not including overhead bytes.
//|
//|     This is the maximum number of bytes that can be sent in a notification,
//|     which must be sent in a single packet.
//|     But for a regular characteristic read or write, may be sent in multiple packets,
//|     so this limit does not apply."""
//|
STATIC mp_obj_t bleio_connection_get_max_packet_length(mp_obj_t self_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_connection_ensure_connected(self);
    return mp_obj_new_int(common_hal_bleio_connection_get_max_packet_length(self->connection));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_connection_get_max_packet_length_obj, bleio_connection_get_max_packet_length);


STATIC mp_obj_t bleio_connection_set_connection_interval(mp_obj_t self_in, mp_obj_t interval_in) {
    bleio_connection_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_float_t interval = mp_obj_get_float(interval_in);

    bleio_connection_ensure_connected(self);
    common_hal_bleio_connection_set_connection_interval(self->connection, interval);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_connection_set_connection_interval_obj, bleio_connection_set_connection_interval);

const mp_obj_property_t bleio_connection_connection_interval_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_connection_get_connection_interval_obj,
               (mp_obj_t)&bleio_connection_set_connection_interval_obj,
               MP_ROM_NONE },
};

const mp_obj_property_t bleio_connection_max_packet_length_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_connection_get_max_packet_length_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

STATIC const mp_rom_map_elem_t bleio_connection_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_pair),                     MP_ROM_PTR(&bleio_connection_pair_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),               MP_ROM_PTR(&bleio_connection_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_discover_remote_services), MP_ROM_PTR(&bleio_connection_discover_remote_services_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected),           MP_ROM_PTR(&bleio_connection_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_paired),              MP_ROM_PTR(&bleio_connection_paired_obj) },
    { MP_ROM_QSTR(MP_QSTR_connection_interval), MP_ROM_PTR(&bleio_connection_connection_interval_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_packet_length),   MP_ROM_PTR(&bleio_connection_max_packet_length_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_connection_locals_dict, bleio_connection_locals_dict_table);

const mp_obj_type_t bleio_connection_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Connection,
    .locals_dict = (mp_obj_dict_t *)&bleio_connection_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = mp_generic_unary_op,
        ),
};
