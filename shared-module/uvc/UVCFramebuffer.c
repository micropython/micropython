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

#include "shared-module/uvc/__init__.h"
#include "shared-module/uvc/UVCFramebuffer.h"
#include "shared-bindings/uvc/UVCFramebuffer.h"
#include "shared-bindings/uvc/__init__.h"

uvc_uvcframebuffer_obj_t uvc_uvcframebuffer_singleton_obj = {
    .base = { &uvc_UVCFramebuffer_type },
};

void shared_module_uvc_uvcframebuffer_get_bufinfo(uvc_uvcframebuffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    bufinfo->buf = uvc_framebuffer_rgb565;
    bufinfo->len = 2 * uvc_frame_width * uvc_frame_height;
}

void shared_module_uvc_uvcframebuffer_refresh(uvc_uvcframebuffer_obj_t *self) {
    shared_module_uvc_swapbuffers();
}

int shared_module_uvc_uvcframebuffer_get_width(uvc_uvcframebuffer_obj_t *self) {
    return uvc_frame_width;
}

int shared_module_uvc_uvcframebuffer_get_height(uvc_uvcframebuffer_obj_t *self) {
    return uvc_frame_height;
}
