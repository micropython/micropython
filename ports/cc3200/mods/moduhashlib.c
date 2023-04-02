/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2015 Daniel Campora
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

#include <assert.h>
#include <string.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_shamd5.h"
#include "inc/hw_dthe.h"
#include "hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "shamd5.h"
#include "cryptohash.h"


/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/
typedef struct _mp_obj_hash_t {
    mp_obj_base_t base;
    uint8_t *buffer;
    uint32_t b_size;
    uint32_t c_size;
    uint8_t  algo;
    uint8_t  h_size;
    bool  fixedlen;
    bool  digested;
    uint8_t hash[32];
} mp_obj_hash_t;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void hash_update_internal(mp_obj_t self_in, mp_obj_t data, bool digest);
STATIC mp_obj_t hash_read (mp_obj_t self_in);

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void hash_update_internal(mp_obj_t self_in, mp_obj_t data, bool digest) {
    mp_obj_hash_t *self = self_in;
    mp_buffer_info_t bufinfo;

    if (data) {
        mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    }

    if (digest) {
        CRYPTOHASH_SHAMD5Start (self->algo, self->b_size);
    }

    if (self->c_size < self->b_size || !data || !self->fixedlen) {
        if (digest || self->fixedlen) {
            // no data means we want to process our internal buffer
            CRYPTOHASH_SHAMD5Update (data ? bufinfo.buf : self->buffer, data ? bufinfo.len : self->b_size);
            self->c_size += data ? bufinfo.len : 0;
        } else {
            self->buffer = m_renew(byte, self->buffer, self->b_size, self->b_size + bufinfo.len);
            mp_seq_copy((byte*)self->buffer + self->b_size, bufinfo.buf, bufinfo.len, byte);
            self->b_size += bufinfo.len;
            self->digested = false;
        }
    } else {
        mp_raise_OSError(MP_EPERM);
    }
}

STATIC mp_obj_t hash_read (mp_obj_t self_in) {
    mp_obj_hash_t *self = self_in;

    if (!self->fixedlen) {
        if (!self->digested) {
            hash_update_internal(self, MP_OBJ_NULL, true);
        }
    } else if (self->c_size < self->b_size) {
        // it's a fixed len block which is still incomplete
        mp_raise_OSError(MP_EPERM);
    }

    if (!self->digested) {
        CRYPTOHASH_SHAMD5Read ((uint8_t *)self->hash);
        self->digested = true;
    }
    return mp_obj_new_bytes(self->hash, self->h_size);
}

/******************************************************************************/
// MicroPython bindings

/// \classmethod \constructor([data[, block_size]])
/// initial data must be given if block_size wants to be passed
STATIC mp_obj_t hash_make_new(mp_obj_t type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 2, false);
    mp_obj_hash_t *self = m_new0(mp_obj_hash_t, 1);
    self->base.type = type_in;
    if (self->base.type->name == MP_QSTR_sha1) {
        self->algo = SHAMD5_ALGO_SHA1;
        self->h_size = 20;
    } else /* if (self->base.type->name == MP_QSTR_sha256) */ {
        self->algo = SHAMD5_ALGO_SHA256;
        self->h_size = 32;
    } /* else {
        self->algo = SHAMD5_ALGO_MD5;
        self->h_size = 32;
    } */

    if (n_args) {
        // CPython extension to avoid buffering the data before digesting it
        // Note: care must be taken to provide all intermediate blocks as multiple
        //       of four bytes, otherwise the resulting hash will be incorrect.
        //       the final block can be of any length
        if (n_args > 1) {
            // block size given, we will feed the data directly into the hash engine
            self->fixedlen = true;
            self->b_size = mp_obj_get_int(args[1]);
            hash_update_internal(self, args[0], true);
        } else {
            hash_update_internal(self, args[0], false);
        }
    }
    return self;
}

STATIC mp_obj_t hash_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = self_in;
    hash_update_internal(self, arg, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(hash_update_obj, hash_update);

STATIC mp_obj_t hash_digest(mp_obj_t self_in) {
    return hash_read(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(hash_digest_obj, hash_digest);

STATIC const mp_rom_map_elem_t hash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update),    MP_ROM_PTR(&hash_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest),    MP_ROM_PTR(&hash_digest_obj) },
};

STATIC MP_DEFINE_CONST_DICT(hash_locals_dict, hash_locals_dict_table);

//STATIC const mp_obj_type_t md5_type = {
//    { &mp_type_type },
//    .name = MP_QSTR_md5,
//    .make_new = hash_make_new,
//    .locals_dict = (mp_obj_t)&hash_locals_dict,
//};

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    sha1_type,
    MP_QSTR_sha1,
    MP_TYPE_FLAG_NONE,
    make_new, hash_make_new,
    locals_dict, &hash_locals_dict
    );

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    sha256_type,
    MP_QSTR_sha256,
    MP_TYPE_FLAG_NONE,
    make_new, hash_make_new,
    locals_dict, &hash_locals_dict
    );

STATIC const mp_rom_map_elem_t mp_module_hashlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_uhashlib) },
    //{ MP_ROM_QSTR(MP_QSTR_md5),         MP_ROM_PTR(&md5_type) },
    { MP_ROM_QSTR(MP_QSTR_sha1),        MP_ROM_PTR(&sha1_type) },
    { MP_ROM_QSTR(MP_QSTR_sha256),      MP_ROM_PTR(&sha256_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_hashlib_globals, mp_module_hashlib_globals_table);

const mp_obj_module_t mp_module_uhashlib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_hashlib_globals,
};

