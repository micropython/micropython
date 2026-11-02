// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

// Destination for a video stream. The caller checks that the whole frame
// fits before handing it to a source.
typedef struct {
    // First pixel of the destination.
    uint8_t *base;
    // Destination row stride, in bytes.
    size_t stride;
    // Source frame row size, in bytes.
    size_t row_bytes;
    // Source frame height, in rows.
    size_t height;
} videocore_destination_t;

// Called (in interrupt context) when a frame has landed. frame is the
// destination base it landed at: usually the latest accepted destination, but
// a frame already being written when the destination changed (or when the
// source was paused) still lands at the previous one.
typedef void (*videocore_on_frame_fun)(void *ctx, const void *frame);

// Protocol implemented by video streamers such as mipicsi.Camera. A source
// streams continuously while playing (unless paused); the target sets (and can
// change) where the pixels go by calling set_destination() at any time,
// including between frames.
//
// This is the video equivalent of the audiosample protocol in the audiocore
// module, inverted: instead of the target pulling frames, the source writes
// where it is told to.
typedef struct _videosource_p_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_videosource

    // Start the source's hardware (e.g. start the sensor). Called from
    // VideoBuffer.play, so it may raise, including when the source is
    // already playing. No frames are written before set_destination(). When
    // paused is true, start as if pause() had already been called. May be
    // NULL.
    void (*play)(mp_obj_t self, bool paused);

    // Pause streaming without stopping the camera or releasing its hardware:
    // e.g. stop the CSI controller's DMA while the sensor keeps streaming.
    // No frames are written until resume(). Called when a framebuffer target
    // stops being shown. set_destination() while paused updates where resume()
    // will stream. May raise; VideoBuffer ignores errors since it also pauses
    // during display reset.
    void (*pause)(mp_obj_t self);

    // Resume streaming after pause() into the most recent destination. Called
    // from Python contexts (when the framebuffer target is shown again), so it
    // may raise; the source must stay paused if it does.
    void (*resume)(mp_obj_t self);

    // Direct frames into dest, starting with the next frame. Never raises:
    // returns whether the source accepted dest. on_frame(ctx, frame) is
    // called from interrupt context each time a frame lands, with the base
    // it landed at.
    bool (*set_destination)(mp_obj_t self, const videocore_destination_t *dest,
        videocore_on_frame_fun on_frame, void *ctx);

    // Stop streaming and release the hardware (e.g. stop the sensor so its
    // settings can be changed). Called from VideoBuffer.stop, so it may
    // raise. May be NULL.
    void (*stop)(mp_obj_t self);

    // Width of the frames the source streams, in pixels.
    mp_int_t (*get_width)(mp_obj_t self);
    // Height of the frames the source streams, in lines.
    mp_int_t (*get_height)(mp_obj_t self);
    // Bits per pixel of the frames the source streams.
    mp_int_t (*get_bits_per_pixel)(mp_obj_t self);
} videosource_p_t;

// Protocol dispatch helpers. play and stop are no-ops when the source doesn't
// implement them; the rest are required.
void videosource_play(mp_obj_t source, bool paused);
void videosource_stop(mp_obj_t source);
void videosource_pause(mp_obj_t source);
void videosource_resume(mp_obj_t source);

// Return the videosource protocol of obj or raise TypeError.
const videosource_p_t *videosource_check(mp_obj_t obj);

bool videosource_set_destination(mp_obj_t source, const videocore_destination_t *dest,
    videocore_on_frame_fun on_frame, void *ctx);
mp_int_t videosource_get_width(mp_obj_t source);
mp_int_t videosource_get_height(mp_obj_t source);
mp_int_t videosource_get_bits_per_pixel(mp_obj_t source);

// Raise when the group tree contains a VideoBuffer that streams into a
// framebuffer other than the one this display renders. Called when a group
// is made a display's root group. Implemented in shared-module/videocore/
// VideoBuffer.c.
void videocore_validate_root_group(mp_obj_t root_group, mp_obj_t display);
