#include "py/runtime.h"
#include <stdint.h>
#include "shared-module/uvc/__init__.h"
#include "shared-module/uvc/uvc_usb_descriptors.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-module/bitmapfilter/macros.h"
#include "class/video/video_device.h"
#include "supervisor/shared/tick.h"
#include "supervisor/background_callback.h"

#if CFG_TUD_VIDEO_STREAMING_BULK
static const uint8_t usb_uvc_descriptor_template[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR_BULK(0xaa, 0x80, FRAME_WIDTH, FRAME_HEIGHT, 10, 64)
};

// Use the "descriptor.c" file to generate/verify these offsets
const uint8_t interface_indices[] = {2, 10, 29, 59};
const uint8_t string_indices[] = {7, 16, 65};
const uint8_t endpoint_indices[] = {72, 153};

#else
static const uint8_t usb_uvc_descriptor_template[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(0xaa, 0x80, FRAME_WIDTH, FRAME_HEIGHT, 10, CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE)
};

// Use the "descriptor.c" file to generate/verify these offsets
const uint8_t interface_indices[] = {2, 10, 29, 59, 153};
const uint8_t string_indices[] = {7, 16, 65, 159};
const uint8_t endpoint_indices[] = {72, 162};
#endif

static unsigned frame_num = 0;
static unsigned tx_busy = 0;
static unsigned interval_ms = 1000 / FRAME_RATE;

// TODO must dynamically allocate this, otherwise everyone pays for it
static uint8_t frame_buffer_yuyv[FRAME_WIDTH * FRAME_HEIGHT * 16 / 8];
static uint16_t frame_buffer_rgb565[FRAME_WIDTH * FRAME_HEIGHT];

displayio_bitmap_t uvc_bitmap_obj = {
    .base = {.type = &displayio_bitmap_type },
    .width = FRAME_WIDTH,
    .height = FRAME_HEIGHT,
    .data = (uint32_t *)frame_buffer_rgb565,
    .stride = FRAME_WIDTH,
    .bits_per_value = 16,
    .x_shift = 1,
    .x_mask = 1,
    .bitmask = 0xffff,
};

bool usb_uvc_enabled(void) {
    return true; // TODO
}

size_t usb_uvc_descriptor_length(void) {
    return sizeof(usb_uvc_descriptor_template);
}

static void convert_framebuffer(void) {
    uint8_t *dest = frame_buffer_yuyv;
    uint16_t *src = frame_buffer_rgb565;

    for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT / 2; i++) {
        uint16_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(src, 0);
        uint16_t p2 = IMAGE_GET_RGB565_PIXEL_FAST(src, 1);
        src += 2;

        int y1 = COLOR_RGB565_TO_Y(p1);
        int u = COLOR_RGB565_TO_U(p1) + 128;  // openmv UV are signed in the range [-127,128]
        int v = COLOR_RGB565_TO_V(p1) + 128;
        int y2 = COLOR_RGB565_TO_Y(p2);

        *dest++ = y1;
        *dest++ = u;
        *dest++ = y2;
        *dest++ = v;
    }
}

size_t usb_uvc_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    usb_add_interface_string(*current_interface_string, "CircuitPython UVC");

    memcpy(descriptor_buf, usb_uvc_descriptor_template, sizeof(usb_uvc_descriptor_template));

    for (size_t i = 0; i < TU_ARRAY_SIZE(string_indices); i++) {
        descriptor_buf[string_indices[i]] = *current_interface_string;
    }

    (*current_interface_string)++;

    for (size_t i = 0; i < TU_ARRAY_SIZE(interface_indices); i++) {
        descriptor_buf[interface_indices[i]] += descriptor_counts->current_interface;
    }
    descriptor_counts->current_interface += 2;

    for (size_t i = 0; i < TU_ARRAY_SIZE(endpoint_indices); i++) {
        // the only endpoint is an IN endpoint so set the 0x80 bit accordingly
        descriptor_buf[endpoint_indices[i]] = descriptor_counts->current_endpoint | 0x80;
    }
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;
    descriptor_counts->current_interface++;

    supervisor_enable_tick();

    return sizeof(usb_uvc_descriptor_template);
}

background_callback_t uvc_cb;

#define printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)

STATIC void uvc_cb_fun(void *unused) {
    (void)unused;

    static unsigned start_ms = 0;
    static unsigned already_sent = 0;

    {
        static int i;
        if (i++ % 100 == 0) {
            // printf("usb_uvc_task i=%d, tud_video_n_streaming=%d already_sent=%d start_ms=%d\n", i, tud_video_n_streaming(0, 0), already_sent, start_ms);
        }
    }

    if (!tud_video_n_streaming(0, 0)) {
        already_sent = 0;
        frame_num = 0;
        // printf("not already streaming\n");
        return;
    }

    if (!already_sent) {
        already_sent = 1;
        start_ms = supervisor_ticks_ms32();
        convert_framebuffer();
        bool result = tud_video_n_frame_xfer(0, 0, (void *)frame_buffer_yuyv, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
        (void)result;
        // printf("(!already_sent) frame_xfer -> %d\n", (int)result);
    }

    unsigned cur = supervisor_ticks_ms32();
    if (cur - start_ms < interval_ms) {
        background_callback_add(&uvc_cb, uvc_cb_fun, NULL); // re-queue
        return;                             // not enough time
    }
    if (tx_busy) {
        background_callback_add(&uvc_cb, uvc_cb_fun, NULL); // re-queue
        return;
    }
    start_ms += interval_ms;

    convert_framebuffer();
    bool result = tud_video_n_frame_xfer(0, 0, (void *)frame_buffer_yuyv, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
    (void)result;
    // printf("frame_xfer -> %d\n", (int)result);
}

void usb_uvc_task(void) {
    background_callback_add(&uvc_cb, uvc_cb_fun, NULL);
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) {
    (void)ctl_idx;
    (void)stm_idx;
    usb_uvc_task();
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
