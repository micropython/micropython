/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/util.h"

//| class PacketBuffer:
//|     """Accumulates a Characteristic's incoming packets in a FIFO buffer and facilitates packet aware
//|     outgoing writes. A packet's size is either the characteristic length or the maximum transmission
//|     unit (MTU) minus overhead, whichever is smaller. The MTU can change so check `incoming_packet_length`
//|     and `outgoing_packet_length` before creating a buffer to store data.
//|
//|     When we're the server, we ignore all connections besides the first to subscribe to
//|     notifications."""
//|
//|     def __init__(self, characteristic: Characteristic, *, buffer_size: int, max_packet_size: Optional[int] = None) -> None:
//|         """Monitor the given Characteristic. Each time a new value is written to the Characteristic
//|         add the newly-written bytes to a FIFO buffer.
//|
//|         Monitor the given Characteristic. Each time a new value is written to the Characteristic
//|         add the newly-written packet of bytes to a FIFO buffer.
//|
//|         :param Characteristic characteristic: The Characteristic to monitor.
//|           It may be a local Characteristic provided by a Peripheral Service, or a remote Characteristic
//|           in a remote Service that a Central has connected to.
//|         :param int buffer_size: Size of ring buffer (in packets of the Characteristic's maximum
//|           length) that stores incoming packets coming from the peer.
//|         :param int max_packet_size: Maximum size of packets. Overrides value from the characteristic.
//|           (Remote characteristics may not have the correct length.)"""
//|         ...
//|
STATIC mp_obj_t bleio_packet_buffer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_characteristic, ARG_buffer_size, ARG_max_packet_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_characteristic,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_max_packet_size, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t characteristic = args[ARG_characteristic].u_obj;

    const mp_int_t buffer_size = args[ARG_buffer_size].u_int;
    if (buffer_size < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_buffer_size);
    }

    if (!mp_obj_is_type(characteristic, &bleio_characteristic_type)) {
        mp_raise_TypeError(translate("Expected a Characteristic"));
    }

    size_t max_packet_size = common_hal_bleio_characteristic_get_max_length(characteristic);
    if (args[ARG_max_packet_size].u_obj != mp_const_none) {
        max_packet_size = mp_obj_get_int(args[ARG_max_packet_size].u_obj);
    }

    bleio_packet_buffer_obj_t *self = m_new_obj(bleio_packet_buffer_obj_t);
    self->base.type = &bleio_packet_buffer_type;

    common_hal_bleio_packet_buffer_construct(self, MP_OBJ_TO_PTR(characteristic), buffer_size, max_packet_size);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(bleio_packet_buffer_obj_t *self) {
    if (common_hal_bleio_packet_buffer_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def readinto(self, buf: WriteableBuffer) -> int:
//|         """Reads a single BLE packet into the ``buf``. Raises an exception if the next packet is longer
//|         than the given buffer. Use `incoming_packet_length` to read the maximum length of a single packet.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: int"""
//|         ...
//|
STATIC mp_obj_t bleio_packet_buffer_readinto(mp_obj_t self_in, mp_obj_t buffer_obj) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_obj, &bufinfo, MP_BUFFER_WRITE);

    mp_int_t size = common_hal_bleio_packet_buffer_readinto(self, bufinfo.buf, bufinfo.len);
    if (size < 0) {
        mp_raise_ValueError_varg(translate("Buffer too short by %d bytes"), size * -1);
    }

    return MP_OBJ_NEW_SMALL_INT(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_packet_buffer_readinto_obj, bleio_packet_buffer_readinto);

//|     def write(self, data: ReadableBuffer, *, header: Optional[bytes] = None) -> int:
//|         """Writes all bytes from data into the same outgoing packet. The bytes from header are included
//|         before data when the pending packet is currently empty.
//|
//|         This does not block until the data is sent. It only blocks until the data is pending.
//|
//|         :return: number of bytes written. May include header bytes when packet is empty.
//|         :rtype: int"""
//|         ...
//|
// TODO: Add a kwarg `merge=False` to dictate whether subsequent writes are merged into a pending
// one.
STATIC mp_obj_t bleio_packet_buffer_write(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_header };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_header, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    mp_buffer_info_t data_bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t header_bufinfo;
    header_bufinfo.len = 0;
    if (args[ARG_header].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_header].u_obj, &header_bufinfo, MP_BUFFER_READ);
    }

    mp_int_t num_bytes_written = common_hal_bleio_packet_buffer_write(
        self, data_bufinfo.buf, data_bufinfo.len, header_bufinfo.buf, header_bufinfo.len);
    if (num_bytes_written < 0) {
        // TODO: Raise an error if not connected. Right now the not-connected error
        // is unreliable, because common_hal_bleio_packet_buffer_write()
        // checks for conn_handle being set, but setting that
        // can be delayed because conn_handle is discovered by spying on
        // gatts write events, which may not have been sent yet.
        //
        // IDEAL:
        // mp_raise_ConnectionError(translate("Not connected"));
        // TEMPORARY:
        num_bytes_written = 0;
    }
    return MP_OBJ_NEW_SMALL_INT(num_bytes_written);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_packet_buffer_write_obj, 1, bleio_packet_buffer_write);

//|     def deinit(self) -> None:
//|         """Disable permanently."""
//|         ...
STATIC mp_obj_t bleio_packet_buffer_deinit(mp_obj_t self_in) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_bleio_packet_buffer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_packet_buffer_deinit_obj, bleio_packet_buffer_deinit);

//|     incoming_packet_length: int
//|     """Maximum length in bytes of a packet we are reading."""
//|
STATIC mp_obj_t bleio_packet_buffer_get_incoming_packet_length(mp_obj_t self_in) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t size = common_hal_bleio_packet_buffer_get_incoming_packet_length(self);
    if (size < 0) {
        mp_raise_ValueError(translate("No connection: length cannot be determined"));
    }
    return MP_OBJ_NEW_SMALL_INT(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_packet_buffer_get_incoming_packet_length_obj, bleio_packet_buffer_get_incoming_packet_length);

const mp_obj_property_t bleio_packet_buffer_incoming_packet_length_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_packet_buffer_get_incoming_packet_length_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

//|     outgoing_packet_length: int
//|     """Maximum length in bytes of a packet we are writing."""
//|
STATIC mp_obj_t bleio_packet_buffer_get_outgoing_packet_length(mp_obj_t self_in) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t size = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);
    if (size < 0) {
        mp_raise_ValueError(translate("No connection: length cannot be determined"));
    }
    return MP_OBJ_NEW_SMALL_INT(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_packet_buffer_get_outgoing_packet_length_obj, bleio_packet_buffer_get_outgoing_packet_length);

const mp_obj_property_t bleio_packet_buffer_outgoing_packet_length_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_packet_buffer_get_outgoing_packet_length_obj,
               MP_ROM_NONE,
               MP_ROM_NONE },
};

STATIC const mp_rom_map_elem_t bleio_packet_buffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),                     MP_ROM_PTR(&bleio_packet_buffer_deinit_obj) },

    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto),               MP_ROM_PTR(&bleio_packet_buffer_readinto_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),                  MP_ROM_PTR(&bleio_packet_buffer_write_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_incoming_packet_length), MP_ROM_PTR(&bleio_packet_buffer_incoming_packet_length_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_outgoing_packet_length), MP_ROM_PTR(&bleio_packet_buffer_outgoing_packet_length_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_packet_buffer_locals_dict, bleio_packet_buffer_locals_dict_table);


const mp_obj_type_t bleio_packet_buffer_type = {
    { &mp_type_type },
    .name = MP_QSTR_PacketBuffer,
    .make_new = bleio_packet_buffer_make_new,
    .locals_dict = (mp_obj_dict_t *)&bleio_packet_buffer_locals_dict
};
