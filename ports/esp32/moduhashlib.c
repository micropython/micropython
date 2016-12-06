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
#include "py/runtime.h"

#include "mbedtls/sha256.h"
#include "mbedtls/sha1.h"

union sha_ctxs {
        mbedtls_sha256_context sha256;
        mbedtls_sha1_context sha1;
};
typedef struct _mp_obj_hash_t {
    mp_obj_base_t base;
    union sha_ctxs state;
} mp_obj_hash_t;

STATIC mp_obj_t sha256_update(mp_obj_t self_in, mp_obj_t arg);
STATIC mp_obj_t sha1_update(mp_obj_t self_in, mp_obj_t arg);

STATIC mp_obj_t sha256_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = m_new_obj_var(mp_obj_hash_t, char, sizeof(union sha_ctxs));
    o->base.type = type;
    mbedtls_sha256_init(&o->state.sha256);
    if (n_args == 1) {
        sha256_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t sha1_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = m_new_obj_var(mp_obj_hash_t, char, sizeof(union sha_ctxs));
    o->base.type = type;
    mbedtls_sha1_init(&o->state.sha1);
    if (n_args == 1) {
        sha1_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t sha256_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    mbedtls_sha256_update(&self->state.sha256, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(sha256_update_obj, sha256_update);

STATIC mp_obj_t sha1_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    mbedtls_sha1_update(&self->state.sha1, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(sha1_update_obj, sha1_update);

STATIC mp_obj_t sha256_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    vstr_init_len(&vstr, 32);
    mbedtls_sha256_finish(&self->state.sha256, (unsigned char *)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(sha256_digest_obj, sha256_digest);

STATIC mp_obj_t sha1_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    vstr_init_len(&vstr, 20);
    mbedtls_sha1_finish(&self->state.sha1, (unsigned char *)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(sha1_digest_obj, sha1_digest);

STATIC const mp_rom_map_elem_t sha256_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&sha256_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&sha256_digest_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sha256_locals_dict, sha256_locals_dict_table);

STATIC const mp_obj_type_t sha256_type = {
    { &mp_type_type },
    .name = MP_QSTR_sha256,
    .make_new = sha256_make_new,
    .locals_dict = (void*)&sha256_locals_dict,
};

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

STATIC const mp_rom_map_elem_t mp_module_hashlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uhashlib) },
    { MP_ROM_QSTR(MP_QSTR_sha256), MP_ROM_PTR(&sha256_type) },
    { MP_ROM_QSTR(MP_QSTR_sha1), MP_ROM_PTR(&sha1_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_hashlib_globals,
    mp_module_hashlib_globals_table);

const mp_obj_module_t mp_module_uhashlib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_hashlib_globals,
};
