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
#include "shared-bindings/bleio/Attribute.h"
#include "shared-bindings/bleio/Descriptor.h"
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`Descriptor` -- BLE descriptor
//| =========================================================
//|
//| Stores information about a BLE descriptor.
//| Descriptors are attached to BLE characteristics and provide contextual
//| information about the characteristic.
//|
//| A Descriptors cannot be created directly. A new local Descriptors can be created
//| and attached to a Descriptors by calling `Service.add_characteristic()`.
//| Remote Descriptor objects are created by `Central.discover_remote_services()`
//| or `Peripheral.discover_remote_services()` as part of remote Characteristics
//| in the remote Services that are discovered.

//|   .. attribute:: uuid
//|
//|     The descriptor uuid. (read-only)
//|
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

//|   .. attribute:: characteristic (read-only)
//|
//|     The Characteristic this Descriptor is a part of.
//|
STATIC mp_obj_t bleio_descriptor_get_characteristic(mp_obj_t self_in) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_descriptor_get_characteristic(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_descriptor_get_characteristic_obj, bleio_descriptor_get_characteristic);

const mp_obj_property_t bleio_descriptor_characteristic_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_descriptor_get_characteristic_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: value
//|
//|     The value of this descriptor.
//|
STATIC mp_obj_t bleio_descriptor_get_value(mp_obj_t self_in) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_descriptor_get_value(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_descriptor_get_value_obj, bleio_descriptor_get_value);

STATIC mp_obj_t bleio_descriptor_set_value(mp_obj_t self_in, mp_obj_t value_in) {
    bleio_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(value_in, &bufinfo, MP_BUFFER_READ);

    common_hal_bleio_descriptor_set_value(self, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_descriptor_set_value_obj, bleio_descriptor_set_value);

const mp_obj_property_t bleio_descriptor_value_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_descriptor_get_value_obj,
               (mp_obj_t)&bleio_descriptor_set_value_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_descriptor_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_uuid), MP_ROM_PTR(&bleio_descriptor_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_characteristic), MP_ROM_PTR(&bleio_descriptor_characteristic_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&bleio_descriptor_value_obj) },
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
    .print = bleio_descriptor_print,
    .locals_dict = (mp_obj_dict_t*)&bleio_descriptor_locals_dict
};
