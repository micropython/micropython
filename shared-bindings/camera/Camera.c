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
//|         cam = camera.Camera(camera.ImageSize.IMAGE_SIZE_1920x1080)
//|
//|         file = open("/sd/image.jpg","wb")
//|         cam.take_picture()
//|         file.write(cam.picture)
//|         file.close()"""
//|

//|     def __init__(self, ):
//|         """Initialize camera.
//|
//|         :param camera.ImageSize size: image size"""
//|         ...
//|
STATIC mp_obj_t camera_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    camera_obj_t *self = m_new_obj(camera_obj_t);
    self->base.type = &camera_type;
    enum { ARG_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    camera_imagesize_t size = camera_imagesize_obj_to_type(args[ARG_size].u_obj);

    common_hal_camera_construct(self, size);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self, ) -> Any:
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

//|     def take_picture(self, ) -> Any:
//|         """Take picture."""
//|         ...
//|
STATIC mp_obj_t camera_obj_take_picture(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_camera_take_picture(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_take_picture_obj, camera_obj_take_picture);

//|     picture: Any = ...
//|     """Image buffer."""
//|
STATIC mp_obj_t camera_obj_get_picture(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    uint8_t *buffer = common_hal_camera_get_picture_buffer(self);
    size_t size = common_hal_camera_get_picture_size(self);

    return mp_obj_new_bytearray_by_ref(size, buffer);
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_get_picture_obj, camera_obj_get_picture);

const mp_obj_property_t camera_picture_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&camera_get_picture_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     size: Any = ...
//|     """Image size."""
//|
STATIC mp_obj_t camera_obj_get_size(mp_obj_t self_in) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return camera_imagesize_type_to_obj(common_hal_camera_get_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(camera_get_size_obj, camera_obj_get_size);

STATIC mp_obj_t camera_obj_set_size(mp_obj_t self_in, mp_obj_t value) {
    camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    camera_imagesize_t size = camera_imagesize_obj_to_type(value);
    if (size == IMAGESIZE_NONE) {
        mp_raise_ValueError(translate("Invalid image size."));
    }

    common_hal_camera_set_size(self, size);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(camera_set_size_obj, camera_obj_set_size);

const mp_obj_property_t camera_size_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&camera_get_size_obj,
              (mp_obj_t)&camera_set_size_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_picture), MP_ROM_PTR(&camera_take_picture_obj) },

    { MP_ROM_QSTR(MP_QSTR_picture), MP_ROM_PTR(&camera_picture_obj) },
    { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&camera_size_obj) },
};
STATIC MP_DEFINE_CONST_DICT(camera_locals_dict, camera_locals_dict_table);

const mp_obj_type_t camera_type = {
   { &mp_type_type },
   .name = MP_QSTR_GNSS,
   .make_new = camera_make_new,
   .locals_dict = (mp_obj_dict_t*)&camera_locals_dict,
};
