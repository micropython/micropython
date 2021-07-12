/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "py/mperrno.h"
#include "py/ioctl.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/util.h"

STATIC void raise_error_if_not_connected(bleio_characteristic_buffer_obj_t *self) {
    if (!common_hal_bleio_characteristic_buffer_connected(self)) {
        mp_raise_ConnectionError(translate("Not connected"));
    }
}

//| class CharacteristicBuffer:
//|     """Accumulates a Characteristic's incoming values in a FIFO buffer."""
//|
//|     def __init__(self, characteristic: Characteristic, *, timeout: int = 1, buffer_size: int = 64) -> None:
//|
//|         """Monitor the given Characteristic. Each time a new value is written to the Characteristic
//|         add the newly-written bytes to a FIFO buffer.
//|
//|         :param Characteristic characteristic: The Characteristic to monitor.
//|           It may be a local Characteristic provided by a Peripheral Service, or a remote Characteristic
//|           in a remote Service that a Central has connected to.
//|         :param int timeout:  the timeout in seconds to wait for the first character and between subsequent characters.
//|         :param int buffer_size: Size of ring buffer that stores incoming data coming from client.
//|           Must be >= 1."""
//|         ...
//|
STATIC mp_obj_t bleio_characteristic_buffer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_characteristic, ARG_timeout, ARG_buffer_size, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_characteristic,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t characteristic = args[ARG_characteristic].u_obj;

    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    if (timeout < 0.0f) {
        mp_raise_ValueError(translate("timeout must be >= 0.0"));
    }

    const int buffer_size = args[ARG_buffer_size].u_int;
    if (buffer_size < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_buffer_size);
    }

    if (!mp_obj_is_type(characteristic, &bleio_characteristic_type)) {
        mp_raise_TypeError(translate("Expected a Characteristic"));
    }

    bleio_characteristic_buffer_obj_t *self = m_new_obj(bleio_characteristic_buffer_obj_t);
    self->base.type = &bleio_characteristic_buffer_type;

    common_hal_bleio_characteristic_buffer_construct(self, MP_OBJ_TO_PTR(characteristic), timeout, buffer_size);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (common_hal_bleio_characteristic_buffer_deinited(self)) {
        raise_deinited_error();
    }
}

// These are standard stream methods. Code is in py/stream.c.
//
//|     def read(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read characters.  If ``nbytes`` is specified then read at most that many
//|         bytes. Otherwise, read everything that arrives until the connection
//|         times out. Providing the number of bytes expected is highly recommended
//|         because it will be faster.
//|
//|         :return: Data read
//|         :rtype: bytes or None"""
//|         ...
//|
//|     def readinto(self, buf: WriteableBuffer) -> Optional[int]:
//|         """Read bytes into the ``buf``. Read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: int or None (on a non-blocking error)"""
//|         ...
//|
//|     def readline(self) -> bytes:
//|         """Read a line, ending in a newline character.
//|
//|         :return: the line read
//|         :rtype: int or None"""
//|         ...
//|

// These three methods are used by the shared stream methods.
STATIC mp_uint_t bleio_characteristic_buffer_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    raise_error_if_not_connected(self);
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return common_hal_bleio_characteristic_buffer_read(self, buf, size, errcode);
}

STATIC mp_uint_t bleio_characteristic_buffer_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    mp_raise_NotImplementedError(translate("CharacteristicBuffer writing not provided"));
    return 0;
}

STATIC mp_uint_t bleio_characteristic_buffer_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    raise_error_if_not_connected(self);
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && common_hal_bleio_characteristic_buffer_rx_characters_available(self) > 0) {
            ret |= MP_IOCTL_POLL_RD;
        }
// No writing provided.
//        if ((flags & MP_IOCTL_POLL_WR) && common_hal_busio_uart_ready_to_tx(self)) {
//            ret |= MP_IOCTL_POLL_WR;
//        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

//|     in_waiting: int
//|     """The number of bytes in the input buffer, available to be read"""
//|
STATIC mp_obj_t bleio_characteristic_buffer_obj_get_in_waiting(mp_obj_t self_in) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_characteristic_buffer_rx_characters_available(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_buffer_get_in_waiting_obj, bleio_characteristic_buffer_obj_get_in_waiting);

const mp_obj_property_t bleio_characteristic_buffer_in_waiting_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&bleio_characteristic_buffer_get_in_waiting_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def reset_input_buffer(self) -> None:
//|         """Discard any unread characters in the input buffer."""
//|         ...
//|
STATIC mp_obj_t bleio_characteristic_buffer_obj_reset_input_buffer(mp_obj_t self_in) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_bleio_characteristic_buffer_clear_rx_buffer(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_buffer_reset_input_buffer_obj, bleio_characteristic_buffer_obj_reset_input_buffer);

//|     def deinit(self) -> None:
//|         """Disable permanently."""
//|         ...
//|
STATIC mp_obj_t bleio_characteristic_buffer_deinit(mp_obj_t self_in) {
    bleio_characteristic_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_bleio_characteristic_buffer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_characteristic_buffer_deinit_obj, bleio_characteristic_buffer_deinit);

STATIC const mp_rom_map_elem_t bleio_characteristic_buffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&bleio_characteristic_buffer_deinit_obj) },

    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    // CharacteristicBuffer is currently read-only.
    // { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_input_buffer), MP_ROM_PTR(&bleio_characteristic_buffer_reset_input_buffer_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_in_waiting), MP_ROM_PTR(&bleio_characteristic_buffer_in_waiting_obj) },

};

STATIC MP_DEFINE_CONST_DICT(bleio_characteristic_buffer_locals_dict, bleio_characteristic_buffer_locals_dict_table);

STATIC const mp_stream_p_t characteristic_buffer_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = bleio_characteristic_buffer_read,
    .write = bleio_characteristic_buffer_write,
    .ioctl = bleio_characteristic_buffer_ioctl,
    .is_text = false,
    // Disallow readinto() size parameter.
    .pyserial_readinto_compatibility = true,
};


const mp_obj_type_t bleio_characteristic_buffer_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_CharacteristicBuffer,
    .make_new = bleio_characteristic_buffer_make_new,
    .locals_dict = (mp_obj_dict_t *)&bleio_characteristic_buffer_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &characteristic_buffer_stream_p,
        ),
};
