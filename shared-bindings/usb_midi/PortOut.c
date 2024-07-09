// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/usb_midi/PortOut.h"
#include "shared-bindings/util.h"

#include "py/stream.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"

//| class PortOut:
//|     """Sends midi messages to a computer over USB"""
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of `usb_midi.PortOut`.
//|
//|         PortOut objects are constructed for every corresponding entry in the USB
//|         descriptor and added to the ``usb_midi.ports`` tuple."""

// These are standard stream methods. Code is in py/stream.c.
//
//|     def write(self, buf: ReadableBuffer) -> Optional[int]:
//|         """Write the buffer of bytes to the bus.
//|
//|         :return: the number of bytes written
//|         :rtype: int or None"""
//|         ...
//|

static mp_uint_t usb_midi_portout_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    usb_midi_portout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    return common_hal_usb_midi_portout_write(self, buf, size, errcode);
}

static mp_uint_t usb_midi_portout_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    usb_midi_portout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_WR) && common_hal_usb_midi_portout_ready_to_tx(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static const mp_rom_map_elem_t usb_midi_portout_locals_dict_table[] = {
    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },
};
static MP_DEFINE_CONST_DICT(usb_midi_portout_locals_dict, usb_midi_portout_locals_dict_table);

static const mp_stream_p_t usb_midi_portout_stream_p = {
    .read = NULL,
    .write = usb_midi_portout_write,
    .ioctl = usb_midi_portout_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    usb_midi_portout_type,
    MP_QSTR_PortOut,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    locals_dict, &usb_midi_portout_locals_dict,
    iter, mp_stream_unbuffered_iter,
    protocol, &usb_midi_portout_stream_p
    );
