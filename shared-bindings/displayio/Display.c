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
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: Display(display_bus, *, width, height, colstart=0, rowstart=0, color_depth=16,
//|                    set_column_command=0x2a set_row_command=0x2b, write_ram_command=0x2c)
//|
//|   Create a Display object.
//|
STATIC mp_obj_t displayio_display_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_display_bus, ARG_init_sequence, ARG_width, ARG_height, ARG_colstart, ARG_rowstart, ARG_color_depth, ARG_set_column_command, ARG_set_row_command, ARG_write_ram_command };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_init_sequence, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
        { MP_QSTR_colstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_rowstart, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_depth, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_set_column_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2a} },
        { MP_QSTR_set_row_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2b} },
        { MP_QSTR_write_ram_command, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x2c} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t display_bus = args[ARG_display_bus].u_obj;

    mp_int_t width = args[ARG_width].u_int;
    mp_int_t height = args[ARG_height].u_int;
    if (width == -1 || height == -1) {
        mp_raise_ValueError(translate("Width and height kwargs required"));
    }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_init_sequence].u_obj, &bufinfo, MP_BUFFER_READ);

    displayio_display_obj_t *self = NULL;
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].display.base.type == NULL ||
            displays[i].display.base.type == &mp_type_NoneType) {
            self = &displays[i].display;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_RuntimeError(translate("Display limit reached"));
    }
    self->base.type = &displayio_display_type;
    common_hal_displayio_display_construct(self,
            display_bus, width, height, args[ARG_colstart].u_int, args[ARG_rowstart].u_int,
            args[ARG_color_depth].u_int, args[ARG_set_column_command].u_int, args[ARG_set_row_command].u_int,
            args[ARG_write_ram_command].u_int, bufinfo.buf, bufinfo.len);

    return self;
}

//|   .. method:: show(group)
//|
//|     Switches to displaying the given group of layers.
//|
STATIC mp_obj_t displayio_display_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t native_layer = mp_instance_cast_to_native_base(group_in, &displayio_group_type);
    if (native_layer == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("Must be a Group subclass."));
    }
    displayio_group_t* group = MP_OBJ_TO_PTR(native_layer);
    common_hal_displayio_display_show(self, group);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_display_show_obj, displayio_display_obj_show);

//|   .. method:: refresh_soon()
//|
//|     Queues up a display refresh that happens in the background.
//|
STATIC mp_obj_t displayio_display_obj_refresh_soon(mp_obj_t self_in) {
    displayio_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
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
    displayio_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_display_wait_for_frame(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_display_wait_for_frame_obj, displayio_display_obj_wait_for_frame);


STATIC const mp_rom_map_elem_t displayio_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_display_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh_soon), MP_ROM_PTR(&displayio_display_refresh_soon_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_for_frame), MP_ROM_PTR(&displayio_display_wait_for_frame_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_display_locals_dict, displayio_display_locals_dict_table);

const mp_obj_type_t displayio_display_type = {
    { &mp_type_type },
    .name = MP_QSTR_Display,
    .make_new = displayio_display_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_display_locals_dict,
};
