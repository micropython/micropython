// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

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
static mp_obj_t bleio_service_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_uuid, ARG_secondary };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_secondary, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_uuid_obj_t *uuid = mp_arg_validate_type(args[ARG_uuid].u_obj, &bleio_uuid_type, MP_QSTR_uuid);

    const bool is_secondary = args[ARG_secondary].u_bool;

    bleio_service_obj_t *service = m_new_obj_with_finaliser(bleio_service_obj_t);
    service->base.type = &bleio_service_type;

    common_hal_bleio_service_construct(service, uuid, is_secondary);

    return MP_OBJ_FROM_PTR(service);
}

//|     def deinit(self) -> None:
//|         """Disable and deinitialise the Service."""
//|         ...
static mp_obj_t bleio_service_deinit(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_bleio_service_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_deinit_obj, bleio_service_deinit);

//|     characteristics: Tuple[Characteristic, ...]
//|     """A tuple of :py:class:`Characteristic` designating the characteristics that are offered by
//|     this service. (read-only)"""
static mp_obj_t bleio_service_get_characteristics(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(common_hal_bleio_service_get_characteristics(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_characteristics_obj, bleio_service_get_characteristics);

MP_PROPERTY_GETTER(bleio_service_characteristics_obj,
    (mp_obj_t)&bleio_service_get_characteristics_obj);

//|     remote: bool
//|     """True if this is a service provided by a remote device. (read-only)"""
static mp_obj_t bleio_service_get_remote(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_service_get_is_remote(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_remote_obj, bleio_service_get_remote);

MP_PROPERTY_GETTER(bleio_service_remote_obj,
    (mp_obj_t)&bleio_service_get_remote_obj);

//|     secondary: bool
//|     """True if this is a secondary service. (read-only)"""
static mp_obj_t bleio_service_get_secondary(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_service_get_is_secondary(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_secondary_obj, bleio_service_get_secondary);

MP_PROPERTY_GETTER(bleio_service_secondary_obj,
    (mp_obj_t)&bleio_service_get_secondary_obj);

//|     uuid: Optional[UUID]
//|     """The UUID of this service. (read-only)
//|
//|     Will be ``None`` if the 128-bit UUID for this service is not known."""
//|
static mp_obj_t bleio_service_get_uuid(mp_obj_t self_in) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_uuid_obj_t *uuid = common_hal_bleio_service_get_uuid(self);
    return uuid ? MP_OBJ_FROM_PTR(uuid) : mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(bleio_service_get_uuid_obj, bleio_service_get_uuid);

MP_PROPERTY_GETTER(bleio_service_uuid_obj,
    (mp_obj_t)&bleio_service_get_uuid_obj);


static const mp_rom_map_elem_t bleio_service_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),            MP_ROM_PTR(&bleio_service_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),           MP_ROM_PTR(&bleio_service_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_characteristics),   MP_ROM_PTR(&bleio_service_characteristics_obj) },
    { MP_ROM_QSTR(MP_QSTR_secondary),         MP_ROM_PTR(&bleio_service_secondary_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),              MP_ROM_PTR(&bleio_service_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote),            MP_ROM_PTR(&bleio_service_remote_obj) },
};
static MP_DEFINE_CONST_DICT(bleio_service_locals_dict, bleio_service_locals_dict_table);

static void bleio_service_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_service_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->uuid) {
        mp_printf(print, "Service(");
        bleio_uuid_print(print, MP_OBJ_FROM_PTR(self->uuid), kind);
        mp_printf(print, ")");
    } else {
        mp_printf(print, "<Service with unregistered UUID>");
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    bleio_service_type,
    MP_QSTR_Service,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, bleio_service_make_new,
    print, bleio_service_print,
    locals_dict, &bleio_service_locals_dict
    );
