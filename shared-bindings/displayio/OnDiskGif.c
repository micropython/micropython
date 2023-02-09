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

#include "shared-bindings/displayio/OnDiskGif.h"

#include <stdint.h>

#include "py/runtime.h"
#include "py/objproperty.h"
#include "supervisor/shared/translate/translate.h"
#include "shared-bindings/displayio/OnDiskGif.h"

//| class OnDiskGif:
//|     """Loads values straight from disk. This minimizes memory use but can lead to
//|     much slower pixel load times
//|
//|     .. code-block:: Python
//|
//|       import board
//|       import displayio
//|       import time
//|       import pulseio
//|
//|       splash = displayio.Group()
//|       board.DISPLAY.show(splash)
//|
//|       odg = displayio.OnDiskBitmap('/sample.gif')
//|       odg.play_frame() # Load the first frame
//|       face = displayio.TileGrid(odg, pixel_shader=displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565))
//|       splash.append(face)
//|       board.DISPLAY.refresh()
//|
//|       # Wait forever
//|       while True:
//|           gif.play_frame()
//|           time.sleep(0.1)"""
//|
//|     def __init__(self, file: str) -> None:
//|         """Create an OnDiskGif object with the given file.
//|
//|         :param file file: The name of the GIF file.
//|
//|         """
//|         ...
STATIC mp_obj_t displayio_ondiskgif_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_t arg = all_args[0];

    if (mp_obj_is_str(arg)) {
        arg = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), arg, MP_ROM_QSTR(MP_QSTR_rb));
    }

    if (!mp_obj_is_type(arg, &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }

    displayio_ondiskgif_t *self = m_new_obj(displayio_ondiskgif_t);
    self->base.type = &displayio_ondiskgif_type;
    common_hal_displayio_ondiskgif_construct(self, MP_OBJ_TO_PTR(arg));

    return MP_OBJ_FROM_PTR(self);
}

//|     width: int
//|     """Width of the gif. (read only)"""
STATIC mp_obj_t displayio_ondiskgif_obj_get_width(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_width_obj, displayio_ondiskgif_obj_get_width);

MP_PROPERTY_GETTER(displayio_ondiskgif_width_obj,
    (mp_obj_t)&displayio_ondiskgif_get_width_obj);

//|     height: int
//|     """Height of the gif. (read only)"""
STATIC mp_obj_t displayio_ondiskgif_obj_get_height(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_height_obj, displayio_ondiskgif_obj_get_height);

MP_PROPERTY_GETTER(displayio_ondiskgif_height_obj,
    (mp_obj_t)&displayio_ondiskgif_get_height_obj);

//|     bitmap: Bitmap
//|     """The bitmap used to hold the current frame."""
STATIC mp_obj_t displayio_ondiskgif_obj_get_bitmap(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_displayio_ondiskgif_get_bitmap(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_bitmap_obj, displayio_ondiskgif_obj_get_bitmap);

MP_PROPERTY_GETTER(displayio_ondiskgif_bitmap_obj,
    (mp_obj_t)&displayio_ondiskgif_get_bitmap_obj);

//|     play_frame: int
//|     """Play next frame. Returns expected delay until the next frame."""
STATIC mp_obj_t displayio_ondiskgif_obj_play_frame(size_t n_args, const mp_obj_t *args) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(args[0]);
    bool setDirty = mp_const_true;

    if (n_args == 1) {
        setDirty = mp_obj_is_true(args[1]);
    }

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_play_frame(self, setDirty));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(displayio_ondiskgif_play_frame_obj, 1, 2, displayio_ondiskgif_obj_play_frame);

//|     duration: int
//|     """Returns the total duration of the GIF in milliseconds. (read only)"""
STATIC mp_obj_t displayio_ondiskgif_obj_get_duration(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_duration(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_duration_obj, displayio_ondiskgif_obj_get_duration);

MP_PROPERTY_GETTER(displayio_ondiskgif_duration_obj,
    (mp_obj_t)&displayio_ondiskgif_get_duration_obj);

//|     frame_count: int
//|     """Returns the number of frames in the GIF. (read only)"""
STATIC mp_obj_t displayio_ondiskgif_obj_get_frame_count(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_frame_count(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_frame_count_obj, displayio_ondiskgif_obj_get_frame_count);

MP_PROPERTY_GETTER(displayio_ondiskgif_frame_count_obj,
    (mp_obj_t)&displayio_ondiskgif_get_frame_count_obj);

//|     min_delay: int
//|     """The minimum delay found between frames. (read only)"""
STATIC mp_obj_t displayio_ondiskgif_obj_get_min_delay(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_min_delay(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_min_delay_obj, displayio_ondiskgif_obj_get_min_delay);

MP_PROPERTY_GETTER(displayio_ondiskgif_min_delay_obj,
    (mp_obj_t)&displayio_ondiskgif_get_min_delay_obj);

//|     max_delay: int
//|     """The maximum delay found between frames. (read only)"""
//|
STATIC mp_obj_t displayio_ondiskgif_obj_get_max_delay(mp_obj_t self_in) {
    displayio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskgif_get_max_delay(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskgif_get_max_delay_obj, displayio_ondiskgif_obj_get_max_delay);

MP_PROPERTY_GETTER(displayio_ondiskgif_max_delay_obj,
    (mp_obj_t)&displayio_ondiskgif_get_max_delay_obj);

STATIC const mp_rom_map_elem_t displayio_ondiskgif_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_ondiskgif_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&displayio_ondiskgif_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_ondiskgif_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_frame), MP_ROM_PTR(&displayio_ondiskgif_play_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_duration), MP_ROM_PTR(&displayio_ondiskgif_duration_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_count), MP_ROM_PTR(&displayio_ondiskgif_frame_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_delay), MP_ROM_PTR(&displayio_ondiskgif_min_delay_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_delay), MP_ROM_PTR(&displayio_ondiskgif_max_delay_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_ondiskgif_locals_dict, displayio_ondiskgif_locals_dict_table);

const mp_obj_type_t displayio_ondiskgif_type = {
    { &mp_type_type },
    .name = MP_QSTR_OnDiskGif,
    .make_new = displayio_ondiskgif_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_ondiskgif_locals_dict,
};
