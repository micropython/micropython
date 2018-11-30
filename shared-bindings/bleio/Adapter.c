/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/objproperty.h"
#include "shared-bindings/bleio/Address.h"
#include "shared-bindings/bleio/Adapter.h"

//| .. currentmodule:: bleio
//|
//| :class:`Adapter` --- BLE adapter information
//| ----------------------------------------------------
//|
//| Get current status of the BLE adapter
//|
//| Usage::
//|
//|    import bleio
//|    bleio.adapter.enabled = True
//|    print(bleio.adapter.address)
//|

//| .. class:: Adapter()
//|
//|     You cannot create an instance of `bleio.Adapter`.
//|     Use `bleio.adapter` to access the sole instance available.
//|

//|     .. attribute:: adapter.enabled
//|
//|         State of the BLE adapter.
//|

//|     .. attribute:: adapter.address
//|
//|         MAC address of the BLE adapter. (read-only)
//|

STATIC mp_obj_t bleio_adapter_get_enabled(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_bleio_adapter_get_enabled());
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_enabled_obj, bleio_adapter_get_enabled);

static mp_obj_t bleio_adapter_set_enabled(mp_obj_t self, mp_obj_t value) {
    const bool enabled = mp_obj_is_true(value);

    common_hal_bleio_adapter_set_enabled(enabled);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_adapter_set_enabled_obj, bleio_adapter_set_enabled);

const mp_obj_property_t bleio_adapter_enabled_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_enabled_obj,
               (mp_obj_t)&bleio_adapter_set_enabled_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC mp_obj_t bleio_adapter_get_address(mp_obj_t self) {
    mp_obj_t obj = bleio_address_type.make_new(&bleio_address_type, 1, 0, mp_const_none);
    bleio_address_obj_t *address = MP_OBJ_TO_PTR(obj);

    common_hal_bleio_adapter_get_address(address);

    return obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_adapter_get_address_obj, bleio_adapter_get_address);

const mp_obj_property_t bleio_adapter_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_adapter_get_address_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_adapter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled), MP_ROM_PTR(&bleio_adapter_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&bleio_adapter_address_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_adapter_locals_dict, bleio_adapter_locals_dict_table);

const mp_obj_type_t bleio_adapter_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Adapter,
    .locals_dict = (mp_obj_t)&bleio_adapter_locals_dict,
};
