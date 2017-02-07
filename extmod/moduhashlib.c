/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_PY_UHASHLIB

#include "crypto-algorithms/sha256.h"
#if MICROPY_PY_UHASHLIB_SHA1
#include "lib/axtls/crypto/crypto.h"
#endif

typedef struct _mp_obj_hash_t {
    mp_obj_base_t base;
    char state[0];
} mp_obj_hash_t;

STATIC mp_obj_t hash_update(mp_obj_t self_in, mp_obj_t arg);

STATIC mp_obj_t hash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = m_new_obj_var(mp_obj_hash_t, char, sizeof(CRYAL_SHA256_CTX));
    o->base.type = type;
    sha256_init((CRYAL_SHA256_CTX*)o->state);
    if (n_args == 1) {
        hash_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

#if MICROPY_PY_UHASHLIB_SHA1
STATIC mp_obj_t sha1_update(mp_obj_t self_in, mp_obj_t arg);

STATIC mp_obj_t sha1_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = m_new_obj_var(mp_obj_hash_t, char, sizeof(SHA1_CTX));
    o->base.type = type;
    SHA1_Init((SHA1_CTX*)o->state);
    if (n_args == 1) {
        sha1_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}
#endif

STATIC mp_obj_t hash_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    sha256_update((CRYAL_SHA256_CTX*)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(hash_update_obj, hash_update);

#if MICROPY_PY_UHASHLIB_SHA1
STATIC mp_obj_t sha1_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    SHA1_Update((SHA1_CTX*)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(sha1_update_obj, sha1_update);
#endif

STATIC mp_obj_t hash_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    vstr_init_len(&vstr, SHA256_BLOCK_SIZE);
    sha256_final((CRYAL_SHA256_CTX*)self->state, (byte*)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(hash_digest_obj, hash_digest);

#if MICROPY_PY_UHASHLIB_SHA1
STATIC mp_obj_t sha1_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    vstr_init_len(&vstr, SHA1_SIZE);
    SHA1_Final((byte*)vstr.buf, (SHA1_CTX*)self->state);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(sha1_digest_obj, sha1_digest);
#endif

STATIC const mp_rom_map_elem_t hash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hash_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hash_digest_obj) },
};

STATIC MP_DEFINE_CONST_DICT(hash_locals_dict, hash_locals_dict_table);

STATIC const mp_obj_type_t sha256_type = {
    { &mp_type_type },
    .name = MP_QSTR_sha256,
    .make_new = hash_make_new,
    .locals_dict = (void*)&hash_locals_dict,
};

#if MICROPY_PY_UHASHLIB_SHA1
STATIC const mp_rom_map_elem_t sha1_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&sha1_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&sha1_digest_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sha1_locals_dict, sha1_locals_dict_table);

STATIC const mp_obj_type_t sha1_type = {
    { &mp_type_type },
    .name = MP_QSTR_sha1,
    .make_new = sha1_make_new,
    .locals_dict = (void*)&sha1_locals_dict,
};
#endif

STATIC const mp_rom_map_elem_t mp_module_hashlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uhashlib) },
    { MP_ROM_QSTR(MP_QSTR_sha256), MP_ROM_PTR(&sha256_type) },
    #if MICROPY_PY_UHASHLIB_SHA1
    { MP_ROM_QSTR(MP_QSTR_sha1), MP_ROM_PTR(&sha1_type) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_hashlib_globals, mp_module_hashlib_globals_table);

const mp_obj_module_t mp_module_uhashlib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_hashlib_globals,
};

#include "crypto-algorithms/sha256.c"

#endif //MICROPY_PY_UHASHLIB
