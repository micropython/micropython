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
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`Characteristic` -- BLE service characteristic
//| =========================================================
//|
//| Stores information about a BLE service characteristic and allows reading
//| and writing of the characteristic's value.
//|
//|
//| .. class:: Characteristic(uuid, *, broadcast=False, indicate=False, notify=False, read=False, write=False, write_no_response=False)
//|
//|   Create a new Characteristic object identified by the specified UUID.
//|
//|   :param bleio.UUID uuid: The uuid of the characteristic
//|   :param bool broadcast: Allowed in advertising packets
//|   :param bool indicate: Server will indicate to the client when the value is set and wait for a response
//|   :param bool notify: Server will notify the client when the value is set
//|   :param bool read: Clients may read this characteristic
//|   :param bool write: Clients may write this characteristic; a response will be sent back
//|   :param bool write_no_response: Clients may write this characteristic; no response will be sent back
//|
STATIC mp_obj_t bleio_characteristic_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_uuid, ARG_broadcast, ARG_indicate, ARG_notify, ARG_read, ARG_write, ARG_write_no_response,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_broadcast, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_indicate, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_notify, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_read, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_write, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_write_no_response, MP_ARG_KW_ONLY| MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid_obj = args[ARG_uuid].u_obj;

    if (!MP_OBJ_IS_TYPE(uuid_obj, &bleio_uuid_type)) {
        mp_raise_ValueError(translate("Expected a UUID"));
    }
    bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_obj);

    bleio_characteristic_obj_t *self = m_new_obj(bleio_characteristic_obj_t);
    self->base.type = &bleio_characteristic_type;

    bleio_characteristic_properties_t properties;

    properties.broadcast = args[ARG_broadcast].u_bool;
    properties.indicate = args[ARG_indicate].u_bool;
    properties.notify = args[ARG_notify].u_bool;
    properties.read = args[ARG_read].u_bool;
    properties.write = args[ARG_write].u_bool;
    properties.write_no_response = args[ARG_write_no_response].u_bool;

    // Initialize, with an empty descriptor list.
    common_hal_bleio_characteristic_construct(self, uuid, properties, mp_obj_new_list(0, NULL));

    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: broadcast
//|
//|     A `bool` specifying if the characteristic allows broadcasting its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_broadcast(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).broadcast);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_broadcast_obj, bleio_characteristic_get_broadcast);

const mp_obj_property_t bleio_characteristic_broadcast_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_broadcast_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: indicate
//|
//|     A `bool` specifying if the characteristic allows indicating its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_indicate(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).indicate);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_indicate_obj, bleio_characteristic_get_indicate);


const mp_obj_property_t bleio_characteristic_indicate_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_indicate_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: notify
//|
//|     A `bool` specifying if the characteristic allows notifying its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_notify(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).notify);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_notify_obj, bleio_characteristic_get_notify);

const mp_obj_property_t bleio_characteristic_notify_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_notify_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: read
//|
//|     A `bool` specifying if the characteristic allows reading its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_read(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).read);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_read_obj, bleio_characteristic_get_read);

const mp_obj_property_t bleio_characteristic_read_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_read_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: write
//|
//|     A `bool` specifying if the characteristic allows writing to its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_write(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).write);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_write_obj, bleio_characteristic_get_write);

const mp_obj_property_t bleio_characteristic_write_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_write_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: write_no_response
//|
//|     A `bool` specifying if the characteristic allows writing to its value without response. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_write_no_response(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_bleio_characteristic_get_properties(self).write_no_response);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_write_no_response_obj, bleio_characteristic_get_write_no_response);

const mp_obj_property_t bleio_characteristic_write_no_response_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_write_no_response_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: uuid
//|
//|     The UUID of this characteristic. (read-only)
//|       Will be ``None`` if the 128-bit UUID for this characteristic is not known.
//|
STATIC mp_obj_t bleio_characteristic_get_uuid(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bleio_uuid_obj_t *uuid = common_hal_bleio_characteristic_get_uuid(self);
    return uuid ? MP_OBJ_FROM_PTR(uuid) : mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_uuid_obj, bleio_characteristic_get_uuid);

const mp_obj_property_t bleio_characteristic_uuid_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_uuid_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: value
//|
//|     The value of this characteristic. The value can be written to if the `write` property allows it.
//|     If the `read` property allows it, the value can be read. If the `notify` property is set, writing
//|     to the value will generate a BLE notification.
//|
STATIC mp_obj_t bleio_characteristic_get_value(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_bleio_characteristic_get_value(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_value_obj, bleio_characteristic_get_value);

STATIC mp_obj_t bleio_characteristic_set_value(mp_obj_t self_in, mp_obj_t value_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(value_in, &bufinfo, MP_BUFFER_READ);

    common_hal_bleio_characteristic_set_value(self, &bufinfo);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_value_obj, bleio_characteristic_set_value);

const mp_obj_property_t bleio_characteristic_value_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_value_obj,
               (mp_obj_t)&bleio_characteristic_set_value_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. attribute:: descriptors
//|
//|     A tuple of `bleio.Descriptor` that describe this characteristic. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_descriptors(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Return list as a tuple so user won't be able to change it.
    mp_obj_list_t *char_list = common_hal_bleio_characteristic_get_descriptor_list(self);
    return mp_obj_new_tuple(char_list->len, char_list->items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_descriptors_obj, bleio_characteristic_get_descriptors);

const mp_obj_property_t bleio_characteristic_descriptors_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_descriptors_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|   .. method:: set_cccd(*, notify=False, indicate=False)
//|
//|     Set the remote characteristic's CCCD to enable or disable notification and indication.
//|
//|     :param bool notify: True if Characteristic should receive notifications of remote writes
//|     :param float indicate: True if Characteristic should receive indications of remote writes
//|
STATIC mp_obj_t bleio_characteristic_set_cccd(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_notify, ARG_indicate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_notify, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_indicate, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_bleio_characteristic_set_cccd(self, args[ARG_notify].u_bool, args[ARG_indicate].u_bool);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_characteristic_set_cccd_obj, 1, bleio_characteristic_set_cccd);


STATIC const mp_rom_map_elem_t bleio_characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_broadcast),     MP_ROM_PTR(&bleio_characteristic_broadcast_obj) },
    { MP_ROM_QSTR(MP_QSTR_descriptors),   MP_ROM_PTR(&bleio_characteristic_descriptors_obj) },
    { MP_ROM_QSTR(MP_QSTR_indicate),      MP_ROM_PTR(&bleio_characteristic_indicate_obj) },
    { MP_ROM_QSTR(MP_QSTR_notify),        MP_ROM_PTR(&bleio_characteristic_notify_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),          MP_ROM_PTR(&bleio_characteristic_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_cccd),      MP_ROM_PTR(&bleio_characteristic_set_cccd_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),          MP_ROM_PTR(&bleio_characteristic_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),         MP_ROM_PTR(&bleio_characteristic_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),         MP_ROM_PTR(&bleio_characteristic_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_no_response), MP_ROM_PTR(&bleio_characteristic_write_no_response_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_locals_dict, bleio_characteristic_locals_dict_table);

STATIC void bleio_characteristic_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->uuid) {
        mp_printf(print, "Characteristic(");
        bleio_uuid_print(print, MP_OBJ_FROM_PTR(self->uuid), kind);
        mp_printf(print, ")");
    } else {
        mp_printf(print, "<Characteristic with Unregistered UUID>");
    }
}

const mp_obj_type_t bleio_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .make_new = bleio_characteristic_make_new,
    .print = bleio_characteristic_print,
    .locals_dict = (mp_obj_dict_t*)&bleio_characteristic_locals_dict
};
