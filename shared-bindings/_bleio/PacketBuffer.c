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

//| .. currentmodule:: _bleio
//|
//| :class:`PacketBuffer` -- Packet-oriented characteristic usage.
//| =====================================================================
//|
//| Accumulates a Characteristic's incoming packets in a FIFO buffer and facilitates packet aware
//| outgoing writes. A packet's size is either the characteristic length or the maximum transmission
//| unit (MTU), whichever is smaller. The MTU can change so check `packet_size` before creating a
//| buffer to store data.
//|
//| When we're the server, we ignore all connections besides the first to subscribe to
//| notifications.
//|
//| .. class:: PacketBuffer(characteristic, *, buffer_size)
//|
//|   Monitor the given Characteristic. Each time a new value is written to the Characteristic
//|   add the newly-written bytes to a FIFO buffer.
//|
//|   :param Characteristic characteristic: The Characteristic to monitor.
//|     It may be a local Characteristic provided by a Peripheral Service, or a remote Characteristic
//|     in a remote Service that a Central has connected to.
//|   :param int buffer_size: Size of ring buffer (in packets of the Characteristic's maximum
//|     length) that stores incoming packets coming from the peer.
//|
STATIC mp_obj_t bleio_packet_buffer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_characteristic, ARG_buffer_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_characteristic,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t characteristic = args[ARG_characteristic].u_obj;

    const int buffer_size = args[ARG_buffer_size].u_int;
    if (buffer_size < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_buffer_size);
    }

    if (!MP_OBJ_IS_TYPE(characteristic, &bleio_characteristic_type)) {
        mp_raise_TypeError(translate("Expected a Characteristic"));
    }

    bleio_packet_buffer_obj_t *self = m_new_obj(bleio_packet_buffer_obj_t);
    self->base.type = &bleio_packet_buffer_type;

    common_hal_bleio_packet_buffer_construct(self, MP_OBJ_TO_PTR(characteristic), buffer_size);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(bleio_packet_buffer_obj_t *self) {
    if (common_hal_bleio_packet_buffer_deinited(self)) {
        raise_deinited_error();
    }
}

//|   .. method:: readinto(buf)
//|
//|     Reads a single BLE packet into the ``buf``. Raises an exception if the next packet is longer
//|     than the given buffer. Use `packet_size` to read the maximum length of a single packet.
//|
//|     :return: number of bytes read and stored into ``buf``
//|     :rtype: int
//|
STATIC mp_obj_t bleio_packet_buffer_readinto(mp_obj_t self_in, mp_obj_t buffer_obj) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_obj, &bufinfo, MP_BUFFER_WRITE);

    int size = common_hal_bleio_packet_buffer_readinto(self, bufinfo.buf, bufinfo.len);
    if (size < 0) {
        mp_raise_ValueError_varg(translate("Buffer too short by %d bytes"), size * -1);
    }

    return MP_OBJ_NEW_SMALL_INT(size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bleio_packet_buffer_readinto_obj, bleio_packet_buffer_readinto);

//|   .. method:: write(data, *, header=None)
//|
//|     Writes all bytes from data into the same outgoing packet. The bytes from header are included
//|     before data when the pending packet is currently empty.
//|
//|     This does not block until the data is sent. It only blocks until the data is pending.
//|
// TODO: Add a kwarg `merge=False` to dictate whether subsequent writes are merged into a pending
// one.
STATIC mp_obj_t bleio_packet_buffer_write(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_header };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_header, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    mp_buffer_info_t data_bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data_bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t header_bufinfo;
    header_bufinfo.len = 0;
    if (args[ARG_header].u_obj != MP_OBJ_NULL) {
        mp_get_buffer_raise(args[ARG_header].u_obj, &header_bufinfo, MP_BUFFER_READ);
    }

    common_hal_bleio_packet_buffer_write(self, data_bufinfo.buf, data_bufinfo.len,
                                         header_bufinfo.buf, header_bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bleio_packet_buffer_write_obj, 1, bleio_packet_buffer_write);

//|   .. method:: deinit()
//|
//|     Disable permanently.
//|
STATIC mp_obj_t bleio_packet_buffer_deinit(mp_obj_t self_in) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_bleio_packet_buffer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_packet_buffer_deinit_obj, bleio_packet_buffer_deinit);

//|   .. attribute:: packet_size
//|
//|     Maximum size of each packet in bytes. This is the minimum of the Characteristic length and
//|     the negotiated Maximum Transfer Unit (MTU).
//|
STATIC mp_obj_t bleio_packet_buffer_get_packet_size(mp_obj_t self_in) {
    bleio_packet_buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_bleio_packet_buffer_get_packet_size(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bleio_packet_buffer_get_packet_size_obj, bleio_packet_buffer_get_packet_size);

const mp_obj_property_t bleio_packet_buffer_packet_size_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&bleio_packet_buffer_get_packet_size_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

STATIC const mp_rom_map_elem_t bleio_packet_buffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),             MP_ROM_PTR(&bleio_packet_buffer_deinit_obj) },

    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto),       MP_ROM_PTR(&bleio_packet_buffer_readinto_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),          MP_ROM_PTR(&bleio_packet_buffer_write_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_packet_size),    MP_ROM_PTR(&bleio_packet_buffer_packet_size_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_packet_buffer_locals_dict, bleio_packet_buffer_locals_dict_table);


const mp_obj_type_t bleio_packet_buffer_type = {
    { &mp_type_type },
    .name = MP_QSTR_PacketBuffer,
    .make_new = bleio_packet_buffer_make_new,
    .locals_dict = (mp_obj_dict_t*)&bleio_packet_buffer_locals_dict
};
