// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"

#include "shared-bindings/usb_video/__init__.h"
#include "shared-bindings/usb_video/USBFramebuffer.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"
#include "shared-module/framebufferio/__init__.h"
#include "shared-module/usb_video/__init__.h"
#include "shared-module/usb_video/USBFramebuffer.h"
#include "shared-bindings/util.h"

static void check_for_deinit(usb_video_uvcframebuffer_obj_t *self) {
    if (!shared_module_usb_video_uvcframebuffer_get_width(self)) {
        raise_deinited_error();
    }
}

//| class USBFramebuffer:
//|     """Displays to a USB connected computer using the UVC protocol
//|
//|     The data in the framebuffer is in RGB565_SWAPPED format.
//|
//|     This object is most often used with `framebufferio.FramebufferDisplay`. However,
//|     it also supports the ``WritableBuffer`` protocol and can be accessed
//|     as an array of ``H`` (unsigned 16-bit values)."""
//|
//|     def __new__(self):
//|         """Returns the singleton framebuffer object, if USB video is enabled"""
static mp_obj_t usb_video_uvcframebuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {};
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    usb_video_uvcframebuffer_obj_t *self = &usb_video_uvcframebuffer_singleton_obj;
    return self;
}

//|     def refresh(self) -> None:
//|         """Transmits the color data in the buffer to the pixels so that
//|         they are shown."""
//|         ...
static mp_obj_t usb_video_uvcframebuffer_refresh(mp_obj_t self_in) {
    usb_video_uvcframebuffer_obj_t *self = (usb_video_uvcframebuffer_obj_t *)self_in;
    check_for_deinit(self);
    shared_module_usb_video_uvcframebuffer_refresh(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_video_uvcframebuffer_refresh_obj, usb_video_uvcframebuffer_refresh);

//|     width: int
//|     """The width of the display, in pixels"""
static mp_obj_t usb_video_uvcframebuffer_get_width(mp_obj_t self_in) {
    usb_video_uvcframebuffer_obj_t *self = (usb_video_uvcframebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(shared_module_usb_video_uvcframebuffer_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_video_uvcframebuffer_get_width_obj, usb_video_uvcframebuffer_get_width);
MP_PROPERTY_GETTER(usb_video_uvcframebuffer_width_obj,
    (mp_obj_t)&usb_video_uvcframebuffer_get_width_obj);

//|     height: int
//|     """The height of the display, in pixels"""
//|
static mp_obj_t usb_video_uvcframebuffer_get_height(mp_obj_t self_in) {
    usb_video_uvcframebuffer_obj_t *self = (usb_video_uvcframebuffer_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(shared_module_usb_video_uvcframebuffer_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_video_uvcframebuffer_get_height_obj, usb_video_uvcframebuffer_get_height);

MP_PROPERTY_GETTER(usb_video_uvcframebuffer_height_obj,
    (mp_obj_t)&usb_video_uvcframebuffer_get_height_obj);

static const mp_rom_map_elem_t usb_video_uvcframebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&usb_video_uvcframebuffer_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&usb_video_uvcframebuffer_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&usb_video_uvcframebuffer_height_obj) },
};
static MP_DEFINE_CONST_DICT(usb_video_uvcframebuffer_locals_dict, usb_video_uvcframebuffer_locals_dict_table);

static void usb_video_uvcframebuffer_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    shared_module_usb_video_uvcframebuffer_get_bufinfo(self_in, bufinfo);
}

// These version exists so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
static void usb_video_uvcframebuffer_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmap) {
    (void)dirty_row_bitmap;
    shared_module_usb_video_uvcframebuffer_refresh(self_in);
}

static void usb_video_uvcframebuffer_deinit_proto(mp_obj_t self_in) {
    /* NOTHING */
}

static int usb_video_uvcframebuffer_get_width_proto(mp_obj_t self_in) {
    return shared_module_usb_video_uvcframebuffer_get_width(self_in);
}

static int usb_video_uvcframebuffer_get_height_proto(mp_obj_t self_in) {
    return shared_module_usb_video_uvcframebuffer_get_height(self_in);
}

static int usb_video_uvcframebuffer_get_native_frames_per_second_proto(mp_obj_t self_in) {
    return 10;
}

static bool usb_video_uvcframebuffer_get_reverse_pixels_in_word_proto(mp_obj_t self_in) {
    return true;
}


static const framebuffer_p_t usb_video_uvcframebuffer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = usb_video_uvcframebuffer_get_bufinfo,
    .get_width = usb_video_uvcframebuffer_get_width_proto,
    .get_height = usb_video_uvcframebuffer_get_height_proto,
    .get_native_frames_per_second = usb_video_uvcframebuffer_get_native_frames_per_second_proto,
    .swapbuffers = usb_video_uvcframebuffer_swapbuffers,
    .deinit = usb_video_uvcframebuffer_deinit_proto,
    .get_reverse_pixels_in_word = usb_video_uvcframebuffer_get_reverse_pixels_in_word_proto,
};

static mp_int_t usb_video_uvcframebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    shared_module_usb_video_uvcframebuffer_get_bufinfo(self_in, bufinfo);
    bufinfo->typecode = 'H';
    return 0;
}

MP_DEFINE_CONST_OBJ_TYPE(
    usb_video_USBFramebuffer_type,
    MP_QSTR_USBFramebuffer,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &usb_video_uvcframebuffer_locals_dict,
    make_new, usb_video_uvcframebuffer_make_new,
    buffer, usb_video_uvcframebuffer_get_buffer,
    protocol, &usb_video_uvcframebuffer_proto
    );
