/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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
#if MICROPY_VFS
#include "extmod/vfs.h"
#endif
#include "py/runtime.h"
#include "shared-bindings/gifio/GifWriter.h"
#include "shared-module/gifio/GifWriter.h"
#include "shared/runtime/context_manager_helpers.h"

//| class GifWriter:
//|     def __init__(
//|         self,
//|         file: Union[typing.BinaryIO, str],
//|         width: int,
//|         height: int,
//|         colorspace: displayio.Colorspace,
//|         loop: bool = True,
//|         dither: bool = False,
//|     ) -> None:
//|         """Construct a GifWriter object
//|
//|         :param file: Either a file open in bytes mode, or the name of a file to open in bytes mode.
//|         :param width: The width of the image.  All frames must have the same width.
//|         :param height: The height of the image.  All frames must have the same height.
//|         :param colorspace: The colorspace of the image.  All frames must have the same colorspace.  The supported colorspaces are ``RGB565``, ``BGR565``, ``RGB565_SWAPPED``, ``BGR565_SWAPPED``, and ``L8`` (greyscale)
//|         :param loop: If True, the GIF is marked for looping playback
//|         :param dither: If True, and the image is in color, a simple ordered dither is applied.
//|         """
//|         ...
static mp_obj_t gifio_gifwriter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_file, ARG_width, ARG_height, ARG_colorspace, ARG_loop, ARG_dither };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL} },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_colorspace, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL} },
        { MP_QSTR_loop, MP_ARG_BOOL, { .u_bool = true } },
        { MP_QSTR_dither, MP_ARG_BOOL, { .u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t file = args[ARG_file].u_obj;
    bool own_file = false;
    if (mp_obj_is_str(file)) {
        file = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), file, MP_OBJ_NEW_QSTR(MP_QSTR_wb));
        own_file = true;
    }

    gifio_gifwriter_t *self = mp_obj_malloc(gifio_gifwriter_t, &gifio_gifwriter_type);
    shared_module_gifio_gifwriter_construct(
        self,
        file,
        args[ARG_width].u_int,
        args[ARG_height].u_int,
        (displayio_colorspace_t)cp_enum_value(&displayio_colorspace_type, args[ARG_colorspace].u_obj, MP_QSTR_colorspace),
        args[ARG_loop].u_bool,
        args[ARG_dither].u_bool,
        own_file);

    return self;
}


//|     def __enter__(self) -> GifWriter:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t gifio_gifwriter___exit__(size_t n_args, const mp_obj_t *args) {
    gifio_gifwriter_t *self = MP_OBJ_TO_PTR(args[0]);
    shared_module_gifio_gifwriter_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gifio_gifwriter___exit___obj, 4, 4, gifio_gifwriter___exit__);

//|     def deinit(self) -> None:
//|         """Close the underlying file."""
//|         ...
static mp_obj_t gifio_gifwriter_deinit(mp_obj_t self_in) {
    gifio_gifwriter_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_gifio_gifwriter_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gifio_gifwriter_deinit_obj, gifio_gifwriter_deinit);

//|     def add_frame(self, bitmap: ReadableBuffer, delay: float = 0.1) -> None:
//|         """Add a frame to the GIF.
//|
//|         :param bitmap: The frame data
//|         :param delay: The frame delay in seconds.  The GIF format rounds this to the nearest 1/100 second, and the largest permitted value is 655 seconds.
//|         """
//|         ...
//|
static mp_obj_t gifio_gifwriter_add_frame(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bitmap, ARG_delay };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bitmap, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL} },
        { MP_QSTR_delay, MP_ARG_OBJ, {.u_obj = NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    gifio_gifwriter_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    shared_module_gifio_gifwriter_check_for_deinit(self);


    mp_float_t delay = mp_arg_validate_obj_float_non_negative(args[ARG_delay].u_obj, MICROPY_FLOAT_CONST(0.1), MP_QSTR_delay);
    if (delay > MICROPY_FLOAT_CONST(655.)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be <= %d"), MP_QSTR_delay, 655);
    }

    int delay_centiseconds = (int)(delay * 100);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_bitmap].u_obj, &bufinfo, MP_BUFFER_READ);
    shared_module_gifio_gifwriter_add_frame(self, &bufinfo, delay_centiseconds);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(gifio_gifwriter_add_frame_obj, 1, gifio_gifwriter_add_frame);

STATIC const mp_rom_map_elem_t gifio_gifwriter_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_GifWriter) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&gifio_gifwriter___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&gifio_gifwriter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_add_frame), MP_ROM_PTR(&gifio_gifwriter_add_frame_obj) },
};

STATIC MP_DEFINE_CONST_DICT(gifio_gifwriter_locals, gifio_gifwriter_locals_table);

MP_DEFINE_CONST_OBJ_TYPE(
    gifio_gifwriter_type,
    MP_QSTR_GifWriter,
    MP_TYPE_FLAG_NONE,
    make_new, gifio_gifwriter_make_new,
    locals_dict, &gifio_gifwriter_locals
    );
