/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "bindings/espnow/Peer.h"
#include "common-hal/espnow/__init__.h"

//| class Peer:
//|     """A data class to store parameters specific to a peer."""
//|
//|     def __init__(
//|         self,
//|         mac: bytes,
//|         *,
//|         lmk: Optional[bytes],
//|         channel: int = 0,
//|         interface: int = 0,
//|         encrypted: bool = False,
//|     ) -> None:
//|         """Construct a new peer object.
//|
//|         :param bytes mac: The mac address of the peer.
//|         :param bytes lmk: The Local Master Key (lmk) of the peer.
//|         :param int channel: The peer's channel. Default: 0 ie. use the current channel.
//|         :param int interface: The WiFi interface to use. Default: 0 ie. STA.
//|         :param bool encrypted: Whether or not to use encryption.
//|         """
//|         ...
STATIC mp_obj_t espnow_peer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_mac, ARG_lmk, ARG_channel, ARG_interface, ARG_encrypted };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mac,       MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_lmk,       MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
        { MP_QSTR_channel,   MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0 } },
        { MP_QSTR_interface, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0 } },
        { MP_QSTR_encrypted, MP_ARG_BOOL | MP_ARG_KW_ONLY, { .u_bool = false } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_peer_obj_t *self = mp_obj_malloc(espnow_peer_obj_t, &espnow_peer_type);
    self->peer_info = (esp_now_peer_info_t) {
        .channel = 0,
        .ifidx = WIFI_IF_STA,
        .encrypt = false
    };

    memcpy(self->peer_info.peer_addr, common_hal_espnow_get_bytes_len(args[ARG_mac].u_obj, ESP_NOW_ETH_ALEN), ESP_NOW_ETH_ALEN);

    self->peer_info.channel = mp_arg_validate_int_range(args[ARG_channel].u_int, 0, 14, MP_QSTR_channel);

    self->peer_info.ifidx = (wifi_interface_t)mp_arg_validate_int_range(args[ARG_interface].u_int, 0, 1, MP_QSTR_interface);

    self->peer_info.encrypt = args[ARG_encrypted].u_bool;
    self->lmk_set = false;

    const mp_obj_t lmk = args[ARG_lmk].u_obj;
    if (lmk != mp_const_none) {
        self->lmk_set = true;
        memcpy(self->peer_info.lmk, common_hal_espnow_get_bytes_len(lmk, ESP_NOW_KEY_LEN), ESP_NOW_KEY_LEN);
    } else if (self->peer_info.encrypt) {
        mp_raise_ValueError_varg(translate("%q is %q"), MP_QSTR_lmk, MP_QSTR_None);
    }

    return self;
}

//|     mac: ReadableBuffer
//|     """The WiFi mac to use."""
//|
STATIC mp_obj_t espnow_peer_get_mac(const mp_obj_t self_in) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bytes(self->peer_info.peer_addr, MP_ARRAY_SIZE(self->peer_info.peer_addr));
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_get_mac_obj, espnow_peer_get_mac);

STATIC mp_obj_t espnow_peer_set_mac(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    memcpy(self->peer_info.peer_addr, common_hal_espnow_get_bytes_len(value, ESP_NOW_ETH_ALEN), ESP_NOW_ETH_ALEN);
    esp_now_mod_peer(&self->peer_info);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_peer_set_mac_obj, espnow_peer_set_mac);

MP_PROPERTY_GETSET(espnow_peer_mac_obj,
    (mp_obj_t)&espnow_peer_get_mac_obj,
    (mp_obj_t)&espnow_peer_set_mac_obj);

//|     lmk: ReadableBuffer
//|     """The WiFi lmk to use."""
//|
STATIC mp_obj_t espnow_peer_get_lmk(const mp_obj_t self_in) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bytes(self->peer_info.lmk, MP_ARRAY_SIZE(self->peer_info.lmk));
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_get_lmk_obj, espnow_peer_get_lmk);

STATIC mp_obj_t espnow_peer_set_lmk(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    memcpy(self->peer_info.lmk, common_hal_espnow_get_bytes_len(value, ESP_NOW_KEY_LEN), ESP_NOW_KEY_LEN);
    esp_now_mod_peer(&self->peer_info);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_peer_set_lmk_obj, espnow_peer_set_lmk);

MP_PROPERTY_GETSET(espnow_peer_lmk_obj,
    (mp_obj_t)&espnow_peer_get_lmk_obj,
    (mp_obj_t)&espnow_peer_set_lmk_obj);

//|     channel: int
//|     """The WiFi channel to use."""
//|
STATIC mp_obj_t espnow_peer_get_channel(const mp_obj_t self_in) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->peer_info.channel);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_get_channel_obj, espnow_peer_get_channel);

STATIC mp_obj_t espnow_peer_set_channel(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->peer_info.channel = mp_arg_validate_int_range(mp_obj_get_int(value), 0, 14, MP_QSTR_channel);
    esp_now_mod_peer(&self->peer_info);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_peer_set_channel_obj, espnow_peer_set_channel);

MP_PROPERTY_GETSET(espnow_peer_channel_obj,
    (mp_obj_t)&espnow_peer_get_channel_obj,
    (mp_obj_t)&espnow_peer_set_channel_obj);

//|     interface: int
//|     """The WiFi interface to use."""
//|
STATIC mp_obj_t espnow_peer_get_interface(const mp_obj_t self_in) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->peer_info.ifidx);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_get_interface_obj, espnow_peer_get_interface);

STATIC mp_obj_t espnow_peer_set_interface(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->peer_info.ifidx = (wifi_interface_t)mp_arg_validate_int_range(mp_obj_get_int(value), 0, 1, MP_QSTR_interface);
    esp_now_mod_peer(&self->peer_info);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_peer_set_interface_obj, espnow_peer_set_interface);

MP_PROPERTY_GETSET(espnow_peer_interface_obj,
    (mp_obj_t)&espnow_peer_get_interface_obj,
    (mp_obj_t)&espnow_peer_set_interface_obj);

//|     encrypted: bool
//|     """Whether or not to use encryption."""
//|
STATIC mp_obj_t espnow_peer_get_encrypted(const mp_obj_t self_in) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->peer_info.encrypt);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_peer_get_encrypted_obj, espnow_peer_get_encrypted);

STATIC mp_obj_t espnow_peer_set_encrypted(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_peer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->peer_info.encrypt = mp_obj_is_true(value);

    if (!self->lmk_set) {
        mp_raise_ValueError_varg(translate("%q is %q"), MP_QSTR_lmk, MP_QSTR_None);
    }

    esp_now_mod_peer(&self->peer_info);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_peer_set_encrypted_obj, espnow_peer_set_encrypted);

MP_PROPERTY_GETSET(espnow_peer_encrypted_obj,
    (mp_obj_t)&espnow_peer_get_encrypted_obj,
    (mp_obj_t)&espnow_peer_set_encrypted_obj);

STATIC const mp_rom_map_elem_t espnow_peer_locals_dict_table[] = {
    // Peer parameters
    { MP_ROM_QSTR(MP_QSTR_mac),         MP_ROM_PTR(&espnow_peer_mac_obj) },
    { MP_ROM_QSTR(MP_QSTR_lmk),         MP_ROM_PTR(&espnow_peer_lmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel),     MP_ROM_PTR(&espnow_peer_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_interface),   MP_ROM_PTR(&espnow_peer_interface_obj) },
    { MP_ROM_QSTR(MP_QSTR_encrypted),   MP_ROM_PTR(&espnow_peer_encrypted_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_peer_locals_dict, espnow_peer_locals_dict_table);

const mp_obj_type_t espnow_peer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Peer,
    .make_new = espnow_peer_make_new,
    .locals_dict = (mp_obj_t)&espnow_peer_locals_dict,
};
