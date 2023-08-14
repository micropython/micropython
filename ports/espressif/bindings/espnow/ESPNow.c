/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2020 Nick Moore
 * Copyright (c) 2018 shawwwn <shawwwn1@gmail.com>
 * Copyright (c) 2020-2021 Glenn Moloney @glenn20
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

#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/util.h"

#include "bindings/espnow/ESPNow.h"
#include "bindings/espnow/Peer.h"

#include "common-hal/espnow/__init__.h"
#include "common-hal/espnow/ESPNow.h"

#include "esp_now.h"

// Raise ValueError if the ESPNow object is deinited
static void espnow_check_for_deinit(espnow_obj_t *self) {
    if (common_hal_espnow_deinited(self)) {
        raise_deinited_error();
    }
}

// --- Initialisation and Config functions ---

//| class ESPNow:
//|     """Provides access to the ESP-NOW protocol."""
//|
//|     def __init__(self, buffer_size: int = 526, phy_rate: int = 0) -> None:
//|         """Allocate and initialize `ESPNow` instance as a singleton.
//|
//|         :param int buffer_size: The size of the internal ring buffer. Default: 526 bytes.
//|         :param int phy_rate: The ESP-NOW physical layer rate. Default: 1 Mbps.
//|             `wifi_phy_rate_t <https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/network/esp_wifi.html#_CPPv415wifi_phy_rate_t>`_
//|         """
//|         ...
STATIC mp_obj_t espnow_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer_size, ARG_phy_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_size,  MP_ARG_INT, { .u_int = 526 } },
        { MP_QSTR_phy_rate,     MP_ARG_INT, { .u_int = WIFI_PHY_RATE_1M_L } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_obj_t *self = MP_STATE_PORT(espnow_singleton);

    if (!common_hal_espnow_deinited(self)) {
        mp_raise_RuntimeError(translate("Already running"));
    }

    // Allocate a new object
    self = mp_obj_malloc(espnow_obj_t, &espnow_type);

    // Construct the object
    common_hal_espnow_construct(self, args[ARG_buffer_size].u_int, args[ARG_phy_rate].u_int);

    // Set the global singleton pointer for the espnow protocol.
    MP_STATE_PORT(espnow_singleton) = self;
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitializes ESP-NOW and releases it for another program."""
//|         ...
STATIC mp_obj_t espnow_deinit(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);
    common_hal_espnow_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_deinit_obj, espnow_deinit);

//|     def __enter__(self) -> ESPNow:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t espnow_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return espnow_deinit(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espnow___exit___obj, 4, 4, espnow_obj___exit__);

// --- Send and Read messages ---

//|     def send(
//|         self,
//|         message: ReadableBuffer,
//|         peer: Optional[Peer] = None,
//|     ) -> None:
//|         """Send a message to the peer's mac address.
//|
//|         This blocks until a timeout of ``2`` seconds if the ESP-NOW internal buffers are full.
//|
//|         :param ReadableBuffer message: The message to send (length <= 250 bytes).
//|         :param Peer peer: Send message to this peer. If `None`, send to all registered peers.
//|         """
//|         ...
STATIC mp_obj_t espnow_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_message, ARG_peer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_message,  MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_peer,     MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_obj_t *self = pos_args[0];
    espnow_check_for_deinit(self);

    // Get a pointer to the data buffer of the message
    mp_buffer_info_t message;
    mp_get_buffer_raise(args[ARG_message].u_obj, &message, MP_BUFFER_READ);

    const uint8_t *mac = NULL;
    if (args[ARG_peer].u_obj != mp_const_none) {
        const espnow_peer_obj_t *peer = MP_OBJ_FROM_PTR(mp_arg_validate_type_or_none(args[ARG_peer].u_obj, &espnow_peer_type, MP_QSTR_peer));
        mac = peer->peer_info.peer_addr;
    }

    return common_hal_espnow_send(self, &message, mac);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_send_obj, 2, espnow_send);

//|     def read(self) -> Optional[ESPNowPacket]:
//|         """Read a packet from the receive buffer.
//|
//|         This is non-blocking, the packet is received asynchronously from the peer(s).
//|
//|         :returns: An `ESPNowPacket` if available in the buffer, otherwise `None`."""
//|         ...
STATIC mp_obj_t espnow_read(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);

    return common_hal_espnow_read(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_read_obj, espnow_read);

//|     send_success: int
//|     """The number of tx packets received by the peer(s) ``ESP_NOW_SEND_SUCCESS``. (read-only)"""
//|
STATIC mp_obj_t espnow_get_send_success(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->send_success);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_send_success_obj, espnow_get_send_success);

MP_PROPERTY_GETTER(espnow_send_success_obj,
    (mp_obj_t)&espnow_get_send_success_obj);

//|     send_failure: int
//|     """The number of failed tx packets ``ESP_NOW_SEND_FAIL``. (read-only)"""
//|
STATIC mp_obj_t espnow_send_get_failure(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->send_failure);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_send_get_failure_obj, espnow_send_get_failure);

MP_PROPERTY_GETTER(espnow_send_failure_obj,
    (mp_obj_t)&espnow_send_get_failure_obj);

//|     read_success: int
//|     """The number of rx packets captured in the buffer. (read-only)"""
//|
STATIC mp_obj_t espnow_get_read_success(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->read_success);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_read_success_obj, espnow_get_read_success);

MP_PROPERTY_GETTER(espnow_read_success_obj,
    (mp_obj_t)&espnow_get_read_success_obj);

//|     read_failure: int
//|     """The number of dropped rx packets due to buffer overflow. (read-only)"""
//|
STATIC mp_obj_t espnow_read_get_failure(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->read_failure);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_read_get_failure_obj, espnow_read_get_failure);

MP_PROPERTY_GETTER(espnow_read_failure_obj,
    (mp_obj_t)&espnow_read_get_failure_obj);

//|     def set_pmk(self, pmk: ReadableBuffer) -> None:
//|         """Set the ESP-NOW Primary Master Key (pmk) for encrypted communications.
//|
//|         :param ReadableBuffer pmk: The ESP-NOW Primary Master Key (length = 16 bytes)."""
//|         ...
STATIC mp_obj_t espnow_set_pmk(mp_obj_t self_in, mp_obj_t key) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);
    common_hal_espnow_set_pmk(self, common_hal_espnow_get_bytes_len(key, ESP_NOW_KEY_LEN));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_pmk_obj, espnow_set_pmk);

//|     buffer_size: int
//|     """The size of the internal ring buffer. (read-only)"""
//|
STATIC mp_obj_t espnow_get_buffer_size(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->recv_buffer_size);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_buffer_size_obj, espnow_get_buffer_size);

MP_PROPERTY_GETTER(espnow_buffer_size_obj,
    (mp_obj_t)&espnow_get_buffer_size_obj);

//|     phy_rate: int
//|     """The ESP-NOW physical layer rate.
//|     `wifi_phy_rate_t <https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/network/esp_wifi.html#_CPPv415wifi_phy_rate_t>`_
//|     """
//|
STATIC mp_obj_t espnow_get_phy_rate(const mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->phy_rate);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_phy_rate_obj, espnow_get_phy_rate);

STATIC mp_obj_t espnow_set_phy_rate(const mp_obj_t self_in, const mp_obj_t value) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);
    common_hal_espnow_set_phy_rate(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(espnow_set_phy_rate_obj, espnow_set_phy_rate);

MP_PROPERTY_GETSET(espnow_phy_rate_obj,
    (mp_obj_t)&espnow_get_phy_rate_obj,
    (mp_obj_t)&espnow_set_phy_rate_obj);

// --- Peer Related Properties ---

//|     peers: Peers
//|     """The peer info records for all registered `ESPNow` peers. (read-only)"""
//|
STATIC mp_obj_t espnow_get_peers(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->peers);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_get_peers_obj, espnow_get_peers);

MP_PROPERTY_GETTER(espnow_peers_obj,
    (mp_obj_t)&espnow_get_peers_obj);

STATIC const mp_rom_map_elem_t espnow_locals_dict_table[] = {
    // Context managers
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&espnow___exit___obj) },

    // Deinit the object
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&espnow_deinit_obj) },

    // Send messages
    { MP_ROM_QSTR(MP_QSTR_send),         MP_ROM_PTR(&espnow_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_success), MP_ROM_PTR(&espnow_send_success_obj)},
    { MP_ROM_QSTR(MP_QSTR_send_failure), MP_ROM_PTR(&espnow_send_failure_obj)},

    // Read messages
    { MP_ROM_QSTR(MP_QSTR_read),         MP_ROM_PTR(&espnow_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_success), MP_ROM_PTR(&espnow_read_success_obj)},
    { MP_ROM_QSTR(MP_QSTR_read_failure), MP_ROM_PTR(&espnow_read_failure_obj)},

    // Config parameters
    { MP_ROM_QSTR(MP_QSTR_set_pmk),      MP_ROM_PTR(&espnow_set_pmk_obj) },
    { MP_ROM_QSTR(MP_QSTR_buffer_size),  MP_ROM_PTR(&espnow_buffer_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_phy_rate),     MP_ROM_PTR(&espnow_phy_rate_obj) },

    // Peer related properties
    { MP_ROM_QSTR(MP_QSTR_peers),        MP_ROM_PTR(&espnow_peers_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_locals_dict, espnow_locals_dict_table);

// --- Dummy Buffer Protocol support ---
// ...so asyncio can poll.ipoll() on this device

// Support ioctl(MP_STREAM_POLL, ) for asyncio
STATIC mp_uint_t espnow_stream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);
    switch (request) {
        case MP_STREAM_POLL: {
            mp_uint_t flags = arg;
            mp_uint_t ret = 0;
            if ((flags & MP_STREAM_POLL_RD) && ringbuf_num_filled(self->recv_buffer) > 0) {
                ret |= MP_STREAM_POLL_RD;
            }
            return ret;
        }
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC const mp_stream_p_t espnow_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .ioctl = espnow_stream_ioctl,
};

//|     def __bool__(self) -> bool:
//|         """``True`` if `len()` is greater than zero.
//|         This is an easy way to check if the buffer is empty.
//|         """
//|         ...
//|     def __len__(self) -> int:
//|         """Return the number of `bytes` available to read. Used to implement ``len()``."""
//|         ...
//|
STATIC mp_obj_t espnow_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    espnow_check_for_deinit(self);
    size_t len = ringbuf_num_filled(self->recv_buffer);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return mp_obj_new_int_from_uint(len);
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

const mp_obj_type_t espnow_type = {
    { &mp_type_type },
    .name = MP_QSTR_ESPNow,
    .make_new = espnow_make_new,
    .locals_dict = (mp_obj_t)&espnow_locals_dict,
    .flags = MP_TYPE_FLAG_EXTENDED,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &espnow_stream_p,
        .unary_op = &espnow_unary_op
        ),
};
