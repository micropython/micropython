/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
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

#include <string.h>
#include <fcntl.h>

#include <arch/board/board.h>
#include <nuttx/video/video.h>
#include <nuttx/arch.h>

#include "py/runtime.h"

#include "shared-bindings/camera/Camera.h"

#define JPG_COMPRESS_RATIO (9)
#define SPRESENSE_CAMIMAGE_MEM_ALIGN (32)

typedef struct {
    const char* devpath;
    int fd;
} camera_dev_t;

STATIC camera_dev_t camera_dev = {"/dev/video", -1};

static void camera_size_to_width_and_height(camera_imagesize_t size, uint16_t *width, uint16_t *height) {
    switch (size) {
        case IMAGESIZE_320x240:
            *height = VIDEO_VSIZE_QVGA;
            *width = VIDEO_HSIZE_QVGA;
            break;
        case IMAGESIZE_640x320:
            *height = VIDEO_VSIZE_VGA;
            *width = VIDEO_HSIZE_VGA;
            break;
        case IMAGESIZE_1280x720:
            *height = VIDEO_VSIZE_HD;
            *width = VIDEO_HSIZE_HD;
            break;
        case IMAGESIZE_1280x960:
            *height = VIDEO_VSIZE_QUADVGA;
            *width = VIDEO_HSIZE_QUADVGA;
            break;
        case IMAGESIZE_1920x1080:
            *height = VIDEO_VSIZE_FULLHD;
            *width = VIDEO_HSIZE_FULLHD;
            break;
        case IMAGESIZE_2048x1536:
            *height = VIDEO_VSIZE_3M;
            *width = VIDEO_HSIZE_3M;
            break;
        case IMAGESIZE_2560x1920:
            *height = VIDEO_VSIZE_5M;
            *width = VIDEO_HSIZE_5M;
            break;
        default:
            mp_raise_ValueError(translate("Size not supported"));
            break;
    }
}

static void camera_set_format(enum v4l2_buf_type type, uint32_t pixformat, uint16_t width, uint16_t height) {
    v4l2_requestbuffers_t req = {0};

    // Set Buffer Mode.
    req.type = type;
    req.memory = V4L2_MEMORY_USERPTR;
    req.count = 1;
    req.mode = V4L2_BUF_MODE_RING;
    ioctl(camera_dev.fd, VIDIOC_REQBUFS, (unsigned long)&req);
    v4l2_format_t fmt = {0};

    // Set Format.
    fmt.type = type;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    fmt.fmt.pix.pixelformat = pixformat;
    ioctl(camera_dev.fd, VIDIOC_S_FMT, (unsigned long)&fmt);
}

static void camera_start_streaming(enum v4l2_buf_type type) {
    ioctl(camera_dev.fd, VIDIOC_STREAMON, (unsigned long)&type);
}

static void camera_start_preview() {
    camera_set_format(V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_PIX_FMT_UYVY, VIDEO_HSIZE_QVGA, VIDEO_VSIZE_QVGA);

    v4l2_buffer_t buf;

    memset(&buf, 0, sizeof(v4l2_buffer_t));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    ioctl(camera_dev.fd, VIDIOC_QBUF, (unsigned long)&buf);

    camera_start_streaming(V4L2_BUF_TYPE_VIDEO_CAPTURE);
}

void common_hal_camera_construct(camera_obj_t *self, camera_imagesize_t size) {
    if (camera_dev.fd < 0) {
        if (video_initialize(camera_dev.devpath) < 0) {
            mp_raise_ValueError(translate("Could not initialize Camera"));
        }
        camera_dev.fd = open(camera_dev.devpath, 0);
        if (camera_dev.fd < 0) {
            mp_raise_ValueError(translate("Could not initialize Camera"));
        }
    }

    uint16_t width, height;

    camera_size_to_width_and_height(size, &width, &height);

    self->size = size;

    // In SPRESENSE SDK, JPEG compression quality=80 by default.
    // In such setting, the maximum actual measured size of JPEG image
    // is about width * height * 2 / 9.
    self->buffer_size = (size_t)(width * height * 2 / JPG_COMPRESS_RATIO);;
    self->buffer = m_malloc(self->buffer_size, true);
    if (self->buffer == NULL) {
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate picture buffer"));
    }
    self->picture_buffer = self->buffer;
    while ((uint32_t)self->picture_buffer % SPRESENSE_CAMIMAGE_MEM_ALIGN != 0) {
        self->picture_buffer++;
    }

    camera_start_preview();

    camera_set_format(V4L2_BUF_TYPE_STILL_CAPTURE, V4L2_PIX_FMT_JPEG, width, height);

    camera_start_streaming(V4L2_BUF_TYPE_STILL_CAPTURE);

    sleep(1);
}

void common_hal_camera_deinit(camera_obj_t *self) {
    if (common_hal_camera_deinited(self)) {
        return;
    }

    video_uninitialize();

    m_free(self->buffer);

    close(camera_dev.fd);
    camera_dev.fd = -1;
}

bool common_hal_camera_deinited(camera_obj_t *self) {
    return camera_dev.fd < 0;
}

void common_hal_camera_take_picture(camera_obj_t *self) {
    v4l2_buffer_t buf;

    memset(&buf, 0, sizeof(v4l2_buffer_t));
    buf.type = V4L2_BUF_TYPE_STILL_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    buf.m.userptr = (unsigned long)self->picture_buffer;
    buf.length = self->buffer_size - SPRESENSE_CAMIMAGE_MEM_ALIGN;
    ioctl(camera_dev.fd, VIDIOC_QBUF, (unsigned long)&buf);

    ioctl(camera_dev.fd, VIDIOC_TAKEPICT_START, 0);

    ioctl(camera_dev.fd, VIDIOC_DQBUF, (unsigned long)&buf);

    ioctl(camera_dev.fd, VIDIOC_TAKEPICT_STOP, false);

    self->picture_size = (size_t)buf.bytesused;
}

uint8_t *common_hal_camera_get_picture_buffer(camera_obj_t *self) {
    return self->picture_buffer;
}

size_t common_hal_camera_get_picture_size(camera_obj_t *self) {
    return self->picture_size;
}

camera_imagesize_t common_hal_camera_get_size(camera_obj_t *self) {
    return self->size;
}

void common_hal_camera_set_size(camera_obj_t *self, camera_imagesize_t size) {
    uint16_t width, height;

    camera_size_to_width_and_height(size, &width, &height);

    self->buffer_size = (size_t)(width * height * 2 / JPG_COMPRESS_RATIO);;
    self->buffer = m_realloc(self->buffer, self->buffer_size);
    if (self->buffer == NULL) {
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate picture buffer"));
    }
    self->picture_buffer = self->buffer;
    while ((uint32_t)self->picture_buffer % SPRESENSE_CAMIMAGE_MEM_ALIGN != 0) {
        self->picture_buffer++;
    }

    camera_set_format(V4L2_BUF_TYPE_STILL_CAPTURE, V4L2_PIX_FMT_JPEG, width, height);
}
