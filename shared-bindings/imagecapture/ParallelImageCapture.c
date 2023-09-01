/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared/runtime/context_manager_helpers.h"

#include "shared-bindings/imagecapture/ParallelImageCapture.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/imagecapture/ParallelImageCapture.h"

//| class ParallelImageCapture:
//|     """Capture image frames from a camera with parallel data interface"""
//|
//|     def __init__(
//|         self,
//|         *,
//|         data_pins: List[microcontroller.Pin],
//|         clock: microcontroller.Pin,
//|         vsync: Optional[microcontroller.Pin],
//|         href: Optional[microcontroller.Pin],
//|     ) -> None:
//|         """Create a parallel image capture object
//|
//|         This object is usually used with a camera-specific wrapper library such as `adafruit_ov5640 <https://circuitpython.readthedocs.io/projects/ov5640/en/latest/>`_.
//|
//|         :param List[microcontroller.Pin] data_pins: The data pins.
//|         :param microcontroller.Pin clock: The pixel clock input.
//|         :param microcontroller.Pin vsync: The vertical sync input, which has a negative-going pulse at the beginning of each frame.
//|         :param microcontroller.Pin href: The horizontal reference input, which is high whenever the camera is transmitting valid pixel information.
//|         """
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data_pins, ARG_clock, ARG_vsync, ARG_href,
           NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_clock, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_vsync, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_href, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t pins[32];
    uint8_t pin_count;
    validate_pins(MP_QSTR_data, pins, MP_ARRAY_SIZE(pins), args[ARG_data_pins].u_obj, &pin_count);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *vsync = validate_obj_is_free_pin_or_none(args[ARG_vsync].u_obj, MP_QSTR_vsync);
    const mcu_pin_obj_t *href = validate_obj_is_free_pin_or_none(args[ARG_href].u_obj, MP_QSTR_href);

    imagecapture_parallelimagecapture_obj_t *self =
        mp_obj_malloc(imagecapture_parallelimagecapture_obj_t, &imagecapture_parallelimagecapture_type);

    common_hal_imagecapture_parallelimagecapture_construct(self, pins, pin_count, clock, vsync, href);

    return self;
}

//|     def capture(self, buffer: WriteableBuffer) -> WriteableBuffer:
//|         """Capture a single frame into the given buffer.
//|
//|         This will stop a continuous-mode capture, if one is in progress."""
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_capture(mp_obj_t self_in, mp_obj_t buffer) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    common_hal_imagecapture_parallelimagecapture_singleshot_capture(self, buffer);

    return buffer;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(imagecapture_parallelimagecapture_capture_obj, imagecapture_parallelimagecapture_capture);

//|     def continuous_capture_start(
//|         self, buffer1: WriteableBuffer, buffer2: WriteableBuffer, /
//|     ) -> None:
//|         """Begin capturing into the given buffers in the background.
//|
//|         Call `continuous_capture_get_frame` to get the next available
//|         frame, and `continuous_capture_stop` to stop capturing.
//|
//|         Until `continuous_capture_stop` (or `deinit`) is called, the
//|         `ParallelImageCapture` object keeps references to ``buffer1`` and
//|         ``buffer2``, so the objects will not be garbage collected."""
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_continuous_capture_start(mp_obj_t self_in, mp_obj_t buffer1, mp_obj_t buffer2) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    common_hal_imagecapture_parallelimagecapture_continuous_capture_start(self, buffer1, buffer2);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(imagecapture_parallelimagecapture_continuous_capture_start_obj, imagecapture_parallelimagecapture_continuous_capture_start);

//|     def continuous_capture_get_frame(self) -> WriteableBuffer:
//|         """Return the next available frame, one of the two buffers passed to `continuous_capture_start`"""
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_continuous_capture_get_frame(mp_obj_t self_in) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    return common_hal_imagecapture_parallelimagecapture_continuous_capture_get_frame(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imagecapture_parallelimagecapture_continuous_capture_get_frame_obj, imagecapture_parallelimagecapture_continuous_capture_get_frame);



//|     def continuous_capture_stop(self) -> None:
//|         """Stop continuous capture.
//|
//|         Calling this method also causes the object to release its
//|         references to the buffers passed to `continuous_capture_start`,
//|         potentially allowing the objects to be garbage collected."""
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_continuous_capture_stop(mp_obj_t self_in) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    common_hal_imagecapture_parallelimagecapture_continuous_capture_stop(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imagecapture_parallelimagecapture_continuous_capture_stop_obj, imagecapture_parallelimagecapture_continuous_capture_stop);




//|     def deinit(self) -> None:
//|         """Deinitialize this instance"""
//|         ...
STATIC mp_obj_t imagecapture_parallelimagecapture_deinit(mp_obj_t self_in) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    common_hal_imagecapture_parallelimagecapture_deinit(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(imagecapture_parallelimagecapture_deinit_obj, imagecapture_parallelimagecapture_deinit);

//|     def __enter__(self) -> ParallelImageCapture:
//|         """No-op used in Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t imagecapture_parallelimagecapture___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_imagecapture_parallelimagecapture_deinit(args[0]);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(imagecapture_parallelimagecapture___exit___obj, 4, 4, imagecapture_parallelimagecapture___exit__);


STATIC const mp_rom_map_elem_t imagecapture_parallelimagecapture_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&imagecapture_parallelimagecapture_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&imagecapture_parallelimagecapture___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&imagecapture_parallelimagecapture_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_continuous_capture_start), MP_ROM_PTR(&imagecapture_parallelimagecapture_continuous_capture_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_continuous_capture_stop), MP_ROM_PTR(&imagecapture_parallelimagecapture_continuous_capture_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_continuous_capture_get_frame), MP_ROM_PTR(&imagecapture_parallelimagecapture_continuous_capture_get_frame_obj) },
};

STATIC MP_DEFINE_CONST_DICT(imagecapture_parallelimagecapture_locals_dict, imagecapture_parallelimagecapture_locals_dict_table);

const mp_obj_type_t imagecapture_parallelimagecapture_type = {
    { &mp_type_type },
    .name = MP_QSTR_ParallelImageCapture,
    .make_new = imagecapture_parallelimagecapture_make_new,
    .locals_dict = (mp_obj_dict_t *)&imagecapture_parallelimagecapture_locals_dict,
};
