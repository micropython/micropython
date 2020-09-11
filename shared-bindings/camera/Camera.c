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
//|         cam = camera.Camera(1920, 1080)
//|
//|         buffer = bytearray(512 * 1024)
//|         file = open("/sd/image.jpg","wb")
//|         size = cam.take_picture()
//|         file.write(buffer, size)
//|         file.close()"""
//|

//|     def __init__(self, width: int, height: int) -> None:
//|         """Initialize camera.
//|
//|         :param int width: Width in pixels
//|         :param int height: Height in pixels"""
//|         ...
//|
STATIC mp_obj_t camera_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    camera_obj_t *self = m_new_obj(camera_obj_t);
    self->base.type = &camera_type;
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_camera_construct(self, args[ARG_width].u_int, args[ARG_height].u_int);
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
//|         """Take picture and save to ``buf`` in the given ``format``
//|
//|         :return: the size of the picture taken
//|         :rtype: int"""
//|         ...
//|
STATIC mp_obj_t camera_obj_take_picture(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_format };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_format, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    camera_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    camera_imageformat_t format = camera_imageformat_obj_to_type(args[ARG_format].u_obj);

    return MP_OBJ_NEW_SMALL_INT(common_hal_camera_take_picture(self, (uint8_t *)bufinfo.buf, bufinfo.len, format));
}
MP_DEFINE_CONST_FUN_OBJ_KW(camera_take_picture_obj, 3, camera_obj_take_picture);

//|     width: int
//|     """Image width in pixels."""
//|
STATIC mp_obj_t camera_obj_get_width(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_camera_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_get_width_obj, camera_obj_get_width);

STATIC mp_obj_t camera_obj_set_width(mp_obj_t self_in, mp_obj_t value) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_camera_set_width(self, mp_obj_get_int(value));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(camera_set_width_obj, camera_obj_set_width);

const mp_obj_property_t camera_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&camera_get_width_obj,
              (mp_obj_t)&camera_set_width_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     height: int
//|     """Image height in pixels."""
//|
STATIC mp_obj_t camera_obj_get_height(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_camera_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_get_height_obj, camera_obj_get_height);

STATIC mp_obj_t camera_obj_set_height(mp_obj_t self_in, mp_obj_t value) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_camera_set_height(self, mp_obj_get_int(value));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(camera_set_height_obj, camera_obj_set_height);

const mp_obj_property_t camera_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&camera_get_height_obj,
              (mp_obj_t)&camera_set_height_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_picture), MP_ROM_PTR(&camera_take_picture_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&camera_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&camera_height_obj) },
};
STATIC MP_DEFINE_CONST_DICT(camera_locals_dict, camera_locals_dict_table);

const mp_obj_type_t camera_type = {
   { &mp_type_type },
   .name = MP_QSTR_Camera,
   .make_new = camera_make_new,
   .locals_dict = (mp_obj_dict_t*)&camera_locals_dict,
};
