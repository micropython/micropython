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

#include <string.h>
#include <stdio.h>

#include "ble_drv.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Peripheral.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-module/_bleio/ScanEntry.h"

#include "common-hal/_bleio/Peripheral.h"

#define ADV_INTERVAL_MIN (0.0020f)
#define ADV_INTERVAL_MIN_STRING "0.0020"
#define ADV_INTERVAL_MAX (10.24f)
#define ADV_INTERVAL_MAX_STRING "10.24"
// 20ms is recommended by Apple
#define ADV_INTERVAL_DEFAULT (0.1f)

//| .. currentmodule:: _bleio
//|
//| :class:`Peripheral` -- A BLE peripheral device
//| =========================================================
//|
//| Implement a BLE peripheral which runs locally.
//| Set up using the supplied services, and then allow advertising to be started and stopped.
//|
//| Usage::
//|
//|    from _bleio import Characteristic, Peripheral, Service
//|    from adafruit_ble.advertising import ServerAdvertisement
//|
//|    # Create a peripheral and start it up.
//|    peripheral = _bleio.Peripheral()
//|
//|    # Create a Service and add it to this Peripheral.
//|    service = Service.add_to_peripheral(peripheral, _bleio.UUID(0x180f))
//|
//|    # Create a Characteristic and add it to the Service.
//|    characteristic = Characteristic.add_to_service(service,
//|        _bleio.UUID(0x2919), properties=Characteristic.READ | Characteristic.NOTIFY)
//|
//|    adv = ServerAdvertisement(peripheral)
//|    peripheral.start_advertising(adv.advertising_data_bytes, scan_response=adv.scan_response_bytes)
//|
//|    while not peripheral.connected:
//|        # Wait for connection.
//|        pass
//|
//| .. class:: Peripheral(name=None)
//|
//|   Create a new Peripheral object.
//|
//|   :param str name: The name used when advertising this peripheral. If name is None,
//|     _bleio.adapter.default_name will be used.
//|
STATIC mp_obj_t bleio_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_name };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_name, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_peripheral_obj_t *self = m_new_obj(bleio_peripheral_obj_t);
    self->base.type = &bleio_peripheral_type;

    mp_obj_t name = args[ARG_name].u_obj;
    if (name == mp_const_none) {
        name = common_hal_bleio_adapter_get_default_name();
    } else if (!MP_OBJ_IS_STR(name)) {
        mp_raise_ValueError(translate("name must be a string"));
    }

    common_hal_bleio_peripheral_construct(self, name);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: connected (read-only)
//|
//|     True if connected to a BLE Central device.
//|
STATIC mp_obj_t bleio_peripheral_get_connected(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_peripheral_get_connected(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_get_connected_obj, bleio_peripheral_get_connected);

const mp_obj_property_t bleio_peripheral_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_peripheral_get_connected_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: services
//|
//|     A tuple of :py:class:`Service` objects offered by this peripheral. (read-only)
//|
STATIC mp_obj_t bleio_peripheral_get_services(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Return list as a tuple so user won't be able to change it.
    mp_obj_list_t *services_list = common_hal_bleio_peripheral_get_services(self);
    return mp_obj_new_tuple(services_list->len, services_list->items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_get_services_obj, bleio_peripheral_get_services);

const mp_obj_property_t bleio_peripheral_services_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_peripheral_get_services_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: name
//|
//|     The peripheral's name, included when advertising. (read-only)
//|
STATIC mp_obj_t bleio_peripheral_get_name(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_peripheral_get_name(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_get_name_obj, bleio_peripheral_get_name);

const mp_obj_property_t bleio_peripheral_name_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_peripheral_get_name_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. method:: start_advertising(data, *, scan_response=None, connectable=True, interval=0.1)
//|
//|     Starts advertising the peripheral. The peripheral's name and
//|     services are included in the advertisement packets.
//|
//|     :param buf data: advertising data packet bytes
//|     :param buf scan_response: scan response data packet bytes. ``None`` if no scan response is needed.
//|     :param bool connectable:  If `True` then other devices are allowed to connect to this peripheral.
//|     :param float interval:  advertising interval, in seconds
//|
STATIC mp_obj_t bleio_peripheral_start_advertising(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_data, ARG_scan_response, ARG_connectable, ARG_interval };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_scan_response, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_connectable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t data_bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_bufinfo, MP_BUFFER_READ);

    // Pass an empty buffer if scan_response not provided.
    mp_buffer_info_t scan_response_bufinfo = { 0 };
    if (args[ARG_scan_response].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_scan_response].u_obj, &scan_response_bufinfo, MP_BUFFER_READ);
    }

    if (args[ARG_interval].u_obj == MP_OBJ_NULL) {
        args[ARG_interval].u_obj = mp_obj_new_float(ADV_INTERVAL_DEFAULT);
    }

    const mp_float_t interval = mp_obj_float_get(args[ARG_interval].u_obj);
    if (interval < ADV_INTERVAL_MIN || interval > ADV_INTERVAL_MAX) {
        mp_raise_ValueError_varg(translate("interval must be in range %s-%s"),
                                 ADV_INTERVAL_MIN_STRING, ADV_INTERVAL_MAX_STRING);
    }

    common_hal_bleio_peripheral_start_advertising(self, args[ARG_connectable].u_bool, interval,
                                                  &data_bufinfo, &scan_response_bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_peripheral_start_advertising_obj, 2, bleio_peripheral_start_advertising);

//|   .. method:: stop_advertising()
//|
//|     Stop sending advertising packets.
STATIC mp_obj_t bleio_peripheral_stop_advertising(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_peripheral_stop_advertising(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_stop_advertising_obj, bleio_peripheral_stop_advertising);

//|   .. method:: disconnect()
//|
//|     Disconnects from the remote central.
//|     Normally the central initiates a disconnection. Use this only
//|     if necessary for your application.
//|
STATIC mp_obj_t bleio_peripheral_disconnect(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_peripheral_disconnect(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_disconnect_obj, bleio_peripheral_disconnect);

//|   .. method:: discover_remote_services(service_uuids_whitelist=None)
//|      Do BLE discovery for all services or for the given service UUIDS,
//|      to find their handles and characteristics, and return the discovered services.
//|      `Peripheral.connected` must be True.
//|
//|   :param iterable service_uuids_whitelist: an iterable of :py:class:~`UUID` objects for the services
//|      provided by the peripheral that you want to use.
//|      The peripheral may provide more services, but services not listed are ignored
//|      and will not be returned.
//|
//|      If service_uuids_whitelist is None, then all services will undergo discovery, which can be slow.
//|
//|      If the service UUID is 128-bit, or its characteristic UUID's are 128-bit, you
//|      you must have already created a :py:class:~`UUID` object for that UUID in order for the
//|      service or characteristic to be discovered. Creating the UUID causes the UUID to be registered
//|      for use. (This restriction may be lifted in the future.)
//|
//|      Thought it is unusual for a peripheral to act as a BLE client, it can do so, and
//|      needs to be able to do discovery on its peer (a central).
//|      Examples include a peripheral accessing a central that provides Current Time Service,
//|      Apple Notification Center Service, or Battery Service.
//|
//|    :return: A tuple of services provided by the remote central.
//|
STATIC mp_obj_t bleio_peripheral_discover_remote_services(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_service_uuids_whitelist };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_service_uuids_whitelist, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!common_hal_bleio_peripheral_get_connected(self)) {
        mp_raise_ValueError(translate("Not connected"));
    }

    return MP_OBJ_FROM_PTR(common_hal_bleio_peripheral_discover_remote_services(
                               MP_OBJ_FROM_PTR(self),
                               args[ARG_service_uuids_whitelist].u_obj));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_peripheral_discover_remote_services_obj, 1, bleio_peripheral_discover_remote_services);

//|   .. method:: pair()
//|
//|     Request pairing with connected central.
STATIC mp_obj_t bleio_peripheral_pair(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_peripheral_pair(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_pair_obj, bleio_peripheral_pair);

STATIC const mp_rom_map_elem_t bleio_peripheral_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_start_advertising),        MP_ROM_PTR(&bleio_peripheral_start_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_advertising),         MP_ROM_PTR(&bleio_peripheral_stop_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),               MP_ROM_PTR(&bleio_peripheral_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_discover_remote_services), MP_ROM_PTR(&bleio_peripheral_discover_remote_services_obj) },
    { MP_ROM_QSTR(MP_QSTR_pair),                     MP_ROM_PTR(&bleio_peripheral_pair_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected),       MP_ROM_PTR(&bleio_peripheral_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_name),            MP_ROM_PTR(&bleio_peripheral_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_services),        MP_ROM_PTR(&bleio_peripheral_services_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_peripheral_locals_dict, bleio_peripheral_locals_dict_table);

const mp_obj_type_t bleio_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .make_new = bleio_peripheral_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_peripheral_locals_dict
};
