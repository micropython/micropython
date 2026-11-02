// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "esp_cam_ctlr.h"
#include "esp_ldo_regulator.h"

#if SOC_ISP_SUPPORTED
#include "driver/isp.h"
#endif

#include "shared-bindings/mipicsi/__init__.h"

typedef struct mipicsi_camera_obj mipicsi_camera_obj_t;

// Sensor parameters used to build the receiver. Read off the sensor object
// when a stream starts (the sensor is free to reconfigure between streams).
typedef struct {
    mipicsi_pixel_format_t pixel_format;
    mp_int_t width;
    mp_int_t height;
    mp_int_t bayer_order;
    mp_int_t data_lanes;
    mp_int_t lane_bit_rate_mbps;
} mipicsi_sensor_params_t;

struct mipicsi_camera_obj {
    mp_obj_base_t base;
    esp_cam_ctlr_handle_t handle;

    // Power for the MIPI D-PHY, shared with the DSI transmitter. Acquired
    // when a stream starts and released when it (or the camera) ends.
    esp_ldo_channel_handle_t phy_ldo;

    // Camera sensor to receive from, or None once deinitialised.
    mp_obj_t sensor;

    mipicsi_pixel_format_t output_format;
    #if SOC_ISP_SUPPORTED
    // Converts sensor frames to output_format when they differ. Only exists
    // while playing.
    isp_proc_handle_t isp_proc;
    #endif

    // Streaming destination, set by videocore.VideoBuffer. The CSI controller
    // only runs while dest_base is set. Frames are DMA'd contiguously into
    // dest_base, which must be cache-line aligned. dest_on_frame(dest_ctx,
    // frame) is called from the ISR with where each frame landed; a frame in
    // flight when the destination changes lands at the previous one.
    volatile void *dest_base;
    void (*dest_on_frame)(void *ctx, const void *frame);
    void *dest_ctx;
    bool ctlr_started;
    // True between pause and resume: only the CSI controller is stopped.
    // set_destination() still updates where resume() starts.
    bool paused;

    portMUX_TYPE spinlock;

    mp_int_t width;
    mp_int_t height;
    mipicsi_pixel_format_t pixel_format;
    mp_int_t data_lanes;
    mp_int_t lane_bit_rate_mbps;
    // Bayer element order of the sensor's RAW output, only used when the ISP
    // converts from a RAW format.
    mp_int_t bayer_order;
    size_t buffer_len; // Length handed to the driver, includes cache-alignment padding.
};
