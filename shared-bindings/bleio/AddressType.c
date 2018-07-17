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

#include "shared-bindings/bleio/AddressType.h"

//| .. currentmodule:: bleio
//|
//| :class:`AddressType` -- defines the type of a BLE address
//| =============================================================
//|
//| .. class:: bleio.AddressType
//|
//|     Enum-like class to define the type of a BLE address, see also `bleio.Address`.
//|
//|     .. data:: PUBLIC
//|
//|       The address is public
//|
//|     .. data:: RANDOM_STATIC
//|
//|       The address is random static
//|
//|     .. data:: RANDOM_PRIVATE_RESOLVABLE
//|
//|       The address is random private resolvable
//|
//|     .. data:: RANDOM_PRIVATE_NON_RESOLVABLE
//|
//|       The address is private non-resolvable
//|
const mp_obj_type_t bleio_addresstype_type;

const bleio_addresstype_obj_t bleio_addresstype_public_obj = {
    { &bleio_addresstype_type },
};

const bleio_addresstype_obj_t bleio_addresstype_random_static_obj = {
    { &bleio_addresstype_type },
};

const bleio_addresstype_obj_t bleio_addresstype_random_private_resolvable_obj = {
    { &bleio_addresstype_type },
};

const bleio_addresstype_obj_t bleio_addresstype_random_private_non_resolvable_obj = {
    { &bleio_addresstype_type },
};

STATIC const mp_rom_map_elem_t bleio_addresstype_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PUBLIC),                        MP_ROM_PTR(&bleio_addresstype_public_obj) },
    { MP_ROM_QSTR(MP_QSTR_RANDOM_STATIC),                 MP_ROM_PTR(&bleio_addresstype_random_static_obj) },
    { MP_ROM_QSTR(MP_QSTR_RANDOM_PRIVATE_RESOLVABLE),     MP_ROM_PTR(&bleio_addresstype_random_private_resolvable_obj) },
    { MP_ROM_QSTR(MP_QSTR_RANDOM_PRIVATE_NON_RESOLVABLE), MP_ROM_PTR(&bleio_addresstype_random_private_non_resolvable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bleio_addresstype_locals_dict, bleio_addresstype_locals_dict_table);

STATIC void bleio_addresstype_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr type = MP_QSTR_PUBLIC;

    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&bleio_addresstype_random_static_obj)) {
        type = MP_QSTR_RANDOM_STATIC;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&bleio_addresstype_random_private_resolvable_obj)) {
        type = MP_QSTR_RANDOM_PRIVATE_RESOLVABLE;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&bleio_addresstype_random_private_non_resolvable_obj)) {
        type = MP_QSTR_RANDOM_PRIVATE_NON_RESOLVABLE;
    }

    mp_printf(print, "%q.%q.%q", MP_QSTR_bleio, MP_QSTR_AddressType, type);
}

const mp_obj_type_t bleio_addresstype_type = {
    { &mp_type_type },
    .name = MP_QSTR_AddressType,
    .print = bleio_addresstype_print,
    .locals_dict = (mp_obj_t)&bleio_addresstype_locals_dict,
};
