// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 by Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
