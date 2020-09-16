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

#include "shared-bindings/_canio/Message.h"
#include "shared-module/_canio/Message.h"

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| class Message:
//|     def __init__(self, id: int=0, data: Optional[bytes] = None, *, size: Optional[int] = None, rtr: bool = False):
//|         """Construct a Message to send on a CAN bus
//|
//|         :param int id: The numeric ID of the message
//|         :param bytes data: The content of the message
//|         :param int size: The amount of data requested, for an rtr
//|         :param bool rtr: True if the message represents an rtr (Remote Transmission Request)
//|
//|         In CAN, messages can have a size from 0 to 8 bytes.
//|
//|         For a non-rtr message, specify `data`.  For an rtr-message, specify either `data` (a dummy buffer of the requested size) or `size`.
//|         """
//|         ...
//|
STATIC mp_obj_t canio_message_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_data, ARG_size, ARG_rtr, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_obj = 0} },
        { MP_QSTR_data, MP_ARG_OBJ, {.u_obj = 0} },
        { MP_QSTR_size, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rtr, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT( MP_ARRAY_SIZE(allowed_args) == NUM_ARGS );

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool rtr = args[ARG_rtr].u_bool;
    size_t size = (size_t)args[ARG_size].u_int;
    bool specified_size = (size != (size_t)-1);
    bool specified_data = (args[ARG_data].u_obj != NULL);

    if(specified_size && specified_data) {
        mp_raise_TypeError(translate("specify size or data, but not both"));
    }

    mp_buffer_info_t data;
    if (specified_data) {
        mp_get_buffer_raise(args[ARG_data].u_obj, &data, MP_BUFFER_READ);
    } else if (specified_size) {
        data.buf = 0;
        data.len = size;
    } else {
        data.buf = 0;
        data.len = 0;
    }

    if(data.len > 8) {
        mp_raise_ValueError(translate("Messages limited to 8 bytes"));
    }

    canio_message_obj_t *self = m_new_obj(canio_message_obj_t);
    self->base.type = &canio_message_type;
    common_hal_canio_message_construct(self, args[ARG_id].u_int, data.buf, data.len, rtr);
    return self;
}

//|     id: int
//|     """The numeric ID of the message"""
//|
STATIC mp_obj_t canio_message_id_get(const mp_obj_t self_in) {
    canio_message_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_message_id_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_message_id_get_obj, canio_message_id_get);

STATIC mp_obj_t canio_message_id_set(const mp_obj_t self_in, const mp_obj_t id) {
    canio_message_obj_t *self = self_in;
    common_hal_canio_message_id_set(self, mp_obj_get_int(id));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_message_id_set_obj, canio_message_id_set);

STATIC const mp_obj_property_t canio_message_id_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_message_id_get_obj,
              (mp_obj_t)&canio_message_id_set_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     data: bytes
//|     """The content of the message, or dummy content in the case of an rtr.
//|
//|     Assigning to data also sets the length."""
//|
STATIC mp_obj_t canio_message_data_get(const mp_obj_t self_in) {
    canio_message_obj_t *self = self_in;
    return mp_obj_new_bytes((const byte*)common_hal_canio_message_data_get(self), common_hal_canio_message_size_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_message_data_get_obj, canio_message_data_get);

STATIC mp_obj_t canio_message_data_set(const mp_obj_t self_in, const mp_obj_t data_in) {
    canio_message_obj_t *self = self_in;
    mp_buffer_info_t data;
    mp_get_buffer_raise(data_in, &data, MP_BUFFER_READ);
    if(data.len > 8) {
        mp_raise_ValueError(translate("Messages limited to 8 bytes"));
    }
    common_hal_canio_message_data_set(self, data.buf, data.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_message_data_set_obj, canio_message_data_set);


STATIC const mp_obj_property_t canio_message_data_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_message_data_get_obj,
              (mp_obj_t)&canio_message_data_set_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|     size: int
//|     """The length of the message, or the length of the requested data in the case of an rtr
//|
//|     Assigning to the length sets all the data bytes to zero"""
//|
STATIC mp_obj_t canio_message_size_get(const mp_obj_t self_in) {
    canio_message_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_message_size_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_message_size_get_obj, canio_message_size_get);

STATIC mp_obj_t canio_message_size_set(const mp_obj_t self_in, const mp_obj_t size_in) {
    canio_message_obj_t *self = self_in;
    int size = mp_obj_get_int(size_in);
    if(size > 8) {
        mp_raise_ValueError(translate("Messages limited to 8 bytes"));
    }
    common_hal_canio_message_size_set(self, size);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_message_size_set_obj, canio_message_size_set);


STATIC const mp_obj_property_t canio_message_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_message_size_get_obj,
              (mp_obj_t)&canio_message_size_set_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     rtr: bool
//|     """True if the message represents a remote transmission request (RTR)"""
//|
STATIC mp_obj_t canio_message_rtr_get(const mp_obj_t self_in) {
    canio_message_obj_t *self = self_in;
    return mp_obj_new_bool(common_hal_canio_message_rtr_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_message_rtr_get_obj, canio_message_rtr_get);

STATIC mp_obj_t canio_message_rtr_set(const mp_obj_t self_in, const mp_obj_t rtr) {
    canio_message_obj_t *self = self_in;
    common_hal_canio_message_size_set(self, mp_obj_is_true(rtr));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_message_rtr_set_obj, canio_message_rtr_set);


STATIC const mp_obj_property_t canio_message_rtr_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_message_rtr_get_obj,
              (mp_obj_t)&canio_message_rtr_set_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t canio_message_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&canio_message_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&canio_message_data_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&canio_message_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_rtr), MP_ROM_PTR(&canio_message_rtr_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canio_message_locals_dict, canio_message_locals_dict_table);

const mp_obj_type_t canio_message_type = {
    { &mp_type_type },
    .name = MP_QSTR_Message,
    .make_new = canio_message_make_new,
    .locals_dict = (mp_obj_t)&canio_message_locals_dict,
};
