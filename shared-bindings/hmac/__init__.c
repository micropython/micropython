// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/hmac/__init__.h"
#include "shared-bindings/hmac/HMAC.h"
#include "shared-module/hmac/__init__.h"

#if CIRCUITPY_HARDWAREKEY
#include "shared-bindings/hardwarekey/HardwareKey.h"
#endif

//| """Keyed hashing for message authentication
//|
//| |see_cpython_module| :mod:`cpython:hmac`.
//|
//| The key may be a ``bytes``-like object or, where the port provides them, a
//| ``hardwarekey.HardwareKey`` -- so the same code works with a key in flash
//| during development and a key held in hardware in production. The one
//| exception: an empty message raises `ValueError` with a
//| ``hardwarekey.HardwareKey`` (a hardware driver limitation), where it is
//| valid with a ``bytes`` key.
//|
//| Only ``"sha256"`` and ``"sha1"`` are supported for ``digestmod``.
//| """
//|

static psa_algorithm_t hash_alg_from_digestmod(mp_obj_t digestmod) {
    const char *name = mp_obj_str_get_str(digestmod);
    psa_algorithm_t hash_alg;
    if (!hmac_hash_alg_from_name(name, &hash_alg)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported hash algorithm"));
    }
    return hash_alg;
}

static hmac_hmac_obj_t *hmac_new_internal(mp_obj_t key_in, psa_algorithm_t hash_alg) {
    hmac_hmac_obj_t *self = mp_obj_malloc(hmac_hmac_obj_t, &hmac_hmac_type);

    #if CIRCUITPY_HARDWAREKEY
    if (mp_obj_is_type(key_in, &hardwarekey_hardwarekey_type)) {
        hardwarekey_hardwarekey_obj_t *key = MP_OBJ_TO_PTR(key_in);
        if (common_hal_hardwarekey_hardwarekey_get_purpose(key) != HARDWAREKEY_PURPOSE_HMAC) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("key does not have the expected %q purpose"), MP_QSTR_HMAC_UP);
        }
        psa_key_id_t key_id = common_hal_hardwarekey_hardwarekey_get_key_id(key);
        common_hal_hmac_new(self, NULL, 0, key_id, hash_alg);
        return self;
    }
    #endif

    mp_buffer_info_t keyinfo;
    mp_get_buffer_raise(key_in, &keyinfo, MP_BUFFER_READ);

    common_hal_hmac_new(self, keyinfo.buf, keyinfo.len, 0, hash_alg);
    return self;
}

//| def new(
//|     key: ReadableBuffer | hardwarekey.HardwareKey,
//|     msg: ReadableBuffer = b"",
//|     digestmod: str = ...,
//| ) -> HMAC:
//|     """Create a new HMAC object.
//|
//|     :param key: the secret key -- a ``bytes``-like object, or a
//|       ``hardwarekey.HardwareKey`` on ports that provide it
//|     :param ReadableBuffer msg: initial data to authenticate; add more with `HMAC.update()`
//|     :param str digestmod: the digest name, ``"sha256"`` or ``"sha1"``. Required.
//|     """
//|     ...
//|
static mp_obj_t hmac_new(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_key, ARG_msg, ARG_digestmod };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_msg, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_digestmod, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    psa_algorithm_t hash_alg = hash_alg_from_digestmod(args[ARG_digestmod].u_obj);
    hmac_hmac_obj_t *self = hmac_new_internal(args[ARG_key].u_obj, hash_alg);

    if (args[ARG_msg].u_obj != mp_const_none) {
        hmac_hmac_update(MP_OBJ_FROM_PTR(self), args[ARG_msg].u_obj);
    }
    return MP_OBJ_FROM_PTR(self);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(hmac_new_obj, 1, hmac_new);

//| def digest(
//|     key: ReadableBuffer | hardwarekey.HardwareKey, msg: ReadableBuffer, digest: str
//| ) -> bytes:
//|     """Return the HMAC of ``msg`` under ``key`` for the named ``digest``, in one call.
//|
//|     Equivalent to ``new(key, msg, digestmod=digest).digest()`` but does not build an
//|     intermediate object."""
//|     ...
//|
static mp_obj_t hmac_digest(mp_obj_t key_in, mp_obj_t msg_in, mp_obj_t digest_in) {
    psa_algorithm_t hash_alg = hash_alg_from_digestmod(digest_in);
    hmac_hmac_obj_t *self = hmac_new_internal(key_in, hash_alg);
    hmac_hmac_update(MP_OBJ_FROM_PTR(self), msg_in);

    size_t size = common_hal_hmac_get_digest_size(self);
    mp_obj_t obj = mp_obj_new_bytes_of_zeros(size);
    mp_obj_str_t *o = MP_OBJ_TO_PTR(obj);
    common_hal_hmac_digest(self, (uint8_t *)o->data, size);
    return obj;
}
static MP_DEFINE_CONST_FUN_OBJ_3(hmac_digest_obj, hmac_digest);

//| def compare_digest(a: ReadableBuffer, b: ReadableBuffer) -> bool:
//|     """Return ``a == b`` using a constant-time comparison, to avoid leaking timing
//|     information about a MAC check."""
//|     ...
//|
static mp_obj_t hmac_compare_digest(mp_obj_t a_in, mp_obj_t b_in) {
    mp_buffer_info_t a, b;
    mp_get_buffer_raise(a_in, &a, MP_BUFFER_READ);
    mp_get_buffer_raise(b_in, &b, MP_BUFFER_READ);
    return mp_obj_new_bool(common_hal_hmac_compare_digest(a.buf, a.len, b.buf, b.len));
}
static MP_DEFINE_CONST_FUN_OBJ_2(hmac_compare_digest_obj, hmac_compare_digest);

static const mp_rom_map_elem_t hmac_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hmac) },
    { MP_ROM_QSTR(MP_QSTR_new), MP_ROM_PTR(&hmac_new_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hmac_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR_compare_digest), MP_ROM_PTR(&hmac_compare_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR_HMAC), MP_ROM_PTR(&hmac_hmac_type) },
};
static MP_DEFINE_CONST_DICT(hmac_module_globals, hmac_module_globals_table);

const mp_obj_module_t hmac_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&hmac_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_hmac, hmac_module);
