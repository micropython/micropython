/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
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

#include "py/objproperty.h"
#include "py/runtime.h"

#include "bindings/espnow/Peer.h"
#include "common-hal/espnow/__init__.h"

// --- Send and Receive ESP-NOW data ---

//| class Communicate:
//|     """Provides methods and statistics related to communication
//|     with the ESP-NOW peers.
//|
//|     Dictionary:
//|         * "Send" = "Transmit" = ``TX``
//|         * "Recv" = "Receive" = ``RX``
//|     """
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of `Communicate`."""
//|         ...

//|     job: str
//|     """Used to decide whether to call `__send` or `__recv`. (read-only)"""
//|
STATIC mp_obj_t espnow_com_get_job(const mp_obj_t self_in) {
    espnow_com_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_QSTR(self->job);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_com_get_job_obj, espnow_com_get_job);

MP_PROPERTY_GETTER(espnow_com_job_obj,
    (mp_obj_t)&espnow_com_get_job_obj);

//|     success: int
//|     """The number of successes. (read-only)
//|
//|     * In case of transmit ``TX``:
//|         The number of tx packets received by the peer(s) ``ESP_NOW_SEND_SUCCESS``.
//|
//|     * In case of receive ``RX``:
//|         The number of rx packets captured in the buffer."""
//|
STATIC mp_obj_t espnow_com_get_success(const mp_obj_t self_in) {
    espnow_com_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->success);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_com_get_success_obj, espnow_com_get_success);

MP_PROPERTY_GETTER(espnow_com_success_obj,
    (mp_obj_t)&espnow_com_get_success_obj);

//|     failure: int
//|     """The number of failures. (read-only)
//|
//|     * In case of transmit ``TX``:
//|         The number of failed tx packets ``ESP_NOW_SEND_FAIL``.
//|
//|     * In case of receive ``RX``:
//|         The number of dropped rx packets due to buffer overflow."""
//|
STATIC mp_obj_t espnow_com_get_failure(const mp_obj_t self_in) {
    espnow_com_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->failure);
}
MP_DEFINE_CONST_FUN_OBJ_1(espnow_com_get_failure_obj, espnow_com_get_failure);

MP_PROPERTY_GETTER(espnow_com_failure_obj,
    (mp_obj_t)&espnow_com_get_failure_obj);

//|     def __send(
//|         self,
//|         message: ReadableBuffer,
//|         peer: Peer,
//|     ) -> bool:
//|         """Send a message to the peer's mac address.
//|
//|         :param ReadableBuffer message: The message to send (length <= 250 bytes).
//|         :param Peer peer: Send message to this peer. If `None`, send to all registered peers.
//|         """
//|         ...
STATIC mp_obj_t espnow_com___send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_message, ARG_peer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_message,  MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_peer,     MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espnow_obj_t *self = pos_args[0];
    common_hal_espnow_check_for_deinit(self);

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
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(espnow_com___send_obj, 2, espnow_com___send);

//|     def __recv(self) -> Optional[ESPNowPacket]:
//|         """Receive a message from the peer(s).
//|
//|         :returns: An `ESPNowPacket` if available in the buffer, otherwise `None`."""
//|         ...
STATIC mp_obj_t espnow_com___recv(mp_obj_t self_in) {
    espnow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espnow_check_for_deinit(self);

    return common_hal_espnow_recv(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(espnow_com___recv_obj, espnow_com___recv);

STATIC const mp_rom_map_elem_t espnow_com_locals_dict_table[] = {
    // Config parameters
    { MP_ROM_QSTR(MP_QSTR_job),     MP_ROM_PTR(&espnow_com_job_obj) },

    // Packet statistics
    { MP_ROM_QSTR(MP_QSTR_success), MP_ROM_PTR(&espnow_com_success_obj) },
    { MP_ROM_QSTR(MP_QSTR_failure), MP_ROM_PTR(&espnow_com_failure_obj) },

    // Communication methods
    { MP_ROM_QSTR(MP_QSTR___send), MP_ROM_PTR(&espnow_com___send_obj) },
    { MP_ROM_QSTR(MP_QSTR___recv), MP_ROM_PTR(&espnow_com___recv_obj) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_com_locals_dict, espnow_com_locals_dict_table);

//|     def __call__(self, *args: Optional[Any], **kwargs: Optional[Any]) -> Optional[Any]:
//|         """Calls the private `__send` or `__recv` methods with the supplied ``args`` and ``kwargs``
//|         based on whether the job parameter is set to ``send`` or ``recv``."""
//|         ...
//|
STATIC mp_obj_t espnow_com_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    espnow_com_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t meth = NULL;
    switch (self->job) {
        case MP_QSTR_send:
            meth = MP_OBJ_FROM_PTR(&espnow_com___send_obj);
            break;
        case MP_QSTR_recv:
            meth = MP_OBJ_FROM_PTR(&espnow_com___recv_obj);
            break;
        default:
            break;
    }
    return meth ? mp_call_method_self_n_kw(meth, MP_STATE_PORT(espnow_singleton), n_args, n_kw, args) : mp_const_none;
}

espnow_com_obj_t *espnow_com_new(qstr job) {
    espnow_com_obj_t *self = m_new_obj(espnow_com_obj_t);
    self->base.type = &espnow_com_type;
    self->job = job;
    return self;
}

const mp_obj_type_t espnow_com_type = {
    { &mp_type_type },
    .name = MP_QSTR_Communicate,
    .locals_dict = (mp_obj_t)&espnow_com_locals_dict,
    .flags = MP_TYPE_FLAG_EXTENDED,
    MP_TYPE_EXTENDED_FIELDS(
        .call = &espnow_com_call,
        ),
};
