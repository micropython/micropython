/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

    const mp_obj_t uuid = args[ARG_uuid].u_obj;

    if (!MP_OBJ_IS_TYPE(uuid, &bleio_uuid_type)) {
        mp_raise_ValueError(translate("Expected a UUID"));
    }

    bleio_characteristic_obj_t *self = m_new_obj(bleio_characteristic_obj_t);
    self->base.type = &bleio_characteristic_type;
    self->uuid = MP_OBJ_TO_PTR(uuid);

    bleio_characteristic_properties_t properties;

    properties.broadcast = args[ARG_broadcast].u_bool;
    properties.indicate = args[ARG_indicate].u_bool;
    properties.notify = args[ARG_notify].u_bool;
    properties.read = args[ARG_read].u_bool;
    properties.write = args[ARG_write].u_bool;
    properties.write_no_response = args[ARG_write_no_response].u_bool;

    common_hal_bleio_characteristic_construct(self, uuid, properties);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: broadcast
//|
//|     A `bool` specifying if the characteristic allows broadcasting its value. (read-only)
//|
STATIC mp_obj_t bleio_characteristic_get_broadcast(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.broadcast);
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

    return mp_obj_new_bool(self->props.indicate);
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

    return mp_obj_new_bool(self->props.notify);
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

    return mp_obj_new_bool(self->props.read);
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

    return mp_obj_new_bool(self->props.write);
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

    return mp_obj_new_bool(self->props.write_no_response);
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
//|
STATIC mp_obj_t bleio_characteristic_get_uuid(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_FROM_PTR(self->uuid);
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

    common_hal_bleio_characteristic_get_value(self);

    return self->value_data;
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

STATIC const mp_rom_map_elem_t bleio_characteristic_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_broadcast),     MP_ROM_PTR(&bleio_characteristic_broadcast_obj) },
    { MP_ROM_QSTR(MP_QSTR_indicate),      MP_ROM_PTR(&bleio_characteristic_indicate_obj) },
    { MP_ROM_QSTR(MP_QSTR_notify),        MP_ROM_PTR(&bleio_characteristic_notify_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),          MP_ROM_PTR(&bleio_characteristic_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_uuid),          MP_ROM_PTR(&bleio_characteristic_uuid_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),         MP_ROM_PTR(&bleio_characteristic_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),         MP_ROM_PTR(&bleio_characteristic_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_no_response), MP_ROM_PTR(&bleio_characteristic_write_no_response_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_locals_dict, bleio_characteristic_locals_dict_table);

const mp_obj_type_t bleio_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .make_new = bleio_characteristic_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_characteristic_locals_dict
};
