/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared-bindings/displayio/EPaperDisplay.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/translate.h"

//| class EPaperDisplay:
//|     """Manage updating an epaper display over a display bus
//|
//|     This initializes an epaper display and connects it into CircuitPython. Unlike other
//|     objects in CircuitPython, EPaperDisplay objects live until `displayio.release_displays()`
//|     is called. This is done so that CircuitPython can use the display itself.
//|
//|     Most people should not use this class directly. Use a specific display driver instead that will
//|     contain the startup and shutdown sequences at minimum."""
//|
//|     def __init__(self, display_bus: _DisplayBus,
//|                  start_sequence: ReadableBuffer, stop_sequence: ReadableBuffer, *,
//|                  width: int, height: int, ram_width: int, ram_height: int,
//|                  colstart: int = 0, rowstart: int = 0, rotation: int = 0,
//|                  set_column_window_command: Optional[int] = None,
//|                  set_row_window_command: Optional[int] = None, single_byte_bounds: bool = False,
//|                  write_black_ram_command: int, black_bits_inverted: bool = False,
//|                  write_color_ram_command: Optional[int] = None,
//|                  color_bits_inverted: bool = False, highlight_color: int = 0x000000,
//|                  refresh_display_command: int, refresh_time: float = 40,
//|                  busy_pin: Optional[microcontroller.Pin] = None, busy_state: bool = True,
//|                  seconds_per_frame: float = 180, always_toggle_chip_select: bool = False,
//|                  grayscale: bool = False) -> None:
//|         """Create a EPaperDisplay object on the given display bus (`displayio.FourWire` or `displayio.ParallelBus`).
//|
//|         The ``start_sequence`` and ``stop_sequence`` are bitpacked to minimize the ram impact. Every
//|         command begins with a command byte followed by a byte to determine the parameter count and
//|         delay. When the top bit of the second byte is 1 (0x80), a delay will occur after the command
//|         parameters are sent. The remaining 7 bits are the parameter count excluding any delay
//|         byte. The bytes following are the parameters. When the delay bit is set, a single byte after
//|         the parameters specifies the delay duration in milliseconds. The value 0xff will lead to an
//|         extra long 500 ms delay instead of 255 ms. The next byte will begin a new command definition.
//|
//|         :param display_bus: The bus that the display is connected to
//|         :type _DisplayBus: displayio.FourWire or displayio.ParallelBus
//|         :param ~_typing.ReadableBuffer start_sequence: Byte-packed initialization sequence.
//|         :param ~_typing.ReadableBuffer stop_sequence: Byte-packed initialization sequence.
//|         :param int width: Width in pixels
//|         :param int height: Height in pixels
//|         :param int ram_width: RAM width in pixels
//|         :param int ram_height: RAM height in pixels
//|         :param int colstart: The index if the first visible column
//|         :param int rowstart: The index if the first visible row
//|         :param int rotation: The rotation of the display in degrees clockwise. Must be in 90 degree increments (0, 90, 180, 270)
//|         :param int set_column_window_command: Command used to set the start and end columns to update
//|         :param int set_row_window_command: Command used so set the start and end rows to update
//|         :param int set_current_column_command: Command used to set the current column location
//|         :param int set_current_row_command: Command used to set the current row location
//|         :param int write_black_ram_command: Command used to write pixels values into the update region
//|         :param bool black_bits_inverted: True if 0 bits are used to show black pixels. Otherwise, 1 means to show black.
//|         :param int write_color_ram_command: Command used to write pixels values into the update region
//|         :param bool color_bits_inverted: True if 0 bits are used to show the color. Otherwise, 1 means to show color.
//|         :param int highlight_color: RGB888 of source color to highlight with third ePaper color.
//|         :param int refresh_display_command: Command used to start a display refresh
//|         :param float refresh_time: Time it takes to refresh the display before the stop_sequence should be sent. Ignored when busy_pin is provided.
//|         :param microcontroller.Pin busy_pin: Pin used to signify the display is busy
//|         :param bool busy_state: State of the busy pin when the display is busy
//|         :param float seconds_per_frame: Minimum number of seconds between screen refreshes
//|         :param bool always_toggle_chip_select: When True, chip select is toggled every byte
//|         :param bool grayscale: When true, the color ram is the low bit of 2-bit grayscale"""
//|         ...
//|
STATIC mp_obj_t displayio_epaperdisplay_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display_bus, ARG_start_sequence, ARG_stop_sequence, ARG_width, ARG_height,
           ARG_ram_width, ARG_ram_height, ARG_colstart, ARG_rowstart, ARG_rotation,
           ARG_set_column_window_command, ARG_set_row_window_command, ARG_set_current_column_command,
           ARG_set_current_row_command, ARG_write_black_ram_command, ARG_black_bits_inverted,
           ARG_write_color_ram_command, ARG_color_bits_inverted, ARG_highlight_color,
           ARG_refresh_display_command,  ARG_refresh_time, ARG_busy_pin, ARG_busy_state,
           ARG_seconds_per_frame, ARG_always_toggle_chip_select, ARG_grayscale };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_stop_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_ram_width, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_ram_height, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_colstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rowstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rotation, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_set_column_window_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = NO_COMMAND} },
        { MP_QSTR_set_row_window_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = NO_COMMAND} },
        { MP_QSTR_set_current_column_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = NO_COMMAND} },
        { MP_QSTR_set_current_row_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = NO_COMMAND} },
        { MP_QSTR_write_black_ram_command, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_black_bits_inverted, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_write_color_ram_command, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_color_bits_inverted, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_highlight_color, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x000000} },
        { MP_QSTR_refresh_display_command, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_refresh_time, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(40)} },
        { MP_QSTR_busy_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_busy_state, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_seconds_per_frame, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(180)} },
        { MP_QSTR_always_toggle_chip_select, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_grayscale, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t display_bus = args[ARG_display_bus].u_obj;

    mp_buffer_info_t start_bufinfo;
    mp_get_buffer_raise(args[ARG_start_sequence].u_obj, &start_bufinfo, MP_BUFFER_READ);
    mp_buffer_info_t stop_bufinfo;
    mp_get_buffer_raise(args[ARG_stop_sequence].u_obj, &stop_bufinfo, MP_BUFFER_READ);


    const mcu_pin_obj_t *busy_pin = validate_obj_is_free_pin_or_none(args[ARG_busy_pin].u_obj);

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(translate("Display rotation must be in 90 degree increments"));
    }

    primary_display_t *disp = allocate_display_or_raise();
    displayio_epaperdisplay_obj_t *self = &disp->epaper_display;
    ;

    mp_float_t refresh_time = mp_obj_get_float(args[ARG_refresh_time].u_obj);
    mp_float_t seconds_per_frame = mp_obj_get_float(args[ARG_seconds_per_frame].u_obj);

    mp_int_t write_color_ram_command = NO_COMMAND;
    mp_int_t highlight_color = args[ARG_highlight_color].u_int;
    if (args[ARG_write_color_ram_command].u_obj != mp_const_none) {
        write_color_ram_command = mp_obj_get_int(args[ARG_write_color_ram_command].u_obj);
    }

    self->base.type = &displayio_epaperdisplay_type;
    common_hal_displayio_epaperdisplay_construct(
        self,
        display_bus,
        start_bufinfo.buf, start_bufinfo.len, stop_bufinfo.buf, stop_bufinfo.len,
        args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_ram_width].u_int, args[ARG_ram_height].u_int,
        args[ARG_colstart].u_int, args[ARG_rowstart].u_int, rotation,
        args[ARG_set_column_window_command].u_int, args[ARG_set_row_window_command].u_int,
        args[ARG_set_current_column_command].u_int, args[ARG_set_current_row_command].u_int,
        args[ARG_write_black_ram_command].u_int, args[ARG_black_bits_inverted].u_bool, write_color_ram_command,
        args[ARG_color_bits_inverted].u_bool, highlight_color, args[ARG_refresh_display_command].u_int, refresh_time,
        busy_pin, args[ARG_busy_state].u_bool, seconds_per_frame,
        args[ARG_always_toggle_chip_select].u_bool, args[ARG_grayscale].u_bool
        );

    return self;
}

// Helper to ensure we have the native super class instead of a subclass.
static displayio_epaperdisplay_obj_t *native_display(mp_obj_t display_obj) {
    mp_obj_t native_display = mp_obj_cast_to_native_base(display_obj, &displayio_epaperdisplay_type);
    mp_obj_assert_native_inited(native_display);
    return MP_OBJ_TO_PTR(native_display);
}

//|     def show(self, group: Group) -> None:
//|         """Switches to displaying the given group of layers. When group is None, the default
//|         CircuitPython terminal will be shown.
//|
//|         :param Group group: The group to show."""
//|         ...
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    displayio_group_t *group = NULL;
    if (group_in != mp_const_none) {
        group = MP_OBJ_TO_PTR(native_group(group_in));
    }

    bool ok = common_hal_displayio_epaperdisplay_show(self, group);
    if (!ok) {
        mp_raise_ValueError(translate("Group already used"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_epaperdisplay_show_obj, displayio_epaperdisplay_obj_show);

//|     def refresh(self) -> None:
//|         """Refreshes the display immediately or raises an exception if too soon. Use
//|         ``time.sleep(display.time_to_refresh)`` to sleep until a refresh can occur."""
//|         ...
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_refresh(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    bool ok = common_hal_displayio_epaperdisplay_refresh(self);
    if (!ok) {
        mp_raise_RuntimeError(translate("Refresh too soon"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_refresh_obj, displayio_epaperdisplay_obj_refresh);

//|     time_to_refresh: float
//|     """Time, in fractional seconds, until the ePaper display can be refreshed."""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_time_to_refresh(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return mp_obj_new_float(common_hal_displayio_epaperdisplay_get_time_to_refresh(self) / 1000.0);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_time_to_refresh_obj, displayio_epaperdisplay_obj_get_time_to_refresh);

const mp_obj_property_t displayio_epaperdisplay_time_to_refresh_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_time_to_refresh_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     busy: bool
//|     """True when the display is refreshing. This uses the ``busy_pin`` when available or the
//|        ``refresh_time`` otherwise."""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_busy(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return mp_obj_new_bool(common_hal_displayio_epaperdisplay_get_busy(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_busy_obj, displayio_epaperdisplay_obj_get_busy);

const mp_obj_property_t displayio_epaperdisplay_busy_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_busy_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     width: int
//|     """Gets the width of the display in pixels"""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_width(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_width_obj, displayio_epaperdisplay_obj_get_width);

const mp_obj_property_t displayio_epaperdisplay_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_width_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     height: int
//|     """Gets the height of the display in pixels"""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_height(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_height_obj, displayio_epaperdisplay_obj_get_height);

const mp_obj_property_t displayio_epaperdisplay_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_height_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     rotation: int
//|     """The rotation of the display as an int in degrees."""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_rotation(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_get_rotation(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_rotation_obj, displayio_epaperdisplay_obj_get_rotation);
STATIC mp_obj_t displayio_epaperdisplay_obj_set_rotation(mp_obj_t self_in, mp_obj_t value) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    common_hal_displayio_epaperdisplay_set_rotation(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_epaperdisplay_set_rotation_obj, displayio_epaperdisplay_obj_set_rotation);


const mp_obj_property_t displayio_epaperdisplay_rotation_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_rotation_obj,
              (mp_obj_t)&displayio_epaperdisplay_set_rotation_obj,
              MP_ROM_NONE},
};

//|     bus: _DisplayBus
//|     """The bus being used by the display"""
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_bus(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return common_hal_displayio_epaperdisplay_get_bus(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_bus_obj, displayio_epaperdisplay_obj_get_bus);

const mp_obj_property_t displayio_epaperdisplay_bus_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_bus_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};


STATIC const mp_rom_map_elem_t displayio_epaperdisplay_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_epaperdisplay_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&displayio_epaperdisplay_refresh_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_epaperdisplay_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_epaperdisplay_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&displayio_epaperdisplay_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_bus), MP_ROM_PTR(&displayio_epaperdisplay_bus_obj) },
    { MP_ROM_QSTR(MP_QSTR_busy), MP_ROM_PTR(&displayio_epaperdisplay_busy_obj) },
    { MP_ROM_QSTR(MP_QSTR_time_to_refresh), MP_ROM_PTR(&displayio_epaperdisplay_time_to_refresh_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_epaperdisplay_locals_dict, displayio_epaperdisplay_locals_dict_table);

const mp_obj_type_t displayio_epaperdisplay_type = {
    { &mp_type_type },
    .name = MP_QSTR_EPaperDisplay,
    .make_new = displayio_epaperdisplay_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_epaperdisplay_locals_dict,
};
