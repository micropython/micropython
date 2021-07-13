/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/canio/RemoteTransmissionRequest.h"

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| class RemoteTransmissionRequest:
//|     def __init__(self, id: int, length: int, *, extended: bool = False) -> None:
//|         """Construct a RemoteTransmissionRequest to send on a CAN bus.
//|
//|         :param int id: The numeric ID of the requested message
//|         :param int length: The length of the requested message
//|         :param bool extended: True if the message has an extended identifier, False if it has a standard identifier
//|
//|         In CAN, messages can have a length from 0 to 8 bytes.
//|         """
//|         ...
//|
STATIC mp_obj_t canio_remote_transmission_request_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_length, ARG_extended, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_length, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_extended, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int length = args[ARG_length].u_int;
    if (length < 0 || length > 8) {
        mp_raise_ValueError(translate("RemoteTransmissionRequests limited to 8 bytes"));
    }

    canio_remote_transmission_request_obj_t *self = m_new_obj(canio_remote_transmission_request_obj_t);
    self->base.type = &canio_remote_transmission_request_type;
    common_hal_canio_remote_transmission_request_construct(self, args[ARG_id].u_int, length, args[ARG_extended].u_bool);
    return self;
}


//|     id: int
//|     """The numeric ID of the message"""
//|
STATIC mp_obj_t canio_remote_transmission_request_id_get(const mp_obj_t self_in) {
    canio_remote_transmission_request_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_remote_transmission_request_get_id(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_remote_transmission_request_id_get_obj, canio_remote_transmission_request_id_get);

STATIC mp_obj_t canio_remote_transmission_request_id_set(const mp_obj_t self_in, const mp_obj_t id) {
    canio_remote_transmission_request_obj_t *self = self_in;
    common_hal_canio_remote_transmission_request_set_id(self, mp_obj_get_int(id));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_remote_transmission_request_id_set_obj, canio_remote_transmission_request_id_set);

STATIC const mp_obj_property_t canio_remote_transmission_request_id_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_remote_transmission_request_id_get_obj,
              (mp_obj_t)&canio_remote_transmission_request_id_set_obj,
              MP_ROM_NONE},
};

//|     extended: bool
//|     """True if the message's id is an extended id"""
//|
STATIC mp_obj_t canio_remote_transmission_request_extended_get(const mp_obj_t self_in) {
    canio_remote_transmission_request_obj_t *self = self_in;
    return mp_obj_new_bool(common_hal_canio_remote_transmission_request_get_extended(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_remote_transmission_request_extended_get_obj, canio_remote_transmission_request_extended_get);

STATIC mp_obj_t canio_remote_transmission_request_extended_set(const mp_obj_t self_in, const mp_obj_t extended) {
    canio_remote_transmission_request_obj_t *self = self_in;
    common_hal_canio_remote_transmission_request_set_extended(self, mp_obj_is_true(extended));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_remote_transmission_request_extended_set_obj, canio_remote_transmission_request_extended_set);


STATIC const mp_obj_property_t canio_remote_transmission_request_extended_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_remote_transmission_request_extended_get_obj,
              (mp_obj_t)&canio_remote_transmission_request_extended_set_obj,
              MP_ROM_NONE},
};

//|     length: int
//|     """The length of the requested message."""
//|
STATIC mp_obj_t canio_remote_transmission_request_length_get(const mp_obj_t self_in) {
    canio_remote_transmission_request_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_remote_transmission_request_get_length(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_remote_transmission_request_length_get_obj, canio_remote_transmission_request_length_get);

STATIC mp_obj_t canio_remote_transmission_request_length_set(const mp_obj_t self_in, const mp_obj_t length_in) {
    canio_remote_transmission_request_obj_t *self = self_in;
    int length = mp_obj_get_int(length_in);
    if (length < 0 || length > 8) {
        mp_raise_ValueError(translate("RemoteTransmissionRequests limited to 8 bytes"));
    }
    common_hal_canio_remote_transmission_request_set_length(self, length);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_remote_transmission_request_length_set_obj, canio_remote_transmission_request_length_set);


STATIC const mp_obj_property_t canio_remote_transmission_request_length_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_remote_transmission_request_length_get_obj,
              (mp_obj_t)&canio_remote_transmission_request_length_set_obj,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t canio_remote_transmission_request_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&canio_remote_transmission_request_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_length), MP_ROM_PTR(&canio_remote_transmission_request_length_obj) },
    { MP_ROM_QSTR(MP_QSTR_extended), MP_ROM_PTR(&canio_remote_transmission_request_extended_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canio_remote_transmission_request_locals_dict, canio_remote_transmission_request_locals_dict_table);

const mp_obj_type_t canio_remote_transmission_request_type = {
    { &mp_type_type },
    .name = MP_QSTR_RemoteTransmissionRequest,
    .make_new = canio_remote_transmission_request_make_new,
    .locals_dict = (mp_obj_t)&canio_remote_transmission_request_locals_dict,
};
