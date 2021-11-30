#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/aesio/__init__.h"

// Defined at the end of this file

//| MODE_ECB: int
//| MODE_CBC: int
//| MODE_CTR: int
//|
//| class AES:
//|     """Encrypt and decrypt AES streams"""
//|
//|     def __init__(self, key: ReadableBuffer, mode: int = 0, iv: Optional[ReadableBuffer] = None, segment_size: int = 8) -> None:
//|         """Create a new AES state with the given key.
//|
//|            :param ~_typing.ReadableBuffer key: A 16-, 24-, or 32-byte key
//|            :param int mode: AES mode to use.  One of: `MODE_ECB`, `MODE_CBC`, or
//|                             `MODE_CTR`
//|            :param ~_typing.ReadableBuffer iv: Initialization vector to use for CBC or CTR mode
//|
//|            Additional arguments are supported for legacy reasons.
//|
//|            Encrypting a string::
//|
//|              import aesio
//|              from binascii import hexlify
//|
//|              key = b'Sixteen byte key'
//|              inp = b'CircuitPython!!!' # Note: 16-bytes long
//|              outp = bytearray(len(inp))
//|              cipher = aesio.AES(key, aesio.MODE_ECB)
//|              cipher.encrypt_into(inp, outp)
//|              hexlify(outp)"""
//|         ...
//|

STATIC mp_obj_t aesio_aes_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    (void)type;
    enum { ARG_key, ARG_mode, ARG_IV, ARG_counter, ARG_segment_size };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_key, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_mode, MP_ARG_INT, {.u_int = AES_MODE_ECB} },
        {MP_QSTR_IV, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_counter, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        {MP_QSTR_segment_size, MP_ARG_INT, {.u_int = 8}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args),
        allowed_args, args);

    aesio_aes_obj_t *self = m_new_obj(aesio_aes_obj_t);
    self->base.type = &aesio_aes_type;

    mp_buffer_info_t bufinfo;

    const uint8_t *key = NULL;
    uint32_t key_length = 0;
    if (mp_get_buffer(args[ARG_key].u_obj, &bufinfo, MP_BUFFER_READ)) {
        if ((bufinfo.len != 16) && (bufinfo.len != 24) && (bufinfo.len != 32)) {
            mp_raise_TypeError(translate("Key must be 16, 24, or 32 bytes long"));
        }
        key = bufinfo.buf;
        key_length = bufinfo.len;
    } else {
        mp_raise_TypeError(translate("No key was specified"));
    }

    int mode = args[ARG_mode].u_int;
    switch (args[ARG_mode].u_int) {
        case AES_MODE_CBC:
        case AES_MODE_ECB:
        case AES_MODE_CTR:
            break;
        default:
            mp_raise_TypeError(translate("Requested AES mode is unsupported"));
    }

    // IV is required for CBC mode and is ignored for other modes.
    const uint8_t *iv = NULL;
    if (args[ARG_IV].u_obj != NULL &&
        mp_get_buffer(args[ARG_IV].u_obj, &bufinfo, MP_BUFFER_READ)) {
        if (bufinfo.len != AES_BLOCKLEN) {
            mp_raise_TypeError_varg(translate("IV must be %d bytes long"),
                AES_BLOCKLEN);
        }
        iv = bufinfo.buf;
    }

    common_hal_aesio_aes_construct(self, key, key_length, iv, mode,
        args[ARG_counter].u_int);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t aesio_aes_rekey(size_t n_args, const mp_obj_t *pos_args) {
    aesio_aes_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(pos_args[1], &bufinfo, MP_BUFFER_READ);
    const uint8_t *key = bufinfo.buf;
    size_t key_length = bufinfo.len;
    if (key == NULL) {
        mp_raise_ValueError(translate("No key was specified"));
    }
    if ((key_length != 16) && (key_length != 24) && (key_length != 32)) {
        mp_raise_TypeError(translate("Key must be 16, 24, or 32 bytes long"));
    }

    const uint8_t *iv = NULL;
    if (n_args > 2) {
        mp_get_buffer_raise(pos_args[2], &bufinfo, MP_BUFFER_READ);
        size_t iv_length = bufinfo.len;
        iv = (const uint8_t *)bufinfo.buf;
        if (iv_length != AES_BLOCKLEN) {
            mp_raise_TypeError_varg(translate("IV must be %d bytes long"),
                AES_BLOCKLEN);
        }
    }

    common_hal_aesio_aes_rekey(self, key, key_length, iv);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR(aesio_aes_rekey_obj, 2, aesio_aes_rekey);

STATIC void validate_length(aesio_aes_obj_t *self, size_t src_length,
    size_t dest_length) {
    if (src_length != dest_length) {
        mp_raise_ValueError(
            translate("Source and destination buffers must be the same length"));
    }

    switch (self->mode) {
        case AES_MODE_ECB:
            if (src_length != 16) {
                mp_raise_msg(&mp_type_ValueError,
                    translate("ECB only operates on 16 bytes at a time"));
            }
            break;
        case AES_MODE_CBC:
            if ((src_length & 15) != 0) {
                mp_raise_msg(&mp_type_ValueError,
                    translate("CBC blocks must be multiples of 16 bytes"));
            }
            break;
        case AES_MODE_CTR:
            break;
    }
}

//|     def encrypt_into(self, src: ReadableBuffer, dest: WriteableBuffer) -> None:
//|         """Encrypt the buffer from ``src`` into ``dest``.
//|
//|            For ECB mode, the buffers must be 16 bytes long.  For CBC mode, the
//|            buffers must be a multiple of 16 bytes, and must be equal length.  For
//|            CTX mode, there are no restrictions."""
//|         ...
//|
STATIC mp_obj_t aesio_aes_encrypt_into(mp_obj_t aesio_obj, mp_obj_t src,
    mp_obj_t dest) {
    if (!mp_obj_is_type(aesio_obj, &aesio_aes_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), aesio_aes_type.name);
    }
    // Convert parameters into expected types.
    aesio_aes_obj_t *aes = MP_OBJ_TO_PTR(aesio_obj);

    mp_buffer_info_t srcbufinfo, destbufinfo;
    mp_get_buffer_raise(src, &srcbufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(dest, &destbufinfo, MP_BUFFER_WRITE);
    validate_length(aes, srcbufinfo.len, destbufinfo.len);

    memcpy(destbufinfo.buf, srcbufinfo.buf, srcbufinfo.len);

    common_hal_aesio_aes_encrypt(aes, (uint8_t *)destbufinfo.buf,
        destbufinfo.len);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(aesio_aes_encrypt_into_obj,
    aesio_aes_encrypt_into);

//|     def decrypt_into(self, src: ReadableBuffer, dest: WriteableBuffer) -> None:
//|         """Decrypt the buffer from ``src`` into ``dest``.
//|            For ECB mode, the buffers must be 16 bytes long.  For CBC mode, the
//|            buffers must be a multiple of 16 bytes, and must be equal length.  For
//|            CTX mode, there are no restrictions."""
//|         ...
//|
STATIC mp_obj_t aesio_aes_decrypt_into(mp_obj_t aesio_obj, mp_obj_t src,
    mp_obj_t dest) {
    if (!mp_obj_is_type(aesio_obj, &aesio_aes_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), aesio_aes_type.name);
    }
    // Convert parameters into expected types.
    aesio_aes_obj_t *aes = MP_OBJ_TO_PTR(aesio_obj);

    mp_buffer_info_t srcbufinfo, destbufinfo;
    mp_get_buffer_raise(src, &srcbufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(dest, &destbufinfo, MP_BUFFER_WRITE);
    validate_length(aes, srcbufinfo.len, destbufinfo.len);

    memcpy(destbufinfo.buf, srcbufinfo.buf, srcbufinfo.len);

    common_hal_aesio_aes_decrypt(aes, (uint8_t *)destbufinfo.buf,
        destbufinfo.len);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(aesio_aes_decrypt_into_obj,
    aesio_aes_decrypt_into);

STATIC mp_obj_t aesio_aes_get_mode(mp_obj_t aesio_obj) {
    if (!mp_obj_is_type(aesio_obj, &aesio_aes_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), aesio_aes_type.name);
    }
    aesio_aes_obj_t *self = MP_OBJ_TO_PTR(aesio_obj);
    return MP_OBJ_NEW_SMALL_INT(self->mode);
}
MP_DEFINE_CONST_FUN_OBJ_1(aesio_aes_get_mode_obj, aesio_aes_get_mode);

STATIC mp_obj_t aesio_aes_set_mode(mp_obj_t aesio_obj, mp_obj_t mode_obj) {
    if (!mp_obj_is_type(aesio_obj, &aesio_aes_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), aesio_aes_type.name);
    }
    aesio_aes_obj_t *self = MP_OBJ_TO_PTR(aesio_obj);

    int mode = mp_obj_get_int(mode_obj);
    switch (mode) {
        case AES_MODE_CBC:
        case AES_MODE_ECB:
        case AES_MODE_CTR:
            break;
        default:
            mp_raise_TypeError(translate("Requested AES mode is unsupported"));
    }

    common_hal_aesio_aes_set_mode(self, mode);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(aesio_aes_set_mode_obj, aesio_aes_set_mode);

const mp_obj_property_t aesio_aes_mode_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&aesio_aes_get_mode_obj,
              (mp_obj_t)&aesio_aes_set_mode_obj, MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t aesio_locals_dict_table[] = {
    // Methods
    {MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_AES)},
    {MP_ROM_QSTR(MP_QSTR_encrypt_into), (mp_obj_t)&aesio_aes_encrypt_into_obj},
    {MP_ROM_QSTR(MP_QSTR_decrypt_into), (mp_obj_t)&aesio_aes_decrypt_into_obj},
    {MP_ROM_QSTR(MP_QSTR_rekey), (mp_obj_t)&aesio_aes_rekey_obj},
    {MP_ROM_QSTR(MP_QSTR_mode), (mp_obj_t)&aesio_aes_mode_obj},
};
STATIC MP_DEFINE_CONST_DICT(aesio_locals_dict, aesio_locals_dict_table);

const mp_obj_type_t aesio_aes_type = {
    {&mp_type_type},
    .name = MP_QSTR_AES,
    .make_new = aesio_aes_make_new,
    .locals_dict = (mp_obj_dict_t *)&aesio_locals_dict,
};
