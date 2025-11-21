/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// Camera support for Linux using V4L2 (Video4Linux2)
// Provides machine.Camera functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

// Tentative declaration
extern const mp_obj_type_t machine_camera_type;

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_FORMAT V4L2_PIX_FMT_RGB24

typedef struct _machine_camera_obj_t {
    mp_obj_base_t base;
    int fd;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint8_t *buffer;
    size_t buffer_size;
    bool is_open;
} machine_camera_obj_t;

static void machine_camera_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Camera(device=/dev/video0, width=%u, height=%u, open=%s)",
        self->width, self->height, self->is_open ? "True" : "False");
}

static mp_obj_t machine_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_device, ARG_width, ARG_height, ARG_format };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_width, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_WIDTH} },
        { MP_QSTR_height, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_HEIGHT} },
        { MP_QSTR_format, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },  // 0=RGB, 1=JPEG
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create camera object
    machine_camera_obj_t *self = mp_obj_malloc(machine_camera_obj_t, &machine_camera_type);
    self->width = parsed_args[ARG_width].u_int;
    self->height = parsed_args[ARG_height].u_int;
    self->format = (parsed_args[ARG_format].u_int == 1) ? V4L2_PIX_FMT_MJPEG : V4L2_PIX_FMT_RGB24;
    self->buffer = NULL;
    self->buffer_size = 0;
    self->is_open = false;

    // Open video device
    char device[32];
    snprintf(device, sizeof(device), "/dev/video%ld", (long)parsed_args[ARG_device].u_int);
    self->fd = open(device, O_RDWR | O_NONBLOCK);
    if (self->fd < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Cannot open %s: %s"), device, strerror(errno));
    }

    // Query capabilities
    struct v4l2_capability cap;
    if (ioctl(self->fd, VIDIOC_QUERYCAP, &cap) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        close(self->fd);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Device does not support video capture"));
    }

    // Set format
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = self->width;
    fmt.fmt.pix.height = self->height;
    fmt.fmt.pix.pixelformat = self->format;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(self->fd, VIDIOC_S_FMT, &fmt) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    // Update dimensions (driver may have changed them)
    self->width = fmt.fmt.pix.width;
    self->height = fmt.fmt.pix.height;
    self->buffer_size = fmt.fmt.pix.sizeimage;

    // Allocate buffer
    self->buffer = malloc(self->buffer_size);
    if (!self->buffer) {
        close(self->fd);
        mp_raise_OSError(ENOMEM);
    }

    self->is_open = true;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_camera_capture(mp_obj_t self_in) {
    machine_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->is_open) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Camera not open"));
    }

    // For simplicity, use read() method
    // In production, should use mmap/streaming for better performance
    ssize_t bytes_read = read(self->fd, self->buffer, self->buffer_size);
    if (bytes_read < 0) {
        if (errno == EAGAIN) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("No frame available (try again)"));
        }
        mp_raise_OSError(errno);
    }

    // Return as bytes object
    return mp_obj_new_bytes(self->buffer, bytes_read);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_camera_capture_obj, machine_camera_capture);

static mp_obj_t machine_camera_deinit(mp_obj_t self_in) {
    machine_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->is_open) {
        close(self->fd);
        self->is_open = false;
    }

    if (self->buffer) {
        free(self->buffer);
        self->buffer = NULL;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_camera_deinit_obj, machine_camera_deinit);

static const mp_rom_map_elem_t machine_camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&machine_camera_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_camera_deinit_obj) },

    // Format constants
    { MP_ROM_QSTR(MP_QSTR_RGB), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_JPEG), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(machine_camera_locals_dict, machine_camera_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_NONE,
    make_new, machine_camera_make_new,
    print, machine_camera_print,
    locals_dict, &machine_camera_locals_dict
);
