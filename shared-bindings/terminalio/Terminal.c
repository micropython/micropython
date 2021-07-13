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

#include "shared-bindings/terminalio/Terminal.h"
#include "shared-bindings/util.h"

#include "py/ioctl.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/fontio/BuiltinFont.h"
#include "supervisor/shared/translate.h"

//| class Terminal:
//|     """Display a character stream with a TileGrid"""
//|
//|     def __init__(self, tilegrid: displayio.TileGrid, font: fontio.BuiltinFont) -> None:
//|         """Terminal manages tile indices and cursor position based on VT100 commands. The font should be
//|         a `fontio.BuiltinFont` and the TileGrid's bitmap should match the font's bitmap."""
//|         ...
//|

STATIC mp_obj_t terminalio_terminal_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_tilegrid, ARG_font };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tilegrid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_font, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t tilegrid = args[ARG_tilegrid].u_obj;
    if (!mp_obj_is_type(tilegrid, &displayio_tilegrid_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), displayio_tilegrid_type.name);
    }

    mp_obj_t font = args[ARG_font].u_obj;
    if (!mp_obj_is_type(font, &fontio_builtinfont_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), fontio_builtinfont_type.name);
    }
    terminalio_terminal_obj_t *self = m_new_obj(terminalio_terminal_obj_t);
    self->base.type = &terminalio_terminal_type;
    common_hal_terminalio_terminal_construct(self, MP_OBJ_TO_PTR(tilegrid), MP_OBJ_TO_PTR(font));
    return MP_OBJ_FROM_PTR(self);
}

// These are standard stream methods. Code is in py/stream.c.
//
//|     def write(self, buf: ReadableBuffer) -> Optional[int]:
//|         """Write the buffer of bytes to the bus.
//|
//|         :return: the number of bytes written
//|         :rtype: int or None"""
//|         ...
//|
STATIC mp_uint_t terminalio_terminal_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    return common_hal_terminalio_terminal_write(self, buf, size, errcode);
}

STATIC mp_uint_t terminalio_terminal_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_WR) && common_hal_terminalio_terminal_ready_to_tx(self)) {
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_rom_map_elem_t terminalio_terminal_locals_dict_table[] = {
    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(terminalio_terminal_locals_dict, terminalio_terminal_locals_dict_table);

STATIC const mp_stream_p_t terminalio_terminal_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = NULL,
    .write = terminalio_terminal_write,
    .ioctl = terminalio_terminal_ioctl,
    .is_text = true,
};

const mp_obj_type_t terminalio_terminal_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Terminal,
    .make_new = terminalio_terminal_make_new,
    .locals_dict = (mp_obj_dict_t *)&terminalio_terminal_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &terminalio_terminal_stream_p,
        ),
};
