/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "shared-bindings/bleio/CharacteristicBuffer.h"
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`CharacteristicBuffer` -- BLE Service incoming values buffer.
//| =====================================================================
//|
//| Accumulates a Characteristic's incoming values in a FIFO buffer.
//|
//| .. class:: CharacteristicBuffer(Characteristic, buffer_size=0)
//|
//|   Create a new Characteristic object identified by the specified UUID.
//|
//|   :param bleio.Characteristic characteristic: The characteristic to monitor
//|   :param int buffer_size: Size of ring buffer that stores incoming data coming from client.
//|     Must be >= 1.
//|
STATIC mp_obj_t bleio_characteristic_buffer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_characteristic, ARG_buffer_size, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_characteristic,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_size, MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t characteristic = args[ARG_characteristic].u_obj;
    const int buffer_size = args[ARG_buffer_size].u_int;

    if (buffer_size < 1) {
        mp_raise_ValueError(translate("buffer_size must be >= 1"));
    }

    if (!MP_OBJ_IS_TYPE(characteristic, &bleio_characteristic_type)) {
        mp_raise_ValueError(translate("Expected a Characteristic"));
    }

    bleio_characteristic_buffer_obj_t *self = m_new_obj(bleio_characteristic_buffer_obj_t);
    self->base.type = &bleio_characteristic_buffer_type;
    self->characteristic = MP_OBJ_TO_PTR(characteristic);

    common_hal_bleio_characteristic_buffer_construct(self, self->characteristic, buffer_size);

    return MP_OBJ_FROM_PTR(self);
}


//|   .. method:: read()
//|
//|     Read a single byte from the buffer. If no character is available, return None.
STATIC mp_obj_t bleio_characteristic_buffer_read(mp_obj_t self_in) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int byte = common_hal_bleio_characteristic_buffer_read(self);
    if (byte == -1) {
        return mp_const_none;
    }

    return MP_OBJ_NEW_SMALL_INT(byte);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_buffer_read_obj, bleio_characteristic_buffer_read);

//|   .. method:: deinit()
//|
//|     Disable permanently.
STATIC mp_obj_t bleio_characteristic_buffer_deinit(mp_obj_t self_in) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_bleio_characteristic_buffer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_buffer_deinit_obj, bleio_characteristic_buffer_deinit);

STATIC const mp_rom_map_elem_t bleio_characteristic_buffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read),          MP_ROM_PTR(&bleio_characteristic_buffer_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&bleio_characteristic_buffer_deinit_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_buffer_locals_dict, bleio_characteristic_buffer_locals_dict_table);

const mp_obj_type_t bleio_characteristic_buffer_type = {
    { &mp_type_type },
    .name = MP_QSTR_CharacteristicBuffer,
    .make_new = bleio_characteristic_buffer_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_characteristic_buffer_locals_dict
};
