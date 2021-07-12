/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "shared-bindings/usb_midi/PortIn.h"
#include "shared-bindings/util.h"

#include "py/ioctl.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

//| class PortIn:
//|     """Receives midi commands over USB"""
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of `usb_midi.PortIn`.
//|
//|         PortIn objects are constructed for every corresponding entry in the USB
//|         descriptor and added to the ``usb_midi.ports`` tuple."""
//|         ...
//|

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
//|     def readinto(self, buf: WriteableBuffer, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
//|         that many bytes.  Otherwise, read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: bytes or None"""
//|         ...
//|

// These three methods are used by the shared stream methods.
STATIC mp_uint_t usb_midi_portin_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    usb_midi_portin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return common_hal_usb_midi_portin_read(self, buf, size, errcode);
}

STATIC mp_uint_t usb_midi_portin_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    usb_midi_portin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && common_hal_usb_midi_portin_bytes_available(self) > 0) {
            ret |= MP_IOCTL_POLL_RD;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_rom_map_elem_t usb_midi_portin_locals_dict_table[] = {
    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
};
STATIC MP_DEFINE_CONST_DICT(usb_midi_portin_locals_dict, usb_midi_portin_locals_dict_table);

STATIC const mp_stream_p_t usb_midi_portin_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = usb_midi_portin_read,
    .write = NULL,
    .ioctl = usb_midi_portin_ioctl,
    .is_text = false,
};

const mp_obj_type_t usb_midi_portin_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_PortIn,
    .locals_dict = (mp_obj_dict_t *)&usb_midi_portin_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &usb_midi_portin_stream_p,
        ),
};
