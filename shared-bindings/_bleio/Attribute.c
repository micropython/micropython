/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/UUID.h"

//| class Attribute:
//|     """Definitions associated with all BLE attributes: characteristics, descriptors, etc.
//|
//|     :py:class:`~_bleio.Attribute` is, notionally, a superclass of
//|     :py:class:`~Characteristic` and :py:class:`~Descriptor`,
//|     but is not defined as a Python superclass of those classes."""
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of :py:class:`~_bleio.Attribute`."""
//|         ...
//|

STATIC const mp_rom_map_elem_t bleio_attribute_locals_dict_table[] = {

//|     NO_ACCESS: int
//|     """security mode: access not allowed"""
//|
//|     OPEN: int
//|     """security_mode: no security (link is not encrypted)"""
//|
//|     ENCRYPT_NO_MITM: int
//|     """security_mode: unauthenticated encryption, without man-in-the-middle protection"""
//|
//|     ENCRYPT_WITH_MITM: int
//|     """security_mode: authenticated encryption, with man-in-the-middle protection"""
//|
//|     LESC_ENCRYPT_WITH_MITM: int
//|     """security_mode: LESC encryption, with man-in-the-middle protection"""
//|
//|     SIGNED_NO_MITM: int
//|     """security_mode: unauthenticated data signing, without man-in-the-middle protection"""
//|
//|     SIGNED_WITH_MITM: int
//|     """security_mode: authenticated data signing, without man-in-the-middle protection"""
//|
    { MP_ROM_QSTR(MP_QSTR_NO_ACCESS),              MP_ROM_INT(SECURITY_MODE_NO_ACCESS) },
    { MP_ROM_QSTR(MP_QSTR_OPEN),                   MP_ROM_INT(SECURITY_MODE_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_ENCRYPT_NO_MITM),        MP_ROM_INT(SECURITY_MODE_ENC_NO_MITM) },
    { MP_ROM_QSTR(MP_QSTR_ENCRYPT_WITH_MITM),      MP_ROM_INT(SECURITY_MODE_ENC_WITH_MITM) },
    { MP_ROM_QSTR(MP_QSTR_LESC_ENCRYPT_WITH_MITM), MP_ROM_INT(SECURITY_MODE_LESC_ENC_WITH_MITM) },
    { MP_ROM_QSTR(MP_QSTR_SIGNED_NO_MITM),         MP_ROM_INT(SECURITY_MODE_SIGNED_NO_MITM) },
    { MP_ROM_QSTR(MP_QSTR_SIGNED_WITH_MITM),       MP_ROM_INT(SECURITY_MODE_SIGNED_WITH_MITM) },

};
STATIC MP_DEFINE_CONST_DICT(bleio_attribute_locals_dict, bleio_attribute_locals_dict_table);

const mp_obj_type_t bleio_attribute_type = {
    { &mp_type_type },
    .name = MP_QSTR_Attribute,
    .locals_dict = (mp_obj_dict_t *)&bleio_attribute_locals_dict,
};
