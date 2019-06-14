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

//| .. currentmodule:: displayio
//|
//| :class:`Display` -- Manage updating a display over a display bus
//| ==========================================================================
//|
//| This initializes a display and connects it into CircuitPython. Unlike other
//| objects in CircuitPython, Display objects live until `displayio.release_displays()`
//| is called. This is done so that CircuitPython can use the display itself.
//|
//| Most people should not use this class directly. Use a specific display driver instead that will
//| contain the initialization sequence at minimum.
//|
//| .. class:: Display(display_bus, init_sequence, *, width, height, colstart=0, rowstart=0, rotation=0, color_depth=16, set_column_command=0x2a, set_row_command=0x2b, write_ram_command=0x2c, set_vertical_scroll=0, backlight_pin=None, brightness=1.0, auto_brightness=False, single_byte_bounds=False, data_as_commands=False)
//|
//|   Create a Display object on the given display bus (`displayio.FourWire` or `displayio.ParallelBus`).
//|
//|   The ``init_sequence`` is bitpacked to minimize the ram impact. Every command begins with a
//|   command byte followed by a byte to determine the parameter count and if a delay is need after.
//|   When the top bit of the second byte is 1, the next byte will be the delay time in milliseconds.
//|   The remaining 7 bits are the parameter count excluding any delay byte. The third through final
//|   bytes are the remaining command parameters. The next byte will begin a new command definition.
//|   Here is a portion of ILI9341 init code:
//|
//|   .. code-block:: python
//|
//|     init_sequence = (b"\xe1\x0f\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F" # Set Gamma
//|                      b"\x11\x80\x78"# Exit Sleep then delay 0x78 (120ms)
//|                      b"\x29\x80\x78"# Display on then delay 0x78 (120ms)
//|                     )
//|      display = displayio.Display(display_bus, init_sequence, width=320, height=240)
//|
//|   The first command is 0xe1 with 15 (0xf) parameters following. The second and third are 0x11 and
//|   0x29 respectively with delays (0x80) of 120ms (0x78) and no parameters. Multiple byte literals
//|   (b"") are merged together on load. The parens are needed to allow byte literals on subsequent
//|   lines.
//|
//|   The initialization sequence should always leave the display memory access inline with the scan
//|   of the display to minimize tearing artifacts.
//|
//|   :param display_bus: The bus that the display is connected to
//|   :type display_bus: displayio.FourWire or displayio.ParallelBus
//|   :param buffer init_sequence: Byte-packed initialization sequence.
//|   :param int width: Width in pixels
//|   :param int height: Height in pixels
//|   :param int colstart: The index if the first visible column
//|   :param int rowstart: The index if the first visible row
//|   :param int rotation: The rotation of the display in degrees clockwise. Must be in 90 degree increments (0, 90, 180, 270)
//|   :param int color_depth: The number of bits of color per pixel transmitted. (Some displays
//|       support 18 bit but 16 is easier to transmit. The last bit is extrapolated.)
//|   :param int set_column_command: Command used to set the start and end columns to update
//|   :param int set_row_command: Command used so set the start and end rows to update
//|   :param int write_ram_command: Command used to write pixels values into the update region
//|   :param int set_vertical_scroll: Command used to set the first row to show
//|   :param microcontroller.Pin backlight_pin: Pin connected to the display's backlight
//|   :param bool brightness: Initial display brightness. This value is ignored if auto_brightness is True.
//|   :param bool auto_brightness: If True, brightness is controlled via an ambient light sensor or other mechanism.
//|   :param bool single_byte_bounds: Display column and row commands use single bytes
//|   :param bool data_as_commands: Treat all init and boundary data as SPI commands. Certain displays require this.
//|
STATIC mp_obj_t displayio_display_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display_bus, ARG_init_sequence, ARG_width, ARG_height, ARG_colstart, ARG_rowstart, ARG_rotation, ARG_color_depth, ARG_set_column_command, ARG_set_row_command, ARG_write_ram_command, ARG_set_vertical_scroll, ARG_backlight_pin, ARG_brightness, ARG_auto_brightness, ARG_single_byte_bounds, ARG_data_as_commands };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_init_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED, },
        { MP_QSTR_colstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rowstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rotation, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_depth, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_set_column_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2a} },
        { MP_QSTR_set_row_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2b} },
        { MP_QSTR_write_ram_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2c} },
        { MP_QSTR_set_vertical_scroll, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x0} },
        { MP_QSTR_backlight_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_brightness, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_auto_brightness, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_single_byte_bounds, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_data_as_commands, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t display_bus = args[ARG_display_bus].u_obj;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_init_sequence].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_obj_t backlight_pin_obj = args[ARG_backlight_pin].u_obj;
    assert_pin(backlight_pin_obj, true);
    const mcu_pin_obj_t* backlight_pin = NULL;
    if (backlight_pin_obj != NULL && backlight_pin_obj != mp_const_none) {
        backlight_pin = MP_OBJ_TO_PTR(backlight_pin_obj);
        assert_pin_free(backlight_pin);
    }

    mp_float_t brightness = mp_obj_get_float(args[ARG_brightness].u_obj);

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(translate("Display rotation must be in 90 degree increments"));
    }

    displayio_display_obj_t *self = NULL;
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL ||
            displays[i].display.base.type == &mp_type_NoneType) {
            self = &displays[i].display;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_RuntimeError(translate("Too many displays"));
    }
    self->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(
        self,
        display_bus, args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_colstart].u_int, args[ARG_rowstart].u_int, rotation,
        args[ARG_color_depth].u_int, args[ARG_set_column_command].u_int, args[ARG_set_row_command].u_int,
        args[ARG_write_ram_command].u_int,
        args[ARG_set_vertical_scroll].u_int,
        bufinfo.buf, bufinfo.len,
        MP_OBJ_TO_PTR(backlight_pin),
        brightness,
        args[ARG_auto_brightness].u_bool,
        args[ARG_single_byte_bounds].u_bool,
        args[ARG_data_as_commands].u_bool
        );

    return self;
}

// Helper to ensure we have the native super class instead of a subclass.
static displayio_display_obj_t* native_display(mp_obj_t display_obj) {
    mp_obj_t native_display = mp_instance_cast_to_native_base(display_obj, &displayio_display_type);
    mp_obj_assert_native_inited(native_display);
    return MP_OBJ_TO_PTR(native_display);
}

//|   .. method:: show(group)
//|
//|     Switches to displaying the given group of layers. When group is None, the default
//|     CircuitPython terminal will be shown.
//|
//|     :param Group group: The group to show.
STATIC mp_obj_t displayio_display_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_display_obj_t *self = native_display(self_in);
    displayio_group_t* group = NULL;
    if (group_in != mp_const_none) {
        group = MP_OBJ_TO_PTR(native_group(group_in));
    }

    common_hal_displayio_display_show(self, group);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_show_obj, displayio_display_obj_show);

//|   .. method:: refresh_soon()
//|
//|     Queues up a display refresh that happens in the background.
//|
STATIC mp_obj_t displayio_display_obj_refresh_soon(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    common_hal_displayio_display_refresh_soon(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_refresh_soon_obj, displayio_display_obj_refresh_soon);

//|   .. method:: wait_for_frame()
//|
//|     Waits until the next frame has been transmitted to the display unless the wait count is
//|     behind the rendered frames. In that case, this will return immediately with the wait count.
//|
STATIC mp_obj_t displayio_display_obj_wait_for_frame(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_wait_for_frame(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_wait_for_frame_obj, displayio_display_obj_wait_for_frame);

//|   .. attribute:: brightness
//|
//|     The brightness of the display as a float. 0.0 is off and 1.0 is full brightness. When
//|     `auto_brightness` is True, the value of `brightness` will change automatically.
//|     If `brightness` is set, `auto_brightness` will be disabled and will be set to False.
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

STATIC mp_obj_t displayio_display_obj_set_brightness(mp_obj_t self_in, mp_obj_t brightness) {
    displayio_display_obj_t *self = native_display(self_in);
    common_hal_displayio_display_set_auto_brightness(self, false);
    bool ok = common_hal_displayio_display_set_brightness(self, mp_obj_get_float(brightness));
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
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: auto_brightness
//|
//|     True when the display brightness is adjusted automatically, based on an ambient
//|     light sensor or other method. Note that some displays may have this set to True by default,
//|     but not actually implement automatic brightness adjustment. `auto_brightness` is set to False
//|     if `brightness` is set manually.
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
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: width
//|
//|	Gets the width of the board
//|
//|
STATIC mp_obj_t displayio_display_obj_get_width(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_width_obj, displayio_display_obj_get_width);

const mp_obj_property_t displayio_display_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_width_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: height
//|
//|	Gets the height of the board
//|
//|
STATIC mp_obj_t displayio_display_obj_get_height(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_height_obj, displayio_display_obj_get_height);

const mp_obj_property_t displayio_display_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_height_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: bus
//|
//|	The bus being used by the display
//|
//|
STATIC mp_obj_t displayio_display_obj_get_bus(mp_obj_t self_in) {
    displayio_display_obj_t *self = native_display(self_in);
    return self->bus;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_get_bus_obj, displayio_display_obj_get_bus);

const mp_obj_property_t displayio_display_bus_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_display_get_bus_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t displayio_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh_soon), MP_ROM_PTR(&displayio_display_refresh_soon_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_for_frame), MP_ROM_PTR(&displayio_display_wait_for_frame_obj) },

    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&displayio_display_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_auto_brightness), MP_ROM_PTR(&displayio_display_auto_brightness_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_display_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_display_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_bus), MP_ROM_PTR(&displayio_display_bus_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_display_locals_dict, displayio_display_locals_dict_table);

const mp_obj_type_t displayio_display_type = {
    { &mp_type_type },
    .name = MP_QSTR_Display,
    .make_new = displayio_display_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_display_locals_dict,
};
