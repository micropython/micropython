// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/__init__.h"

extern const mp_obj_type_t mipicsi_camera_type;

// Record the output format (possibly the sensor's). No hardware is created
// here: that happens in play().
void common_hal_mipicsi_camera_construct(
    mipicsi_camera_obj_t *self,
    mipicsi_pixel_format_t output_format);

// Remember the sensor. Its mode is read at play time.
void common_hal_mipicsi_camera_set_sensor(mipicsi_camera_obj_t *self, mp_obj_t sensor);
// The sensor passed to set_sensor. Only valid while not deinited.
mp_obj_t common_hal_mipicsi_camera_get_sensor(mipicsi_camera_obj_t *self);

// Videosource protocol play step: create the receiver hardware for the
// sensor's current mode (params) and start the sensor. Raises if the port
// can't convert params->pixel_format to the output format. When paused is
// true, no frames are captured until resume().
void common_hal_mipicsi_camera_videosource_play(
    mipicsi_camera_obj_t *self,
    const mipicsi_sensor_params_t *params,
    bool paused);

// Videosource protocol stop step: tear down the receiver and stop the
// sensor. May raise.
void common_hal_mipicsi_camera_videosource_stop(mipicsi_camera_obj_t *self);

// Videosource protocol pause step: stop capturing frames but keep the
// receiver, ISP and sensor running.
void common_hal_mipicsi_camera_videosource_pause(mipicsi_camera_obj_t *self);

// Videosource protocol resume step: capture again into the latest
// destination. May raise.
void common_hal_mipicsi_camera_videosource_resume(mipicsi_camera_obj_t *self);

void common_hal_mipicsi_camera_deinit(mipicsi_camera_obj_t *self);
bool common_hal_mipicsi_camera_deinited(mipicsi_camera_obj_t *self);

// Stream frames contiguously into base. base must be cache-line aligned and
// stride must equal the frame's row size; the caller checks the fit.
// Returns false, rather than raising, when the destination isn't usable.
bool common_hal_mipicsi_camera_set_destination(
    mipicsi_camera_obj_t *self,
    uint8_t *base,
    size_t stride,
    void (*on_frame)(void *ctx, const void *frame),
    void *ctx);

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_pixel_format(mipicsi_camera_obj_t *self);
mipicsi_pixel_format_t common_hal_mipicsi_camera_get_output_format(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_width(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_height(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_bits_per_pixel(mipicsi_camera_obj_t *self);
