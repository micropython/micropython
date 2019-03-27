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
#include "shared-bindings/bleio/Device.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"
#include "shared-module/bleio/AdvertisementData.h"
#include "shared-module/bleio/Device.h"
#include "shared-module/bleio/ScanEntry.h"

// Work-in-progress: orphaned for now.
//| :orphan:
//|
//| .. currentmodule:: bleio
//|
//| :class:`Device` -- BLE device
//| =========================================================
//|
//| **IGNORE ``Device`` and all its documentation.
//| It is being replaced by `Peripheral` and other classes.**
//|
//| Provides access a to BLE device, either in a Peripheral or Central role.
//| When a device is created without any parameter passed to the constructor,
//| it will be set to the Peripheral role. If a address is passed, the device
//| will be a Central. For a Peripheral you can set the `name`, add services
//| via `add_service` and then start and stop advertising via `bleio.Device.start_advertising`
//| and `bleio.Device.stop_advertising`. For the Central, you can `bleio.Device.connect` and `bleio.Device.disconnect`
//| to the device, once a connection is established, the device's services can
//| be accessed using `bleio.Device.services`.
//|
//| Usage::
//|
//|    import bleio
//|
//|    # Peripheral
//|    periph = bleio.Device()
//|
//|    serv = bleio.Service(bleio.UUID(0x180f))
//|    p.add_service(serv)
//|
//|    chara = bleio.Characteristic(bleio.UUID(0x2919))
//|    chara.read = True
//|    chara.notify = True
//|    serv.add_characteristic(chara)
//|
//|    periph.start_advertising()
//|
//|    # Central
//|    scanner = bleio.Scanner()
//|    entries = scanner.scan(2500)
//|
//|    my_entry = None
//|    for entry in entries:
//|        if entry.name is not None and entry.name == 'MyDevice':
//|            my_entry = entry
//|            break
//|
//|    central = bleio.Device(my_entry.address)
//|    central.connect()
//|

//| .. class:: Device(address=None, scan_entry=None)
//|
//|   Create a new Device object. If the `address` or :py:data:`scan_entry` parameters are not `None`,
//|   the role is set to Central, otherwise it's set to Peripheral.
//|
//|   :param bleio.Address address: The address of the device to connect to
//|   :param bleio.ScanEntry scan_entry: The scan entry returned from `bleio.Scanner`
//|

//|   .. attribute:: name
//|
//|     For the Peripheral role, this property can be used to read and write the device's name.
//|     For the Central role, this property will equal the name of the remote device, if one was
//|     advertised by the device. In the Central role this property is read-only.
//|

//|   .. attribute:: services
//|
//|     A `list` of `bleio.Service` that are offered by this device. (read-only)
//|     For a Peripheral device, this list will contain services added using `add_service`,
//|     for a Central, this list will be empty until a connection is established, at which point
//|     it will be filled with the remote device's services.
//|

//|   .. method:: add_service(service)
//|
//|     Appends the :py:data:`service` to the list of this devices's services.
//|     This method can only be called for Peripheral devices.
//|
//|     :param bleio.Service service: the service to append
//|

//|   .. method:: connect()
//|
//|     Attempts a connection to the remote device. If the connection is successful,
//|     the device's services are available via `services`.
//|     This method can only be called for Central devices.
//|

//|   .. method:: disconnect()
//|
//|     Disconnects from the remote device.
//|     This method can only be called for Central devices.
//|

//|   .. method:: start_advertising(connectable=True)
//|
//|     Starts advertising the device. The device's name and
//|     services are put into the advertisement packets.
//|     If :py:data:`connectable` is `True` then other devices are allowed to conncet to this device.
//|     This method can only be called for Peripheral devices.
//|

//|   .. method:: stop_advertising()
//|
//|     Disconnects from the remote device.
//|     This method can only be called for Peripheral devices.
//|

// TODO: Add unique MAC address part to name
static const char default_name[] = "CIRCUITPY";

STATIC void bleio_device_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Device(role: %s)", self->is_peripheral ? "Peripheral" : "Central");
}

STATIC mp_obj_t bleio_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, true);
    bleio_device_obj_t *self = m_new_obj(bleio_device_obj_t);
    self->base.type = &bleio_device_type;
    self->service_list = mp_obj_new_list(0, NULL);
    self->notif_handler = mp_const_none;
    self->conn_handler = mp_const_none;
    self->conn_handle = 0xFFFF;
    self->is_peripheral = true;

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

    if (address_obj != mp_const_none) {
        bleio_address_obj_t *address = MP_OBJ_TO_PTR(address_obj);

        self->is_peripheral = false;
        self->address.type = address->type;
        memcpy(self->address.value, address->value, BLEIO_ADDRESS_BYTES);
    } else if (scan_entry_obj != mp_const_none) {
        bleio_scanentry_obj_t *scan_entry = MP_OBJ_TO_PTR(scan_entry_obj);

        self->is_peripheral = false;
        self->address.type = scan_entry->address.type;
        memcpy(self->address.value, scan_entry->address.value, BLEIO_ADDRESS_BYTES);
    } else {
        self->name = mp_obj_new_str(default_name, strlen(default_name));
        common_hal_bleio_adapter_get_address(&self->address);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t bleio_device_add_service(mp_obj_t self_in, mp_obj_t service_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_in);

    if (!self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't add services in Central mode")));
    }

    service->device = self;

    mp_obj_list_append(self->service_list, service);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_device_add_service_obj, bleio_device_add_service);

STATIC mp_obj_t bleio_device_connect(mp_obj_t self_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't connect in Peripheral mode")));
    }

    common_hal_bleio_device_connect(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_device_connect_obj, bleio_device_connect);

STATIC mp_obj_t bleio_device_disconnect(mp_obj_t self_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_device_disconnect(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_device_disconnect_obj, bleio_device_disconnect);

STATIC mp_obj_t bleio_device_get_name(mp_obj_t self_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return self->name;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_device_get_name_obj, bleio_device_get_name);

static mp_obj_t bleio_device_set_name(mp_obj_t self_in, mp_obj_t value) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't change the name in Central mode")));
    }

    self->name = value;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_device_set_name_obj, bleio_device_set_name);

const mp_obj_property_t bleio_device_name_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_device_get_name_obj,
               (mp_obj_t)&bleio_device_set_name_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_device_start_advertising(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (!self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't advertise in Central mode")));
    }

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

    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    for (size_t i = 0; i < service_list->len; ++i) {
        bleio_service_obj_t *service = service_list->items[i];
        if (service->handle == 0xFFFF) {
            common_hal_bleio_device_add_service(self, service);
        }
    }

    common_hal_bleio_device_start_advertising(self, args[ARG_connectable].u_bool, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_device_start_advertising_obj, 0, bleio_device_start_advertising);

STATIC mp_obj_t bleio_device_stop_advertising(mp_obj_t self_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->is_peripheral) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  translate("Can't advertise in Central mode")));
    }

    common_hal_bleio_device_stop_advertising(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_device_stop_advertising_obj, bleio_device_stop_advertising);

STATIC mp_obj_t bleio_device_get_services(mp_obj_t self_in) {
    bleio_device_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return self->service_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_device_get_services_obj, bleio_device_get_services);

const mp_obj_property_t bleio_device_services_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_device_get_services_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_device_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_add_service),        MP_ROM_PTR(&bleio_device_add_service_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),            MP_ROM_PTR(&bleio_device_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),         MP_ROM_PTR(&bleio_device_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_advertising),  MP_ROM_PTR(&bleio_device_start_advertising_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_advertising),   MP_ROM_PTR(&bleio_device_stop_advertising_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_name),               MP_ROM_PTR(&bleio_device_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_services),           MP_ROM_PTR(&bleio_device_services_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_device_locals_dict, bleio_device_locals_dict_table);

const mp_obj_type_t bleio_device_type = {
    { &mp_type_type },
    .name = MP_QSTR_Device,
    .print = bleio_device_print,
    .make_new = bleio_device_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_device_locals_dict
};
