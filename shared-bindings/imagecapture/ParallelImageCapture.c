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
#include "py/runtime.h"

#include "lib/utils/context_manager_helpers.h"

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
//|         :param List[microcontroller.Pin] data_pins: The data pins.
//|         :param microcontroller.Pin clock: The pixel clock input.
//|         :param microcontroller.Pin vsync: The vertical sync input, which has a negative-going pulse at the beginning of each frame.
//|         :param microcontroller.Pin href: The horizontal reference input, which is high whenever the camera is transmitting valid pixel information.
//|         """
//|         ...
//|
STATIC mp_obj_t imagecapture_parallelimagecapture_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t pins[32];
    uint8_t pin_count;
    validate_pins(MP_QSTR_data, pins, MP_ARRAY_SIZE(pins), args[ARG_data_pins].u_obj, &pin_count);

    mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj);
    mcu_pin_obj_t *vsync = validate_obj_is_free_pin_or_none(args[ARG_vsync].u_obj);
    mcu_pin_obj_t *href = validate_obj_is_free_pin_or_none(args[ARG_href].u_obj);

    imagecapture_parallelimagecapture_obj_t *self = m_new_obj(imagecapture_parallelimagecapture_obj_t);
    self->base.type = &imagecapture_parallelimagecapture_type;

    common_hal_imagecapture_parallelimagecapture_construct(self, pins, pin_count, clock, vsync, href);

    return self;
}

//|     def capture(self, buffer: WriteableBuffer, width: int, height: int, bpp: int=16) -> None:
//|         """Capture a single frame into the given buffer"""
//|         ...
//|
STATIC mp_obj_t imagecapture_parallelimagecapture_capture(mp_obj_t self_in, mp_obj_t buffer) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_RW);
    common_hal_imagecapture_parallelimagecapture_capture(self, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(imagecapture_parallelimagecapture_capture_obj, imagecapture_parallelimagecapture_capture);

//|     def deinit(self) -> None:
//|         """Deinitialize this instance"""
//|         ...
//|
STATIC mp_obj_t imagecapture_parallelimagecapture_deinit(mp_obj_t self_in) {
    imagecapture_parallelimagecapture_obj_t *self = (imagecapture_parallelimagecapture_obj_t *)self_in;
    common_hal_imagecapture_parallelimagecapture_deinit(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(imagecapture_parallelimagecapture_deinit_obj, imagecapture_parallelimagecapture_deinit);

//|     def __enter__(self) -> ParallelImageCapture:
//|         """No-op used in Context Managers."""
//|         ...
//|
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
};

STATIC MP_DEFINE_CONST_DICT(imagecapture_parallelimagecapture_locals_dict, imagecapture_parallelimagecapture_locals_dict_table);

const mp_obj_type_t imagecapture_parallelimagecapture_type = {
    { &mp_type_type },
    .name = MP_QSTR_ParallelImageCapture,
    .make_new = imagecapture_parallelimagecapture_make_new,
    .locals_dict = (mp_obj_dict_t *)&imagecapture_parallelimagecapture_locals_dict,
};
