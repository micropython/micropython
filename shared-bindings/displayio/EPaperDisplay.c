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

//| .. currentmodule:: displayio
//|
//| :class:`EPaperDisplay` -- Manage updating an epaper display over a display bus
//| ==========================================================================
//|
//| This initializes an epaper display and connects it into CircuitPython. Unlike other
//| objects in CircuitPython, EPaperDisplay objects live until `displayio.release_displays()`
//| is called. This is done so that CircuitPython can use the display itself.
//|
//| Most people should not use this class directly. Use a specific display driver instead that will
//| contain the startup and shutdown sequences at minimum.
//|
//| .. class:: EPaperDisplay(display_bus, start_sequence, stop_sequence, *, width, height, ram_width, ram_height, colstart=0, rowstart=0, rotation=0, set_column_window_command=None, set_row_window_command=None, single_byte_bounds=False, write_black_ram_command, black_bits_inverted=False, write_color_ram_command=None, color_bits_inverted=False, refresh_display_command, busy_pin=None, busy_state=True, seconds_per_frame=180, always_toggle_chip_select=False)
//|
//|   Create a EPaperDisplay object on the given display bus (`displayio.FourWire` or `displayio.ParallelBus`).
//|
//|   The ``start_sequence`` and ``stop_sequence`` bitpacked to minimize the ram impact. Every
//|   command begins with a command byte followed by a byte to determine the parameter count and if
//|   a delay is need after. When the top bit of the second byte is 1, the next byte will be the
//|   delay time in milliseconds. The remaining 7 bits are the parameter count excluding any delay
//|   byte. The third through final bytes are the remaining command parameters. The next byte will
//|   begin a new command definition.
//|
//|   :param display_bus: The bus that the display is connected to
//|   :type display_bus: displayio.FourWire or displayio.ParallelBus
//|   :param buffer start_sequence: Byte-packed initialization sequence.
//|   :param buffer stop_sequence: Byte-packed initialization sequence.
//|   :param int width: Width in pixels
//|   :param int height: Height in pixels
//|   :param int ram_width: RAM width in pixels
//|   :param int ram_height: RAM height in pixels
//|   :param int colstart: The index if the first visible column
//|   :param int rowstart: The index if the first visible row
//|   :param int rotation: The rotation of the display in degrees clockwise. Must be in 90 degree increments (0, 90, 180, 270)
//|   :param int set_column_window_command: Command used to set the start and end columns to update
//|   :param int set_row_window_command: Command used so set the start and end rows to update
//|   :param int set_current_column_command: Command used to set the current column location
//|   :param int set_current_row_command: Command used to set the current row location
//|   :param int write_black_ram_command: Command used to write pixels values into the update region
//|   :param bool black_bits_inverted: True if 0 bits are used to show black pixels. Otherwise, 1 means to show black.
//|   :param int write_color_ram_command: Command used to write pixels values into the update region
//|   :param bool color_bits_inverted: True if 0 bits are used to show the color. Otherwise, 1 means to show color.
//|   :param int third_color: Color of third ePaper color in RGB888.
//|   :param int refresh_display_command: Command used to start a display refresh
//|   :param microcontroller.Pin busy_pin: Pin used to signify the display is busy
//|   :param bool busy_state: State of the busy pin when the display is busy
//|   :param int seconds_per_frame: Minimum number of seconds between screen refreshes
//|   :param bool always_toggle_chip_select: When True, chip select is toggled every byte
//|
STATIC mp_obj_t displayio_epaperdisplay_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display_bus, ARG_start_sequence, ARG_stop_sequence, ARG_width, ARG_height, ARG_ram_width, ARG_ram_height, ARG_colstart, ARG_rowstart, ARG_rotation, ARG_set_column_window_command, ARG_set_row_window_command, ARG_set_current_column_command, ARG_set_current_row_command, ARG_write_black_ram_command, ARG_black_bits_inverted, ARG_write_color_ram_command, ARG_color_bits_inverted, ARG_third_color, ARG_refresh_display_command,  ARG_busy_pin, ARG_busy_state, ARG_seconds_per_frame, ARG_always_toggle_chip_select };
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
        { MP_QSTR_third_color, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x000000} },
        { MP_QSTR_refresh_display_command, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_busy_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_busy_state, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_seconds_per_frame, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NEW_SMALL_INT(180)} },
        { MP_QSTR_always_toggle_chip_select, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t display_bus = args[ARG_display_bus].u_obj;

    mp_buffer_info_t start_bufinfo;
    mp_get_buffer_raise(args[ARG_start_sequence].u_obj, &start_bufinfo, MP_BUFFER_READ);
    mp_buffer_info_t stop_bufinfo;
    mp_get_buffer_raise(args[ARG_stop_sequence].u_obj, &stop_bufinfo, MP_BUFFER_READ);


    mp_obj_t busy_pin_obj = args[ARG_busy_pin].u_obj;
    assert_pin(busy_pin_obj, true);
    const mcu_pin_obj_t* busy_pin = NULL;
    if (busy_pin_obj != NULL && busy_pin_obj != mp_const_none) {
        busy_pin = MP_OBJ_TO_PTR(busy_pin_obj);
        assert_pin_free(busy_pin);
    }

    mp_int_t rotation = args[ARG_rotation].u_int;
    if (rotation % 90 != 0) {
        mp_raise_ValueError(translate("Display rotation must be in 90 degree increments"));
    }

    displayio_epaperdisplay_obj_t *self = NULL;
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL ||
            displays[i].display.base.type == &mp_type_NoneType) {
            self = &displays[i].epaper_display;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_RuntimeError(translate("Too many displays"));
    }

    mp_float_t seconds_per_frame = mp_obj_get_float(args[ARG_seconds_per_frame].u_obj);

    mp_int_t write_color_ram_command = NO_COMMAND;
    mp_int_t third_color = args[ARG_third_color].u_int;
    if (args[ARG_write_color_ram_command].u_obj != mp_const_none) {
        write_color_ram_command = mp_obj_get_int(args[ARG_write_color_ram_command].u_obj);
    }

    self->base.type = &displayio_epaperdisplay_type;
    common_hal_displayio_epaperdisplay_construct(
        self,
        display_bus,
        start_bufinfo.buf, start_bufinfo.len, stop_bufinfo.buf, stop_bufinfo.len,
        args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_ram_width].u_int, args[ARG_ram_height].u_int, args[ARG_colstart].u_int, args[ARG_rowstart].u_int, rotation,
        args[ARG_set_column_window_command].u_int, args[ARG_set_row_window_command].u_int,
        args[ARG_set_current_column_command].u_int, args[ARG_set_current_row_command].u_int,
        args[ARG_write_black_ram_command].u_int, args[ARG_black_bits_inverted].u_bool, write_color_ram_command, args[ARG_color_bits_inverted].u_bool, third_color, args[ARG_refresh_display_command].u_int,
        busy_pin, args[ARG_busy_state].u_bool, seconds_per_frame, args[ARG_always_toggle_chip_select].u_bool
        );

    return self;
}

// Helper to ensure we have the native super class instead of a subclass.
static displayio_epaperdisplay_obj_t* native_display(mp_obj_t display_obj) {
    mp_obj_t native_display = mp_instance_cast_to_native_base(display_obj, &displayio_epaperdisplay_type);
    mp_obj_assert_native_inited(native_display);
    return MP_OBJ_TO_PTR(native_display);
}

//|   .. method:: show(group)
//|
//|     Switches to displaying the given group of layers. When group is None, the default
//|     CircuitPython terminal will be shown.
//|
//|     :param Group group: The group to show.
STATIC mp_obj_t displayio_epaperdisplay_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    displayio_group_t* group = NULL;
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

//|   .. method:: refresh_soon()
//|
//|     Queues up a display refresh that happens in the background.
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_refresh_soon(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    common_hal_displayio_epaperdisplay_refresh_soon(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_refresh_soon_obj, displayio_epaperdisplay_obj_refresh_soon);

//|   .. method:: wait_for_frame()
//|
//|     Waits until the next frame has been transmitted to the display unless the wait count is
//|     behind the rendered frames. In that case, this will return immediately with the wait count.
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_wait_for_frame(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_wait_for_frame(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_wait_for_frame_obj, displayio_epaperdisplay_obj_wait_for_frame);

//|   .. attribute:: width
//|
//|	Gets the width of the display in pixels
//|
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_width(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_width_obj, displayio_epaperdisplay_obj_get_width);

const mp_obj_property_t displayio_epaperdisplay_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_width_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: height
//|
//|	Gets the height of the display in pixels
//|
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_height(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_epaperdisplay_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_height_obj, displayio_epaperdisplay_obj_get_height);

const mp_obj_property_t displayio_epaperdisplay_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_height_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: bus
//|
//|	The bus being used by the display
//|
//|
STATIC mp_obj_t displayio_epaperdisplay_obj_get_bus(mp_obj_t self_in) {
    displayio_epaperdisplay_obj_t *self = native_display(self_in);
    return self->bus;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_epaperdisplay_get_bus_obj, displayio_epaperdisplay_obj_get_bus);

const mp_obj_property_t displayio_epaperdisplay_bus_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_epaperdisplay_get_bus_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t displayio_epaperdisplay_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_epaperdisplay_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh_soon), MP_ROM_PTR(&displayio_epaperdisplay_refresh_soon_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_for_frame), MP_ROM_PTR(&displayio_epaperdisplay_wait_for_frame_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_epaperdisplay_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_epaperdisplay_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_bus), MP_ROM_PTR(&displayio_epaperdisplay_bus_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_epaperdisplay_locals_dict, displayio_epaperdisplay_locals_dict_table);

const mp_obj_type_t displayio_epaperdisplay_type = {
    { &mp_type_type },
    .name = MP_QSTR_EPaperDisplay,
    .make_new = displayio_epaperdisplay_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_epaperdisplay_locals_dict,
};
