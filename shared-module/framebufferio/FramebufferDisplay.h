/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FRAMEBUFFERDISPLAY_H
#define MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FRAMEBUFFERDISPLAY_H

#include "py/obj.h"
#include "py/proto.h"

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/display_core.h"

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    mp_obj_t framebuffer;
    const struct _framebuffer_p_t *framebuffer_protocol;
    mp_buffer_info_t bufinfo;
    uint64_t last_backlight_refresh;
    uint64_t last_refresh_call;
    uint16_t native_frames_per_second;
    uint16_t native_ms_per_frame;
    uint16_t first_pixel_offset;
    uint16_t row_stride;
    bool auto_refresh;
    bool first_manual_refresh;
} framebufferio_framebufferdisplay_obj_t;

void framebufferio_framebufferdisplay_background(framebufferio_framebufferdisplay_obj_t *self);
void release_framebufferdisplay(framebufferio_framebufferdisplay_obj_t *self);
void framebufferio_framebufferdisplay_reset(framebufferio_framebufferdisplay_obj_t *self);

void framebufferio_framebufferdisplay_collect_ptrs(framebufferio_framebufferdisplay_obj_t *self);

mp_obj_t common_hal_framebufferio_framebufferdisplay_get_framebuffer(framebufferio_framebufferdisplay_obj_t *self);

typedef bool (*framebuffer_get_auto_brightness_fun)(mp_obj_t);
typedef bool (*framebuffer_get_reverse_pixels_in_byte_fun)(mp_obj_t);
typedef bool (*framebuffer_get_reverse_pixels_in_word_fun)(mp_obj_t);
typedef bool (*framebuffer_set_auto_brightness_fun)(mp_obj_t, bool);
typedef bool (*framebuffer_set_brightness_fun)(mp_obj_t, mp_float_t);
typedef int (*framebuffer_get_bytes_per_cell_fun)(mp_obj_t);
typedef int (*framebuffer_get_color_depth_fun)(mp_obj_t);
typedef int (*framebuffer_get_first_pixel_offset_fun)(mp_obj_t);
typedef int (*framebuffer_get_grayscale_fun)(mp_obj_t);
typedef int (*framebuffer_get_height_fun)(mp_obj_t);
typedef int (*framebuffer_get_native_frames_per_second_fun)(mp_obj_t);
typedef bool (*framebuffer_get_pixels_in_byte_share_row_fun)(mp_obj_t);
typedef int (*framebuffer_get_row_stride_fun)(mp_obj_t);
typedef int (*framebuffer_get_width_fun)(mp_obj_t);
typedef mp_float_t (*framebuffer_get_brightness_fun)(mp_obj_t);
typedef void (*framebuffer_deinit_fun)(mp_obj_t);
typedef void (*framebuffer_get_bufinfo_fun)(mp_obj_t, mp_buffer_info_t *bufinfo);
typedef void (*framebuffer_swapbuffers_fun)(mp_obj_t, uint8_t *dirty_row_bitmask);

typedef struct _framebuffer_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_framebuffer

    // Mandatory
    framebuffer_get_bufinfo_fun get_bufinfo;
    framebuffer_swapbuffers_fun swapbuffers;
    framebuffer_deinit_fun deinit;
    framebuffer_get_width_fun get_width;
    framebuffer_get_height_fun get_height;

    // Optional getters
    framebuffer_get_bytes_per_cell_fun get_bytes_per_cell; // default: 2
    framebuffer_get_color_depth_fun get_color_depth; // default: 16
    framebuffer_get_first_pixel_offset_fun get_first_pixel_offset; // default: 0
    framebuffer_get_grayscale_fun get_grayscale; // default: grayscale if depth < 8
    framebuffer_get_native_frames_per_second_fun get_native_frames_per_second; // default: 60
    framebuffer_get_pixels_in_byte_share_row_fun get_pixels_in_byte_share_row; // default: false
    framebuffer_get_reverse_pixels_in_byte_fun get_reverse_pixels_in_byte; // default: false
    framebuffer_get_reverse_pixels_in_word_fun get_reverse_pixels_in_word; // default: false
    framebuffer_get_row_stride_fun get_row_stride; // default: 0 (no extra row padding)

    // Optional -- default is no brightness control
    framebuffer_get_brightness_fun get_brightness;
    framebuffer_set_brightness_fun set_brightness;

    // Optional -- default is no automatic brightness control
    framebuffer_get_auto_brightness_fun get_auto_brightness;
    framebuffer_set_auto_brightness_fun set_auto_brightness;
} framebuffer_p_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_DISPLAYIO_FRAMEBUFFERDISPLAY_H
