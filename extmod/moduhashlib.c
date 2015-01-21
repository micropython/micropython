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

typedef struct _mp_obj_hash_t {
    mp_obj_base_t base;
    char state[0];
} mp_obj_hash_t;

STATIC mp_obj_t hash_update(mp_obj_t self_in, mp_obj_t arg);

STATIC mp_obj_t hash_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_hash_t *o = m_new_obj_var(mp_obj_hash_t, char, sizeof(SHA256_CTX));
    o->base.type = type_in;
    sha256_init((SHA256_CTX*)o->state);
    if (n_args == 1) {
        hash_update(o, args[0]);
    }
    return o;
}

STATIC mp_obj_t hash_update(mp_obj_t self_in, mp_obj_t arg) {
    mp_obj_hash_t *self = self_in;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    sha256_update((SHA256_CTX*)self->state, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(hash_update_obj, hash_update);

STATIC mp_obj_t hash_digest(mp_obj_t self_in) {
    mp_obj_hash_t *self = self_in;
    vstr_t vstr;
    vstr_init_len(&vstr, SHA256_BLOCK_SIZE);
    sha256_final((SHA256_CTX*)self->state, (byte*)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_1(hash_digest_obj, hash_digest);

STATIC mp_obj_t hash_hexdigest(mp_obj_t self_in) {
    (void)self_in;
    mp_not_implemented("");
#if 0
    mp_obj_hash_t *self = self_in;
    byte hash[SHA256_BLOCK_SIZE];
    sha256_final((SHA256_CTX*)self->state, hash);
    return mp_obj_new_str((char*)hash, SHA256_BLOCK_SIZE, false);
#endif
}
MP_DEFINE_CONST_FUN_OBJ_1(hash_hexdigest_obj, hash_hexdigest);

STATIC const mp_map_elem_t hash_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_update), (mp_obj_t) &hash_update_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_digest), (mp_obj_t) &hash_digest_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hexdigest), (mp_obj_t) &hash_hexdigest_obj },
};

STATIC MP_DEFINE_CONST_DICT(hash_locals_dict, hash_locals_dict_table);

STATIC const mp_obj_type_t sha256_type = {
    { &mp_type_type },
    .name = MP_QSTR_sha256,
    .make_new = hash_make_new,
    .locals_dict = (mp_obj_t)&hash_locals_dict,
};

STATIC const mp_map_elem_t mp_module_hashlib_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_uhashlib) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sha256), (mp_obj_t)&sha256_type },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_hashlib_globals, mp_module_hashlib_globals_table);

const mp_obj_module_t mp_module_uhashlib = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uhashlib,
    .globals = (mp_obj_dict_t*)&mp_module_hashlib_globals,
};

#include "crypto-algorithms/sha256.c"

#endif //MICROPY_PY_UHASHLIB
