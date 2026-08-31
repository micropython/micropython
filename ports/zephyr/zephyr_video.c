/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Panoramix Labs
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/binary.h"
#include "zephyr_device.h"

#if defined(CONFIG_VIDEO)

// Helper to convert a (mp_obj_t)string to an (uint32_t)pixelformat
#define VIDEO_FOURCC_FROM_MP_OBJ(obj) \
    ({ \
        const char *_fourcc_str = mp_obj_str_get_str(obj); \
        strlen(_fourcc_str) < 3 ? 0x00000000 : VIDEO_FOURCC_FROM_STR(_fourcc_str); \
    })

// TODO: move (enum video_ctrl_type) to <zephyr/drivers/video-controls.h>
#define VIDEO_CTRL_TYPE_INTEGER64 3

typedef struct _mp_obj_zephyr_video_t {
    mp_obj_base_t base;
    mp_obj_t buf_obj;
    const struct device *dev;
    struct video_buffer *vbuf;
} mp_obj_zephyr_video_t;

typedef struct _mp_obj_zvbuf_t {
    mp_obj_base_t base;
} mp_obj_zvbuf_t;

static mp_obj_t zephyr_video_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_zephyr_video_t *self = mp_obj_malloc(mp_obj_zephyr_video_t, type);
    self->dev = zephyr_device_find(args[0]);
    self->buf_obj = MP_OBJ_FROM_PTR(self);
    return MP_OBJ_FROM_PTR(self);
}

static void zephyr_video_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_zephyr_video_t *self = self_in;
    const char *name = zephyr_device_get_name(self->dev);

    struct video_format fmt = {};
    (void)video_get_format(self->dev, &fmt);

    struct video_frmival frmival = {};
    (void)video_get_frmival(self->dev, &frmival);

    mp_printf(print, "Video(%s, pixelformat=%s, width=%u, height=%u, fps=%u)",
        name, VIDEO_FOURCC_TO_STR(fmt.pixelformat), fmt.width, fmt.height,
        (uint32_t)(NSEC_PER_SEC / video_frmival_nsec(&frmival)));
}

static mp_int_t zephyr_video_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->vbuf == NULL) {
        return 1;
    }

    bufinfo->len = self->vbuf->bytesused;
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    bufinfo->buf = self->vbuf->buffer;

    return 0;
}

static void zcheck(int ret) {
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
}

static mp_obj_t zephyr_video_format_caps(mp_obj_t self_in) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(self_in);

    struct video_caps caps = { .type = VIDEO_BUF_TYPE_OUTPUT };
    zcheck(video_get_caps(self->dev, &caps));

    size_t num_format_caps = 0;
    while (caps.format_caps[num_format_caps].pixelformat != 0) {
        num_format_caps++;
    }

    mp_obj_tuple_t *t = mp_obj_new_tuple(num_format_caps, NULL);
    for (size_t i = 0; i < num_format_caps; i++) {
        mp_obj_tuple_t *tt = t->items[i] = mp_obj_new_tuple(7, NULL);
        tt->items[0] = mp_obj_new_str_from_cstr(VIDEO_FOURCC_TO_STR(caps.format_caps[i].pixelformat));
        tt->items[1] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].width_min);
        tt->items[2] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].width_max);
        tt->items[3] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].width_step);
        tt->items[4] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].height_min);
        tt->items[5] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].height_max);
        tt->items[6] = MP_OBJ_NEW_SMALL_INT(caps.format_caps[i].height_step);
    }

    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_video_format_caps_obj, zephyr_video_format_caps);

static mp_obj_t zephyr_video_format(size_t n_args, const mp_obj_t *args) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Getter

        struct video_format fmt = {};
        zcheck(video_get_format(self->dev, &fmt));

        mp_obj_tuple_t *t = mp_obj_new_tuple(3, NULL);
        t->items[0] = mp_obj_new_str_from_cstr(VIDEO_FOURCC_TO_STR(fmt.pixelformat));
        t->items[1] = mp_obj_new_int_from_uint(fmt.width);
        t->items[2] = mp_obj_new_int_from_uint(fmt.height);

        return t;
    } else if (n_args == 4) {
        // Setter

        struct video_format fmt = {
            .pixelformat = VIDEO_FOURCC_FROM_MP_OBJ(args[1]),
            .width = mp_obj_get_int(args[2]),
            .height = mp_obj_get_int(args[3]),
        };
        zcheck(video_set_format(self->dev, &fmt));

        return mp_obj_new_int_from_uint(fmt.size);
    } else {
        mp_raise_ValueError("Expecting v.format() or v.format('YUYV', 640, 480)");
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_video_format_obj, 1, 4, zephyr_video_format);

static mp_obj_t zephyr_video_frmival(size_t n_args, const mp_obj_t *args) {
    enum { ARG_id, ARG_frmival };

    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(args[ARG_id]);

    if (n_args == 1) {
        // Getter

        struct video_frmival frmival = {};
        zcheck(video_get_frmival(self->dev, &frmival));

        return mp_obj_new_int_from_uint(video_frmival_nsec(&frmival));
    } else {
        // Setter

        struct video_format fmt = {};
        zcheck(video_get_format(self->dev, &fmt));

        struct video_frmival_enum fie = {
            .format = &fmt,
            .type = VIDEO_FRMIVAL_TYPE_DISCRETE,
            .discrete.numerator = mp_obj_get_int(args[ARG_frmival]),
            .discrete.denominator = NSEC_PER_SEC,
        };
        video_closest_frmival(self->dev, &fie);

        zcheck(video_set_frmival(self->dev, &fie.discrete));

        return mp_obj_new_int_from_uint(video_frmival_nsec(&fie.discrete));
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_video_frmival_obj, 1, 2, zephyr_video_frmival);

// TODO: map the different control types (string, bool, etc.) to native python types
// TODO: accept control strings
static mp_obj_t zephyr_video_control(size_t n_args, const mp_obj_t *args) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(args[0]);

    struct video_ctrl_query cq = { .id = mp_obj_get_int(args[1]), .dev = self->dev };
    zcheck(video_query_ctrl(&cq));
    bool is_val64 = (cq.type == VIDEO_CTRL_TYPE_INTEGER64);

    if (n_args == 2) {
        // Getter

        struct video_control ctrl = { .id = cq.id };
        zcheck(video_get_ctrl(self->dev, &ctrl));

        return mp_obj_new_int(is_val64 ? ctrl.val64 : ctrl.val);
    } else {
        // Setter

        struct video_control ctrl = { .id = cq.id };
        if (is_val64) {
            ctrl.val64 = mp_obj_get_int(args[2]);
        } else {
            ctrl.val = mp_obj_get_int(args[2]);
        }
        zcheck(video_set_ctrl(self->dev, &ctrl));

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_video_control_obj, 2, 3, zephyr_video_control);

static mp_obj_t zephyr_video_start(mp_obj_t self_in) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(self_in);
    zcheck(video_stream_start(self->dev, VIDEO_BUF_TYPE_OUTPUT));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_video_start_obj, zephyr_video_start);

static mp_obj_t zephyr_video_stop(mp_obj_t self_in) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(self_in);
    zcheck(video_stream_stop(self->dev, VIDEO_BUF_TYPE_OUTPUT));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_video_stop_obj, zephyr_video_stop);

static mp_obj_t zephyr_video_capture(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Parse arguments
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(args[ARG_id].u_obj);
    mp_int_t timeout = args[ARG_timeout].u_int;

    // Get the fmt.size field (populated by `set`) to know how much to allocate
    struct video_format fmt = {.type = VIDEO_BUF_TYPE_OUTPUT};
    zcheck(video_get_format(self->dev, &fmt));
    zcheck(video_set_format(self->dev, &fmt));

    // Allocate a new buffer of correct size
    if (self->vbuf != NULL) {
        zcheck(video_buffer_release(self->vbuf));
    }
    self->vbuf = video_buffer_alloc(fmt.size, K_NO_WAIT);
    if (self->vbuf == NULL) {
        mp_raise_OSError(ENOMEM);
    }
    self->vbuf->type = VIDEO_BUF_TYPE_OUTPUT;

    // Enqueue/dequeue
    struct video_buffer *vbuf = &(struct video_buffer) { .type = VIDEO_BUF_TYPE_OUTPUT };
    zcheck(video_enqueue(self->dev, self->vbuf));
    zcheck(video_dequeue(self->dev, &vbuf, timeout > 0 ? K_USEC(timeout) : K_FOREVER));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_video_capture_obj, 1, zephyr_video_capture);

static mp_obj_t zephyr_video_submit(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_buffer, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Parse arguments
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(args[ARG_id].u_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    mp_int_t timeout = args[ARG_timeout].u_int;

    // Craft a video buffer and submit the content
    struct video_buffer vbuf = {
        .type = VIDEO_BUF_TYPE_INPUT,
        .buffer = bufinfo.buf,
        .size = bufinfo.len,
        .bytesused = bufinfo.len,
    };
    zcheck(video_enqueue(self->dev, &vbuf));

    // Wait for completion
    struct video_buffer *vbufp = &(struct video_buffer) { .type = VIDEO_BUF_TYPE_OUTPUT };
    zcheck(video_dequeue(self->dev, &vbufp, timeout > 0 ? K_USEC(timeout) : K_FOREVER));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_video_submit_obj, 2, zephyr_video_submit);

static mp_obj_t zephyr_video___del__(mp_obj_t self_in) {
    mp_obj_zephyr_video_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->vbuf != NULL) {
        zcheck(video_buffer_release(self->vbuf));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_video___del___obj, zephyr_video___del__);

static const mp_rom_map_elem_t zephyr_video_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&zephyr_video___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_format_caps), MP_ROM_PTR(&zephyr_video_format_caps_obj) },
    { MP_ROM_QSTR(MP_QSTR_format), MP_ROM_PTR(&zephyr_video_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_frmival), MP_ROM_PTR(&zephyr_video_frmival_obj) },
    { MP_ROM_QSTR(MP_QSTR_control), MP_ROM_PTR(&zephyr_video_control_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&zephyr_video_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&zephyr_video_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&zephyr_video_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_submit), MP_ROM_PTR(&zephyr_video_submit_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_video_locals_dict, zephyr_video_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_video_type,
    MP_QSTR_video,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_video_make_new,
    print, zephyr_video_print,
    locals_dict, &zephyr_video_locals_dict,
    buffer, zephyr_video_get_buffer
    );

#endif // CONFIG_VIDEO
