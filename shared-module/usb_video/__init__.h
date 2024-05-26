// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "shared-module/storage/__init__.h"
#include "shared-module/displayio/Bitmap.h"

bool usb_video_enabled(void);
size_t usb_video_descriptor_length(void);
size_t usb_video_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);
void usb_video_task(void);

extern uint16_t usb_video_frame_width, usb_video_frame_height;
extern uint16_t *usb_video_framebuffer_rgb565;
