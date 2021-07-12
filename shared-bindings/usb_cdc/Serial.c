/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "shared-bindings/usb_cdc/Serial.h"
#include "shared-bindings/util.h"

#include "py/ioctl.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/translate.h"

//| class Serial:
//|     """Receives cdc commands over USB"""
//|
//|     def __init__(self) -> None:
//|         """You cannot create an instance of `usb_cdc.Serial`.
//|         The available instances are in the ``usb_cdc.serials`` tuple."""
//|         ...
//|
//|     def read(self, size: int = 1) -> bytes:
//|         """Read at most ``size`` bytes. If ``size`` exceeds the internal buffer size
//|         only the bytes in the buffer will be read. If `timeout` is > 0 or ``None``,
//|         and fewer than ``size`` bytes are available, keep waiting until the timeout
//|         expires or ``size`` bytes are available.
//|
//|         :return: Data read
//|         :rtype: bytes"""
//|         ...
//|
//|     def readinto(self, buf: WriteableBuffer) -> int:
//|         """Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
//|         that many bytes, subject to `timeout`.  Otherwise, read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: bytes"""
//|         ...
//|
//|     def readline(self, size: int = -1) -> Optional[bytes]:
//|         r"""Read a line ending in a newline character ("\\n"), including the newline.
//|         Return everything readable if no newline is found and ``timeout`` is 0.
//|         Return ``None`` in case of error.
//|
//|         This is a binary stream: the newline character "\\n" cannot be changed.
//|         If the host computer transmits "\\r" it will also be included as part of the line.
//|
//|         :param int size: maximum number of characters to read. ``-1`` means as many as possible.
//|         :return: the line read
//|         :rtype: bytes or None"""
//|         ...
//|
//|     def readlines(self) -> List[Optional[bytes]]:
//|         """Read multiple lines as a list, using `readline()`.
//|
//|         .. warning:: If ``timeout`` is ``None``,
//|           `readlines()` will never return, because there is no way to indicate end of stream.
//|
//|         :return: a list of the line read
//|         :rtype: list"""
//|         ...
//|
//|     def write(self, buf: ReadableBuffer) -> int:
//|         """Write as many bytes as possible from the buffer of bytes.
//|
//|         :return: the number of bytes written
//|         :rtype: int"""
//|         ...
//|
//|     def flush(self) -> None:
//|         """Force out any unwritten bytes, waiting until they are written."""
//|         ...
//|

// These three methods are used by the shared stream methods.
STATIC mp_uint_t usb_cdc_serial_read_stream(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return common_hal_usb_cdc_serial_read(self, buf, size, errcode);
}

STATIC mp_uint_t usb_cdc_serial_write_stream(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    return common_hal_usb_cdc_serial_write(self, buf, size, errcode);
}

STATIC mp_uint_t usb_cdc_serial_ioctl_stream(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret = 0;
    switch (request) {
        case MP_IOCTL_POLL: {
            mp_uint_t flags = arg;
            ret = 0;
            if ((flags & MP_IOCTL_POLL_RD) && common_hal_usb_cdc_serial_get_in_waiting(self) > 0) {
                ret |= MP_IOCTL_POLL_RD;
            }
            if ((flags & MP_IOCTL_POLL_WR) && common_hal_usb_cdc_serial_get_out_waiting(self) == 0) {
                ret |= MP_IOCTL_POLL_WR;
            }
            break;
        }

        case MP_STREAM_FLUSH:
            common_hal_usb_cdc_serial_flush(self);
            break;

        default:
            *errcode = MP_EINVAL;
            ret = MP_STREAM_ERROR;
    }
    return ret;
}

//|     connected: bool
//|     """True if this Serial is connected to a host. (read-only)
//|
//|     .. note:: The host is considered to be connected if it is asserting DTR (Data Terminal Ready).
//|       Most terminal programs and ``pyserial`` assert DTR when opening a serial connection.
//|       However, the C# ``SerialPort`` API does not. You must set ``SerialPort.DtrEnable``.
//|     """
//|
STATIC mp_obj_t usb_cdc_serial_get_connected(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_usb_cdc_serial_get_connected(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_get_connected_obj, usb_cdc_serial_get_connected);

const mp_obj_property_t usb_cdc_serial_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_cdc_serial_get_connected_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     in_waiting: int
//|     """Returns the number of bytes waiting to be read on the USB serial input. (read-only)"""
//|
STATIC mp_obj_t usb_cdc_serial_get_in_waiting(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_usb_cdc_serial_get_in_waiting(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_get_in_waiting_obj, usb_cdc_serial_get_in_waiting);

const mp_obj_property_t usb_cdc_serial_in_waiting_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_cdc_serial_get_in_waiting_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     out_waiting: int
//|     """Returns the number of bytes waiting to be written on the USB serial output. (read-only)"""
//|
STATIC mp_obj_t usb_cdc_serial_get_out_waiting(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_usb_cdc_serial_get_out_waiting(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_get_out_waiting_obj, usb_cdc_serial_get_out_waiting);

const mp_obj_property_t usb_cdc_serial_out_waiting_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_cdc_serial_get_out_waiting_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def reset_input_buffer(self) -> None:
//|         """Clears any unread bytes."""
//|         ...
//|
STATIC mp_obj_t usb_cdc_serial_reset_input_buffer(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_cdc_serial_reset_input_buffer(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_reset_input_buffer_obj, usb_cdc_serial_reset_input_buffer);

//|     def reset_output_buffer(self) -> None:
//|         """Clears any unwritten bytes."""
//|         ...
//|
STATIC mp_obj_t usb_cdc_serial_reset_output_buffer(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_cdc_serial_reset_output_buffer(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_reset_output_buffer_obj, usb_cdc_serial_reset_output_buffer);

//|     timeout: Optional[float]
//|     """The initial value of `timeout` is ``None``. If ``None``, wait indefinitely to satisfy
//|     the conditions of a read operation. If 0, do not wait. If > 0, wait only ``timeout`` seconds."""
//|
STATIC mp_obj_t usb_cdc_serial_get_timeout(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t timeout = common_hal_usb_cdc_serial_get_timeout(self);
    return (timeout < 0.0f) ? mp_const_none : mp_obj_new_float(self->timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_get_timeout_obj, usb_cdc_serial_get_timeout);

STATIC mp_obj_t usb_cdc_serial_set_timeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_cdc_serial_set_timeout(self,
        timeout_in == mp_const_none ? -1.0f : mp_obj_get_float(timeout_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(usb_cdc_serial_set_timeout_obj, usb_cdc_serial_set_timeout);

const mp_obj_property_t usb_cdc_serial_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_cdc_serial_get_timeout_obj,
              (mp_obj_t)&usb_cdc_serial_set_timeout_obj,
              MP_ROM_NONE},
};

//|     write_timeout: Optional[float]
//|     """The initial value of `write_timeout` is ``None``. If ``None``, wait indefinitely to finish
//|     writing all the bytes passed to ``write()``.If 0, do not wait.
//|     If > 0, wait only ``write_timeout`` seconds."""
//|
STATIC mp_obj_t usb_cdc_serial_get_write_timeout(mp_obj_t self_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t write_timeout = common_hal_usb_cdc_serial_get_write_timeout(self);
    return (write_timeout < 0.0f) ? mp_const_none : mp_obj_new_float(self->write_timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_cdc_serial_get_write_timeout_obj, usb_cdc_serial_get_write_timeout);

STATIC mp_obj_t usb_cdc_serial_set_write_timeout(mp_obj_t self_in, mp_obj_t write_timeout_in) {
    usb_cdc_serial_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_cdc_serial_set_write_timeout(self,
        write_timeout_in == mp_const_none ? -1.0f : mp_obj_get_float(write_timeout_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(usb_cdc_serial_set_write_timeout_obj, usb_cdc_serial_set_write_timeout);

const mp_obj_property_t usb_cdc_serial_write_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&usb_cdc_serial_get_write_timeout_obj,
              (mp_obj_t)&usb_cdc_serial_set_write_timeout_obj,
              MP_ROM_NONE},
};


STATIC const mp_rom_map_elem_t usb_cdc_serial_locals_dict_table[] = {
    // Standard stream methods.
    { MP_ROM_QSTR(MP_QSTR_flush),        MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline),     MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_ROM_QSTR(MP_QSTR_readlines),    MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },

    // Other pyserial-inspired attributes.
    { MP_OBJ_NEW_QSTR(MP_QSTR_in_waiting),          MP_ROM_PTR(&usb_cdc_serial_in_waiting_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_out_waiting),         MP_ROM_PTR(&usb_cdc_serial_out_waiting_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_input_buffer),  MP_ROM_PTR(&usb_cdc_serial_reset_input_buffer_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_output_buffer), MP_ROM_PTR(&usb_cdc_serial_reset_output_buffer_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_timeout),             MP_ROM_PTR(&usb_cdc_serial_timeout_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_timeout),       MP_ROM_PTR(&usb_cdc_serial_write_timeout_obj) },

    // Not in pyserial protocol.
    { MP_OBJ_NEW_QSTR(MP_QSTR_connected),     MP_ROM_PTR(&usb_cdc_serial_connected_obj) },



};
STATIC MP_DEFINE_CONST_DICT(usb_cdc_serial_locals_dict, usb_cdc_serial_locals_dict_table);

STATIC const mp_stream_p_t usb_cdc_serial_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = usb_cdc_serial_read_stream,
    .write = usb_cdc_serial_write_stream,
    .ioctl = usb_cdc_serial_ioctl_stream,
    .is_text = false,
    .pyserial_read_compatibility = true,
    .pyserial_readinto_compatibility = true,
    .pyserial_dont_return_none_compatibility = true,
};

const mp_obj_type_t usb_cdc_serial_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Serial,
    .locals_dict = (mp_obj_dict_t *)&usb_cdc_serial_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &usb_cdc_serial_stream_p,
        ),
};
