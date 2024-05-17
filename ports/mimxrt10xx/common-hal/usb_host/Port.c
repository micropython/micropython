// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/usb_host/Port.h"

#include "py/runtime.h"

#include "tusb.h"

#include "imx_usb.h"

usb_host_port_obj_t usb_host_instance;

usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    const mcu_pin_obj_t *supported_dp;
    const mcu_pin_obj_t *supported_dm;
    if (CIRCUITPY_USB_HOST_INSTANCE == 0) {
        supported_dp = &pin_USB_OTG1_DP;
        supported_dm = &pin_USB_OTG1_DN;
    } else if (CIRCUITPY_USB_HOST_INSTANCE == 1) {
        supported_dp = &pin_USB_OTG2_DP;
        supported_dm = &pin_USB_OTG2_DN;
    }
    // Return the singleton if given the same pins.
    usb_host_port_obj_t *self = &usb_host_instance;
    if (self->dp != NULL) {
        if (self->dp != dp || self->dm != dm) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
        }
        return self;
    }
    if (dp != supported_dp || dm != supported_dm) {
        raise_ValueError_invalid_pins();
    }

    assert_pin_free(dp);
    assert_pin_free(dm);

    init_usb_instance(CIRCUITPY_USB_HOST_INSTANCE);
    tuh_init(TUH_OPT_RHPORT);

    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;

    return self;
}
