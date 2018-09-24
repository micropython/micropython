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
#include "supervisor/shared/translate.h"

//| .. currentmodule:: displayio
//|
//| :class:`OnDiskBitmap` -- Loads pixels straight from disk
//| ==========================================================================
//|
//| Loads values straight from disk. This minimizes memory use but can lead to
//| much slower pixel load times. These load times may result in frame tearing where only part of
//| the image is visible.
//|
//| .. warning:: This will likely be changed before 4.0.0. Consider it very experimental.
//|
//| It's easiest to use on a board with a built in display such as the `Hallowing M0 Express
//| <https://www.adafruit.com/product/3900>`_.
//|
//| .. code-block:: Python
//|
//|   import board
//|   import displayio
//|   import time
//|   import pulseio
//|
//|   backlight = pulseio.PWMOut(board.TFT_BACKLIGHT)
//|   splash = displayio.Group()
//|   board.DISPLAY.show(splash)
//|
//|   with open("/sample.bmp", "rb") as f:
//|       odb = displayio.OnDiskBitmap(f)
//|       face = displayio.Sprite(odb, pixel_shader=displayio.ColorConverter(), position=(0,0))
//|       splash.append(face)
//|       # Wait for the image to load.
//|       board.DISPLAY.wait_for_frame()
//|
//|       # Fade up the backlight
//|       for i in range(100):
//|           backlight.duty_cycle = i * (2 ** 15) // 100
//|           time.sleep(0.01)
//|
//|       # Wait forever
//|       while True:
//|           pass
//|
//| .. class:: OnDiskBitmap(file)
//|
//|   Create an OnDiskBitmap object with the given file.
//|
//|   :param file file: The open bitmap file
//|
STATIC mp_obj_t displayio_ondiskbitmap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    if (!MP_OBJ_IS_TYPE(pos_args[0], &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }

    displayio_ondiskbitmap_t *self = m_new_obj(displayio_ondiskbitmap_t);
    self->base.type = &displayio_ondiskbitmap_type;
    common_hal_displayio_ondiskbitmap_construct(self, MP_OBJ_TO_PTR(pos_args[0]));

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_rom_map_elem_t displayio_ondiskbitmap_locals_dict_table[] = {
};
STATIC MP_DEFINE_CONST_DICT(displayio_ondiskbitmap_locals_dict, displayio_ondiskbitmap_locals_dict_table);

const mp_obj_type_t displayio_ondiskbitmap_type = {
    { &mp_type_type },
    .name = MP_QSTR_OnDiskBitmap,
    .make_new = displayio_ondiskbitmap_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_ondiskbitmap_locals_dict,
};
