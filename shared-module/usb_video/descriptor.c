/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 by Jeff Epler for Adafruit Industries
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

// To find the offsets where the string and endpoint value are used,
// build this on your host computer:
// gcc descriptor.c  -I ../../lib/tinyusb/src -iquote . -DTUP_DCD_ENDPOINT_MAX=0 && ./a.out

#include "class/video/video_device.h"
#include "uvc_usb_descriptors.h"

#define ITF_NUM_VIDEO_CONTROL (8)
#define ITF_NUM_VIDEO_STREAMING (9)

static const uint8_t usb_cdc_descriptor_template1[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(0xaa, 0x85, 240, 240, 15, 64)
};
static const uint8_t usb_cdc_descriptor_template1_bulk[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR_BULK(0xaa, 0x85, 240, 240, 15, 64)
};

#undef ITF_NUM_VIDEO_CONTROL
#undef ITF_NUM_VIDEO_STREAMING
#define ITF_NUM_VIDEO_CONTROL (10)
#define ITF_NUM_VIDEO_STREAMING (11)

static const uint8_t usb_cdc_descriptor_template2[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR(0x55, 0x8a, 240, 240, 15, 64)
};

static const uint8_t usb_cdc_descriptor_template2_bulk[] = {
    TUD_VIDEO_CAPTURE_DESCRIPTOR_UNCOMPR_BULK(0x55, 0x8a, 240, 240, 15, 64)
};

int main() {
    for (int i = 0; i < sizeof(usb_cdc_descriptor_template1); i++) {
        if (usb_cdc_descriptor_template1[i] == 0xaa && usb_cdc_descriptor_template2[i] == 0x55) {
            printf("UVC_STRING_INDEX @ %d\n", i);
        } else
        if ((usb_cdc_descriptor_template1[i] & 0x80) == 0x05 && (usb_cdc_descriptor_template2[i] & 0x80) == 0x0a) {
            printf("UVC_ENDPOINT_INDEX @ %d\n", i);
        } else
        if (usb_cdc_descriptor_template1[i] != usb_cdc_descriptor_template2[i]) {
            printf("??? @ %d: %02x vs %02x\n", i, usb_cdc_descriptor_template1[i], usb_cdc_descriptor_template2[i]);
        }
    }
    for (int i = 0; i < sizeof(usb_cdc_descriptor_template1); i++) {
        if (i % 16 == 0) {
            if (i) {
                printf("\n");
            }
            printf("%04x ", i);
        } else if (i % 8 == 0) {
            printf("  ");
        } else {
            printf(" ");
        }
        printf("%02x", usb_cdc_descriptor_template1[i]);
    }
    printf("\n");
    printf("\n");

    printf("bulk\n");
    for (int i = 0; i < sizeof(usb_cdc_descriptor_template1_bulk); i++) {
        if (usb_cdc_descriptor_template1_bulk[i] == 0xaa && usb_cdc_descriptor_template2_bulk[i] == 0x55) {
            printf("UVC_STRING_INDEX @ %d\n", i);
        } else
        if ((usb_cdc_descriptor_template1_bulk[i] & 0x80) == 0x05 && (usb_cdc_descriptor_template2_bulk[i] & 0x80) == 0x0a) {
            printf("UVC_ENDPOINT_INDEX @ %d\n", i);
        } else
        if (usb_cdc_descriptor_template1_bulk[i] != usb_cdc_descriptor_template2_bulk[i]) {
            printf("??? @ %d: %02x vs %02x\n", i, usb_cdc_descriptor_template1_bulk[i], usb_cdc_descriptor_template2_bulk[i]);
        }
    }
    for (int i = 0; i < sizeof(usb_cdc_descriptor_template1_bulk); i++) {
        if (i % 16 == 0) {
            if (i) {
                printf("\n");
            }
            printf("%04x ", i);
        } else if (i % 8 == 0) {
            printf("  ");
        } else {
            printf(" ");
        }
        printf("%02x", usb_cdc_descriptor_template1_bulk[i]);
    }
    printf("\n");
}
