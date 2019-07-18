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
#include "shared-bindings/bleio/Descriptor.h"
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`Descriptor` -- BLE descriptor
//| =========================================================
//|
//| Stores information about a BLE descriptor.
//| Descriptors are encapsulated by BLE characteristics and provide contextual
//| information about the characteristic.
//|

//| .. class:: Descriptor(uuid)
//|
//|   Create a new descriptor object with the UUID uuid.

//|   .. attribute:: handle
//|
//|     The descriptor handle. (read-only)
//|

//|   .. attribute:: uuid
//|
//|     The descriptor uuid. (read-only)
//|
STATIC mp_obj_t bleio_descriptor_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_uuid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid_arg = args[ARG_uuid].u_obj;

    if (!MP_OBJ_IS_TYPE(uuid_arg, &bleio_uuid_type)) {
        mp_raise_ValueError(translate("Expected a UUID"));
    }

    bleio_descriptor_obj_t *self = m_new_obj(bleio_descriptor_obj_t);
    self->base.type = type;
    bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_arg);
    common_hal_bleio_descriptor_construct(self, uuid);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t bleio_descriptor_get_handle(mp_obj_t self_in) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_int(common_hal_bleio_descriptor_get_handle(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_descriptor_get_handle_obj, bleio_descriptor_get_handle);

const mp_obj_property_t bleio_descriptor_handle_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_descriptor_get_handle_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC mp_obj_t bleio_descriptor_get_uuid(mp_obj_t self_in) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_uuid_obj_t *uuid = common_hal_bleio_descriptor_get_uuid(self);
    return uuid ? MP_OBJ_FROM_PTR(uuid) : mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_descriptor_get_uuid_obj, bleio_descriptor_get_uuid);

const mp_obj_property_t bleio_descriptor_uuid_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_descriptor_get_uuid_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t bleio_descriptor_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_handle), MP_ROM_PTR(&bleio_descriptor_handle_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid), MP_ROM_PTR(&bleio_descriptor_uuid_obj) },

    // Static variables
    { MP_ROM_QSTR(MP_QSTR_CHARACTERISTIC_EXTENDED_PROPERTIES),
      MP_ROM_INT(DESCRIPTOR_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES) },

    { MP_ROM_QSTR(MP_QSTR_CHARACTERISTIC_USER_DESCRIPTION),
      MP_ROM_INT(DESCRIPTOR_UUID_CHARACTERISTIC_USER_DESCRIPTION) },

    { MP_ROM_QSTR(MP_QSTR_CLIENT_CHARACTERISTIC_CONFIGURATION),
      MP_ROM_INT(DESCRIPTOR_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION) },

    { MP_ROM_QSTR(MP_QSTR_SERVER_CHARACTERISTIC_CONFIGURATION),
      MP_ROM_INT(DESCRIPTOR_UUID_SERVER_CHARACTERISTIC_CONFIGURATION) },

    { MP_ROM_QSTR(MP_QSTR_CHARACTERISTIC_PRESENTATION_FORMAT),
      MP_ROM_INT(DESCRIPTOR_UUID_CHARACTERISTIC_PRESENTATION_FORMAT) },

    { MP_ROM_QSTR(MP_QSTR_CHARACTERISTIC_AGGREGATE_FORMAT),
      MP_ROM_INT(DESCRIPTOR_UUID_CHARACTERISTIC_AGGREGATE_FORMAT) },

    { MP_ROM_QSTR(MP_QSTR_VALID_RANGE),
      MP_ROM_INT(DESCRIPTOR_UUID_VALID_RANGE) },

    { MP_ROM_QSTR(MP_QSTR_EXTERNAL_REPORT_REFERENCE),
      MP_ROM_INT(DESCRIPTOR_UUID_EXTERNAL_REPORT_REFERENCE) },

    { MP_ROM_QSTR(MP_QSTR_REPORT_REFERENCE),
      MP_ROM_INT(DESCRIPTOR_UUID_REPORT_REFERENCE) },

    { MP_ROM_QSTR(MP_QSTR_NUMBER_OF_DIGITALS),
      MP_ROM_INT(DESCRIPTOR_UUID_NUMBER_OF_DIGITALS) },

    { MP_ROM_QSTR(MP_QSTR_VALUE_TRIGGER_SETTING),
      MP_ROM_INT(DESCRIPTOR_UUID_VALUE_TRIGGER_SETTING) },

    { MP_ROM_QSTR(MP_QSTR_ENVIRONMENTAL_SENSING_CONFIGURATION),
      MP_ROM_INT(DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_CONFIGURATION) },

    { MP_ROM_QSTR(MP_QSTR_ENVIRONMENTAL_SENSING_MEASUREMENT  ),
      MP_ROM_INT(DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_MEASUREMENT) },

    { MP_ROM_QSTR(MP_QSTR_ENVIRONMENTAL_SENSING_TRIGGER_SETTING),
      MP_ROM_INT(DESCRIPTOR_UUID_ENVIRONMENTAL_SENSING_TRIGGER_SETTING) },

    { MP_ROM_QSTR(MP_QSTR_TIME_TRIGGER_SETTING),
      MP_ROM_INT(DESCRIPTOR_UUID_TIME_TRIGGER_SETTING) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_descriptor_locals_dict, bleio_descriptor_locals_dict_table);

STATIC void bleio_descriptor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->uuid) {
        mp_printf(print, "Descriptor(");
        bleio_uuid_print(print, MP_OBJ_FROM_PTR(self->uuid), kind);
        mp_printf(print, ")");
    } else {
        mp_printf(print, "<Descriptor with Unregistered UUID>");
    }
}

const mp_obj_type_t bleio_descriptor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Descriptor,
    .make_new = bleio_descriptor_make_new,
    .print = bleio_descriptor_print,
    .locals_dict = (mp_obj_dict_t*)&bleio_descriptor_locals_dict
};
