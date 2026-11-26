/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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
#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_UVC_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_UVC_H

#include "mp_usbh.h"

#if MICROPY_HW_USB_HOST && MICROPY_HW_USB_HOST_UVC

// Frames are received into a set of buffers: one being filled by the IRQ, one holding
// the latest complete frame, and one handed out to the caller.
#define MP_USBH_UVC_NUM_BUFS (3)

// A frame size supported by one of the camera's formats.
typedef struct {
    char fourcc[4];            // eg "MJPG", "YUY2", "NV12" or "H264"
    uint8_t format_index;
    uint8_t frame_index;
    uint8_t bits_per_pixel;    // for uncompressed formats, otherwise 0
    bool is_default;           // the format's default frame size
    uint16_t width;
    uint16_t height;
    uint32_t default_interval; // in 100ns units
    uint8_t interval_type;     // 0 for a continuous range (min, max, step), else the number of intervals
    const uint8_t *intervals;  // little-endian uint32 values, in 100ns units
    uint32_t max_frame_size;   // 0 if not given
} mp_usbh_uvc_frame_t;

typedef struct {
    uint16_t pos;
    bool in_vs;
    uint8_t frame_subtype;
    uint8_t default_frame_index;
    mp_usbh_uvc_frame_t frame;
} mp_usbh_uvc_iter_t;

// Streaming parameters agreed with the camera.
typedef struct {
    uint32_t interval;
    uint32_t max_frame_size;
    uint32_t max_payload_size;
} mp_usbh_uvc_commit_t;

typedef struct {
    uint32_t frames;     // complete frames received
    uint32_t dropped;    // frames that were corrupt or incomplete
    uint32_t errors;     // (micro)frames lost to bus errors
    uint32_t overflows;  // frames that didn't fit in a buffer
    uint8_t alt_setting; // alternate setting of the streaming interface
    uint16_t xfer_size;  // bytes per (micro)frame of that alternate setting
} mp_usbh_uvc_stats_t;

// Functions returning int give 0, or a negative MP_Exxx error code.

// The address of the camera, or 0 if there isn't one.
uint8_t mp_usbh_uvc_daddr(void);

// Iterate over the frame sizes of all the formats the camera supports.
void mp_usbh_uvc_iter_init(mp_usbh_uvc_iter_t *iter);
const mp_usbh_uvc_frame_t *mp_usbh_uvc_iter_next(mp_usbh_uvc_iter_t *iter);

// Agree the streaming parameters with the camera (the UVC probe and commit).
int mp_usbh_uvc_negotiate(const mp_usbh_uvc_frame_t *frame, uint32_t interval, mp_usbh_uvc_commit_t *commit);

// Start streaming the negotiated format into buffers of bufsize bytes, which are
// allocated on the heap and kept until streaming stops.
int mp_usbh_uvc_stream_start(const mp_usbh_uvc_frame_t *frame, const mp_usbh_uvc_commit_t *commit, size_t bufsize);
void mp_usbh_uvc_stream_stop(void);
bool mp_usbh_uvc_is_streaming(void);

// Take the latest frame if it's new, which stays valid until the next call or until
// streaming stops.
bool mp_usbh_uvc_frame_take(uint8_t **buf, size_t *len);

void mp_usbh_uvc_get_stats(mp_usbh_uvc_stats_t *stats);

// Camera controls such as focus and exposure, by index.  mp_usbh_uvc_control_name()
// returns NULL past the last one, and request is one of the GET requests below.
#define MP_USBH_UVC_GET_CUR (0x81)
#define MP_USBH_UVC_GET_MIN (0x82)
#define MP_USBH_UVC_GET_MAX (0x83)
#define MP_USBH_UVC_GET_RES (0x84)
#define MP_USBH_UVC_GET_DEF (0x87)
const char *mp_usbh_uvc_control_name(size_t idx);
bool mp_usbh_uvc_control_available(size_t idx);
int mp_usbh_uvc_control_get(size_t idx, uint8_t request, int32_t *value);
int mp_usbh_uvc_control_set(size_t idx, int32_t value);

#endif // MICROPY_HW_USB_HOST && MICROPY_HW_USB_HOST_UVC

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_UVC_H
