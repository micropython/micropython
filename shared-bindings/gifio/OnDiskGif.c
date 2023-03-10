/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Mark Komus
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

#include "shared-bindings/gifio/OnDiskGif.h"

#include <stdint.h>

#include "py/runtime.h"
#include "py/objproperty.h"
#include "supervisor/shared/translate/translate.h"
#include "shared-bindings/gifio/OnDiskGif.h"

//| class OnDiskGif:
//|     """Loads one frame of a GIF into memory at a time.
//|
//|     The code can be used in cooperation with displayio but this mode is relatively slow:
//|
//|     .. code-block:: Python
//|
//|       import board
//|       import gifio
//|       import displayio
//|       import time
//|
//|       display = board.DISPLAY
//|       splash = displayio.Group()
//|       display.root_group = splash
//|
//|       odg = gifio.OnDiskGif('/sample.gif')
//|
//|       start = time.monotonic()
//|       odg.next_frame() # Load the first frame
//|       end = time.monotonic()
//|       overhead = end - start
//|
//|       face = displayio.TileGrid(
//|           odg.bitmap,
//|           pixel_shader=displayio.ColorConverter(
//|               input_colorspace=displayio.Colorspace.RGB565_SWAPPED
//|           ),
//|       )
//|       splash.append(face)
//|       board.DISPLAY.refresh()
//|
//|       # Display repeatedly.
//|       while True:
//|           # Sleep for the frame delay specified by the GIF,
//|           # minus the overhead measured to advance between frames.
//|           time.sleep(max(0, next_delay - overhead))
//|           next_delay = odg.next_frame()
//|
//|     The displayio Group and TileGrid layers can be bypassed and the image can
//|     be directly blitted to the full screen. This can give a speed-up of ~4x to
//|     ~6x depending on the GIF and display. This requires an LCD that uses
//|     standard codes to set the update area, and which accepts RGB565_SWAPPED
//|     pixel data directly:
//|
//|     .. code-block:: Python
//|
//|       # Initial set-up the same as above
//|
//|       # Take over display to drive directly
//|       display.auto_refresh = False
//|       display_bus = display.bus
//|
//|       # Display repeatedly & directly.
//|       while True:
//|           # Sleep for the frame delay specified by the GIF,
//|           # minus the overhead measured to advance between frames.
//|           time.sleep(max(0, next_delay - overhead))
//|           next_delay = odg.next_frame()
//|
//|           display_bus.send(42, struct.pack(">hh", 0, odg.bitmap.width - 1))
//|           display_bus.send(43, struct.pack(">hh", 0, odg.bitmap.height - 1))
//|           display_bus.send(44, d.bitmap)
//|     """
//|
//|     def __init__(self, file: str) -> None:
//|         """Create an `OnDiskGif` object with the given file.
//|         The GIF frames are decoded into RGB565 big-endian format.
//|         `displayio` expects little-endian, so the example above uses `Colorspace.RGB565_SWAPPED`.
//|
//|         :param file file: The name of the GIF file.
//|         """
//|         ...
STATIC mp_obj_t gifio_ondiskgif_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_t arg = all_args[0];

    if (mp_obj_is_str(arg)) {
        arg = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), arg, MP_ROM_QSTR(MP_QSTR_rb));
    }

    if (!mp_obj_is_type(arg, &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }

    gifio_ondiskgif_t *self = m_new_obj(gifio_ondiskgif_t);
    self->base.type = &gifio_ondiskgif_type;
    common_hal_gifio_ondiskgif_construct(self, MP_OBJ_TO_PTR(arg));

    return MP_OBJ_FROM_PTR(self);
}

//|     width: int
//|     """Width of the gif. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_width(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_width_obj, gifio_ondiskgif_obj_get_width);

MP_PROPERTY_GETTER(gifio_ondiskgif_width_obj,
    (mp_obj_t)&gifio_ondiskgif_get_width_obj);

//|     height: int
//|     """Height of the gif. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_height(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_height_obj, gifio_ondiskgif_obj_get_height);

MP_PROPERTY_GETTER(gifio_ondiskgif_height_obj,
    (mp_obj_t)&gifio_ondiskgif_get_height_obj);

//|     bitmap: displayio.Bitmap
//|     """The bitmap used to hold the current frame."""
STATIC mp_obj_t gifio_ondiskgif_obj_get_bitmap(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_gifio_ondiskgif_get_bitmap(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_bitmap_obj, gifio_ondiskgif_obj_get_bitmap);

MP_PROPERTY_GETTER(gifio_ondiskgif_bitmap_obj,
    (mp_obj_t)&gifio_ondiskgif_get_bitmap_obj);

//|     def next_frame(self) -> float:
//|         """Loads the next frame. Returns expected delay before the next frame in seconds."""
STATIC mp_obj_t gifio_ondiskgif_obj_next_frame(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_next_frame(self, true) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_next_frame_obj, gifio_ondiskgif_obj_next_frame);


//|     duration: float
//|     """Returns the total duration of the GIF in seconds. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_duration(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_get_duration(self) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_duration_obj, gifio_ondiskgif_obj_get_duration);

MP_PROPERTY_GETTER(gifio_ondiskgif_duration_obj,
    (mp_obj_t)&gifio_ondiskgif_get_duration_obj);

//|     frame_count: int
//|     """Returns the number of frames in the GIF. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_frame_count(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_frame_count(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_frame_count_obj, gifio_ondiskgif_obj_get_frame_count);

MP_PROPERTY_GETTER(gifio_ondiskgif_frame_count_obj,
    (mp_obj_t)&gifio_ondiskgif_get_frame_count_obj);

//|     min_delay: float
//|     """The minimum delay found between frames. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_min_delay(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_get_min_delay(self) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_min_delay_obj, gifio_ondiskgif_obj_get_min_delay);

MP_PROPERTY_GETTER(gifio_ondiskgif_min_delay_obj,
    (mp_obj_t)&gifio_ondiskgif_get_min_delay_obj);

//|     max_delay: float
//|     """The maximum delay found between frames. (read only)"""
//|
STATIC mp_obj_t gifio_ondiskgif_obj_get_max_delay(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_get_max_delay(self) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_max_delay_obj, gifio_ondiskgif_obj_get_max_delay);

MP_PROPERTY_GETTER(gifio_ondiskgif_max_delay_obj,
    (mp_obj_t)&gifio_ondiskgif_get_max_delay_obj);

STATIC const mp_rom_map_elem_t gifio_ondiskgif_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&gifio_ondiskgif_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&gifio_ondiskgif_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&gifio_ondiskgif_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_next_frame), MP_ROM_PTR(&gifio_ondiskgif_next_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_duration), MP_ROM_PTR(&gifio_ondiskgif_duration_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_count), MP_ROM_PTR(&gifio_ondiskgif_frame_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_delay), MP_ROM_PTR(&gifio_ondiskgif_min_delay_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_delay), MP_ROM_PTR(&gifio_ondiskgif_max_delay_obj) },
};
STATIC MP_DEFINE_CONST_DICT(gifio_ondiskgif_locals_dict, gifio_ondiskgif_locals_dict_table);

const mp_obj_type_t gifio_ondiskgif_type = {
    { &mp_type_type },
    .name = MP_QSTR_OnDiskGif,
    .make_new = gifio_ondiskgif_make_new,
    .locals_dict = (mp_obj_dict_t *)&gifio_ondiskgif_locals_dict,
};
