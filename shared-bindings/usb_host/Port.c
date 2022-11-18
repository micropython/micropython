/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/usb_host/Port.h"
#include "shared-bindings/util.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| class Port:
//|     """USB host port. Also known as a root hub port."""
//|
//|     def __init__(self, dp: microcontroller.Pin, dm: microcontroller.Pin) -> None:
//|         """Create a USB host port on the given pins. Access attached devices
//|         through the `usb` module. Keep this object referenced while
//|         interacting with devices, otherwise they will be disconnected.
//|
//|         :param ~microcontroller.Pin dp: The data plus pin
//|         :param ~microcontroller.Pin dm: The data minus pin
//|         """
//|         ...
STATIC mp_obj_t usb_host_port_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    const mcu_pin_obj_t *dp = validate_obj_is_free_pin(args[0]);
    const mcu_pin_obj_t *dm = validate_obj_is_free_pin(args[1]);

    usb_host_port_obj_t *self = m_new_obj(usb_host_port_obj_t);
    self->base.type = &usb_host_port_type;
    common_hal_usb_host_port_construct(self, dp, dm);

    return (mp_obj_t)self;
}

//|     def deinit(self) -> None:
//|         """Turn off the USB host port and release the pins for other use."""
//|         ...
STATIC mp_obj_t usb_host_port_obj_deinit(mp_obj_t self_in) {
    usb_host_port_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_host_port_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_port_deinit_obj, usb_host_port_obj_deinit);

//|     def __enter__(self) -> Port:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t usb_host_port_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_usb_host_port_deinit(MP_OBJ_TO_PTR(args[0]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_host_port_obj___exit___obj, 4, 4, usb_host_port_obj___exit__);

STATIC const mp_rom_map_elem_t usb_host_port_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),             MP_ROM_PTR(&usb_host_port_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),          MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),           MP_ROM_PTR(&usb_host_port_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(usb_host_port_locals_dict, usb_host_port_locals_dict_table);

const mp_obj_type_t usb_host_port_type = {
    { &mp_type_type },
    .name = MP_QSTR_Port,
    .make_new = usb_host_port_make_new,
    .locals_dict = (mp_obj_t)&usb_host_port_locals_dict,
};
