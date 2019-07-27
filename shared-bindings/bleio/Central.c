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
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/Address.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Central.h"
#include "shared-bindings/bleio/Service.h"

//| .. currentmodule:: bleio
//|
//| :class:`Central` -- A BLE central device
//| =========================================================
//|
//| Implement a BLE central, which runs locally. Can connect to a given address.
//|
//| Usage::
//|
//|    import bleio
//|
//|    scanner = bleio.Scanner()
//|    entries = scanner.scan(2.5)
//|
//|    my_entry = None
//|    for entry in entries:
//|        if entry.name is not None and entry.name == 'MyPeripheral':
//|            my_entry = entry
//|            break
//|
//|    central = bleio.Central(my_entry.address)
//|    central.connect(10.0)  # timeout after 10 seconds
//|

//| .. class:: Central()
//|
//|   Create a new Central object.
//|
STATIC mp_obj_t bleio_central_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 0, false);

    bleio_central_obj_t *self = m_new_obj(bleio_central_obj_t);
    self->base.type = &bleio_central_type;

    common_hal_bleio_central_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: connect(address, timeout, *, service_uuids=None)
//|     Attempts a connection to the remote peripheral. If the connection is successful,
//|      Do BLE discovery for the listed services, to find their handles and characteristics.
//|      The attribute `remote_services` will contain a list of all discovered services.
//|
//|   :param bleio.Address address: The address of the peripheral to connect to
//|   :param float/int timeout: Try to connect for timeout seconds.
//|   :param iterable service_uuids_whitelist: an iterable of :py:class:~`UUID` objects for the services
//|      provided by the peripheral that you want to use.
//|      The peripheral may provide more services, but services not listed are ignored.
//|      If a service in service_uuids is not found during discovery, it will not
//|      appear in `remote_services`.
//|
//|      If service_uuids_whitelist is None, then all services will undergo discovery, which can be slow.
//|
//|      If the service UUID is 128-bit, or its characteristic UUID's are 128-bit, you
//|      you must have already created a :py:class:~`UUID` object for that UUID in order for the
//|      service or characteristic to be discovered. (This restriction may be lifted in the future.)
//|
STATIC mp_obj_t bleio_central_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_address, ARG_timeout, ARG_service_uuids_whitelist };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_timeout, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_service_uuids_whitelist, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!MP_OBJ_IS_TYPE(args[ARG_address].u_obj, &bleio_address_type)) {
        mp_raise_ValueError(translate("Expected an Address"));
    }

    bleio_address_obj_t *address = MP_OBJ_TO_PTR(args[ARG_address].u_obj);
    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);

    // common_hal_bleio_central_connect() will validate that services is an iterable or None.
    common_hal_bleio_central_connect(self, address, timeout, args[ARG_service_uuids_whitelist].u_obj);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_central_connect_obj, 3, bleio_central_connect);


//|   .. method:: disconnect()
//|
//|     Disconnects from the remote peripheral.
//|
STATIC mp_obj_t bleio_central_disconnect(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_central_disconnect(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_central_disconnect_obj, bleio_central_disconnect);

//|   .. attribute:: connected
//|
//|     True if connected to a remove peripheral.
//|
STATIC mp_obj_t bleio_central_get_connected(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_central_get_connected(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_central_get_connected_obj, bleio_central_get_connected);

const mp_obj_property_t bleio_central_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_central_get_connected_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};


//|   .. attribute:: remote_services (read-only)
//|
//|     A tuple of services provided by the remote peripheral.
//|     If the Central is not connected, an empty tuple will be returned.
//|
STATIC mp_obj_t bleio_central_get_remote_services(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Return list as a tuple so user won't be able to change it.
    mp_obj_list_t *service_list = common_hal_bleio_central_get_remote_services(self);
    return mp_obj_new_tuple(service_list->len, service_list->items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_central_get_remote_services_obj, bleio_central_get_remote_services);

const mp_obj_property_t bleio_central_remote_services_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_central_get_remote_services_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_central_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_connect),         MP_ROM_PTR(&bleio_central_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),      MP_ROM_PTR(&bleio_central_disconnect_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected),       MP_ROM_PTR(&bleio_central_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote_services), MP_ROM_PTR(&bleio_central_remote_services_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_central_locals_dict, bleio_central_locals_dict_table);

const mp_obj_type_t bleio_central_type = {
    { &mp_type_type },
    .name = MP_QSTR_Central,
    .make_new = bleio_central_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_central_locals_dict
};
