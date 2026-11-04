// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/OnDiskBitmap.h"

#include <stdint.h>

#include "py/runtime.h"
#include "extmod/vfs.h"
#include "py/objproperty.h"
#include "shared/runtime/context_manager_helpers.h"

#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/util.h"

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
//|       board.DISPLAY.brightness = 0
//|       splash = displayio.Group()
//|       board.DISPLAY.root_group = splash
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
//|     def __init__(self, file: str) -> None:
//|         """Create an OnDiskBitmap object with the given file.
//|
//|         :param str file: The name of the bitmap file.
//|
//|         CircuitPython 11 no longer accepts a file opened in binary mode. Pass the filename instead.
//|         OnDiskBitmap opens the file itself and keeps it open until `deinit()` or
//|         until the object is garbage collected.
//|         """
//|         ...
//|
static mp_obj_t displayio_ondiskbitmap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_t filename = mp_arg_validate_type_string(all_args[0], MP_QSTR_file);
    mp_obj_t file = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), filename, MP_ROM_QSTR(MP_QSTR_rb));

    displayio_ondiskbitmap_t *self = mp_obj_malloc(displayio_ondiskbitmap_t, &displayio_ondiskbitmap_type);
    common_hal_displayio_ondiskbitmap_construct(self, file);

    return MP_OBJ_FROM_PTR(self);
}

static void check_for_deinit(displayio_ondiskbitmap_t *self) {
    if (common_hal_displayio_ondiskbitmap_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Close the file. The object can no longer be used. If a `TileGrid` still shows
//|         this bitmap, areas redrawn after this use color 0, so remove the `TileGrid` first."""
//|         ...
//|
static mp_obj_t displayio_ondiskbitmap_obj_deinit(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_displayio_ondiskbitmap_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_deinit_obj, displayio_ondiskbitmap_obj_deinit);

//|     def __enter__(self) -> OnDiskBitmap:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the object when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     width: int
//|     """Width of the bitmap. (read only)"""
static mp_obj_t displayio_ondiskbitmap_obj_get_width(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskbitmap_get_width(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_width_obj, displayio_ondiskbitmap_obj_get_width);

MP_PROPERTY_GETTER(displayio_ondiskbitmap_width_obj,
    (mp_obj_t)&displayio_ondiskbitmap_get_width_obj);

//|     height: int
//|     """Height of the bitmap. (read only)"""
static mp_obj_t displayio_ondiskbitmap_obj_get_height(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_ondiskbitmap_get_height(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_height_obj, displayio_ondiskbitmap_obj_get_height);

MP_PROPERTY_GETTER(displayio_ondiskbitmap_height_obj,
    (mp_obj_t)&displayio_ondiskbitmap_get_height_obj);

//|     pixel_shader: Union[ColorConverter, Palette]
//|     """The image's pixel_shader.  The type depends on the underlying
//|     bitmap's structure.  The pixel shader can be modified (e.g., to set the
//|     transparent pixel or, for palette shaded images, to update the palette.)"""
//|
//|
static mp_obj_t displayio_ondiskbitmap_obj_get_pixel_shader(mp_obj_t self_in) {
    displayio_ondiskbitmap_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_displayio_ondiskbitmap_get_pixel_shader(self);
}

MP_DEFINE_CONST_FUN_OBJ_1(displayio_ondiskbitmap_get_pixel_shader_obj, displayio_ondiskbitmap_obj_get_pixel_shader);

MP_PROPERTY_GETTER(displayio_ondiskbitmap_pixel_shader_obj,
    (mp_obj_t)&displayio_ondiskbitmap_get_pixel_shader_obj);

static const mp_rom_map_elem_t displayio_ondiskbitmap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&displayio_ondiskbitmap_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&displayio_ondiskbitmap_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&displayio_ondiskbitmap_pixel_shader_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&displayio_ondiskbitmap_width_obj) },
};
static MP_DEFINE_CONST_DICT(displayio_ondiskbitmap_locals_dict, displayio_ondiskbitmap_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    displayio_ondiskbitmap_type,
    MP_QSTR_OnDiskBitmap,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, displayio_ondiskbitmap_make_new,
    locals_dict, &displayio_ondiskbitmap_locals_dict
    );
