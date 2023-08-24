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
#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/util.h"
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
//|       next_delay = odg.next_frame() # Load the first frame
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
//|           display_bus.send(44, odg.bitmap)
//|
//|       # The following optional code will free the OnDiskGif and allocated resources
//|       # after use. This may be required before loading a new GIF in situations
//|       # where RAM is limited and the first GIF took most of the RAM.
//|       odg.deinit()
//|       odg = None
//|       gc.collect()
//|
//|     """
//|
//|     def __init__(self, file: str) -> None:
//|         """Create an `OnDiskGif` object with the given file.
//|         The GIF frames are decoded into RGB565 big-endian format.
//|         `displayio` expects little-endian, so the example above uses `Colorspace.RGB565_SWAPPED`.
//|
//|         :param file file: The name of the GIF file.
//|
//|         If the image is too large it will be cropped at the bottom and right when displayed.
//|
//|         **Limitations**: The image width is limited to 320 pixels at present. `ValueError`
//|         will be raised if the image is too wide. The height
//|         is not limited but images that are too large will cause a memory exception.
//|         """
//|         ...
STATIC mp_obj_t gifio_ondiskgif_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_filename, ARG_use_palette, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filename, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_use_palette, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t filename = all_args[0];
    if (mp_obj_is_str(filename)) {
        filename = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), filename, MP_ROM_QSTR(MP_QSTR_rb));
    }

    if (!mp_obj_is_type(filename, &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }

    gifio_ondiskgif_t *self = mp_obj_malloc(gifio_ondiskgif_t, &gifio_ondiskgif_type);
    common_hal_gifio_ondiskgif_construct(self, MP_OBJ_TO_PTR(filename), args[ARG_use_palette].u_bool);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(gifio_ondiskgif_t *self) {
    if (common_hal_gifio_ondiskgif_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> OnDiskGif:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the GIF when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t gifio_ondiskgif_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_gifio_ondiskgif_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gifio_ondiskgif___exit___obj, 4, 4, gifio_ondiskgif_obj___exit__);

//|     width: int
//|     """Width of the gif. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_width(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_width_obj, gifio_ondiskgif_obj_get_width);

MP_PROPERTY_GETTER(gifio_ondiskgif_width_obj,
    (mp_obj_t)&gifio_ondiskgif_get_width_obj);

//|     height: int
//|     """Height of the gif. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_height(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_height_obj, gifio_ondiskgif_obj_get_height);

MP_PROPERTY_GETTER(gifio_ondiskgif_height_obj,
    (mp_obj_t)&gifio_ondiskgif_get_height_obj);

//|     bitmap: displayio.Bitmap
//|     """The bitmap used to hold the current frame."""
STATIC mp_obj_t gifio_ondiskgif_obj_get_bitmap(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return common_hal_gifio_ondiskgif_get_bitmap(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_bitmap_obj, gifio_ondiskgif_obj_get_bitmap);

MP_PROPERTY_GETTER(gifio_ondiskgif_bitmap_obj,
    (mp_obj_t)&gifio_ondiskgif_get_bitmap_obj);

//|     palette: Optional[displayio.Palette]
//|     """The palette for the current frame if it exists."""
STATIC mp_obj_t gifio_ondiskgif_obj_get_palette(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return common_hal_gifio_ondiskgif_get_palette(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_palette_obj, gifio_ondiskgif_obj_get_palette);

MP_PROPERTY_GETTER(gifio_ondiskgif_palette_obj,
    (mp_obj_t)&gifio_ondiskgif_get_palette_obj);

//|     def next_frame(self) -> float:
//|         """Loads the next frame. Returns expected delay before the next frame in seconds."""
STATIC mp_obj_t gifio_ondiskgif_obj_next_frame(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_next_frame(self, true) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_next_frame_obj, gifio_ondiskgif_obj_next_frame);


//|     duration: float
//|     """Returns the total duration of the GIF in seconds. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_duration(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_get_duration(self) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_duration_obj, gifio_ondiskgif_obj_get_duration);

MP_PROPERTY_GETTER(gifio_ondiskgif_duration_obj,
    (mp_obj_t)&gifio_ondiskgif_get_duration_obj);

//|     frame_count: int
//|     """Returns the number of frames in the GIF. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_frame_count(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_gifio_ondiskgif_get_frame_count(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_frame_count_obj, gifio_ondiskgif_obj_get_frame_count);

MP_PROPERTY_GETTER(gifio_ondiskgif_frame_count_obj,
    (mp_obj_t)&gifio_ondiskgif_get_frame_count_obj);

//|     min_delay: float
//|     """The minimum delay found between frames. (read only)"""
STATIC mp_obj_t gifio_ondiskgif_obj_get_min_delay(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);

    check_for_deinit(self);
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

    check_for_deinit(self);
    return mp_obj_new_float((float)common_hal_gifio_ondiskgif_get_max_delay(self) / 1000);
}

MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_get_max_delay_obj, gifio_ondiskgif_obj_get_max_delay);

MP_PROPERTY_GETTER(gifio_ondiskgif_max_delay_obj,
    (mp_obj_t)&gifio_ondiskgif_get_max_delay_obj);

//|     def deinit(self) -> None:
//|         """Release resources allocated by OnDiskGif."""
//|         ...
//|
STATIC mp_obj_t gifio_ondiskgif_obj_deinit(mp_obj_t self_in) {
    gifio_ondiskgif_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_gifio_ondiskgif_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gifio_ondiskgif_deinit_obj, gifio_ondiskgif_obj_deinit);

STATIC const mp_rom_map_elem_t gifio_ondiskgif_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&gifio_ondiskgif_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&gifio_ondiskgif___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&gifio_ondiskgif_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&gifio_ondiskgif_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette), MP_ROM_PTR(&gifio_ondiskgif_palette_obj) },
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
