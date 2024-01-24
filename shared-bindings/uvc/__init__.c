/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/uvc/__init__.h"
#include "shared-module/uvc/__init__.h"

//| """Allows streaming bitmaps to a host computer via USB UVC
//|
//| This interface is experimental and may change without notice even in stable
//| versions of CircuitPython."""
//|
//| bitmap: displayio.Bitmap
//| """The content of this bitmap image is streamed to the host computer. To change the
//| displayed image, modify the bitmap contents.
//|
//| The bitmap content is in RGB565_SWAPPED format. It is converted to "YUYV" format before
//| transmission to the host computer. The "YUYV" format stores a "Y" (luminance) sample
//| for every pixel, but a chromonance (UV) value only for each pair of pixels; due to this,
//| the color of adjacent pixels cannot be fully controlled. For instance, there is no way
//| for the first pixel to be red and the second pixel to be green. However, they can both
//| be different shades of red.
//| """
//|

//| def enable_framebuffer(width: int, height: int) -> None:
//|     """Enable the UVC gadget, setting the given width & height
//|
//|     During ``boot.py``, this function may be used to enable the UVC gadget.
//|
//|     Width is rounded up to a multiple of 2.
//|
//|     After boot.py completes, the framebuffer will be allocated. Total storage
//|     of 4×``width``×``height`` bytes is required, reducing the amount available
//|     for Python objects. If the allocation fails, a note will be written in boot.py
//|     and UVC will be disabled"""
//|

STATIC mp_obj_t uvc_enable_framebuffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // (but note that most devices will not be able to allocate this much memory.
    uint32_t width = mp_arg_validate_int_range(args[ARG_width].u_int, 0, 32767, MP_QSTR_width);
    uint32_t height = mp_arg_validate_int_range(args[ARG_height].u_int, 0, 32767, MP_QSTR_height);
    if (!shared_module_uvc_enable(width, height)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }

    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(uvc_enable_framebuffer_obj, 0, uvc_enable_framebuffer);

//|
//| def disable() -> None:
//|     """Disable the UVC gadget
//|
//|     During ``boot.py``, this function may be used to enable the UVC gadget.
//|
//|     This function is provided for completeness. The default state of UVC is disabled."""
//|
STATIC mp_obj_t uvc_disable(void) {
    if (!shared_module_uvc_disable()) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }

    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_0(uvc_disable_obj, uvc_disable);


static const mp_rom_map_elem_t uvc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uvc) },
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&uvc_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&uvc_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_framebuffer), MP_ROM_PTR(&uvc_enable_framebuffer_obj) },
};

static MP_DEFINE_CONST_DICT(uvc_module_globals, uvc_module_globals_table);

const mp_obj_module_t uvc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&uvc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_uvc, uvc_module);
