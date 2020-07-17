// Copyright (c) 2014 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_MODUBINASCII_H
#define MICROPY_INCLUDED_EXTMOD_MODUBINASCII_H

extern mp_obj_t mod_binascii_hexlify(size_t n_args, const mp_obj_t *args);
extern mp_obj_t mod_binascii_unhexlify(mp_obj_t data);
extern mp_obj_t mod_binascii_a2b_base64(mp_obj_t data);
extern mp_obj_t mod_binascii_b2a_base64(mp_obj_t data);
extern mp_obj_t mod_binascii_crc32(size_t n_args, const mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mod_binascii_hexlify_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mod_binascii_unhexlify_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mod_binascii_a2b_base64_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mod_binascii_b2a_base64_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mod_binascii_crc32_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MODUBINASCII_H
