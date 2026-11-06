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


#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_HW_USB_HOST

#include "shared/tinyusb/mp_usbh.h"
#include "shared/tinyusb/mp_usbh_uvc.h"

// The singleton USB host object, and the singleton UVC camera object, on top of the
// TinyUSB host stack (see shared/tinyusb/mp_usbh.c).

typedef struct _mp_obj_usb_host_t {
    mp_obj_base_t base;
} mp_obj_usb_host_t;

const mp_obj_type_t machine_usb_host_type;

static const mp_obj_usb_host_t usb_host_obj = {{&machine_usb_host_type}};

static mp_obj_t usb_host_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return MP_OBJ_FROM_PTR(&usb_host_obj);
}

static mp_obj_t usb_host_active(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            if (!mp_usbh_init()) {
                mp_raise_OSError(MP_EIO);
            }
        } else {
            mp_usbh_deinit();
        }
    }
    return mp_obj_new_bool(mp_usbh_is_active());
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_active_obj, 1, 2, usb_host_active);

// Return a list of (address, vid, pid) tuples for the mounted devices.
static mp_obj_t usb_host_devices(mp_obj_t self_in) {
    (void)self_in;
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (mp_usbh_is_active()) {
        mp_usbh_task();
        for (uint8_t daddr = 1; daddr <= CFG_TUH_DEVICE_MAX + CFG_TUH_HUB; ++daddr) {
            uint16_t vid, pid;
            if (tuh_mounted(daddr) && tuh_vid_pid_get(daddr, &vid, &pid)) {
                mp_obj_t items[3] = {
                    MP_OBJ_NEW_SMALL_INT(daddr),
                    MP_OBJ_NEW_SMALL_INT(vid),
                    MP_OBJ_NEW_SMALL_INT(pid),
                };
                mp_obj_list_append(list, mp_obj_new_tuple(3, items));
            }
        }
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_devices_obj, usb_host_devices);

#if MICROPY_HW_USB_HOST_UVC

/******************************************************************************/
// USBHost.UVC

#define FRAME_INTERVAL_PER_SEC (10000000)

static const mp_obj_type_t usb_host_uvc_type;

// The frame buffers belong to the driver, so the object has no state of its own.
static const mp_obj_base_t usb_host_uvc_obj = {&usb_host_uvc_type};

static mp_obj_t uvc_new_fps(uint32_t interval) {
    // Round to 2 decimal places, so eg 333333 gives 30.0 rather than 30.00003.
    mp_int_t fps_x100 = ((mp_int_t)FRAME_INTERVAL_PER_SEC * 100 + interval / 2) / interval;
    return mp_obj_new_float((mp_float_t)fps_x100 / 100);
}

static void uvc_check_error(int ret) {
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
}

static void uvc_check_streaming(void) {
    if (!mp_usbh_uvc_is_streaming()) {
        mp_raise_OSError(mp_usbh_uvc_daddr() == 0 ? MP_ENODEV : MP_EPERM);
    }
}

static mp_obj_t usb_host_uvc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 3000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_usbh_is_active()) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("USB host not active"));
    }

    // Wait for a camera to be attached and enumerated.
    mp_uint_t t0 = mp_hal_ticks_ms();
    for (;;) {
        mp_usbh_task();
        if (mp_usbh_uvc_daddr() != 0) {
            break;
        }
        if (mp_hal_ticks_ms() - t0 >= (mp_uint_t)args[ARG_timeout].u_int) {
            mp_raise_OSError(MP_ENODEV);
        }
        mp_event_wait_ms(5);
    }

    return MP_OBJ_FROM_PTR(&usb_host_uvc_obj);
}

// Return a list of (fourcc, width, height, (fps, ...)) tuples, one per supported frame size.
static mp_obj_t usb_host_uvc_formats(mp_obj_t self_in) {
    (void)self_in;
    if (mp_usbh_uvc_daddr() == 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    mp_obj_t list = mp_obj_new_list(0, NULL);
    mp_usbh_uvc_iter_t iter;
    mp_usbh_uvc_iter_init(&iter);
    const mp_usbh_uvc_frame_t *frame;
    while ((frame = mp_usbh_uvc_iter_next(&iter)) != NULL) {
        // Continuous frame intervals are given as (min, max, step), so just give the range.
        size_t n = frame->interval_type ? frame->interval_type : 2;
        mp_obj_tuple_t *fps = MP_OBJ_TO_PTR(mp_obj_new_tuple(n, NULL));
        for (size_t i = 0; i < n; ++i) {
            fps->items[i] = uvc_new_fps(tu_le32toh(tu_unaligned_read32(frame->intervals + 4 * i)));
        }
        mp_obj_t items[4] = {
            mp_obj_new_str(frame->fourcc, 4),
            MP_OBJ_NEW_SMALL_INT(frame->width),
            MP_OBJ_NEW_SMALL_INT(frame->height),
            MP_OBJ_FROM_PTR(fps),
        };
        mp_obj_list_append(list, mp_obj_new_tuple(4, items));
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_uvc_formats_obj, usb_host_uvc_formats);

// Pick the frame interval closest to the requested one.
static uint32_t uvc_select_interval(const mp_usbh_uvc_frame_t *frame, uint32_t target) {
    if (frame->interval_type == 0) {
        uint32_t min = tu_le32toh(tu_unaligned_read32(frame->intervals));
        uint32_t max = tu_le32toh(tu_unaligned_read32(frame->intervals + 4));
        uint32_t step = tu_le32toh(tu_unaligned_read32(frame->intervals + 8));
        target = MAX(min, MIN(max, target));
        if (step > 0) {
            target = min + (target - min + step / 2) / step * step;
        }
        return target;
    }

    uint32_t best = 0;
    uint32_t best_diff = UINT32_MAX;
    for (size_t i = 0; i < frame->interval_type; ++i) {
        uint32_t interval = tu_le32toh(tu_unaligned_read32(frame->intervals + 4 * i));
        uint32_t diff = interval > target ? interval - target : target - interval;
        if (diff < best_diff) {
            best = interval;
            best_diff = diff;
        }
    }
    return best;
}

static mp_obj_t usb_host_uvc_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_format, ARG_width, ARG_height, ARG_fps, ARG_bufsize };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_format, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_width, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_fps, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bufsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (mp_usbh_uvc_daddr() == 0) {
        mp_raise_OSError(MP_ENODEV);
    }

    // Restarting with new settings.
    mp_usbh_uvc_stream_stop();

    // Default to MJPEG if the camera supports it, otherwise its first format.
    char fourcc[4];
    if (args[ARG_format].u_obj != mp_const_none) {
        size_t len;
        const char *str = mp_obj_str_get_data(args[ARG_format].u_obj, &len);
        if (len != 4) {
            mp_raise_ValueError(MP_ERROR_TEXT("format"));
        }
        memcpy(fourcc, str, 4);
    } else {
        mp_usbh_uvc_iter_t iter;
        mp_usbh_uvc_iter_init(&iter);
        const mp_usbh_uvc_frame_t *frame = mp_usbh_uvc_iter_next(&iter);
        if (frame == NULL) {
            mp_raise_OSError(MP_EOPNOTSUPP);
        }
        memcpy(fourcc, frame->fourcc, 4);
        while ((frame = mp_usbh_uvc_iter_next(&iter)) != NULL) {
            if (memcmp(frame->fourcc, "MJPG", 4) == 0) {
                memcpy(fourcc, "MJPG", 4);
                break;
            }
        }
    }

    // Find the requested frame size, or the format's default if none was given.
    const mp_int_t width = args[ARG_width].u_int;
    const mp_int_t height = args[ARG_height].u_int;
    mp_usbh_uvc_frame_t selected;
    bool found = false;
    mp_usbh_uvc_iter_t iter;
    mp_usbh_uvc_iter_init(&iter);
    const mp_usbh_uvc_frame_t *frame;
    while ((frame = mp_usbh_uvc_iter_next(&iter)) != NULL) {
        if (memcmp(frame->fourcc, fourcc, 4) != 0) {
            continue;
        }
        if (width == 0 && height == 0) {
            if (!found || frame->is_default) {
                selected = *frame;
                found = true;
            }
        } else if ((width == 0 || frame->width == width) && (height == 0 || frame->height == height)) {
            selected = *frame;
            found = true;
            break;
        }
    }
    if (!found) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported format or frame size"));
    }

    uint32_t interval = selected.default_interval;
    if (args[ARG_fps].u_obj != mp_const_none) {
        mp_float_t fps = mp_obj_get_float(args[ARG_fps].u_obj);
        if (fps <= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("fps"));
        }
        interval = uvc_select_interval(&selected, (uint32_t)(FRAME_INTERVAL_PER_SEC / fps + (mp_float_t)0.5));
    }

    mp_usbh_uvc_commit_t commit;
    uvc_check_error(mp_usbh_uvc_negotiate(&selected, interval, &commit));

    // Uncompressed frames have a known size.  For compressed frames the camera's maximum
    // is usually the uncompressed size, so use a more realistic default to save memory.
    size_t bufsize = args[ARG_bufsize].u_int;
    if (bufsize == 0) {
        if (selected.bits_per_pixel) {
            bufsize = (size_t)selected.width * selected.height * selected.bits_per_pixel / 8;
        } else {
            bufsize = (size_t)selected.width * selected.height / 2;
            if (commit.max_frame_size != 0) {
                bufsize = MIN(bufsize, commit.max_frame_size);
            }
        }
    }

    uvc_check_error(mp_usbh_uvc_stream_start(&selected, &commit, bufsize));

    mp_obj_t items[4] = {
        mp_obj_new_str(fourcc, 4),
        MP_OBJ_NEW_SMALL_INT(selected.width),
        MP_OBJ_NEW_SMALL_INT(selected.height),
        uvc_new_fps(commit.interval ? commit.interval : interval),
    };
    return mp_obj_new_tuple(4, items);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(usb_host_uvc_start_obj, 1, usb_host_uvc_start);

static mp_obj_t usb_host_uvc_stop(mp_obj_t self_in) {
    (void)self_in;
    mp_usbh_uvc_stream_stop();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_uvc_stop_obj, usb_host_uvc_stop);

// Wait for a new frame, returning false on timeout.  A negative timeout waits forever.
static bool uvc_wait_frame(mp_int_t timeout, uint8_t **buf, size_t *len) {
    mp_uint_t t0 = mp_hal_ticks_ms();
    for (;;) {
        uvc_check_streaming();
        if (mp_usbh_uvc_frame_take(buf, len)) {
            return true;
        }
        if (timeout >= 0 && mp_hal_ticks_ms() - t0 >= (mp_uint_t)timeout) {
            return false;
        }
        mp_event_wait_ms(1);
    }
}

// Return a memoryview of the next frame, or None on timeout.  The data is only valid
// until the next call to read(), readinto() or stop().
static mp_obj_t usb_host_uvc_read(size_t n_args, const mp_obj_t *args) {
    mp_int_t timeout = n_args > 1 ? mp_obj_get_int(args[1]) : 1000;
    uint8_t *buf;
    size_t len;
    if (!uvc_wait_frame(timeout, &buf, &len)) {
        return mp_const_none;
    }
    return mp_obj_new_memoryview('B', len, buf);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_uvc_read_obj, 1, 2, usb_host_uvc_read);

// Copy the next frame into the given buffer, returning its length or None on timeout.
static mp_obj_t usb_host_uvc_readinto(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t timeout = n_args > 2 ? mp_obj_get_int(args[2]) : 1000;
    uint8_t *buf;
    size_t len;
    if (!uvc_wait_frame(timeout, &buf, &len)) {
        return mp_const_none;
    }
    if (len > bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }
    memcpy(bufinfo.buf, buf, len);
    return MP_OBJ_NEW_SMALL_INT(len);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_uvc_readinto_obj, 2, 3, usb_host_uvc_readinto);

static mp_obj_t usb_host_uvc_stats(mp_obj_t self_in) {
    (void)self_in;
    mp_usbh_uvc_stats_t stats;
    mp_usbh_uvc_get_stats(&stats);
    mp_obj_t dict = mp_obj_new_dict(6);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_frames), mp_obj_new_int_from_uint(stats.frames));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_dropped), mp_obj_new_int_from_uint(stats.dropped));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_errors), mp_obj_new_int_from_uint(stats.errors));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_overflows), mp_obj_new_int_from_uint(stats.overflows));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_alt), MP_OBJ_NEW_SMALL_INT(stats.alt_setting));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_xfer_size), MP_OBJ_NEW_SMALL_INT(stats.xfer_size));
    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_uvc_stats_obj, usb_host_uvc_stats);

static size_t uvc_find_control(mp_obj_t name_in) {
    const char *name = mp_obj_str_get_str(name_in);
    for (size_t i = 0; mp_usbh_uvc_control_name(i) != NULL; ++i) {
        if (strcmp(name, mp_usbh_uvc_control_name(i)) == 0) {
            return i;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("unknown control"));
}

// Return a dict of the controls the camera supports, as name: (min, max, step, default).
static mp_obj_t usb_host_uvc_controls(mp_obj_t self_in) {
    (void)self_in;
    if (mp_usbh_uvc_daddr() == 0) {
        mp_raise_OSError(MP_ENODEV);
    }
    static const uint8_t requests[4] = { MP_USBH_UVC_GET_MIN, MP_USBH_UVC_GET_MAX, MP_USBH_UVC_GET_RES, MP_USBH_UVC_GET_DEF };
    mp_obj_t dict = mp_obj_new_dict(0);
    for (size_t i = 0; mp_usbh_uvc_control_name(i) != NULL; ++i) {
        if (!mp_usbh_uvc_control_available(i)) {
            continue;
        }
        mp_obj_t items[4];
        for (size_t j = 0; j < 4; ++j) {
            int32_t value;
            int ret = mp_usbh_uvc_control_get(i, requests[j], &value);
            if (ret < 0) {
                // Some cameras don't give the range of on/off controls.
                static const int32_t fallback[4] = { 0, 1, 1, 0 };
                value = fallback[j];
            }
            items[j] = mp_obj_new_int(value);
        }
        const char *name = mp_usbh_uvc_control_name(i);
        mp_obj_dict_store(dict, mp_obj_new_str(name, strlen(name)), mp_obj_new_tuple(4, items));
    }
    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_uvc_controls_obj, usb_host_uvc_controls);

// control(name) gets the value of a control, and control(name, value) sets it.
static mp_obj_t usb_host_uvc_control(size_t n_args, const mp_obj_t *args) {
    size_t idx = uvc_find_control(args[1]);
    if (n_args == 2) {
        int32_t value;
        uvc_check_error(mp_usbh_uvc_control_get(idx, MP_USBH_UVC_GET_CUR, &value));
        return mp_obj_new_int(value);
    }
    uvc_check_error(mp_usbh_uvc_control_set(idx, mp_obj_get_int(args[2])));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_uvc_control_obj, 2, 3, usb_host_uvc_control);

static const mp_rom_map_elem_t usb_host_uvc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_formats), MP_ROM_PTR(&usb_host_uvc_formats_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&usb_host_uvc_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&usb_host_uvc_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&usb_host_uvc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&usb_host_uvc_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_stats), MP_ROM_PTR(&usb_host_uvc_stats_obj) },
    { MP_ROM_QSTR(MP_QSTR_controls), MP_ROM_PTR(&usb_host_uvc_controls_obj) },
    { MP_ROM_QSTR(MP_QSTR_control), MP_ROM_PTR(&usb_host_uvc_control_obj) },
};
static MP_DEFINE_CONST_DICT(usb_host_uvc_locals_dict, usb_host_uvc_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    usb_host_uvc_type,
    MP_QSTR_UVC,
    MP_TYPE_FLAG_NONE,
    make_new, usb_host_uvc_make_new,
    locals_dict, &usb_host_uvc_locals_dict
    );

#endif // MICROPY_HW_USB_HOST_UVC

/******************************************************************************/
// USBHost

static const mp_rom_map_elem_t usb_host_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usb_host_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_devices), MP_ROM_PTR(&usb_host_devices_obj) },
    #if MICROPY_HW_USB_HOST_UVC
    { MP_ROM_QSTR(MP_QSTR_UVC), MP_ROM_PTR(&usb_host_uvc_type) },
    #endif
};
static MP_DEFINE_CONST_DICT(usb_host_locals_dict, usb_host_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_usb_host_type,
    MP_QSTR_USBHost,
    MP_TYPE_FLAG_NONE,
    make_new, usb_host_make_new,
    locals_dict, &usb_host_locals_dict
    );

#endif // MICROPY_HW_USB_HOST
