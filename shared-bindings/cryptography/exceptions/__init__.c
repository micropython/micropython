// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>

#include "py/objexcept.h"
#include "py/runtime.h"

#include "shared-bindings/cryptography/exceptions/__init__.h"

//| """Exceptions
//|
//| A subset of :py:mod:`cryptography.exceptions`: just `InvalidSignature`, so
//| that code written against this module can use
//| ``except cryptography.exceptions.InvalidSignature:`` unchanged on CPython.
//| """
//|

//| class InvalidSignature(Exception):
//|     """Raised by
//|     :py:meth:`cryptography.hazmat.primitives.asymmetric.ec.EllipticCurvePublicKey.verify`
//|     when a signature does not verify, or is malformed. Mirrors
//|     :py:class:`cryptography.exceptions.InvalidSignature`."""
//|
//|     ...
//|
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_cryptography_InvalidSignature,
    MP_QSTR_InvalidSignature,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_exception_make_new,
    print, mp_obj_exception_print,
    attr, mp_obj_exception_attr,
    parent, &mp_type_Exception
    );

MP_NORETURN void mp_raise_cryptography_InvalidSignature(mp_rom_error_text_t fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_cryptography_InvalidSignature, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

static mp_rom_map_elem_t cryptography_exceptions_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cryptography_dot_exceptions) },
    { MP_ROM_QSTR(MP_QSTR_InvalidSignature), MP_ROM_PTR(&mp_type_cryptography_InvalidSignature) },
};
static MP_DEFINE_CONST_DICT(cryptography_exceptions_module_globals, cryptography_exceptions_module_globals_table);

const mp_obj_module_t cryptography_exceptions_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cryptography_exceptions_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cryptography_dot_exceptions, cryptography_exceptions_module);
