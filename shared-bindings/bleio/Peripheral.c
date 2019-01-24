/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include "shared-bindings/bleio/Peripheral.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"
#include "shared-module/bleio/AdvertisementData.h"
#include "shared-module/bleio/ScanEntry.h"

#include "common-hal/bleio/Peripheral.h"

// TODO: Add unique MAC address part to name
static const char default_name[] = "CIRCUITPY";

//| .. currentmodule:: bleio
//|
//| :class:`Peripheral` -- A BLE peripheral device
//| =========================================================
//|
//| Implement a BLE peripheral which runs locally.
//| Set up using the supplied services, and then allow advertising to be started and stopped.
//|
//| Usage::
//|
//|    import bleio
//|
//|    # Create a Characteristic.
//|    chara = bleio.Characteristic(bleio.UUID(0x2919), read=True, notify=True)
//|
//|    # Create a Service providing that one Characteristic.
//|    serv = bleio.Service(bleio.UUID(0x180f), [chara])
//|
//|    # Create a peripheral and start it up.
//|    periph = bleio.Peripheral([service])
//|    periph.start_advertising()
//|
//|    while not periph.connected:
//|        # Wait for connection.
//|        pass
//|
//| .. class:: Peripheral(services, *, name='CIRCUITPY')
//|
//|   Create a new Peripheral object.

//|   :param iterable services: the Service objects representing services available from this peripheral.
//|   :param str name: The name used when advertising this peripheral
//|

STATIC mp_obj_t bleio_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_services, ARG_name };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_services, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_name, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // If services is not an iterable, an exception will be thrown.
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(args[ARG_services].u_obj, &iter_buf);
    mp_obj_t service;

    bleio_peripheral_obj_t *self = m_new_obj(bleio_peripheral_obj_t);
    self->base.type = &bleio_peripheral_type;
    self->service_list = mp_obj_new_list(0, NULL);
    self->notif_handler = mp_const_none;
    while ((service = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (!MP_OBJ_IS_TYPE(service, &bleio_service_type)) {
            mp_raise_ValueError(translate("services includes an object that is not a Service"));
        }
        bleio_service_obj_t *service_ptr = MP_OBJ_TO_PTR(service);
        service_ptr->device = MP_OBJ_FROM_PTR(self);
        mp_obj_list_append(self->service_list, service);
    }

    const mp_obj_t name = args[ARG_name].u_obj;
    if (name == mp_const_none) {
        self->name = mp_obj_new_str(default_name, strlen(default_name));
    } else if (MP_OBJ_IS_STR(name)) {
        self->name = name;
    } else {
        mp_raise_ValueError(translate("name must be a string"));
    }

    // Do port-specific initialization.
    common_hal_bleio_peripheral_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: connected
//|
//|     True if connected to a BLE Central device.
//|
STATIC mp_obj_t bleio_peripheral_get_connected(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Return list as a tuple so user won't be able to change it.
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
//|     A `tuple` of `bleio.Service` that are offered by this peripheral. (read-only)
//|
STATIC mp_obj_t bleio_peripheral_get_services(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Return list as a tuple so user won't be able to change it.
    mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    return mp_obj_new_tuple(service_list->len, service_list->items);
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

    return self->name;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_get_name_obj, bleio_peripheral_get_name);

const mp_obj_property_t bleio_peripheral_name_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_peripheral_get_name_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. method:: start_advertising(*, connectable=True, data=None)
//|
//|     Starts advertising the peripheral. The peripheral's name and
//|     services are included in the advertisement packets.
//|
//|     :param bool connectable:  If `True` then other devices are allowed to connect to this peripheral.
//|     :param buf data: If `None`, advertise the services passed to this Peripheral when it was created.
//|       If not `None`, then send the bytes in ``data`` as the advertising packet.
//|
STATIC mp_obj_t bleio_peripheral_start_advertising(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_connectable, ARG_data };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_connectable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_data, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo = { 0 };
    if (args[ARG_data].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);
    }

    common_hal_bleio_peripheral_start_advertising(self, args[ARG_connectable].u_bool, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_peripheral_start_advertising_obj, 0, bleio_peripheral_start_advertising);

//|   .. method:: stop_advertising()
//|
//|     Stop sending advertising packets.
STATIC mp_obj_t bleio_peripheral_stop_advertising(mp_obj_t self_in) {
    bleio_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_peripheral_stop_advertising(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_peripheral_stop_advertising_obj, bleio_peripheral_stop_advertising);

STATIC const mp_rom_map_elem_t bleio_peripheral_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_start_advertising),  MP_ROM_PTR(&bleio_peripheral_start_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_advertising),   MP_ROM_PTR(&bleio_peripheral_stop_advertising_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected),          MP_ROM_PTR(&bleio_peripheral_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_name),               MP_ROM_PTR(&bleio_peripheral_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_services),           MP_ROM_PTR(&bleio_peripheral_services_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_peripheral_locals_dict, bleio_peripheral_locals_dict_table);

const mp_obj_type_t bleio_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peripheral,
    .make_new = bleio_peripheral_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_peripheral_locals_dict
};
