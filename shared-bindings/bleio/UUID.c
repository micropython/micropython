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
#include "shared-bindings/bleio/UUID.h"

//| .. currentmodule:: bleio
//|
//| :class:`UUID` -- BLE UUID
//| =========================================================
//|
//| Encapsulates both 16-bit and 128-bit UUIDs. Can be used for services,
//| characteristics, descriptors and more.
//|

//| .. class:: UUID(uuid)
//|
//|   Create a new UUID object encapsulating the uuid value.
//|   The value itself can be one of:
//|
//|   - a `int` value in range of 0 to 0xFFFF
//|   - a `str` value in the format of '0xXXXX' for 16-bit or 'XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX' for 128-bit
//|
//|   :param int/str uuid: The uuid to encapsulate
//|

//|   .. attribute:: type
//|
//|     The UUID type. One of:
//|
//|     - `bleio.UUIDType.TYPE_16BIT`
//|     - `bleio.UUIDType.TYPE_128BIT`
//|
STATIC mp_obj_t bleio_uuid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    bleio_uuid_obj_t *self = m_new_obj(bleio_uuid_obj_t);
    self->base.type = &bleio_uuid_type;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);

    enum { ARG_uuid };
    static const mp_arg_t allowed_args[] = {
        { ARG_uuid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t uuid = args[ARG_uuid].u_obj;

    common_hal_bleio_uuid_construct(self, &uuid);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void bleio_uuid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_bleio_uuid_print(self, print);
}

STATIC mp_obj_t bleio_uuid_get_type(mp_obj_t self_in) {
    bleio_uuid_obj_t *self = MP_OBJ_TO_PTR(self_in);

    const bleio_uuid_type_t type = common_hal_bleio_uuid_get_type(self);
    if (type == UUID_TYPE_16BIT) {
        return (mp_obj_t)&bleio_uuidtype_16bit_obj;
    }

    if (type == UUID_TYPE_128BIT) {
        return (mp_obj_t)&bleio_uuidtype_128bit_obj;
    }

    return (mp_obj_t)&mp_const_none_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_uuid_get_type_obj, bleio_uuid_get_type);

const mp_obj_property_t bleio_uuid_type_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_uuid_get_type_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t bleio_uuid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_type), MP_ROM_PTR(&bleio_uuid_type_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_uuid_locals_dict, bleio_uuid_locals_dict_table);

const mp_obj_type_t bleio_uuid_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUID,
    .print = bleio_uuid_print,
    .make_new = bleio_uuid_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_uuid_locals_dict
};
