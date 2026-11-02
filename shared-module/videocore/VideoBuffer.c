// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/videocore/VideoBuffer.h"

#include "py/runtime.h"

#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/videocore/VideoBuffer.h"
#include "shared-bindings/videocore/__init__.h"
#include "shared-module/displayio/Group.h"
#include "shared-module/videocore/__init__.h"

#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif

#define FB_GETTER(method, default_value) \
    (self->framebuffer_protocol->method \
        ? self->framebuffer_protocol->method(self->framebuffer) \
        : (default_value))

// Add the screen area of a frame that landed at frame to vacated_area. Uses
// only the cached framebuffer geometry since this runs in interrupt context.
// A shifted frame can start mid-row and wrap its rows; then cover the full
// width.
static void videocore_videobuffer_add_vacated_frame(videocore_videobuffer_obj_t *self, const void *frame) {
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const uint8_t *pixels = self->fb_pixels;
    if (pixels == NULL || bytes_per_pixel == 0 || self->fb_row_stride == 0 ||
        (const uint8_t *)frame < pixels) {
        return; // Plain buffer frames are never shown where they land.
    }
    const size_t offset = (const uint8_t *)frame - pixels;
    if (offset >= self->fb_pixels_len) {
        return;
    }
    displayio_area_t area = {
        .x1 = (offset % self->fb_row_stride) / bytes_per_pixel,
        .y1 = offset / self->fb_row_stride,
    };
    area.x2 = area.x1 + self->source_width;
    area.y2 = area.y1 + self->source_height;
    if (area.x2 > self->fb_width) {
        area.x1 = 0;
        area.x2 = self->fb_width;
        area.y2 += 1;
    }
    displayio_area_union(&self->vacated_area, &area, &self->vacated_area);
}

// Called from interrupt context when the source lands a frame, so keep it
// tiny.
static void videocore_videobuffer_on_frame(void *ctx, const void *frame) {
    videocore_videobuffer_obj_t *self = ctx;
    if (frame == self->stream_base) {
        self->frame_ready = true;
        return;
    }
    // It landed where the video used to be (the frame in flight when it moved
    // or paused). Repaint there, since that area may already have been
    // repainted.
    videocore_videobuffer_add_vacated_frame(self, frame);
}

// Cache the framebuffer geometry for on_frame (see fb_pixels).
static void videocore_videobuffer_cache_fb_geometry(videocore_videobuffer_obj_t *self,
    const mp_buffer_info_t *bufinfo, mp_int_t fb_width, mp_int_t row_stride, mp_int_t first_pixel_offset) {
    self->fb_pixels = (const uint8_t *)bufinfo->buf + first_pixel_offset;
    self->fb_pixels_len = bufinfo->len - first_pixel_offset;
    self->fb_row_stride = row_stride;
    self->fb_width = fb_width;
}

// Add the currently rendered rect to the dirty area so the next refresh
// repaints what a move leaves behind. Accumulates across moves. Does nothing
// while the video isn't shown.
static void videocore_videobuffer_dirty_current_area(videocore_videobuffer_obj_t *self) {
    if (self->absolute_transform.scale == 0 || self->hidden) {
        return;
    }
    displayio_area_union(
        &self->ephemeral_dirty_area, &self->current_area, &self->ephemeral_dirty_area);
}

// Recompute the rendered rect (screen coordinates) of the video from the
// x/y position and the absolute transform. Video only supports scale 1 with
// no mirroring (validated in update_transform), so the rect is the frame
// size translated into place.
static void videocore_videobuffer_update_area(videocore_videobuffer_obj_t *self) {
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    self->current_area.x1 = self->x + t->x;
    self->current_area.y1 = self->y + t->y;
    self->current_area.x2 = self->current_area.x1 + self->source_width;
    self->current_area.y2 = self->current_area.y1 + self->source_height;
    self->current_area.next = NULL;
}

// Compute the destination for the current position, including a containing
// group's translation. Raises unless the whole frame fits in the target,
// since sources write contiguous frames. move_dest() is the non-raising
// variant used from displayio.
static void videocore_videobuffer_compute_destination(
    videocore_videobuffer_obj_t *self, videocore_destination_t *dest) {
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const size_t row_bytes = (size_t)self->source_width * bytes_per_pixel;

    if (self->framebuffer_protocol == NULL) {
        // Raw buffer mode: frames are written compactly from the start.
        dest->base = self->bufinfo.buf;
        dest->stride = row_bytes;
        dest->row_bytes = row_bytes;
        dest->height = self->source_height;
        if (self->bufinfo.len < self->source_frame_bytes) {
            mp_raise_ValueError(MP_ERROR_TEXT("Buffer too small"));
        }
        return;
    }

    mp_buffer_info_t bufinfo;
    self->framebuffer_protocol->get_bufinfo(self->framebuffer, &bufinfo);
    if (bufinfo.buf == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("framebuffer went away"));
    }

    const mp_int_t fb_width = FB_GETTER(get_width, 0);
    const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
    const mp_int_t first_pixel_offset = FB_GETTER(get_first_pixel_offset, 0);

    // The absolute position includes a containing group's translation. The
    // shift may land the frame's rows mid-row: the image won't look right,
    // but the contiguous DMA stays contiguous. Only the frame's start and
    // end must remain within the buffer.
    const displayio_buffer_transform_t *t = &self->absolute_transform;
    const mp_int_t shift_x = self->x + t->x;
    const mp_int_t shift_y = self->y + t->y;
    if (self->x < 0 || self->y < 0 || shift_x < 0 || shift_y < 0 ||
        first_pixel_offset + shift_y * row_stride + shift_x * bytes_per_pixel +
        (mp_int_t)self->source_frame_bytes >
        bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("video doesn't fit in framebuffer"));
    }
    videocore_videobuffer_cache_fb_geometry(self, &bufinfo, fb_width, row_stride, first_pixel_offset);

    dest->base = (uint8_t *)bufinfo.buf + first_pixel_offset +
        shift_y * row_stride + shift_x * bytes_per_pixel;
    dest->stride = row_stride;
    dest->row_bytes = row_bytes;
    dest->height = self->source_height;
}

// Move the framebuffer destination for a new absolute position, capping the
// shift so the frame stays completely within the framebuffer. Best effort:
// never raises, because this runs from displayio's refresh.
static void videocore_videobuffer_move_dest(
    videocore_videobuffer_obj_t *self, mp_int_t shift_x, mp_int_t shift_y) {
    if (!self->playing || self->paused) {
        return;
    }
    mp_buffer_info_t bufinfo;
    self->framebuffer_protocol->get_bufinfo(self->framebuffer, &bufinfo);
    if (bufinfo.buf == NULL) {
        return;
    }
    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const size_t row_bytes = (size_t)self->source_width * bytes_per_pixel;
    const size_t frame_bytes = (size_t)self->source_height * row_bytes;
    const mp_int_t fb_width = FB_GETTER(get_width, 0);
    const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
    const mp_int_t first_pixel_offset = FB_GETTER(get_first_pixel_offset, 0);

    if (shift_x < 0) {
        shift_x = 0;
    }
    if (shift_y < 0) {
        shift_y = 0;
    }
    // Cap the start and the end. Cap the rows first: enough rows must fit
    // above the frame for it to fit at all.
    const mp_int_t max_end = (mp_int_t)bufinfo.len - first_pixel_offset - (mp_int_t)frame_bytes;
    if (shift_y > max_end / row_stride) {
        shift_y = max_end / row_stride;
    }
    if (shift_y < 0) {
        shift_y = 0;
    }
    const mp_int_t max_x = (max_end - shift_y * row_stride) / bytes_per_pixel;
    if (shift_x > max_x) {
        shift_x = max_x;
    }
    if (shift_x < 0) {
        shift_x = 0;
    }
    videocore_videobuffer_cache_fb_geometry(self, &bufinfo, fb_width, row_stride, first_pixel_offset);

    videocore_destination_t dest = {
        .base = bufinfo.buf + first_pixel_offset + shift_y * row_stride + shift_x * bytes_per_pixel,
        .stride = row_stride,
        .row_bytes = row_bytes,
        .height = self->source_height,
    };
    // A rejected destination leaves the stream where it was.
    if (videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        self->stream_base = dest.base;
    }
}

void common_hal_videocore_videobuffer_construct(videocore_videobuffer_obj_t *self, mp_obj_t framebuffer) {
    // Initialise what the finaliser uses before anything can raise.
    self->source = mp_const_none;
    self->stream_base = NULL;
    self->playing = false;
    self->framebuffer = framebuffer;
    self->framebuffer_protocol = mp_proto_get(MP_QSTR_protocol_framebuffer, framebuffer);
    if (self->framebuffer_protocol != NULL) {
        if (self->framebuffer_protocol->get_bufinfo == NULL ||
            self->framebuffer_protocol->get_width == NULL ||
            self->framebuffer_protocol->get_height == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("framebuffer missing required functions"));
        }
    } else if (!mp_get_buffer(framebuffer, &self->bufinfo, MP_BUFFER_WRITE)) {
        // Not a framebuffer and not a writable buffer such as a bytearray or
        // memoryview.
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
            MP_QSTR_framebuffer, MP_QSTR_buffer, MP_QSTR_framebuffer,
            mp_obj_get_type(framebuffer)->name);
    }

    self->x = 0;
    self->y = 0;
    self->frame_ready = false;
    self->fb_pixels = NULL;
    self->fb_pixels_len = 0;
    self->fb_row_stride = 0;
    self->fb_width = 0;
    self->in_group = false;
    self->hidden = false;
    self->stale_size = false;
    displayio_area_t empty = {};
    self->current_area = empty;
    self->ephemeral_dirty_area = empty;
    self->vacated_area = empty;
    displayio_buffer_transform_t empty_transform = {};
    self->absolute_transform = empty_transform;
    // Nothing is shown until a Group attached to a display updates our
    // transform.
    self->paused = true;
}

// Start the source's hardware and record its frame format, which is fixed
// until stop. The source has no destination yet, so it doesn't write frames.
// When paused is true it starts paused. May raise.
static void videocore_videobuffer_start_source(videocore_videobuffer_obj_t *self, bool paused) {
    mp_obj_t source = self->source;
    const videosource_p_t *protocol = videosource_check(source);

    videosource_play(source, paused);

    mp_int_t bits_per_pixel = protocol->get_bits_per_pixel(source);
    if (bits_per_pixel % 8 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("video format must be byte aligned"));
    }
    mp_int_t width = protocol->get_width(source);
    mp_int_t height = protocol->get_height(source);
    size_t frame_bytes = (size_t)((width * height * bits_per_pixel) / 8);

    if (self->framebuffer_protocol != NULL) {
        mp_int_t color_depth = FB_GETTER(get_color_depth, 16);
        if (color_depth % 8 != 0 || bits_per_pixel != color_depth) {
            mp_raise_ValueError(MP_ERROR_TEXT("video format doesn't match framebuffer"));
        }
    }

    self->source_width = width;
    self->source_height = height;
    self->source_bits_per_pixel = bits_per_pixel;
    self->source_frame_bytes = frame_bytes;

    if (self->framebuffer_protocol == NULL && (mp_int_t)self->bufinfo.len < (mp_int_t)frame_bytes) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer too small"));
    }
}

// Point a started source at the current destination and let it stream. Runs
// from Python contexts (play and draw_update_transform) so it may raise.
static void videocore_videobuffer_start_stream(videocore_videobuffer_obj_t *self) {
    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);

    // The source streams from now on; on_frame reports each frame.
    if (!videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("video source rejected the destination"));
    }
    self->stream_base = dest.base;
    self->paused = false;
}

// Un-show a framebuffer target by pausing its source. The play session and
// the source's hardware stay alive, but no frames are written to the target.
// Best effort: this also runs during display reset, so errors are ignored.
static void videocore_videobuffer_pause_stream(videocore_videobuffer_obj_t *self) {
    if (self->paused || self->source == mp_const_none) {
        return; // Already paused, or nothing has been started yet.
    }
    // Drop the destination before pausing the source: the source finishes its
    // pause at the next frame boundary, so the frame being written still
    // lands after this, and on_frame adds it to vacated_area to repaint over.
    self->stream_base = NULL;
    self->paused = true;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        videosource_pause(self->source);
        nlr_pop();
    }
    self->frame_ready = false;
}

// Re-show a framebuffer target by pointing its paused source at the current
// destination and resuming it. Position changes while hidden are applied here;
// the source's format was fixed at play(), so reject a changed source rather
// than resuming into a wrong-sized frame.
static void videocore_videobuffer_resume_stream(videocore_videobuffer_obj_t *self) {
    const videosource_p_t *protocol = videosource_check(self->source);
    if (protocol->get_width(self->source) != self->source_width ||
        protocol->get_height(self->source) != self->source_height ||
        protocol->get_bits_per_pixel(self->source) != self->source_bits_per_pixel) {
        self->stale_size = true;
        return;
    }

    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);
    if (!videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("video source rejected the destination"));
    }
    videosource_resume(self->source);
    self->stream_base = dest.base;
    self->paused = false;
    // Wait for the first frame at the new destination.
    self->frame_ready = false;
}

// Start, resume or pause a framebuffer target's source to match whether it is
// shown (on a display and not hidden). While hidden the source stays active
// but paused, so showing it again is quick.
static void videocore_videobuffer_update_stream(videocore_videobuffer_obj_t *self) {
    if (self->framebuffer_protocol == NULL) {
        return; // Plain buffer targets always stream.
    }
    const bool shown = self->absolute_transform.scale != 0 && !self->hidden;
    if (!self->playing) {
        // Remember whether the target is shown so play() can start the source
        // in the right state.
        self->paused = !shown;
        return;
    }
    if (shown && !self->stale_size) {
        if (self->paused) {
            videocore_videobuffer_resume_stream(self);
        }
    } else if (!shown && !self->paused) {
        videocore_videobuffer_pause_stream(self);
    }
}

void common_hal_videocore_videobuffer_play(videocore_videobuffer_obj_t *self, mp_obj_t source) {
    if (self->playing) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }
    videosource_check(source);
    self->source = source;
    self->stale_size = false;

    // A framebuffer target only streams while shown. When it isn't, start the
    // source paused rather than pausing it afterwards, so no frame slips out
    // in between. Plain buffer targets always stream.
    const bool start_paused = self->framebuffer_protocol != NULL && self->paused;
    videocore_videobuffer_start_source(self, start_paused);
    self->playing = true;
    if (start_paused) {
        // No destination is set until the VideoBuffer is shown, so the source
        // has nowhere to put a frame while paused.
        self->frame_ready = false;
        self->stream_base = NULL;
    } else {
        videocore_videobuffer_start_stream(self);
    }
}

void common_hal_videocore_videobuffer_set_position(videocore_videobuffer_obj_t *self, mp_int_t x, mp_int_t y) {
    if (self->framebuffer_protocol != NULL && x != 0) {
        // The source DMAs whole, contiguous frames with no stride, so every
        // frame row must land on a framebuffer row: the video's x must be 0.
        mp_raise_ValueError(MP_ERROR_TEXT("video in a framebuffer must be at x == 0"));
    }
    // Repaint where the video used to be. Union (rather than overwrite) so a
    // move before the previous one was refreshed isn't lost.
    videocore_videobuffer_dirty_current_area(self);
    self->x = x;
    self->y = y;
    videocore_videobuffer_update_area(self);
    if (!self->playing || self->paused || self->framebuffer_protocol == NULL) {
        // Raw buffer targets are rendered by displayio; their capture
        // destination doesn't move with the position. A paused framebuffer
        // target has no destination to move yet.
        return;
    }
    // The source's frame size was fixed when play() was called. Reject
    // sources that changed it (e.g. a sensor mode change) rather than
    // blitting at stale geometry.
    const videosource_p_t *protocol = videosource_check(self->source);
    mp_int_t width = protocol->get_width(self->source);
    mp_int_t height = protocol->get_height(self->source);
    mp_int_t bits_per_pixel = protocol->get_bits_per_pixel(self->source);
    if (width != self->source_width || height != self->source_height ||
        bits_per_pixel != self->source_bits_per_pixel) {
        mp_raise_ValueError(MP_ERROR_TEXT("video size changed since play; stop and play again"));
    }
    // The source picks up the new destination on its next frame. A rejected
    // destination leaves the stream where it was.
    videocore_destination_t dest;
    videocore_videobuffer_compute_destination(self, &dest);
    if (videosource_set_destination(self->source, &dest, videocore_videobuffer_on_frame, self)) {
        self->stream_base = dest.base;
    }
}

static void videocore_videobuffer_clear(videocore_videobuffer_obj_t *self) {
    self->playing = false;
    self->frame_ready = false;
    self->stream_base = NULL;
    self->source = mp_const_none;
}

void common_hal_videocore_videobuffer_stop(videocore_videobuffer_obj_t *self) {
    // Stop the source; the camera also stops the sensor so its settings can
    // be changed. That may raise: surface it, after cleaning up, since the
    // stream did stop.
    if (self->source != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            videosource_stop(self->source);
            nlr_pop();
        } else {
            videocore_videobuffer_clear(self);
            nlr_raise(nlr.ret_val);
        }
    }
    videocore_videobuffer_clear(self);
}

// Stop without raising: runs from background contexts (reload). A failed
// stop is best-effort; the source's own finaliser stops it as well.
void videocore_videobuffer_stop_quiet(videocore_videobuffer_obj_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (self->source != mp_const_none) {
            videosource_stop(self->source);
        }
        nlr_pop();
    }
    videocore_videobuffer_clear(self);
}

bool common_hal_videocore_videobuffer_get_playing(videocore_videobuffer_obj_t *self) {
    return self->playing;
}

mp_obj_t common_hal_videocore_videobuffer_get_source(videocore_videobuffer_obj_t *self) {
    return self->source;
}

// Draw protocol implementation. displayio copies plain-buffer frames into the
// display. Framebuffer-target frames are already in place; fill_area only
// copies them into displayio's scratch buffer when other layers overlap.

static mp_obj_t videocore_videobuffer_draw_protocol_self(mp_obj_t protocol_container) {
    return protocol_container;
}

static bool videocore_videobuffer_draw_fill_area(mp_obj_t self_in, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // buffer isn't necessarily the framebuffer. It may be a scratch buffer.
    if (!self->playing || self->stream_base == NULL || self->hidden) {
        return false;
    }

    displayio_area_t overlap;
    if (!displayio_area_compute_overlap(area, &self->current_area, &overlap)) {
        return false;
    }
    const bool full_coverage = displayio_area_equal(area, &overlap);
    const uint16_t span_px = displayio_area_width(area);
    const size_t overlap_width = displayio_area_width(&overlap);

    if (self->framebuffer_protocol != NULL) {
        // The source already wrote these pixels into the framebuffer. If
        // nothing else covers this area, claim no pixels so the display skips
        // its write-back. Otherwise copy the video's unclaimed pixels into
        // the scratch buffer and claim them, since the scratch buffer is
        // written back over the framebuffer.
        const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
        const mp_int_t fb_width = FB_GETTER(get_width, 0);
        const mp_int_t row_stride = FB_GETTER(get_row_stride, fb_width * self->source_bits_per_pixel / 8);
        const uint8_t *frame = (const uint8_t *)self->stream_base;
        uint8_t *out = (uint8_t *)buffer;

        // Check whole words. This may include pixels between overlap rows or
        // just outside the overlap, which can only make us take the slower
        // per-pixel path below; it never misses a claimed pixel.
        const size_t first_px = (size_t)(overlap.y1 - area->y1) * span_px +
            (overlap.x1 - area->x1);
        const size_t end_px = (size_t)(overlap.y2 - 1 - area->y1) * span_px +
            (overlap.x2 - area->x1);
        bool masked = false;
        for (size_t word = first_px / 32; word <= (end_px - 1) / 32; word++) {
            if (mask[word] != 0) {
                masked = true;
                break;
            }
        }
        if (full_coverage && !masked) {
            // Sole coverer: nothing was composed, so the driver skips the copy.
            return true;
        }

        for (int16_t y = overlap.y1; y < overlap.y2; y++) {
            const size_t px = (size_t)(y - area->y1) * span_px + (overlap.x1 - area->x1);
            const int16_t fy = y - self->current_area.y1;
            const uint8_t *row = frame + (size_t)fy * row_stride +
                (size_t)(overlap.x1 - self->current_area.x1) * bytes_per_pixel;
            size_t i = 0;
            while (i < overlap_width) {
                // Skip pixels a higher layer already claimed or drew.
                while (i < overlap_width && (mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    i++;
                }
                const size_t start = i;
                while (i < overlap_width && !(mask[(px + i) / 32] & (1u << ((px + i) % 32)))) {
                    mask[(px + i) / 32] |= 1u << ((px + i) % 32);
                    i++;
                }
                if (i > start) {
                    memcpy(
                        out + (px + start) * bytes_per_pixel,
                        row + start * bytes_per_pixel,
                        (i - start) * bytes_per_pixel);
                }
            }
        }
        return full_coverage;
    }

    // Raw buffer target: bulk copy, no color conversion: the display depth
    // must match the video's pixel format byte for byte.
    if (colorspace->depth != self->source_bits_per_pixel || colorspace->depth != 16) {
        return false;
    }

    // Opaque: claim the pixels.
    for (int16_t y = overlap.y1; y < overlap.y2; y++) {
        const size_t px = (size_t)(y - area->y1) * span_px + (overlap.x1 - area->x1);
        for (size_t i = 0; i < overlap_width; i++) {
            mask[(px + i) / 32] |= 1u << ((px + i) % 32);
        }
    }

    const size_t bytes_per_pixel = self->source_bits_per_pixel / 8;
    const size_t row_bytes = (size_t)self->source_width * bytes_per_pixel;
    const uint8_t *frame = (const uint8_t *)self->stream_base;

    for (int16_t y = overlap.y1; y < overlap.y2; y++) {
        const size_t px = (size_t)(y - area->y1) * span_px + (overlap.x1 - area->x1);
        const int16_t fy = y - self->current_area.y1;
        memcpy(
            (uint16_t *)buffer + px,
            frame + (size_t)fy * row_bytes + (size_t)(overlap.x1 - self->current_area.x1) * bytes_per_pixel,
            overlap_width * bytes_per_pixel);
    }
    return full_coverage;
}

static bool videocore_videobuffer_draw_get_dirty_area(mp_obj_t self_in, displayio_area_t *out_area) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    out_area->x1 = out_area->x2;
    displayio_area_union(&self->ephemeral_dirty_area, &self->current_area, out_area);
    return true;
}

static void videocore_videobuffer_draw_update_transform(mp_obj_t self_in, displayio_buffer_transform_t *group_transform) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Remember where the video was so a move can repaint the pixels it
    // vacates. The area is only valid while the video is shown; a layer being
    // attached or detached is handled by the display's full refresh.
    displayio_area_t previous_area = self->current_area;
    const bool was_shown = self->absolute_transform.scale != 0 && !self->hidden;
    // A group that isn't attached to a display has no transform, and its
    // update_transform clears the absolute transform we receive. A real
    // (shown) transform always has scale >= 1.
    if (group_transform == NULL || group_transform->scale == 0) {
        displayio_buffer_transform_t no_transform = {};
        self->absolute_transform = no_transform;
    } else {
        // Validate the transform: video frames are bulk copied (raw targets)
        // or DMA'd contiguously (framebuffer targets), so only translation
        // moves.
        if (group_transform->scale != 1 ||
            group_transform->mirror_x || group_transform->mirror_y ||
            group_transform->transpose_xy ||
            group_transform->dx != 1 || group_transform->dy != 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer does not support transforms"));
        }
        self->absolute_transform = *group_transform;
    }

    // A framebuffer target starts or stops with whether it is shown (the
    // absolute transform is known now). This runs from Python contexts, so a
    // (re)start may raise.
    videocore_videobuffer_update_stream(self);

    videocore_videobuffer_update_area(self);

    // Moved by a containing Group (or a transform change): dirty the rect it
    // used to occupy so the refresh repaints those pixels instead of leaving
    // the previous frame there.
    if (was_shown && self->absolute_transform.scale != 0 &&
        !displayio_area_equal(&previous_area, &self->current_area)) {
        displayio_area_union(
            &self->ephemeral_dirty_area, &previous_area, &self->ephemeral_dirty_area);
    }

    // Framebuffer targets stream straight into the buffer: move the
    // destination along with the shift. Best effort, since this can't raise:
    // the shift may land the start mid-row, which won't look right but is
    // obvious; the frame is capped to stay completely within the buffer.
    if (self->framebuffer_protocol != NULL) {
        videocore_videobuffer_move_dest(
            self,
            self->x + self->absolute_transform.x,
            self->y + self->absolute_transform.y);
    }
}

static void videocore_videobuffer_draw_finish_refresh(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // The frame (or move) rendered; stop flagging it.
    self->frame_ready = false;
    displayio_area_t empty = {};
    self->ephemeral_dirty_area = empty;
}

static displayio_area_t *videocore_videobuffer_draw_get_refresh_areas(mp_obj_t self_in, displayio_area_t *tail) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Frames that landed where the video used to be, possibly over the repaint
    // of that area: repaint there again.
    common_hal_mcu_disable_interrupts();
    displayio_area_t vacated = self->vacated_area;
    displayio_area_t empty = {};
    self->vacated_area = empty;
    common_hal_mcu_enable_interrupts();
    displayio_area_union(&self->ephemeral_dirty_area, &vacated, &self->ephemeral_dirty_area);
    // Always repaint the area the video vacated, even once it is hidden or
    // stopped, so its last frame doesn't stay on the target.
    if (!displayio_area_empty(&self->ephemeral_dirty_area)) {
        // The video moved or was hidden: repaint with the lower layers.
        self->ephemeral_dirty_area.next = tail;
        tail = &self->ephemeral_dirty_area;
    }
    if (!self->playing || self->paused || self->hidden || self->stale_size) {
        return tail;
    }
    if (self->frame_ready) {
        // New frame: bulk copy it in.
        self->current_area.next = tail;
        tail = &self->current_area;
    }
    return tail;
}

static void videocore_videobuffer_draw_set_dirty(mp_obj_t self_in) {
    (void)self_in;
}

static void videocore_videobuffer_draw_set_hidden(mp_obj_t self_in, bool hidden) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->hidden == hidden) {
        return;
    }
    self->hidden = hidden;
    // Repaint what the video showed or vacated.
    self->ephemeral_dirty_area = self->current_area;
    if (!hidden && self->playing) {
        // The source normally can't change while the sensor stays streaming,
        // but validate its sizing again in case it did: if it no longer
        // matches, don't raise; simply stop showing the stream until stop and
        // play again.
        const videosource_p_t *protocol = mp_proto_get(MP_QSTR_protocol_videosource, self->source);
        if (protocol != NULL &&
            (protocol->get_width(self->source) != self->source_width ||
             protocol->get_height(self->source) != self->source_height ||
             protocol->get_bits_per_pixel(self->source) != self->source_bits_per_pixel)) {
            self->stale_size = true;
        }
    }
    // Hiding or unhiding a framebuffer target pauses or resumes its source.
    videocore_videobuffer_update_stream(self);
}

static bool videocore_videobuffer_draw_set_in_group(mp_obj_t self_in, bool in_group) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

static vectorio_draw_protocol_impl_t videocore_videobuffer_draw_protocol_impl = {
    .draw_fill_area = videocore_videobuffer_draw_fill_area,
    .draw_get_dirty_area = videocore_videobuffer_draw_get_dirty_area,
    .draw_update_transform = videocore_videobuffer_draw_update_transform,
    .draw_finish_refresh = videocore_videobuffer_draw_finish_refresh,
    .draw_get_refresh_areas = videocore_videobuffer_draw_get_refresh_areas,
    .draw_set_dirty = videocore_videobuffer_draw_set_dirty,
    .draw_set_hidden = videocore_videobuffer_draw_set_hidden,
    .draw_set_in_group = videocore_videobuffer_draw_set_in_group,
};

const vectorio_draw_protocol_t videocore_videobuffer_draw_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_draw)
    .draw_get_protocol_self = videocore_videobuffer_draw_protocol_self,
    .draw_protocol_impl = &videocore_videobuffer_draw_protocol_impl,
};

// Root group validation: a VideoBuffer that streams straight into a
// framebuffer bypasses displayio rendering, so it may only be shown on a
// display that renders that same framebuffer. Plain buffer targets are drawn
// by displayio and are fine anywhere. Walks the group tree and raises when a
// VideoBuffer doesn't match the display it would render on.
static void videocore_validate_member(mp_obj_t member, mp_obj_t display) {
    displayio_group_t *group = mp_obj_cast_to_native_base(member, &displayio_group_type);
    if (group != MP_OBJ_NULL) {
        for (size_t i = 0; i < group->members->len; i++) {
            videocore_validate_member(group->members->items[i], display);
        }
        return;
    }
    const vectorio_draw_protocol_t *draw_protocol = mp_proto_get(MP_QSTR_protocol_draw, member);
    if (draw_protocol == NULL) {
        return;
    }
    mp_obj_t layer = draw_protocol->draw_get_protocol_self(member);
    if (mp_obj_get_type(layer) != &videocore_videobuffer_type) {
        return;
    }
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(layer);
    if (self->framebuffer_protocol == NULL || display == self->framebuffer) {
        return;
    }
    #if CIRCUITPY_FRAMEBUFFERIO
    // A FramebufferDisplay renders the framebuffer it wraps. That is the only
    // kind of display a framebuffer-target VideoBuffer can be shown on.
    mp_obj_t native_display = mp_obj_cast_to_native_base(display, &framebufferio_framebufferdisplay_type);
    if (native_display != MP_OBJ_NULL) {
        framebufferio_framebufferdisplay_obj_t *fd = MP_OBJ_TO_PTR(native_display);
        if (fd->framebuffer == self->framebuffer) {
            return;
        }
    }
    #endif
    mp_raise_ValueError(MP_ERROR_TEXT("VideoBuffer is playing into a different framebuffer"));
}

void videocore_validate_root_group(mp_obj_t root_group, mp_obj_t display) {
    videocore_validate_member(root_group, display);
}
