/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Paul Sokolovsky
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

#include "py/mpconfig.h"
#if MICROPY_PY_UCRYPTOLIB

#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "py/runtime.h"

// This module implements crypto ciphers API, roughly following
// https://www.python.org/dev/peps/pep-0272/ . Exact implementation
// of PEP 272 can be made with a simple wrapper which adds all the
// needed boilerplate.

#if MICROPY_SSL_AXTLS
#include "lib/axtls/crypto/crypto.h"
#endif

#define MODE_ECB 1
#define MODE_CBC 2

typedef struct _mp_obj_aes_t {
    mp_obj_base_t base;
    AES_CTX ctx;
    uint8_t mode: 7;
    uint8_t is_decrypt_key: 1;
} mp_obj_aes_t;

STATIC mp_obj_t aes_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 3, false);
    mp_obj_aes_t *o = m_new_obj(mp_obj_aes_t);
    o->base.type = type;

    o->mode = mp_obj_get_int(args[1]);
    o->is_decrypt_key = 0;

    if (o->mode < MODE_ECB || o->mode > MODE_CBC) {
        mp_raise_ValueError("mode");
    }

    mp_buffer_info_t keyinfo;
    mp_get_buffer_raise(args[0], &keyinfo, MP_BUFFER_READ);

    mp_buffer_info_t ivinfo;
    ivinfo.buf = NULL;
    if (n_args > 2 && args[2] != mp_const_none) {
        mp_get_buffer_raise(args[2], &ivinfo, MP_BUFFER_READ);
    }

    AES_MODE keysize = AES_MODE_128;
    if (keyinfo.len == 32) {
        keysize = AES_MODE_256;
    }

    AES_set_key(&o->ctx, keyinfo.buf, ivinfo.buf, keysize);

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t aes_process(size_t n_args, const mp_obj_t *args, bool encrypt) {
    mp_obj_aes_t *self = MP_OBJ_TO_PTR(args[0]);

    if (encrypt && self->is_decrypt_key) {
        mp_raise_TypeError("can't enc after dec");
    }

    mp_obj_t in_buf = args[1];
    mp_obj_t out_buf = MP_OBJ_NULL;
    if (n_args > 2) {
        out_buf = args[2];
    }

    mp_buffer_info_t in_bufinfo;
    mp_get_buffer_raise(in_buf, &in_bufinfo, MP_BUFFER_READ);

    if (in_bufinfo.len % 16 != 0) {
        mp_raise_ValueError("blksize % 16");
    }

    vstr_t vstr;
    mp_buffer_info_t out_bufinfo;
    uint8_t *out_buf_ptr;

    if (out_buf != MP_OBJ_NULL) {
        mp_get_buffer_raise(out_buf, &out_bufinfo, MP_BUFFER_WRITE);
        if (out_bufinfo.len < in_bufinfo.len) {
            mp_raise_ValueError("out blksize");
        }
        out_buf_ptr = out_bufinfo.buf;
    } else {
        vstr_init_len(&vstr, in_bufinfo.len);
        out_buf_ptr = (uint8_t*)vstr.buf;
    }

    if (!encrypt && !self->is_decrypt_key) {
        AES_convert_key(&self->ctx);
        self->is_decrypt_key = 1;
    }

    if (self->mode == MODE_ECB) {
            uint8_t *in = in_bufinfo.buf, *out = out_buf_ptr;
            uint8_t *top = in + in_bufinfo.len;
            for (; in < top; in += 16, out += 16) {
                memcpy(out, in, 16);
                // We assume that vstr.buf is uint32_t aligned
                uint32_t *p = (uint32_t*)out;
                // axTLS likes it weird and complicated with byteswaps
                for (int i = 0; i < 4; i++) {
                    p[i] = MP_HTOBE32(p[i]);
                }
                if (encrypt) {
                    AES_encrypt(&self->ctx, p);
                } else {
                    AES_decrypt(&self->ctx, p);
                }
                for (int i = 0; i < 4; i++) {
                    p[i] = MP_BE32TOH(p[i]);
                }
            }
    } else {
        if (encrypt) {
            AES_cbc_encrypt(&self->ctx, in_bufinfo.buf, out_buf_ptr, in_bufinfo.len);
        } else {
            AES_cbc_decrypt(&self->ctx, in_bufinfo.buf, out_buf_ptr, in_bufinfo.len);
        }
    }

    if (out_buf != MP_OBJ_NULL) {
        return out_buf;
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}

STATIC mp_obj_t aes_encrypt(size_t n_args, const mp_obj_t *args) {
    return aes_process(n_args, args, true);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(aes_encrypt_obj, 2, 3, aes_encrypt);

STATIC mp_obj_t aes_decrypt(size_t n_args, const mp_obj_t *args) {
    return aes_process(n_args, args, false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(aes_decrypt_obj, 2, 3, aes_decrypt);

STATIC const mp_rom_map_elem_t aes_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_encrypt), MP_ROM_PTR(&aes_encrypt_obj) },
    { MP_ROM_QSTR(MP_QSTR_decrypt), MP_ROM_PTR(&aes_decrypt_obj) },
};
STATIC MP_DEFINE_CONST_DICT(aes_locals_dict, aes_locals_dict_table);

STATIC const mp_obj_type_t aes_type = {
    { &mp_type_type },
    .name = MP_QSTR_aes,
    .make_new = aes_make_new,
    .locals_dict = (void*)&aes_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_ucryptolib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ucryptolib) },
    { MP_ROM_QSTR(MP_QSTR_aes), MP_ROM_PTR(&aes_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ucryptolib_globals, mp_module_ucryptolib_globals_table);

const mp_obj_module_t mp_module_ucryptolib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ucryptolib_globals,
};

#endif //MICROPY_PY_UCRYPTOLIB
