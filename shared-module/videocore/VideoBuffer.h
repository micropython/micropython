// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/vectorio/__init__.h" // draw protocol
#include "shared-module/displayio/area.h"

#include "shared-module/framebufferio/FramebufferDisplay.h" // framebuffer protocol
#include "shared-module/videocore/__init__.h"

typedef struct {
    mp_obj_base_t base;
    // Object frames are streamed into: an object implementing the framebuffer
    // protocol, such as mipidsi.Display, or a writable buffer such as a
    // bytearray or memoryview.
    mp_obj_t framebuffer;
    const framebuffer_p_t *framebuffer_protocol;
    // Target buffer info in raw buffer mode (framebuffer_protocol == NULL).
    mp_buffer_info_t bufinfo;
    // The video source being played, or None.
    mp_obj_t source;
    // Destination position of the video within the target.
    mp_int_t x;
    mp_int_t y;
    mp_int_t source_width;
    mp_int_t source_height;
    mp_int_t source_bits_per_pixel;
    size_t source_frame_bytes;
    // Base of the destination the source last accepted, or NULL when there
    // is none.
    const void *stream_base;
    // Set from interrupt context when the source lands a frame in the
    // destination; cleared by the refresh pass once the new frame has been
    // rendered into the display.
    volatile bool frame_ready;
    // Screen area of frames that landed where the video used to be: the frame
    // in flight when the destination moves or the source pauses still lands
    // at the old destination, possibly after that area was repainted. Grown
    // by on_frame in interrupt context; the refresh moves it into
    // ephemeral_dirty_area with interrupts disabled.
    displayio_area_t vacated_area;
    // Framebuffer geometry cached for on_frame, which can't call the
    // framebuffer protocol from interrupt context. fb_pixels is NULL for
    // plain buffer targets.
    const uint8_t *fb_pixels;
    size_t fb_pixels_len;
    size_t fb_row_stride;
    mp_int_t fb_width;
    // displayio draw protocol state: the rect currently rendered (in screen
    // coordinates) and the rect that changed since the last refresh.
    displayio_area_t current_area;
    displayio_area_t ephemeral_dirty_area;
    displayio_buffer_transform_t absolute_transform;
    bool in_group;
    // Set by the group when hidden; while hidden the video doesn't show and
    // a framebuffer target's source is paused (it is revalidated on unhide).
    bool hidden;
    // True when the source's sizing no longer matches what play() captured:
    // the video won't show until stop and play again.
    bool stale_size;
    bool playing;
    // True while a framebuffer target isn't shown, so its source is paused.
    // Tracked while stopped too, so play() can start the source paused.
    // Plain buffer targets are never paused.
    bool paused;
} videocore_videobuffer_obj_t;

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer);
void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source);
void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y);
void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self);
// Stop without raising (from the finaliser). A failed stop is best-effort;
// the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self);
bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self);
mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self);

// Draw protocol that lets a VideoBuffer be added to a displayio Group.
// displayio copies plain-buffer frames into the display; framebuffer-target
// frames are already in place, so displayio only marks their rows.
extern const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol;
