// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

static const mp_rom_map_elem_t bleio_attribute_locals_dict_table[] = {

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
static MP_DEFINE_CONST_DICT(bleio_attribute_locals_dict, bleio_attribute_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    bleio_attribute_type,
    MP_QSTR_Attribute,
    MP_TYPE_FLAG_NONE,
    locals_dict, &bleio_attribute_locals_dict
    );
