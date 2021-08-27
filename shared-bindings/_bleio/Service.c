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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

//| class Service:
//|     """Stores information about a BLE service and its characteristics."""
//|
//|     def __init__(self, uuid: UUID, *, secondary: bool = False) -> None:
//|         """Create a new Service identified by the specified UUID. It can be accessed by all
//|         connections. This is known as a Service server. Client Service objects are created via
//|         `Connection.discover_remote_services`.
//|
//|         To mark the Service as secondary, pass `True` as :py:data:`secondary`.
//|
//|         :param UUID uuid: The uuid of the service
//|         :param bool secondary: If the service is a secondary one
//|
//|         :return: the new Service"""
//|         ...
//|
STATIC mp_obj_t bleio_service_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_uuid, ARG_secondary };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_secondary, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid_obj = args[ARG_uuid].u_obj;
    if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
        mp_raise_TypeError(translate("Expected a UUID"));
    }

    const bool is_secondary = args[ARG_secondary].u_bool;

    bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
    service->base.type = &bleio_service_type;

    common_hal_bleio_service_construct(service, MP_OBJ_TO_PTR(uuid_obj), is_secondary);

    return MP_OBJ_FROM_PTR(service);
}

//|     characteristics: Tuple[Characteristic, ...]
//|     """A tuple of :py:class:`Characteristic` designating the characteristics that are offered by
//|     this service. (read-only)"""
//|
STATIC mp_obj_t bleio_service_get_characteristics(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(common_hal_bleio_service_get_characteristics(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_characteristics_obj, bleio_service_get_characteristics);

const mp_obj_property_t bleio_service_characteristics_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_service_get_characteristics_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     remote: bool
//|     """True if this is a service provided by a remote device. (read-only)"""
//|
STATIC mp_obj_t bleio_service_get_remote(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_service_get_is_remote(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_remote_obj, bleio_service_get_remote);

const mp_obj_property_t bleio_service_remote_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_service_get_remote_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     secondary: bool
//|     """True if this is a secondary service. (read-only)"""
//|
STATIC mp_obj_t bleio_service_get_secondary(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_service_get_is_secondary(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_secondary_obj, bleio_service_get_secondary);

const mp_obj_property_t bleio_service_secondary_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_service_get_secondary_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     uuid: Optional[UUID]
//|     """The UUID of this service. (read-only)
//|
//|     Will be ``None`` if the 128-bit UUID for this service is not known."""
//|
STATIC mp_obj_t bleio_service_get_uuid(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_uuid_obj_t *uuid = common_hal_bleio_service_get_uuid(self);
    return uuid ? MP_OBJ_FROM_PTR(uuid) : mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_uuid_obj, bleio_service_get_uuid);

const mp_obj_property_t bleio_service_uuid_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_service_get_uuid_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};


STATIC const mp_rom_map_elem_t bleio_service_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_characteristics),   MP_ROM_PTR(&bleio_service_characteristics_obj) },
    { MP_ROM_QSTR(MP_QSTR_secondary),         MP_ROM_PTR(&bleio_service_secondary_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),              MP_ROM_PTR(&bleio_service_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote),            MP_ROM_PTR(&bleio_service_remote_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bleio_service_locals_dict, bleio_service_locals_dict_table);

STATIC void bleio_service_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->uuid) {
        mp_printf(print, "Service(");
        bleio_uuid_print(print, MP_OBJ_FROM_PTR(self->uuid), kind);
        mp_printf(print, ")");
    } else {
        mp_printf(print, "<Service with unregistered UUID>");
    }
}

const mp_obj_type_t bleio_service_type = {
    { &mp_type_type },
    .name = MP_QSTR_Service,
    .make_new = bleio_service_make_new,
    .print = bleio_service_print,
    .locals_dict = (mp_obj_dict_t *)&bleio_service_locals_dict
};
