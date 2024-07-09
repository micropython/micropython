// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "__init__.h"

//| """AES encryption routines
//|
//| The `AES` module contains classes used to implement encryption
//| and decryption. It aims to be low overhead in terms of memory.
//|
//| For more information on AES, refer to `the Wikipedia entry
//| <https://en.wikipedia.org/wiki/Advanced_Encryption_Standard>`_.
//| """


static const mp_obj_tuple_t mp_aes_key_size_obj = {
    {&mp_type_tuple},
    3,
    {
        MP_OBJ_NEW_SMALL_INT(16),
        MP_OBJ_NEW_SMALL_INT(24),
        MP_OBJ_NEW_SMALL_INT(32),
    }
};

static const mp_rom_map_elem_t aesio_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_aesio)},
    {MP_ROM_QSTR(MP_QSTR_AES), MP_ROM_PTR(&aesio_aes_type) },
    {MP_ROM_QSTR(MP_QSTR_MODE_ECB), MP_ROM_INT(AES_MODE_ECB)},
    {MP_ROM_QSTR(MP_QSTR_MODE_CBC), MP_ROM_INT(AES_MODE_CBC)},
    {MP_ROM_QSTR(MP_QSTR_MODE_CTR), MP_ROM_INT(AES_MODE_CTR)},
    {MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_INT(AES_BLOCKLEN)},
    {MP_ROM_QSTR(MP_QSTR_key_size), (mp_obj_t)&mp_aes_key_size_obj},
};

static MP_DEFINE_CONST_DICT(aesio_module_globals, aesio_module_globals_table);

const mp_obj_module_t aesio_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&aesio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_aesio, aesio_module);
