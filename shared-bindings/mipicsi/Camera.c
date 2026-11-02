// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>

#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/mipicsi/__init__.h"
#include "shared-bindings/mipicsi/Camera.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"

#if CIRCUITPY_VIDEOCORE
#include "shared-module/videocore/__init__.h"
#endif

//| class Camera:
//|     """Receive frames from a camera sensor on a MIPI CSI-2 interface
//|
//|     The camera sensor is configured separately (typically over I2C) to
//|     transmit frames. Use the dedicated MIPI CSI-2 receiver pins.
//|
//|     The sensor object must implement ``circuitpython_typing.CameraSensor``,
//|     which has these properties:
//|
//|     * ``width`` and ``height``: the image size in pixels
//|     * ``pixel_format``: the `PixelFormat` the sensor transmits
//|     * ``lane_bit_rate_mbps``: the bit rate of each data lane
//|     * ``data_lanes``: the number of data lanes used, 1 or 2
//|     * ``bayer_order``: the `BayerOrder` of RAW data
//|
//|     and these methods:
//|
//|     * ``_start()``: start transmitting frames
//|     * ``_stop()``: stop transmitting frames
//|
//|     ``_start()`` and ``_stop()`` are internal. The camera calls them when
//|     streaming starts and stops; user code should not call them.
//|
//|     The sensor's properties are validated when the Camera is created and
//|     are validated again when `videocore.VideoBuffer.play()` is called. The
//|     sensor may change its configuration while it is not playing.
//|
//|     The sensor sets the image size and format, and the receiver, such as
//|     `videocore.VideoBuffer`, is set up to match when playing starts. The
//|     sensor's mode should not change between ``_start()`` and ``_stop()``
//|     calls. (The sensor class should raise an exception when it is
//|     attempted.) To use a new mode, stop playing, change the sensor, and
//|     play again.
//|
//|     Camera objects implement the internal videosource protocol, so a
//|     `videocore.VideoBuffer` can stream their frames into a framebuffer.
//|     """
//|
//|     def __init__(
//|         self,
//|         sensor: circuitpython_typing.CameraSensor,
//|         *,
//|         output_format: Optional[PixelFormat] = None,
//|     ) -> None:
//|         """
//|         Configure and initialize the MIPI CSI-2 camera receiver
//|
//|         .. code-block:: python
//|
//|             sensor = sc2336.SC2336(i2c)
//|             camera = mipicsi.Camera(sensor)
//|
//|         :param sensor: A camera sensor to receive frames from. The receiver
//|             is configured to match the sensor's current mode. The sensor
//|             starts streaming when a `videocore.VideoBuffer` plays the
//|             camera.
//|         :param output_format: The format of the captured frames. Defaults
//|             to the sensor's ``pixel_format``. When different, frames are
//|             converted as they are captured, for example from RAW10 to
//|             `PixelFormat.RGB565`. Not all conversions are supported.
//|         """
//|
// Read and validate the sensor's parameters. mp_load_attr raises
// AttributeError for missing ones.
static void mipicsi_camera_load_sensor_params(
    mp_obj_t sensor,
    mipicsi_sensor_params_t *params) {
    params->pixel_format = validate_pixel_format(mp_load_attr(sensor, MP_QSTR_pixel_format), MP_QSTR_pixel_format);
    params->width = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_width)), 1, MP_QSTR_width);
    params->height = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_height)), 1, MP_QSTR_height);
    params->lane_bit_rate_mbps = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_lane_bit_rate_mbps)), 1, MP_QSTR_lane_bit_rate_mbps);
    params->data_lanes = mp_arg_validate_int_range(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_data_lanes)), 1, 2, MP_QSTR_data_lanes);
    params->bayer_order = validate_bayer_order(mp_load_attr(sensor, MP_QSTR_bayer_order), MP_QSTR_bayer_order);
}

static mp_obj_t mipicsi_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_obj_t sensor = all_args[0];

    enum { ARG_output_format, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_output_format, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(0, n_kw, all_args + 1, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate the sensor now so mistakes show up early. play() reads the
    // parameters again since the sensor may change between streams.
    mipicsi_sensor_params_t params;
    mipicsi_camera_load_sensor_params(sensor, &params);
    mipicsi_pixel_format_t output_format = params.pixel_format;
    if (args[ARG_output_format].u_obj != mp_const_none) {
        output_format = validate_pixel_format(args[ARG_output_format].u_obj, MP_QSTR_output_format);
    }

    // Validate the sensor contract methods (_start/_stop) exist now too,
    // since the camera drives them from C as the stream starts and stops.
    mp_obj_t validated[2];
    mp_load_method(sensor, MP_QSTR__start, validated);
    mp_load_method(sensor, MP_QSTR__stop, validated);

    mipicsi_camera_obj_t *self = mp_obj_malloc_with_finaliser(mipicsi_camera_obj_t, &mipicsi_camera_type);
    common_hal_mipicsi_camera_construct(self, output_format);
    // The sensor isn't started until a VideoBuffer plays the camera.
    common_hal_mipicsi_camera_set_sensor(self, sensor);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the camera receiver and releases all memory resources for reuse."""
//|         ...
//|
static mp_obj_t mipicsi_camera_deinit(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_mipicsi_camera_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mipicsi_camera_deinit_obj, mipicsi_camera_deinit);

//|     def __enter__(self) -> Camera:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//|

static const mp_rom_map_elem_t mipicsi_camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mipicsi_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mipicsi_camera_deinit_obj) },
};

static MP_DEFINE_CONST_DICT(mipicsi_camera_locals_dict, mipicsi_camera_locals_dict_table);

#if CIRCUITPY_VIDEOCORE
// Implementation of the videocore videosource protocol so a VideoBuffer can
// tell the Camera where to put frames.

static void check_for_deinit(mipicsi_camera_obj_t *self) {
    if (common_hal_mipicsi_camera_deinited(self)) {
        raise_deinited_error();
    }
}

static void mipicsi_camera_play(mp_obj_t self_in, bool paused) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    check_for_deinit(self);
    // Reread the sensor's mode; it may change between streams. The sensor
    // starts even when paused; only the receiver waits.
    mipicsi_sensor_params_t params;
    mipicsi_camera_load_sensor_params(common_hal_mipicsi_camera_get_sensor(self), &params);
    common_hal_mipicsi_camera_videosource_play(self, &params, paused);
}

static void mipicsi_camera_stop(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    common_hal_mipicsi_camera_videosource_stop(self);
}

static void mipicsi_camera_pause(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    common_hal_mipicsi_camera_videosource_pause(self);
}

static void mipicsi_camera_resume(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    common_hal_mipicsi_camera_videosource_resume(self);
}

static bool mipicsi_camera_set_destination(mp_obj_t self_in, const videocore_destination_t *dest,
    videocore_on_frame_fun on_frame, void *ctx) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    return common_hal_mipicsi_camera_set_destination(self, dest->base, dest->stride, on_frame, ctx);
}

static mp_int_t mipicsi_camera_get_width(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_width(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static mp_int_t mipicsi_camera_get_height(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_height(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static mp_int_t mipicsi_camera_get_bits_per_pixel(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_bits_per_pixel(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static const videosource_p_t mipicsi_camera_videosource_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_videosource)
    .play = mipicsi_camera_play,
    .pause = mipicsi_camera_pause,
    .resume = mipicsi_camera_resume,
    .stop = mipicsi_camera_stop,
    .set_destination = mipicsi_camera_set_destination,
    .get_width = mipicsi_camera_get_width,
    .get_height = mipicsi_camera_get_height,
    .get_bits_per_pixel = mipicsi_camera_get_bits_per_pixel,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mipicsi_camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mipicsi_camera_make_new,
    locals_dict, &mipicsi_camera_locals_dict,
    protocol, &mipicsi_camera_videosource_protocol
    );
#else
MP_DEFINE_CONST_OBJ_TYPE(
    mipicsi_camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mipicsi_camera_make_new,
    locals_dict, &mipicsi_camera_locals_dict
    );
#endif
