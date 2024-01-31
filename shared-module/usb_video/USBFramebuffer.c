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

#include "shared-module/usb_video/__init__.h"
#include "shared-module/usb_video/USBFramebuffer.h"
#include "shared-bindings/usb_video/USBFramebuffer.h"
#include "shared-bindings/usb_video/__init__.h"

usb_video_uvcframebuffer_obj_t usb_video_uvcframebuffer_singleton_obj = {
    .base = { &usb_video_USBFramebuffer_type },
};

void shared_module_usb_video_uvcframebuffer_get_bufinfo(usb_video_uvcframebuffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    bufinfo->buf = usb_video_framebuffer_rgb565;
    bufinfo->len = 2 * usb_video_frame_width * usb_video_frame_height;
}

void shared_module_usb_video_uvcframebuffer_refresh(usb_video_uvcframebuffer_obj_t *self) {
    shared_module_usb_video_swapbuffers();
}

int shared_module_usb_video_uvcframebuffer_get_width(usb_video_uvcframebuffer_obj_t *self) {
    return usb_video_frame_width;
}

int shared_module_usb_video_uvcframebuffer_get_height(usb_video_uvcframebuffer_obj_t *self) {
    return usb_video_frame_height;
}
