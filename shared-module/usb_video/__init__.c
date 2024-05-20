// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 by Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include <stdint.h>
#include "class/video/video_device.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/usb_video/__init__.h"
#include "shared-module/bitmapfilter/macros.h"
#include "shared-module/usb_video/__init__.h"
#include "shared-module/usb_video/uvc_usb_descriptors.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/tick.h"
#include "device/usbd.h"

static bool do_convert = true;
static unsigned frame_num = 0;
static unsigned tx_busy = 0;
static unsigned interval_ms = 1000 / DEFAULT_FRAME_RATE;

// TODO must dynamically allocate this, otherwise everyone pays for it
static uint8_t *frame_buffer_yuyv;
uint16_t *usb_video_framebuffer_rgb565;

static bool usb_video_is_enabled = false;
uint16_t usb_video_frame_width, usb_video_frame_height;

bool shared_module_usb_video_enable(mp_int_t frame_width, mp_int_t frame_height) {
    if (tud_connected()) {
        return false;
    }

    // this will free any previously allocated framebuffer as a side-effect
    shared_module_usb_video_disable();

    if (frame_width & 1) {
        // frame_width must be even, round it up
        frame_width++;
    }

    usb_video_frame_width = frame_width;
    usb_video_frame_height = frame_height;

    size_t framebuffer_size = usb_video_frame_width * usb_video_frame_height * 2;
    frame_buffer_yuyv = port_malloc(framebuffer_size, false);
    uint32_t *frame_buffer_rgb565_uint32 = port_malloc(framebuffer_size, false);
    usb_video_framebuffer_rgb565 = (uint16_t *)frame_buffer_rgb565_uint32;

    if (!frame_buffer_yuyv || !usb_video_framebuffer_rgb565) {
        // this will free either of the buffers allocated just above, in
        // case one succeeded and the other failed.
        shared_module_usb_video_disable();
        m_malloc_fail(2 * framebuffer_size);
    }
    memset(frame_buffer_yuyv, 0, framebuffer_size);
    memset(usb_video_framebuffer_rgb565, 0, framebuffer_size);

    usb_video_is_enabled = true;

    return true;
}

bool shared_module_usb_video_disable(void) {
    if (tud_connected()) {
        return false;
    }
    usb_video_is_enabled = false;
    port_free(frame_buffer_yuyv);
    port_free(usb_video_framebuffer_rgb565);
    frame_buffer_yuyv = NULL;
    usb_video_framebuffer_rgb565 = NULL;
    return true;
}

bool usb_video_enabled(void) {
    return usb_video_is_enabled;
}

size_t usb_video_descriptor_length(void) {
    #if CFG_TUD_VIDEO_STREAMING_BULK
    return sizeof((char[]) {TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR_BULK(0, 0, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT, DEFAULT_FRAME_RATE, CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE, 0, 0)});
    #else
    return sizeof((char[]) {TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(0, 0, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT, DEFAULT_FRAME_RATE, 64, 0, 0)});
    #endif
}

static void convert_framebuffer_maybe(void) {
    if (!do_convert) {
        return; // new data not ready yet
    }
    do_convert = false; // assumes this happens via background, not interrupt

    uint8_t *dest = frame_buffer_yuyv;
    uint16_t *src = usb_video_framebuffer_rgb565;

    for (int i = 0; i < usb_video_frame_width * usb_video_frame_height / 2; i++) {
        uint16_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(src, 0);
        uint16_t p2 = IMAGE_GET_RGB565_PIXEL_FAST(src, 1);
        src += 2;

        int y1 = COLOR_RGB565_TO_Y(p1);
        int y2 = COLOR_RGB565_TO_Y(p2);
        if (y2 > y1) {
            p1 = p2;          /* Use UV value of the brighter pixel */
        }
        int u = COLOR_RGB565_TO_U(p1) + 128;  // openmv UV are signed in the range [-127,128]
        int v = COLOR_RGB565_TO_V(p1) + 128;

        *dest++ = y1;
        *dest++ = u;
        *dest++ = y2;
        *dest++ = v;
    }
}

void shared_module_usb_video_swapbuffers(void) {
    do_convert = true;
}

size_t usb_video_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    usb_add_interface_string(*current_interface_string, "CircuitPython UVC");
    const uint8_t usb_video_descriptor[] = {
        #if CFG_TUD_VIDEO_STREAMING_BULK
        TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR_BULK(*current_interface_string, descriptor_counts->current_endpoint | 0x80, usb_video_frame_width, usb_video_frame_height, DEFAULT_FRAME_RATE, 64, descriptor_counts->current_interface, descriptor_counts->current_interface + 1)
        #else
        TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(*current_interface_string, descriptor_counts->current_endpoint | 0x80, usb_video_frame_width, usb_video_frame_height, DEFAULT_FRAME_RATE, CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE, descriptor_counts->current_interface, descriptor_counts->current_interface + 1)
        #endif
    };
    (*current_interface_string)++;
    descriptor_counts->current_interface += 2;
    descriptor_counts->num_in_endpoints++;
    descriptor_counts->current_endpoint++;
    descriptor_counts->current_interface++;

    memcpy(descriptor_buf, usb_video_descriptor, sizeof(usb_video_descriptor));

    return sizeof(usb_video_descriptor);
}

background_callback_t usb_video_cb;

static void usb_video_cb_fun(void *unused) {
    (void)unused;

    static unsigned start_ms = 0;
    static unsigned already_sent = 0;

    if (!tud_video_n_streaming(0, 0)) {
        already_sent = 0;
        frame_num = 0;
        return;
    }

    if (!already_sent) {
        already_sent = 1;
        start_ms = supervisor_ticks_ms32();
        convert_framebuffer_maybe();
        bool result = tud_video_n_frame_xfer(0, 0, (void *)frame_buffer_yuyv, usb_video_frame_width * usb_video_frame_height * 16 / 8);
        (void)result;
    }

    unsigned cur = supervisor_ticks_ms32();
    if (cur - start_ms < interval_ms) {
        background_callback_add(&usb_video_cb, usb_video_cb_fun, NULL); // re-queue
        return;                             // not enough time
    }
    if (tx_busy) {
        background_callback_add(&usb_video_cb, usb_video_cb_fun, NULL); // re-queue
        return;
    }
    start_ms += interval_ms;

    convert_framebuffer_maybe();
    bool result = tud_video_n_frame_xfer(0, 0, (void *)frame_buffer_yuyv, usb_video_frame_width * usb_video_frame_height * 16 / 8);
    (void)result;
}


void usb_video_task(void) {
    if (usb_video_is_enabled) {
        background_callback_add(&usb_video_cb, usb_video_cb_fun, NULL);
    }
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) {
    (void)ctl_idx;
    (void)stm_idx;
    usb_video_task();
    tx_busy = 0;
    /* flip buffer */
    ++frame_num;
}

int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx,
    video_probe_and_commit_control_t const *parameters) {
    (void)ctl_idx;
    (void)stm_idx;
    /* convert unit to ms from 100 ns */
    interval_ms = parameters->dwFrameInterval / 10000;
    return VIDEO_ERROR_NONE;
}
