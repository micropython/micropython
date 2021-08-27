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

typedef struct {
    const char *devpath;
    int fd;
} camera_dev_t;

STATIC camera_dev_t camera_dev = {"/dev/video", -1};

typedef struct {
    uint16_t width;
    uint16_t height;
} image_size_t;

STATIC const image_size_t image_size_table[] = {
    { VIDEO_HSIZE_QVGA, VIDEO_VSIZE_QVGA },
    { VIDEO_HSIZE_VGA, VIDEO_VSIZE_VGA },
    { VIDEO_HSIZE_HD, VIDEO_VSIZE_HD },
    { VIDEO_HSIZE_QUADVGA, VIDEO_VSIZE_QUADVGA },
    { VIDEO_HSIZE_FULLHD, VIDEO_VSIZE_FULLHD },
    { VIDEO_HSIZE_3M, VIDEO_VSIZE_3M },
    { VIDEO_HSIZE_5M, VIDEO_VSIZE_5M },
};

static bool camera_check_width_and_height(uint16_t width, uint16_t height) {
    for (int i = 0; i < MP_ARRAY_SIZE(image_size_table); i++) {
        if (image_size_table[i].width == width && image_size_table[i].height == height) {
            return true;
        }
    }
    return false;
}

static bool camera_check_buffer_length(uint16_t width, uint16_t height, camera_imageformat_t format, size_t length) {
    if (format == IMAGEFORMAT_JPG) {
        // In SPRESENSE SDK, JPEG compression quality=80 by default.
        // In such setting, the maximum actual measured size of JPEG image
        // is about width * height * 2 / 9.
        return length >= (size_t)(width * height * 2 / 9);
    } else {
        return false;
    }
}

static bool camera_check_format(camera_imageformat_t format) {
    return format == IMAGEFORMAT_JPG;
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

void common_hal_camera_construct(camera_obj_t *self) {
    if (camera_dev.fd < 0) {
        if (video_initialize(camera_dev.devpath) < 0) {
            mp_raise_ValueError(translate("Could not initialize Camera"));
        }
        camera_dev.fd = open(camera_dev.devpath, 0);
        if (camera_dev.fd < 0) {
            mp_raise_ValueError(translate("Could not initialize Camera"));
        }
    }

    camera_start_preview();

    camera_start_streaming(V4L2_BUF_TYPE_STILL_CAPTURE);

    sleep(1);
}

void common_hal_camera_deinit(camera_obj_t *self) {
    if (common_hal_camera_deinited(self)) {
        return;
    }

    video_uninitialize();

    close(camera_dev.fd);
    camera_dev.fd = -1;
}

bool common_hal_camera_deinited(camera_obj_t *self) {
    return camera_dev.fd < 0;
}

size_t common_hal_camera_take_picture(camera_obj_t *self, uint8_t *buffer, size_t len, uint16_t width, uint16_t height, camera_imageformat_t format) {
    if (!camera_check_width_and_height(width, height)) {
        mp_raise_ValueError(translate("Size not supported"));
    }
    if (!camera_check_buffer_length(width, height, format, len)) {
        mp_raise_ValueError(translate("Buffer is too small"));
    }
    if (!camera_check_format(format)) {
        mp_raise_ValueError(translate("Format not supported"));
    }

    camera_set_format(V4L2_BUF_TYPE_STILL_CAPTURE, V4L2_PIX_FMT_JPEG, width, height);

    v4l2_buffer_t buf;

    memset(&buf, 0, sizeof(v4l2_buffer_t));
    buf.type = V4L2_BUF_TYPE_STILL_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;
    buf.m.userptr = (unsigned long)buffer;
    buf.length = len;
    ioctl(camera_dev.fd, VIDIOC_QBUF, (unsigned long)&buf);

    ioctl(camera_dev.fd, VIDIOC_TAKEPICT_START, 0);

    ioctl(camera_dev.fd, VIDIOC_DQBUF, (unsigned long)&buf);

    ioctl(camera_dev.fd, VIDIOC_TAKEPICT_STOP, false);

    return (size_t)buf.bytesused;
}
