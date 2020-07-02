/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "__init__.h"

//| """AES encryption routines
//|
//| The `AES` module contains classes used to implement encryption
//| and decryption. It aims to be low overhead in terms of memory."""


STATIC const mp_obj_tuple_t mp_aes_key_size_obj = {
    {&mp_type_tuple},
    3,
    {
        MP_OBJ_NEW_SMALL_INT(16),
        MP_OBJ_NEW_SMALL_INT(24),
        MP_OBJ_NEW_SMALL_INT(32),
    }
};

STATIC const mp_rom_map_elem_t aesio_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_aesio)},
    {MP_ROM_QSTR(MP_QSTR_AES), MP_ROM_PTR(&aesio_aes_type) },
    {MP_ROM_QSTR(MP_QSTR_MODE_ECB), MP_ROM_INT(AES_MODE_ECB)},
    {MP_ROM_QSTR(MP_QSTR_MODE_CBC), MP_ROM_INT(AES_MODE_CBC)},
    {MP_ROM_QSTR(MP_QSTR_MODE_CTR), MP_ROM_INT(AES_MODE_CTR)},
    {MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_INT(AES_BLOCKLEN)},
    {MP_ROM_QSTR(MP_QSTR_key_size), (mp_obj_t)&mp_aes_key_size_obj},
};

STATIC MP_DEFINE_CONST_DICT(aesio_module_globals, aesio_module_globals_table);

const mp_obj_module_t aesio_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&aesio_module_globals,
};
