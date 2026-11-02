// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/videocore/__init__.h"
#include "shared-module/videocore/VideoBuffer.h"

const videosource_p_t *videosource_check(mp_obj_t obj) {
    return (const videosource_p_t *)mp_proto_get_or_throw(MP_QSTR_protocol_videosource, obj);
}

void videosource_play(mp_obj_t source, bool paused) {
    const videosource_p_t *protocol = videosource_check(source);
    if (protocol->play != NULL) {
        protocol->play(source, paused);
    }
}

void videosource_stop(mp_obj_t source) {
    const videosource_p_t *protocol = videosource_check(source);
    if (protocol->stop != NULL) {
        protocol->stop(source);
    }
}

void videosource_pause(mp_obj_t source) {
    videosource_check(source)->pause(source);
}

void videosource_resume(mp_obj_t source) {
    videosource_check(source)->resume(source);
}

bool videosource_set_destination(mp_obj_t source, const videocore_destination_t *dest,
    videocore_on_frame_fun on_frame, void *ctx) {
    return videosource_check(source)->set_destination(source, dest, on_frame, ctx);
}

mp_int_t videosource_get_width(mp_obj_t source) {
    return videosource_check(source)->get_width(source);
}

mp_int_t videosource_get_height(mp_obj_t source) {
    return videosource_check(source)->get_height(source);
}

mp_int_t videosource_get_bits_per_pixel(mp_obj_t source) {
    return videosource_check(source)->get_bits_per_pixel(source);
}
