/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "shared-bindings/bleio/UUIDType.h"

//| .. currentmodule:: bleio
//|
//| :class:`UUIDType` -- defines the type of a BLE UUID
//| =============================================================
//|
//| .. class:: bleio.UUIDType
//|
//|     Enum-like class to define the type of a BLE UUID.
//|
//|     .. data:: TYPE_16BIT
//|
//|       The UUID is 16-bit
//|
//|     .. data:: TYPE_128BIT
//|
//|       The UUID is 128-bit
//|
const mp_obj_type_t bleio_uuidtype_type;

const bleio_uuidtype_obj_t bleio_uuidtype_16bit_obj = {
    { &bleio_uuidtype_type },
};

const bleio_uuidtype_obj_t bleio_uuidtype_128bit_obj = {
    { &bleio_uuidtype_type },
};

STATIC const mp_rom_map_elem_t bleio_uuidtype_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TYPE_16BIT),   MP_ROM_PTR(&bleio_uuidtype_16bit_obj) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_128BIT),  MP_ROM_PTR(&bleio_uuidtype_128bit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bleio_uuidtype_locals_dict, bleio_uuidtype_locals_dict_table);

STATIC void bleio_uuidtype_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr type = MP_QSTR_TYPE_128BIT;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&bleio_uuidtype_16bit_obj)) {
        type = MP_QSTR_TYPE_16BIT;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_bleio, MP_QSTR_UUIDType, type);
}

const mp_obj_type_t bleio_uuidtype_type = {
    { &mp_type_type },
    .name = MP_QSTR_UUIDType,
    .print = bleio_uuidtype_print,
    .locals_dict = (mp_obj_t)&bleio_uuidtype_locals_dict,
};
