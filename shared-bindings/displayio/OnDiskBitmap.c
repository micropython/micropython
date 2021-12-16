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

#include "shared-bindings/displayio/OnDiskBitmap.h"

#include <stdint.h>

#include "py/runtime.h"
#include "py/objproperty.h"
#include "supervisor/shared/translate.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"

//| class OnDiskBitmap:
//|     """Loads values straight from disk. This minimizes memory use but can lead to
//|     much slower pixel load times. These load times may result in frame tearing where only part of
//|     the image is visible.
//|
//|     It's easiest to use on a board with a built in display such as the `Hallowing M0 Express
//|     <https://www.adafruit.com/product/3900>`_.
//|
//|     .. code-block:: Python
//|
//|       import board
//|       import displayio
//|       import time
//|       import pulseio
//|
//|       board.DISPLAY.auto_brightness = False
//|       board.DISPLAY.brightness = 0
//|       splash = displayio.Group()
//|       board.DISPLAY.show(splash)
//|
//|       odb = displayio.OnDiskBitmap('/sample.bmp')
//|       face = displayio.TileGrid(odb, pixel_shader=odb.pixel_shader)
//|       splash.append(face)
//|       # Wait for the image to load.
//|       board.DISPLAY.refresh(target_frames_per_second=60)
//|
//|       # Fade up the backlight
//|       for i in range(100):
//|           board.DISPLAY.brightness = 0.01 * i
//|           time.sleep(0.05)
//|
//|       # Wait forever
//|       while True:
//|           pass"""
//|
//|     def __init__(self, file: Union[str,typing.BinaryIO]) -> None:
//|         """Create an OnDiskBitmap object with the given file.
//|
//|         :param file file: The name of the bitmap file.  For backwards compatibility, a file opened in binary mode may also be passed.
//|
//|         Older versions of CircuitPython required a file opened in binary
//|         mode. CircuitPython 7.0 modified OnDiskBitmap so that it takes a
//|         filename instead, and opens the file internally.  A future version
//|         of CircuitPython will remove the ability to pass in an opened file.
//|         """
//|         ...
//|
STATIC mp_obj_t displayio_ondiskbitmap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_t arg = all_args[0];

    if (mp_obj_is_str(arg)) {
        arg = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), arg, MP_ROM_QSTR(MP_QSTR_rb));
    }
    if (!mp_obj_is_type(arg, &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }

    displayio_ondiskbitmap_t *self = m_new_obj(displayio_ondiskbitmap_t);
    self->base.type = &displayio_ondiskbitmap_type;
    common_hal_displayio_ondiskbitmap_construct(self, MP_OBJ_TO_PTR(arg));

    return MP_OBJ_FROM_PTR(self);
}

//|     width: int
//|     """Width of the bitmap. (read only)"""
//|
STATIC mp_obj_t displayio_ondiskbitmap_obj_get_width(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskbitmap_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_width_obj, displayio_ondiskbitmap_obj_get_width);

const mp_obj_property_t displayio_ondiskbitmap_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_ondiskbitmap_get_width_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},

};

//|     height: int
//|     """Height of the bitmap. (read only)"""
//|
STATIC mp_obj_t displayio_ondiskbitmap_obj_get_height(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskbitmap_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_height_obj, displayio_ondiskbitmap_obj_get_height);

const mp_obj_property_t displayio_ondiskbitmap_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_ondiskbitmap_get_height_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},

};

//|     pixel_shader: Union[ColorConverter, Palette]
//|     """The image's pixel_shader.  The type depends on the underlying
//|     bitmap's structure.  The pixel shader can be modified (e.g., to set the
//|     transparent pixel or, for palette shaded images, to update the palette.)"""
//|
STATIC mp_obj_t displayio_ondiskbitmap_obj_get_pixel_shader(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_displayio_ondiskbitmap_get_pixel_shader(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_pixel_shader_obj, displayio_ondiskbitmap_obj_get_pixel_shader);

const mp_obj_property_t displayio_ondiskbitmap_pixel_shader_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&displayio_ondiskbitmap_get_pixel_shader_obj,
              (mp_obj_t)MP_ROM_NONE,
              (mp_obj_t)MP_ROM_NONE},
};


STATIC const mp_rom_map_elem_t displayio_ondiskbitmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_ondiskbitmap_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&displayio_ondiskbitmap_pixel_shader_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_ondiskbitmap_width_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_ondiskbitmap_locals_dict, displayio_ondiskbitmap_locals_dict_table);

const mp_obj_type_t displayio_ondiskbitmap_type = {
    { &mp_type_type },
    .name = MP_QSTR_OnDiskBitmap,
    .make_new = displayio_ondiskbitmap_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_ondiskbitmap_locals_dict,
};
