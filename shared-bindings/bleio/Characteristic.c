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
//| Stores information about a BLE service characteristic and allows to read
//| and write the characteristic's value.
//|

//| .. class:: Characteristic(uuid)
//|
//|   Create a new Characteristic object identified by the specified UUID.
//|
//|   :param bleio.UUID uuid: The uuid of the characteristic
//|

//|   .. attribute:: broadcast
//|
//|     A `bool` specifying if the characteristic allows broadcasting its value.
//|

//|   .. attribute:: indicate
//|
//|     A `bool` specifying if the characteristic allows indicating its value.
//|

//|   .. attribute:: notify
//|
//|     A `bool` specifying if the characteristic allows notifying its value.
//|

//|   .. attribute:: read
//|
//|     A `bool` specifying if the characteristic allows reading its value.
//|

//|   .. attribute:: uuid
//|
//|     The UUID of this characteristic. (read-only)
//|

//|   .. attribute:: value
//|
//|     The value of this characteristic. The value can be written to if the `write` property allows it.
//|     If the `read` property allows it, the value can be read. If the `notify` property is set, writting
//|     to the value will generate a BLE notification.
//|

//|   .. attribute:: write
//|
//|     A `bool` specifying if the characteristic allows writing to its value.
//|

//|   .. attribute:: write_no_resp
//|
//|     A `bool` specifying if the characteristic allows writing to its value without response.
//|
STATIC void bleio_characteristic_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Characteristic(");
    bleio_uuid_print(print, self->uuid, kind);
    mp_printf(print, ")");
}

STATIC mp_obj_t bleio_characteristic_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    bleio_characteristic_obj_t *self = m_new_obj(bleio_characteristic_obj_t);
    self->base.type = &bleio_characteristic_type;
    self->service = NULL;
    self->value_data = NULL;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);

    enum { ARG_uuid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_uuid,  MP_ARG_REQUIRED| MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid = args[ARG_uuid].u_obj;

    if (!MP_OBJ_IS_TYPE(uuid, &bleio_uuid_type)) {
        mp_raise_ValueError(translate("Expected a UUID"));
    }

    self->uuid = MP_OBJ_TO_PTR(uuid);
    common_hal_bleio_characteristic_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t bleio_characteristic_get_broadcast(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.broadcast);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_broadcast_obj, bleio_characteristic_get_broadcast);

STATIC mp_obj_t bleio_characteristic_set_broadcast(mp_obj_t self_in, mp_obj_t broadcast_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.broadcast = mp_obj_is_true(broadcast_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_broadcast_obj, bleio_characteristic_set_broadcast);

const mp_obj_property_t bleio_characteristic_broadcast_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_broadcast_obj,
               (mp_obj_t)&bleio_characteristic_set_broadcast_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_characteristic_get_indicate(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.indicate);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_indicate_obj, bleio_characteristic_get_indicate);

STATIC mp_obj_t bleio_characteristic_set_indicate(mp_obj_t self_in, mp_obj_t indicate_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.indicate = mp_obj_is_true(indicate_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_indicate_obj, bleio_characteristic_set_indicate);

const mp_obj_property_t bleio_characteristic_indicate_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_indicate_obj,
               (mp_obj_t)&bleio_characteristic_set_indicate_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_characteristic_get_notify(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.notify);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_notify_obj, bleio_characteristic_get_notify);

STATIC mp_obj_t bleio_characteristic_set_notify(mp_obj_t self_in, mp_obj_t notify_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.notify = mp_obj_is_true(notify_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_notify_obj, bleio_characteristic_set_notify);

const mp_obj_property_t bleio_characteristic_notify_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_notify_obj,
               (mp_obj_t)&bleio_characteristic_set_notify_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_characteristic_get_read(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.read);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_read_obj, bleio_characteristic_get_read);

STATIC mp_obj_t bleio_characteristic_set_read(mp_obj_t self_in, mp_obj_t read_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.read = mp_obj_is_true(read_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_read_obj, bleio_characteristic_set_read);

const mp_obj_property_t bleio_characteristic_read_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_read_obj,
               (mp_obj_t)&bleio_characteristic_set_read_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_characteristic_get_write(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.write);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_write_obj, bleio_characteristic_get_write);

STATIC mp_obj_t bleio_characteristic_set_write(mp_obj_t self_in, mp_obj_t write_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.write = mp_obj_is_true(write_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_write_obj, bleio_characteristic_set_write);

const mp_obj_property_t bleio_characteristic_write_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_write_obj,
               (mp_obj_t)&bleio_characteristic_set_write_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_characteristic_get_write_wo_resp(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(self->props.write_wo_resp);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_write_wo_resp_obj, bleio_characteristic_get_write_wo_resp);

STATIC mp_obj_t bleio_characteristic_set_write_wo_resp(mp_obj_t self_in, mp_obj_t write_wo_resp_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->props.write_wo_resp = mp_obj_is_true(write_wo_resp_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_characteristic_set_write_wo_resp_obj, bleio_characteristic_set_write_wo_resp);

const mp_obj_property_t bleio_characteristic_write_wo_resp_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_characteristic_get_write_wo_resp_obj,
               (mp_obj_t)&bleio_characteristic_set_write_wo_resp_obj,
               (mp_obj_t)&mp_const_none_obj },
};

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

STATIC mp_obj_t bleio_characteristic_get_value(mp_obj_t self_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_characteristic_read_value(self);

    return self->value_data;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_get_value_obj, bleio_characteristic_get_value);

STATIC mp_obj_t bleio_characteristic_set_value(mp_obj_t self_in, mp_obj_t value_in) {
    bleio_characteristic_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(value_in, &bufinfo, MP_BUFFER_READ);

    common_hal_bleio_characteristic_write_value(self, &bufinfo);

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
    { MP_ROM_QSTR(MP_QSTR_write_wo_resp), MP_ROM_PTR(&bleio_characteristic_write_wo_resp_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_locals_dict, bleio_characteristic_locals_dict_table);

const mp_obj_type_t bleio_characteristic_type = {
    { &mp_type_type },
    .name = MP_QSTR_Characteristic,
    .print = bleio_characteristic_print,
    .make_new = bleio_characteristic_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_characteristic_locals_dict
};
