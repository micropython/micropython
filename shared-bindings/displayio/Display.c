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

#include "shared-bindings/displayio/Display.h"

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

//| _DisplayBus = Union['FourWire', 'ParallelBus', 'I2CDisplay']
//| """:py:class:`FourWire`, :py:class:`ParallelBus` or :py:class:`I2CDisplay`"""
//|

//|
//| class Display:
//|     """Manage updating a display over a display bus
//|
//|     This initializes a display and connects it into CircuitPython. Unlike other
//|     objects in CircuitPython, Display objects live until `displayio.release_displays()`
//|     is called. This is done so that CircuitPython can use the display itself.
//|
//|     Most people should not use this class directly. Use a specific display driver instead that will
//|     contain the initialization sequence at minimum."""
//|
//|     def __init__(self, display_bus: _DisplayBus, init_sequence: ReadableBuffer, *, width: int, height: int, colstart: int = 0, rowstart: int = 0, rotation: int = 0, color_depth: int = 16, grayscale: bool = False, pixels_in_byte_share_row: bool = True, bytes_per_cell: int = 1, reverse_pixels_in_byte: bool = False, set_column_command: int = 0x2a, set_row_command: int = 0x2b, write_ram_command: int = 0x2c, set_vertical_scroll: int = 0, backlight_pin: Optional[microcontroller.Pin] = None, brightness_command: Optional[int] = None, brightness: float = 1.0, auto_brightness: bool = False, single_byte_bounds: bool = False, data_as_commands: bool = False,  auto_refresh: bool = True, native_frames_per_second: int = 60, backlight_on_high: bool = True, SH1107_addressing: bool = False) -> None:
//|         r"""Create a Display object on the given display bus (`FourWire`, `ParallelBus` or `I2CDisplay`).
//|
//|         The ``init_sequence`` is bitpacked to minimize the ram impact. Every command begins with a
//|         command byte followed by a byte to determine the parameter count and delay. When the top bit
//|         of the second byte is 1 (0x80), a delay will occur after the command parameters are sent.
//|         The remaining 7 bits are the parameter count excluding any delay byte. The bytes following
//|         are the parameters. When the delay bit is set, a single byte after the parameters specifies
//|         the delay duration in milliseconds. The value 0xff will lead to an extra long 500 ms delay
//|         instead of 255 ms. The next byte will begin a new command definition.
//|         Here is an example:
//|
//|         .. code-block:: python
//|
//|           init_sequence = (b"\xe1\x0f\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F" # Set Gamma
//|                            b"\x11\x80\x78"# Exit Sleep then delay 0x78 (120ms)
//|                            b"\x29\x81\xaa\x78"# Display on then delay 0x78 (120ms)
//|                           )
//|            display = displayio.Display(display_bus, init_sequence, width=320, height=240)
//|
//|         The first command is 0xe1 with 15 (0xf) parameters following. The second is 0x11 with 0
//|         parameters and a 120ms (0x78) delay. The third command is 0x29 with one parameter 0xaa and a
//|         120ms delay (0x78). Multiple byte literals (b"") are merged together on load. The parens
//|         are needed to allow byte literals on subsequent lines.
//|
//|         The initialization sequence should always leave the display memory access inline with the scan
//|         of the display to minimize tearing artifacts.
//|
//|         :param display_bus: The bus that the display is connected to
//|         :type _DisplayBus: FourWire, ParallelBus or I2CDisplay
//|         :param ~_typing.ReadableBuffer init_sequence: Byte-packed initialization sequence.
//|         :param int width: Width in pixels
//|         :param int height: Height in pixels
//|         :param int colstart: The index if the first visible column
//|         :param int rowstart: The index if the first visible row
//|         :param int rotation: The rotation of the display in degrees clockwise. Must be in 90 degree increments (0, 90, 180, 270)
//|         :param int color_depth: The number of bits of color per pixel transmitted. (Some displays
//|             support 18 bit but 16 is easier to transmit. The last bit is extrapolated.)
//|         :param bool grayscale: True if the display only shows a single color.
//|         :param bool pixels_in_byte_share_row: True when pixels are less than a byte and a byte includes pixels from the same row of the display. When False, pixels share a column.
//|         :param int bytes_per_cell: Number of bytes per addressable memory location when color_depth < 8. When greater than one, bytes share a row or column according to pixels_in_byte_share_row.
//|         :param bool reverse_pixels_in_byte: Reverses the pixel order within each byte when color_depth < 8. Does not apply across multiple bytes even if there is more than one byte per cell (bytes_per_cell.)
//|         :param bool reverse_bytes_in_word: Reverses the order of bytes within a word when color_depth == 16
//|         :param int set_column_command: Command used to set the start and end columns to update
//|         :param int set_row_command: Command used so set the start and end rows to update
//|         :param int write_ram_command: Command used to write pixels values into the update region. Ignored if data_as_commands is set.
//|         :param int set_vertical_scroll: Command used to set the first row to show
//|         :param microcontroller.Pin backlight_pin: Pin connected to the display's backlight
//|         :param int brightness_command: Command to set display brightness. Usually available in OLED controllers.
//|         :param float brightness: Initial display brightness. This value is ignored if auto_brightness is True.
//|         :param bool auto_brightness: If True, brightness is controlled via an ambient light sensor or other mechanism.
//|         :param bool single_byte_bounds: Display column and row commands use single bytes
//|         :param bool data_as_commands: Treat all init and boundary data as SPI commands. Certain displays require this.
//|         :param bool auto_refresh: Automatically refresh the screen
//|         :param int native_frames_per_second: Number of display refreshes per second that occur with the given init_sequence.
//|         :param bool backlight_on_high: If True, pulling the backlight pin high turns the backlight on.
//|         :param bool SH1107_addressing: Special quirk for SH1107, use upper/lower column set and page set
//|         """
//|         ...
//|
STATIC mp_obj_t displayio_display_make_new(const mp_obj_type_t *type, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display_bus, ARG_init_sequence, ARG_width, ARG_height, ARG_colstart, ARG_rowstart,
           ARG_rotation, ARG_color_depth, ARG_grayscale, ARG_pixels_in_byte_share_row,
           ARG_bytes_per_cell, ARG_reverse_pixels_in_byte, ARG_reverse_bytes_in_word,
           ARG_set_column_command, ARG_set_row_command, ARG_write_ram_command,
           ARG_set_vertical_scroll, ARG_backlight_pin, ARG_brightness_command,
           ARG_brightness, ARG_auto_brightness, ARG_single_byte_bounds, ARG_data_as_commands,
           ARG_auto_refresh, ARG_native_frames_per_second, ARG_backlight_on_high,
           ARG_SH1107_addressing };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_init_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_colstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rowstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rotation, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_depth, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_grayscale, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_pixels_in_byte_share_row, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_bytes_per_cell, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
        { MP_QSTR_reverse_pixels_in_byte, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_reverse_bytes_in_word, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_set_column_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2a} },
        { MP_QSTR_set_row_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2b} },
        { MP_QSTR_write_ram_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2c} },
        { MP_QSTR_set_vertical_scroll, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x0} },
        { MP_QSTR_backlight_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_brightness_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = NO_BRIGHTNESS_COMMAND} },
        { MP_QSTR_brightness, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_auto_brightness, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_single_byte_bounds, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_data_as_commands, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_auto_refresh, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_native_frames_per_second, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 60} },
        { MP_QSTR_backlight_on_high, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_SH1107_addressing, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t display_bus = args[ARG_display_bus].u_obj;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_init_sequence].u_obj, &bufinfo, MP_BUFFER_READ);

    const mcu_pin_obj_t *backlight_pin = validate_obj_is_free_pin_or_none(args[ARG_backlight_pin].u_obj);

    mp_float_t brightness = mp_obj_get_float(args[ARG_brightness].u_obj);

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(translate("Display rotation must be in 90 degree increments"));
    }

    primary_display_t *disp = allocate_display_or_raise();
    displayio_display_obj_t *self = &disp->display;
    ;
    self->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(
        self,
        display_bus, args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_colstart].u_int, args[ARG_rowstart].u_int, rotation,
        args[ARG_color_depth].u_int, args[ARG_grayscale].u_bool,
        args[ARG_pixels_in_byte_share_row].u_bool,
        args[ARG_bytes_per_cell].u_bool,
        args[ARG_reverse_pixels_in_byte].u_bool,
        args[ARG_reverse_bytes_in_word].u_bool,
        args[ARG_set_column_command].u_int, args[ARG_set_row_command].u_int,
        args[ARG_write_ram_command].u_int,
        args[ARG_set_vertical_scroll].u_int,
        bufinfo.buf, bufinfo.len,
        MP_OBJ_TO_PTR(backlight_pin),
        args[ARG_brightness_command].u_int,
        brightness,
        args[ARG_auto_brightness].u_bool,
        args[ARG_single_byte_bounds].u_bool,
        args[ARG_data_as_commands].u_bool,
        args[ARG_auto_refresh].u_bool,
        args[ARG_native_frames_per_second].u_int,
        args[ARG_backlight_on_high].u_bool,
        args[ARG_SH1107_addressing].u_bool
        );

    return self;
}

// Helper to ensure we have the native super class instead of a subclass.
static displayio_display_obj_t *native_display(mp_obj_t display_obj) {
    mp_obj_t native_display = mp_obj_cast_to_native_base(display_obj, &displayio_display_type);
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
STATIC mp_obj_t displayio_display_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_display_obj_t *self = native_display(self_in);
    displayio_group_t *group = NULL;
    if (group_in != mp_const_none) {
        group = MP_OBJ_TO_PTR(native_group(group_in));
    }

    bool ok = common_hal_displayio_display_show(self, group);
    if (!ok) {
        mp_raise_ValueError(translate("Group already used"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_show_obj, displayio_display_obj_show);

//|     def refresh(self, *, target_frames_per_second: Optional[int] = None, minimum_frames_per_second: int = 1) -> bool:
//|         """When auto refresh is off, waits for the target frame rate and then refreshes the display,
//|         returning True. If the call has taken too long since the last refresh call for the given
//|         target frame rate, then the refresh returns False immediately without updating the screen to
//|         hopefully help getting caught up.
//|
//|         If the time since the last successful refresh is below the minimum frame rate, then an
//|         exception will be raised. Set ``minimum_frames_per_second`` to 0 to disable.
//|
//|         When auto refresh is off, ``display.refresh()`` or ``display.refresh(target_frames_per_second=None)``
//|         will update the display immediately.
//|
//|         When auto refresh is on, updates the display immediately. (The display will also update
//|         without calls to this.)
//|
//|         :param int target_frames_per_second: How many times a second `refresh` should be called and the screen updated.
//|             Set to `None` for immediate refresh.
//|         :param int minimum_frames_per_second: The minimum number of times the screen should be updated per second."""
//|         ...
//|
STATIC mp_obj_t displayio_display_obj_refresh(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_target_frames_per_second, ARG_minimum_frames_per_second };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_target_frames_per_second, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_minimum_frames_per_second, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    displayio_display_obj_t *self = native_display(pos_args[0]);
    uint32_t maximum_ms_per_real_frame = 0xffffffff;
    mp_int_t minimum_frames_per_second = args[ARG_minimum_frames_per_second].u_int;
    if (minimum_frames_per_second > 0) {
        maximum_ms_per_real_frame = 1000 / minimum_frames_per_second;
    }

    uint32_t target_ms_per_frame;
    if (args[ARG_target_frames_per_second].u_obj == mp_const_none) {
        target_ms_per_frame = 0xffffffff;
    } else {
        target_ms_per_frame = 1000 / mp_obj_get_int(args[ARG_target_frames_per_second].u_obj);
    }

    return mp_obj_new_bool(common_hal_displayio_display_refresh(self, target_ms_per_frame, maximum_ms_per_real_frame));
}

MP_DEFINE_CONST_FUN_OBJ_KW(displayio_display_refresh_obj, 1, displayio_display_obj_refresh);

//|     auto_refresh: bool
//|     """True when the display is refreshed automatically."""
//|
STATIC mp_obj_t displayio_display_obj_get_auto_refresh(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return mp_obj_new_bool(common_hal_displayio_display_get_auto_refresh(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_auto_refresh_obj, displayio_display_obj_get_auto_refresh);

STATIC mp_obj_t displayio_display_obj_set_auto_refresh(mp_obj_t self_in, mp_obj_t auto_refresh) {
    displayio_display_obj_t *self = native_display(self_in);

    common_hal_displayio_display_set_auto_refresh(self, mp_obj_is_true(auto_refresh));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_set_auto_refresh_obj, displayio_display_obj_set_auto_refresh);

const mp_obj_property_t displayio_display_auto_refresh_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_auto_refresh_obj,
              (mp_obj_t)&displayio_display_set_auto_refresh_obj,
              MP_ROM_NONE},
};

//|     brightness: float
//|     """The brightness of the display as a float. 0.0 is off and 1.0 is full brightness. When
//|     `auto_brightness` is True, the value of `brightness` will change automatically.
//|     If `brightness` is set, `auto_brightness` will be disabled and will be set to False."""
//|
STATIC mp_obj_t displayio_display_obj_get_brightness(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    mp_float_t brightness = common_hal_displayio_display_get_brightness(self);
    if (brightness < 0) {
        mp_raise_RuntimeError(translate("Brightness not adjustable"));
    }
    return mp_obj_new_float(brightness);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_brightness_obj, displayio_display_obj_get_brightness);

STATIC mp_obj_t displayio_display_obj_set_brightness(mp_obj_t self_in, mp_obj_t brightness_obj) {
    displayio_display_obj_t *self = native_display(self_in);
    common_hal_displayio_display_set_auto_brightness(self, false);
    mp_float_t brightness = mp_obj_get_float(brightness_obj);
    if (brightness < 0 || brightness > 1.0) {
        mp_raise_ValueError(translate("Brightness must be 0-1.0"));
    }
    bool ok = common_hal_displayio_display_set_brightness(self, brightness);
    if (!ok) {
        mp_raise_RuntimeError(translate("Brightness not adjustable"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_set_brightness_obj, displayio_display_obj_set_brightness);

const mp_obj_property_t displayio_display_brightness_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_brightness_obj,
              (mp_obj_t)&displayio_display_set_brightness_obj,
              MP_ROM_NONE},
};

//|     auto_brightness: bool
//|     """True when the display brightness is adjusted automatically, based on an ambient
//|     light sensor or other method. Note that some displays may have this set to True by default,
//|     but not actually implement automatic brightness adjustment. `auto_brightness` is set to False
//|     if `brightness` is set manually."""
//|
STATIC mp_obj_t displayio_display_obj_get_auto_brightness(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return mp_obj_new_bool(common_hal_displayio_display_get_auto_brightness(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_auto_brightness_obj, displayio_display_obj_get_auto_brightness);

STATIC mp_obj_t displayio_display_obj_set_auto_brightness(mp_obj_t self_in, mp_obj_t auto_brightness) {
    displayio_display_obj_t *self = native_display(self_in);

    common_hal_displayio_display_set_auto_brightness(self, mp_obj_is_true(auto_brightness));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_set_auto_brightness_obj, displayio_display_obj_set_auto_brightness);

const mp_obj_property_t displayio_display_auto_brightness_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_auto_brightness_obj,
              (mp_obj_t)&displayio_display_set_auto_brightness_obj,
              MP_ROM_NONE},
};




//|     width: int
//|     """Gets the width of the board"""
//|
STATIC mp_obj_t displayio_display_obj_get_width(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_width_obj, displayio_display_obj_get_width);

const mp_obj_property_t displayio_display_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_width_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     height: int
//|     """Gets the height of the board"""
//|
STATIC mp_obj_t displayio_display_obj_get_height(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_height_obj, displayio_display_obj_get_height);

const mp_obj_property_t displayio_display_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_height_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     rotation: int
//|     """The rotation of the display as an int in degrees."""
//|
STATIC mp_obj_t displayio_display_obj_get_rotation(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_get_rotation(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_rotation_obj, displayio_display_obj_get_rotation);
STATIC mp_obj_t displayio_display_obj_set_rotation(mp_obj_t self_in, mp_obj_t value) {
    displayio_display_obj_t *self = native_display(self_in);
    common_hal_displayio_display_set_rotation(self, mp_obj_get_int(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_set_rotation_obj, displayio_display_obj_set_rotation);


const mp_obj_property_t displayio_display_rotation_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_rotation_obj,
              (mp_obj_t)&displayio_display_set_rotation_obj,
              MP_ROM_NONE},
};

//|     bus: _DisplayBus
//|     """The bus being used by the display"""
//|
//|
STATIC mp_obj_t displayio_display_obj_get_bus(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return common_hal_displayio_display_get_bus(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_bus_obj, displayio_display_obj_get_bus);

const mp_obj_property_t displayio_display_bus_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_bus_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};


//|     def fill_row(self, y: int, buffer: WriteableBuffer) -> WriteableBuffer:
//|         """Extract the pixels from a single row
//|
//|         :param int y: The top edge of the area
//|         :param ~_typing.WriteableBuffer buffer: The buffer in which to place the pixel data"""
//|         ...
//|
STATIC mp_obj_t displayio_display_obj_fill_row(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_y, ARG_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = -1} },
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    displayio_display_obj_t *self = native_display(pos_args[0]);
    mp_int_t y = args[ARG_y].u_int;
    mp_obj_t *result = args[ARG_buffer].u_obj;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(result, &bufinfo, MP_BUFFER_WRITE);

    if (bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError(translate("Buffer is not a bytearray."));
    }
    if (self->core.colorspace.depth != 16) {
        mp_raise_ValueError(translate("Display must have a 16 bit colorspace."));
    }

    displayio_area_t area = {
        .x1 = 0,
        .y1 = y,
        .x2 = self->core.width,
        .y2 = y + 1
    };
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->core.colorspace.depth;
    uint16_t buffer_size = self->core.width / pixels_per_word;
    uint16_t pixels_per_buffer = displayio_area_size(&area);
    if (pixels_per_buffer % pixels_per_word) {
        buffer_size += 1;
    }

    uint32_t *result_buffer = bufinfo.buf;
    size_t result_buffer_size = bufinfo.len;

    if (result_buffer_size >= (buffer_size * 4)) {
        volatile uint32_t mask_length = (pixels_per_buffer / 32) + 1;
        uint32_t mask[mask_length];

        for (uint16_t k = 0; k < mask_length; k++) {
            mask[k] = 0x00000000;
        }

        displayio_display_core_fill_area(&self->core, &area, mask, result_buffer);
        return result;
    } else {
        mp_raise_ValueError(translate("Buffer is too small"));
    }
}
MP_DEFINE_CONST_FUN_OBJ_KW(displayio_display_fill_row_obj, 1, displayio_display_obj_fill_row);

STATIC const mp_rom_map_elem_t displayio_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&displayio_display_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_row), MP_ROM_PTR(&displayio_display_fill_row_obj) },

    { MP_ROM_QSTR(MP_QSTR_auto_refresh), MP_ROM_PTR(&displayio_display_auto_refresh_obj) },

    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&displayio_display_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_auto_brightness), MP_ROM_PTR(&displayio_display_auto_brightness_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_display_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_display_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&displayio_display_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_bus), MP_ROM_PTR(&displayio_display_bus_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_display_locals_dict, displayio_display_locals_dict_table);

const mp_obj_type_t displayio_display_type = {
    { &mp_type_type },
    .name = MP_QSTR_Display,
    .make_new = displayio_display_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_display_locals_dict,
};
