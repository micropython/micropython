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
#include "shared-bindings/bleio/AddressType.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Central.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

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
//|        if entry.name is not None and entry.name == 'MyCentral':
//|            my_entry = entry
//|            break
//|
//|    central = bleio.Central(my_entry.address)
//|    central.connect()
//|

//| .. class:: Central(address=None, scan_entry=None)
//|
//|   Create a new Central object. If the `address` or :py:data:`scan_entry` parameters are not `None`,
//|   the role is set to Central, otherwise it's set to Peripheral.
//|
//|   :param bleio.Address address: The address of the central to connect to
//|   :param bleio.ScanEntry scan_entry: The scan entry returned from `bleio.Scanner`
//|
STATIC mp_obj_t bleio_central_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    bleio_central_obj_t *self = m_new_obj(bleio_central_obj_t);
    self->base.type = &bleio_central_type;
    self->service_list = mp_obj_new_list(0, NULL);
    self->conn_handler = mp_const_none;
    self->conn_handle = 0xFFFF;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);

    enum { ARG_address, ARG_scan_entry };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_scan_entry, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t address_obj = args[ARG_address].u_obj;
    const mp_obj_t scan_entry_obj = args[ARG_scan_entry].u_obj;

    if (!MP_OBJ_IS_TYPE(address_obj, &bleio_address_type)) {
        mp_raise_ValueError("Expected an Address");
    }

    return MP_OBJ_FROM_PTR(self);
}



//|   .. method:: connect()
//|
//|     Attempts a connection to the remote peripheral. If the connection is successful,
//|
STATIC mp_obj_t bleio_central_connect(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't connect in Peripheral mode")));
    }

    common_hal_bleio_central_connect(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_central_connect_obj, bleio_central_connect);


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

//|   .. attribute:: remote_name (read-only)
//|
//|     The name of the remote peripheral, if connected. May be None if no name was advertised.
//|
STATIC mp_obj_t bleio_central_get_remote_name(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return self->remote_name;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_central_get_remote_name_obj, bleio_central_get_remote_name);

//|   .. attribute:: remote_services (read-only)
//|
//|     Empty until connected, then a list of services provided by the remote peripheral.
//|
STATIC mp_obj_t bleio_central_get_remote_services(mp_obj_t self_in) {
    bleio_central_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return self->service_list;
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
    { MP_ROM_QSTR(MP_QSTR_connect),            MP_ROM_PTR(&bleio_central_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),         MP_ROM_PTR(&bleio_central_disconnect_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_remote_name),        MP_ROM_PTR(&bleio_central_remote_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote_services),    MP_ROM_PTR(&bleio_central_remote_services_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_central_locals_dict, bleio_central_locals_dict_table);

const mp_obj_type_t bleio_central_type = {
    { &mp_type_type },
    .name = MP_QSTR_Central,
    .make_new = bleio_central_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_central_locals_dict
};
