// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/terminalio/Terminal.h"
#include "shared-bindings/util.h"

#include "py/stream.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/fontio/BuiltinFont.h"

//| class Terminal:
//|     """Display a character stream with a TileGrid
//|
//|     ASCII control:
//|     * ``\\r`` - Move cursor to column 1
//|     * ``\\n`` - Move cursor down a row
//|     * ``\\b`` - Move cursor left one if possible
//|
//|     OSC control sequences:
//|     * ``ESC ] 0; <s> ESC \\`` - Set title bar to <s>
//|     * ``ESC ] ####; <s> ESC \\`` - Ignored
//|
//|     VT100 control sequences:
//|     * ``ESC [ K`` - Clear the remainder of the line
//|     * ``ESC [ #### D`` - Move the cursor to the left by ####
//|     * ``ESC [ 2 J`` - Erase the entire display
//|     * ``ESC [ nnnn ; mmmm H`` - Move the cursor to mmmm, nnnn.
//|     """
//|
//|     def __init__(
//|         self,
//|         scroll_area: displayio.TileGrid,
//|         font: fontio.BuiltinFont,
//|         *,
//|         status_bar: Optional[displayio.TileGrid] = None
//|     ) -> None:
//|         """Terminal manages tile indices and cursor position based on VT100 commands. The font should be
//|         a `fontio.BuiltinFont` and the TileGrid's bitmap should match the font's bitmap."""
//|         ...

static mp_obj_t terminalio_terminal_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_scroll_area, ARG_font, ARG_status_bar };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scroll_area, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_font, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_status_bar, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_tilegrid_t *scroll_area = mp_arg_validate_type(args[ARG_scroll_area].u_obj, &displayio_tilegrid_type, MP_QSTR_scroll_area);
    displayio_tilegrid_t *status_bar = NULL;
    if (args[ARG_status_bar].u_obj != mp_const_none) {
        status_bar = mp_arg_validate_type(args[ARG_status_bar].u_obj, &displayio_tilegrid_type, MP_QSTR_status_bar);
    }

    fontio_builtinfont_t *font = mp_arg_validate_type(args[ARG_font].u_obj, &fontio_builtinfont_type, MP_QSTR_font);

    mp_arg_validate_int_min(scroll_area->width_in_tiles, 2, MP_QSTR_scroll_area_width);
    mp_arg_validate_int_min(scroll_area->height_in_tiles, 2, MP_QSTR_scroll_area_height);

    terminalio_terminal_obj_t *self = mp_obj_malloc(terminalio_terminal_obj_t, &terminalio_terminal_type);

    common_hal_terminalio_terminal_construct(self, scroll_area, font, status_bar);
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
static mp_uint_t terminalio_terminal_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    return common_hal_terminalio_terminal_write(self, buf, size, errcode);
}

static mp_uint_t terminalio_terminal_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_WR) && common_hal_terminalio_terminal_ready_to_tx(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static const mp_rom_map_elem_t terminalio_terminal_locals_dict_table[] = {
    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },
};
static MP_DEFINE_CONST_DICT(terminalio_terminal_locals_dict, terminalio_terminal_locals_dict_table);

static const mp_stream_p_t terminalio_terminal_stream_p = {
    .read = NULL,
    .write = terminalio_terminal_write,
    .ioctl = terminalio_terminal_ioctl,
    .is_text = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    terminalio_terminal_type,
    MP_QSTR_Terminal,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    make_new, terminalio_terminal_make_new,
    locals_dict, (mp_obj_dict_t *)&terminalio_terminal_locals_dict,
    iter, mp_stream_unbuffered_iter,
    protocol, &terminalio_terminal_stream_p
    );
