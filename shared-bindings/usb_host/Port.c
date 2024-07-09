// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
//|         through the `usb` module.
//|
//|         The resulting object lives longer than the CircuitPython VM so that
//|         USB devices such as keyboards can continue to be used. Subsequent
//|         calls to this constructor will return the same object and *not*
//|         reinitialize the USB host port. It will raise an exception when
//|         given different arguments from the first successful call.
//|
//|         :param ~microcontroller.Pin dp: The data plus pin
//|         :param ~microcontroller.Pin dm: The data minus pin
//|         """
//|         ...
//|
static mp_obj_t usb_host_port_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check number of arguments
    mp_arg_check_num(n_args, n_kw, 2, 2, false);

    const mcu_pin_obj_t *dp = validate_obj_is_pin(args[0], MP_QSTR_dp);
    const mcu_pin_obj_t *dm = validate_obj_is_pin(args[1], MP_QSTR_dm);

    // Pin in use checks happen in the implementation so they can be ignored
    // when returning the singleton.

    usb_host_port_obj_t *self = common_hal_usb_host_port_construct(dp, dm);

    return (mp_obj_t)self;
}

static const mp_rom_map_elem_t usb_host_port_locals_dict_table[] = {
};

static MP_DEFINE_CONST_DICT(usb_host_port_locals_dict, usb_host_port_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    usb_host_port_type,
    MP_QSTR_Port,
    MP_TYPE_FLAG_NONE,
    make_new, usb_host_port_make_new,
    locals_dict, &usb_host_port_locals_dict
    );
