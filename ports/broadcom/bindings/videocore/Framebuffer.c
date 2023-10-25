/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "bindings/videocore/Framebuffer.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

//| class Framebuffer:
//|     """A VideoCore managed frame buffer."""
//|
//|     def __init__(self, width: int, height: int) -> None:
//|         """Create a Framebuffer object with the given dimensions. Memory is
//|            allocated outside of the heap in GPU memory.
//|
//|         The framebuffer is in "ARGB8888" format.
//|
//|         A Framebuffer is often used in conjunction with a
//|         `framebufferio.FramebufferDisplay`."""

STATIC mp_obj_t videocore_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_width, ARG_height, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    videocore_framebuffer_obj_t *self = &allocate_display_bus_or_raise()->videocore;
    self->base.type = &videocore_framebuffer_type;

    mp_uint_t width = (mp_uint_t)mp_arg_validate_int_min(args[ARG_width].u_int, 0, MP_QSTR_width);
    mp_uint_t height = (mp_uint_t)mp_arg_validate_int_min(args[ARG_height].u_int, 0, MP_QSTR_height);
    common_hal_videocore_framebuffer_construct(self, width, height);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         rgbmatrix instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
STATIC mp_obj_t videocore_framebuffer_deinit(mp_obj_t self_in) {
    videocore_framebuffer_obj_t *self = (videocore_framebuffer_obj_t *)self_in;
    common_hal_videocore_framebuffer_deinit(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(videocore_framebuffer_deinit_obj, videocore_framebuffer_deinit);

static void check_for_deinit(videocore_framebuffer_obj_t *self) {
    if (common_hal_videocore_framebuffer_deinited(self)) {
        raise_deinited_error();
    }
}

//|     width: int
//|     """The width of the display, in pixels"""
STATIC mp_obj_t videocore_framebuffer_get_width(mp_obj_t self_in) {
    videocore_framebuffer_obj_t *self = (videocore_framebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_videocore_framebuffer_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(videocore_framebuffer_get_width_obj, videocore_framebuffer_get_width);
MP_PROPERTY_GETTER(videocore_framebuffer_width_obj,
    (mp_obj_t)&videocore_framebuffer_get_width_obj);

//|     height: int
//|     """The height of the display, in pixels"""
//|
STATIC mp_obj_t videocore_framebuffer_get_height(mp_obj_t self_in) {
    videocore_framebuffer_obj_t *self = (videocore_framebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_videocore_framebuffer_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(videocore_framebuffer_get_height_obj, videocore_framebuffer_get_height);

MP_PROPERTY_GETTER(videocore_framebuffer_height_obj,
    (mp_obj_t)&videocore_framebuffer_get_height_obj);

STATIC const mp_rom_map_elem_t videocore_framebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&videocore_framebuffer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&videocore_framebuffer_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&videocore_framebuffer_height_obj) },
};
STATIC MP_DEFINE_CONST_DICT(videocore_framebuffer_locals_dict, videocore_framebuffer_locals_dict_table);

STATIC void videocore_framebuffer_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    common_hal_videocore_framebuffer_get_buffer(self_in, bufinfo, 0);
}

// These versions exist so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
STATIC void videocore_framebuffer_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmap) {
    (void)dirty_row_bitmap;
    common_hal_videocore_framebuffer_refresh(self_in);
}

STATIC void videocore_framebuffer_deinit_proto(mp_obj_t self_in) {
    common_hal_videocore_framebuffer_deinit(self_in);
}

STATIC int videocore_framebuffer_get_width_proto(mp_obj_t self_in) {
    return common_hal_videocore_framebuffer_get_width(self_in);
}

STATIC int videocore_framebuffer_get_height_proto(mp_obj_t self_in) {
    return common_hal_videocore_framebuffer_get_height(self_in);
}

STATIC int videocore_framebuffer_get_color_depth_proto(mp_obj_t self_in) {
    return 32;
}

STATIC int videocore_framebuffer_get_bytes_per_cell_proto(mp_obj_t self_in) {
    return 1;
}

STATIC int videocore_framebuffer_get_native_frames_per_second_proto(mp_obj_t self_in) {
    return 30;
}

STATIC int videocore_framebuffer_get_row_stride_proto(mp_obj_t self_in) {
    return common_hal_videocore_framebuffer_get_row_stride(self_in);
}

STATIC const framebuffer_p_t videocore_framebuffer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = videocore_framebuffer_get_bufinfo,
    .get_width = videocore_framebuffer_get_width_proto,
    .get_height = videocore_framebuffer_get_height_proto,
    .get_color_depth = videocore_framebuffer_get_color_depth_proto,
    .get_row_stride = videocore_framebuffer_get_row_stride_proto,
    .get_bytes_per_cell = videocore_framebuffer_get_bytes_per_cell_proto,
    .get_native_frames_per_second = videocore_framebuffer_get_native_frames_per_second_proto,
    .swapbuffers = videocore_framebuffer_swapbuffers,
    .deinit = videocore_framebuffer_deinit_proto,
};

MP_DEFINE_CONST_OBJ_TYPE(
    videocore_framebuffer_type,
    MP_QSTR_Framebuffer,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, (mp_obj_dict_t *)&videocore_framebuffer_locals_dict,
    make_new, videocore_framebuffer_make_new,
    buffer, common_hal_videocore_framebuffer_get_buffer,
    protocol, &videocore_framebuffer_proto
    );
