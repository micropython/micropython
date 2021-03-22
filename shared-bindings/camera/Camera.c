/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
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

#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/camera/Camera.h"
#include "shared-bindings/util.h"

//| class Camera:
//|     """The class to control camera.
//|
//|     Usage::
//|
//|         import board
//|         import sdioio
//|         import storage
//|         import camera
//|
//|         sd = sdioio.SDCard(
//|             clock=board.SDIO_CLOCK,
//|             command=board.SDIO_COMMAND,
//|             data=board.SDIO_DATA,
//|             frequency=25000000)
//|         vfs = storage.VfsFat(sd)
//|         storage.mount(vfs, '/sd')
//|
//|         cam = camera.Camera()
//|
//|         buffer = bytearray(512 * 1024)
//|         file = open("/sd/image.jpg","wb")
//|         size = cam.take_picture(buffer, width=1920, height=1080, format=camera.ImageFormat.JPG)
//|         file.write(buffer, size)
//|         file.close()"""
//|

//|     def __init__(self) -> None:
//|         """Initialize camera."""
//|         ...
//|
STATIC mp_obj_t camera_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    camera_obj_t *self = m_new_obj(camera_obj_t);
    self->base.type = &camera_type;
    // No arguments
    mp_arg_check_num(n_args, kw_args, 0, 0, false);

    common_hal_camera_construct(self);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """De-initialize camera."""
//|         ...
//|
STATIC mp_obj_t camera_obj_deinit(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_camera_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_deinit_obj, camera_obj_deinit);

STATIC void check_for_deinit(camera_obj_t *self) {
    if (common_hal_camera_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def take_picture(self, buf: WriteableBuffer, format: ImageFormat) -> int:
//|         """Take picture and save to ``buf`` in the given ``format``. The size of the picture
//|         taken is ``width`` by ``height`` in pixels.
//|
//|         :return: the number of bytes written into buf
//|         :rtype: int"""
//|         ...
//|
STATIC mp_obj_t camera_obj_take_picture(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_width, ARG_height, ARG_format };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_width, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_format, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    camera_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    camera_imageformat_t format = camera_imageformat_obj_to_type(args[ARG_format].u_obj);

    return MP_OBJ_NEW_SMALL_INT(common_hal_camera_take_picture(self, (uint8_t *)bufinfo.buf, bufinfo.len, args[ARG_width].u_int, args[ARG_height].u_int, format));
}
MP_DEFINE_CONST_FUN_OBJ_KW(camera_take_picture_obj, 2, camera_obj_take_picture);

STATIC const mp_rom_map_elem_t camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_picture), MP_ROM_PTR(&camera_take_picture_obj) },
};
STATIC MP_DEFINE_CONST_DICT(camera_locals_dict, camera_locals_dict_table);

const mp_obj_type_t camera_type = {
    { &mp_type_type },
    .name = MP_QSTR_Camera,
    .make_new = camera_make_new,
    .locals_dict = (mp_obj_dict_t *)&camera_locals_dict,
};
