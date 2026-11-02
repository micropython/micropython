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

#if CIRCUITPY_LVFONTIO
#include "shared-bindings/lvfontio/OnDiskFont.h"
#endif

//| class Terminal:
//|     """Terminal manages tile indices and cursor position based on VT100 commands. The ``font`` should be
//|     a `fontio.BuiltinFont` and the ``scroll_area`` TileGrid's bitmap should match the font's bitmap.
//|
//|     Display a character stream with a TileGrid
//|
//|     ASCII control:
//|
//|     * ``\\r`` - Move cursor to column 1
//|     * ``\\n`` - Move cursor down a row
//|     * ``\\b`` - Move cursor left one if possible
//|
//|     OSC control sequences:
//|
//|     * ``ESC ] 0; <s> ESC \\`` - Set title bar to <s>
//|     * ``ESC ] ####; <s> ESC \\`` - Ignored
//|
//|     VT100 control sequences:
//|
//|     * ``ESC [ K`` - Clear the remainder of the line
//|     * ``ESC [ 0 K`` - Clear the remainder of the line
//|     * ``ESC [ 1 K`` - Clear start of the line to cursor
//|     * ``ESC [ 2 K`` - Clear the entire line
//|     * ``ESC [ #### D`` - Move the cursor to the left by ####
//|     * ``ESC [ 2 J`` - Erase the entire display
//|     * ``ESC [ nnnn ; mmmm H`` - Move the cursor to mmmm, nnnn.
//|     * ``ESC [ H`` - Move the cursor to 0,0.
//|     * ``ESC M`` - Move the cursor up one line, scrolling if necessary.
//|     * ``ESC D`` - Move the cursor down one line, scrolling if necessary.
//|     * ``ESC [ r`` - Disable scrolling range (set to fullscreen).
//|     * ``ESC [ nnnn ; mmmm r`` - Set scrolling range between rows nnnn and mmmm.
//|     * ``ESC [ ## m`` - Set the terminal display attributes.
//|     * ``ESC [ ## ; ## m`` - Set the terminal display attributes.
//|     * ``ESC [ ## ; ## ; ## m`` - Set the terminal display attributes.
//|
//|     Supported Display attributes:
//|
//|     +--------+------------+------------+
//|     | Color  | Foreground | Background |
//|     +========+============+============+
//|     | Reset  | 0          | 0          |
//|     +--------+------------+------------+
//|     | Black  | 30         | 40         |
//|     +--------+------------+------------+
//|     | Red    | 31         | 41         |
//|     +--------+------------+------------+
//|     | Green  | 32         | 42         |
//|     +--------+------------+------------+
//|     | Yellow | 33         | 43         |
//|     +--------+------------+------------+
//|     | Blue   | 34         | 44         |
//|     +--------+------------+------------+
//|     | Magenta| 35         | 45         |
//|     +--------+------------+------------+
//|     | Cyan   | 36         | 46         |
//|     +--------+------------+------------+
//|     | White  | 37         | 47         |
//|     +--------+------------+------------+
//|
//|     .. note:: Colors apply to the whole terminal, not to individual characters. They change the
//|         ``scroll_area`` palette, so all text changes color at once, including text already on
//|         screen. The ``scroll_area`` ``pixel_shader`` must be a `displayio.Palette` with at least
//|         2 colors to use these attributes.
//|
//|         For color per character, see the
//|         `adafruit_color_terminal <https://github.com/adafruit/Adafruit_CircuitPython_Color_Terminal>`_
//|         library, which uses `tilepalettemapper`.
//|
//|     Example Usage:
//|
//|     .. code-block:: python
//|
//|         import time
//|         import displayio
//|         import supervisor
//|         from displayio import Group, TileGrid
//|         from terminalio import FONT, Terminal
//|
//|         main_group = Group()
//|         display = supervisor.runtime.display
//|         font_bb = FONT.get_bounding_box()
//|         screen_size = (display.width // font_bb[0], display.height // font_bb[1])
//|         char_size = FONT.get_bounding_box()
//|
//|         palette = displayio.Palette(2)
//|         palette[0] = 0x000000
//|         palette[1] = 0xffffff
//|
//|         tilegrid = TileGrid(
//|             bitmap=FONT.bitmap, width=screen_size[0], height=screen_size[1],
//|             tile_width=char_size[0], tile_height=char_size[1], pixel_shader=palette)
//|
//|         terminal = Terminal(tilegrid, FONT)
//|
//|         main_group.append(tilegrid)
//|         display.root_group = main_group
//|
//|         message = "Hello World\\n"
//|         terminal.write(message)
//|
//|         print(terminal.cursor_x, terminal.cursor_y)
//|         move_cursor = chr(27) + "[10;10H"
//|         terminal.write(f"Moving the cursor\\n{move_cursor} To here")
//|
//|         cursor_home = chr(27) + f"[{screen_size[1]};0H"
//|         terminal.write(cursor_home)
//|         i = 1
//|         while True:
//|             terminal.write(f"Writing again {i}\\n")
//|             i = i + 1
//|             time.sleep(1)
//|
//|
//|     """
//|
//|     def __init__(
//|         self,
//|         scroll_area: displayio.TileGrid,
//|         font: fontio.BuiltinFont,
//|         *,
//|         status_bar: Optional[displayio.TileGrid] = None,
//|     ) -> None:
//|         ...
//|

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

    mp_obj_t font = args[ARG_font].u_obj;

    // Ensure the font is one of the supported types
    bool valid_font = false;

    #if CIRCUITPY_FONTIO
    if (mp_obj_is_type(font, &fontio_builtinfont_type)) {
        valid_font = true;
    }
    #endif

    #if CIRCUITPY_LVFONTIO
    if (mp_obj_is_type(font, &lvfontio_ondiskfont_type)) {
        valid_font = true;
    }
    #endif

    if (!valid_font) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_font);
    }

    mp_arg_validate_int_min(scroll_area->width_in_tiles * scroll_area->height_in_tiles, 2, MP_QSTR_scroll_area_area);

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
//|
static mp_uint_t terminalio_terminal_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    return common_hal_terminalio_terminal_write(self, buf, size, errcode);
}

//|     cursor_x: int
//|     """The x position of the cursor."""
//|
static mp_obj_t terminalio_terminal_obj_get_cursor_x(mp_obj_t self_in) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_terminalio_terminal_get_cursor_x(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(terminalio_terminal_get_cursor_x_obj, terminalio_terminal_obj_get_cursor_x);

MP_PROPERTY_GETTER(terminalio_terminal_cursor_x_obj,
    (mp_obj_t)&terminalio_terminal_get_cursor_x_obj);

//|     cursor_y: int
//|     """The y position of the cursor."""
//|
static mp_obj_t terminalio_terminal_obj_get_cursor_y(mp_obj_t self_in) {
    terminalio_terminal_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_terminalio_terminal_get_cursor_y(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(terminalio_terminal_get_cursor_y_obj, terminalio_terminal_obj_get_cursor_y);

MP_PROPERTY_GETTER(terminalio_terminal_cursor_y_obj,
    (mp_obj_t)&terminalio_terminal_get_cursor_y_obj);

static mp_uint_t terminalio_terminal_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
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
    { MP_ROM_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_cursor_x), MP_ROM_PTR(&terminalio_terminal_cursor_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_cursor_y), MP_ROM_PTR(&terminalio_terminal_cursor_y_obj) },
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
    MP_TYPE_FLAG_ITER_IS_ITERNEXT | MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, terminalio_terminal_make_new,
    locals_dict, (mp_obj_dict_t *)&terminalio_terminal_locals_dict,
    iter, mp_stream_unbuffered_iter,
    protocol, &terminalio_terminal_stream_p
    );
