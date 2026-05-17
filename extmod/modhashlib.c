/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
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

#include "py/runtime.h"

#if MICROPY_PY_HASHLIB

#if MICROPY_SSL_MBEDTLS
#include "mbedtls/version.h"
#endif

#if MICROPY_PY_HASHLIB_SHA256

#if MICROPY_SSL_MBEDTLS
#include "mbedtls/sha256.h"
#else
#include "lib/crypto-algorithms/sha256.h"
#endif

#endif

#if MICROPY_PY_HASHLIB_SHA1 || MICROPY_PY_HASHLIB_MD5

#if MICROPY_SSL_AXTLS
#include "lib/axtls/crypto/crypto.h"
#endif

#if MICROPY_SSL_MBEDTLS
#include "mbedtls/md5.h"
#include "mbedtls/sha1.h"
#endif

#endif

typedef struct _mp_obj_hash_t {
    mp_obj_base_t base;
    bool final; // if set, update and digest raise an exception
    uintptr_t state[0]; // must be aligned to a machine word
} mp_obj_hash_t;

static void hashlib_ensure_not_final(mp_obj_hash_t *self) {
    if (self->final) {
        mp_raise_ValueError(MP_ERROR_TEXT("hash is final"));
    }
}

#if MICROPY_PY_HASHLIB_SHA256
static mp_obj_t hashlib_sha256_update(mp_obj_t self_in, mp_obj_t arg);

#if MICROPY_SSL_MBEDTLS

#if MBEDTLS_VERSION_NUMBER < 0x02070000 || MBEDTLS_VERSION_NUMBER >= 0x03000000
#define mbedtls_sha256_starts_ret mbedtls_sha256_starts
#define mbedtls_sha256_update_ret mbedtls_sha256_update
#define mbedtls_sha256_finish_ret mbedtls_sha256_finish
#endif

static mp_obj_t hashlib_sha256_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(mbedtls_sha256_context), type);
    o->final = false;
    mbedtls_sha256_init((mbedtls_sha256_context *)&o->state);
    mbedtls_sha256_starts_ret((mbedtls_sha256_context *)&o->state, 0);
    if (n_args == 1) {
        hashlib_sha256_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_sha256_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    mbedtls_sha256_update_ret((mbedtls_sha256_context *)&self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_sha256_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, 32);
    mbedtls_sha256_finish_ret((mbedtls_sha256_context *)&self->state, (unsigned char *)vstr.buf);
    return mp_obj_new_bytes_from_vstr(&vstr);
}

#else

#include "lib/crypto-algorithms/sha256.c"

static mp_obj_t hashlib_sha256_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(CRYAL_SHA256_CTX), type);
    o->final = false;
    sha256_init((CRYAL_SHA256_CTX *)o->state);
    if (n_args == 1) {
        hashlib_sha256_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_sha256_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    sha256_update((CRYAL_SHA256_CTX *)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_sha256_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, SHA256_BLOCK_SIZE);
    sha256_final((CRYAL_SHA256_CTX *)self->state, (byte *)vstr.buf);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
#endif

static MP_DEFINE_CONST_FUN_OBJ_2(hashlib_sha256_update_obj, hashlib_sha256_update);
static MP_DEFINE_CONST_FUN_OBJ_1(hashlib_sha256_digest_obj, hashlib_sha256_digest);

static const mp_rom_map_elem_t hashlib_sha256_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hashlib_sha256_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hashlib_sha256_digest_obj) },
};

static MP_DEFINE_CONST_DICT(hashlib_sha256_locals_dict, hashlib_sha256_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    hashlib_sha256_type,
    MP_QSTR_sha256,
    MP_TYPE_FLAG_NONE,
    make_new, hashlib_sha256_make_new,
    locals_dict, &hashlib_sha256_locals_dict
    );
#endif

#if MICROPY_PY_HASHLIB_SHA1
static mp_obj_t hashlib_sha1_update(mp_obj_t self_in, mp_obj_t arg);

#if MICROPY_SSL_AXTLS
static mp_obj_t hashlib_sha1_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(SHA1_CTX), type);
    o->final = false;
    SHA1_Init((SHA1_CTX *)o->state);
    if (n_args == 1) {
        hashlib_sha1_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_sha1_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    SHA1_Update((SHA1_CTX *)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_sha1_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, SHA1_SIZE);
    SHA1_Final((byte *)vstr.buf, (SHA1_CTX *)self->state);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
#endif

#if MICROPY_SSL_MBEDTLS

#if MBEDTLS_VERSION_NUMBER < 0x02070000 || MBEDTLS_VERSION_NUMBER >= 0x03000000
#define mbedtls_sha1_starts_ret mbedtls_sha1_starts
#define mbedtls_sha1_update_ret mbedtls_sha1_update
#define mbedtls_sha1_finish_ret mbedtls_sha1_finish
#endif

static mp_obj_t hashlib_sha1_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(mbedtls_sha1_context), type);
    o->final = false;
    mbedtls_sha1_init((mbedtls_sha1_context *)o->state);
    mbedtls_sha1_starts_ret((mbedtls_sha1_context *)o->state);
    if (n_args == 1) {
        hashlib_sha1_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_sha1_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    mbedtls_sha1_update_ret((mbedtls_sha1_context *)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_sha1_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, 20);
    mbedtls_sha1_finish_ret((mbedtls_sha1_context *)self->state, (byte *)vstr.buf);
    mbedtls_sha1_free((mbedtls_sha1_context *)self->state);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
#endif

static MP_DEFINE_CONST_FUN_OBJ_2(hashlib_sha1_update_obj, hashlib_sha1_update);
static MP_DEFINE_CONST_FUN_OBJ_1(hashlib_sha1_digest_obj, hashlib_sha1_digest);

static const mp_rom_map_elem_t hashlib_sha1_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hashlib_sha1_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hashlib_sha1_digest_obj) },
};
static MP_DEFINE_CONST_DICT(hashlib_sha1_locals_dict, hashlib_sha1_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    hashlib_sha1_type,
    MP_QSTR_sha1,
    MP_TYPE_FLAG_NONE,
    make_new, hashlib_sha1_make_new,
    locals_dict, &hashlib_sha1_locals_dict
    );
#endif

#if MICROPY_PY_HASHLIB_MD5
static mp_obj_t hashlib_md5_update(mp_obj_t self_in, mp_obj_t arg);

#if MICROPY_SSL_AXTLS
static mp_obj_t hashlib_md5_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(MD5_CTX), type);
    o->final = false;
    MD5_Init((MD5_CTX *)o->state);
    if (n_args == 1) {
        hashlib_md5_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_md5_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    MD5_Update((MD5_CTX *)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_md5_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, MD5_SIZE);
    MD5_Final((byte *)vstr.buf, (MD5_CTX *)self->state);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
#endif // MICROPY_SSL_AXTLS

#if MICROPY_SSL_MBEDTLS

#if MBEDTLS_VERSION_NUMBER < 0x02070000 || MBEDTLS_VERSION_NUMBER >= 0x03000000
#define mbedtls_md5_starts_ret mbedtls_md5_starts
#define mbedtls_md5_update_ret mbedtls_md5_update
#define mbedtls_md5_finish_ret mbedtls_md5_finish
#endif

static mp_obj_t hashlib_md5_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = mp_obj_malloc_var(mp_obj_hash_t, state, char, sizeof(mbedtls_md5_context), type);
    o->final = false;
    mbedtls_md5_init((mbedtls_md5_context *)o->state);
    mbedtls_md5_starts_ret((mbedtls_md5_context *)o->state);
    if (n_args == 1) {
        hashlib_md5_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t hashlib_md5_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    mbedtls_md5_update_ret((mbedtls_md5_context *)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}

static mp_obj_t hashlib_md5_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    hashlib_ensure_not_final(self);
    self->final = true;
    vstr_t vstr;
    vstr_init_len(&vstr, 16);
    mbedtls_md5_finish_ret((mbedtls_md5_context *)self->state, (byte *)vstr.buf);
    mbedtls_md5_free((mbedtls_md5_context *)self->state);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
#endif // MICROPY_SSL_MBEDTLS

static MP_DEFINE_CONST_FUN_OBJ_2(hashlib_md5_update_obj, hashlib_md5_update);
static MP_DEFINE_CONST_FUN_OBJ_1(hashlib_md5_digest_obj, hashlib_md5_digest);

static const mp_rom_map_elem_t hashlib_md5_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hashlib_md5_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hashlib_md5_digest_obj) },
};
static MP_DEFINE_CONST_DICT(hashlib_md5_locals_dict, hashlib_md5_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    hashlib_md5_type,
    MP_QSTR_md5,
    MP_TYPE_FLAG_NONE,
    make_new, hashlib_md5_make_new,
    locals_dict, &hashlib_md5_locals_dict
    );
#endif // MICROPY_PY_HASHLIB_MD5

static const mp_rom_map_elem_t mp_module_hashlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hashlib) },
    #if MICROPY_PY_HASHLIB_SHA256
    { MP_ROM_QSTR(MP_QSTR_sha256), MP_ROM_PTR(&hashlib_sha256_type) },
    #endif
    #if MICROPY_PY_HASHLIB_SHA1
    { MP_ROM_QSTR(MP_QSTR_sha1), MP_ROM_PTR(&hashlib_sha1_type) },
    #endif
    #if MICROPY_PY_HASHLIB_MD5
    { MP_ROM_QSTR(MP_QSTR_md5), MP_ROM_PTR(&hashlib_md5_type) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_hashlib_globals, mp_module_hashlib_globals_table);

const mp_obj_module_t mp_module_hashlib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_hashlib_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_hashlib, mp_module_hashlib);

#endif // MICROPY_PY_HASHLIB
